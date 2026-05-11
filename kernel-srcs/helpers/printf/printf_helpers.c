#include "helpers.h"

int	print_unsigned_nbr(unsigned int nbr)
{
	int	len;

	put_unsigned_nbr(nbr);
	len = 1;
	while (nbr > 9)
	{
		nbr = nbr / 10;
		len++;
	}
	return (len);
}

int	print_hex(unsigned int nbr, char c)
{
	int	len;

	if (c == 'x')
		putnbr_base(nbr, "0123456789abcdef");
	else if (c == 'X')
		putnbr_base(nbr, "0123456789ABCDEF");
	else
		return (0);
	len = strlen_base(nbr);
	return (len);
}

int	print_nbr(int nbr)
{
	int		len;
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

int	print_ptr(unsigned long long nbr)
{
	int	len;

	if (nbr == 0)
		return (0);
	putstr("0x");
	len = 2;
	putlongnbr_base(nbr, "0123456789abcdef");
	len += strlen_base(nbr);
	return (len);
}

int	print_char(char c)
{
	putchar(c);
	return (1);
}

int	putstr(const char *s)
{
	if (!s)
		return (6);
	while (*s)
		putchar(*s++);
	return (strlen(s));
}
