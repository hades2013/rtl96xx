/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  hal_interrupt.c
*
* DESCRIPTION: 
*	
*
* Date Created: Mar 18, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/src/hal_interrupt.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos.h>
#include <hal.h>
#include <hal_interrupt.h>
//#include <interrupt.h>
#include "opconn_usr_ioctrl.h"
#include "opconn_hw_reg.h"


static VOS_THREAD_t g_pstIntProcessThreadId = NULL;

#define REGADDRESS(x)		((x)*4)
OPL_INT_t oplIntInfo[] =
{
    { OPL_IRQ0_INTR, "IRQ0 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ1_INTR, "IRQ1 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ2_INTR, "IRQ2 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ3_INTR, "IRQ3 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ4_INTR, "IRQ4 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ5_INTR, "IRQ5 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ6_INTR, "IRQ6 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ7_INTR, "IRQ7 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ8_INTR, "IRQ8 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ9_INTR, "IRQ9 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ10_INTR, "IRQ10 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_IRQ11_INTR, "IRQ11 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },	
    { OPL_MISC_INTR, "MISC INTR", OPL_ZERO, OPL_ZERO,
    	{
        	{MISC_DATA_SPACE_FULL_INT_TYPE, "DATA_SPACE_FULL_INT", REGADDRESS(0X5042), OPCONN_BIT(0), REGADDRESS(0X5043), OPCONN_BIT(0), OPL_ZERO, OPL_ZERO},
        	{MISC_TRAF_OVFLW_INT_TYPE, "TRAF_OVFLW_INT", REGADDRESS(0X5042), OPCONN_BIT(1), REGADDRESS(0X5043), OPCONN_BIT(1), OPL_ZERO, OPL_ZERO},
        	{MISC_MPIS_UFLOW_INT_TYPE, "MPIS_UFLOW_INT", REGADDRESS(0X5042), OPCONN_BIT(2), REGADDRESS(0X5043), OPCONN_BIT(2), OPL_ZERO, OPL_ZERO},	
        	{MISC_LL_EMPTY_INT_TYPE, "LL_EMPTY_INT", REGADDRESS(0X5042), OPCONN_BIT(3), REGADDRESS(0X5043), OPCONN_BIT(3), OPL_ZERO, OPL_ZERO},	
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_BRG_INTR, "BRG INTR", OPL_ZERO, OPL_ZERO,
    	{
    		{RX_DIS_DROP_INT_TYPE, "RX_DIS_DROP_INT_EN", REGADDRESS(0X3041), OPCONN_BIT(0), REGADDRESS(0X3040), OPCONN_BIT(0), OPL_ZERO, OPL_ZERO},
        	{TX_DIS_DROP_INT_TYPE, "TX_DIS_DROP_INT_EN", REGADDRESS(0X3041), OPCONN_BIT(1), REGADDRESS(0X3040), OPCONN_BIT(1), OPL_ZERO, OPL_ZERO},
        	{RSV_MAC_DROP_INT_TYPE, "RSV_MAC_DROP_INT_EN", REGADDRESS(0X3041), OPCONN_BIT(2), REGADDRESS(0X3040), OPCONN_BIT(2), OPL_ZERO, OPL_ZERO},
        	{USR_MAC_DROP_INT_TYPE, "USR_MAC_DROP_INT_EN", REGADDRESS(0X3041), OPCONN_BIT(3), REGADDRESS(0X3040), OPCONN_BIT(3), OPL_ZERO, OPL_ZERO},
        	{US_VTT_ERR_INT_TYPE, "US_VTT_ERR_INT", REGADDRESS(0X3041), OPCONN_BIT(4), REGADDRESS(0X3040), OPCONN_BIT(4), OPL_ZERO, OPL_ZERO},
        	{DS_VTT_ERR_INT_TYPE, "DS_VTT_ERR_INT", REGADDRESS(0X3041), OPCONN_BIT(5), REGADDRESS(0X3040), OPCONN_BIT(5), OPL_ZERO, OPL_ZERO},
        	{US_TAG_ERR_INT_TYPE, "US_TAG_ERR_INT", REGADDRESS(0X3041), OPCONN_BIT(6), REGADDRESS(0X3040), OPCONN_BIT(6), OPL_ZERO, OPL_ZERO},
        	{DS_TAG_ERR_INT_TYPE, "US_TAG_ERR_INT", REGADDRESS(0X3041), OPCONN_BIT(7), REGADDRESS(0X3040), OPCONN_BIT(7), OPL_ZERO, OPL_ZERO},
        	{US_ARL_SMAC_CFL_INT_TYPE, "US_ARL_SMAC_CFL_INT", REGADDRESS(0X3041), OPCONN_BIT(8), REGADDRESS(0X3040), OPCONN_BIT(8), OPL_ZERO, OPL_ZERO},
        	{DS_ARL_SMAC_CFL_INT_TYPE, "DS_ARL_SMAC_CFL_INT", REGADDRESS(0X3041), OPCONN_BIT(9), REGADDRESS(0X3040), OPCONN_BIT(9), OPL_ZERO, OPL_ZERO},
        	{US_ZERO_DA_DROP_INT_TYPE, "US_ZERO_DA_DROP_INT", REGADDRESS(0X3041), OPCONN_BIT(10), REGADDRESS(0X3040), OPCONN_BIT(10), OPL_ZERO, OPL_ZERO},
        	{DS_ZERO_DA_DROP_INT_TYPE, "DS_ZERO_DA_DROP_INT", REGADDRESS(0X3041), OPCONN_BIT(11), REGADDRESS(0X3040), OPCONN_BIT(11), OPL_ZERO, OPL_ZERO},
        	{ARL_FULL_ARL_NEW_REQ_INT_TYPE, "ARL_FULL/ARL_NEW_REQ_INT_EN", REGADDRESS(0X3041), OPCONN_BIT(21), REGADDRESS(0X3040), OPCONN_BIT(21), OPL_ZERO, OPL_ZERO},
        	{US_NON_UCST_DROP_INT_TYPE, "US_NON_UCST_DROP_INT", REGADDRESS(0X3041), OPCONN_BIT(23), REGADDRESS(0X3040), OPCONN_BIT(23), OPL_ZERO, OPL_ZERO},
        	{DS_NON_UCST_DROP_INT_TYPE, "US_NON_UCST_DROP_INT", REGADDRESS(0X3041), OPCONN_BIT(24), REGADDRESS(0X3040), OPCONN_BIT(24), OPL_ZERO, OPL_ZERO},
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },

    { OPL_UPTM_INTR, "UPTM INTR", OPL_ZERO, OPL_ZERO,
    	{
            {TMUS_QUEUE_FULL_INT_TYPE, "US_QUEUE_FULL_INT", REGADDRESS(0X2003), OPCONN_BIT(2), REGADDRESS(0x2005), OPCONN_BIT(2), OPL_ZERO, OPL_ZERO},
            {TMUS_RD_ERR_INT_TYPE, "US_RD_ERR_INT", REGADDRESS(0X2003), OPCONN_BIT(3), REGADDRESS(	0x2005), OPCONN_BIT(3), OPL_ZERO, OPL_ZERO},
            {TMUS_PKT_POS_ERR_INT_TYPE, "US_PKT_POS_ERR_INT", REGADDRESS(0X2003), OPCONN_BIT(4), REGADDRESS(0x2005), OPCONN_BIT(4), OPL_ZERO, OPL_ZERO},
            {TMUS_PKT_LEN_ERR_INT_TYPE, "US_PKT_LEN_ERR_INT", REGADDRESS(0X2003), OPCONN_BIT(5), REGADDRESS(0x2005), OPCONN_BIT(5), OPL_ZERO, OPL_ZERO},
            {TMUS_LL_FULL_INT_TYPE, "US_LL_FULL_INT", REGADDRESS(0X2003), OPCONN_BIT(6), REGADDRESS(0x2005), OPCONN_BIT(6), OPL_ZERO, OPL_ZERO},
            {TMUS_RPT_ERR_INT_TYPE, "US_RPT_ERR_INT", REGADDRESS(0X2003), OPCONN_BIT(7), REGADDRESS(0x2005), OPCONN_BIT(7), OPL_ZERO, OPL_ZERO},
            {TM_HEAD_DISORDER_INT_TYPE, "HEAD_DISORDER_INT", REGADDRESS(0X2003), OPCONN_BIT(8), REGADDRESS(0x2005), OPCONN_BIT(8), OPL_ZERO, OPL_ZERO},
            {TM_HEAD_OVFLW_INT_TYPE, "HEAD_OVFLW_INT", REGADDRESS(0X2003), OPCONN_BIT(9), REGADDRESS(0x2005), OPCONN_BIT(9), OPL_ZERO, OPL_ZERO},
            {TM_MPCP_SEND_FAIL_ERR_INT_TYPE, "MPCP_SEND_FAIL_ERR_INT", REGADDRESS(0X2003), OPCONN_BIT(10), REGADDRESS(0x2005), OPCONN_BIT(10), OPL_ZERO, OPL_ZERO},
            {TM_MPCP_SEND_OK_ERR_INT_TYPE, "MPCP_SEND_OK_ERR_INT", REGADDRESS(0X2003), OPCONN_BIT(11), REGADDRESS(0x2005), OPCONN_BIT(11), OPL_ZERO, OPL_ZERO},
            {-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },

    { OPL_DNTM_INTR, "DNTM INTR", OPL_ZERO, OPL_ZERO,
    	{
            {TMDS_QUEUE_FULL_INT_TYPE, "DS_QUEUE_FULL_INT", REGADDRESS(0X2004), OPCONN_BIT(2), REGADDRESS(0X2006), OPCONN_BIT(2), OPL_ZERO, OPL_ZERO},
            {TMDS_RD_ERR_INT_TYPE, "DS_RD_ERR_INT", REGADDRESS(0X2004), OPCONN_BIT(3), REGADDRESS(0X2006), OPCONN_BIT(3), OPL_ZERO, OPL_ZERO},
            {TMDS_PKT_POS_ERR_INT_TYPE, "DS_PKT_POS_ERR_INT", REGADDRESS(0X2004), OPCONN_BIT(4), REGADDRESS(0X2006), OPCONN_BIT(4), OPL_ZERO, OPL_ZERO},
            {TMDS_PKT_LEN_ERR_INT_TYPE, "DS_PKT_LEN_ERR_INT", REGADDRESS(0X2004), OPCONN_BIT(5), REGADDRESS(0X2006), OPCONN_BIT(5), OPL_ZERO, OPL_ZERO},
            {TMDS_LL_FULL_INT_TYPE, "DS_LL_FULL_INT", REGADDRESS(0X2004), OPCONN_BIT(6), REGADDRESS(0X2006), OPCONN_BIT(6), OPL_ZERO, OPL_ZERO},
            {-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },

    { OPL_MPCP_INTR, "MPCP INTR", OPL_ZERO, OPL_ZERO,
    	{
            {MPCP_STATE_INT_TYPE, "STATE_INT", REGADDRESS(0X4001), OPCONN_BIT(0), REGADDRESS(0X4000),OPCONN_BIT(0), OPL_ZERO, OPL_ZERO},
            {MPCP_GATE_DISORDER_INT_TYPE, "GATE_DISORDER_INT", REGADDRESS(0X4001), OPCONN_BIT(1), REGADDRESS(0X4000),OPCONN_BIT(1), OPL_ZERO, OPL_ZERO},
            {MPCP_ECHO_PENDING_ERR_INT_TYPE, "ECHO_PENDING_ERR_INT", REGADDRESS(0X4001), OPCONN_BIT(2), REGADDRESS(0X4000),OPCONN_BIT(2),	OPL_ZERO, OPL_ZERO},
            {MPCP_STATE_CHANGE_INT_TYPE, "STATE_CHANGE_INT", REGADDRESS(0X4001), OPCONN_BIT(4), REGADDRESS(0X4000),OPCONN_BIT(4), OPL_ZERO, OPL_ZERO},
            {MPCP_GRANT_LIST_FULL_INT_TYPE, "GRANT_LIST_FULL_INT", REGADDRESS(0X4001), OPCONN_BIT(5), REGADDRESS(0X4000),OPCONN_BIT(5), OPL_ZERO, OPL_ZERO},
            {MPCP_GATE_TIMEOUT_INT_TYPE, "GATE_TIMEOUT_INT", REGADDRESS(0X4001), OPCONN_BIT(6), REGADDRESS(0X4000),OPCONN_BIT(6), OPL_ZERO, OPL_ZERO},
            {MPCP_LINK_UP_INT_TYPE, "LINK_UP_INT", REGADDRESS(0X4001), OPCONN_BIT(8), REGADDRESS(0X4000),OPCONN_BIT(8), OPL_ZERO, OPL_ZERO},
            {MPCP_LINK_DOWN_INT_TYPE, "LINK_DOWN_INT", REGADDRESS(0X4001), OPCONN_BIT(9), REGADDRESS(0X4000),OPCONN_BIT(9), OPL_ZERO, OPL_ZERO},
            {-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_PMAC_INTR, "PMAC INTR", OPL_ZERO, OPL_ZERO,
    	{
            {SIGNAL_VAL_INT_TYPE, "SIGNAL_VAL_INT", REGADDRESS(0x0A1A), OPCONN_BIT(16), REGADDRESS(0x0A1B), OPCONN_BIT(16), OPL_ZERO, OPL_ZERO},
            {SYNC_ERR_RX_INT_TYPE, "SYNC_ERR_RX_INT", REGADDRESS(0x0A1A), OPCONN_BIT(17), REGADDRESS(0x0A1B), OPCONN_BIT(17), OPL_ZERO, OPL_ZERO},
            {SYNC_ERR_TX_INT_TYPE, "SYNC_ERR_TX_INT", REGADDRESS(0x0A1A), OPCONN_BIT(16), REGADDRESS(0x0A1B), OPCONN_BIT(16), OPL_ZERO, OPL_ZERO},
            {RXPLL_LCK_CHANGE_INT_TYPE, "RXPLL_LCK_CHANGE",REGADDRESS(0x0A1A), OPCONN_BIT(19), REGADDRESS(0x0A1B), OPCONN_BIT(19), OPL_ZERO, OPL_ZERO},
            {TXPLL_LCK_CHANGE_INT_TYPE, "TXPLL_LCK_CHANGE",REGADDRESS(0x0A1A), OPCONN_BIT(20), REGADDRESS(0x0A1B), OPCONN_BIT(20), OPL_ZERO, OPL_ZERO},
            {LINK_CHANGE_INT_TYPE, "LINK_CHANGE_INT", REGADDRESS(0x0A1A), OPCONN_BIT(29), REGADDRESS(0x0A1B), OPCONN_BIT(29), OPL_ZERO, OPL_ZERO},
            {CRC8_ERR_INT_TYPE, "CRC8_ERR_INT", REGADDRESS(0x0A1A), OPCONN_BIT(30), REGADDRESS(0x0A1B), OPCONN_BIT(30), OPL_ZERO, OPL_ZERO},
            {LOS_IND_INT_TYPE, "LOS_IND_INT", REGADDRESS(0x0A1A), OPCONN_BIT(31), REGADDRESS(0x0A1B), OPCONN_BIT(31), OPL_ZERO, OPL_ZERO},
            {-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO} /*last one*/
        }
    },
    { OPL_GE_INTR, "GE INTR", OPL_ZERO, OPL_ZERO,
    	{
            {0, "FIFO_FULL_INT", REGADDRESS(0x8010), OPCONN_BIT(0), REGADDRESS(0x8011), OPCONN_BIT(0), OPL_ZERO, OPL_ZERO},
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_PCM_INTR, "PCM INTR", OPL_ZERO, OPL_ZERO,
    	{
    		{TX_FIFO_FULL_ERR_INT_TYPE, "TX_FIFO_FULL_ERR_INT", REGADDRESS(0X0404), OPCONN_BIT(0), REGADDRESS(0X0405), OPCONN_BIT(0), OPL_ZERO, OPL_ZERO},
        	{TX_DIS_DROP_INT_TYPE, "TX_DIS_DROP_INT", REGADDRESS(0X0404), OPCONN_BIT(1), REGADDRESS(0X0405), OPCONN_BIT(1), OPL_ZERO, OPL_ZERO},
        	{RX_FIFO_FULL_ERR_INT_TYPE, "RX_FIFO_FULL_ERR_INT", REGADDRESS(0X0404), OPCONN_BIT(2), REGADDRESS(0X0405), OPCONN_BIT(2), OPL_ZERO, OPL_ZERO},
        	{RX_FIFO_EMPTY_ERR_INT_TYPE, "RX_FIFO_EMPTY_ERR_INT", REGADDRESS(0X0404), OPCONN_BIT(3), REGADDRESS(0X0405), OPCONN_BIT(3), OPL_ZERO, OPL_ZERO},
        	{TX_FIFO_ALMOST_EMPTY_INT_TYPE, "TX_FIFO_ALMOST_EMPTY_INT", REGADDRESS(0X0404), OPCONN_BIT(4), REGADDRESS(0X0405), OPCONN_BIT(4), OPL_ZERO, OPL_ZERO},
        	{RX_FIFO_ALMOST_FULL_INT_TYPE, "RX_FIFO_ALMOST_FULL_INT", REGADDRESS(0X0404), OPCONN_BIT(5), REGADDRESS(0X0405), OPCONN_BIT(5), OPL_ZERO, OPL_ZERO},
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_DMA0_INTR, "DMA0 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_DMA1_INTR, "DMA1 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_SPI_INTR, "SPI INTR", OPL_ZERO, OPL_ZERO,
    	{
    		{0, "SPI_INT", REGADDRESS(0x030D), OPCONN_BIT(0), REGADDRESS(0), OPCONN_BIT(0), OPL_ZERO, OPL_ZERO},
        	{1, "CFG_ERR_INT", REGADDRESS(0x030D), OPCONN_BIT(1), REGADDRESS(0), OPCONN_BIT(1), OPL_ZERO, OPL_ZERO},
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_WDOG_INTR, "WDOG INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_RTC_INTR, "RTC INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_TIMER0_INTR, "TIMER0 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_TIMER1_INTR, "TIMER1 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_TIMER2_INTR, "TIMER2 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_TIMER3_INTR, "TIMER3 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_I2C_INTR, "I2C INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_UART0_INTR, "UART0 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { OPL_UART1_INTR, "UART1 INTR", OPL_ZERO, OPL_ZERO,
    	{
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
    	}
    },
    { -1, OPL_NULL, OPL_ZERO, OPL_ZERO,
        {
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
        }
    }
};

OPL_INT_t oplIntInfoForShow[] =
{
	{ 0,    "PMAC INTR",  OPL_ZERO, OPL_ZERO,
       	{
            {16, "SIGNAL_VAL",      REGADDRESS(0x0A1A), OPCONN_BIT(16), REGADDRESS(0x0A1B), OPCONN_BIT(16), OPL_ZERO, OPL_ZERO},
            {17, "KCODE_CHANGE",    REGADDRESS(0x0A1A), OPCONN_BIT(17), REGADDRESS(0x0A1B), OPCONN_BIT(17), OPL_ZERO, OPL_ZERO},
            {18, "KCODE_ERR",       REGADDRESS(0x0A1A), OPCONN_BIT(18), REGADDRESS(0x0A1B), OPCONN_BIT(18), OPL_ZERO, OPL_ZERO},
            {19, "RXPLL_LCK_CHANGE",REGADDRESS(0x0A1A), OPCONN_BIT(19), REGADDRESS(0x0A1B), OPCONN_BIT(19), OPL_ZERO, OPL_ZERO},
            {20, "TXPLL_LCK_CHANGE",REGADDRESS(0x0A1A), OPCONN_BIT(20), REGADDRESS(0x0A1B), OPCONN_BIT(20), OPL_ZERO, OPL_ZERO},

            {29, "LINK_CHANGE_INT", REGADDRESS(0x0A1A), OPCONN_BIT(29), REGADDRESS(0x0A1B), OPCONN_BIT(29), OPL_ZERO, OPL_ZERO},
            {30, "CRC8_ERR_INT",    REGADDRESS(0x0A1A), OPCONN_BIT(30), REGADDRESS(0x0A1B), OPCONN_BIT(30), OPL_ZERO, OPL_ZERO},
            {31, "LOS_IND_INT",     REGADDRESS(0x0A1A), OPCONN_BIT(31), REGADDRESS(0x0A1B), OPCONN_BIT(31), OPL_ZERO, OPL_ZERO},
           	{12345, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO} /*last one*/
        }
     },
     { 1,    "MISC INTR",  OPL_ZERO, OPL_ZERO,
       	{
			{0, 	"DATA_SPACE_FULL_INT", 	    REGADDRESS(0X5042), OPCONN_BIT(0), REGADDRESS(0X5043),		OPCONN_BIT(0), 		OPL_ZERO, OPL_ZERO},
			{1, 	"TRAFFIC_OFLOW_INT", 	    REGADDRESS(0X5042), OPCONN_BIT(1), REGADDRESS(0X5043),		OPCONN_BIT(1), 		OPL_ZERO, OPL_ZERO},
			{2,	 	"MIPS_UFLOW_INT", 		    REGADDRESS(0X5042), OPCONN_BIT(2), REGADDRESS(0X5043),		OPCONN_BIT(2), 		OPL_ZERO, OPL_ZERO},	
			{3,	    "LL_EMPTY_INT", 			REGADDRESS(0X5042), OPCONN_BIT(3), REGADDRESS(0X5043),		OPCONN_BIT(3), 		OPL_ZERO, OPL_ZERO},	
         	{12345, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
       	}
     },
     { 2,    "BRG INTR",  OPL_ZERO, OPL_ZERO,
       	{
       		{0,     "RX_DIS_DROP_INT", 			REGADDRESS(0X3041), OPCONN_BIT(0), 	REGADDRESS(0X3040),		OPCONN_BIT(0), 		OPL_ZERO, OPL_ZERO},
			{1, 	"TX_DIS_DROP_INT", 			REGADDRESS(0X3041), OPCONN_BIT(1), 	REGADDRESS(0X3040),		OPCONN_BIT(1), 		OPL_ZERO, OPL_ZERO},
			{2, 	"RSV_MAC_DROP_INT", 		REGADDRESS(0X3041), OPCONN_BIT(2), 	REGADDRESS(0X3040),		OPCONN_BIT(2), 		OPL_ZERO, OPL_ZERO},
			{3,		"USER_MAC_DROP_INT", 		REGADDRESS(0X3041), OPCONN_BIT(3), 	REGADDRESS(0X3040),		OPCONN_BIT(3), 		OPL_ZERO, OPL_ZERO},
			{4, 	"US_VTT_ERR_INT", 			REGADDRESS(0X3041), OPCONN_BIT(4), 	REGADDRESS(0X3040),		OPCONN_BIT(4), 		OPL_ZERO, OPL_ZERO},
			{5, 	"DS_VTT_ERR_INT", 			REGADDRESS(0X3041), OPCONN_BIT(5), 	REGADDRESS(0X3040),		OPCONN_BIT(5), 		OPL_ZERO, OPL_ZERO},
			{6, 	"US_TAG_ERR_INT", 			REGADDRESS(0X3041), OPCONN_BIT(6), 	REGADDRESS(0X3040),		OPCONN_BIT(6), 		OPL_ZERO, OPL_ZERO},
			{7, 	"DS_TAG_ERR_INT", 			REGADDRESS(0X3041), OPCONN_BIT(7), 	REGADDRESS(0X3040),		OPCONN_BIT(7), 		OPL_ZERO, OPL_ZERO},
			{8, 	"US_ARL_SMAC_CFL_INT", 	    REGADDRESS(0X3041), OPCONN_BIT(8),  REGADDRESS(0X3040),		OPCONN_BIT(8), 	    OPL_ZERO, OPL_ZERO},
			{9, 	"DS_ARL_SMAC_CFL_INT", 		REGADDRESS(0X3041), OPCONN_BIT(9),  REGADDRESS(0X3040),		OPCONN_BIT(9), 	    OPL_ZERO, OPL_ZERO},
            {10, 	"US_ZERO_DA_DROP_INT", 		REGADDRESS(0X3041), OPCONN_BIT(10), REGADDRESS(0X3040),		OPCONN_BIT(10), 	OPL_ZERO, OPL_ZERO},
			{11, 	"DS_ZERO_DA_DROP_INT", 		REGADDRESS(0X3041), OPCONN_BIT(11), REGADDRESS(0X3040),		OPCONN_BIT(11), 	OPL_ZERO, OPL_ZERO},

            {21, 	"ARL_FULL/ARL_NEW_REQ_INT", REGADDRESS(0X3041), OPCONN_BIT(21), REGADDRESS(0X3040),		OPCONN_BIT(21), 	OPL_ZERO, OPL_ZERO},

            {23,	"US_NON_UCST_DROP_INT", 	REGADDRESS(0X3041), OPCONN_BIT(23), REGADDRESS(0X3040),		OPCONN_BIT(23), 	OPL_ZERO, OPL_ZERO},
			{24, 	"DS_NON_UCST_DROP_INT", 	REGADDRESS(0X3041), OPCONN_BIT(24), REGADDRESS(0X3040),		OPCONN_BIT(24), 	OPL_ZERO, OPL_ZERO},

            {12345, 															(UINT8 *)OPL_NULL, 								OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
       	}
     },

  	{ 3,    "UPTM INTR",  OPL_ZERO, OPL_ZERO,
       	{
            {2, 	"US_QUEUE_FULL_INT", 		REGADDRESS(0X2003), 		OPCONN_BIT(2), REGADDRESS(0x2005),		OPCONN_BIT(2), 	OPL_ZERO, OPL_ZERO},
			{3, 	"US_RD_ERR_INT", 			REGADDRESS(0X2003), 		OPCONN_BIT(3), REGADDRESS(0x2005),		OPCONN_BIT(3), 	OPL_ZERO, OPL_ZERO},
	 		{4, 	"US_PKT_POS_ERR_INT", 		REGADDRESS(0X2003), 		OPCONN_BIT(4), REGADDRESS(0x2005),		OPCONN_BIT(4), 	OPL_ZERO, OPL_ZERO},
	 		{5, 	"US_PKT_LEN_ERR_INT", 		REGADDRESS(0X2003), 		OPCONN_BIT(5), REGADDRESS(0x2005),		OPCONN_BIT(5), 	OPL_ZERO, OPL_ZERO},
		 	{6, 	"US_LL_FULL_INT", 			REGADDRESS(0X2003), 		OPCONN_BIT(6), REGADDRESS(0x2005),		OPCONN_BIT(6), 	OPL_ZERO, OPL_ZERO},
            {7, 	"TM_RPT_ERR_INT", 			REGADDRESS(0X2003), 		OPCONN_BIT(7), REGADDRESS(0x2005),		OPCONN_BIT(7), 	OPL_ZERO, OPL_ZERO},
			{8, 	"HEAD_DISODR_INT", 			REGADDRESS(0X2003), 		OPCONN_BIT(8), REGADDRESS(0x2005),		OPCONN_BIT(8), 	OPL_ZERO, OPL_ZERO},
	 		{9, 	"HEAD_OVFLW_INT", 		    REGADDRESS(0X2003), 		OPCONN_BIT(9), REGADDRESS(0x2005),		OPCONN_BIT(9), 	OPL_ZERO, OPL_ZERO},
	 		{10, 	"MPCP_SEND_FAIL_ERR_INT", 	REGADDRESS(0X2003), 		OPCONN_BIT(10),REGADDRESS(0x2005),		OPCONN_BIT(10), OPL_ZERO, OPL_ZERO},
		 	{11, 	"MPCP_SEND_OK_ERR_INT", 	REGADDRESS(0X2003), 		OPCONN_BIT(11),REGADDRESS(0x2005),		OPCONN_BIT(11), OPL_ZERO, OPL_ZERO},

            {12345, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
       	}
     },

     { 4,    "DNTM INTR",  OPL_ZERO, OPL_ZERO,
       	{
       		{2, 	"DS_QUEUE_FULL_INT", 	REGADDRESS(0X2004), 		OPCONN_BIT(2), 	REGADDRESS(0X2006),		OPCONN_BIT(2),	OPL_ZERO, OPL_ZERO},
			{3, 	"DS_RD_ERR_INT", 		REGADDRESS(0X2004), 		OPCONN_BIT(3), 	REGADDRESS(0X2006),		OPCONN_BIT(3),	OPL_ZERO, OPL_ZERO},
			{4, 	"DS_PKT_POS_ERR_INT", 	REGADDRESS(0X2004), 		OPCONN_BIT(4), 	REGADDRESS(0X2006),		OPCONN_BIT(4),	OPL_ZERO, OPL_ZERO},
			{5, 	"DS_PKT_LEN_ERR_INT", 	REGADDRESS(0X2004), 		OPCONN_BIT(5), 	REGADDRESS(0X2006),		OPCONN_BIT(5),	OPL_ZERO, OPL_ZERO},
			{6, 	"DS_LL_FULL_INT", 		REGADDRESS(0X2004), 		OPCONN_BIT(6), 	REGADDRESS(0X2006),		OPCONN_BIT(6),	OPL_ZERO, OPL_ZERO},
            {12345, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
       	}
     },

     { 5,    "MPCP INTR",  OPL_ZERO, OPL_ZERO,
       	{
     		{0, 	"STATE_INT", 			REGADDRESS(0X4001), OPCONN_BIT(0), REGADDRESS(0X4000),OPCONN_BIT(0),	OPL_ZERO, OPL_ZERO},
			{1, 	"GATE_DIS_INT", 		REGADDRESS(0X4001), OPCONN_BIT(1), REGADDRESS(0X4000),OPCONN_BIT(1),	OPL_ZERO, OPL_ZERO},
			{2, 	"ECHO_PEND_ERR_INT", 	REGADDRESS(0X4001), OPCONN_BIT(2), REGADDRESS(0X4000),OPCONN_BIT(2),	OPL_ZERO, OPL_ZERO},
			{3, 	"MPCP_TIME_DRIFT_INT", 	REGADDRESS(0X4001), OPCONN_BIT(3), REGADDRESS(0X4000),OPCONN_BIT(3),	OPL_ZERO, OPL_ZERO},
			{4,		"STATE_CHG_INT",		REGADDRESS(0X4001), OPCONN_BIT(4), REGADDRESS(0X4000),OPCONN_BIT(4),	OPL_ZERO, OPL_ZERO},
			{5,		"GRANT_LIST_FULL_INT",	REGADDRESS(0X4001), OPCONN_BIT(5), REGADDRESS(0X4000),OPCONN_BIT(5),	OPL_ZERO, OPL_ZERO},
			{6,		"MPCP_GATE_TIMEOUT_INT",REGADDRESS(0X4001), OPCONN_BIT(6), REGADDRESS(0X4000),OPCONN_BIT(6),	OPL_ZERO, OPL_ZERO},
            
			{8,		"LINK_UP_INT",			REGADDRESS(0X4001), OPCONN_BIT(8), REGADDRESS(0X4000),OPCONN_BIT(8),	OPL_ZERO, OPL_ZERO},
			{9,		"LINK_DN_INT",			REGADDRESS(0X4001), OPCONN_BIT(9), REGADDRESS(0X4000),OPCONN_BIT(9),	OPL_ZERO, OPL_ZERO},
            {12345, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
       	}
     },

     { 6,    "MARB INTR",  OPL_ZERO, OPL_ZERO,
       	{
     		{0, 	"DATA_SPACE_FULL_INT",  REGADDRESS(0X5042), OPCONN_BIT(0), REGADDRESS(0X5043),OPCONN_BIT(0),	OPL_ZERO, OPL_ZERO},
			{1, 	"TRAFFIC_OFLOW_INT", 	REGADDRESS(0X5042), OPCONN_BIT(1), REGADDRESS(0X5043),OPCONN_BIT(1),	OPL_ZERO, OPL_ZERO},
			{2, 	"MIPS_UFLOW_INT", 	    REGADDRESS(0X5042), OPCONN_BIT(2), REGADDRESS(0X5043),OPCONN_BIT(2),	OPL_ZERO, OPL_ZERO},
			{3, 	"LL_EMPTY_INT", 	    REGADDRESS(0X5042), OPCONN_BIT(3), REGADDRESS(0X5043),OPCONN_BIT(3),	OPL_ZERO, OPL_ZERO},
			{12345, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
       	}
     },


     { 12345,   OPL_NULL,  OPL_ZERO,  OPL_ZERO,
       {
         	{-1, (UINT8 *)OPL_NULL, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO, OPL_ZERO}/* last one */
       }
     }
};

OPL_STATUS intStatusShow(void)
{
    UINT32 rowIndex = 0;
    UINT32 colIndex = 0;
    UINT32 intStatusAddr;
    UINT32 intStatus;
    
    while(oplIntInfoForShow[colIndex].id != 12345)
    {
        intStatusAddr = oplIntInfoForShow[colIndex].level2Int[0].penAddr;
        oplRegRead(intStatusAddr,&intStatus);

        rowIndex = 0;
        OPL_DRV_PRINTF(("%-20s.addr = 0x%04x:: 0x%08x\n",
            oplIntInfoForShow[colIndex].name,
            intStatusAddr/4,
            intStatus));
        while(oplIntInfoForShow[colIndex].level2Int[rowIndex].id != 12345)
        {
            if(intStatus&oplIntInfoForShow[colIndex].level2Int[rowIndex].penBit)
            {
                OPL_DRV_PRINTF(("%s.%15s:   1\n",
                    oplIntInfoForShow[colIndex].name,
                    oplIntInfoForShow[colIndex].level2Int[rowIndex].name));
            }
            rowIndex++;
        }
        colIndex++;
    }
	return OPL_OK;
}

/*******************************************************************************
*
* interruptLevel1Enable - enable or disable level 1 interrupts
*
* DESCRIPTION:
* 	enable or disable level 1 interrupts.
*
* INPUT:
*	ulModule    - level 1 interrupt: 0 ~ 32, 32 enable or disable all level 1 interrupts
*	ulEnable    - 0 or 1, 0 to disable; 1 to enbale:
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
OPL_STATUS interruptLevel1Enable (UINT32 ulModule, UINT32 ulEnable)
{
    int nFrom;
    char * pCaption[] = {"Disable", "Enable", OPL_NULL};
    
    /* enable or disable level 1 interrupt here */
    if (ulModule >= OPL_END_INTR)
    {
        for (nFrom = OPL_ZERO; nFrom < OPL_END_INTR; nFrom++)
        {
            if (OPL_ERROR == oplIntInfo[nFrom].id)
            {
                OPL_DRV_PRINTF (("bad level 1 id %d.\n", nFrom));
                break;
            }

            if (ulEnable)
            {
                oplRegFieldWrite(INTC_PENDING_EN, nFrom, 1, 1);
            }
            else {
                oplRegFieldWrite(INTC_PENDING_EN, nFrom, 1, 0);
            }
            OPL_DRV_PRINTF (("%s level 1 interrupt(%s, id=%d), reg=%#x, bit=%#x.\n", 
                pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], oplIntInfo[nFrom].name, 
                oplIntInfo[nFrom].id, INTC_PENDING_EN, OPCONN_BIT(nFrom)));
        }
    }
    else {
        if (OPL_ERROR == oplIntInfo[ulModule].id)
        {
            OPL_DRV_PRINTF (("bad level 1 id %d.\n", ulModule));
            return OPL_ERR_INVALID_PARAMETERS;
        }

        if (ulEnable)
        {
            oplRegFieldWrite(INTC_PENDING_EN, ulModule, 1, 1);
        }
        else {
            oplRegFieldWrite(INTC_PENDING_EN, ulModule, 1, 0);
        }
        OPL_DRV_PRINTF (("%s level 1 interrupt(%s, id=%d), reg=%#x, bit=%#x.\n", 
            pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], oplIntInfo[ulModule].name, 
            oplIntInfo[ulModule].id, INTC_PENDING_EN, OPCONN_BIT(ulModule)));
    }

    return OPL_OK;
}

/*******************************************************************************
*
* interruptLevel1EnableState - get level 1 interrupt state
*
* DESCRIPTION:
* 	get level 1 interrupt state
*
* INPUT:
*	ulModule	- level 1 interrupt: 0 ~ 31
*	pulEnable	- pointer to destination value, 0 or 1, 0 to disable; 1 to enbale
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
OPL_STATUS interruptLevel1EnableState (UINT32 ulModule, UINT32 *pulEnable)
{
    if (NULL == pulEnable)
    {
        OPL_DRV_PRINTF (("null pointer pulEnable.\n"));
        return OPL_ERR_NULL_POINTER;
    }

    if (ulModule >= OPL_END_INTR)
    {
        OPL_DRV_PRINTF (("bad level 1 id %d.\n", ulModule));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_ERROR == oplIntInfo[ulModule].id)
    {
        OPL_DRV_PRINTF (("bad level 1 id %d.\n", ulModule));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    return oplRegFieldRead(INTC_PENDING_EN, ulModule, 1, pulEnable);
}



/*******************************************************************************
*
* interruptLevel2Enable - enable or disable level 2 interrupts
*
* DESCRIPTION:
* 	enable or disable level 2 interrupts.
*
* INPUT:
*	ulModule	- level 1 interrupt: 0 ~ 32, 32 enable or disable all level1 interrupts
*   ulSubModule - level 2 interrupt: 0 ~ 32, 32 enable or disable all level2 interrupts in the module
*	ulEnable	- 0 or 1, 0 to disable; 1 to enbale:
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
OPL_STATUS interruptLevel2Enable (UINT32 ulModule, UINT32 ulSubModule, UINT32 ulEnable)
{
    int nFrom, nSubFrom, nCount;
    char * pCaption[] = {"Disable", "Enable", OPL_NULL};
    
    /* enable or disable level1 interrupt here */
    if (ulModule >= OPL_END_INTR)
    {
        for (nFrom = OPL_ZERO; nFrom < OPL_END_INTR; nFrom++)
        {
            if (OPL_ERROR == oplIntInfo[nFrom].id)
            {
                OPL_DRV_PRINTF (("bad level 1 id %d.\n", nFrom));
                return OPL_ERR_INVALID_PARAMETERS;
            }

            if (OPL_MAX_NUM_OF_INTR_LEVEL2 <= ulSubModule)
            {
                /* enable or disable level2 interrupts here */
                OPL_DRV_PRINTF (("%s %s(id=%d) level2 interrupts:\n", 
                    pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], oplIntInfo[nFrom].name, oplIntInfo[nFrom].id));

                nCount = 0;
                for (nSubFrom = OPL_ZERO; nSubFrom < OPL_MAX_NUM_OF_INTR_LEVEL2; nSubFrom++)
                {
                    if (OPL_ERROR != oplIntInfo[nFrom].level2Int[nSubFrom].id && 
                        OPL_NULL != oplIntInfo[nFrom].level2Int[nSubFrom].enAddr)
                    {
                        if (ulEnable)
                        {
                            oplRegBitSet(oplIntInfo[nFrom].level2Int[nSubFrom].enAddr, 
                                oplIntInfo[nFrom].level2Int[nSubFrom].enBit);
                        }
                        else {
                            oplRegBitClear(oplIntInfo[nFrom].level2Int[nSubFrom].enAddr, 
                                oplIntInfo[nFrom].level2Int[nSubFrom].enBit);
                        }
                        OPL_DRV_PRINTF (("\t%s level2 interrupt %s, reg=%#x,bit=%#x:\n", 
                            pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], 
                            oplIntInfo[nFrom].level2Int[nSubFrom].name, 
                            oplIntInfo[nFrom].level2Int[nSubFrom].enAddr, 
                            oplIntInfo[nFrom].level2Int[nSubFrom].enBit));
                        nCount++;
                    }
                    else {
                        break;
                    }
                }
                if (OPL_ZERO == nCount)
                {
                    OPL_DRV_PRINTF (("\tlevel2 interrupt has no Enable-register.\n"));
                }
            }
            else {
                /* enable or disable level2 interrupts here */
                OPL_DRV_PRINTF (("%s %s(id=%d) level2 interrupts:\n", 
                    pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], oplIntInfo[ulModule].name, oplIntInfo[ulModule].id));
                if (OPL_ERROR == oplIntInfo[ulModule].level2Int[ulSubModule].id)
                {
                    OPL_DRV_PRINTF (("\tbad level 2 id %d.\n", ulSubModule));
                    continue;
                }
                
                if (ulEnable)
                {
                    oplRegBitSet(oplIntInfo[ulModule].level2Int[ulSubModule].enAddr, 
                        oplIntInfo[ulModule].level2Int[ulSubModule].enBit);
                }
                else {
                    oplRegBitClear(oplIntInfo[ulModule].level2Int[ulSubModule].enAddr, 
                        oplIntInfo[ulModule].level2Int[ulSubModule].enBit);
                }
                OPL_DRV_PRINTF (("\t%s level2 interrupt %s, reg=%#x,bit=%#x:\n", 
                    pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], 
                    oplIntInfo[ulModule].level2Int[ulSubModule].name, 
                    oplIntInfo[ulModule].level2Int[ulSubModule].enAddr, 
                    oplIntInfo[ulModule].level2Int[ulSubModule].enBit));
            }
        }
    }
    else {
        if (OPL_ERROR == oplIntInfo[ulModule].id)
        {
            OPL_DRV_PRINTF (("bad level 1 id %d.\n", ulModule));
            return OPL_ERR_INVALID_PARAMETERS;
        }

        if (OPL_MAX_NUM_OF_INTR_LEVEL2 <= ulSubModule)
        {
            /* enable or disable level2 interrupts here */
            OPL_DRV_PRINTF (("%s %s(id=%d) level2 interrupts:\n", 
                pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], oplIntInfo[ulModule].name, oplIntInfo[ulModule].id));

            nCount = 0;
            for (nSubFrom = OPL_ZERO; nSubFrom < OPL_MAX_NUM_OF_INTR_LEVEL2; nSubFrom++)
            {
                if (OPL_ERROR != oplIntInfo[ulModule].level2Int[nSubFrom].id && 
                    OPL_NULL != oplIntInfo[ulModule].level2Int[nSubFrom].enAddr)
                {
                    if (ulEnable)
                    {
                        oplRegBitSet(oplIntInfo[ulModule].level2Int[nSubFrom].enAddr, 
                            oplIntInfo[ulModule].level2Int[nSubFrom].enBit);
                    }
                    else {
                        oplRegBitClear(oplIntInfo[ulModule].level2Int[nSubFrom].enAddr, 
                            oplIntInfo[ulModule].level2Int[nSubFrom].enBit);
                    }
                    OPL_DRV_PRINTF (("\t%s level2 interrupt %s, reg=%#x,bit=%#x:\n", 
                        pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], 
                        oplIntInfo[ulModule].level2Int[nSubFrom].name, 
                        oplIntInfo[ulModule].level2Int[nSubFrom].enAddr, 
                        oplIntInfo[ulModule].level2Int[nSubFrom].enBit));
                    nCount++;
                }
                else {
                    break;
                }
            }
            if (OPL_ZERO == nCount)
            {
                OPL_DRV_PRINTF (("\tlevel2 interrupt has no Enable-register.\n"));
            }
        }
        else {
            /* enable or disable level2 interrupts here */
            OPL_DRV_PRINTF (("%s %s(id=%d) level2 interrupts:\n", 
                pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], oplIntInfo[ulModule].name, oplIntInfo[ulModule].id));
            if (OPL_ERROR == oplIntInfo[ulModule].level2Int[ulSubModule].id)
            {
                OPL_DRV_PRINTF (("bad level 2 id %d.\n", ulSubModule));
                return OPL_ERR_INVALID_PARAMETERS;
            }
            
            if (ulEnable)
            {
                oplRegBitSet(oplIntInfo[ulModule].level2Int[ulSubModule].enAddr, 
                    oplIntInfo[ulModule].level2Int[ulSubModule].enBit);
            }
            else {
                oplRegBitClear(oplIntInfo[ulModule].level2Int[ulSubModule].enAddr, 
                    oplIntInfo[ulModule].level2Int[ulSubModule].enBit);
            }
            OPL_DRV_PRINTF (("\t%s level2 interrupt %s, reg=%#x,bit=%#x:\n", 
                pCaption[ulEnable ? OPL_ENABLE : OPL_DISABLE], 
                oplIntInfo[ulModule].level2Int[ulSubModule].name, 
                oplIntInfo[ulModule].level2Int[ulSubModule].enAddr, 
                oplIntInfo[ulModule].level2Int[ulSubModule].enBit));
        }
    }

    return OPL_OK;
}


/*******************************************************************************
*
* interruptLevel2EnableState - get level 2 interrupt state
*
* DESCRIPTION:
* 	get level 2 interrupt state
*
* INPUT:
*	ulModule	- level 1 interrupt: 0 ~ 31
*   ulSubModule - level 2 interrupt: 0 ~ 31
*	pulEnable	- pointer to destination value, 0 or 1, 0 to disable; 1 to enbale
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
OPL_STATUS interruptLevel2EnableState (UINT32 ulModule, UINT32 ulSubModule, UINT32 *pulEnable)
{
    OPL_STATUS retVal = OPL_ERROR;

    if (NULL == pulEnable)
    {
        OPL_DRV_PRINTF (("null pointer pulEnable.\n"));
        return OPL_ERR_NULL_POINTER;
    }

    if (ulModule >= OPL_END_INTR)
    {
        OPL_DRV_PRINTF (("bad level 1 id %d.\n", ulModule));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_ERROR == oplIntInfo[ulModule].id)
    {
        OPL_DRV_PRINTF (("bad level 1 id %d.\n", ulModule));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_MAX_NUM_OF_INTR_LEVEL2 <= ulSubModule)
    {
        OPL_DRV_PRINTF (("bad level 2 id %d.\n", ulSubModule));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    retVal = oplRegBitTest(oplIntInfo[ulModule].level2Int[ulSubModule].enAddr, 
                    oplIntInfo[ulModule].level2Int[ulSubModule].enBit);
    if (retVal >= 0)
    {
        *pulEnable = retVal;
        retVal = OPL_OK;
    }

    return retVal;
}


/*******************************************************************************
* intProcesstHandler
*
* DESCRIPTION:
* 
*	this function process the interrrupt 
* 
* INPUTS: 
*	pstHandleInt: 
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intProcesstHandler(HAL_EVENT_INT_HANDLING_t *pstHandleInt)
{
	INT32  	i;

    if (NULL == pstHandleInt)
    {
        return OPL_ERR_NULL_POINTER;
    }

    //printk("handle level1 0x%08X\r\n", pstHandleInt->ulModInt);
	
	for (i = OPL_ZERO; i<OPL_END_INTR; i++)
	{ 
	    if (pstHandleInt->ulModInt & OPCONN_BIT(i))
	    {
	      switch (oplIntInfo[i].id)
	      {
			case OPL_IRQ0_INTR:
			case OPL_IRQ1_INTR:
			case OPL_IRQ2_INTR:
			case OPL_IRQ3_INTR:
			case OPL_IRQ4_INTR:
			case OPL_IRQ5_INTR:
			case OPL_IRQ6_INTR:
			case OPL_IRQ7_INTR:
            case OPL_IRQ8_INTR:
			case OPL_IRQ9_INTR:
			case OPL_IRQ10_INTR:
			case OPL_IRQ11_INTR:
				oplIntInfo[i].counter++;
				break;
			case OPL_MISC_INTR:
				oplIntInfo[i].counter++;
			case OPL_BRG_INTR:
				oplIntInfo[i].counter++;
			case OPL_UPTM_INTR:
				oplIntInfo[i].counter++;
			case OPL_DNTM_INTR:
				oplIntInfo[i].counter++;
				break;
			case OPL_MPCP_INTR:
				oplIntInfo[i].counter++;
				break;
	      	case OPL_PMAC_INTR:
				oplIntInfo[i].counter++;
				break;
			case OPL_GE_INTR:
				oplIntInfo[i].counter++;
				break;
            case OPL_PCM_INTR:
				oplIntInfo[i].counter++;
				break;
			case OPL_DMA0_INTR:
			case OPL_DMA1_INTR:
				oplIntInfo[i].counter++;
                break;
			case OPL_SPI_INTR:
				oplIntInfo[i].counter++;
				break;
			case OPL_WDOG_INTR:
			case OPL_RTC_INTR:
			case OPL_TIMER0_INTR:
			case OPL_TIMER1_INTR:
			case OPL_TIMER2_INTR:
			case OPL_TIMER3_INTR:
			case OPL_I2C_INTR:
			case OPL_UART0_INTR:
			case OPL_UART1_INTR:
				oplIntInfo[i].counter++;
				break;
			default:
				return OPL_ERROR;
				break;
	      }
	}	
   }
	return OPL_OK;
}
/*******************************************************************************
* intCounterShow
*
* DESCRIPTION:
* 
*	this function used to show the int counter  
* 
* INPUTS: 
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intCounterShow(void)
{
	INT32 firsIntIndex = OPL_ZERO;
	INT32 secondIntIndex = OPL_ZERO;

	OPL_DRV_PRINTF(("%-20s %-10s %s\n","intDesc","counter","errorCounter"));
	for(firsIntIndex = OPL_ZERO; oplIntInfo[firsIntIndex].id != -1; firsIntIndex++)
	{
		OPL_DRV_PRINTF((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
		OPL_DRV_PRINTF(("%-20s %-10d %-10d\n",
			oplIntInfo[firsIntIndex].name,
			oplIntInfo[firsIntIndex].counter,
			oplIntInfo[firsIntIndex].errCounter));
		
		for(secondIntIndex = OPL_ZERO; oplIntInfo[firsIntIndex].level2Int[secondIntIndex].id != -1; secondIntIndex++)
		{
			OPL_DRV_PRINTF(("\n%40s %-10d %-10d\n",
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].name,
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].counter,
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].ulErrCnt));
		}	
	}
	return OPL_OK;
}

/*******************************************************************************
* intCountEnable
*
* DESCRIPTION:
* 
*	this function used to disable or enable interrupt counter
* 
* INPUTS: 
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/

UINT32 intEnable = 1;

OPL_STATUS intCountEnable(UINT8 enable)
{
	if(enable)
	{
		intEnable = OPL_ENABLE;
	}else
	{
		intEnable = OPL_DISABLE;
	}
	return OPL_OK;
}

OPL_STATUS intCountEnableState(UINT32 *enable)
{
    if (NULL == enable)
    {
        return OPL_ERR_NULL_POINTER;
    }

	*enable = intEnable;

	return OPL_OK;
}



/*******************************************************************************
*
* halInterruptLevel1Enable:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel1Enable(VOID *pParam)
{
    HAL_API_INT_LEVEL1_ENABLE_t *pIntParam = (HAL_API_INT_LEVEL1_ENABLE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel1Enable(pIntParam->ulModule, pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptLevel1EnableState:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel1EnableState(VOID *pParam)
{
    HAL_API_INT_LEVEL1_ENABLE_STATE_t *pIntParam = (HAL_API_INT_LEVEL1_ENABLE_STATE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel1EnableState(pIntParam->ulModule, &pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptLevel2Enable:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel2Enable(VOID *pParam)
{
    HAL_API_INT_LEVEL2_ENABLE_t *pIntParam = (HAL_API_INT_LEVEL2_ENABLE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel2Enable(pIntParam->ulModule, pIntParam->ulSubModule, pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptLevel2EnableState:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel2EnableState(VOID *pParam)
{
    HAL_API_INT_LEVEL2_ENABLE_t *pIntParam = (HAL_API_INT_LEVEL2_ENABLE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel2EnableState(pIntParam->ulModule, 
        pIntParam->ulSubModule, &pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptCounterEnable:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptCounterEnable(VOID *pParam)
{
    HAL_API_INT_COUNTER_EN_t *pIntParam = (HAL_API_INT_COUNTER_EN_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = intCountEnable(pIntParam->ulEnable);
    
}


/*******************************************************************************
*
* halInterruptCounterEnableState:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptCounterEnableState(VOID *pParam)
{
    HAL_API_INT_COUNTER_EN_t *pIntParam = (HAL_API_INT_COUNTER_EN_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = intCountEnableState(&pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptCounterShow:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptCounterShow(VOID *pParam)
{
    HAL_API_INT_COUNTER_SHOW_t *pIntParam = (HAL_API_INT_COUNTER_SHOW_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    if (0 == pIntParam->ulFd)
    {
        pIntParam->ulFd = OP_STDOUT_FILENO;
    }

	UINT32 firsIntIndex = OPL_ZERO;
	UINT32 secondIntIndex = OPL_ZERO;

	vosPrintf(pIntParam->ulFd, "%-20s %-12s %s\r\n", " Description", " Counter", " Error Counter");
	for(firsIntIndex = OPL_ZERO; (INT32)oplIntInfo[firsIntIndex].id != -1; firsIntIndex++)
	{
		vosPrintf(pIntParam->ulFd, "-------------------- ------------ ------------\r\n");
		vosPrintf(pIntParam->ulFd, "%-20s %-12d %-12d\r\n",
			oplIntInfo[firsIntIndex].name,
			oplIntInfo[firsIntIndex].counter,
			oplIntInfo[firsIntIndex].errCounter);
		
		for(secondIntIndex = OPL_ZERO; oplIntInfo[firsIntIndex].level2Int[secondIntIndex].id != -1; secondIntIndex++)
		{
			vosPrintf(pIntParam->ulFd, "\r\n%40s %-10d %-10d\r\n",
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].name,
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].counter,
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].ulErrCnt);
		}	
	}
}

#define waitForintrrupt 	wait_for_ipmux_intr
#define enableIntrrupt  	enable_ipmux_intr

static void halInterruptThread()
{
    UINT8 i;
    HAL_EVENT_INT_HANDLING_t stIntParam;
    OPL_EVENT_DATA_t stEventData;
    UINT32 ulParamLen = sizeof(HAL_EVENT_INT_HANDLING_t);

    stIntParam.ulModInt = OPL_ZERO;
    /* enable LOS_IND_INT_EN */
    oplRegFieldWrite(REG_PMAC_INT_EN, 31, 1, 1);
    oplRegFieldWrite(REG_PMAC_INT_EN, 29, 1, 1);

	while(1)
	{
		waitForintrrupt(&(stIntParam.ulModInt));

        for (i = OPL_ZERO; i<OPL_END_INTR; i++)
    	{ 
    	    if ((stIntParam.ulModInt & OPCONN_BIT(i)) &&
                (oplIntInfo[i].id < OPL_END_INTR) && 
                oplIntInfo[i].level2Int[OPL_ZERO].penAddr > OPL_ZERO)
    	    {
                oplRegRead(oplIntInfo[i].level2Int[OPL_ZERO].penAddr,
                    &(stIntParam.ulSubModInt[i]));
    	    }
        }

        if (NO_ERROR == halAppEventIsExisted(HAL_EVENT_INT_HANDLING) &&
            (OPCONN_BIT(OPL_PMAC_INTR) & stIntParam.ulModInt) && 
            (stIntParam.ulSubModInt[OPL_PMAC_INTR]&
            oplIntInfo[OPL_PMAC_INTR].level2Int[LOS_IND_INT_TYPE].penBit))
        {
            stEventData.eventId = HAL_EVENT_INT_HANDLING;
            stEventData.length = ulParamLen;
            stEventData.param = (VOID *)&stIntParam;
            halDrvEventRaise(&stEventData);
        }

		intProcesstHandler(&stIntParam);

		enableIntrrupt();
	}
}

STATUS halInterruptThreadInit(void)
{
    int ret;

    if (g_pstIntProcessThreadId) {
        return ERROR;
    }

    g_pstIntProcessThreadId = vosThreadCreate("tIntProc", OP_VOS_THREAD_STKSZ, 80,
                        (void *)halInterruptThread, (void *)NULL);
    if (g_pstIntProcessThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS halInterruptThreadShutdown(void)
{
    if (!g_pstIntProcessThreadId) {
        return ERROR;
    }

    if (g_pstIntProcessThreadId != NULL) {
        vosThreadDestroy(g_pstIntProcessThreadId);
    }

    return OK;
}




/*******************************************************************************
*
* halInterruptInit:	 to initialize interrupt hal module
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	n/a.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	OK      - initialize successfully.
*   ERROR   - initialize failed.
*
* SEE ALSO: 
*/
OPL_RESULT halInterruptInit(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    /* register interrupt API */

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE;
    stApiUnit.apiFunc = halInterruptLevel1Enable;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE_STATE;
    stApiUnit.apiFunc = halInterruptLevel1EnableState;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE;
    stApiUnit.apiFunc = halInterruptLevel2Enable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE_STATE;
    stApiUnit.apiFunc = halInterruptLevel2EnableState;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE;
    stApiUnit.apiFunc = halInterruptCounterEnable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE_STATE;
    stApiUnit.apiFunc = halInterruptCounterEnableState;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_SHOW;
    stApiUnit.apiFunc = halInterruptCounterShow;
    ulRet += halDrvApiRegister(&stApiUnit);

    if(intEnable)
	{
        /* enable global interrupt */
		enableIntrrupt();
        
        /* enable specific interrupt here */
#if 0  /* deleted by Gan Zhiheng - 2009/07/15, only use for MPW */
        oplRegFieldWrite(REG_PMAC_INT_EN, 31, 1, 1);
#endif /* #if 0 */

	}

    halInterruptThreadInit();

    return ulRet;
}

/*******************************************************************************
*
* halInterruptDestroy:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptDestroy()
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    halInterruptThreadShutdown();

    stApiUnit.apiFunc = NULL;

    /* deregister interrupt API */

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE_STATE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE_STATE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE_STATE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_SHOW;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ;
}

