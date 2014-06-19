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
* FILENAME:  vos_tlv.c
*
* DESCRIPTION: Library functions for TLV API.
*	
*
* Date Created: Sep 21, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_tlv.c#1 $
* $Log:$
*
*
**************************************************************************/

/***** INCLUDE FILES *****/
#include <endian.h>

/***** LOCAL DEFINES and ENUM *****/
/***** LOCAL STRUCTURES and UNIONS *****/
/***** LOCAL TYPEDEFS ****/
/***** LOCAL (PRIVATE) PROTOTYPES *****/
/***** DATA ALLOCATION *****/

/***** PUBLIC FUNCTIONS ****/

/*******************************************************************************
*
* NAME: vosTlvByteSet
*
* DESCRIPTION: Write byte value to the address pointer b_p(big endian format).
*
* RETURN: new offset after put data to address pointer b_p
*
* INPUTS: 
*         char * b_p: address pointer 
*         unsigned int id: identifier
*         unsigned char data: byte write to address 
*         int offset: old offset before put data to address pointer b_p
*
* OUTPUTS: None
*
* SIDE EFFECTS: Caller need to make sure no buffer overflow problem.
*
*******************************************************************************/
int vosTlvByteSet(char *b_p, unsigned short id, unsigned char data, int offset)
{
	*b_p++ = (id >> 8);		// ID
	*b_p++ = id;			// ID
	*b_p++ = 0;				// Length
	*b_p++ = 1;				// Length
	*b_p = data;
	return(offset+5);
}

/*******************************************************************************
*
* NAME: vosTlvByteGet
*
* DESCRIPTION: Get byte value from the address pointer b_p.
*
* RETURN: Byte data value
*
* INPUTS: 
*         char *b_p: address pointer 
*         int *offset: old offset pointer
*
* OUTPUTS:
*         int *offset: new offset pointer
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. If the machine is big endian format, offset requires to 
*                  define int (not char, or short).  Otherwise, offset will
*                  get incorrect value.
*
*******************************************************************************/
int vosTlvByteGet(char *b_p, int *offset)
{
	unsigned int x;

	x = *b_p++;
	*offset = *offset + 1;
	return x;
}

/*******************************************************************************
*
* NAME: vosTlvShortSet
*
* DESCRIPTION: Write short integer value to the address pointer b_p
*              (big endian format).
*
* RETURN: new offset after put data to address pointer b_p
*
* INPUTS: 
*         char *b_p: address pointer 
*         unsigned short id: identifier
*         unsigned short data: short integer data write to address 
*         int offset: old offset before put data to address pointer b_p
*
* OUTPUTS: None
*
* SIDE EFFECTS: Caller need to make sure no buffer overflow problem.
*
*******************************************************************************/
int vosTlvShortSet(char *b_p, unsigned short id, unsigned short data, int offset)
{
	*b_p++ = (id >> 8);		// ID
	*b_p++ = id;			// ID
	*b_p++ = 0;				// Length
	*b_p++ = 2;				// Length
	*b_p++ = (data >> 8);
	*b_p = data;
	return(offset+6);
}

/*******************************************************************************
*
* NAME: vosTlvShortGet
*
* DESCRIPTION: Get short value from the address pointer b_p.
*
* RETURN: Short data value
*
* INPUTS: 
*         char *b_p: address pointer 
*         int *offset: old offset pointer
*
* OUTPUTS:
*         int *offset: new offset pointer
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. If the machine is big endian format, offset requires to 
*                  define int (not char, or short).  Otherwise, offset will
*                  get incorrect value.
*
*******************************************************************************/
int vosTlvShortGet(char *b_p, int *offset)
{
	unsigned int x;

	x = *b_p++;
	x = x << 8 | (*b_p++&0x000000ff);
	*offset = *offset + 2;
	return x;
}

/*******************************************************************************
*
* NAME: vosTlvIntSet
*
* DESCRIPTION: Write integer value to the address pointer b_p
*              (big endian format).
*
* RETURN: new offset after put data to address pointer b_p
*
* INPUTS: 
*         char * b_p: address pointer 
*         uint id: identifier
*         uint data: integer data write to address 
*         int offset: old offset before put data to address pointer b_p
*
* OUTPUTS: None
*
* SIDE EFFECTS: Caller need to make sure no buffer overflow problem.
*
*******************************************************************************/
int vosTlvIntSet(char *b_p, unsigned short id, unsigned int data, int offset)
{
	*b_p++ = (id >> 8);		// ID
	*b_p++ = id;			// ID
	*b_p++ = 0;				// Length
	*b_p++ = 4;				// Length
	*b_p++ = (data >> 24);
	*b_p++ = (data >> 16);
	*b_p++ = (data >> 8);
	*b_p = data;
	return(offset+8);
}

/*******************************************************************************
*
* NAME: vosTlvIntGet
*
* DESCRIPTION: Get integer value from the address pointer b_p.
*
* RETURN: Integer data value
*
* INPUTS: 
*         char * b_p: address pointer 
*         int *offset: old offset pointer
*
* OUTPUTS:
*         int *offset: new offset pointer
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. If the machine is big endian format, offset requires to 
*                  define int (not char, or short).  Otherwise, offset will
*                  get incorrect value.
*
*******************************************************************************/
int vosTlvIntGet(char *b_p, int *offset)
{
	unsigned int x;

	x = *b_p++;
	x = x << 8 | (*b_p++&0x000000ff);
	x = x << 8 | (*b_p++&0x000000ff);
	x = x << 8 | (*b_p++&0x000000ff);
	*offset = *offset + 4;
	return x;
}

/*******************************************************************************
*
* NAME: vosTlvArraySet
*
* DESCRIPTION: Write data array data to the address pointer b_p
*              (big endian format).
*
* RETURN: new offset after put data to address pointer b_p
*
* INPUTS: 
*         char *b_p: address pointer 
*         uint id: identifier
*         char *data_p: byte array data write to address 
*         int data_len: data length
*         int offset: old offset before put data to address pointer b_p
*
* OUTPUTS: None
*
* SIDE EFFECTS: Caller need to make sure no buffer overflow problem.
*
*******************************************************************************/
int vosTlvArraySet(char *b_p, unsigned short id, char *data_p, int data_len, int offset)
{
	int i;

	*b_p++ = (id >> 8);		// ID
	*b_p++ = id;			// ID
	*b_p++ = (data_len >> 8);
	*b_p++ = data_len;		// Length
	for (i=0;i<data_len;i++) {
		*b_p++ = *data_p++ ;
	}
	return(offset+data_len+4);
}

/*******************************************************************************
*
* NAME: vosTlvArrayGet
*
* DESCRIPTION: Get data array data from the address pointer b_p.
*
* RETURN: None
*
* INPUTS: 
*         char *b_p: address pointer 
*         int data_len: data length
*         int *offset: old offset pointer
*
* OUTPUTS:
*         char *data_p: byte array data 
*         int *offset: new offset pointer
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. If the machine is big endian format, offset requires to 
*                  define int (not char, or short).  Otherwise, offset will
*                  get incorrect value.
*
*******************************************************************************/
void vosTlvArrayGet(char *b_p, char *data_p, int data_len, int *offset)
{
	int i;

	for (i=0;i<data_len;i++) {
		*data_p++ = *b_p++ ;
	}
	*offset = *offset + data_len;
}

/*******************************************************************************
*
* NAME: vosTlvLenSet
*
* DESCRIPTION: Write data value to the address pointer b_p(big endian format).
*
* RETURN: new offset after put data to address pointer b_p
*
* INPUTS: 
*         char * b_p: address pointer 
*         unsigned int id: identifier
*         unsigned int data: data write to address 
*         int data_len: data length
*         int offset: old offset before put data to address pointer b_p
*
* OUTPUTS: None
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. Data length can't bigger than 4 bytes.  If bigger than
*                  4 bytes, use vosTlvArraySet.
*
*******************************************************************************/
int vosTlvLenSet(char *b_p, unsigned short id, unsigned int data, int data_len, int offset)
{
	if (data_len > 4)
		return(offset);  // Invalid data
	*b_p++ = (id >> 8);		// ID
	*b_p++ = id;			// ID
	*b_p++ = 0;				// Length
	*b_p++ = data_len;		// Length
	switch (data_len) {
	case 4:
		*b_p++ = (data >> 24);
		*b_p++ = (data >> 16);
		*b_p++ = (data >> 8);
		*b_p = data;
		break;
	case 3:
		*b_p++ = (data >> 16);
		*b_p++ = (data >> 8);
		*b_p = data;
		break;
	case 2:
		*b_p++ = (data >> 8);
		*b_p = data;
		break;
	case 1:
		*b_p = data;
		break;
	}
	return(offset+data_len+4);
}

/*******************************************************************************
*
* NAME: vosTlvLenGet
*
* DESCRIPTION: Get data from the address pointer b_p.
*
* RETURN: data value
*
* INPUTS: 
*         char * b_p: address pointer 
*         data_len: data offset 
*         int *offset: old offset pointer
*
* OUTPUTS:
*         int *offset: new offset pointer
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. Data length can't bigger than 4 bytes.  If bigger than
*                  4 bytes, use vosTlvArrayGet.
*               3. If the machine is big endian format, offset requires to 
*                  define int (not char, or short).  Otherwise, offset will
*                  get incorrect value.
*
*******************************************************************************/
int vosTlvLenGet(char *b_p, int data_len, int *offset)
{
	unsigned int x = 0;

	if (data_len > 4)
		return(0);
	switch (data_len) {
	case 1:
		x = *b_p++;
		break;
	case 2:
		x = *b_p++;
		x = x << 8 | (*b_p++&0x000000ff);
		break;
	case 3:
		x = *b_p++;
		x = x << 8 | (*b_p++&0x000000ff);
		x = x << 8 | (*b_p++&0x000000ff);
		break;
	case 4:
		x = *b_p++;
		x = x << 8 | (*b_p++&0x000000ff);
		x = x << 8 | (*b_p++&0x000000ff);
		x = x << 8 | (*b_p++&0x000000ff);
		break;
	}
	*offset = *offset + data_len;
	return x;
}

/*******************************************************************************
*
* NAME: vosTlvUnknownGet
*
* DESCRIPTION: Handle unknown attribute from the address pointer b_p.
*
* RETURN: data value
*
* INPUTS: 
*         char * b_p: address pointer 
*         data_len: data length 
*         int *offset: old offset pointer
*
* OUTPUTS:
*         int *offset: new offset pointer
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. If the machine is big endian format, offset requires to 
*                  define int (not char, or short).  Otherwise, offset will
*                  get incorrect value.
*
*******************************************************************************/
int vosTlvUnknownGet(char *b_p, int data_len, int *offset)
{
	*offset = *offset + data_len;
	return 0;
}

/*******************************************************************************
*
* NAME: vosTlvLastEntrySet
*
* DESCRIPTION: Write empty entry to the address pointer b_p.
*
* RETURN: new offset after put last entry to address pointer b_p
*
* INPUTS: 
*         char * b_p: address pointer 
*         int offset: old offset before put data to address pointer b_p
*
* OUTPUTS: None
*
* SIDE EFFECTS: 1. Caller need to make sure no buffer overflow problem.
*               2. Caller must use this one for last TLV entry.
*
*******************************************************************************/
int vosTlvLastEntrySet(char *b_p, int offset)
{
	*b_p++ = 0; // ID
	*b_p++ = 0; // ID
	*b_p++ = 0;	// Size
	*b_p = 0;  // Size
	return(offset+4);
}

/*******************************************************************************
*
* NAME: vosTlvSetByte
*
* DESCRIPTION: Write byte value to the address pointer b_p.
*
* RETURN: None
*
* INPUTS: 
*         char * b_p: address pointer 
*         int x: interger write to address 
*
* OUTPUTS: None
*
* SIDE EFFECTS: Caller need to make sure no buffer overflow problem.
*
*******************************************************************************/
void vosTlvSetByte(char *b_p, int x) 
{
	*b_p++ = x;
}

/*******************************************************************************
*
* NAME: vosTlvGetByte
*
* DESCRIPTION: Read byte value from the address pointer b_p.
*
* RETURN: integer value
*
* INPUTS: 
*         char * b_p: address pointer 
*
* OUTPUTS: None
*
* SIDE EFFECTS: Caller need to make sure no buffer overflow problem.
*
*******************************************************************************/
int vosTlvGetByte(char *b_p) 
{
	unsigned int x;

	x = *b_p++;
	return x;
}

/*******************************************************************************
*
* NAME: vosTlvSetShort
*
* DESCRIPTION: Write short integer value to the address pointer b_p.
*
* RETURN: None
*
* INPUTS: 
*         char * b_p: address pointer 
*         int x: short interger write to address 
*
* OUTPUTS: None
*
* SIDE EFFECTS: This is big endian format, not little endian format(Intel).
*
*******************************************************************************/
void vosTlvSetShort(char *b_p, int x) 
{
	*b_p++ = (x >> 8);
	*b_p++ = x;
}

/*******************************************************************************
*
* NAME: vosTlvGetShort
*
* DESCRIPTION: Read short integer value from the address pointer b_p.
*
* RETURN: integer value
*
* INPUTS: 
*         char * b_p: address pointer 
*
* OUTPUTS: None
*
* SIDE EFFECTS: This is big endian format, not little endian format(Intel).
*
*******************************************************************************/
int vosTlvGetShort(char *b_p) 
{
	unsigned int x;

	x = *b_p++;
	x = x << 8 | (*b_p++&0x000000ff);
	return x;
}

/*******************************************************************************
*
* NAME: vosTlvSetInt
*
* DESCRIPTION: Write integer value to the address pointer b_p.
*
* RETURN: None
*
* INPUTS: 
*         char * b_p: address pointer 
*         int x: interger write to address 
*
* OUTPUTS: None
*
* SIDE EFFECTS: This is big endian format, not little endian format(Intel).
*
*******************************************************************************/
void vosTlvSetInt(char *b_p, int x) 
{
	*b_p++ = (x >> 24);
	*b_p++ = (x >> 16);
	*b_p++ = (x >> 8);
	*b_p++ = x;
}

/*******************************************************************************
*
* NAME: vosTlvGetInt
*
* DESCRIPTION: Read integer value from the address pointer b_p.
*
* RETURN: integer value
*
* INPUTS: 
*         char * b_p: address pointer 
*
* OUTPUTS: None
*
* SIDE EFFECTS: This is big endian format, not little endian format(Intel).
*
*******************************************************************************/
int vosTlvGetInt(char *b_p) 
{
	unsigned int x;

	x = *b_p++;
	x = x << 8 | (*b_p++&0x000000ff);
	x = x << 8 | (*b_p++&0x000000ff);
	x = x << 8 | (*b_p++&0x000000ff);
	return x;
}

void vosTlvSetArray(char *b_p,char *data_p, int data_len)
{
	int i;
	for (i=0;i<data_len;i++) {
		*b_p++ = *data_p++ ;
	}
}

/***** PRIVATE FUNCTIONS *****/
/* END FILE */
