/*
 * NS16550 Serial Port
 */

/*
 * Figure out which file will have the definitons of COMx
 */

/* Some machines have their uart registers 16 bytes apart.  Most don't.
 * TODO: Make this work like drivers/char/serial does - Tom */
#if !defined(UART_REG_PAD)
#define UART_REG_PAD(x)
#endif

struct NS16550
 {
  unsigned char rbr;  /* 0 */
  UART_REG_PAD(rbr)
  unsigned char ier;  /* 1 */
  UART_REG_PAD(ier)
  unsigned char fcr;  /* 2 */
  UART_REG_PAD(fcr)
  unsigned char lcr;  /* 3 */
  UART_REG_PAD(lcr)
  unsigned char mcr;  /* 4 */
  UART_REG_PAD(mcr)
  unsigned char lsr;  /* 5 */
  UART_REG_PAD(lsr)
  unsigned char msr;  /* 6 */
  UART_REG_PAD(msr)
  unsigned char scr;  /* 7 */
 };

#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier

#define LSR_DR   0x01  /* Data ready */
#define LSR_OE   0x02  /* Overrun */
#define LSR_PE   0x04  /* Parity error */
#define LSR_FE   0x08  /* Framing error */
#define LSR_BI   0x10  /* Break */
#define LSR_THRE 0x20  /* Xmit holding register empty */
#define LSR_TEMT 0x40  /* Xmitter empty */
#define LSR_ERR  0x80  /* Error */
