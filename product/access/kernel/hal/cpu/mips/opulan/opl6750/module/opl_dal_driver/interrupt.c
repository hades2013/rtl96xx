/*
=============================================================================
     File Name: interrupt.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2008/2/3		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include "opconn_lib.h"
#include "interrupt.h"


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
* intSerEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used to enable the opconn interrupt.
* 
* INPUTS: 
*		enable:1 for enable while 0 for disable 
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intSerEnHwWrite(UINT8 enable)
{
	UINT32 regVal;
	OPL_STATUS retVal = OPL_OK;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;

	retVal = oplRegFieldWrite(INTC_IRQ_CFG, 0, 1, regVal);

	return retVal;
}
/*******************************************************************************
* intSerEnHwRead
*
* DESCRIPTION:
* 
*	this function is used to get wether the interrupt is enable
* 
* INPUTS: 
*
* OUTPUTS: 
*	eanbe: 0 for disable while 1 for enable		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intSerEnHwRead(UINT8 *enable)
{
	UINT32 regVal;
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(INTC_IRQ_CFG, 0, 1, &regVal);

	*enable = regVal?OPL_ENABLE:OPL_DISABLE;
	return retVal;
}
/*******************************************************************************
* intUPTmEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the tm interrupt
* 
* INPUTS: 
*	type: 
*
* OUTPUT:
*	enable:
*		0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intUPTmEnHwWrite(UINT8 type,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	if(type >= TMUS_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(type)
	{
    	case TMUS_QUEUE_FULL_INT_TYPE:
    	case TMUS_RD_ERR_INT_TYPE:
    	case TMUS_PKT_POS_ERR_INT_TYPE:
    	case TMUS_PKT_LEN_ERR_INT_TYPE:
    	case TMUS_LL_FULL_INT_TYPE:
    	case TMUS_RPT_ERR_INT_TYPE:
    	case TM_HEAD_DISORDER_INT_TYPE:
    	case TM_HEAD_OVFLW_INT_TYPE:
    	case TM_MPCP_SEND_FAIL_ERR_INT_TYPE:
    	case TM_MPCP_SEND_OK_ERR_INT_TYPE:
            retVal = interruptLevel2Enable(OPL_UPTM_INTR, type, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* intUPTmEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get the  enable or disable status of tm interrupt
* 
* INPUTS: 
*		type: 
*			
*	OUTPUT
*		enable:
*			0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intUPTmEnHwRead(UINT8 type,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	if(type >= TMUS_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(type)
	{
    	case TMUS_QUEUE_FULL_INT_TYPE:
    	case TMUS_RD_ERR_INT_TYPE:
    	case TMUS_PKT_POS_ERR_INT_TYPE:
    	case TMUS_PKT_LEN_ERR_INT_TYPE:
    	case TMUS_LL_FULL_INT_TYPE:
    	case TMUS_RPT_ERR_INT_TYPE:
    	case TM_HEAD_DISORDER_INT_TYPE:
    	case TM_HEAD_OVFLW_INT_TYPE:
    	case TM_MPCP_SEND_FAIL_ERR_INT_TYPE:
    	case TM_MPCP_SEND_OK_ERR_INT_TYPE:
            retVal = oplRegRead(oplIntInfo[OPL_UPTM_INTR].level2Int[type].enAddr, &regVal);
            if (oplIntInfo[OPL_UPTM_INTR].level2Int[type].enBit & regVal)
            {
                regVal = OPL_ENABLE; 
            }
            else {
                regVal = OPL_DISABLE; 
            }
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*enable = regVal;
	return retVal;
}
/*******************************************************************************
* intDNTmEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the tm interrupt
* 
* INPUTS: 
*	type: 
*		
* OUTPUT:
*	enable:
*		0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDNTmEnHwWrite(UINT8 type,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	if(type >= TMDS_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(type)
	{
    	case TMDS_QUEUE_FULL_INT_TYPE:
    	case TMDS_RD_ERR_INT_TYPE:
    	case TMDS_PKT_POS_ERR_INT_TYPE:
    	case TMDS_PKT_LEN_ERR_INT_TYPE:
    	case TMDS_LL_FULL_INT_TYPE:
            retVal = interruptLevel2Enable(OPL_DNTM_INTR, type, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* intDNTmEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get the  enable or disable status of tm interrupt
* 
* INPUTS: 
*		type: 
*			
*	OUTPUT
*		enable:
*			0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDNTmEnHwRead(UINT8 type,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	if(type >= TMDS_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(type)
	{
    	case TMDS_QUEUE_FULL_INT_TYPE:
    	case TMDS_RD_ERR_INT_TYPE:
    	case TMDS_PKT_POS_ERR_INT_TYPE:
    	case TMDS_PKT_LEN_ERR_INT_TYPE:
    	case TMDS_LL_FULL_INT_TYPE:
            retVal = oplRegRead(oplIntInfo[OPL_DNTM_INTR].level2Int[type].enAddr, &regVal);
            if (oplIntInfo[OPL_DNTM_INTR].level2Int[type].enBit & regVal)
            {
                regVal = OPL_ENABLE; 
            }
            else {
                regVal = OPL_DISABLE; 
            }
            break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*enable = regVal;
	return retVal;
}
/*******************************************************************************
* intBrgEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the bridge interrupt
* 
* INPUTS: 
*	type: 
*			
*
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intBrgEnHwWrite(UINT8 type,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	if(type >= BRG_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(type)
	{
    	case RX_DIS_DROP_INT_TYPE:
    	case TX_DIS_DROP_INT_TYPE:
    	case RSV_MAC_DROP_INT_TYPE:
    	case USR_MAC_DROP_INT_TYPE:
    	case US_VTT_ERR_INT_TYPE:
        case DS_VTT_ERR_INT_TYPE:
    	case US_TAG_ERR_INT_TYPE:
    	case DS_TAG_ERR_INT_TYPE:
    	case US_ARL_SMAC_CFL_INT_TYPE:
    	case DS_ARL_SMAC_CFL_INT_TYPE:
    	case US_ZERO_DA_DROP_INT_TYPE:
        case DS_ZERO_DA_DROP_INT_TYPE:
    	case ARL_FULL_ARL_NEW_REQ_INT_TYPE:
    	case US_NON_UCST_DROP_INT_TYPE:
        case DS_NON_UCST_DROP_INT_TYPE:
            retVal = interruptLevel2Enable(OPL_BRG_INTR, type, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* intBrgEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get wether the interrupt is enable .
* 
* INPUTS: 
*	type: 
*			
*	OUTPUT:
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intBrgEnHwRead(UINT8 type,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	
	if(type >= BRG_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	switch(type)
	{
    	case RX_DIS_DROP_INT_TYPE:
    	case TX_DIS_DROP_INT_TYPE:
    	case RSV_MAC_DROP_INT_TYPE:
    	case USR_MAC_DROP_INT_TYPE:
    	case US_VTT_ERR_INT_TYPE:
        case DS_VTT_ERR_INT_TYPE:
    	case US_TAG_ERR_INT_TYPE:
    	case DS_TAG_ERR_INT_TYPE:
    	case US_ARL_SMAC_CFL_INT_TYPE:
    	case DS_ARL_SMAC_CFL_INT_TYPE:
    	case US_ZERO_DA_DROP_INT_TYPE:
        case DS_ZERO_DA_DROP_INT_TYPE:
    	case ARL_FULL_ARL_NEW_REQ_INT_TYPE:
    	case US_NON_UCST_DROP_INT_TYPE:
        case DS_NON_UCST_DROP_INT_TYPE:
            retVal = oplRegRead(oplIntInfo[OPL_BRG_INTR].level2Int[type].enAddr, &regVal);
            if (oplIntInfo[OPL_BRG_INTR].level2Int[type].enBit & regVal)
            {
                regVal = OPL_ENABLE; 
            }
            else {
                regVal = OPL_DISABLE; 
            }
            break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}	
	if(OPL_OK == retVal)
	{
		*enable = regVal?OPL_ENABLE:OPL_DISABLE;
	}
	return retVal;
}
/*******************************************************************************
* intMpcpEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used to config the interrupt enable or disable
* 
* INPUTS: 
*	type: 
*		
*	enable:
*		0 disable
*		1 enable
*	OUTPUT:
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpEnHwWrite(UINT8 type,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(MPCP_INT_TYPE_END <= type)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	switch(type)
	{
    	case MPCP_STATE_INT_TYPE:
    	case MPCP_GATE_DISORDER_INT_TYPE:
    	case MPCP_ECHO_PENDING_ERR_INT_TYPE:
    	case MPCP_STATE_CHANGE_INT_TYPE:
    	case MPCP_GRANT_LIST_FULL_INT_TYPE:
    	case MPCP_GATE_TIMEOUT_INT_TYPE:
    	case MPCP_LINK_UP_INT_TYPE:
    	case MPCP_LINK_DOWN_INT_TYPE:
            retVal = interruptLevel2Enable(OPL_MPCP_INTR, type, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* intMpcpEnHwRead
*
* DESCRIPTION:
* 
*	this function is used to get the config value of the interrrupt
* 
* INPUTS: 
*	type: 
*		
*	enable:
*		0 disable
*		1 enable
*	OUTPUT:
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpEnHwRead(UINT8 type,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(MPCP_INT_TYPE_END <= type)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	switch(type)
	{
    	case MPCP_STATE_INT_TYPE:
    	case MPCP_GATE_DISORDER_INT_TYPE:
    	case MPCP_ECHO_PENDING_ERR_INT_TYPE:
    	case MPCP_STATE_CHANGE_INT_TYPE:
    	case MPCP_GRANT_LIST_FULL_INT_TYPE:
    	case MPCP_GATE_TIMEOUT_INT_TYPE:
    	case MPCP_LINK_UP_INT_TYPE:
    	case MPCP_LINK_DOWN_INT_TYPE:
            retVal = oplRegRead(oplIntInfo[OPL_MPCP_INTR].level2Int[type].enAddr, &regVal);
            if (oplIntInfo[OPL_MPCP_INTR].level2Int[type].enBit & regVal)
            {
                regVal = OPL_ENABLE; 
            }
            else {
                regVal = OPL_DISABLE; 
            }
            break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	if(OPL_OK == retVal)
	{
		*enable = regVal?OPL_ENABLE:OPL_DISABLE;
	}
	return retVal;
}
/*******************************************************************************
* intMiscEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the Misc interrupt
* 
* INPUTS: 
*	type: 
*
* OUTPUT:
*	enable:
*		0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMiscEnHwWrite(UINT8 type,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	if(type >= MISC_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(type)
	{
    	case MISC_DATA_SPACE_FULL_INT_TYPE:
    	case MISC_TRAF_OVFLW_INT_TYPE:
    	case MISC_MPIS_UFLOW_INT_TYPE:
    	case MISC_LL_EMPTY_INT_TYPE:
            retVal = interruptLevel2Enable(OPL_MISC_INTR, type, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* intMiscEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get the  enable or disable status of Misc interrupt
* 
* INPUTS: 
*		type: 
*		
*	OUTPUT
*		enable:
*			0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMiscEnHwRead(UINT8 type,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	if(type >= MISC_INT_TYPE_END)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(type)
	{
    	case MISC_DATA_SPACE_FULL_INT_TYPE:
    	case MISC_TRAF_OVFLW_INT_TYPE:
    	case MISC_MPIS_UFLOW_INT_TYPE:
    	case MISC_LL_EMPTY_INT_TYPE:
            retVal = oplRegRead(oplIntInfo[OPL_MISC_INTR].level2Int[type].enAddr, &regVal);
            if (oplIntInfo[OPL_MISC_INTR].level2Int[type].enBit & regVal)
            {
                regVal = OPL_ENABLE; 
            }
            else {
                regVal = OPL_DISABLE; 
            }
            break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*enable = regVal;
	return retVal;
}

/*******************************************************************************
* intMpcpLinkDowHandler
*
* DESCRIPTION:
* 
*	this function handler the mpcp link down
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpLinkDowHandler(void)
{
	ponMpcpStatusShow();
	return OPL_OK;
}
/*******************************************************************************
* intMiscHandler
*
* DESCRIPTION:
* 
*	this function handler the misc interrupt,such as mpcp interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMiscHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;

	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
            /* special treatment for every int ?? */
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
            	case MISC_DATA_SPACE_FULL_INT_TYPE:
            	case MISC_TRAF_OVFLW_INT_TYPE:
            	case MISC_MPIS_UFLOW_INT_TYPE:
            	case MISC_LL_EMPTY_INT_TYPE:
				default:
					break;
			}
		}			
	}
	return retVal;
}

/*******************************************************************************
* intBrgHandler
*
* DESCRIPTION:
* 
*	this function handler the brg interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intBrgHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 learnMod = OPL_ZERO;
	INT32  i = OPL_ZERO;

	oplRegFieldRead(REG_BRG_AGE_CTRL,31,1,&learnMod);

	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
				case RX_DIS_DROP_INT_TYPE:
					break;
				case TX_DIS_DROP_INT_TYPE:
					break;
				case RSV_MAC_DROP_INT_TYPE:
					break;
				case USR_MAC_DROP_INT_TYPE:
					break;
				case US_VTT_ERR_INT_TYPE:
					break;
				case DS_VTT_ERR_INT_TYPE:
					break;
				case US_TAG_ERR_INT_TYPE:
					break;
				case DS_TAG_ERR_INT_TYPE:
					break;
				case US_ARL_SMAC_CFL_INT_TYPE:
					break;
				case DS_ARL_SMAC_CFL_INT_TYPE:
					break;
				case US_ZERO_DA_DROP_INT_TYPE:
					break;
				case DS_ZERO_DA_DROP_INT_TYPE:
					break;
				case ARL_FULL_ARL_NEW_REQ_INT_TYPE:
					brgArlIntProcess();
					break;
				case US_NON_UCST_DROP_INT_TYPE:
					break;
				case DS_NON_UCST_DROP_INT_TYPE:
					break;
				default:
					break;
			}
		}
	}
	return retVal;
}
/*******************************************************************************
* intUPTMHandler
*
* DESCRIPTION:
* 
*	this function handler the UPTM interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intUPTMHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;

	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
				case TMUS_QUEUE_FULL_INT_TYPE:
					break;
				case TMUS_RD_ERR_INT_TYPE:
					break;
				case TMUS_PKT_POS_ERR_INT_TYPE:
					break;
				case TMUS_PKT_LEN_ERR_INT_TYPE:
					break;
				case TMUS_LL_FULL_INT_TYPE:
					break;
				case TMUS_RPT_ERR_INT_TYPE:
					break;
				case TM_HEAD_DISORDER_INT_TYPE:
					break;
				case TM_HEAD_OVFLW_INT_TYPE:
					break;
				case TM_MPCP_SEND_FAIL_ERR_INT_TYPE:
					break;
				case TM_MPCP_SEND_OK_ERR_INT_TYPE:
					break;
				default:
					break;
			}
		}			
	}
	return retVal;
}
/*******************************************************************************
* intDNTMHandler
*
* DESCRIPTION:
* 
*	this function handler the UPTM interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDNTMHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;

	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
				case TMDS_QUEUE_FULL_INT_TYPE:
					break;
				case TMDS_RD_ERR_INT_TYPE:
					break;
				case TMDS_PKT_POS_ERR_INT_TYPE:
					break;
				case TMDS_PKT_LEN_ERR_INT_TYPE:
					break;
				case TMDS_LL_FULL_INT_TYPE:
					break;
				default:
					break;
			}
		}			
	}
	return retVal;
}

/*******************************************************************************
* intMpcpHandler
*
* DESCRIPTION:
* 
*	this function handler the mpcp interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;

	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
				case MPCP_STATE_INT_TYPE:
					break;
				case MPCP_GATE_DISORDER_INT_TYPE:
					break;
				case MPCP_ECHO_PENDING_ERR_INT_TYPE:
					break;
				case MPCP_STATE_CHANGE_INT_TYPE:
					break;
				case MPCP_GRANT_LIST_FULL_INT_TYPE:
					break;
                case MPCP_GATE_TIMEOUT_INT_TYPE:
                    break;
                case MPCP_LINK_UP_INT_TYPE:
                    break;
				case MPCP_LINK_DOWN_INT_TYPE:
					intMpcpLinkDowHandler();
					break;
				default:
					break;
			}
		}			
	}
	return retVal;
}
/*******************************************************************************
* intPMACHandler
*
* DESCRIPTION:
* 
*	this function handler the PMAC interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intPMACHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;


	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
                case SIGNAL_VAL_INT_TYPE:
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
                case SYNC_ERR_RX_INT_TYPE:
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
                case SYNC_ERR_TX_INT_TYPE:
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
                case RXPLL_LCK_CHANGE_INT_TYPE:
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
                case TXPLL_LCK_CHANGE_INT_TYPE:
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
                case LINK_CHANGE_INT_TYPE:
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
                case CRC8_ERR_INT_TYPE:
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
                case LOS_IND_INT_TYPE:
                    if (level2Int&oplIntInfo[intIndex].level2Int[SIGNAL_VAL_INT_TYPE].penBit) 
                    {
                        //ponSerdesReset();
				
#if defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
						/* turn off GE transeiver*/
						oplRegFieldWrite(REG_GMAC_CFG_TRANSFER_ENA,0, 2, 0X3);

						/* write FID = 0 */
						gvlnSetPortVlanDBNum(dev, 10, 0);
						gprtSetLearnDisable(dev,10,GT_FALSE);
#endif
                    }
					else
					{
#if defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
						/* turn on GE transeiver*/
						oplRegFieldWrite(REG_GMAC_CFG_TRANSFER_ENA,0, 2, 0X0);

						/* write FID = 0 */
						gvlnSetPortVlanDBNum(dev, 10, 0);
						gprtSetLearnDisable(dev,10,GT_FALSE);
#endif
					}

					/* write */
					
                    oplIntInfo[intIndex].level2Int[i].counter++;
                    break;
				default:
					break;
			}
		}			
	}
	return retVal;
}
/*******************************************************************************
* intGEHandler
*
* DESCRIPTION:
* 
*	this function handler the GE interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intGEHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;


	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
				default:
					break;
			}
		}			
	}
	return retVal;
}

/*******************************************************************************
* intPCMHandler
*
* DESCRIPTION:
* 
*	this function handler the PCM interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intPCMHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;


	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
				default:
					break;
			}
		}			
	}
	return retVal;
}

/*******************************************************************************
* intSPIHandler
*
* DESCRIPTION:
* 
*	this function handler the SPI interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intSPIHandler(UINT32 intIndex, UINT32 level2Int)
{
	OPL_STATUS retVal = OPL_OK;
	INT32  i = OPL_ZERO;


	if(!level2Int)
	{
		return OPL_OK;
	}
	
	for(i = OPL_ZERO;oplIntInfo[intIndex].level2Int[i].id != -1; i++)
	{
		if(level2Int&oplIntInfo[intIndex].level2Int[i].penBit)
		{
			oplIntInfo[intIndex].level2Int[i].counter++;
			switch(oplIntInfo[intIndex].level2Int[i].id)
			{
				default:
					break;
			}
		}			
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
				intMiscHandler(i, pstHandleInt->ulSubModInt[i]);
			case OPL_BRG_INTR:
				oplIntInfo[i].counter++;
				intBrgHandler(i, pstHandleInt->ulSubModInt[i]);
			case OPL_UPTM_INTR:
				oplIntInfo[i].counter++;
				intUPTMHandler(i, pstHandleInt->ulSubModInt[i]);
			case OPL_DNTM_INTR:
				oplIntInfo[i].counter++;
				intDNTMHandler(i, pstHandleInt->ulSubModInt[i]);
				break;
			case OPL_MPCP_INTR:
				oplIntInfo[i].counter++;
				intMpcpHandler(i, pstHandleInt->ulSubModInt[i]);
				break;
	      	case OPL_PMAC_INTR:
				oplIntInfo[i].counter++;
                intPMACHandler(i, pstHandleInt->ulSubModInt[i]);
				break;
			case OPL_GE_INTR:
				oplIntInfo[i].counter++;
                intGEHandler(i, pstHandleInt->ulSubModInt[i]);
				break;
            case OPL_PCM_INTR:
				oplIntInfo[i].counter++;
                intPCMHandler(i, pstHandleInt->ulSubModInt[i]);
				break;
			case OPL_DMA0_INTR:
			case OPL_DMA1_INTR:
				oplIntInfo[i].counter++;
                break;
			case OPL_SPI_INTR:
				oplIntInfo[i].counter++;
                intSPIHandler(i, pstHandleInt->ulSubModInt[i]);
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
#if 0/*wfxu*/
OPL_STATUS cliIntStatusShow(int fd)
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
        cliVosShowPrintf((fd,"%-20s.addr = 0x%04x:: 0x%08x\r\n",
            oplIntInfoForShow[colIndex].name,
            intStatusAddr/4,
            intStatus));
        while(oplIntInfoForShow[colIndex].level2Int[rowIndex].id != 12345)
        {
            if(intStatus&oplIntInfoForShow[colIndex].level2Int[rowIndex].penBit)
            {
                cliVosShowPrintf((fd,"%s.%15s:   1\r\n",
                    oplIntInfoForShow[colIndex].name,
                    oplIntInfoForShow[colIndex].level2Int[rowIndex].name));
            }
            rowIndex++;
        }
        colIndex++;
    }
}
#endif

