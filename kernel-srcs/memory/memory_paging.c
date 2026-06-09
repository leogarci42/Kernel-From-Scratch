#include "kernel.h"

#define PAGE_SIZE 4096
#define KERNEL_VMA 0xC0000000
#define KERNEL_HEAP_START 0xD0000000

#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4

#define MAX_TRACKED_PAGES 1048576
#define TOTAL_BITMAP_BYTES (MAX_TRACKED_PAGES / 8)

uint_32t page_dir[1024] __attribute__((aligned(4096)));
static uint_32t low_page_table[1024] __attribute__((aligned(4096)));
static uint_32t high_page_table[1024] __attribute__((aligned(4096)));

static uint_8t page_bitmap[TOTAL_BITMAP_BYTES];
static uint_32t total_pages = 0;

typedef struct {
	uint_32t vaddr;
	uint_32t size;
	uint_8t allocated;
} vm_allocation_t;

#define MAX_ALLOCATIONS 256
static vm_allocation_t vm_allocations[MAX_ALLOCATIONS];
static uint_32t next_vaddr = KERNEL_HEAP_START;

typedef struct {
	uint_32t flags;
	uint_32t mem_lower;
	uint_32t mem_upper;
	uint_32t boot_device;
	uint_32t cmdline;
	uint_32t mods_count;
	uint_32t mods_addr;
	uint_32t syms[4];
	uint_32t mmap_length;
	uint_32t mmap_addr;
} multiboot_info_t;

typedef struct {
	uint_32t size;
	uint_32t addr_low;
	uint_32t addr_high;
	uint_32t len_low;
	uint_32t len_high;
	uint_32t type;
} __attribute__((packed)) multiboot_mmap_entry_t;

extern uint_8t __kernel_phys_start;
extern uint_8t __kernel_phys_end;
extern uint_8t __boot_phys_start;
extern uint_8t __boot_phys_end;

static inline uint_32t page_align_down(uint_32t addr)
{
	return addr & 0xFFFFF000;
}

static inline uint_32t page_align_up(uint_32t addr)
{
	return (addr + PAGE_SIZE - 1) & 0xFFFFF000;
}

static inline void bitmap_set(uint_32t page)
{
	if (page < MAX_TRACKED_PAGES)
		page_bitmap[page / 8] |= (1U << (page % 8));
}

static inline void bitmap_clear(uint_32t page)
{
	if (page < MAX_TRACKED_PAGES)
		page_bitmap[page / 8] &= ~(1U << (page % 8));
}

static inline uint_8t bitmap_test(uint_32t page)
{
	if (page >= MAX_TRACKED_PAGES)
		return 1;
	return (page_bitmap[page / 8] >> (page % 8)) & 1U;
}

static void mark_range_used(uint_32t start, uint_32t end)
{
	uint_32t pstart = page_align_down(start) / PAGE_SIZE;
	uint_32t pend = page_align_up(end) / PAGE_SIZE;

	for (uint_32t p = pstart; p < pend && p < total_pages; p++)
		bitmap_set(p);
}

static void mark_range_free(uint_32t start, uint_32t end)
{
	uint_32t pstart = page_align_up(start) / PAGE_SIZE;
	uint_32t pend = page_align_down(end) / PAGE_SIZE;

	for (uint_32t p = pstart; p < pend && p < total_pages; p++)
		bitmap_clear(p);
}

static void reserve_critical_ranges(void)
{
	mark_range_used(0x00000000, 0x00100000);
	mark_range_used((uint_32t)&__boot_phys_start, (uint_32t)&__boot_phys_end);
	mark_range_used((uint_32t)&__kernel_phys_start, (uint_32t)&__kernel_phys_end);
	mark_range_used(0x000B8000, 0x000B9000);
}

static void pmm_init_from_multiboot(void *mboot_header)
{
	multiboot_info_t *mbi = (multiboot_info_t *)mboot_header;
	uint_32t memory_top = 64 * 1024 * 1024;

	for (uint_32t i = 0; i < TOTAL_BITMAP_BYTES; i++)
		page_bitmap[i] = 0xFF;

	if (mbi && (mbi->flags & (1U << 0)))
		memory_top = (mbi->mem_upper + 1024) * 1024;

	total_pages = memory_top / PAGE_SIZE;
	if (total_pages > MAX_TRACKED_PAGES)
		total_pages = MAX_TRACKED_PAGES;

	if (mbi && (mbi->flags & (1U << 6)))
	{
		uint_32t cursor = mbi->mmap_addr;
		uint_32t end = mbi->mmap_addr + mbi->mmap_length;

		while (cursor < end)
		{
			multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t *)cursor;
			if (entry->type == 1 && entry->addr_high == 0 && entry->len_high == 0)
				mark_range_free(entry->addr_low, entry->addr_low + entry->len_low);
			cursor += entry->size + sizeof(entry->size);
		}
	}
	else
	{
		mark_range_free(0x00100000, memory_top);
	}

	reserve_critical_ranges();
}

static void *alloc_page_below(uint_32t max_phys_addr)
{
	uint_32t max_page = max_phys_addr / PAGE_SIZE;
	if (max_page > total_pages)
		max_page = total_pages;

	for (uint_32t i = 1; i < max_page; i++)
	{
		if (!bitmap_test(i))
		{
			bitmap_set(i);
			return (void *)(i * PAGE_SIZE);
		}
	}

	return 0;
}

static uint_32t get_physical_address(uint_32t vaddr)
{
	uint_32t pde = page_dir[(vaddr >> 22) & 0x3FF];
	if (!(pde & PAGE_PRESENT))
		return 0;

	uint_32t *pt = (uint_32t *)(pde & 0xFFFFF000);
	uint_32t pte = pt[(vaddr >> 12) & 0x3FF];
	if (!(pte & PAGE_PRESENT))
		return 0;

	return (pte & 0xFFFFF000) | (vaddr & 0xFFF);
}

static void invlpg(uint_32t addr)
{
	asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
}

void init_memory_paging(void *mboot_header)
{
	for (uint_32t i = 0; i < 1024; i++)
	{
		page_dir[i] = PAGE_RW;
		low_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;
		high_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;
	}

	page_dir[0] = (((uint_32t)low_page_table - KERNEL_VMA) & 0xFFFFF000) | PAGE_PRESENT | PAGE_RW;
	page_dir[KERNEL_VMA >> 22] = (((uint_32t)high_page_table - KERNEL_VMA) & 0xFFFFF000) | PAGE_PRESENT | PAGE_RW;

	pmm_init_from_multiboot(mboot_header);

	for (uint_32t i = 0; i < MAX_ALLOCATIONS; i++)
		vm_allocations[i].allocated = 0;

	next_vaddr = page_align_up(KERNEL_HEAP_START);
}

void *alloc_page(void)
{
	for (uint_32t i = 1; i < total_pages; i++)
	{
		if (!bitmap_test(i))
		{
			bitmap_set(i);
			return (void *)(i * PAGE_SIZE);
		}
	}
	return 0;
}

void free_page(void *page)
{
	uint_32t page_num = ((uint_32t)page) / PAGE_SIZE;
	if (page_num < total_pages)
		bitmap_clear(page_num);
}

uint_32t get_page_size(void)
{
	return PAGE_SIZE;
}

void map_page(uint_32t vaddr, uint_32t paddr)
{
	uint_32t pde_index = (vaddr >> 22) & 0x3FF;
	uint_32t pte_index = (vaddr >> 12) & 0x3FF;
	uint_32t pde = page_dir[pde_index];
	uint_32t *pt;

	if (!(pde & PAGE_PRESENT))
	{
		void *new_pt_phys = alloc_page_below(0x00400000);
		if (!new_pt_phys)
			return;

		pt = (uint_32t *)new_pt_phys;
		for (uint_32t i = 0; i < 1024; i++)
			pt[i] = PAGE_RW;

		page_dir[pde_index] = ((uint_32t)new_pt_phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_RW;
	}
	else
	{
		pt = (uint_32t *)(pde & 0xFFFFF000);
	}

	pt[pte_index] = (paddr & 0xFFFFF000) | PAGE_PRESENT | PAGE_RW;
	invlpg(vaddr);
}

void unmap_page(uint_32t vaddr)
{
	uint_32t pde_index = (vaddr >> 22) & 0x3FF;
	uint_32t pte_index = (vaddr >> 12) & 0x3FF;
	uint_32t pde = page_dir[pde_index];

	if (!(pde & PAGE_PRESENT))
		return;

	uint_32t *pt = (uint_32t *)(pde & 0xFFFFF000);
	pt[pte_index] = PAGE_RW;
	invlpg(vaddr);
}

void *alloc_virtual(uint_32t size)
{
	if (size == 0)
		return 0;

	uint_32t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	uint_32t vaddr = page_align_up(next_vaddr);

	int slot = -1;
	for (uint_32t i = 0; i < MAX_ALLOCATIONS; i++)
	{
		if (!vm_allocations[i].allocated)
		{
			slot = (int)i;
			break;
		}
	}
	if (slot < 0)
		return 0;

	for (uint_32t i = 0; i < pages_needed; i++)
	{
		void *phys = alloc_page();
		if (!phys)
		{
			for (uint_32t j = 0; j < i; j++)
			{
				uint_32t page_addr = vaddr + (j * PAGE_SIZE);
				uint_32t mapped_phys = get_physical_address(page_addr) & 0xFFFFF000;
				unmap_page(page_addr);
				if (mapped_phys)
					free_page((void *)mapped_phys);
			}
			return 0;
		}
		uint_32t vpage = vaddr + (i * PAGE_SIZE);
		map_page(vpage, (uint_32t)phys);
	}
	vm_allocations[(uint_32t)slot].vaddr = vaddr;
	vm_allocations[(uint_32t)slot].size = size;
	vm_allocations[(uint_32t)slot].allocated = 1;
	next_vaddr += (pages_needed * PAGE_SIZE);
	return (void *)vaddr;
}

void free_virtual(void *addr)
{
	uint_32t vaddr = (uint_32t)addr;
	
	for (uint_32t i = 0; i < MAX_ALLOCATIONS; i++)
	{
		if (vm_allocations[i].allocated && vm_allocations[i].vaddr == vaddr)
		{
			uint_32t pages = (vm_allocations[i].size + PAGE_SIZE - 1) / PAGE_SIZE;
			for (uint_32t j = 0; j < pages; j++)
			{
				uint_32t page_addr = vaddr + (j * PAGE_SIZE);
				uint_32t phys = get_physical_address(page_addr) & 0xFFFFF000;
				unmap_page(page_addr);
				if (phys)
					free_page((void *)phys);
			}
			vm_allocations[i].allocated = 0;
			return;
		}
	}
}

uint_32t get_allocation_size(void *addr)
{
	uint_32t vaddr = (uint_32t)addr;
	
	for (uint_32t i = 0; i < MAX_ALLOCATIONS; i++)
	{
		if (vm_allocations[i].allocated && vm_allocations[i].vaddr == vaddr)
			return vm_allocations[i].size;
	}
	return 0;
}