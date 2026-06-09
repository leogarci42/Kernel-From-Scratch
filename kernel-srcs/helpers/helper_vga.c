#include "kernel.h"
#include "vga.h"

const uint_32t MAX_HEIGHT = 25;
const uint_32t MAX_WIDTH = 80;

struct screen screens[3];
uint_32t current_screen = 0;

enum vga_colors vga_color = 15;

volatile uint_16t color = (15 << 8);
volatile uint_32t i = 0;
volatile uint_16t *vga = (volatile uint_16t *)0xB8000;

void update_cursor(uint_32t pos)
{
	// 0x0F is the register for the low byte of the cursor position
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint_8t)(pos & 0xFF));

	// 0x0E is the register for the high byte of the cursor position
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint_8t)((pos >> 8) & 0xFF));
}

void clear_screen()
{
	for (uint_32t j = 0; j < MAX_WIDTH * MAX_HEIGHT; j++)
		vga[j] = color | ' ';
	i = 0;
	update_cursor(0);
}

void init_screens(void)
{
	for (uint_32t s = 0; s < 3; s++)
	{
		for (uint_32t j = 0; j < MAX_WIDTH * MAX_HEIGHT; j++)
			screens[s].buffer[j] = (uint_16t)0x0720;
		screens[s].cursor_pos = 0;
	}
}

void scroll() 
{
	// if i (cursor) still on the visible window
	if (i < MAX_WIDTH * MAX_HEIGHT)
		return;
	// if not, then we shift memory line[0] = 0 - 80; line[1] = 80 - 160; line[0] = line[1];
	for (uint_32t j = 0; j < (MAX_HEIGHT - 1) * MAX_WIDTH; j++)
		vga[j] = vga[j + MAX_WIDTH];
	for (uint_32t j = (MAX_HEIGHT - 1) * MAX_WIDTH; j < MAX_HEIGHT * MAX_WIDTH; j++)
		vga[j] = color | ' ';
	i = (MAX_HEIGHT - 1) * MAX_WIDTH;
}

void change_color()
{
	if (vga_color == 15)
		vga_color = 1;
	else
		vga_color++;

	color = vga_color << 8;
}

void putchar(char c)
{
	if (c == '\n')
	{
		i = (i / MAX_WIDTH + 1) * MAX_WIDTH;
	}
	// basically it's bringing the write to the next line (the one under it)
	else if (c == '\b')
	{
		if (i == 0)
		{
			update_cursor(0);
			return;
		}
		//if at the start of  a newline
		if (i % MAX_WIDTH == 0) 
		{
			//brin back in front of the last char / the start of the line before
			i--;
			while (i % MAX_WIDTH != 0 && (vga[i] & 0xFF) == ' ')
				i--;
			if ((vga[i] & 0xFF) != ' ')
				i++;
		}
		else
		{
			i--;
			vga[i] = color | ' ';
		}
	}
	else
		vga[i++] = color | (uint_8t)c;
	// allow to add the char to the already made color into vga.
	scroll();
	update_cursor(i);
}

void switch_screen(uint_32t screen_num)
{
	if (screen_num >= 3)
		return;
	// save current screen
	struct screen *current = &screens[current_screen];
	for (uint_32t i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++)
		current->buffer[i] = vga[i];
	current->cursor_pos = i;

	// clear_screen();

	// load new screen
	struct screen *new_screen = &screens[screen_num];
	for (uint_32t i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++)
		vga[i] = new_screen->buffer[i];
	i = new_screen->cursor_pos;
	update_cursor(i);
	current_screen = screen_num;
}
