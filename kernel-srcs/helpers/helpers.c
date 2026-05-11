#include "helpers.h"

int strlen(const char *s)
{
	int len = 0;
	while (*s++)
		len++;
	return len;
}

int	strlen_base(unsigned int nbr)
{
	int	len;

	len = 1;
	while (nbr >= 16)
	{
		nbr = nbr / 16;
		len++;
	}
	return (len);
}

unsigned int	check_base(char *base)
{
	int	i;
	int	j;

	if (strlen(base) <= 1)
		return (0);
	i = 0;
	while (base[i])
	{
		if (base[i] == '+' || base[i] == '-')
			return (0);
		j = i + 1;
		while (base[j])
		{
			if (base[i] == base[j])
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

void	putnbr_base(unsigned int nbr, char *base)
{
	unsigned int	len;

	len = strlen(base);
	if (check_base(base) == 1)
	{
		if (nbr >= len)
		{
			putnbr_base(nbr / len, base);
			putnbr_base(nbr % len, base);
		}
		else
			putchar(base[nbr]);
	}
}

void	putnbr(int n)
{
	if (n == -2147483648)
		putstr("-2147483648");
	else if (n < 0)
	{
		putchar('-');
		putnbr(-n);
	}
	else if (n >= 10)
	{
		putnbr(n / 10);
		putnbr(n % 10);
	}
	else
		putchar(n + 48);
}

void	putlongnbr_base(unsigned long long nbr, char *base)
{
	if (nbr >= 16)
	{
		putlongnbr_base(nbr / 16, base);
		putlongnbr_base(nbr % 16, base);
	}
	else
		putchar(base[nbr]);
}

void	put_unsigned_nbr(unsigned int n)
{
	if (n >= 10)
	{
		put_unsigned_nbr(n / 10);
		put_unsigned_nbr(n % 10);
	}
	else
		putchar(n + 48);
}


