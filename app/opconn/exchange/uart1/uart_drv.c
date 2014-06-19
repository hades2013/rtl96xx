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
* FILENAME:  uart_drv.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/uart1/uart_drv.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/major.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>
#include "ipmux_interrupt.h"
#include "uart_drv.h"


extern void ipmux_irq_enable( unsigned int irq );
extern void ipmux_irq_disable( unsigned int irq);

static unsigned char rxDataBuffMem[RX_BUFF_LEN];
static TX_DATA_BUFF_t txDataBuffMem;

static unsigned int rxPutOffset = 0;
static unsigned int rxGetOffset = 0;

static unsigned int  uartBaurtRate = 115200;
static wait_queue_head_t uart_rx_wait_queue;
static int      uart_rx_ready_event = 0;
static int      opl_uart1_is_open = 0;

static struct file_operations ufile_ctl_fops = { 
      read:ufile_read,
	  write:ufile_write,
      open: ufile_open , 
      ioctl: ufile_ioctl ,
      release: ufile_release , 
};

static struct tasklet_struct uart1_tx_pkt_tasklet;

static struct tasklet_struct uart1_rx_pkt_tasklet;
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

int serial_init (void)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);

	/* disable UART interrupts for now */
	pUart->w1.ier = 0; 	

	/*set the baudrate*/
	serial_setbrg();
	
	/*set the char len, partity mode, stop bit*/
	pUart->lcr = CHAR_LEN_8 | PARITY_NONE | ONE_STOP;

	return 0;
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
void serial_setbrg (void)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
	unsigned short fdiv;
	
	/*set divisor register dll, dlm */
	pUart->lcr |= LCR_DLAB;

	fdiv = (FPCLK /16) / uartBaurtRate;
	pUart->w0.dll = fdiv & 0xFF;
	pUart->w1.dlm = fdiv >> 8;
	pUart->lcr &= ~LCR_DLAB;

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
void serial_putc (const char c)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);

	//while((pUart->lsr & 0x20/*LSR_THRE*/)==0); 
	while((pUart->lsr & 0x40/*LSR_THRE*/)==0); 
	pUart->w0.thr = c;
	
	/*opipmux_sync();*/

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
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);

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
void serial_puts (const char *s)
{
	while (*s)
	{
		serial_putc (*s++);
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
int serial_getc (void)
{
	unsigned char c ;
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
	
	while((pUart->lsr & LSR_DR ) == 0);
	c = pUart->w0.rbr;

	return c;
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
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);

	c = pUart->w0.rbr;
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
int serial_tstc (void)
{
	unsigned char t;
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
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
		serial_puts("fifo data err\n");
	}
	if( t & LSR_DR ) /* LSR_DR 0x1  */
	{
		/* Data in rfifo */
		return(1);
	}
	if( t & 0x02 )
	{
		serial_puts("overrun err\n");
	}
	if( t & 0x04 )
	{
		serial_puts("parity err\n");
	}
	if( t & 0x08 )
	{
		serial_puts("frame err\n");
	}
	if( t & 0x10 )
	{
		serial_puts("break interrupt\n");
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
* 		designate 8, 16, 32, 48 bytes trigger point. 
* 	   2: the tx trigger level.  It has four mode.  The value 0,1,2,3 
* 		designate 8, 16, 32, 48 bytes trigger point. 
*  
* RETURNS: N/A
*/
void set_tx_rx_trig_level(unsigned char t,unsigned char r)
{
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
	
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
* This routine set fifo receiver trigger level
*
* INPUT:   l: the rx trigger level.  It has four mode.  The value 0,1,2,3 
* 		designate 8, 16, 32, 48 bytes trigger point. 
*  
* RETURNS: N/A
*/

unsigned char get_int_stat(void)
{		
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
	
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
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
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
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
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
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
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
	volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
	pUart->w2.fcr = ~FIFO_ENABLE; 
}

int uart2Init(void)
{
#if defined(__ONU_SDK__)
    *(unsigned int *)0xBF0000c0 = 1;
#elif defined(__FPGA_ONU_SDK__)
    //*(unsigned int *)0xBF000008 = 1;
    *(unsigned int *)0xBF0000c0 = 1;
#elif defined(__IPMUX_SDK__)
    *(unsigned int *)0xBF000008 = 1;
#endif

    serial_init();

    *(unsigned int *)(UART1_ADDR + 0x4) = 0x00;
    *(unsigned int *)(UART1_ADDR + 0xc) = 0x83;
    *(unsigned int *)(UART1_ADDR + 0x8) = 0xc0;
    
    *(unsigned int *)(UART1_ADDR + 0xc) = 0x03;    
    *(unsigned int *)(UART1_ADDR + 0x4) = 0x05;
    *(unsigned int *)(UART1_ADDR + 0x8) = 0xf1;
    
    return OPL_OK;
}
#ifdef LINUX_24
static void uartInterrupt(int irq, void *dev_id, struct pt_regs * regs)
#else
static irqreturn_t uartInterrupt(int irq, void *dev_id, struct pt_regs * regs)
#endif
{
    unsigned int intStatus = 0;
    unsigned int count = 0;
    volatile struct uart *pUart = (volatile struct uart*)(UART1_ADDR);
    unsigned int needWakeupRead = 0;

    intStatus = *(unsigned int *)(UART1_ADDR + 0x8);    
    ipmux_irq_disable(OPL_UART1_IRQ);

    switch(intStatus&0x3f)
    {
        case LINE_STATUS_INT:
            
            while((pUart->lsr & LSR_DR) == 1)
            {
                rxDataBuffMem[rxPutOffset++] = pUart->w0.rbr;  
                rxPutOffset = rxPutOffset%RX_BUFF_LEN;
            }
            if (uart1_rx_pkt_tasklet.func != NULL)
                tasklet_hi_schedule(&uart1_rx_pkt_tasklet);
            //uart_rx_ready_event = 1;
            //wake_up_interruptible(&uart_rx_wait_queue);
            break;
        case RX_READY_INT:
            count = 0;
            /*first read fifo length data*/
            while(count < FIFO_LEN)
            {
                rxDataBuffMem[rxPutOffset++] = pUart->w0.rbr;
                rxPutOffset = rxPutOffset%RX_BUFF_LEN;
                count++;
            }
            /*other fifo lenth data continue*/
            while((*(unsigned int *)(UART1_ADDR + 0x8))&0x04)
            {
                count = 0;
                while(count < FIFO_LEN)
                {
                    rxDataBuffMem[rxPutOffset++] = pUart->w0.rbr;
                    rxPutOffset = rxPutOffset%RX_BUFF_LEN;
                    count++;
                }
            }
            /*other data less than fifo length*/
            while((pUart->lsr & LSR_DR) == 1)
            {
                rxDataBuffMem[rxPutOffset++] = pUart->w0.rbr;  
                rxPutOffset = rxPutOffset%RX_BUFF_LEN;
            }
            if (uart1_rx_pkt_tasklet.func != NULL)
                tasklet_hi_schedule(&uart1_rx_pkt_tasklet);
            //uart_rx_ready_event = 1;
            //wake_up_interruptible(&uart_rx_wait_queue);
            break;
        case RX_TIMEOUT_INT:      
            while((pUart->lsr & LSR_DR) == 1)
            {
                rxDataBuffMem[rxPutOffset++] = pUart->w0.rbr; 
                rxPutOffset = rxPutOffset%RX_BUFF_LEN;
            }
            if (uart1_rx_pkt_tasklet.func != NULL)
                tasklet_hi_schedule(&uart1_rx_pkt_tasklet);

            //uart_rx_ready_event = 1;
            //wake_up_interruptible(&uart_rx_wait_queue);
            break;
        case TX_HOLD_REG_EMPTY_INT:
            /*opl_sdk_printf(("tx hold register empty.\n"));*/
            break;
        case UNUSED_INT:
            opl_sdk_printf(("unused.\n"));
            break;
        case CHANGE_OF_CTS_STS_INT:
            opl_sdk_printf(("changed of cts and rts.\n"));
            break;
        default:
            opl_sdk_printf(("NULL interrupt.\n"));
            break;
    }
   
    ipmux_irq_enable(OPL_UART1_IRQ);
#ifndef LINUX_24
    return IRQ_HANDLED;
#endif
}

static size_t ufile_read(struct file *file, char *buf,
			size_t count, loff_t * ppos)
{
    size_t len = 0; 
    unsigned int putOffset = 0;
    unsigned int getOffset = 0;
    int iRet = 0;
    
    /*sem protected.*/

    if(rxPutOffset == rxGetOffset)
    {
        wait_event_interruptible(uart_rx_wait_queue,uart_rx_ready_event);
    }
    
    putOffset = rxPutOffset;
    getOffset = rxGetOffset;
    
    if(putOffset == getOffset)
    {
        len = 0;
    }else if(getOffset < putOffset)
    {
        if(count <=  (putOffset - getOffset))

        {
            if (copy_to_user(buf, &rxDataBuffMem[getOffset], count))
            {
                iRet = -EFAULT;   
                goto exit;
            }
			    
            len = count; 
        }else
        {
            if (copy_to_user(buf,&rxDataBuffMem[getOffset],putOffset - getOffset))
            {
                 iRet = -EFAULT;   
                 goto exit;
            }
            len = putOffset- getOffset;
        }
    }else
    {
        if(count <= (RX_BUFF_LEN - (getOffset - putOffset)))
        {
            if(count <= RX_BUFF_LEN - getOffset)
            {
                if (copy_to_user(buf,&rxDataBuffMem[getOffset],count))
			    {
                     iRet = -EFAULT;   
                     goto exit;
                }
            }else
            {
                if (copy_to_user(buf,&rxDataBuffMem[getOffset],RX_BUFF_LEN - getOffset))
			    {
                     iRet = -EFAULT;   
                     goto exit;
                }
                if (copy_to_user(&buf[RX_BUFF_LEN - getOffset],&rxDataBuffMem[0],count - (RX_BUFF_LEN - getOffset)))
			    {
                     iRet = -EFAULT;   
                     goto exit;
                }
            }
            len = count;
        }else 
        {
            if (copy_to_user(buf,&rxDataBuffMem[getOffset],RX_BUFF_LEN - getOffset))
			{
                 iRet = -EFAULT;   
                 goto exit;
            }
            if (copy_to_user(&buf[RX_BUFF_LEN - getOffset],&rxDataBuffMem[0],putOffset))
			{
                 iRet = -EFAULT;   
                 goto exit;
            }
            len = RX_BUFF_LEN - (getOffset - putOffset);
        }
    }

   iRet = len;
   
exit:
    
    rxGetOffset = (rxGetOffset + len)%(RX_BUFF_LEN);

    if(rxGetOffset == rxPutOffset)
    {
        uart_rx_ready_event = 0;    
    }
    
    return iRet;
}

static size_t ufile_write(struct file *file, char *buf,
			size_t count, loff_t * ppos)
{
    size_t len = count;
    
    if(count > TX_BUFF_LEN)
    {
        return -EFAULT;
    }
    
    if (copy_from_user(&txDataBuffMem.buff[0], buf, count))
			return -EFAULT;

    txDataBuffMem.len = count;
    
    tasklet_hi_schedule(&uart1_tx_pkt_tasklet);
    
    return count;
}
/*tx tasklet handle*/
static int uartTxPacket(TX_DATA_BUFF_t *pBuff)
{
    unsigned int len = 0;

/*  ipmux_irq_disable(OPL_UART1_IRQ);*/
    while(len < txDataBuffMem.len)
    {
        serial_putc(pBuff->buff[len++]);   
    }
    
/*  opl_dump_data(txDataBuffMem.buff,txDataBuffMem.len,16);*/
    
    memset(&txDataBuffMem,0x00,sizeof(TX_DATA_BUFF_t));
/*  ipmux_irq_enable(OPL_UART1_IRQ);*/
    return 0;
}
/*rx tasklet handle*/
static int uartRxPacket(int reg)
{
    size_t len = 0; 
    unsigned int   putOffset = 0;
    unsigned int   getOffset = 0;
    unsigned int   paloadlen  = 0;
  
    unsigned char  buff[1518];
    int iRet  = 0;
    int flag  = 1;
    int count = 1518;

    unsigned char palSfd[4] = {0x5a,0x5a,0x5a,0xd5};
    unsigned char palEfd[4] = {0xa5,0xa5,0xa5,0x5d};
    
    putOffset = rxPutOffset;
    getOffset = rxGetOffset;
    memset(buff,0x0,1518);
    if(putOffset == getOffset)
    {
        len = 0;
    }else if(getOffset < putOffset)
    {
        if(count <=  (putOffset - getOffset))
        {
          memcpy(buff, &rxDataBuffMem[getOffset], count);
          len = count; 
        }else
        {
            memcpy(buff,&rxDataBuffMem[getOffset],putOffset - getOffset);           
            len = putOffset- getOffset;
        }
    }else
    {
        if(count <= (RX_BUFF_LEN - (getOffset - putOffset)))
        {
            if(count <= RX_BUFF_LEN - getOffset)
            {
               memcpy(buff,&rxDataBuffMem[getOffset],count);
			 
            }else
            {
                memcpy(buff,&rxDataBuffMem[getOffset],RX_BUFF_LEN - getOffset);			   
                memcpy(&buff[RX_BUFF_LEN - getOffset],&rxDataBuffMem[0],count - (RX_BUFF_LEN - getOffset));
			  
            }
            len = count;
        }else 
        {
            memcpy(buff,&rxDataBuffMem[getOffset],RX_BUFF_LEN - getOffset);			
            memcpy(&buff[RX_BUFF_LEN - getOffset],&rxDataBuffMem[0],putOffset);			
            len = RX_BUFF_LEN - (getOffset - putOffset);
        }
    }
    opl_dump_data(buff,len,16);
    if(0 == memcmp(&buff[len - sizeof(palEfd)],palEfd,sizeof(palEfd)))
    {
        /*rxDataBuffMem empty*/
        /*if (0 == uart_rx_ready_event)
        {
            paloadlen  = buff[sizeof(palSfd)]<<8;
            paloadlen |= buff[sizeof(palSfd) + 1];

            if (memcmp(buff,palSfd,sizeof(palSfd)) ||
                memcmp(&buff[6+paloadlen+2],palEfd,sizeof(palEfd)))
            {
                rxGetOffset = (rxGetOffset + len)%(RX_BUFF_LEN);
                opl_dump_data(buff,len,16);
                return 0;
            }
            
        }*/
        uart_rx_ready_event = 1;
        wake_up_interruptible(&uart_rx_wait_queue);
    }

    return 0;
}
static int ufile_open(struct inode *inode , struct file *file)
{
    unsigned int intStatus;
    /*disable uart1 interrupt*/   
    *(unsigned int *)(UART1_ADDR + 0x4) = 0x0;
    /*clear Rx/Tx fifo*/
    *(unsigned int *)(UART1_ADDR + 0x8) = 0xf7;
    /*clear the interrupt single*/
    intStatus = *(unsigned int *)(UART1_ADDR + 0x8); 
    test_and_set_bit(0,&opl_uart1_is_open);
    memset(&txDataBuffMem,0x00,sizeof(TX_DATA_BUFF_t));
    memset(&rxDataBuffMem,0x00,RX_BUFF_LEN);
    rxPutOffset = rxGetOffset = 0;
    /*register two tasklet handle*/
    tasklet_init(&uart1_tx_pkt_tasklet,uartTxPacket,(unsigned long long)&txDataBuffMem);    
    tasklet_init(&uart1_rx_pkt_tasklet,uartRxPacket,(unsigned long long)0);
    /*enable uart1 interrupt*/
    *(unsigned int *)(UART1_ADDR + 0x4) = 0x05;
    return 0;
}

static int ufile_release(struct inode *inode, struct file *filp)
{
    /*disable uart1 Rx interrupt*/
    *(unsigned int *)(UART1_ADDR + 0x4) = 0x0;
    tasklet_kill(&uart1_tx_pkt_tasklet);
    tasklet_kill(&uart1_rx_pkt_tasklet);
    return 0;
}

static int ufile_ioctl(struct inode *inode, struct file *file, unsigned int command ,unsigned long arg)
{
    int status;
    int len;
    if(!test_bit(0,&opl_uart1_is_open)){
        return -ENODEV;
    }

    switch (command)
    {
        case UART1_COMMAND_CLEAR_RXBUFFER:
            rxPutOffset = 0;
            rxGetOffset = 0;
            memset(rxDataBuffMem,0x0,RX_BUFF_LEN);
            break;
        case UART1_COMMAND_CLEAR_TXBUFFER:
            memset(txDataBuffMem.buff,0x0,TX_BUFF_LEN);
            break;
        case ENABLE_UART1_INTERRUPT:
            ipmux_irq_enable(OPL_UART1_IRQ);
            break;
        case DISABLE_UART1_INTERRUPT:
            ipmux_irq_disable(OPL_UART1_IRQ);
            break;
        case DUMP_UART1_RXBUFFER:
            len = ((arg > RX_BUFF_LEN)?RX_BUFF_LEN:arg);
            opl_dump_data(rxDataBuffMem,len,16);
            break;
        case DUMP_UART1_TXBUFFER:
            len = ((arg > TX_BUFF_LEN)?TX_BUFF_LEN:arg);
            opl_dump_data(txDataBuffMem.buff,len,16);
            break;
        default:
            return -ENODEV;
    
    }
    return 0;    
}

static int rxDataBuffInit(void)
{
    memset(rxDataBuffMem,0x0,RX_BUFF_LEN);
    rxPutOffset = rxGetOffset = 0;

    return 0;
}

static int __init u_file_init(void)
{
	int err;
    int threadId;



    uart2Init();

    ipmux_irq_disable(OPL_UART1_IRQ);
    
	err = register_chrdev(UART1_MAJOR, UFILE_NAME,  &ufile_ctl_fops);
    
	if(err<0)
	{
		printk("fail to register\n");   	
		return -1;
	}


    init_waitqueue_head(&uart_rx_wait_queue);

    if(request_irq(OPL_UART1_IRQ, &uartInterrupt,0, "uart1", NULL)) {
        unregister_chrdev(UART1_MAJOR, UFILE_NAME);	
	    return -1;
    }

    ipmux_irq_enable(OPL_UART1_IRQ);
    
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));

    opl_sdk_printf(("ufile name:%s\n",UFILE_NAME));
       


	return 0;
}

static void __exit u_file_exit(void)
{
	printk ( "release this device!!!\n" );
    free_irq(OPL_UART1_IRQ,NULL);
	unregister_chrdev(UART1_MAJOR, UFILE_NAME);	
    opl_sdk_printf(("ufile name:%s\n",UFILE_NAME));
    return;
}

module_init(u_file_init);
module_exit(u_file_exit);

#ifdef LINUX_24
MODULE_LICENSE("GPL2");
#else
MODULE_LICENSE("Dual BSD/GPL");
#endif
