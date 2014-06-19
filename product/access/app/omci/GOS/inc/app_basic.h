 /******************************************************************************
 * (c) Copyright 2005, Cambridge Industry Group, All Rights Reserved.
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF CIG, INC.
 * The copyright notice above does not evidence any actual or intended
 * publication of such source code.
 * 
 * FILE: app_basic.h
 * 
 * DESCRIPTION: 
 * 
 * 
 * DATE: 14 feb, 2006
 * 
 * AUTHOR: Zhou Shengjin
 * 
 ******************************************************************************/

#ifndef __APP_BASIC_H__
#define __APP_BASIC_H__

#ifdef  __cplusplus
extern "C" {
#endif

/* linux defintions */
#include "gos_linux.h"

/* basic type definitions */
#include "gos_type.h"

/* basic funciton definitions */
#include "gos_general.h"

/* GPON OMCI(984.4) definitions */
#include "omci.h"

/* MIB exported definitions */
#include "mib.h"
#include "mib_tree.h"
#include "mib_defs.h"
#ifdef  __cplusplus
}
#endif

#endif /* __APP_BASIC_H__ */

