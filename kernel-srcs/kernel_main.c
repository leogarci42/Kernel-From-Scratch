#include "kernel.h"

extern uint_32t mboot_magic_saved;

void kernel_main(uint_32t mboot_magic, void *mboot_header)
{
	clear_screen();
	init_screens();
	(void)mboot_header;
	// printf("mboot_magic: 0x%x\n", mboot_magic);
	if (mboot_magic != 0x2BADB002)
		kernel_panic("Invalid multiboot magic");
	putstr("42");
	update_cursor(2);
	while (1)
	{
		uint_8t status = inb(0x64);
		if (status & 1) // output buffer full
		{
			if (!(status & 0x20)) // bit 5 set means AUX (mouse) data
				keyboard_handler();
			else
				(void)inb(0x60);
		}
	}
}
