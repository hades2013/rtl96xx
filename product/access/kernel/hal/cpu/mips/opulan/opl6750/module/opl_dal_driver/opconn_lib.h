/*
=============================================================================
     Header Name: opconn_lib.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef OPCONN_LIB_H
#define OPCONN_LIB_H
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"
#include "opconn_hw_reg.h"
#include "peripheral.h"
#include "i2c.h"
#include "opconnmisc.h"

#include "bridgeport.h"
#include "bridgevlan.h"
#include "bridgemac.h"
#include "tm.h"
#include "pon.h"
#include "classify.h"
#include "phy.h"
#include "interrupt.h"


#define ONU_REGBASE		0xBF000000

OPL_STATUS oplDevInit(UINT8 *pMemBuff);

OPL_STATUS oplDriverInit(void);

#endif
