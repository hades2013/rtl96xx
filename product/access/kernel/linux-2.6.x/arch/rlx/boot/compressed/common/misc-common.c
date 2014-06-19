/*
 * arch/mips/boot/compressed/common/misc-common.c
 * 
 * Misc. bootloader code (almost) all platforms can use
 *
 * Author: Johnnie Peters <jpeters@mvista.com>
 * Editor: Tom Rini <trini@mvista.com>
 *
 * Derived from arch/ppc/boot/prep/misc.c
 *
 * Ported by Pete Popov <ppopov@mvista.com> to
 * support mips board(s).  I also got rid of the vga console
 * code.
 *
 * Copyright 2000-2001 MontaVista Software Inc.
 *
 * Ported to MIPS 2.6 by Pete Popov, <ppopov@embeddedalley.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR   IMPLIED
 * WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT,  INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 * USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdarg.h>    /* for va_ bits */
#include <linux/autoconf.h>
#include <linux/string.h>
#include <linux/zlib.h>
#include <linux/types.h>

extern char *avail_ram;
extern char *end_avail;
extern char _end[];

void puts(const char *);
void putc(const char c);
void puthex(unsigned long val);
void _bcopy(char *src, char *dst, int len);
static int _cvt(unsigned long val, char *buf, long radix, char *digits);

void _vprintk(void(*)(const char), const char *, va_list ap);
void _printk(char const *fmt, ...);

void serial_putc(unsigned char);

void pause(void)
{
       puts("pause\n");
}

void puts_exit(void)
{
       puts("exit\n");
       while(1); 
}


void 
putc(const char c)
{
       serial_putc(c);
       if ( c == '\n' )
               serial_putc('\r');
}

void puts(const char *s)
{
       char c;
       while ( ( c = *s++ ) != '\0' ) {
               serial_putc(c);
               if ( c == '\n' ) serial_putc('\r');
       }
}

void error(char *x)
{
       puts("\n\n");
       puts(x);
       puts("\n\n -- System halted");

       while(1);       /* Halt */
}

static void *zalloc(unsigned size)
{
       void *p = avail_ram;
//       _printk("zalloc: size %x avail_ram %x end_avail %x",
//	       size, avail_ram, end_avail);
       size = (size + 7) & -8;
       avail_ram += size;
       if (avail_ram > end_avail) {
               puts("oops... out of memory\n");
               pause();
       }
       return p;
}


#define HEAD_CRC       2
#define EXTRA_FIELD    4
#define ORIG_NAME      8
#define COMMENT                0x10
#define RESERVED       0xe0

#define DEFLATED       8

#ifdef CONFIG_RTK_UNGZIP_WATCHDOG_RESET
/* Register access macro (REG*()) */
#ifndef REG32
#define REG32(reg)  (*((volatile unsigned int *)(reg)))
#endif
#define MEM32_WRITE(reg,val) REG32(reg)=val
#define MEM32_READ(reg) REG32(reg)
/*
 * GPIO
 */
#define GPIO_BASE            0xB8003500
#define GPIO_PABCDCNR        (GPIO_BASE + 0x00)
#define GPIO_PABCDDIR        (GPIO_BASE + 0x08)
#define GPIO_PABCDDAT        (GPIO_BASE + 0x0C)

#define GPIO_WATCHDOG_PIN    0x00800000//B7

void WATCHDOG_RESET(void)
{
    unsigned int val;
    
    val = MEM32_READ(GPIO_PABCDDAT);

    if(val & GPIO_WATCHDOG_PIN)
    {
        val &= (~GPIO_WATCHDOG_PIN);
    }
    else
    {
        val |= GPIO_WATCHDOG_PIN;
    }
    MEM32_WRITE(GPIO_PABCDDAT, val);
}
#else
#define IPMUX_GPIO_DIR_ADDR 0xbf002c84
#define IPMUX_GPIO_SET_ADDR 0xbf002c88
#define IPMUX_GPIO_CLR_ADDR 0xbf002c8c

#define WATCHDOG_RESET() \
{\
    *((volatile unsigned int *)(IPMUX_GPIO_DIR_ADDR)) |= (1U << 8);  \
    *((volatile unsigned int *)(IPMUX_GPIO_CLR_ADDR)) |= (1U << 8);  \
    *((volatile unsigned int *)(IPMUX_GPIO_SET_ADDR)) ^= 0x00000100; \
}

//void WATCHDOG_RESET(void)
//{
    //return;
//}
#endif

void gunzip(void *dst, int dstlen, unsigned char *src, uint64_t *lenp)
{
       z_stream s;
       int r, i, flags;

       //_printk("gunzip %x %x %x %x\n",dst,dstlen,src,lenp);

       /* skip header */
       i = 10;
       flags = src[3];
       if (src[2] != Z_DEFLATED || (flags & RESERVED) != 0) {
               puts("bad gzipped data\n");
               puts_exit();
       }
       if ((flags & EXTRA_FIELD) != 0)
               i = 12 + src[10] + (src[11] << 8);
       if ((flags & ORIG_NAME) != 0)
               while (src[i++] != 0)
                       ;
       if ((flags & COMMENT) != 0)
               while (src[i++] != 0)
                       ;
       if ((flags & HEAD_CRC) != 0)
               i += 2;
       if (i >= *lenp) {
               puts("gunzip: ran out of data in header\n");
               puts_exit();
       }
       //WATCHDOG_RESET();
       /* Initialize ourself. */
       s.workspace = zalloc(zlib_inflate_workspacesize());
       r = zlib_inflateInit2(&s, -MAX_WBITS);
       //WATCHDOG_RESET();
       if (r != Z_OK) {
               puts("zlib_inflateInit2 returned "); puthex(r); puts("\n");
               puts_exit();
       }
              
       s.next_in = src + i;
       s.avail_in = *lenp - i;
       s.next_out = dst;
       s.avail_out = dstlen;
       r = zlib_inflate(&s, Z_FINISH);
       //WATCHDOG_RESET();       
       if (r != Z_OK && r != Z_STREAM_END) {
               puts("inflate returned "); puthex(r); puts("\n");
               puts_exit();
       }
       *lenp = s.next_out - (unsigned char *) dst;
       //WATCHDOG_RESET();       
       zlib_inflateEnd(&s);
       //WATCHDOG_RESET();
}

void
puthex(unsigned long val)
{

       unsigned char buf[10];
       int i;	
       
       for (i = 7;  i >= 0;  i--)
       {
               buf[i] = "0123456789ABCDEF"[val & 0x0F];
               val >>= 4;
       }
       buf[8] = '\0';
              
       puts(buf);
}

#define FALSE 0
#define TRUE  1

void _printk(char const *fmt, ...)
{
       va_list ap;

       va_start(ap, fmt);
       _vprintk(putc, fmt, ap);
       va_end(ap);
       return;
}

#define is_digit(c) ((c >= '0') && (c <= '9'))

void
_vprintk(void(*putc)(const char), const char *fmt0, va_list ap)
{
       char c, sign, *cp = 0;
       int left_prec, right_prec, zero_fill, length = 0, pad, pad_on_right;
       char buf[32];
       long val;
       while ((c = *fmt0++))
       {
               if (c == '%')
               {
                       c = *fmt0++;
                       left_prec = right_prec = pad_on_right = 0;
                       if (c == '-')
                       {
                               c = *fmt0++;
                               pad_on_right++;
                       }
                       if (c == '0')
                       {
                               zero_fill = TRUE;
                               c = *fmt0++;
                       } else
                       {
                               zero_fill = FALSE;
                       }
                       while (is_digit(c))
                       {
                               left_prec = (left_prec * 10) + (c - '0');
                               c = *fmt0++;
                       }
                       if (c == '.')
                       {
                               c = *fmt0++;
                               zero_fill++;
                               while (is_digit(c))
                               {
                                       right_prec = (right_prec * 10) + (c - '0');
                                       c = *fmt0++;
                               }
                       } else
                       {
                               right_prec = left_prec;
                       }
                       sign = '\0';
                       switch (c)
                       {
                       case 'd':
                       case 'x':
                       case 'X':
                               val = va_arg(ap, long);
                               switch (c)
                               {
                               case 'd':
                                       if (val < 0)
                                       {
                                               sign = '-';
                                               val = -val;
                                       }
                                       length = _cvt(val, buf, 10, "0123456789");
                                       break;
                               case 'x':
                                       length = _cvt(val, buf, 16, "0123456789abcdef");
                                       break;
                               case 'X':
                                       length = _cvt(val, buf, 16, "0123456789ABCDEF");
                                       break;
                               }
                               cp = buf;
                               break;
                       case 's':
                               cp = va_arg(ap, char *);
                               length = strlen(cp);
                               break;
                       case 'c':
                               c = va_arg(ap, long /*char*/);
                               (*putc)(c);
                               continue;
                       default:
                               (*putc)('?');
                       }
                       pad = left_prec - length;
                       if (sign != '\0')
                       {
                               pad--;
                       }
                       if (zero_fill)
                       {
                               c = '0';
                               if (sign != '\0')
                               {
                                       (*putc)(sign);
                                       sign = '\0';
                               }
                       } else
                       {
                               c = ' ';
                       }
                       if (!pad_on_right)
                       {
                               while (pad-- > 0)
                               {
                                       (*putc)(c);
                               }
                       }
                       if (sign != '\0')
                       {
                               (*putc)(sign);
                       }
                       while (length-- > 0)
                       {
                               (*putc)(c = *cp++);
                               if (c == '\n')
                               {
                                       (*putc)('\r');
                               }
                       }
                       if (pad_on_right)
                       {
                               while (pad-- > 0)
                               {
                                       (*putc)(c);
                               }
                       }
               } else
               {
                       (*putc)(c);
                       if (c == '\n')
                       {
                               (*putc)('\r');
                       }
               }
       }
}

int
_cvt(unsigned long val, char *buf, long radix, char *digits)
{
       char temp[80];
       char *cp = temp;
       int length = 0;
       if (val == 0)
       { /* Special case */
               *cp++ = '0';
       } else
               while (val)
               {
                       *cp++ = digits[val % radix];
                       val /= radix;
               }
       while (cp != temp)
       {
               *buf++ = *--cp;
               length++;
       }
       *buf = '\0';
       return (length);
}

/*

void
_dump_buf_with_offset(unsigned char *p, int s, unsigned char *base)
{
       int i, c;
       if ((unsigned int)s > (unsigned int)p)
       {
               s = (unsigned int)s - (unsigned int)p;
       }
       while (s > 0)
       {
               if (base)
               {
                       _printk("%06X: ", (int)p - (int)base);
               } else
               {
                       _printk("%06X: ", p);
               }
               for (i = 0;  i < 16;  i++)
               {
                       if (i < s)
                       {
                               _printk("%02X", p[i] & 0xFF);
                       } else
                       {
                               _printk("  ");
                       }
                       if ((i % 2) == 1) _printk(" ");
                       if ((i % 8) == 7) _printk(" ");
               }
               _printk(" |");
               for (i = 0;  i < 16;  i++)
               {
                       if (i < s)
                       {
                               c = p[i] & 0xFF;
                               if ((c < 0x20) || (c >= 0x7F)) c = '.';
                       } else
                       {
                               c = ' ';
                       }
                       _printk("%c", c);
               }
               _printk("|\n");
               s -= 16;
               p += 16;
       }
}


void
_dump_buf(unsigned char *p, int s)
{
       _printk("\n");
       _dump_buf_with_offset(p, s, 0);
}

*/

/*
 * Local variables:
 *  c-indent-level: 8
 *  c-basic-offset: 8
 *  tab-width: 8
 * End:
 */
