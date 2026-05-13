#include "helpers.h"
#include "symtab.h"

uint_32t strlen(const char *s)
{
	uint_32t len = 0;
	while (*s++)
		len++;
	return len;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (unsigned char)*s1 - (unsigned char)*s2;
}

uint_32t	strlen_base(uint_32t nbr)
{
	uint_32t	len;

	len = 1;
	while (nbr >= 16)
	{
		nbr = nbr / 16;
		len++;
	}
	return (len);
}

uint_32t	check_base(char *base)
{
	uint_32t	i;
	uint_32t	j;

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

void	putnbr_base(uint_32t nbr, char *base)
{
	uint_32t	len;

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

void	putnbr(uint_32t n)
{
	if (n >= 10)
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

void	put_unsigned_nbr(uint_32t n)
{
	if (n >= 10)
	{
		put_unsigned_nbr(n / 10);
		put_unsigned_nbr(n % 10);
	}
	else
		putchar(n + 48);
}

const char *find_symbol(uint_32t addr, uint_32t *offset)
{
	const char *deflt = "unknown";
	uint_32t default_addr = 0;

	for (uint_32t i = 0; i < symtab_size; i++)
	{
		if (symtab[i].addr <= addr && symtab[i].addr > default_addr)
		{
			deflt = symtab[i].name;
			default_addr = symtab[i].addr;
		}
	}
	if (offset)
		*offset = addr - default_addr;
	return (deflt);
}

void dump_stack(void)
{
    uint_32t *ebp;

    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    printf("\n");
    printf("\n");
	uint_32t pad = (80 - 8 - strlen("KERNEL STACK TRACE")) / 2;
		for (uint_32t i = 0; i < pad; i++)
			putstr(" ");
    printf("KERNEL STACK TRACE\n");
    printf("\n");
    int depth = 0;
    while (ebp && depth < 32)
    {
        uint_32t ret = ebp[1];
        if (ret == 0)
            break;

        uint_32t offset;
        const char *name = find_symbol(ret, &offset);
        printf("  [%d] 0x%x in %s + 0x%x\n",
                depth,
                ret,
                name,
                offset);
        ebp = (uint_32t *)ebp[0];
        depth++;
    }
    printf("\n");
}
