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
* FILENAME:  hal_host.h
*
* DESCRIPTION: 
*	
*
* Date Created: Mar 18, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/h/hal_host.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __HAL_HOST_H_
#define __HAL_HOST_H_
 
#ifdef __cplusplus
extern "C" {
#endif


#pragma pack (1)

/* API */

/* Host inbound header definition */
typedef struct HAL_API_HOST_INBOUND_HDR_s
{
#ifndef OPL_BIG_ENDIANESS
    UINT32  iport:4,        /* Inbound  port index */
            reserved:28;    /* Reserved */
#else
    UINT32  reserved:28,    /* Reserved */
            iport:4;        /* Inbound  port index */
#endif
} HAL_API_HOST_INBOUND_HDR_t;

/* Host outbound header definition */
typedef struct HAL_API_HOST_OUTBOUND_HDR_s
{
#ifndef OPL_BIG_ENDIANESS
    UINT8   eport:4,        /* Inbound  port index */
            icos:3,         /* ICOS */
            reserved:1;     /* Reserved */

#else
    UINT8   reserved:1,     /* Reserved */
            icos:3,         /* ICOS */
            eport:4;        /* Inbound  port index */  
#endif
} HAL_API_HOST_OUTBOUND_HDR_t;

typedef struct HAL_API_HOST_ETHER_FROM_CPU_DSA_TAG_RSV_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_ETHER_FROM_CPU_DSA_TAG_RSV_t;

typedef struct HAL_API_HOST_ETHER_TO_CPU_DSA_TAG_RSV_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_ETHER_TO_CPU_DSA_TAG_RSV_t;

typedef struct HAL_API_HOST_ETHER_FROM_CPU_DSA_TAG_IGMP_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_ETHER_FROM_CPU_DSA_TAG_IGMP_t;

typedef struct HAL_API_HOST_ETHER_TO_CPU_DSA_TAG_IGMP_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short tpid;
    unsigned short vlanInfo;

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_ETHER_TO_CPU_DSA_TAG_IGMP_t;

typedef struct HAL_API_HOST_FROM_CPU_DSA_TAG_RSV_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_FROM_CPU_DSA_TAG_RSV_t;

typedef struct HAL_API_HOST_TO_CPU_DSA_TAG_RSV_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_TO_CPU_DSA_TAG_RSV_t;

typedef struct HAL_API_HOST_FROM_CPU_DSA_TAG_IGMP_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_FROM_CPU_DSA_TAG_IGMP_t;

typedef struct HAL_API_HOST_TO_CPU_DSA_TAG_IGMP_s
{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short tpid;
    unsigned short vlanInfo;

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
} HAL_API_HOST_TO_CPU_DSA_TAG_IGMP_t;


typedef enum HAL_API_HOST_PKT_TYPE_e
{
	HAL_API_HOST_PKT_TYPE_OAM,
	HAL_API_HOST_PKT_TYPE_STP,
	HAL_API_HOST_PKT_TYPE_IGMP,
	HAL_API_HOST_PKT_TYPE_END
} HAL_API_HOST_PKT_TYPE_t;


typedef struct HAL_API_HOST_SEND_TO_UPLINK_s
{
    UINT16  usPortNum;
    VOID    *pvPacket;
    UINT16  usPacketLen;
    UINT32  ulResult;
} HAL_API_HOST_SEND_TO_UPLINK_t;


typedef struct HAL_API_HOST_SEND_TO_DOWNLINK_s
{
    UINT16  usPortNum;
    UINT16  usWithTag;
    HAL_API_HOST_PKT_TYPE_t stPacketType;
    VOID    *pvPacket;
    UINT16  usPacketLen;
    UINT32  ulResult;
} HAL_API_HOST_SEND_TO_DOWNLINK_t;


typedef struct HAL_API_HOST_SEND_s
{
    VOID    *pvPacket;
    UINT16  usPacketLen;
    UINT32  ulResult;
} HAL_API_HOST_SEND_t;


typedef struct HAL_API_HOST_DUMP_EN_s
{
    UINT32 ulEnable;
} HAL_API_HOST_DUMP_EN_t;


typedef struct HAL_API_HOST_COUNTER_SHOW_s
{
    UINT32 ulFd;
} HAL_API_HOST_COUNTER_SHOW_t;



/* EVENT */

typedef struct HAL_EVENT_HOST_HANDLING_s
{
    VOID    *pvPacket;
    UINT16  usPacketLen;
} HAL_EVENT_HOST_HANDLING_t;


#pragma pack ()



#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __HAL_HOST_H_ */
 

