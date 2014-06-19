/**
*
*  COPYRIGHT (C) 2003-2030 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products.
* 
* @file uart.c Driver source file for UART module
*
* DESCRIPTION: 
*
* Date Created:
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
*
*/
/**
 * Source file changeLog:
 *   - 2010-01-20:add initial code according to ONU2 register document v1.0
 */
/**
 * @defgroup UART UART module
 * @{
 */
/***** INCLUDE FILES *****/
#include "opconn_lib.h"
#include "hw_interface.h"
#include "uart.h"
#include "opconn_hw_reg.h"

/***** LOCAL DEFINES and ENUM *****/


/***** LOCAL STRUCTURES and UNIONS *****/

/***** LOCAL TYPEDEFS ****/


/***** LOCAL (PRIVATE) PROTOTYPES *****/

/***** DATA ALLOCATION *****/

/***** PUBLIC FUNCTIONS ****/

/**
*@par Description
* This function get uart rcv data.
*
* @param [in] uart_num
*		 number of uart
* @param [in] pdata
*	       pointer of recv data.
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartRxDataGet(UINT8 uartId, UINT32 *pdata)
{
    OPL_STATUS retVal;


    if(OPL_NULL == pdata)
	{
		return OPL_ERROR;
	}
    
    retVal = oplRegFieldRead(REG_RECV_HOLD(uartId), 0, 8, pdata);

    return retVal;
    
}

/**
*@par Description
* This function set uart transmit data.
*
* @param [in] uart_num
*		 number of uart
* @param [in] data
*	       data to be tansmited
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartTxDataSet(UINT8 uartId, UINT32 data)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_TRANS_HOLD(uartId), 0, 8, data);
    return retVal;
    
}

/**
*@par Description
* This function enable corresponed uart interrupt.
*
* @param [in] uart_num
*		 number of uart
* @param [in] bit
*	       correspon bit of the interrupt
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartIntEnable(UINT8 uartId, UINT32 bit)
{
    OPL_STATUS retVal ;

	switch (bit){
		case 0:
			OPL_PRINTF("received data availabe interrupt enable: bit %d \n",bit);
			break;
	    case 1:
		    OPL_PRINTF("transmit data empty interrupt enable: bit %d \n",bit);
		    break;
		case 2:
			OPL_PRINTF("receive line status interrupt enable: bit %d \n",bit);
			break;
		case 6:
			OPL_PRINTF("RTS interrupt enable: bit %d \n",bit);
			break;
		case 7:
			OPL_PRINTF("CTS interrupt enable: bit %d \n",bit);
			break;
		default:
			OPL_PRINTF("valid interrupt bit %d \n",bit);
            return OPL_ERROR;

		}
    retVal = oplRegFieldWrite(REG_INT_ENABLE(uartId), bit, 1, 1);
    return retVal;
    
}

/**
*@par Description
* This function get uart interrupt enable status.
*
* @param [in] uart_num
*		 number of uart
* @param [out] enable
*	       interrupt enable status
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartIntEnableGet(UINT8 uartId, UINT32 *enable)
{
    OPL_STATUS retVal ;

    if (OPL_NULL == enable)
    {
        return OPL_ERROR;
    }
    
    retVal = oplRegFieldRead(REG_INT_ENABLE(uartId), 0, 8, enable);    
    return retVal;
    
}

/**
*@par Description
* This function get uart interrupt status.
*
* @param [in] uart_num
*		 number of uart
* @param [in] pstatus
*	       pointer of interrupt status.
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartIntStatusGet(UINT8 uartId, UINT32 *pstatus)
{
    OPL_STATUS retVal;

    if (OPL_NULL == pstatus)
    {
        return OPL_ERROR;
    }
    
    retVal = oplRegFieldRead(REG_INT_STATUS(uartId), 0, 8, pstatus);

    return retVal;
    
}

/**
*@par Description
* This function set uart mode.
*
* @param [in] uart_num
*		 number of uart
* @param [in] mode
*	       mode of uart
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartModeSet(UINT8 uartId, UART_MODE mode)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_FIFO_CTL(uartId), 0, 1, (UINT32)mode);
    return retVal;
    
}

/**
*@par Description
* This function set uart rx reset mode.
*
* @param [in] uart_num
*		 number of uart
* @param [in] mode
*	       mode of uart rx reset
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartRxResetModeSet(UINT8 uartId, UART_RX_MODE mode)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_FIFO_CTL(uartId), 1, 1, (UINT32)mode);
    return retVal;
    
}

/**
*@par Description
* This function set uart tx reset mode.
*
* @param [in] uart_num
*		 number of uart
* @param [in] mode
*	       mode of uart tx reset
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartTxResetModeSet(UINT8 uartId, UART_TX_MODE mode)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_FIFO_CTL(uartId), 2, 1, (UINT32)mode);
    return retVal;
    
}

/**
*@par Description
* This function set uart tx threshold.
*
* @param [in] uart_num
*		 number of uart
* @param [in] mode
*	       uart tx threshold size
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartTxThresholdSet(UINT8 uartId, UART_THRESHOLD_BYTE byte)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_FIFO_CTL(uartId), 4, 2, (UINT32)byte);
    return retVal;
    
}

/**
*@par Description
* This function set uart rx threshold.
*
* @param [in] uart_num
*		 number of uart
* @param [in] mode
*	       uart rx threshold size
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartRxThresholdSet(UINT8 uartId, UART_THRESHOLD_BYTE byte)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_FIFO_CTL(uartId), 6, 2, (UINT32)byte);
    return retVal;
    
}

/**
*@par Description
* This function set uart word length.
*
* @param [in] uart_num
*		 number of uart
* @param [in] mode
*	       uart rx threshold size
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartWordLenSet(UINT8 uartId, UART_OWRD_LEN len)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_LINE_CTL(uartId), 0, 2, (UINT32)len);
    return retVal;
    
}

/**
*@par Description
* This function set uart transmit data.
*
* @param [in] uart_num
*		 number of uart
* @param [in] val
*	       value to be set
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartStopBitSet(UINT8 uartId, UINT32 val)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_LINE_CTL(uartId), 2, 1, val);
    return retVal;
    
}

/**
*@par Description
* This function enable corresponed uart parity, break and system clock divide factor.
*
* @param [in] uart_num
*		 number of uart
* @param [in] func_bit
*	       correspon function type
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartFuncEnable(UINT8 uartId, UART_FUNC  func_bit)
{
    OPL_STATUS retVal ;
	
    retVal = oplRegFieldWrite(REG_LINE_CTL(uartId), (UINT16)func_bit, 1, 1);
    
    return retVal;
}

/**
*@par Description
* This function enable corresponed uart parity, break and system clock divide factor.
*
* @param [in] uart_num
*		 number of uart
* @param [in] func_bit
*	       correspon function type
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartFuncDisable(UINT8 uartId, UART_FUNC  func_bit)
{
    OPL_STATUS retVal ;
	
    retVal = oplRegFieldWrite(REG_LINE_CTL(uartId), (UINT16)func_bit, 1, 0);

    return retVal;
}

/**
*@par Description
* This function set uart rts value.
*
* @param [in] uart_num
*		 number of uart
* @param [in] val
*	       value to be set
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartRtsValSet(UINT8 uartId, UINT32 val)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_MODEM_CTL(uartId), 1, 1, val);
    return retVal;
    
}

/**
*@par Description
* This function enable uart loopback.
*
* @param [in] uart_num
*		 number of uart
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartLoopbackEnable(UINT8 uartId)
{
    OPL_STATUS retVal ;

    
    retVal = oplRegFieldWrite(REG_MODEM_CTL(uartId), 4, 1, 1);
    return retVal;
    
}

/**
*@par Description
* This function uart disable loopback .
*
* @param [in] uart_num
*		 number of uart
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartLoopbackDisable(UINT8 uartId)
{
    OPL_STATUS retVal ;

    
    retVal = oplRegFieldWrite(REG_MODEM_CTL(uartId), 4, 1, 0);
    return retVal;
    
}

/**
*@par Description
* This function get uart line status.
*
* @param [in] uart_num
*		 number of uart
* @param [in] pstatus
*	       pointer of line status.
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartLineStatusGet(UINT8 uartId, UINT32 *pstatus)
{
    OPL_STATUS retVal;

    if(OPL_NULL == pstatus)
	{
		return OPL_ERROR;
	}
    
    retVal = oplRegFieldRead(REG_LINE_STATUS(uartId), 0, 8, pstatus);

    return retVal;
    
}

/**
*@par Description
* This function get uart line status rx ready.
*
* @param [in] uart_num
*		 number of uart
* @param [out] pstatus
*	       pointer of line status.
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartLineStatusRxReadyGet(UINT8 uartId, UINT32 *pstatus)
{
    OPL_STATUS retVal;

    if (OPL_NULL == pstatus)
    {
        return OPL_ERROR;
    }
    
    retVal = oplRegFieldRead(REG_LINE_STATUS(uartId), 0, 1, pstatus);

    return retVal;
}

/**
*@par Description
* This function get uart line status tx empty.
*
* @param [in] uart_num
*		 number of uart
* @param [out] pstatus
*	       pointer of line status.
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartLineStatusTxEmptyGet(UINT8 uartId, UINT32 *pstatus)
{
    OPL_STATUS retVal;

    if (OPL_NULL == pstatus)
    {
        return OPL_ERROR;
    }
    
    retVal = oplRegFieldRead(REG_LINE_STATUS(uartId), 5, 1, pstatus);

    return retVal;
}

/**
*@par Description
* This function get uart modem status.
*
* @param [in] uart_num
*		 number of uart
* @param [in] pstatus
*	       pointer of modem status.
*
 * @retval OPL_ERROR  This operation failed.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartModemStatusGet(UINT8 uartId, UINT32 *pstatus)
{
    OPL_STATUS retVal;

    if(OPL_NULL == pstatus)
	{
		return OPL_ERROR;
	}
    
    retVal = oplRegFieldRead(REG_MODEM_STATUS(uartId), 0, 5, pstatus);

    return retVal;
    
}

/**
*@par Description
* This function set uart tmp data.
*
* @param [in] uart_num
*		 number of uart
* @param [in] data
*	       tmp_data to be set
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartTmpDataSet(UINT8 uartId, UINT32 data)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_SCRATCHPAD(uartId), 0, 8, data);
    return retVal;
    
}

/**
*@par Description
* This function get uart tmp data.
*
* @param [in] uart_num
*		 number of uart
* @param [in] data
*	       pointer of tmp_data 
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartTmpDataGet(UINT8 uartId, UINT32 *Pdata)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldRead(REG_SCRATCHPAD(uartId), 0, 8, Pdata);
    return retVal;
    
}

/**
*@par Description
* This function set uart divider 0.
*
* @param [in] uart_num
*		 number of uart
* @param [in] div
*	       least 8 bit of divider
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartDiv_0Set(UINT8 uartId, UINT32 div)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_DIV_0(uartId), 0, 8, div);
    return retVal;
}

/**
*@par Description
* This function set uart divider 1.
*
* @param [in] uart_num
*		 number of uart
* @param [in] div
*	       high 8 bit of divider
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartDiv_1Set(UINT8 uartId, UINT32 div)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_DIV_1(uartId), 0, 8, div);
    return retVal;
    
}

/**
*@par Description
* This function enable uart rts flow ctl .
*
* @param [in] uart_num
*		 number of uart
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartRtsFlowCtlEnable(UINT8 uartId)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_ENHANC_FEATURE(uartId), 6, 1, 1);
    return retVal;
    
}

/**
*@par Description
* This function disable uart rts flow ctl .
*
* @param [in] uart_num
*		 number of uart
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartRtsFlowCtldisable(UINT8 uartId)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_ENHANC_FEATURE(uartId), 6, 1, 0);
    return retVal;
    
}

/**
*@par Description
* This function enable uart cts flow ctl .
*
* @param [in] uart_num
*		 number of uart
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartCtsFlowCtlEnable(UINT8 uartId)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_ENHANC_FEATURE(uartId), 7, 1, 1);
    return retVal;
    
}

/**
*@par Description
* This function disable uart cts flow ctl .
*
* @param [in] uart_num
*		 number of uart
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS uartCtsFlowCtldisable(UINT8 uartId)
{
    OPL_STATUS retVal ;
    
    retVal = oplRegFieldWrite(REG_ENHANC_FEATURE(uartId), 7, 1, 0);
    return retVal;
    
}

/******************************************************************************
*
* serial_setbrg - set baud rate
*
* This routine set default baud rate in DLL and DLM register. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setbrg(UINT8 uartId, UINT32 baud)
{
    OPL_STATUS retVal ;

    UINT32 fdiv;
#if 1 /* yxzhao added, 2010-10-10, 13:17:12 */
    retVal = uartFuncEnable(uartId, uart_div);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }

    printk("%s[%d] FPCLK=%d\r\n", __FUNCTION__, __LINE__, FPCLK);  /* yxzhao added for debug. */
    fdiv = (FPCLK /16) / baud;
    
    retVal = uartDiv_0Set(uartId, fdiv&0xFF);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }

    retVal = uartDiv_1Set(uartId, fdiv>>8);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }

    retVal = uartFuncDisable(uartId, uart_div);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }
#else
    printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
    oplUart1Set(FPCLK, baud);
    printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
#endif
    
    return OPL_OK;
}

/******************************************************************************
*
* serial_setbrg - get baud rate
*
* This routine get baud rate in DLL and DLM register. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_getbrg(UINT8 uartId, UINT32 *baud)
{
    OPL_STATUS retVal ;
    UINT32 fdiv, data;

    retVal = uartFuncEnable(uartId, uart_div);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }
    
    retVal = oplRegFieldRead(REG_DIV_0(uartId), 0, 8, &data);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }

    fdiv = data;

    retVal = oplRegFieldRead(REG_DIV_1(uartId), 0, 8, &data);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }

    retVal = uartFuncDisable(uartId, uart_div);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }

    fdiv |= data<<8;

    printk("%s[%d] FPCLK=%d, fdiv=%d\r\n", __FUNCTION__, __LINE__, FPCLK, fdiv);  /* yxzhao added for debug. */
    *baud = (FPCLK /16) / fdiv;

    return OPL_OK;
}

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
int serial_init(void)
{
   
	volatile struct uart *pUart = (volatile struct uart*)((UINT32)ONU_REGBASE + UART1_BA_B*4);

   
	/* disable UART interrupts for now */
  
	pUart->w1.ier = 0; 	
  
	/*set the baudrate*/
	serial_setbrg(1, CONFIG_BAUDRATE);
	
	/*set the char len, partity mode, stop bit*/
   
    /*begin modified by liaohongjun 2012/12/25 of EPN204QID0019*/
	//pUart->lcr = CHAR_LEN_8 | PARITY_EVEN | ONE_STOP;
    pUart->lcr = CHAR_LEN_8 | PARITY_NONE | ONE_STOP;
    /*end modified by liaohongjun 2012/12/25 of EPN204QID0019*/
	return 0;
}

/******************************************************************************
*
* serial_setparity - set parity
*
* This routine set parity in LCR register. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setparity(UINT8 uartId, PARITY_TYPE parity)
{
    OPL_STATUS retVal;
    
    switch(parity)
    {
        case NO_PARITY:
            retVal = uartFuncDisable(uartId, uart_parity);
            break;
        case ODD_PARITY:
            retVal = uartFuncEnable(uartId, uart_parity);
            retVal |= uartFuncDisable(uartId, uart_even);
            retVal |= uartFuncDisable(uartId, uart_force_prity);
            break;
        case EVEN_PARITY:
            retVal = uartFuncEnable(uartId, uart_parity);
            retVal |= uartFuncEnable(uartId, uart_even);
            retVal |= uartFuncDisable(uartId, uart_force_prity);
            break;
        case FORCE_1_PARITY:
            retVal = uartFuncEnable(uartId, uart_parity);
            retVal |= uartFuncDisable(uartId, uart_even);
            retVal |= uartFuncEnable(uartId, uart_force_prity);
            break;
        case FORCE_0_PARITY:
            retVal = uartFuncEnable(uartId, uart_parity);
            retVal |= uartFuncEnable(uartId, uart_even);
            retVal |= uartFuncEnable(uartId, uart_force_prity);
            break;
        default:
            return OPL_ERROR;
    }

    return retVal;
}

/******************************************************************************
*
* serial_setbrg - set word length
*
* This routine set word length. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setwordlength(UINT8 uartId, UART_OWRD_LEN len)
{
    OPL_STATUS retVal;
    
    retVal = uartWordLenSet(uartId, len);

    return retVal;
}

/******************************************************************************
*
* serial_setbrg - set stop bit length
*
* This routine set stop bit length. 
*
*
* RETURNS: N/A
*/
OPL_STATUS serial_setstopbitlength(UINT8 uartId, STOP_BIT_LENGTH length)
{
    OPL_STATUS retVal ;

    retVal = uartStopBitSet(uartId, length);
    return retVal;
}

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
void serial_putc(UINT8 uartId, const char c)
{
    UINT32 tx_emply = OPL_FALSE;

    while ((uartLineStatusTxEmptyGet(uartId, &tx_emply) != OPL_OK) 
        || ((uartLineStatusTxEmptyGet(uartId, &tx_emply) == OPL_OK) 
            && (tx_emply == OPL_FALSE)))
    {
        ;
    }

    uartTxDataSet(uartId, c);
}

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
void serial_nowait_putc (const char c)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);

	if (c == '\n') serial_nowait_putc ('\r');

	pUart->w0.thr = c;
}

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
void serial_puts(UINT8 uartId, const char *s)
{
    while (*s)
    {
        serial_putc(uartId, *s++);
    }
}

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
int serial_getc(UINT8 uartId)
{
    UINT32 rx_ready = OPL_FALSE;
    UINT32 c;

    while((uartLineStatusRxReadyGet(uartId, &rx_ready) != OPL_OK) 
        || ((uartLineStatusRxReadyGet(uartId, &rx_ready) == OPL_OK)
            && (rx_ready == OPL_FALSE))); 

    uartRxDataGet(uartId, &c);

    return c;
}

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
int serial_gets(UINT8 uartId, UINT8 *data, UINT16 maxLen, UINT16 byteDly, UINT16 frmDly)
{
    UINT32 rx_ready = OPL_FALSE;
    UINT16 len = 0;

    while(uartLineStatusRxReadyGet(uartId, &rx_ready) && (rx_ready == OPL_TRUE))
    {
        uartRxDataGet(uartId, (UINT32 *)&data[len++]);
    }
    
    return len;
}

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
unsigned char serial_nowait_getc (void)
{
	unsigned char c;
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);

	c = pUart->w0.rhr;
	return c;
}

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
int serial_tstc(UINT8 uartId)
{
	unsigned char t;
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);
 #if 0   
	extern int enable_net_polling;
	
	if (enable_net_polling)
	{
		eth_rx();
	}
 #endif
 
	t = pUart->lsr;

	if( t & 0x80)
	{
		serial_puts(uartId, "fifo data err\n");
	}
	if( t & LSR_DR ) /* LSR_DR 0x1  */
	{
		/* Data in rfifo */
		return(1);
	}
	if( t & 0x02 )
	{
		serial_puts(uartId, "overrun err\n");
	}
	if( t & 0x04 )
	{
		serial_puts(uartId, "parity err\n");
	}
	if( t & 0x08 )
	{
		serial_puts(uartId, "frame err\n");
	}
	if( t & 0x10 )
	{
		serial_puts(uartId, "break interrupt\n");
	}
	return 0;
}

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
void set_tx_rx_trig_level(unsigned char t,unsigned char r)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);
	
	if( (char)t < 0 || t >3 || (char)r < 0 || r > 3)
		return;
	pUart->w1.ier &= ~(TxFIFO_BIT | RxFIFO_BIT);     /* disable interrupt */
	pUart->w2.fcr = (r<<6 | t<<4 |FIFO_ENABLE);	 /* set tx rx trig bits */
	pUart->w1.ier |= (TxFIFO_BIT | RxFIFO_BIT);	 /* enable tx rx interrupt */
}


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

unsigned char get_int_stat(void)
{		
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);
	
	return pUart->w2.isr;
}


/******************************************************************************
*
* serial_set_loopback	- set loopback mode 
*
* This routine set uart to the loopback mode 
*
* RETURNS: N/A
*/
void serial_set_loopback(void)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);
	pUart->mcr |= MCR_LOOP;
}

/******************************************************************************
*
* serial_disable_loopback	- disable loopback mode 
*
* This routine disable loopback mode 
*
* RETURNS: N/A
*/
void serial_disable_loopback(void)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);
	pUart->mcr &= ~MCR_LOOP;
}

/******************************************************************************
*
* serial_enable_fifo	- enable fifo  mode 
*
* This routine enable fifo mode 
*
* RETURNS: N/A
*/
void serial_enable_fifo(void)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);
	/*pUart->efr	|= 0x10; */ 			/*Enable efr[4] */	
	pUart->w2.fcr = FIFO_ENABLE;			/* | RxCLEAR | TxCLEAR;*/ /* reset fifo */
}

/******************************************************************************
*
* serial_disable_fifo	- disable fifo mode 
*
* This routine disable fifo mode 
*
* RETURNS: N/A
*/
void serial_disable_fifo(void)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_BA_B);
	pUart->w2.fcr = ~FIFO_ENABLE; 
}

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

OPL_STATUS serial_getlcr(UINT8 uartId, UINT32 *lcr)
{		
    OPL_STATUS retVal;
    
    printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
    retVal = oplRegFieldRead(REG_LINE_STATUS(uartId), 0, 8, lcr);
    if (OPL_OK != retVal)
    {
        printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
        return retVal;
    }

    printk("%s[%d]\r\n", __FUNCTION__, __LINE__);  /* yxzhao added for debug. */
    return OPL_OK;
}
/*begin added by liaohongjun 2012/12/25 of EPN204QID0019*/
OPL_STATUS Uart1Init(void)
{
    OPL_STATUS retVal;
    
    retVal = oplRegFieldWrite(REG_GB_UART_MODE, 0, 1, 1);
    if (OPL_OK != retVal)
    {
        return retVal;
    }    
    (void)serial_init();
    
    retVal = oplRegWrite(REG_UART1_REG_IE, 0x00);
    if (OPL_OK != retVal)
    {
        return retVal;
    }    
    retVal = oplRegWrite(REG_UART1_REG_LC, 0x83);
    if (OPL_OK != retVal)
    {
        return retVal;
    }     
    retVal = oplRegWrite(REG_UART1_REG_IS, 0xc0);
    if (OPL_OK != retVal)
    {
        return retVal;
    } 
    
    retVal = oplRegWrite(REG_UART1_REG_LC, 0x03);
    if (OPL_OK != retVal)
    {
        return retVal;
    }     
    retVal = oplRegWrite(REG_UART1_REG_IE, 0x05);
    if (OPL_OK != retVal)
    {
        return retVal;
    }     
    retVal = oplRegWrite(REG_UART1_REG_IS, 0xf1);
    if (OPL_OK != retVal)
    {
        return retVal;
    }     

	return  OPL_OK;    
}
/*end added by liaohongjun 2012/12/25 of EPN204QID0019*/

void uart1Test(void)
{
  
   UINT32 *pdata=NULL;


   pdata=kmalloc(16,GFP_KERNEL);
   if(NULL==pdata)
       printk("Malloc mem for uart1 fail!\n");

  while(1)
  {
   msleep(1);
  
   uartRxDataGet(1,pdata);   

   uartTxDataSet(1,*pdata);
  }
}
/** 
 *@} 
 */
 /* END FILE */
