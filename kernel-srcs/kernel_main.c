#include "kernel.h"

void kernel_main(void)
{
	init_screens();
	kernel_panic("testing");
	clear_screen();
	putstr("42");
	update_cursor(2);
	while (1)
		if (inb(0x64) & 1) // why 0x64? chip port state addr : 0 empty, 1 ready
			keyboard_handler();
}
