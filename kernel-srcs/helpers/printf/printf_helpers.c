#include "helpers.h"
#include "kernel.h"


uint_32t	print_unsigned_nbr(uint_32t nbr)
{
	uint_32t	len;

	put_unsigned_nbr(nbr);
	len = 1;
	while (nbr > 9)
	{
		nbr = nbr / 10;
		len++;
	}
	return (len);
}

uint_32t	print_hex(uint_32t nbr, char c)
{
	uint_32t	len;

	if (c == 'x')
		putnbr_base(nbr, "0123456789abcdef");
	else if (c == 'X')
		putnbr_base(nbr, "0123456789ABCDEF");
	else
		return (0);
	len = strlen_base(nbr);
	return (len);
}

uint_32t	print_nbr(uint_32t nbr)
{
	uint_32t		len;
	long	l_nbr;

	putnbr(nbr);
	len = 1;
	l_nbr = nbr;
	if (nbr < 0)
	{
		len++;
		l_nbr = -l_nbr;
	}
	while (l_nbr > 9)
	{
		l_nbr = l_nbr / 10;
		len++;
	}
	return (len);
}

uint_32t	print_ptr(unsigned long long nbr)
{
	uint_32t	len;

	if (nbr == 0)
		return (0);
	putstr("0x");
	len = 2;
	putlongnbr_base(nbr, "0123456789abcdef");
	len += strlen_base(nbr);
	return (len);
}

uint_32t	print_char(char c)
{
	putchar(c);
	return (1);
}

uint_32t	putstr(const char *s)
{
	if (!s)
		return (6);
	while (*s)
		putchar(*s++);
	return (strlen(s));
}
