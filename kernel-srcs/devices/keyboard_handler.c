#include "keyboard_map.h"
#include "kernel.h"
#include "vga.h"

uint_8t inb(uint_16t port)
{
    uint_8t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint_16t port, uint_8t val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

bool check_if_left_pos_empty(uint_32t pos)
{
	if (pos % MAX_WIDTH == 0)
		return false;
	return (vga[pos - 1] & 0xFF) == ' ' || (vga[pos - 1] & 0xFF) == 0;
}

int get_last_filled_row(void)
{
	int last_row = 0;

	for (int row = 0; row < MAX_HEIGHT; row++)
	{
		for (int col = 0; col < MAX_WIDTH; col++)
		{
			char c = (char)(vga[row * MAX_WIDTH + col] & 0xFF);
			if (c != ' ' && c != 0)
			{
				last_row = row;
				break;
			}
		}
	}
	return last_row;
}

void move_cursor(uint_8t scancode)
{
	if (scancode == 0x4B) // LEFT
	{
		if (i > 0)
			i--;
	}
	else if (scancode == 0x4D) // RIGHT
	{
		if (i < MAX_WIDTH * MAX_HEIGHT - 1)
			i++;
	}
	else if (scancode == 0x48) // UP
	{
		if (i >= MAX_WIDTH)
			i -= MAX_WIDTH;
	}
	else if (scancode == 0x50) // DOWN
	{
		int current_row = i / MAX_WIDTH;
		int last_row = get_last_filled_row();
		if (current_row < last_row && i < (MAX_HEIGHT - 1) * MAX_WIDTH)
			i += MAX_WIDTH;
	}
	else if (scancode == 0x47) // HOME
	{
		i = (i / MAX_WIDTH) * MAX_WIDTH;
	}
	while (check_if_left_pos_empty(i) && i > 0)
		i--;
	update_cursor(i);
}

void keyboard_handler()
{
// why 0x60? Intel 8042 PS/2 Controller I/O port addr 
	static bool ctrl_pressed = false;
	static bool alt_pressed = false;
	static bool prefix_e0 = false;
	uint_8t scancode = inb(0x60);

	// prefix e0 (for arrow keys)
	if (scancode == 0xE0)
	{
		prefix_e0 = true;
		return;
	}

	// cursor movement
	if (prefix_e0)
	{
		prefix_e0 = false;
		move_cursor(scancode);
		return;
	}

	// ctrl
	if (scancode == 0x1D)
	{
		ctrl_pressed = true;
		return;
	}
	if (scancode == 0x9D)
	{
		ctrl_pressed = false;
		return;
	}

	// alt
	if (scancode == 0x38)
	{
		alt_pressed = true;
		return;
	}
	if (scancode == 0xB8)
	{
		alt_pressed = false;
		return;
	}

	// key release
	if (scancode & 0x80)
		return;

	// get char from scancode
	char c = keyboard_map[scancode];
	if (c == 0)
		return;

	// switch screen
	if (alt_pressed)
	{
		switch_screen(c - '1');
		return;
	}

	// if ctrl + r is pressed, change color
	if (ctrl_pressed && c == 'r')
	{
		change_color();
		return;
	}

	putchar(c);

	// printf("\nScancode: %x, Char: %c, i: %d\n", scancode, c, i);
}
