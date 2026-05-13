#ifndef HELPERS_H
# define HELPERS_H
#include "kernel.h"

/*				print helpers			*/

uint_32t	print_unsigned_nbr(uint_32t nbr);
uint_32t	print_hex(uint_32t nbr, char c);
uint_32t	print_nbr(uint_32t nbr);
uint_32t	print_ptr(unsigned long long nbr);
uint_32t	print_char(char c);
uint_32t	putstr(const char *s);

/*			more common helpers			*/

uint_32t strlen(const char *s);
uint_32t	strlen_base(uint_32t nbr);
uint_32t	check_base(char *base);
int strcmp(const char *s1, const char *s2);
void	putnbr_base(uint_32t nbr, char *base);
void	putnbr(uint_32t n);
void	putlongnbr_base(unsigned long long nbr, char *base);
void	put_unsigned_nbr(uint_32t n);
void putchar(char c);
void dump_stack(void);

#endif