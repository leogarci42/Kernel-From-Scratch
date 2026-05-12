#include "kernel.h"

uint_32t page_dir[1024] __attribute__(aligned(4096));
uint_32t first_page[1024] __attribute__(aligned(4096));

void init_memory_paging()
{
	for (uint_16t i; i < 1024; i++)
	{
		page_dir[i] = 0x00000002; // set user mod to kernel only
		first_page[i] = (i * 0x1000) | 3; // set user mod to supervisor
	}
}