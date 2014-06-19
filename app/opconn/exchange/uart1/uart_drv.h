/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  uart_drv.h
*
* DESCRIPTION: 
*	uart driver mod for linux 2.4
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/uart1/uart_drv.h#1 $
* $Log:$
*
*
**************************************************************************/
/* Line Control Register */
#ifndef __UART_DRV_H__
#define __UART_DRV_H__

#define CHAR_LEN_5	    0x00
#define CHAR_LEN_6	    0x01
#define CHAR_LEN_7	    0x02
#define CHAR_LEN_8	    0x03
#define LCR_STB		    0x04
#define ONE_STOP	    0x00
#define TWO_STOP	    0x04
#define ONE_HALF_STOP   0x40
#define LCR_PEN		    0x08
#define PARITY_NONE	    0x00
#define PARITY_ODD	    0x08  
#define PARITY_EVEN	    0x18 
#define FORCE_PARITY_1  0x28
#define FORCE_PARITY_0  0x38

#define LCR_EPS		    0x10
#define LCR_SP		    0x20
#define LCR_SBRK	    0x40
#define LCR_DLAB	    0x80
#define DLAB		    LCR_DLAB


/* Line Status Register */

#define LSR_DR		    0x01
#define RxCHAR_AVAIL	LSR_DR
#define LSR_OE		    0x02
#define LSR_PE		    0x04
#define LSR_FE		    0x08
#define LSR_BI		    0x10
#define LSR_THRE	    0x20
#define LSR_TEMT	    0x40
#define LSR_FERR	    0x80

/* Interrupt Status Register */

#define ISR_IP		    0x01
#define ISR_ID		    0x0e
#define ISR_RLS		    0x06
#define Rx_INT		    ISR_RLS
#define ISR_RDA		    0x04
#define RxFIFO_INT	    ISR_RDA
#define ISR_THRE	    0x02
#define TxFIFO_INT	    ISR_THRE
#define ISR_MSTAT	    0x00
#define ISR_TIMEOUT	    0x0c

/* Interrupt Enable Register */

#define IER_ERDAI	    0x01
#define RxFIFO_BIT	    IER_ERDAI
#define IER_ETHREI	    0x02
#define TxFIFO_BIT	    IER_ETHREI
#define IER_ELSI	    0x04
#define Rx_BIT		    IER_ELSI
#define IER_RTS		    0x40
#define IER_CTS		    0x80

/* Modem Control Register */

#define MCR_DTR		    0x01
#define DTR		        MCR_DTR
#define MCR_RTS		    0x02
#define MCR_OUT1	    0x04
#define MCR_OUT2	    0x08
#define MCR_LOOP	    0x10

/* Register offsets from base address */

#define RHR	            0x00
#define THR	            0x00
#define DLL	            0x00
#define IER	            0x01
#define DLM	            0x01
#define IIR	            0x02
#define FCR	            0x02
#define LCR	            0x03
#define MCR	            0x04
#define LSR	            0x05
#define MSR	            0x06
#define SCR	            0x07

/* Modem Status Register */

#define MSR_DCTS	    0x01
#define MSR_DDSR	    0x02
#define MSR_TERI	    0x04
#define MSR_DDCD	    0x08
#define MSR_CTS		    0x10
#define MSR_DSR		    0x20
#define MSR_RI		    0x40
#define MSR_DCD		    0x80

/* FIFO Control Register */

#define FCR_EN		    0x01
#define FIFO_ENABLE	    FCR_EN
#define FCR_RXCLR	    0x02
#define RxCLEAR		    FCR_RXCLR
#define FCR_TXCLR	    0x04
#define TxCLEAR		    FCR_TXCLR
#define FCR_DMA		    0x08
#define FCR_RXTRIG_L	0x40
#define FCR_RXTRIG_H	0x80
#define RX_TRIG_CLR	    0x3f
#define RX_TRIG_8	    0x00
#define RX_TRIG_16	    0x40
#define RX_TRIG_32	    0x80
#define RX_TRIG_48	    0xc0
#define TX_TRIG_CLR	    0xcf
#define TX_TRIG_8	    0x10
#define TX_TRIG_16	    0x00
#define TX_TRIG_32	    0x20
#define TX_TRIG_48	    0x30


#define LINE_STATUS_INT         0X06
#define RX_READY_INT            0X04
#define RX_TIMEOUT_INT          0X0C
#define TX_HOLD_REG_EMPTY_INT   0X02
#define UNUSED_INT              0X10
#define CHANGE_OF_CTS_STS_INT   0X20

//#define __ONU_SDK__             1
#define __FPGA_ONU_SDK__        1
//#define __IPMUX_SDK__           1

#define DEBUG 0	
#undef DEBUG

#define	UART1_MAJOR 	210   //mknod /dev/ufile  c 210  0

#if defined(__ONU_SDK__) 
#define UFILE_NAME      "onuUfile"
#define FPCLK           150000000
#define UART1_ADDR	    0xBF003c00
#elif defined(__FPGA_ONU_SDK__)
#define UFILE_NAME      "fpgaOnuUfile"
//#define FPCLK           62500000
#define FPCLK           125000000
#define UART1_ADDR	    0xBF001c00
#elif defined(__IPMUX_SDK__)
#define FPCLK           150000000
#define UART1_ADDR	    0xBF003c00
#define UFILE_NAME      "ipmuxUfile"    
#endif

#define FIFO_LEN        60

#define OFFSET_RHR_THR_DLL	0x00
#define OFFSET_IER_DLM		0x04
#define OFFSET_FCR_ISR		0x08
#define OFFSET_LCR		    0x0c
#define OFFSET_MCR		    0x10
#define OFFSET_LSR		    0x14
#define OFFSET_MSR		    0x18
#define OFFSET_SPR		    0x1c
#define OFFSET_EFR		    0x20

#define UART_MSG_RX_DEBUG       1


#define OPL_OK              0 
#define OPL_TRUE			1
#define OPL_FALSE			0

#define OPL_ENABLE			1
#define OPL_DISABLE			0

#define RX_BUFF_LEN         (2048*2)
#define TX_BUFF_LEN         (2048*2)


#define UART1_COMMAND_CLEAR_RXBUFFER 0x10000
#define UART1_COMMAND_CLEAR_TXBUFFER 0x10001
#define ENABLE_UART1_INTERRUPT       0x10002
#define DISABLE_UART1_INTERRUPT      0x10003
#define DUMP_UART1_TXBUFFER          0x10004
#define DUMP_UART1_RXBUFFER          0x10005

typedef struct CommPara
{
	char datalen;
	char stopbit;
	char parity;
} OPCONN_UART_PARA_t;

enum COMMAND_REQ{
	BAUD_RATE=0,
	COMM_PARA,
	LOOPBACK_ENABLE,
	LOOPBACK_DISABLE,
	BLOCK_ENABLE,
	BLOCK_DISABLE,
	DMA_ENABLE,
	DMA_DISABLE,
	RX_TRIG_SET,
	TX_TRIG_SET
};

typedef struct TX_DATA_BUFF_s{
    unsigned int len;
    unsigned char buff[TX_BUFF_LEN];
}TX_DATA_BUFF_t;

typedef struct uart
{
	union{
		volatile unsigned int rbr;
		volatile unsigned int thr;
		volatile unsigned int dll;
	} w0;

	union{
		volatile unsigned int ier;
		volatile unsigned int dlm;
	} w1;

	union{
		volatile unsigned int isr;
		volatile unsigned int fcr;
	} w2;

	volatile unsigned int lcr;

	volatile unsigned int mcr;

	volatile unsigned int lsr;

	volatile unsigned int msr;

	volatile unsigned int scr;

	volatile unsigned int efr;
} IPMUX_REG_UART_t;


int serial_init (void);
void serial_setbrg (void);
void serial_putc (const char c);
void serial_nowait_putc (const char c);
void serial_puts (const char *s);
int serial_getc (void);
unsigned char serial_nowait_getc (void);
int serial_tstc (void);
void set_tx_rx_trig_level(unsigned char t,unsigned char r);
unsigned char get_int_stat(void);
void serial_set_loopback(void);
void serial_disable_loopback(void);
void serial_enable_fifo(void);
void serial_disable_fifo(void);


static int ufile_open ( struct inode * , struct file * ); 
static int ufile_release ( struct inode * , struct file * ); 
static int ufile_ioctl ( struct inode * , struct file * , unsigned int , unsigned long);
static size_t ufile_read(struct file *file, char *buf,
			size_t count, loff_t * ppos);
static size_t ufile_write(struct file *file, char *buf,
			size_t count, loff_t * ppos);

#ifdef UART_DRV_DBG
#define opl_sdk_printf(x)   printk x
#define opl_dump_data(p,len,width) {unsigned int index;unsigned char *x = p;for(index = 0; index < len; index++)       \
                                             {                                                          \    
                                                if((index != 0) && (index%width == 0))                  \
                                                {                                                       \
                                                    opl_sdk_printf(("\n%02x ",x[index]));               \
                                                }else                                                   \        
                                                {                                                       \
                                                    opl_sdk_printf(("%02x ",x[index]));                 \      
                                                }                                                       \          
                                             }  opl_sdk_printf(("\n"));  opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));                               \
                                            }
#else
#define opl_sdk_printf(x)   
#define opl_dump_data(p,len,width)
#endif 

#endif

