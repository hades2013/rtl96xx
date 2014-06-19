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
**    FILE       :  rstp_bitmap.h
**      
**    DESCRIPTION:  This file contains the bitmap definitions and functions of 
**    bridge ports.
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

#ifndef __BITMAP_H
#define __BITMAP_H

/* 1024 port
#define BitmapClear(BitmapPtr) \
        {memset(BitmapPtr, 0x00, 128);}

#define BitmapSetAllBits(BitmapPtr) \
        {memset(BitmapPtr, 0xFF, 128);}
*/
#ifdef ONU_1PORT
#define NUMBER_OF_PORTS  1 // rzhou debug 4
#else
#define NUMBER_OF_PORTS	 4
#endif

typedef struct tagBITMAP
{
  unsigned long part[1];     /* Least Significant part */
} BITMAP_T;

#define BitmapClear(BitmapPtr) \
			 {memset(BitmapPtr, 0x00000000, 4);}

#define BitmapSetAllBits(BitmapPtr) \
        {memset(BitmapPtr, 0x0000000F, 4);}
      
#define BitmapGetBit(BitmapPtr,Bit) \
         ((BitmapPtr)->part[(Bit)/(8*sizeof(unsigned long))] & (1 << ((Bit)%(8*sizeof(unsigned long)))))

#define BitmapSetBit(BitmapPtr,Bit) \
	{(BitmapPtr)->part[(Bit)/(8*sizeof(unsigned long))] |= (1 << ((Bit)%(8*sizeof(unsigned long))));}

#define BitmapClearBit(BitmapPtr,Bit) \
(BitmapPtr)->part[(Bit)/(8*sizeof(unsigned long))] &= ~(1 << ((Bit)%(8*sizeof(unsigned long))));

#endif /* __BITMAP_H */

