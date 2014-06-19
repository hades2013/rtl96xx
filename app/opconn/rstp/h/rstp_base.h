/************************************************************************ 
 * RSTP library - Rapid Spanning Tree (802.1t, 802.1w) 
 * Copyright (C) 2001-2003 Optical Access 
 * Author: Alex Rozin 
 * 
 * This file is part of RSTP library. 
 * 
 * RSTP library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; version 2.1 
 * 
 * RSTP library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with RSTP library; see the file COPYING.  If not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA. 
 **********************************************************************/
 
/*******************************************************************************
**    MODULE     :  RSTP
**
**    FILE       :  rstp_base.h
**      
**    DESCRIPTION:  This file contains the base definition of RSTP
**    
**
**    NOTES      :  None.
**
*******************************************************************************/
/* 
**    MODIFICATION HISTORY:
**
**    Rev #    Date         Author         Description
**    -----    ----------   ------------   ------------------------------------
**    0.1  	   1/10/2007    rzhou         Initial                      
**
*******************************************************************************/

/* Mutual RSTP definitions */
 
#ifndef _STP_BASE_H__
#define _STP_BASE_H__

#include "defs.h"

#include <stdlib.h>
#include <string.h>
//rzhou debug
//#ifdef DEBUG
#  define STP_DBG 1
//#endif

/* begin added by jiangmingli for rstp */
#define STRONGLY_SPEC_802_1W 1
/* end added by jiangmingli for rstp */


#  include <stddef.h>
#  include <stdio.h>
#  include <netinet/in.h>
#  include "rstp_bitmap.h"
#  include "uid_stp.h"
#  define Print printf

#ifndef INOUT
#  define IN      /* consider as comments near 'input' parameters */
#  define OUT     /* consider as comments near 'output' parameters */
#  define INOUT   /* consider as comments near 'input/output' parameters */
#endif

#ifndef Zero
#  define Zero        0
#  define One         1
#endif

#ifndef Bool
#  define Bool        int
#  define False       0
#  define True        1
#endif

#include "rstp_bpdu.h"
#include "rstp_vector.h"
#include "rstp_times.h"

#define RSTP_ERRORS { \
  CHOOSE(STP_OK),                                       \
  CHOOSE(STP_Cannot_Find_Vlan),         \
  CHOOSE(STP_Imlicite_Instance_Create_Failed),          \
  CHOOSE(STP_Small_Bridge_Priority),                    \
  CHOOSE(STP_Large_Bridge_Priority),                    \
  CHOOSE(STP_Small_Hello_Time),                         \
  CHOOSE(STP_Large_Hello_Time),                         \
  CHOOSE(STP_Small_Max_Age),                            \
  CHOOSE(STP_Large_Max_Age),                            \
  CHOOSE(STP_Small_Forward_Delay),                      \
  CHOOSE(STP_Large_Forward_Delay),                      \
  CHOOSE(STP_Forward_Delay_And_Max_Age_Are_Inconsistent),\
  CHOOSE(STP_Hello_Time_And_Max_Age_Are_Inconsistent),  \
  CHOOSE(STP_Vlan_Had_Not_Yet_Been_Created),            \
  CHOOSE(STP_Port_Is_Absent_In_The_Vlan),               \
  CHOOSE(STP_Big_len8023_Format),                       \
  CHOOSE(STP_Small_len8023_Format),                     \
  CHOOSE(STP_len8023_Format_Gt_Len),                    \
  CHOOSE(STP_Not_Proper_802_3_Packet),                  \
  CHOOSE(STP_Invalid_Protocol),                         \
  CHOOSE(STP_Invalid_Version),                          \
  CHOOSE(STP_Had_Not_Yet_Been_Enabled_On_The_Vlan),     \
  CHOOSE(STP_Cannot_Create_Instance_For_Vlan),          \
  CHOOSE(STP_Cannot_Create_Instance_For_Port),          \
  CHOOSE(STP_Invalid_Bridge_Priority),                  \
  CHOOSE(STP_There_Are_No_Ports),                       \
  CHOOSE(STP_Cannot_Compute_Bridge_Prio),               \
  CHOOSE(STP_Another_Error),                            \
  CHOOSE(STP_Nothing_To_Do),                            \
  CHOOSE(STP_LAST_DUMMY),                               \
}

#define CHOOSE(a) a
typedef enum RSTP_ERRORS RSTP_ERRORS_T;
#undef CHOOSE

#ifdef __LINUX__
#define STP_FATAL(TXT, MSG, EXCOD)                      \
      {stp_trace ("FATAL:%s failed: %s:%d", TXT, MSG, EXCOD);  \
      exit (EXCOD);}
#else
#define STP_FATAL(TXT, MSG, EXCOD)                      \
      Print ("FATAL: %s code %s:%d\n", TXT, MSG, EXCOD)
#endif

#define STP_MALLOC(PTR, TYPE, MSG)              \
  {                                             \
    PTR = (TYPE*) calloc (1, sizeof (TYPE));    \
    if (! PTR) {                                \
      STP_FATAL("malloc", MSG, -6);             \
    }                                           \
  }

#define STP_FREE(PTR, MSG)              \
  {                                     \
    if (! PTR) {                        \
      STP_FATAL("free", MSG, -66);      \
    }                                   \
    free (PTR);                         \
    PTR = NULL;                         \
  }

#define STP_STRDUP(PTR, SRC, MSG)       \
  {                                     \
    PTR = strdup (SRC);                 \
    if (! PTR) {                        \
      STP_FATAL("strdup", MSG, -7);     \
    }                                   \
  }

#define STP_NEW_IN_LIST(WHAT, TYPE, LIST, MSG)  \
  {                                             \
    STP_MALLOC(WHAT, TYPE, MSG);                \
    WHAT->next = LIST;                          \
    LIST = WHAT;                                \
  }
  
//rzhou pls note the event of rstp
typedef enum {
  RSTP_PORT_EN_T,
  RSTP_PORT_DIS_T,
  RSTP_PORT_SPEED_T,
  RSTP_PORT_DPLEX_T,
  RSTP_PORT_RX_T,
  RSTP_PORT_TIME_T,
  RSTP_PORT_MNGR_T,
  RSTP_EVENT_LAST_DUMMY

} RSTP_EVENT_T;

/* for debug trace messages */


extern char* sprint_time_stump (void);

#if 0
#define RSTP_TRACE 1
#if RSTP_TRACE
#define stp_trace(F, B...) printf("%s:" F "\n", sprint_time_stump(), ##B)
#else
#define stp_trace(F, B...)
#endif
#if 0
extern ULONG stp_trace (const char* fmt, ...);
#endif
#endif

extern int rstptrace;
#define stp_trace(F, B...) if(rstptrace)printf("%s:" F "\n", sprint_time_stump(), ##B)

extern int rstpdbg;
#define opl_rstp_printf(x) if(rstpdbg)printf x

#endif /*  _STP_BASE_H__ */

