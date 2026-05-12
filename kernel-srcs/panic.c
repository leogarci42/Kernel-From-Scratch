#include "kernel.h"
#include "helpers.h"

void kernel_panic(const char *msg)
{
	clear_screen();
	putstr("\n\n");
	putstr("        .--.         _\n");
	putstr("       |o_o |       | |\n");
	putstr("       |:_/ |       | |\n");
	putstr("      //   \\ \\      |_|\n");
	putstr("     (|     | )      _\n");
	putstr("    /'\\_   _/`\\     (_)\n");
	putstr("    \\___)=(___/\n");
	putstr("\n");
	putstr("                             *** KERNEL PANIC ***\n");
	putstr("\n");
	if (msg)
		printf("                               Reason: %s\n", msg);
	printf("                               System halted.\n\n");

	while (1)
	{
		__asm__ volatile ("hlt");
	}
}
