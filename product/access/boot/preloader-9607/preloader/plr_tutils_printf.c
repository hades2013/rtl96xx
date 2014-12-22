#include <preloader.h>
#include <stdarg.h>

#ifndef PRINTF_SECTION
    #define PRINTF_SECTION
#endif

void pblr_putc(const char c)  {
	if (c == '\n')
		parameters._uart_putc('\r');

		parameters._uart_putc(c);
}

void pblr_puts (const char *s) {
	while (*s) {
		pblr_putc(*s++);
	}
}

int pblr_strnlen(const char * s, int count) {
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc);
	return sc - s;
}

#define ZEROPAD 1  /* pad with zero */
#define SIGN    2  /* unsigned/signed long */
#define PLUS    4  /* show plus */
#define SPACE   8  /* space if plus */
#define LEFT    16 /* left justified */
#define SPECIAL 32 /* 0x */
#define LARGE   64 /* use 'ABCDEF' instead of 'abcdef' */

#define is_digit(c)     ((c) >= '0' && (c) <= '9')

#define do_div(n,base) ({	  \
			int __res; \
			__res = ((unsigned long) n) % (unsigned) base; \
			n = ((unsigned long) n) / (unsigned) base; \
			__res; \
		})

PRINTF_SECTION static int 
skip_atoi(const char **s) {
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define USE_PBLR_PRINTF
#ifdef USE_PBLR_PRINTF
// The new printf implement uses much less SRAM and stack area.
// the name is 'pblr_soc_printf'
const char *__lower_digits="0123456789abcdefghijklmnopqrstuvwxyz";
const char *__upper_digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#define my_putc(chr) do {pblr_putc(chr); ++cc;} while (0)
PRINTF_SECTION  static unsigned int
pblr_number(long num, int base, int size, int precision ,int type) {
    unsigned int cc=0;
	char c,sign,tmp[66];
	const char *digits=__lower_digits;
	int i;

	if (type & LARGE)
		digits = __upper_digits;
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0)
		     tmp[i++] = digits[do_div(num,base)];
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0) {
			//*str++ = ' ';
			my_putc(' ');
		}
	if (sign) {
		//*str++ = sign;
		my_putc(sign);
	}
	if (type & SPECIAL) {
		if (base==8) {
			//*str++ = '0';
			my_putc('0');
		} else if (base==16) {
			//*str++ = '0';
			my_putc('0');
			//*str++ = digits[33];
			my_putc(digits[33]);
		}
	}
	if (!(type & LEFT))
		while (size-- > 0) {
			//*str++ = c;
			my_putc(c);
		}
	while (i < precision--) {
		//*str++ = '0';
		my_putc('0');
	}
	while (i-- > 0) {
		//*str++ = tmp[i];
		my_putc(tmp[i]);
	}
	while (size-- > 0) {
		//*str++ = ' ';
		my_putc(' ');
	}
	//return str;
	return cc;
}


PRINTF_SECTION unsigned int
pblr_soc_printf(const char *fmt, ...){
    unsigned int cc=0;
	int len;
	unsigned long num;
	int i, base;
	const char *s;
	int flags;		/* flags to number() */
	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'q' for integer fields */
	va_list args;
	
	va_start(args, fmt);

	//for (str=buf ; *fmt ; ++fmt) {
	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			//*str++ = *fmt;
			my_putc(*fmt);
			continue;
		}

		/* process flags */
		flags = 0;
	repeat:
		++fmt;		/* this also skips first '%' */
		switch (*fmt) {
		case '-': flags |= LEFT; goto repeat;
		case '+': flags |= PLUS; goto repeat;
		case ' ': flags |= SPACE; goto repeat;
		case '#': flags |= SPECIAL; goto repeat;
		case '0': flags |= ZEROPAD; goto repeat;
		}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'q') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT)) {
				while (--field_width > 0) {
					//*str++ = ' ';
					my_putc(' ');
				}
            }
			//*str++ = (unsigned char) va_arg(args, int);
			{   
			    char temp_chr=(unsigned char) va_arg(args, int);
			    my_putc(temp_chr);
			}
			while (--field_width > 0) {
				//*str++ = ' ';
				my_putc(' ');
			}
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = pblr_strnlen(s, precision);

			if (!(flags & LEFT)) {
				while (len < field_width--) {
					//*str++ = ' ';
					my_putc(' ');
				}
            }
			for (i = 0; i < len; ++i) {
				//*str++ = *s++;
				my_putc(*s++);
			}
			while (len < field_width--) {
				//*str++ = ' ';
				my_putc(' ');
			}
			continue;

		case 'p':
			if (field_width == -1) {
				/* 121025,JJ,the final '+2' is for the additional '0x'. */
				field_width = 2*sizeof(void *) + 2;
				flags |= ZEROPAD;
			}
			flags |= SPECIAL;
			/*
			str = pblr_number(str,
			             (unsigned long) va_arg(args, void *), 16,
			             field_width, precision, flags);
            */
            cc += pblr_number((unsigned long) va_arg(args, void *), 16,
                field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				//*ip = (str - buf);
				*ip=cc;
			} else {
				int * ip = va_arg(args, int *);
				//*ip = (str - buf);
				*ip=cc;
			}
			continue;

		case '%':
			//*str++ = '%';
			my_putc('%');
			continue;

			/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			//*str++ = '%';
			my_putc('%');
			if (*fmt) {
				//*str++ = *fmt;
				my_putc(*fmt);
			} else {
				--fmt;
			}
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		//str = pblr_number(str, num, base, field_width, precision, flags);
		cc += pblr_number(num, base, field_width, precision, flags);
	}
	va_end (args);
	//*str = '\0';
	//return str-buf;
	return cc;
}



#else // USE_PBLR_PRINTF

PRINTF_SECTION static char * 
number(char * str, long num, int base, int size, int precision ,int type) {
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0)
		     tmp[i++] = digits[do_div(num,base)];
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type & SPECIAL) {
		if (base==8)
			*str++ = '0';
		else if (base==16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';
	return str;
}

PRINTF_SECTION static u32_t 
vsprintf(char *buf, const char *fmt, va_list args){
	int len;
	unsigned long num;
	int i, base;
	char * str;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
			   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'q' for integer fields */

	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
	repeat:
		++fmt;		/* this also skips first '%' */
		switch (*fmt) {
		case '-': flags |= LEFT; goto repeat;
		case '+': flags |= PLUS; goto repeat;
		case ' ': flags |= SPACE; goto repeat;
		case '#': flags |= SPECIAL; goto repeat;
		case '0': flags |= ZEROPAD; goto repeat;
		}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'q') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = pblr_strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;

		case 'p':
			if (field_width == -1) {
				/* 121025,JJ,the final '+2' is for the additional '0x'. */
				field_width = 2*sizeof(void *) + 2;
				flags |= ZEROPAD;
			}
			flags |= SPECIAL;
			str = number(str,
			             (unsigned long) va_arg(args, void *), 16,
			             field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		case '%':
			*str++ = '%';
			continue;

			/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str-buf;
}

#define CFG_PBSIZE (384+16)
PRINTF_SECTION unsigned int 
pblr_soc_printf(const char *fmt, ...) {
	va_list args;
	unsigned int i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	/* Print the string */
	pblr_puts(printbuffer);

	return i;
}
#endif // USE_PBLR_PRINTF
