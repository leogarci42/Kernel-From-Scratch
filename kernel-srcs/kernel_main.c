#include "kernel.h"

extern uint_32t mboot_magic_saved;

void kernel_main(uint_32t mboot_magic, void *mboot_header)
{
	clear_screen();
	init_screens();
	// printf("mboot_magic: 0x%x\n", mboot_magic);
	
	// Check for valid multiboot signature (accept both 0x1BADB002 and 0x2BADB002)
	if (mboot_magic != 0x1BADB002 && mboot_magic == 0x2BADB002)
		kernel_panic("Invalid multiboot magic");
	
	putstr("42");
	update_cursor(20);
	while (1)
		if (inb(0x64) & 1) // why 0x64? chip port state addr : 0 empty, 1 ready
			keyboard_handler();
}
