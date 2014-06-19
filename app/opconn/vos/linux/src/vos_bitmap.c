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
* FILENAME:  bitmap.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_bitmap.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <vos_types.h>

#define VOS_BITMAP_MAIN

#include <vos_bitmap.h>

uint16 vosFirstSetBitPosGet( uint32 Value, uint32 StartPosition )
{  
   //uint16 BitPosition;
   //FIRST_SET( BitPosition, Value, StartPosition );
   //return BitPosition;
   
  // First mask out the unrelated bit
  Value &= aMaskBit[StartPosition];

  if( Value == 0 ) return INVALID_16;

  // Apply binary search here
  if( Value > 0x0000FFFF )  // it is in 0=10000 - 0xFFFF0000 range
  {
     if( Value > 0x00FFFFFF )       // 1ffffff to ffffffff
        return  aSetBit[*(uint8 *)(&Value )] - 1 ;
     else
       return aSetBit[*((uint8 *)(&Value )+1)] + 7; // 1FFFF to FF FFFF
  } else
  {
     if( Value > 0x000000FF ) // it is in 0x1ff - 0xffff range
        return aSetBit[*((uint8 *)(&Value ) + 2)] + 15;
     else
        return aSetBit[*((uint8 *)(&Value ) + 3)] +23;
  }

};

uint16 vosFirstSetBitPosGet1( uint32 Value, uint32 StartPosition )
{
   uint32 Val;
   VOS_FIRST_SET_BIT_POS_GET( Val, Value, StartPosition);
   return Val;
}

uint16
vosFirstClearBitPosGet( uint32 Value, uint32 StartPosition )
{
   Value ^= 0xFFFFFFFF;  // Exclusive or the bit
   return vosFirstSetBitPosGet(Value, StartPosition);
};

uint32
vosBitClear( uint32 OriginalValue, uint32 BitPosition )
{
   return OriginalValue & aClearBitMap[BitPosition];
};

uint32
vosBitSet( uint32 OriginalValue, uint32 BitPosition )
{
   return OriginalValue | ~aClearBitMap[BitPosition];
};


/*******************************************************************************
*
* vosSetBitsGet:	
*
* DESCRIPTION:
* 	get the count of set bits in the 32-bits value
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
uint32 vosSetBitsGet(uint32 Value)
{
    uint32 cnt = 0;
    uint16 i;
    
    for (i = 0; i < 32; i++)
    {
        if (VOS_IS_BIT_SET(i, Value))
            cnt++;
    }
    return cnt;
}

/*******************************************************************************
*
* vosClearBitsGet:	 
*
* DESCRIPTION:
* 	get the count of cleared bits in the 32-bits value
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
uint32 vosClearBitsGet(uint32 Value)
{
    uint32 cnt = 0;
    uint16 i;
    
    for (i = 0; i < 32; i++)
    {
        if (VOS_IS_BIT_CLEARED(i, Value))
            cnt++;
    }
    return cnt;
}
