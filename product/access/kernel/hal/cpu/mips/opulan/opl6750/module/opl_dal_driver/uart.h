/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/

#ifndef __OP_UART_H__
#define __OP_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "vos_types.h"
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"

#define EXAR_232_MODE 0x4
#define EXAR_485_MODE 0x1

typedef enum {
 rs232_mode,
 rs485_mode
}UART_RS_MODE;

typedef enum {
 register_mode,
 fifo_mode
}UART_MODE;

typedef enum {
 rx_normal_mode,
 rx_reset_mode
}UART_RX_MODE;

typedef enum {
 tx_normal_mode,
 tx_reset_mode
}UART_TX_MODE;

typedef enum {
 threshold_16byte,
 threshold_32byte,
 threshold_48byte,
 threshold_60byte
}UART_THRESHOLD_BYTE;

typedef enum {
 word_len_5,
 word_len_6,
 word_len_7,
 word_len_8
}UART_OWRD_LEN;

typedef enum {
 uart_parity =3,
 uart_even,
 uart_force_prity,
 uart_break,
 uart_div
}UART_FUNC;

typedef struct uart
{
	union{
		volatile unsigned int rhr;
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

/* Line Control Register */

#define CHAR_LEN_5	0x00
#define CHAR_LEN_6	0x01
#define CHAR_LEN_7	0x02
#define CHAR_LEN_8	0x03
#define CHAR_LEN_MASK 0xFC
#define LCR_STB		0x04
#define ONE_STOP	0x00
#define TWO_STOP	0x04
#define STOP_MASK   0xFB
#define ONE_HALF_STOP   0x40
#define LCR_PEN		0x08
#define PARITY_NONE	0x00
#define PARITY_ODD	0x08  
#define PARITY_EVEN	0x18 
#define FORCE_PARITY_1 0x28
#define FORCE_PARITY_0 0x38
#define PARITY_MASK 0xC7

#define LCR_EPS		0x10
#define LCR_SP		0x20
#define LCR_SBRK	0x40
#define LCR_DLAB	0x80
#define DLAB		LCR_DLAB


/* Line Status Register */

#define LSR_DR		0x01
#define RxCHAR_AVAIL	LSR_DR
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_FERR	0x80

/* Interrupt Status Register */

#define ISR_IP		0x01
#define ISR_ID		0x0e
#define ISR_RLS		0x06
#define Rx_INT		ISR_RLS
#define ISR_RDA		0x04
#define RxFIFO_INT	ISR_RDA
#define ISR_THRE	0x02
#define TxFIFO_INT	ISR_THRE
#define ISR_MSTAT	0x00
#define ISR_TIMEOUT	0x0c

/* Interrupt Enable Register */

#define IER_ERDAI	0x01
#define RxFIFO_BIT	IER_ERDAI
#define IER_ETHREI	0x02
#define TxFIFO_BIT	IER_ETHREI
#define IER_ELSI	0x04
#define Rx_BIT		IER_ELSI
#define IER_RTS		0x40
#define IER_CTS		0x80

/* Modem Control Register */

#define MCR_DTR		0x01
#define DTR		MCR_DTR
#define MCR_RTS		0x02
#define MCR_OUT1	0x04
#define MCR_OUT2	0x08
#define MCR_LOOP	0x10

/* Register offsets from base address */

#define RHR	0x00
#define THR	0x00
#define DLL	0x00
#define IER	0x01
#define DLM	0x01
#define IIR	0x02
#define FCR	0x02
#define LCR	0x03
#define MCR	0x04
#define LSR	0x05
#define MSR	0x06
#define SCR	0x07

/* Modem Status Register */

#define MSR_DCTS	0x01
#define MSR_DDSR	0x02
#define MSR_TERI	0x04
#define MSR_DDCD	0x08
#define MSR_CTS		0x10
#define MSR_DSR		0x20
#define MSR_RI		0x40
#define MSR_DCD		0x80

/* FIFO Control Register */

#define FCR_EN		0x01
#define FIFO_ENABLE	FCR_EN
#define FCR_RXCLR	0x02
#define RxCLEAR		FCR_RXCLR
#define FCR_TXCLR	0x04
#define TxCLEAR		FCR_TXCLR
#define FCR_DMA		0x08
#define FCR_RXTRIG_L	0x40
#define FCR_RXTRIG_H	0x80
#define RX_TRIG_CLR	0x3f
#define RX_TRIG_8	0x00
#define RX_TRIG_16	0x40
#define RX_TRIG_32	0x80
#define RX_TRIG_48	0xc0
#define TX_TRIG_CLR	0xcf
#define TX_TRIG_8	0x10
#define TX_TRIG_16	0x00
#define TX_TRIG_32	0x20
#define TX_TRIG_48	0x30

#if defined(__ONU_SDK__) 
#define FPCLK           150000000
#elif defined(__FPGA_ONU_SDK__)
#define FPCLK           125000000
#elif defined(__IPMUX_SDK__)
#define FPCLK           150000000
#else
#define FPCLK           125000000
#endif

typedef struct CommPara
{
	char datalen;
	char stopbit;
	char parity;
} IPMUX_UART_PARA_t;

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

typedef enum{
    NO_PARITY = 0,
    ODD_PARITY,
    EVEN_PARITY,
    FORCE_1_PARITY,
    FORCE_0_PARITY
}PARITY_TYPE;

enum WORD_LENGTH{
    FIVE_CHAR_BIT = 0,
    SIX_CHAR_BIT,
    SEVEN_CHAR_BIT,
    EIGHT_CHAR_BIT
};

typedef enum{
    ONE_STOP_BIT = 0,
    TWO_STOP_BIT
}STOP_BIT_LENGTH;

#define UART_BPS_300    300
#define UART_BPS_600    600
#define UART_BPS_1200   1200
#define UART_BPS_2400   2400
#define UART_BPS_4800   4800
#define UART_BPS_9600   9600
#define UART_BPS_7200   7200
#define UART_BPS_19200  19200
#define UART_BPS_38400   38400
#define UART_BPS_115200  115200

#define CONFIG_BAUDRATE UART_BPS_115200

OPL_STATUS uartRxDataGet(UINT8 uartId, UINT32 *pdata);
OPL_STATUS uartTxDataSet(UINT8 uartId, UINT32 data);
OPL_STATUS uartIntEnable(UINT8 uartId, UINT32 bit);
OPL_STATUS uartIntEnableGet(UINT8 uartId, UINT32 *enable);
OPL_STATUS uartIntStatusGet(UINT8 uartId, UINT32 *pstatus);
OPL_STATUS uartModeSet(UINT8 uartId, UART_MODE mode);
OPL_STATUS uartRxResetModeSet(UINT8 uartId, UART_RX_MODE mode);
OPL_STATUS uartTxResetModeSet(UINT8 uartId, UART_TX_MODE mode);
OPL_STATUS uartTxThresholdSet(UINT8 uartId, UART_THRESHOLD_BYTE byte);
OPL_STATUS uartRxThresholdSet(UINT8 uartId, UART_THRESHOLD_BYTE byte);
OPL_STATUS uartWordLenSet(UINT8 uartId, UART_OWRD_LEN len);
OPL_STATUS uartStopBitSet(UINT8 uartId, UINT32 val);
OPL_STATUS uartFuncEnable(UINT8 uartId, UART_FUNC  func_bit);
OPL_STATUS uartFuncDisable(UINT8 uartId, UART_FUNC  func_bit);
OPL_STATUS uartRtsValSet(UINT8 uartId, UINT32 val);
OPL_STATUS uartLoopbackEnable(UINT8 uartId);
OPL_STATUS uartLoopbackDisable(UINT8 uartId);
OPL_STATUS uartLineStatusGet(UINT8 uartId, UINT32 *pstatus);
OPL_STATUS uartLineStatusRxReadyGet(UINT8 uartId, UINT32 *pstatus);
OPL_STATUS uartLineStatusTxEmptyGet(UINT8 uartId, UINT32 *pstatus);
OPL_STATUS uartModemStatusGet(UINT8 uartId, UINT32 *pstatus);
OPL_STATUS uartTmpDataSet(UINT8 uartId, UINT32 data);
OPL_STATUS uartTmpDataGet(UINT8 uartId ,UINT32 *Pdata);
OPL_STATUS uartDiv_0Set(UINT8 uartId, UINT32 div);
OPL_STATUS uartDiv_1Set(UINT8 uartId, UINT32 div);
OPL_STATUS uartRtsFlowCtlEnable(UINT8 uartId);
OPL_STATUS uartRtsFlowCtldisable(UINT8 uartId);
OPL_STATUS uartCtsFlowCtlEnable(UINT8 uartId);
OPL_STATUS uartCtsFlowCtldisable(UINT8 uartId);
void uart1Test(void);
/*begin added by liaohongjun 2012/12/25 of EPN204QID0019*/
OPL_STATUS Uart1Init(void);
/*end added by liaohongjun 2012/12/25 of EPN204QID0019*/


/******************************************************************************
*
* serial_setbrg - set baud rate
*
* This routine set default baud rate in DLL and DLM register. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setbrg(UINT8 uartId, UINT32 baud);
OPL_STATUS serial_getbrg(UINT8 uartId, UINT32 *baud);

/******************************************************************************
*
* serial_init - initialize a channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/
int serial_init(void);

/******************************************************************************
*
* serial_setparity - set parity
*
* This routine set parity in LCR register. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setparity (UINT8 uartId, PARITY_TYPE parity);

/******************************************************************************
*
* serial_setbrg - set word length
*
* This routine set word length. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setwordlength(UINT8 uartId, UART_OWRD_LEN len);

/******************************************************************************
*
* serial_setbrg - set stop bit length
*
* This routine set stop bit length. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setstopbitlength(UINT8 uartId, STOP_BIT_LENGTH length);

/******************************************************************************
*
* serial_putc - transmitting a single chacter 
*
* This routine poll lsr THRE bit.  If it is empty, then put chacter to be 
* transmited into THR register.  It also translate newline symbol '\n' into '\r'.
*
* INPUT:  c -- character to be transmitted.
* RETURNS: N/A
*/
void serial_putc(UINT8 uartId, const char c);

/******************************************************************************
*
* serial_nowait_putc - transmitting a single chacter 
*
* This routine put chacter directly into  THR register.  It 
* This routine put chacter directly into  THR register.  It 
* also translate newline symbol '\n' into '\r'.
*
* RETURNS: N/A
*/
void serial_nowait_putc(const char c);

/******************************************************************************
*
* serial_puts - initialize a channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/
void serial_puts(UINT8 uartId, const char *s);

/******************************************************************************
*
* serial_getc - retrieve one character from buffer 
*
* This routine check the rx buffer staus, if it is not empty, retrieve one 
* character from receiving buffer and return it to calling routine.
*
* input: none.
*
* RETURNS:  receiving character.
*/
int serial_getc(UINT8 uartId);

/******************************************************************************
*
* serial_gets - retrieve one character from buffer 
*
* This routine check the rx buffer staus, if it is not empty, retrieve one 
* character from receiving buffer and return it to calling routine.
*
* input: none.
*
* RETURNS:  receiving character.
*/
int serial_gets(UINT8 port, UINT8 *data, UINT16 maxLen, UINT16 byteDly, UINT16 frmDly);

/******************************************************************************
*
* serial_nowait_getc - retrieve one character from fifo 
*
* This routine read one character from receiving buffer 
* 
* input: none.
*
* RETURNS:  receiving character.
*/
unsigned char serial_nowait_getc (void);

/******************************************************************************
*
* serial_tstc - check receiving status
*
* This routine check line status bit to see whether the receive buffer is ready.
*
* RETURNS: 
* 	   1:  receive buffer has chacter.
* 	   0:  receive buffer is empty.
*/
int serial_tstc(UINT8 uartId);

/******************************************************************************
*
* set_tx_rx_trig_level - set transmiter and receiver trigger level
*
* This routine set fifo transmiter and receiver trigger level
*
* INPUT:   l: the rx trigger level.  It has four mode.  The value 0,1,2,3 
* 		designate 16, 32, 48, 60 bytes trigger point. 
* 	   2: the tx trigger level.  It has four mode.  The value 0,1,2,3 
* 		designate 16, 32, 48, 60 bytes trigger point. 
*  
* RETURNS: N/A
*/
void set_tx_rx_trig_level(unsigned char t,unsigned char r);


/******************************************************************************
*
* get_int_stat - get interrupt status
*
* This routine get interrupt status
*
* INPUT:   
*  
* RETURNS: N/A
*/

unsigned char get_int_stat(void);

/******************************************************************************
*
* serial_set_loopback	- set loopback mode 
*
* This routine set uart to the loopback mode 
*
* RETURNS: N/A
*/
void serial_set_loopback(void);

/******************************************************************************
*
* serial_disable_loopback	- disable loopback mode 
*
* This routine disable loopback mode 
*
* RETURNS: N/A
*/
void serial_disable_loopback(void);

/******************************************************************************
*
* serial_enable_fifo	- enable fifo  mode 
*
* This routine enable fifo mode 
*
* RETURNS: N/A
*/
void serial_enable_fifo(void);

/******************************************************************************
*
* serial_disable_fifo	- disable fifo mode 
*
* This routine disable fifo mode 
*
* RETURNS: N/A
*/
void serial_disable_fifo(void);

/******************************************************************************
*
* serial_getlcr - get line control register
*
* This routine get line control register value
*
* INPUT:   
*  
* RETURNS: N/A
*/
OPL_STATUS serial_getlcr(UINT8 comm, UINT32 *lcr);

#ifdef __cplusplus
	}
#endif

#endif // __OP_UART_H__

