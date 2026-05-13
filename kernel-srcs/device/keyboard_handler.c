#include "keyboard_map.h"
#include "kernel.h"
#include "vga.h"
#include "helpers.h"

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

bool_t check_if_left_pos_empty(uint_32t pos)
{
	if (pos % MAX_WIDTH == 0)
		return false_t;
	return (vga[pos - 1] & 0xFF) == ' ' || (vga[pos - 1] & 0xFF) == 0;
}

uint_32t get_last_filled_row(void)
{
	uint_32t last_row = 0;

	for (uint_32t row = 0; row < MAX_HEIGHT; row++)
	{
		for (uint_32t col = 0; col < MAX_WIDTH; col++)
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
		uint_32t current_row = i / MAX_WIDTH;
		uint_32t last_row = get_last_filled_row();
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
	static bool_t ctrl_pressed = false_t;
	static bool_t alt_pressed = false_t;
	static bool_t prefix_e0 = false_t;
	static char input_buffer[64] = {0};
	static uint_32t buffer_pos = 0;
	uint_8t scancode = inb(0x60);

	// prefix e0 (for arrow keys)
	if (scancode == 0xE0)
	{
		prefix_e0 = true_t;
		return;
	}

	// cursor movement
	if (prefix_e0)
	{
		prefix_e0 = false_t;
		move_cursor(scancode);
		return;
	}

	// ctrl
	if (scancode == 0x1D)
	{
		ctrl_pressed = true_t;
		return;
	}
	if (scancode == 0x9D)
	{
		ctrl_pressed = false_t;
		return;
	}

	// alt
	if (scancode == 0x38)
	{
		alt_pressed = true_t;
		return;
	}
	if (scancode == 0xB8)
	{
		alt_pressed = false_t;
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

	if (c == '\n')
	{
		input_buffer[buffer_pos] = 0;
		printf("\n[Buffer: '%s', len: %d]\n", input_buffer, buffer_pos);
		// Use string comparison instead of character by character
		if (strcmp(input_buffer, "stack") == 0)
		{
			printf("STACK COMMAND DETECTED\n");
			clear_screen();
			dump_stack();
		}
		// Clear buffer completely
		for (uint_32t i = 0; i < 64; i++)
			input_buffer[i] = 0;
		buffer_pos = 0;
		putchar(c);
	}
	else if (buffer_pos < 63)
	{
		input_buffer[buffer_pos++] = c;
		putchar(c);
	}
	else
	{
		putchar(c);
	}

	// printf("\nScancode: %x, Char: %c, i: %d\n", scancode, c, i);
}
