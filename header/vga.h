#ifndef VGA_H
# define VGA_H

// the vga memory is splitted this way :

// | background color (4 bits) | 
// | foreground color (4 bits) | 
// |   char to print (8 bits)  |

// bc | fc | char = 16 bits

extern volatile uint_32t i;
extern volatile uint_16t *vga;
extern volatile uint_16t color;

extern const int MAX_HEIGHT;
extern const int MAX_WIDTH;

#define NMI_disable_bit 1

struct screen {
	uint_16t buffer[80 * 25];
    uint_32t cursor_pos;
};

// why 15 << 8 ? because it does in memory :

// 0000 | 1111 | 0000 0000 |
// bc = 0000 (it's off, so black)
// fc = 1111 (it's on, so white)
// char = 0000 0000 (not yet set)

/*VGA colors
Bit 76543210
    ||||||||
    |||||^^^-fore color
    ||||^----fore color bright bit
    |^^^-----back color
    ^--------back color bright bit OR enables blinking Text
*/
enum vga_colors
{
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHT_GREY = 7,
	DARK_GREY = 8,
	LIGHT_BLUE = 9,
	LIGHT_GREEN = 10,
	LIGHT_CYAN = 11,
	LIGHT_RED = 12,
	LIGHT_MAGENTA = 13,
	YELLOW = 14,
	WHITE = 15
};

#endif