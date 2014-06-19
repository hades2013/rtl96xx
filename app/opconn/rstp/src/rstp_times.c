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
**    FILE       :  rstp_timers.c
**      
**    DESCRIPTION:  This file contains "Times" API : bridgeTime, rootTimes,   
**    portTimes, designatedTimes, msgTimes.
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
#include "rstp_base.h"
int
STP_compare_times (IN TIMEVALUES_T *t1, IN TIMEVALUES_T *t2)
{
  if (t1->MessageAge < t2->MessageAge)     return -1;
  if (t1->MessageAge > t2->MessageAge)     return  1;

  if (t1->MaxAge < t2->MaxAge)             return -2;
  if (t1->MaxAge > t2->MaxAge)             return  2;

  if (t1->ForwardDelay < t2->ForwardDelay) return -3;
  if (t1->ForwardDelay > t2->ForwardDelay) return  3;

  if (t1->HelloTime < t2->HelloTime)       return -4;
  if (t1->HelloTime > t2->HelloTime)       return  4;

  return 0;
}

void
STP_get_times (IN BPDU_BODY_T *b, OUT TIMEVALUES_T *v)
{
#if 0
  v->MessageAge =   ntohs (*((unsigned short*) b->message_age))   >> 8;
  v->MaxAge =       ntohs (*((unsigned short*) b->max_age))       >> 8;
  v->ForwardDelay = ntohs (*((unsigned short*) b->forward_delay)) >> 8;
  v->HelloTime =    ntohs (*((unsigned short*) b->hello_time))    >> 8;
#endif
	unsigned short mt;
	#define STP_COPY_TIME(f, t)        \
	memcpy(&mt, f, 2); \
	mt /= 256; \
	t = ntohs(mt);
	
#if 0  
	v->MessageAge = b->message_age[0] << 8 + b->message_age[1];
	v->MessageAge = ntohs(v->MessageAge);
	v->MaxAge = b->max_age[0] << 8 + b->max_age[1];
	v->MaxAge = ntohs(v->MaxAge);
	v->ForwardDelay = b->forward_delay[0] << 8 + b->forward_delay[1];
	v->ForwardDelay = ntohs(v->ForwardDelay);
	v->HelloTime = b->hello_time[0] << 8 + b->hello_time[1];
	v->HelloTime = ntohs(v->HelloTime);
#endif
	STP_COPY_TIME(b->message_age, v->MessageAge);
	STP_COPY_TIME(b->max_age, v->MaxAge);
	STP_COPY_TIME(b->forward_delay, v->ForwardDelay);
	STP_COPY_TIME(b->hello_time, v->HelloTime);
}

void
STP_set_times (IN TIMEVALUES_T *v, OUT BPDU_BODY_T *b)
{
  unsigned short mt;
  #define STP_SET_TIME(f, t)        \
     mt = htons(f); \
     mt *= 256;\
     memcpy (t, &mt, 2); 
  
  STP_SET_TIME(v->MessageAge,   b->message_age);
  STP_SET_TIME(v->MaxAge,       b->max_age);
  STP_SET_TIME(v->ForwardDelay, b->forward_delay);
  STP_SET_TIME(v->HelloTime,    b->hello_time);
}

void 
STP_copy_times (OUT TIMEVALUES_T *t, IN TIMEVALUES_T *f)
{
  t->MessageAge = f->MessageAge;
  t->MaxAge = f->MaxAge;
  t->ForwardDelay = f->ForwardDelay;
  t->HelloTime = f->HelloTime;
}

