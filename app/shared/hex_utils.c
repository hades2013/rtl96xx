#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>

#include <basetype.h>

#define DIGITS_HEX 		"0123456789ABCDEF"
#define	HEX_EXTENDER 	':'

unsigned char todigit (unsigned char c)

{
	if ((c >= '0') && (c <= '9'))
	{
		return (c - '0');
	}
	if ((c >= 'A') && (c <= 'Z'))
	{
		return (c - 'A' + 10);
	}
	if ((c >= 'a') && (c <= 'z'))
	{
		return (c - 'a' + 10);
	}
	return (0);
}

void hexdump (const void * memory, size_t offset, size_t extent)

{
	unsigned char* origin = (unsigned char *)(memory);
	unsigned field = sizeof (extent) + sizeof (extent);
	unsigned block = 0x10;
	unsigned lower = block * (offset / block);
	unsigned upper = block + lower;
	unsigned index = 0;
	char buffer [80];
	char * output;
	fprintf(stderr, "\r\n");
	while (lower < extent)
	{
		output = buffer + field;
		for (index = lower; output-- > buffer; index >>= 4)
		{
			*output = DIGITS_HEX [index & 0x0F];
		}
		output = buffer + field;
		*output++ = ' ';
		for (index = lower; index < upper; index++)
		{
			if (index < offset)
			{
				*output++ = ' ';
				*output++ = ' ';
			}
			else if (index < extent)
			{
				*output++ = DIGITS_HEX [(origin [index] >> 4) & 0x0F];
				*output++ = DIGITS_HEX [(origin [index] >> 0) & 0x0F];
			}
			else
			{
				*output++ = ' ';
				*output++ = ' ';
			}
			*output++ = ' ';
		}
		for (index = lower; index < upper; index++)
		{
			if (index < offset)
			{
				*output++ = ' ';
			}
			else if (index < extent)
			{
				unsigned c = origin [index];
				*output++ = isprint (c)? c: '.';
			}
			else
			{
				*output++ = ' ';
			}
		}
		*output++ = '\n';
		*output++ = '\0';
		fprintf(stderr, buffer);
		lower += block;
		upper += block;
	}

#if 1

	output = buffer;
	*output++ = '\n';
	*output++ = '\0';
	fprintf(stderr, "%s\n", buffer);

#endif

	return;
}

size_t hexdecode (const unsigned char memory [], size_t extent, char buffer [], size_t length)
{
	char *string = buffer;
	if (length)
	{
		length /= 3;
		while ((length--) && (extent--))
		{
			*string++ = DIGITS_HEX [(*memory >> 4) & 0x0F];
			*string++ = DIGITS_HEX [(*memory >> 0) & 0x0F];
			if ((length) && (extent))
			{
				*string++ = HEX_EXTENDER;
			}
			memory++;
		}
		*string = (char) (0);
	}
	return (string - buffer);
}

char * hexstring (char buffer [], size_t length, const unsigned char memory [], size_t extent)

{
	hexdecode (memory, extent, buffer, length);
	return (buffer);
}

size_t hexencode (unsigned char memory [], size_t extent, const char * string)

{
	unsigned char * offset = memory;
	unsigned radix = 16;
	unsigned digit = 0;
	while ((extent) && (*string))
	{
		unsigned field = 2;
		unsigned value = 0;
		if ((offset > memory) && (*string == HEX_EXTENDER))
		{
			string++;
		}
		while (field--)
		{
			if ((digit = todigit (*string)) < radix)
			{
				value *= radix;
				value += digit;
				string++;
				continue;
			}
			errno = EINVAL;
			return (0);
		}
		*offset = value;
		offset++;
		extent--;
	}

	if ((extent) || (*string))
	{
		errno = EINVAL;
		return (0);
	}
	return (offset - memory);
}
