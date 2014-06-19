/*
 * ===============================================================================
 * Source Name: opl_dma.c
 *
 * General Description:
 *    DMA driver for Opconn, incl. DMA send and receiving, buffer descriptor status
 * get, etc.   
 *
 * ===============================================================================
 * $Copyright: (c) 2007 OPulan Corp.
 * All Rights Reserved.$
 * ===============================================================================
 *  
 * Revision History:
 *                 
 * Author                Date              Description of Changes
 * ----------------   ------------  ----------------------------------------------
 * rzhou              12/10/2007    initial
 * ----------------   ------------  ----------------------------------------------
 *
 * ===============================================================================
 */
 
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <stdio.h>

#include "opl_driver.h"
//#include "hw_interface.h"
#include "opl_dma.h"
#include "opl_host.h"
#include "opl_debug.h"
#include "opconn_hw_reg.h"


#ifdef OPCONN_DMA_DRV_DEBUG

#define OPL_DMA_DEBUG(format, ...) \
do {\
	if((gbDmaDbg)&&(gbLogDbg)) fprintf(stderr,  format, ##__VA_ARGS__ ); }while(0)
#else
	#define OPL_DMA_DEBUG(format) 
#endif

#define DEFAULT_MAX_TIMEOUT   0x0000FFFF

/* Packet dump switch */
#ifdef OPCONN_DMA_DRV_DEBUG
static void printPacket(volatile char *p, UINT16 len);
#endif

/* Local definitions */
#define MAX_PKT_LENGTH			1536	/* Maximum packet to send */
#define OPCONN_DMA_BUF_LEN	2048	/* Maximum length of BD buffer */

/* CPDMA Register Addresses Defines */
#define OPCONN_DMA_INT_RX     0x1     /* bit 0: RX int enable */  
#define OPCONN_DMA_INT_TX     0x2     /* bit 1: TX int enable */

#define  DMA_BASEADDR         			REG_DMA0_BASE_ADDR  /* Buffer base address low 8 bits */
#define  DMA_DMAEN            				REG_DMA0_EN  /* DMA enable */
#define  DMA_BDINT            					REG_DMA0_INT  /* Buffer DMA interrupt */
#define  DMA_BDIE             					REG_DMA0_INT_EN  /* Buffer DMA interrupt enable */

#define  DMA_RXBD_POINTER  			REG_DMA0_RXBD_POINTER  /* DMA RX BD Pointer */
#define  DMA_TXBD_POINTER  			REG_DMA0_TXBD_POINTER  /* DMA TX BD Pointer */
    
#define  DMA_SHADOW_SET_RX          			REG_DMA0_SHADOW_SET_RXBD      /* Indirected access data for RX */
#define DMA_MEM_INDIR_ACC_CTRL_RX   	REG_DMA0_MEM_INDIR_ACC_CTRL_RXBD      /* Indirected access control for RX */
  
#define  DMA_SHADOW_SET_TX         				REG_DMA0_SHADOW_SET_TXBD      /* Indirected access data for TX */
#define  DMA_MEM_INDIR_ACC_CTRL_TX  	REG_DMA0_MEM_INDIR_ACC_CTRL_TXBD      /* Indirected access control for RX */

/* BD Table Defines */
#define DMABUF_NUM_GEL_RX      64       /* Number of RX BD entry */
#define DMABUF_NUM_GEL_TX      64       /* Number of TX BD entry */

#define DMABUF_TX_OFFSET   (DMABUF_NUM_GEL_RX*OPCONN_DMA_BUF_LEN)

/* Indirect Access Control defines */
#define IND_ACC_TYPE_WRITE 	0x1<<30
#define IND_ACC_TYPE_READ  	0x0<<30
#define IND_ACC_BUSY       	0x1<<31

#define BDTABLE_ID_SHIFT 	0
#define TX_BDTABLE_ID 	0<<BDTABLE_ID_SHIFT
#define RX_BDTABLE_ID 	0<<BDTABLE_ID_SHIFT

/* BD table defines */
#define BD_OFFSET_SHIFT    	11        /* Start byte shift in BD */ 
#define BD_LENGTH_SHIFT    	17       /* Buffer length shift in BD */ 
#define BD_END_FLAG_SHIFT  	28       /* End flag shift in BD */
#define BD_VALID_SHIFT    	29       /* Valid bit shift in BD */

#define BD_OFFSET_MASK      	((UINT32)0x3F << BD_OFFSET_SHIFT)   	/* 11:16 bits, 6 bits Start Bytes */
#define BD_LENGTH_MASK      	((UINT32)0x7FF << BD_LENGTH_SHIFT) 	/* 17:27 bits, 11 bits buffer length */

#define BD_END_FLAG_BIT     	((UINT32)0x1 << BD_END_FLAG_SHIFT)  	/* 28 bit end flag shift in BD */
#define BD_VALID_BIT        	((UINT32)0x1 << BD_VALID_SHIFT)	   	/* 29 bit, Full or Ready bit mask in BD */

#define RX_BUF_BLOCKID(bdIdx) (bdIdx)
#define TX_BUF_BLOCKID(bdIdx) ((bdIdx) + DMABUF_NUM_GEL_RX)

extern UINT32 malloc_dma_rxbuf(UINT32 dmas, UINT32 size);

#ifdef OPL_DMA0_ETH
extern int read_from_dma_rxbuf(int dmas,int bd,void *pkt,int *len);
#else
extern int read_from_dma_rxbuf(int dmas,int bd,void *pkt,int len);
#endif
extern int write_to_dma_txbuf(int dmas,int bd,void *pkt,int len);
void set_dma_txbuf_addr(int dmas,UINT32 addr);

/* Internal Function Declarations */
static void init_bd_rx(void);
static void init_bd_tx(void);

/* Rx and Tx bd read functions */
static UINT32 rx_bd_read(UINT8 bdIndex);
static UINT32 tx_bd_read(UINT8 bdIndex);

/* Rx and Tx bd write functions */
static void rx_bd_write(UINT32 bd_data, UINT8 bd_index);
static void tx_bd_write(UINT32 bd_data, UINT8 bd_index);

static void rx_inc_bd_idx(void);
static void tx_inc_bd_idx(void);

/* Rx and Tx bd index */
volatile static UINT8 rx_bd_id;
volatile static UINT8 tx_bd_id;

volatile static char * DMA_Buffers 		= OPL_NULL;

volatile static char *DMA_BuffersPkts 	= OPL_NULL;

#define KSEG0_TO_KSEG1(x) ((UINT32)(x)|0xa0000000)

/* Store one packet for upper protocol */
#define RX_BUF_ADDR(bdIdx) ((UINT32)DMA_BuffersPkts)

#define TX_BUF_ADDR(bdIdx) (KSEG0_TO_KSEG1((UINT32)DMA_Buffers + (bdIdx)*OPCONN_DMA_BUF_LEN + DMABUF_TX_OFFSET))

/* Register counters */
struct REG_COUNTER_s
{	
	INT8 *name ;                       /* counter register name */
	UINT32 reg_addr;         /* counter register address */
	UINT32 val_mask;         /* counter register bit width */
	UINT8 *description;  /* counter register description */  
	
} REG_COUNTER_t;

static struct REG_COUNTER_s dma_reg_counters[] = 
{
	/* Name                             regAddr   ValMask      Description */
	{"DMA0_Q0_CELL_CNT",	0x6040*4, 	32,    "DMA 0 Queue 0 Cell counter" },
	{"DMA0_Q1_CELL_CNT",	0x6041*4,	32,    "DMA 0 Queue 1 Cell counter" },
	{"DMA0_Q2_CELL_CNT",	0x6042*4,	32,    "DMA 0 Queue 2 Cell counter" },
	{"DMA0_Q3_CELL_CNT",	0x6043*4,	32,    "DMA 0 Queue 3 Cell counter" },
	{"DMA0_Q4_CELL_CNT",	0x6044*4,	32,    "DMA 0 Queue 4 Cell counter" },
	{"DMA0_Q5_CELL_CNT",	0x6045*4,	32,    "DMA 0 Queue 5 Cell counter" },
	{"DMA0_Q6_CELL_CNT",	0x6046*4,	32,    "DMA 0 Queue 6 Cell counter" },
	{"DMA0_Q7_CELL_CNT",	0x6047*4,	32,    "DMA 0 Queue 7 Cell counter" },
	{"DMA0_Q0_PKT_CNT",		0x6048*4,	32,    "DMA 0 Queue 0 packet counter" },
	{"DMA0_Q1_PKT_CNT",		0x6049*4,	32,    "DMA 0 Queue 1 packet counter" },
	{"DMA0_Q2_PKT_CNT",		0x604a*4,	32,    "DMA 0 Queue 2 packet counter" },
	{"DMA0_Q3_PKT_CNT",		0x604b*4,	32,    "DMA 0 Queue 3 packet counter" },
	{"DMA0_Q4_PKT_CNT",		0x604c*4,	32,    "DMA 0 Queue 4 packet counter" },
	{"DMA0_Q5_PKT_CNT",		0x604d*4,	32,    "DMA 0 Queue 5 packet counter" },
	{"DMA0_Q6_PKT_CNT",		0x604e*4,	32,    "DMA 0 Queue 6 packet counter" },
	{"DMA0_Q7_PKT_CNT",		0x604f*4,	32,    "DMA 0 Queue 7 packet counter" },
	{"DMA0_RX_CNT",					0x6050*4,	32,    "DMA 0 RX counter" },
	{"DMA0_TX_CNT",					0x6051*4,	32,    "DMA 0 TX counter" },
	{"DMA0_GE2CPU_DRPPKT_CNT",		0x6052*4,	32,    "DMA 0 GE to CPU drop packet counter" },
	{"DMA0_PON2CPU_DRPPKT_CNT",	0x6053*4,	32,    "DMA 0 PON to CPU drop packet counter" },
	{OPL_NULL,	0,	0,   ""}
};



/*******************************************************************************
*
* bd_rx_read_valid
*
* DESCRIPTION:
*   This function find and read valid rx bd.
*
* INPUT:
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   UINT16      - the valid bd, there is new packet in the bd.
*
* SEE ALSO:
*/
static UINT32 bd_rx_read_valid(UINT8 *bd_index)
{
	UINT32 bd_data = 0;
	UINT8 i ;
	
	for (i=*bd_index; i<(*bd_index+DMABUF_NUM_GEL_RX); i++) 
	{
		bd_data = rx_bd_read(i%DMABUF_NUM_GEL_RX);

		if(bd_data & BD_VALID_BIT)
		{
			*bd_index = i%DMABUF_NUM_GEL_RX;
			return bd_data;
		}
	}
	return 0;
}




/*
*
* rx_bd_write
*
* DESCRIPTION:
*   This function write rx bd.
*
* INPUT:
*   data        - the data to write to bd
*   bdIndex     - the index of bd which is to be written
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
static void rx_bd_write(UINT32 bd_data, UINT8 bd_index)
{
	UINT32 timeout = DEFAULT_MAX_TIMEOUT;
    
	/* Add the end flag to data */
	if (bd_index == (DMABUF_NUM_GEL_RX - 1)) 
	{
		bd_data = bd_data | BD_END_FLAG_BIT;
	}

	oplRegWrite(DMA_SHADOW_SET_RX, bd_data);
	oplRegWrite(DMA_MEM_INDIR_ACC_CTRL_RX, RX_BDTABLE_ID | IND_ACC_BUSY | IND_ACC_TYPE_WRITE | bd_index);
           
	while(oplRegBitTest(DMA_MEM_INDIR_ACC_CTRL_RX, IND_ACC_BUSY))
	{
		OPL_DMA_DEBUG("waiting write  bd table !\n");

		if(!timeout--)
		{
			OPL_DMA_DEBUG("!!!Write rx bd table timeout\n");
			break;
		}
	}
}




/*
*
* rx_bd_read
*
* DESCRIPTION:
*   This function read rx bd.
*
* INPUT:
*   bd_index  - the index of bd which is to be read
*
* OUTPUT: None
*
* RETURNS:
*   The bd data
*
* SEE ALSO:
*/
static UINT32 rx_bd_read(UINT8 bd_index)
{
	UINT32 bd_data = 0;
	UINT32 timeout = DEFAULT_MAX_TIMEOUT;
    
	oplRegWrite(DMA_MEM_INDIR_ACC_CTRL_RX, RX_BDTABLE_ID |IND_ACC_BUSY | IND_ACC_TYPE_READ | bd_index);
	
 	while(oplRegBitTest(DMA_MEM_INDIR_ACC_CTRL_RX, IND_ACC_BUSY))
	{
		OPL_DMA_DEBUG("waiting in rx_bd_read, bdIndex=%x !\n", bd_index);
    	
		if(!timeout--)
		{
 			OPL_DMA_DEBUG("Read rx bd table timeout\n");
			return 0;
   		}
	}
	oplRegRead(DMA_SHADOW_SET_RX, &bd_data);
    
	return bd_data;
}




/*
*
* tx_bd_write
*
* DESCRIPTION:
*   This function write tx bd.
*
* INPUT:
*   data        - data to be written
*   bd_index    - the index of bd which is to be written
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
static void tx_bd_write(UINT32 bd_data, UINT8 bd_index)
{
	UINT32 timeout = DEFAULT_MAX_TIMEOUT;
     
	/* add the end flag to data */
	if (bd_index == (DMABUF_NUM_GEL_TX - 1)) 
	{
		bd_data = bd_data | BD_END_FLAG_BIT;
	}
		
	oplRegWrite(DMA_SHADOW_SET_TX, bd_data);
	oplRegWrite(DMA_MEM_INDIR_ACC_CTRL_TX, TX_BDTABLE_ID |IND_ACC_BUSY | IND_ACC_TYPE_WRITE | bd_index);
    
	while (oplRegBitTest(DMA_MEM_INDIR_ACC_CTRL_TX, IND_ACC_BUSY))
	{
		OPL_DMA_DEBUG("waiting in tx_bd_write !\n");

		if(!timeout--)
		{
			OPL_DMA_DEBUG("!!!Write TX bd table timeout\n");
			break;
		}
	}
}




/*
*
* tx_bd_read
*
* DESCRIPTION:
*   This function read tx bd.
*
* INPUT:
*   bd_index - the index of bd to be read
*
*
* OUTPUT: None
*
* RETURNS:
*   the data of the indicated bd
*
* SEE ALSO:
*/
static UINT32 tx_bd_read(UINT8 bd_index)
{
	UINT32 bd_data = 0;
	UINT32 timeout = DEFAULT_MAX_TIMEOUT;
     
	oplRegWrite(DMA_MEM_INDIR_ACC_CTRL_TX, TX_BDTABLE_ID |IND_ACC_BUSY | IND_ACC_TYPE_READ | bd_index);
	
	while(oplRegBitTest(DMA_MEM_INDIR_ACC_CTRL_TX, IND_ACC_BUSY))
	{
		if(!timeout--)
		{
			OPL_DMA_DEBUG("!!!waiting wrtie tx bd table !\n");
			break;
		}
 	}
	oplRegRead(DMA_SHADOW_SET_TX, &bd_data);
    
	return bd_data;
}




/*
*
* init_bd_rx
*
* DESCRIPTION:
*   This function init rx bds.
*
* INPUT:
*
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
static void init_bd_rx(void)
{
	UINT32 bdData;
	UINT8 i;
	
	for (i=0; i<DMABUF_NUM_GEL_RX; i++) 
	{
		 /* 
		 Indicate the block id. NOTE : the end flag is set in rx_bd_write() 
		 */
		 bdData = RX_BUF_BLOCKID(i);
		 
		 rx_bd_write(bdData, i);
	}
}




/*
*
* init_bd_tx
*
* DESCRIPTION:
*   This function init tx bd.
*
* INPUT:
*
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
static void init_bd_tx(void)
{
	UINT32 bdData;
	UINT8 i;
	
	for (i=0; i<DMABUF_NUM_GEL_TX; i++) 
	{
		/* 
		  Init tx length to 1, and indicate the block id. 
		  NOTE : the end flag is set in rx_bd_write() 
		*/
		bdData = (UINT32)1<<BD_LENGTH_SHIFT | TX_BUF_BLOCKID(i);
		
		tx_bd_write(bdData,i);
	}
}




/*
*
* eopl_dma_send
*
* DESCRIPTION:
*   This function send packet via opconn dma.
*
* INPUT:
*   packet      - the pointer to packet data
*   length      - the length of the packet
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   0      - send success
*   -1     - send failed
*
* SEE ALSO:
*/
INT16 eopl_dma_send(void *packet, UINT16 length)
{
	#ifdef OPL_DMA0_ETH
	if((OPL_NULL == packet)||(length<=sizeof(host_outbound_hdr_t))||(length>MAX_PKT_LENGTH))
	{
		printf("parameter invalid.\n");
		hostCnt.dmaTxFail ++;
		return -1;
			
	}
	if (packet != OPL_NULL) 
	{
		write_to_dma_txbuf(0, 0, packet, length);
	}
    	hostCnt.dmaTx ++;
	
	return OPL_OK;
	#else
	UINT32 bdData;
	UINT32 len;
	volatile INT32 timeout = DEFAULT_MAX_TIMEOUT;

	if((OPL_NULL == packet)||(length<=sizeof(host_outbound_hdr_t))||(length>MAX_PKT_LENGTH))
	{
		printf("invalid parameter.\n");
		return -1;
	}

	OPL_DMA_DEBUG("Wait for dma ready\n");

	/* wait for dma ready for send  */
	while (tx_bd_read(tx_bd_id) & BD_VALID_BIT) 
	{
		if(!timeout--)
		{
			OPL_DMA_DEBUG("TX wait timeout!!!, bd: %d. \n", tx_bd_id);
            		return -1;
		}
	}
	
	/* copy data to buffer  */
	OPL_DMA_DEBUG("TX : bd=%d, packaddr=0x%x, data len=%d, bufaddr=%x\n", 
             tx_bd_id, (INT32)packet, length, TX_BUF_ADDR(tx_bd_id));

	OPL_DMA_DEBUG("Copy pack from %x to %x, length = %d\n", (INT32)packet, TX_BUF_ADDR(tx_bd_id), length);

	/* In case that packet has been copied into DMA buffer yet.*/
	if (packet != OPL_NULL) 
	{
		write_to_dma_txbuf(0, tx_bd_id, packet, length);
	}
	    
	OPL_DMA_DEBUG("Copy packet ok.\n");

#ifdef OPCONN_DMA_DRV_DEBUG
	printPacket((volatile char *)packet, length);
#endif
	
	OPL_DMA_DEBUG("Send:send data\n");

	/* max len = 1536, the length not include 1 byte outbound header */
	len = (UINT32)(length -sizeof(host_outbound_hdr_t)) ;
	len = (len << BD_LENGTH_SHIFT) & BD_LENGTH_MASK;
    
	bdData = len | BD_VALID_BIT | TX_BUF_BLOCKID(tx_bd_id);

	OPL_DMA_DEBUG("Write bd tx_bd_id %d to  0x%x\n", tx_bd_id, bdData);
 	
 	tx_bd_write(bdData, tx_bd_id);
	
	OPL_DMA_DEBUG("Packet send OK!!!\n");
	
	tx_inc_bd_idx();
    hostCnt.dmaTx ++;
	return OPL_OK;
	#endif
}

/*
*
* tx_inc_bd_idx
*
* DESCRIPTION:
*   This function increase bd tx index.
*
* INPUT:
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
static void tx_inc_bd_idx(void)
{
	tx_bd_id++;
	tx_bd_id %= DMABUF_NUM_GEL_TX;
}




/*
*
* rx_inc_bd_idx
*
* DESCRIPTION:
*   This function increase bd rx index.
*
* INPUT:
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
static void rx_inc_bd_idx(void)
{
	rx_bd_id++;
	rx_bd_id %= DMABUF_NUM_GEL_RX;
}



#ifdef OPCONN_DMA_DRV_DEBUG

/*
*
* printPacket
*
* DESCRIPTION:
*   This function print packet content in bytes.
*
* INPUT:
*   p           - the pointer to packet
*   len         - packet length
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
static void printPacket(volatile char *p, UINT16 len)
{
	UINT16 i;
	volatile unsigned char* ch;

	OPL_DMA_DEBUG("        Ram addr = 0x%x , data len = %d \n",(UINT32)p, len);
        
	for(i=0; i<len; i++)
	{
		ch = p+i;
		
		if (0 == (i%16))
		{
		    OPL_DMA_DEBUG("\n	0x%x : ",(UINT32)(p+i));
		}
		OPL_DMA_DEBUG("%02x",*ch);
		OPL_DMA_DEBUG(" ");

		/* Don't print all of the big packet */
		if (i > 0x80) break;
	}

	OPL_DMA_DEBUG("\n\n");
}
#endif





/*
*
* eopl_dma_recv
*
* DESCRIPTION:
*   This function receive packet from opconn dma. It just pass the buffer address
*  to caller, but NOT copy packet to caller. Caller should call the function
*  twice, first time pckCopied = 0, to get the packet buffer address in dma,
*  second time pckCopied = 1, to clear the bd and increase the bd index of soft.
*
* INPUT:
*   pckCopied   - before packet copy (0) or after packet copy (1)
*
* OUTPUT:
*   packet      - pointer to return the received packet buffer address to caller
*
* RETURNS:
*   The packet length.
*
* SEE ALSO:
*/
INT16 eopl_dma_recv(volatile INT8 **packet, UINT8 pckCopied)
{
	#ifdef OPL_DMA0_ETH
		/* bug 3202/3209: if no default 0, cause repeated packets */
		UINT32 length = 0;
	if (pckCopied) 
  {
    return OPL_OK;
  }
  read_from_dma_rxbuf(0, 0, (void *)RX_BUF_ADDR(0), &length);
  *packet = (char *)(RX_BUF_ADDR(0));
	return (INT16)length;
	#else
	UINT32 bdData;
	UINT32 length;
	UINT32 offset;
    
	/* packet copied, clear the bd */
	if (pckCopied) 
	{
		bdData = RX_BUF_BLOCKID(rx_bd_id);
		rx_bd_write(bdData, rx_bd_id);
		rx_inc_bd_idx();
 		return OPL_OK;
	}
    	
	/* Copy new packet out */
	bdData = bd_rx_read_valid((UINT8 *)&rx_bd_id);
	
	if(!(bdData & BD_VALID_BIT)) 
	{
		/* Nothing has been received */
		return -1;
	}

	/* Read packet from dma buffer  */
	length = (bdData & BD_LENGTH_MASK) >> BD_LENGTH_SHIFT;
    
	offset = (bdData & BD_OFFSET_MASK) >> BD_OFFSET_SHIFT;
    
	OPL_DMA_DEBUG("RX : bd=%d, bdData=0x%x, F=%d, Data Len=%d, bufaddr= 0x%x\n",
             rx_bd_id, bdData, (bdData & BD_VALID_BIT) >> BD_VALID_SHIFT, length, RX_BUF_ADDR(rx_bd_id) + offset);

	/* check the dest mac is own mac */
	OPL_DMA_DEBUG("RX : bd=%d, bdData=0x%x, F=%d, Data Len=%d, bufaddr=0x%x\n",
		         rx_bd_id, bdData, (bdData & BD_VALID_BIT)>>BD_VALID_SHIFT, length,	RX_BUF_ADDR(rx_bd_id) + offset);

	read_from_dma_rxbuf(0, rx_bd_id, (void *)RX_BUF_ADDR(rx_bd_id), length);
	
 	/* copy the packet to upper layer */
	*packet = (char *)(RX_BUF_ADDR(rx_bd_id) + (UINT16)offset);
    
		hostCnt.dmaRx ++;
	return (INT16)length;
	#endif
}




/*
*
* eopl_dma_enable
*
* DESCRIPTION:
*   This function enable opconn dma.
*
* INPUT: None
*
* OUTPUT:None
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void eopl_dma_enable(void)
{
	UINT32 regVal;
    
	oplRegRead(DMA_DMAEN, &regVal);
	regVal |= 0xF;
	oplRegWrite(DMA_DMAEN, regVal);
}




/*
*
* eopl_dma_disable
*
* DESCRIPTION:
*   This function disable opconn dma.
*
* INPUT: None
*
* OUTPUT: None
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void eopl_dma_disable(void)
{
	UINT32 regVal;
	
	oplRegRead(DMA_DMAEN, &regVal);
	regVal &= 0xFC;
	oplRegWrite(DMA_DMAEN, regVal);
}




/*
*
* eopl_dma_int_clear
*
* DESCRIPTION:
*   This function opconn clear dma int.
*
* INPUT: None
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_dma_int_clear(void)
{
	UINT32 regVal;
    
	eopl_dma_disable();
    
	eopl_dma_int_disable(OPCONN_DMA_INT_RX | OPCONN_DMA_INT_TX);
	
	/* Read clear */
	oplRegRead(DMA_BDINT, &regVal);
}




/*
*
* eopl_dma_int_pend
*
* DESCRIPTION:
*   This function get opconn dma int pending bits.
*
* INPUT: None
*
* OUTPUT: None
*
* RETURNS:
*   The interrupt value of opconn dma.
*
* SEE ALSO:
*/
UINT32 eopl_dma_int_pend(void)
{
	UINT32 regVal;

	oplRegRead(DMA_BDINT, &regVal);
    
	return regVal;
}



/*
*
* eopl_dma_int_disable
*
* DESCRIPTION:
*   This function disable opconn dma int.
*
* INPUT:
*   dmaIntBit   - the bits of int. such as RX and TX.
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_dma_int_disable(UINT32 dmaIntBit)
{
	UINT32 regVal;
	
	oplRegRead(DMA_BDIE, &regVal);
	regVal &= ~dmaIntBit;  
	oplRegWrite(DMA_BDIE, regVal);
}




/*
*
* eopl_dma_int_enable
*
* DESCRIPTION:
*   This function enable opconn dma int.
*
* INPUT:
*   dmaIntBit   - the bits of int. such as RX and TX.
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void eopl_dma_int_enable(UINT32 dmaIntBit)
{
	UINT32 regVal;
	
	oplRegRead(DMA_BDIE, &regVal);
	regVal |= dmaIntBit;  
	oplRegWrite(DMA_BDIE, regVal);
}





/*
*
* eopl_dma_init
*
* DESCRIPTION:
*   This function init and enable opconn dma.
*
* INPUT: None
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
INT32 eopl_dma_init(void)
{
	#ifdef OPL_DMA0_ETH
	if (!DMA_BuffersPkts)
	{
	  DMA_BuffersPkts = (char *)malloc(OPCONN_DMA_BUF_LEN);
	}
	if (!DMA_BuffersPkts) {
		OPL_DMA_DEBUG(" Unable to allocate buffer for packet.\n");
		return OPL_ERROR;
	}
	else
	{
		OPL_DMA_DEBUG(" Malloc addr 0x%x for packet.\n", (UINT32)DMA_BuffersPkts);
	}
	return OPL_OK;
	#else
	UINT32 RegVal;
	
	OPL_DMA_DEBUG("DEBUG : Enter init dma.\n");
 
	if (!DMA_Buffers) 
	{
		/* for LINUX */
		DMA_Buffers = (char*)malloc_dma_rxbuf(0,(DMABUF_NUM_GEL_RX + DMABUF_NUM_GEL_TX)*OPCONN_DMA_BUF_LEN);
		DMA_BuffersPkts = (char *)malloc(OPCONN_DMA_BUF_LEN);
	}
	else
	{
	    /* have init yet */
		return OPL_OK;
	}
	
	if (!DMA_Buffers) 
	{
		OPL_DMA_DEBUG(" Error : Unable to allocate dma buffer for HOST DMA.\n");
		return OPL_ERROR;
	}
	else
	{
		OPL_DMA_DEBUG(" Malloc addr 0x%x for HOST DMA.\n", (UINT32)DMA_Buffers);
	}

	if (!DMA_BuffersPkts) {
		OPL_DMA_DEBUG(" Unable to allocate buffer for packet.\n");
		return OPL_ERROR;
	}
	else
	{
		OPL_DMA_DEBUG(" Malloc addr 0x%x for packet.\n", (UINT32)DMA_BuffersPkts);
	}
	set_dma_txbuf_addr(0, TX_BUF_ADDR(0));
	
	oplRegWrite(DMA_BASEADDR, (UINT32)DMA_Buffers);

	OPL_DMA_DEBUG("DMA Base =%x  \n", (UINT32)DMA_Buffers);
	   
	/* init TX bd tables */
   	init_bd_tx();
	oplRegRead(DMA_TXBD_POINTER,&RegVal);
   	tx_bd_id = RegVal&0x3F;

	/* init RX bd tables, to be verified */
   	init_bd_rx();
	oplRegRead(DMA_RXBD_POINTER,&RegVal);
	rx_bd_id =  RegVal&0x3F;
	/*enable dma*/
   	eopl_dma_enable();

	OPL_DMA_DEBUG("DEBUG : Init DMA OK.\n");
	
	return OPL_OK;
	#endif
}





/*
*
* eopl_dma_reg_counter_show
*
* DESCRIPTION:
*   This function shows dma register counters.
*
* INPUT: None
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_dma_reg_counter_show(void)
{
	INT32 i =0;
	UINT32 regVal;
	
	OPL_PRINTF("Register                             Counter\n");
	
	while(dma_reg_counters[i].name != NULL)
	{
		oplRegRead(dma_reg_counters[i].reg_addr, &regVal);
		OPL_PRINTF("%-37s %-d \n",  dma_reg_counters[i].name, regVal);
		i++;
	}
}




#ifdef OPCONN_DMA_DRV_DEBUG

#define DMA_BD_RX   0
#define DMA_BD_TX   1

/*
*
* eopl_dma_bd_show
*
* DESCRIPTION:
*   This function shows dma related buffer descriptors.
*
* INPUT: 
*	BDType - 0: RX BD; 1: TX BD.
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_dma_bd_show(UINT32 BDType)
{
	UINT8 i;
	UINT32  bd_val;

	if( DMA_BD_RX == BDType)
	{

		OPL_PRINTF((" Rx BDs: \n"));

		for(i=0;i<DMABUF_NUM_GEL_RX;i++)
		{
			bd_val = rx_bd_read(i);
		
			OPL_PRINTF(" Rx BD %-2d: 0x%08x \n", i, bd_val);
		}
	} else if( DMA_BD_TX == BDType)
	{
	
		OPL_PRINTF((" Tx BDs: \n"));
    
		for(i=0;i<DMABUF_NUM_GEL_TX;i++)
		{
			bd_val = tx_bd_read(i);
			OPL_PRINTF(" Tx BD %-2d: 0x%08x \n", i, bd_val);
		}
	}else
	{
		OPL_PRINTF(" Bad BD Type\n");
	}
}
#endif
