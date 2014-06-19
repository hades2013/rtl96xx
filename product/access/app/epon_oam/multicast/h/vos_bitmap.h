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
* FILENAME:  vos_bitmap.h
*
* DESCRIPTION: 
*	The bit map set bit in a 32-bit value.
*   starting from the left most bit as bit-one, and right most bit as
*   bit-32. The bit position is zero-based and 16 bits.
*
* Date Created: Apr 21, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_bitmap.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_BITMAP_H_
#define __VOS_BITMAP_H_

#ifdef __cplusplus
extern "C"
{
#endif
 
//#include <vos_types.h>
#include <lw_type.h>

UINT16 vosFirstSetBitPosGet( UINT32 Value, UINT32 StartPosition );
UINT16 vosLastSetBitPosGet( UINT32 Value);
UINT16 vosFirstClearBitPosGet( UINT32 Value, UINT32 StartPosition );
UINT32 vosBitClear( UINT32 OriginalValue, UINT32 BitPosition );
UINT32 vosBitSet( UINT32 OriginalValue, UINT32 BitPosition );
UINT32 vosSetBitsGet(UINT32 Value);
UINT32 vosClearBitsGet(UINT32 Value);

enum{   FIRST_BIT_POSITION = 0 };
 
#ifdef VOS_BITMAP_MAIN

UINT16
aSetBit[] =
{
   0, 
   8,
   7,7,
   6,6,6,6,
   5,5,5,5,5,5,5,5, 
   4,4,4,4,4,4,4,4, 4,4,4,4,4,4,4,4,
   3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,

   2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
   2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2, 
   
   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1
};

UINT8
aReverseBit[] = 
{
   0x0, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
   0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
   0x8, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
   0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
   0x4, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
   0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
   0xC, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
   0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
   0x2, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
   0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
   0xA, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
   0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
   0x6, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
   0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
   0xE, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
   0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
   0x1, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
   0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
   0x9, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
   0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
   0x5, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
   0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
   0xD, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
   0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
   0x3, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
   0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
   0xB, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
   0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
   0x7, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
   0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
   0xF, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
   0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

UINT32 aMaskBit[] =
{ 0xFFFFFFFF, 0x7FFFFFFF, 0x3FFFFFFF,  0x1FFFFFFF, 0x0FFFFFFF, 
  0x07FFFFFF, 0x03FFFFFF,  0x01FFFFFF, 0x00FFFFFF,
  0x007FFFFF, 0x003FFFFF,  0x001FFFFF, 0x000FFFFF, 
  0x0007FFFF, 0x0003FFFF,  0x0001FFFF, 0x0000FFFF,
  0x00007FFF, 0x00003FFF,  0x00001FFF, 0x00000FFF,
  0x000007FF, 0x000003FF,  0x000001FF, 0x000000FF,
  0x0000007F, 0x0000003F,  0x0000001F, 0x0000000F,
  0x00000007, 0x00000003,  0x00000001, 0x00000000
};

UINT32 aClearBitMap[] =
{
   0x7FFFFFFF, 0xBFFFFFFF, 0xDFFFFFFF, 0xEFFFFFFF,
   0xF7FFFFFF, 0xFBFFFFFF, 0xFDFFFFFF, 0xFEFFFFFF,
   0xFF7FFFFF, 0xFFBFFFFF, 0xFFDFFFFF, 0xFFEFFFFF,
   0xFFF7FFFF, 0xFFFBFFFF, 0xFFFDFFFF, 0xFFFEFFFF,
   0xFFFF7FFF, 0xFFFFBFFF, 0xFFFFDFFF, 0xFFFFEFFF,
   0xFFFFF7FF, 0xFFFFFBFF, 0xFFFFFDFF, 0xFFFFFEFF,   
   0xFFFFFF7F, 0xFFFFFFBF, 0xFFFFFFDF, 0xFFFFFFEF,
   0xFFFFFFF7, 0xFFFFFFFB, 0xFFFFFFFD, 0xFFFFFFFE,   
};
#else
extern UINT32 aClearBitMap[];
extern UINT16 aSetBit[];
extern UINT32 aMaskBit[];
extern UINT8  aReverseBit[];
#endif

/* this macro reverse the bit position of a UINT32 data*/
#define VOS_UIN32_VALUE_REVERSE( Value ) \
{ \
   UINT8 Temp = (Value); \
   UINT32 Val; \
   Val = aReverseBit[Temp]; \
   Val = Val << 8; \
   Temp = (Value) >> 8; \
   Val += aReverseBit[Temp]; \
   Val = Val << 8; \
   Temp = (Value) >> 16; \
   Val += aReverseBit[Temp]; \
   Val = Val << 8; \
   Temp = (Value) >> 24; \
   Val += aReverseBit[Temp]; \
   (Value) = Val; \
}


/*
// this macro modifies the BitPosition variable and sets to the bit position of the 
// first set bit, following the StartPosition, in the Value bit map.  Please not that the Value
// bitmap itself will not get modified by this macro.
*/
#define VOS_FIRST_SET_BIT_POS_GET( BitPosition, Value, StartPosition  ) \
{ \
  UINT32 TempVal = (Value) & aMaskBit[StartPosition]; \
  if( (TempVal) == 0 ) (BitPosition) = INVALID_16; \
  else {\
  if( TempVal > 0x0000FFFF )  \
     if( TempVal > 0x00FFFFFF ) \
        (BitPosition) = aSetBit[*(UINT8 *)(&TempVal )] - 1 ; \
     else \
        (BitPosition) = aSetBit[*((UINT8 *)(&TempVal )+1)] + 7; \
   else \
     if( (TempVal) > 0x000000FF ) \
         (BitPosition) = aSetBit[*((UINT8 *)(&TempVal ) + 2)] + 15; \
     else \
         (BitPosition) = aSetBit[*((UINT8 *)(&TempVal ) + 3)] +23; \
  } \
}
 
/*
// this macro modifies the BitPosition variable and sets to the bit position of the 
// first clear bit, following the StartPosition, in the Value bit map.  Please not that the Value
// bitmap itself will not get modified by this macro.
*/
#define VOS_FIRST_CLEAR_BIT_POS_GET( BitPosition, Value , StartPosition ) \
{\
  UINT32 Bit = (Value) ^ 0xFFFFFFFF; \
  Bit &= aMaskBit[(StartPosition)]; \
  if( Bit == 0 ) (BitPosition) = INVALID_16; \
  else {\
  if( Bit > 0x0000FFFF )  \
     if( Bit > 0x00FFFFFF ) \
        (BitPosition) = aSetBit[*(UINT8 *)(&Bit )] - 1 ; \
     else \
        (BitPosition) = aSetBit[*((UINT8 *)(&Bit )+1)] + 7; \
   else \
     if( Bit > 0x000000FF ) \
         (BitPosition) = aSetBit[*((UINT8 *)(&Bit ) + 2)] + 15; \
     else \
         (BitPosition) = aSetBit[*((UINT8 *)(&Bit ) + 3)] +23; \
  } \
}    


/*
// The following two macro's return 0 if true or none 0 value if the condition is not true.
// Also, the user MUST check to make sure that the bit position is < 32 or the results will be
// unpredictable!!!
*/
#define VOS_IS_BIT_SET( BitPosition, Val ) \
( (Val) & ~aClearBitMap[(BitPosition)] )

#define VOS_IS_BIT_CLEARED( BitPosition, Val ) \
( ((Val) & ~aClearBitMap[(BitPosition)]) ^ ~aClearBitMap[BitPosition] )

#define VOS_BIT_CLEAR( OriginalValue, BitPosition ) \
{\
   if( (BitPosition) < 32 ) \
      (OriginalValue) &= aClearBitMap[(BitPosition)]; \
}

#define VOS_BIT_SET( OriginalValue, BitPosition ) \
{\
	if( (BitPosition) < 32 ) \
      (OriginalValue) |= ~aClearBitMap[(BitPosition)]; \
}


#ifdef __cplusplus
}
#endif

#endif /* #ifndef __VOS_BITMAP_H_ */

