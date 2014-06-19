/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_libc.c
*
* DESCRIPTION: 
*   
*
* Date Created: Nov 03, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_libc.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos_libc.h>
#include <vos_alloc.h>
#include <stdlib.h>

#ifndef RTOS_STRINGS

int 
vosStrLen(const char *s)
{
    const char *s_orig = s;

    if (NULL == s)
        return 0;

    while (*s != 0) {
    s++;
    }

    return (s - s_orig);
}

char *
vosStrCpy(char *dst, const char *src)
{
    char *dst_orig = dst;

    while ((*dst++ = *src++) != 0)
    ;

    return dst_orig;
}




int 
vosStrCmp(const char *s1, const char *s2)
{
    do {
    if (*s1 < *s2) {
        return -1;
    } else if (*s1 > *s2) {
        return 1;
    }
    s1++;
    } while (*s2++);

    return 0;
}

void *
vosMemCpy(void *dst_void, const void *src_void, size_t len)
{
    unsigned char *dst = dst_void;
    const unsigned char *src = src_void;

    while (len--) {
    *dst++ = *src++;
    }

    return dst_void;
}

void *
vosMemSet(void *dst_void, int val, size_t len)
{
    unsigned char *dst = dst_void;

    while (len--) {
    *dst++ = (unsigned char) val;
    }

    return dst_void;
}


#endif /* !RTOS_STRINGS */

int 
vosMemCmp(const void *m1_void, const void *m2_void, size_t len)
{
    const unsigned char *m1 = m1_void, *m2 = m2_void;

    while (len-- != 0) {
    if (*m1 < *m2) {
        return -1;
    } else if (*m1 > *m2) {
        return 1;
    }
    m1++;
    m2++;
    } 

    return 0;
}

char *
vosStrDup(const char *s)
{
    int len = vosStrLen(s);
    char *rc = vosAlloc(len + 1);
    if (rc != NULL) {
    vosStrCpy(rc, s);
    }
    return rc;
}

/*
 * vosStrToInt
 *
 *   Converts a C-style constant to integer.
 *   Also supports '0b' prefix for binary.
 */

int 
vosStrToInt(const char *s, char **end)
{
    unsigned int    n, neg, base = 10;

    if (s == 0) {
        if (end != 0) {
            end = 0;
        }
        return 0;
    }

    s += (neg = (*s == '-'));

    if (*s == '0') {
    s++;
    if (*s == 'x' || *s == 'X') {
        base = 16;
        s++;
    } else if (*s == 'b' || *s == 'B') {
        base = 2;
        s++;
    } else {
        base = 8;
    }
    }

    for (n = 0; ((*s >= 'a' && *s < (char)('a' + base - 10)) ||
         (*s >= 'A' && *s < (char)('A' + base - 10)) ||
         (*s >= '0' && *s <= '9')); s++) {
    n = n * base + ((*s <= '9' ? *s : *s + 9) & 15);
    }

    if (end != 0) {
        *end = (char *) s;
    }

    return (int) (neg ? -n : n);
}

unsigned int 
vosStrToUInt(const char *s, char **end)
{
    unsigned int    n, neg, base = 10;

    if (s == 0) {
        if (end != 0) {
            end = 0;
        }
        return 0;
    }

    s += (neg = (*s == '-'));

    if (*s == '0') {
    s++;
    if (*s == 'x' || *s == 'X') {
        base = 16;
        s++;
    } else if (*s == 'b' || *s == 'B') {
        base = 2;
        s++;
    } else {
        base = 8;
    }
    }

    for (n = 0; ((*s >= 'a' && *s < (char)('a' + base - 10)) ||
         (*s >= 'A' && *s < (char)('A' + base - 10)) ||
         (*s >= '0' && *s <= '9')); s++) {
    n = n * base + ((*s <= '9' ? *s : *s + 9) & 15);
    }

    if (end != 0) {
        *end = (char *) s;
    }

    return (neg ? -n : n);
}


void
vosIntToStr(char *buf,      /* Large enough result buffer   */
     uint32 num,        /* Number to convert        */
     int base,          /* Conversion base (2 to 16)    */
     int caps,          /* Capitalize letter digits */
     int prec)      /* Precision (minimum digits)   */
{
    char        tmp[36], *s, *digits;

    digits = (caps ? "0123456789ABCDEF" : "0123456789abcdef");

    s = &tmp[sizeof (tmp) - 1];

    for (*s = 0; num || s == &tmp[sizeof (tmp) - 1]; num /= base, prec--)
    *--s = digits[num % base];

    while (prec-- > 0)
    *--s = '0';

    vosStrCpy(buf, s);
}

char *vosStrStr(const char *s1, const char *s2)
{
	register const char *s = s1;
	register const char *p = s2;

	do {
		if (!*p) {
			return (char *) s1;;
		}
		if (*p == *s) {
			++p;
			++s;
		} else {
			p = s2;
			if (!*s) {
				return NULL;
			}
			s = ++s1;
		}
	} while (1);
}

char *
vosStrCat(char *dst, const char *src)
{
	char *s1 = dst;
	const char *s2 = src;
	char c;
	
	/* Find the end of the string.	*/
	do
	  c = *s1++;
	while (c != '\0');
	
	/* Make S1 point before the next character, so we can increment
	   it while memory is read (wins on pipelined cpus).  */
	s1 -= 2;
	
	do
	  {
		c = *s2++;
		*++s1 = c;
	  }
	while (c != '\0');
	
	return dst;

}

#ifdef COMPILER_HAS_DOUBLE

void
vosDoubleToStr(char *buf, double f, int decimals)
{
    int         exp = 0;
    unsigned int    int_part;
    double      round;
    int         i;

    if (f < 0.0) {
    *buf++ = '-';
    f = -f;
    }

    for (round = 0.5, i = 0; i < decimals; i++)
    round /= 10.0;

    f += round;

    if (f >= 4294967296.0)
    while (f >= 10.0) {
        f /= 10.0;
        exp++;
    }

    int_part = (unsigned int) f;
    f -= int_part;

    vosIntToStr(buf, int_part, 10, 0, 0);
    while (*buf)
    buf++;

    *buf++ = '.';

    for (i = 0; i < decimals; i++) {
    f *= 10.0;
    int_part = (unsigned int) f;
    f -= int_part;
    *buf++ = '0' + int_part;
    }

    if (exp) {
    *buf++ = 'e';
    vosIntToStr(buf, exp, 10, 0, 0);
    } else
    *buf = 0;
}

#endif /* COMPILER_HAS_DOUBLE */

#define X_STORE(c) {    \
    if (bp < be)    \
    *bp = (c);  \
    bp++;       \
}

#define X_INF       0x7ffffff0

int vosVSnprintf(char *buf, size_t bufsize, const char *fmt, va_list ap)
{
    char        c, *bp, *be;

    bp = buf;
    be = (bufsize == X_INF) ? ((char *) 0) - 1 : &buf[bufsize - 1];

    while ((c = *fmt++) != 0) {
    int         width = 0, ljust = 0, plus = 0, space = 0;
    int     altform = 0, prec = 0, half = 0, base = 0;
    int     tlong = 0, fillz = 0, plen, pad, prec_given = 0;
    long        num = 0;
    char        tmp[36], *p = tmp;

    if (c != '%') {
        X_STORE(c);
        continue;
    }

    for (c = *fmt++; ; c = *fmt++)
        switch (c) {
        case 'h': half = 1;     break;
        case 'l': tlong = 1;    break;
        case '-': ljust = 1;    break;
        case '+': plus = 1;     break;
        case ' ': space = 1;    break;
        case '0': fillz = 1;    break;
        case '#': altform = 1;  break;
        case '*': width = -1;   break;  /* Mark as need-to-fetch */
        case '.':
        if ((c = *fmt++) == '*')
            prec = -1;          /* Mark as need-to-fetch */
        else {
            for (prec = 0; c >= '0' && c <= '9'; c = *fmt++)
            prec = prec * 10 + (c - '0');
            fmt--;
        }
        prec_given = 1;
        break;
        default:
        if (c >= '1' && c <= '9') {
            for (width = 0; c >= '0' && c <= '9'; c = *fmt++)
            width = width * 10 + (c - '0');
            fmt--;
        } else
            goto break_for;
        break;
        }
    break_for:

    if (width == -1)
        width = va_arg(ap,int);
    if (prec == -1)
        prec = va_arg(ap,int);

    if (c == 0)
        break;

    switch (c) {
    case 'd':
    case 'i':
        num = tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
        num = (int) (short) num;
        /* For zero-fill, the sign must be to the left of the zeroes */
        if (fillz && (num < 0 || plus || space)) {
        X_STORE(num < 0 ? '-' : space ? ' ' : '+');
        if (width > 0)
            width--;
        if (num < 0)
            num = -num;
        }
        if (! fillz) {
        if (num < 0) {
            *p++ = '-';
            num = -num;
        } else if (plus)
            *p++ = '+';
        else if (space)
            *p++ = ' ';
        }
        base = 10;
        break;
    case 'u':
        num = tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
        num = (int) (short) num;
        base = 10;
        break;
    case 'p':
        altform = 0;
        /* Fall through */
    case 'x':
    case 'X':
        num = tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
        num = (int) (unsigned short) num;
        if (altform) {
        prec += 2;
        *p++ = '0';
        *p++ = c;
        }
        base = 16;
        break;
    case 'o':
    case 'O':
        num = tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
        num = (int) (unsigned short) num;
        if (altform) {
        prec++;
        *p++ = '0';
        }
        base = 8;
        break;
#ifdef COMPILER_HAS_DOUBLE
    case 'f':
        {
        double      f;

        f = va_arg(ap, double);
        if (! prec_given)
            prec = 6;
        vosDoubleToStr(p, f, prec);
        fillz = 0;
        p = tmp;
        prec = X_INF;
        }
        break;
#endif /* COMPILER_HAS_DOUBLE */
    case 's':
        p = va_arg(ap,char *);
        if (prec == 0)
        prec = X_INF;
        break;
    case 'c':
        p[0] = va_arg(ap,int);
        p[1] = 0;
        prec = 1;
        break;
    case 'n':
        *va_arg(ap,int *) = bp - buf;
        p[0] = 0;
        break;
    case '%':
        p[0] = '%';
        p[1] = 0;
        prec = 1;
        break;
    default:
        X_STORE(c);
        continue;
    }

    if (base != 0) {
        vosIntToStr(p, (unsigned int) num, base, (c == 'X'), prec);
        if (prec)
        fillz = 0;
        p = tmp;
        prec = X_INF;
    }

    if ((plen = vosStrLen(p)) > prec)
        plen = prec;

    if (width < plen)
        width = plen;

    pad = width - plen;

    while (! ljust && pad-- > 0)
        X_STORE(fillz ? '0' : ' ');
    for (; plen-- > 0 && width-- > 0; p++)
        X_STORE(*p);
    while (pad-- > 0)
        X_STORE(' ');
    }

    if (bp < be)
    *bp = 0;
    else
    *be = 0;

    return (bp - buf);
}

int vosVSprintf(char *buf, const char *fmt, va_list ap)
{
    return vsnprintf(buf, (size_t) X_INF, fmt, ap);
}

int vosSnprintf(char *buf, size_t bufsize, const char *fmt, ...)
{
    va_list     ap;
    int         r;

    va_start(ap,fmt);
    r = vsnprintf(buf, bufsize, fmt, ap);
    va_end(ap);

    return r;
}

int vosSprintf(char *buf, const char *fmt, ...)
{
    va_list     ap;
    int         r;

    va_start(ap,fmt);
    r = vsnprintf(buf, (size_t) X_INF, fmt, ap);
    va_end(ap);

    return r;
}

char * vosSafeStrNCpy(char *dst, const char *src, size_t size)
{
  if (!size) return dst;
  dst[--size] = '\0';
  return strncpy(dst, src, size);
}

char* vosLastCharIs(const char *s, int c)
{
  if (s && *s) {
    size_t sz = strlen(s) - 1;
    s += sz;
    if ( (unsigned char)*s == c)
      return (char*)s;
  }
  return NULL;
}


/*
 * Like vsprintf but provides a pointer to malloc'd storage, which must
 *  be freed by the caller.
 */
char *vosVAsprintf(const char *fmt, va_list *args)
{
  int r;
  char *s, *result;
  const char *p = fmt;
  /* Add one to make sure that it is never zero, which might cause vosAlloc
     to return NULL.  */
  int total_width = vosStrLen (fmt) + 1;
  va_list ap;

  memcpy(&ap, args, sizeof(va_list));
  
  while (*p != '\0')
  {
    if (*p++ == '%')
    {
      while (strchr ("-+ #0", *p))
        ++p;
      if (*p == '*')
      {
        ++p;
        total_width += abs (va_arg (ap, int));
      }
      else
        total_width += strtoul (p, (char **) &p, 10);
      if (*p == '.')
      {
        ++p;
        if (*p == '*')
        {
          ++p;
          total_width += abs (va_arg (ap, int));
        }
        else
          total_width += strtoul (p, (char **) &p, 10);
      }
      while (strchr ("hlL", *p))
        ++p;
      /* Should be big enough for any format specifier except %s and floats.  */
      total_width += 30;
      switch (*p)
      {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
        case 'c':
          (void) va_arg (ap, int);
          break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
          (void) va_arg (ap, double);
          /* Since an ieee double can have an exponent of 307, we'll
          make the buffer wide enough to cover the gross case. */
          total_width += 307;
          break;
        case 's':
          total_width += vosStrLen (va_arg (ap, char *));
          break;
        case 'p':
        case 'n':
          (void) va_arg (ap, char *);
          break;
      }
      p++;
    }
  }
  result = vosAlloc (total_width);

  if (result != NULL) {
    vsprintf(result, fmt, *args);
  }

  return result;
}

char *vosAsprintf(const char *fmt, ...)
{
  va_list p;
  char *string_ptr;
  
  va_start(p, fmt);
  string_ptr = vosVAsprintf(fmt, &p);
  va_end(p);
  
  return string_ptr;
}

uint32 *crc32FillTable(int endian)
{
    uint32 *crc_table = vosAlloc(256 * sizeof(uint32));
    uint32 polynomial = endian ? 0x04c11db7 : 0xedb88320;
    uint32 c;
    int i, j;

    for (i = 0; i < 256; i++) {
        c = endian ? (i << 24) : i;
        for (j = 8; j; j--) {
            if (endian)
                c = (c&0x80000000) ? ((c << 1) ^ polynomial) : (c << 1);
            else
                c = (c&1) ? ((c >> 1) ^ polynomial) : (c >> 1);
        }
        *crc_table++ = c;
    }

    return crc_table - 256;
}

uint32 vosCRC32(const char *s,int size)
{
#if defined(OPL_BIG_ENDIANESS) || defined(OPCONN_BIG_ENDIAN)
    uint32 *crc32_table = crc32FillTable(1);
#else
    uint32 *crc32_table = crc32FillTable(0);
#endif

    uint32 crc;
    long length;
    int bytes_read;
    char *cp;

    crc = 0;
    length = 0;
    bytes_read=size;
    
    cp = (char *)s;
    length += bytes_read;
    while (bytes_read--)
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ (*cp++)) & 0xffL];

    for (; length; length >>= 8)
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ length) & 0xffL];
    crc ^= 0xffffffffL;

    vosFree(crc32_table);
    return crc;
}

int vosCheckCRC(const char *s,int size,unsigned int value)
{
    uint32 crc = vosCRC32(s, size);

    if(crc == value){
        return OK;
    }

    return ERROR;
}


