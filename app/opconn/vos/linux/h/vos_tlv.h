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
* FILENAME:  vos_tlv.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_tlv.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_TLV_H_
#define __VOS_TLV_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
/***** INCLUDE FILES *****/
/***** DEFINES and ENUMS *****/

#define VOS_TLV_DATA_SIZE 1024
#define VOS_TLV_HEADER_SIZE 24	// VOS_TLV_MSG - data field

#define VOS_TLV_SEQ_NO_OFFSET 4
#define VOS_TLV_RTN_SEQ_NO_OFFSET 8

#define VOS_TLV_EMPTY_ENTRY_SIZE 4

#define VOS_TLV_SUCCESS		0
#define VOS_TLV_EMPTY_ENTRY	-21

#define VOS_TLV_MAJOR_VERSION 1
#define VOS_TLV_MINOR_VERSION 3

/* The following two lines fixed by top level Makefile, don't change it */

#define VOS_TLV_RELEASE_VERSION 0
#define VOS_TLV_UPDATE_VERSION 0


/***** STRUCTURES and UNIONS *****/
/***** TYPEDEFS *****/

typedef struct VOS_TLV_MSG_s {
	unsigned char major_version;
	unsigned char minor_version;
	unsigned char release_version;
	unsigned char update_version;
	unsigned long seq_no;       // VOS_TLV_SEQ_NO_OFFSET
	unsigned long rtn_seq_no;	// VOS_TLV_RTN_SEQ_NO_OFFSET return seq no from peer
    int rtn_code;               // Error return code
	int event_id;    			// Command type
	int attribute_len;			// Attribute length
	char data[VOS_TLV_DATA_SIZE];/* Attributes (ID/length/data) */
} VOS_TLV_MSG_t; 

/***** EXTERNS *****/
/***** PROTOTYPES *****/
int/* new offset */ vosTlvByteSet(char * /* address */, unsigned short/* ID */, unsigned char/* data */, int/* old offset */);
int/* data */ vosTlvByteGet(char * /* address */, int * /* offset */);
int/* new offset */ vosTlvShortSet(char * /* address */, unsigned short/* ID */, unsigned short/* data */, int/* old offset */);
int/* data */ vosTlvShortGet(char * /* address */, int * /* offset */);
int/* new offset */ vosTlvIntSet(char * /* address */, unsigned short/* ID */, unsigned int/* data */, int/* old offset */);
int/* data */ vosTlvIntGet(char * /* address */, int * /* offset */);
int/* new offset */ vosTlvArraySet(char * /* address */, unsigned short /* ID */, char * /* data */, int /* data length */, int /* offset */);
void vosTlvArrayGet(char * /* address */, char * /* data */, int/* data length */, int * /* offset */);
int/* new offset */ vosTlvLenSet(char * /* address */, unsigned short /* ID */, unsigned int /* data */, int /* data length */, int/* old offset */);
int/* data */ vosTlvLenGet(char * /* address */, int/* data length */, int * /* offset */);
int/* 0 */ vosTlvUnknownGet(char * /* address */, int/* data length */, int * /* offset */);
int/* new offset */ vosTlvLastEntrySet(char * /* address */, int/* old offset */);
void vosTlvSetByte(char * /* address */, int/* data */);
int/* data */ vosTlvGetByte(char * /* address */);
void vosTlvSetShort(char * /* address */, int/* data */);
int/* data */ vosTlvGetShort(char * /* address */);
void vosTlvSetInt(char * /* address */, int/* data */);
int/* data */ vosTlvGetInt(char * /* address */);
void vosTlvSetArray(char *b_p,char *data_p, int data_len);

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_TLV_H_ */

