#include "kernel.h"

uint_32t page_dir[1024] __attribute__((aligned(4096)));
uint_32t first_page[1024] __attribute__((aligned(4096)));

void init_memory_paging()
{
	for (uint_16t i = 0; i < 1024; i++)
	{
		page_dir[i] = 0x00000002; // set supervisor only, not present
		first_page[i] = (i * 0x1000) | 0x3; // present + writable + supervisor
	}
	page_dir[0] = ((uint_32t)first_page) | 0x3; // point to first_page table with present + writable
}