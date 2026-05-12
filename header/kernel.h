#ifndef KERNEL_H
# define KERNEL_H

// Why ?
// it's our own OS which means no library available

typedef unsigned char uint_8t;
typedef unsigned short uint_16t;
typedef unsigned int uint_32t;
typedef uint_8t bool_t;

#define true_t 1
#define false_t 0

/*		helper function for VGA		*/

void init_screens(void);
void clear_screen();
void update_cursor(int pos);
void keyboard_handler();
void putchar(char c);
void change_color();
void switch_screen(int screen_num);
int	putstr(const char *s);
int	printf(const char *str, ...);
void gdt_init(void);
void gdt_install(void);

/*			keyboard handler		*/

uint_8t inb(uint_16t port);
void outb(uint_16t port, uint_8t val);

/*			kernel panic		*/

void kernel_panic(const char *msg);

#endif