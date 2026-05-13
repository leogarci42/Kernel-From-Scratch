#include "kernel.h"
#include "helpers.h"
#include <stdarg.h>

static uint_32t	display_arg(char c, va_list *ap)
{
	uint_32t	len;

	len = 0;
	if (c == 'c')
		len = print_char(va_arg(*ap, uint_32t));
	else if (c == 's')
		len = putstr(va_arg(*ap, char *));
	else if (c == 'p')
		len = print_ptr(va_arg(*ap, long long));
	else if (c == 'd' || c == 'i')
		len = print_nbr(va_arg(*ap, uint_32t));
	else if (c == 'u')
		len = print_unsigned_nbr(va_arg(*ap, uint_32t));
	else if (c == 'x' || c == 'X')
		len = print_hex(va_arg(*ap, uint_32t), c);
	else if (c == '%')
		len = print_char('%');
	return (len);
}

uint_32t	printf(const char *str, ...)
{
	uint_32t		i;
	uint_32t		len;
	va_list	ap;

	if (!str)
		return (-1);
	va_start(ap, str);
	i = 0;
	len = 0;
	while (str[i])
	{
		if (str[i] == '%')
		{
			len += display_arg(str[i + 1], &ap);
			i++;
		}
		else
			len += print_char(str[i]);
		i++;
	}
	va_end(ap);
	return (len);
}