#include "kernel.h"

extern uint_32t mboot_magic_saved;

void kernel_main(uint_32t mboot_magic, void *mboot_header)
{
	clear_screen();
	init_screens();
	(void)mboot_header;
	// printf("mboot_magic: 0x%x\n", mboot_magic);
	if (mboot_magic != 0x1BADB002 && mboot_magic != 0x2BADB002) // QEMU takes 0x1 and leaves us with magic 0x2
		kernel_panic("Invalid multiboot magic");
	putstr("42");
	update_cursor(2);
	while (1)
		if (inb(0x64) & 1) // why 0x64? chip port state addr : 0 empty, 1 ready
			keyboard_handler();
}
