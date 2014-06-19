/*
 * ===============================================================================
 * Header Name: opl_dma.h
 *
 * General Description:
 *    Extern function declarations for Opconn DMA device driver.  
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

#ifndef OPL_DMA_INCLUDE
#define	OPL_DMA_INCLUDE

#include "opl_driver.h"

/* Opconn DMA Debug Switch */
#if 1
#define OPCONN_DMA_DRV_DEBUG
#endif

/* External Function Declarations */
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
extern int eopl_dma_init(void);

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
*   u32_t      - the packet length
*
* SEE ALSO:
*/
extern INT16 eopl_dma_recv(volatile INT8 **packet, UINT8 pckCopied);

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
*   OPL_OK      - send success
*   OPL_ERROR   - send failed
*
* SEE ALSO:
*/
extern INT16 eopl_dma_send(void *packet, UINT16 length);
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
extern void eopl_dma_enable(void);

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
extern void eopl_dma_disable(void);

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
extern void eopl_dma_int_enable(UINT32 dmaIntBit);

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
extern void eopl_dma_int_disable(UINT32 dmaIntBit);

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
extern void eopl_dma_int_clear(void);

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
extern UINT32 eopl_dma_int_pend(void);

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
void eopl_dma_reg_counter_show(void);

#ifdef OPCONN_DMA_DRV_DEBUG

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
void eopl_dma_bd_show(UINT32 BDType);

#endif /* OPCONN_DMA_DRV_DEBUG */

#endif /* OPL_DMA_INCLUDE */
