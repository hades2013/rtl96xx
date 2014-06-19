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
//#include <vos_types.h>

#define VOS_BITMAP_MAIN

#include <vos_bitmap.h>
#include <vos_types.h>


UINT16 vosFirstSetBitPosGet( UINT32 Value, UINT32 StartPosition )
{  
   //UINT16 BitPosition;
   //FIRST_SET( BitPosition, Value, StartPosition );
   //return BitPosition;
   
  // First mask out the unrelated bit
  Value &= aMaskBit[StartPosition];

  if( Value == 0 ) return INVALID_16;

  // Apply binary search here
  if( Value > 0x0000FFFF )  // it is in 0=10000 - 0xFFFF0000 range
  {
     if( Value > 0x00FFFFFF )       // 1ffffff to ffffffff
        return  aSetBit[*(UINT8 *)(&Value )] - 1 ;
     else
       return aSetBit[*((UINT8 *)(&Value )+1)] + 7; // 1FFFF to FF FFFF
  } else
  {
     if( Value > 0x000000FF ) // it is in 0x1ff - 0xffff range
        return aSetBit[*((UINT8 *)(&Value ) + 2)] + 15;
     else
        return aSetBit[*((UINT8 *)(&Value ) + 3)] +23;
  }

};

UINT16 vosFirstSetBitPosGet1( UINT32 Value, UINT32 StartPosition )
{
   UINT32 Val;
   VOS_FIRST_SET_BIT_POS_GET( Val, Value, StartPosition);
   return Val;
}

UINT16
vosFirstClearBitPosGet( UINT32 Value, UINT32 StartPosition )
{
   Value ^= 0xFFFFFFFF;  // Exclusive or the bit
   return vosFirstSetBitPosGet(Value, StartPosition);
};

UINT32
vosBitClear( UINT32 OriginalValue, UINT32 BitPosition )
{
   return OriginalValue & aClearBitMap[BitPosition];
};

UINT32
vosBitSet( UINT32 OriginalValue, UINT32 BitPosition )
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
UINT32 vosSetBitsGet(UINT32 Value)
{
    UINT32 cnt = 0;
    UINT16 i;
    
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
UINT32 vosClearBitsGet(UINT32 Value)
{
    UINT32 cnt = 0;
    UINT16 i;
    
    for (i = 0; i < 32; i++)
    {
        if (VOS_IS_BIT_CLEARED(i, Value))
            cnt++;
    }
    return cnt;
}
