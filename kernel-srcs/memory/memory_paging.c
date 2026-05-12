#include "kernel.h"

#define TOTAL_PAGES 1024
#define PAGE_SIZE 4096
#define KERNEL_HEAP_START 0x10000000 // 256MB

uint_32t page_dir[1024] __attribute__((aligned(4096)));
uint_32t first_page[1024] __attribute__((aligned(4096)));

// pmm (physical memory management)
uint_8t page_bitmap[TOTAL_PAGES / 8]; // bitmap for physical page (1 bit per page)
uint_32t next_free_page = 0x102000 / PAGE_SIZE; // first free physical page after kernel

// vmm (virtual memory management aka heap)
typedef struct {
	uint_32t vaddr;
	uint_32t size;
	uint_8t allocated;
} vm_allocation_t;

#define MAX_ALLOCATIONS 256
vm_allocation_t vm_allocations[MAX_ALLOCATIONS];
uint_32t next_vaddr = KERNEL_HEAP_START;

void init_memory_paging()
{
	for (uint_16t i = 0; i < 1024; i++)
	{
		page_dir[i] = 0x00000002; // set supervisor only, not present
		first_page[i] = (i * 0x1000) | 0x3; // present + writable + supervisor
	}
	page_dir[0] = ((uint_32t)first_page) | 0x3; // point to first_page table with present + writable
	// init bitmap (all pages free)
	for (uint_16t i = 0; i < TOTAL_PAGES / 8; i++)
		page_bitmap[i] = 0;
	// mark kernel page as used (first 0x102000 bytes)
	for (uint_16t i = 0; i < next_free_page; i++)
		page_bitmap[i / 8] |= (1 << (i % 8));
	// init virtual alloc
	for (uint_16t i = 0; i < MAX_ALLOCATIONS; i++)
		vm_allocations[i].allocated = 0;
}

void *alloc_page(void)
{
	for (uint_16t i = 0; i < TOTAL_PAGES; i++)
	{
		if (!(page_bitmap[i / 8] & (1 << (i % 8))))
		{
			page_bitmap[i / 8] |= (1 << (i % 8));
			return (void *)(i * PAGE_SIZE);
		}
	}
	return 0;
}

void free_page(void *page)
{
	uint_32t page_num = ((uint_32t)page) / PAGE_SIZE;
	if (page_num < TOTAL_PAGES)
		page_bitmap[page_num / 8] &= ~(1 << (page_num % 8));
}

uint_32t get_page_size(void)
{
	return PAGE_SIZE;
}

void map_page(uint_32t vaddr, uint_32t paddr)
{
	uint_32t page_index = (vaddr >> 12) & 0x3FF;  // get 10 bits for page table index
	first_page[page_index] = (paddr & 0xFFFFF000) | 0x3;  // map with present + writable
}

void unmap_page(uint_32t vaddr)
{
	uint_32t page_index = (vaddr >> 12) & 0x3FF;
	first_page[page_index] = 0x00000002;  // clear present bit
}

void *alloc_virtual(uint_32t size)
{
	uint_32t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	uint_32t vaddr = next_vaddr;

	uint_16t slot = 0;
	for (uint_16t i = 0; i < MAX_ALLOCATIONS; i++)
	{
		if (!vm_allocations[i].allocated)
		{
			slot = i;
			break;
		}
	}
	for (uint_32t i = 0; i < pages_needed; i++)
	{
		void *phys = alloc_page();
		if (!phys)
			return 0;
		uint_32t vpage = vaddr + (i * PAGE_SIZE);
		map_page(vpage, (uint_32t)phys);  // map virtual page to physical page
	}
	vm_allocations[slot].vaddr = vaddr;
	vm_allocations[slot].size = size;
	vm_allocations[slot].allocated = 1;
	next_vaddr += (pages_needed * PAGE_SIZE);
	return (void *)vaddr;
}

void free_virtual(void *addr)
{
	uint_32t vaddr = (uint_32t)addr;
	
	for (uint_16t i = 0; i < MAX_ALLOCATIONS; i++)
	{
		if (vm_allocations[i].allocated && vm_allocations[i].vaddr == vaddr)
		{
			uint_32t pages = (vm_allocations[i].size + PAGE_SIZE - 1) / PAGE_SIZE;
		for (uint_32t j = 0; j < pages; j++)
		{
			uint_32t page_addr = vaddr + (j * PAGE_SIZE);
			unmap_page(page_addr);
			free_page((void *)page_addr);
		}
			vm_allocations[i].allocated = 0;
			return;
		}
	}
}

uint_32t get_allocation_size(void *addr)
{
	uint_32t vaddr = (uint_32t)addr;
	
	for (uint_16t i = 0; i < MAX_ALLOCATIONS; i++)
	{
		if (vm_allocations[i].allocated && vm_allocations[i].vaddr == vaddr)
			return vm_allocations[i].size;
	}
	return 0;
}