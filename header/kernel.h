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
void vga_console_init(void);
void clear_screen();
void update_cursor(uint_32t pos);
void keyboard_handler();
void putchar(char c);
void change_color();
void switch_screen(uint_32t screen_num);
uint_32t	putstr(const char *s);
uint_32t	printf(const char *str, ...);
void gdt_init(void);
void gdt_install(void);
void init_memory_paging(void *mboot_header);

void *alloc_page(void);
void free_page(void *page);
uint_32t get_page_size(void);
void map_page(uint_32t vaddr, uint_32t paddr);
void unmap_page(uint_32t vaddr);
void *alloc_virtual(uint_32t size);
void free_virtual(void *addr);
uint_32t get_allocation_size(void *addr);

/*			keyboard handler		*/

uint_8t inb(uint_16t port);
void outb(uint_16t port, uint_8t val);

/*			kernel panic		*/

void kernel_panic(const char *msg);

/*			dump_stack			*/

void dump_stack(void);

#endif