#include "kernel.h"

#define GDTADDR 0x00000800 // fixed physical addr required by subject
// 0x00001000 would have been safer with grub usage 

struct gdtr
{
	uint_16t limit;
	uint_32t base;
} __attribute__ ((packed)); // this tells gcc to shorten it as possible

struct gdt_entry
{
	uint_16t limit_low;
	uint_16t base_low;
	uint_8t base_mid;
	uint_8t access;
	uint_8t gran;
	uint_8t base_high;
} __attribute__ ((packed));


// these two shenanigans forces gdt table and descriptor to be set in the linker dedicated space + 8-bytes alignment
__attribute__((section(".gdt"), aligned(8), used))
static volatile struct gdt_entry gdt[7];

__attribute__((aligned(8), used))
static volatile struct gdtr gdt_ptr;

static void gdt_set_entry(int idx, uint_32t base, uint_32t limit, uint_8t access, uint_8t gran)
{
	if (limit > 0xFFFFF)
		return ((void)putstr("GDT cannot encode limits larger than 0xFFFFF, lame"));
	// split base/limit across the gdt fields
	gdt[idx].limit_low = (uint_16t)(limit & 0xFFFF);
	gdt[idx].base_low = (uint_16t)(base & 0xFFFF);
	gdt[idx].base_mid = (uint_8t)((base >> 16) & 0xFF);
	gdt[idx].access = access;
	// gran = high 4 bits flags, low 4 bits limit[16..19]
	gdt[idx].gran = (uint_8t)(((limit >> 16) & 0x0F) | (gran & 0xF0));
	gdt[idx].base_high = (uint_8t)((base >> 24) & 0xFF);
}

static void gdt_load(const struct gdtr *ptr)
{
	asm volatile (
		"lgdt (%0)\n\t"
		// reload data segments to the new selector
		"mov $0x10, %%ax\n\t" // 0x10 = selector index 2 (kernel data), RPL=0
		"mov %%ax, %%ds\n\t"
		"mov %%ax, %%es\n\t"
		"mov %%ax, %%fs\n\t"
		"mov %%ax, %%gs\n\t"
		"mov %%ax, %%ss\n\t"
		// far jump to reload cs
		"ljmp $0x08, $1f\n\t" // 0x08 = selector index 1 (kernel code), RPL=0
		"1:\n\t"
		:
		: "r"(ptr)
		: "ax", "memory"
	);
}

void gdt_init(void)
{
	// null descriptor, the start
	gdt_set_entry(0, 0, 0, 0, 0);
	// kernel code: base=0, limit=4gb, access=0x9A (exec|read|present|ring0), gran=0xCF (4k|32b|limit hi)
	gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);
	// kernel data: base=0, limit=4gb, access=0x92 (read/write|present|ring0), gran=0xCF
	gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);
	// kernel stack: same as data, separate descriptor per subject
	gdt_set_entry(3, 0, 0xFFFFF, 0x92, 0xCF);
	// user code: base=0, limit=4gb, access=0xFA (exec|read|present|ring3), gran=0xCF
	gdt_set_entry(4, 0, 0xFFFFF, 0xFA, 0xCF);
	// user data: base=0, limit=4gb, access=0xF2 (read/write|present|ring3), gran=0xCF
	gdt_set_entry(5, 0, 0xFFFFF, 0xF2, 0xCF);
	// user stack: same as data, separate descriptor per subject
	gdt_set_entry(6, 0, 0xFFFFF, 0xF2, 0xCF);

	// gdtr points to our gdt table
	gdt_ptr.limit = (uint_16t)(sizeof(gdt) - 1);
	gdt_ptr.base = (uint_32t)GDTADDR;

}

void gdt_install(void)
{
	gdt_load((const struct gdtr *)(&gdt_ptr));
}