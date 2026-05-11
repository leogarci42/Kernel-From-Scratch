#ifndef HELPERS_H
# define HELPERS_H

/*				print helpers			*/

int	print_unsigned_nbr(unsigned int nbr);
int	print_hex(unsigned int nbr, char c);
int	print_nbr(int nbr);
int	print_ptr(unsigned long long nbr);
int	print_char(char c);
int	putstr(const char *s);

/*			more common helpers			*/

int strlen(const char *s);
int	strlen_base(unsigned int nbr);
unsigned int	check_base(char *base);
void	putnbr_base(unsigned int nbr, char *base);
void	putnbr(int n);
void	putlongnbr_base(unsigned long long nbr, char *base);
void	put_unsigned_nbr(unsigned int n);
void putchar(char c);

#endif