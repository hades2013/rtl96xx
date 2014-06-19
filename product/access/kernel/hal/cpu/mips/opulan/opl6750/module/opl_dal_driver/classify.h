/*
=============================================================================
Header Name:cls.h

  General Description:
  
	This file define all opcomm chip regisger and table address.
	===============================================================================
	Opulan Confidential Proprietary                     
	ID and version: xxxxxxxxxxxxxx  Version 1.00
	(c) Copyright Opulan XXXX - XXXX, All Rights Reserved
	
	  
		Revision History:
		Modification
		Author			Date				Description of Changes
		---------------	---------------	----------------------------------------------
		zzhu			2007/10/09		Initial Version
		---------------	---------------	----------------------------------------------
*/
#ifndef OPCONN_CLASSIFY_H
#define OPCONN_CLASSIFY_H

#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

#define ONLY_SUPPORT_SINGLE_TAG    1

#define CLASSIFY_ADDRESS_LENGTH        	 	0X7F
#define CLASSIFY_RULE_PUCKER_LEN       		12	 /*pucker 64 bytes rule into 16 bytes by XOR.*/
#define CLASSIFY_CAL_VALUE_LEN         	 	16
#define CLASSIFY_MAX_RULE_ENTRY          	TAB_CLS_RULE_LEN
#define CLASSIFY_MIN_MASK_ENTRY          	8
#define CLASSIFY_MAX_MASK_ENTRY          	TAB_CLS_MASK_LEN
#define CLASSIFY_MAX_ADDR_ENTRY          	TAB_CLS_ADDR_LEN
#define CLASSIFY_PACKET_HEADER_LEN       80  /*the first 80 bytes of the packet*/

#define CLASSIFY_6OFFSET_LENGTH          24  /*24Bytes,Total length of the six offset length*/
#define CLASSIFY_5OFFSET_LENGTH          20  /*20Bytes,Total length of the five offset length*/
#define CLASSIFY_4OFFSET_LENGTH          16  /*16Bytes,Total length of the four offset length*/
#define CLASSIFY_3OFFSET_LENGTH          12  /*12Bytes,Total length of the three offset length*/
#define CLASSIFY_2OFFSET_LENGTH          8   /*8Bytes,Total length of the two offset length*/
#define CLASSIFY_1OFFSET_LENGTH          4   /*4Bytes,Total length of the one offset length*/

#define CLASSIFY_KEYVALUE_LENGTH     	CLASSIFY_6OFFSET_LENGTH

#define CLASSIFY_RULE_TABLE_GROUP    	4 /*opconn only support 2 group*/ 
#define CLASSIFY_OFFSET_MAX_STEP_VALUE	 12
#define CLASSIFY_OFFSET_LEN		 		 	4
#define CLASSIFY_ENTRYS_PER_GROUP    32
#define CLASSIFY_FIELD_MS_BIT            	0x80000000  /*Most significant bit of the field map*/

#define CLASSIFY_MSEL_SET_FLAG           1
#define CLASSIFY_MSEL_CLR_FLAG           2

#define CLASSIFY_LOCAL_TBL               		1
#define CLASSIFY_HW_TBL                  		2
#define CLASSIFY_OFFSET_NUM            		CLASSIFY_FOFFSET_6

#define CLASSIFY_TRAN_ACTION_COPY		 			OPL_REG_BIT0
#define CLASSIFY_TRAN_ACTION_DROP		 			OPL_REG_BIT1
#define CLASSIFY_TRAN_ACTION_REDIRECTUNI 		OPL_REG_BIT2
#define CLASSIFY_TRAN_ACTION_REDIRECTALL 		OPL_REG_BIT3
#define CLASSIFY_TRAN_ACTION_MIRROR		 		OPL_REG_BIT4
#define CLASSIFY_TRAN_ACTION_NOTCOPY	 			OPL_REG_BIT5
#define CLASSIFY_TRAN_ACTION_NOTDROP	 			OPL_REG_BIT6

#define CLASSIFY_OPTRAN_ACTION_NOTCOPY	 		OPL_REG_BIT2
#define CLASSIFY_OPTRAN_ACTION_NOTDROP	 		OPL_REG_BIT3

#define CLASSIFY_RMK_ACTION_SET_DEI		 		 	OPL_REG_BIT0
#define CLASSIFY_RMK_ACTION_CHANGE_IDEI	 	OPL_REG_BIT1
#define CLASSIFY_RMK_ACTION_CHANGE_V4TOS		OPL_REG_BIT2
#define CLASSIFY_RMK_ACTION_CHANGE_V6TOS 	OPL_REG_BIT3
#define CLASSIFY_RMK_ACTION_CHANGE_ICOS  		OPL_REG_BIT4
#define CLASSIFY_RMK_ACTION_CHANGE_SCOS  		OPL_REG_BIT5
#define CLASSIFY_RMK_ACTION_CHANGE_CCOS  	OPL_REG_BIT6
#define CLASSIFY_RMK_ACTION_CHANGE_CVID  		OPL_REG_BIT7
#define CLASSIFY_RMK_ACTION_CHANGE_SVID  		OPL_REG_BIT8

#define CLASSIFY_RMK_IDEI_FROM_RULE      			0
#define CLASSIFY_RMK_IDEI_FROM_SCOS      			OPL_REG_BIT0
#define CLASSIFY_RMK_IDEI_FROM_V4DSCP    		OPL_REG_BIT1
#define CLASSIFY_RMK_IDEI_FROM_V6DSCP    		((OPL_REG_BIT0) | (OPL_REG_BIT1))

#define CLASSIFY_RMK_TOS_FROM_RULE       			0
#define CLASSIFY_RMK_TOS_FROM_SCOS       			OPL_REG_BIT0
#define CLASSIFY_RMK_TOS_FROM_CCOS       			OPL_REG_BIT1

#define CLASSIFY_RMK_ICOS_FROM_RULE      			0
#define CLASSIFY_RMK_ICOS_FROM_SCOS      			OPL_REG_BIT0
#define CLASSIFY_RMK_ICOS_FROM_V4DSCP    		OPL_REG_BIT1
#define CLASSIFY_RMK_ICOS_FROM_V6DSCP    		((OPL_REG_BIT0) | (OPL_REG_BIT1))

#define CLASSIFY_RMK_SCOS_FROM_RULE      			0
#define CLASSIFY_RMK_SCOS_FROM_CCOS      			OPL_REG_BIT0
#define CLASSIFY_RMK_SCOS_FROM_V4TOS     		OPL_REG_BIT1
#define CLASSIFY_RMK_SCOS_FROM_V6TOS     		((OPL_REG_BIT0) | (OPL_REG_BIT1))

#define CLASSIFY_RMK_CCOS_FROM_RULE      			0
#define CLASSIFY_RMK_CCOS_FROM_SCOS      			OPL_REG_BIT0
#define CLASSIFY_RMK_CCOS_FROM_V4TOS     		OPL_REG_BIT1
#define CLASSIFY_RMK_CCOS_FROM_V6TOS     		((OPL_REG_BIT0) | (OPL_REG_BIT1))

enum
{
	CLASSIFY_FOFFSET_1 = 1,
	CLASSIFY_FOFFSET_2,
	CLASSIFY_FOFFSET_3,
	CLASSIFY_FOFFSET_4,
	CLASSIFY_FOFFSET_5,
	CLASSIFY_FOFFSET_6
};


enum
{
	CLASSIFY_FOFFSET_VALUE_0,
	CLASSIFY_FOFFSET_VALUE_1,
	CLASSIFY_FOFFSET_VALUE_2,
	CLASSIFY_FOFFSET_VALUE_3,
	CLASSIFY_FOFFSET_VALUE_4,
	CLASSIFY_FOFFSET_VALUE_5,
	CLASSIFY_FOFFSET_VALUE_6,
	CLASSIFY_FOFFSET_VALUE_7,
	CLASSIFY_FOFFSET_VALUE_8,
	CLASSIFY_FOFFSET_VALUE_9,
	CLASSIFY_FOFFSET_VALUE_10,
	CLASSIFY_FOFFSET_VALUE_11,
};

enum
{
	OFFSET_FIND_MATCH_STARTBIT = 1,
	OFFSET_FIND_CLOSEST_STARTBIT,
	OFFSET_FIND_NONE,
};

enum
{
	OFFSET_WINDOWS_SEARCH_LOOP_1 = 1,
	OFFSET_WINDOWS_SEARCH_LOOP_2
};

enum                      /* Dest field */
{
	SET_DEI,
	CHANGE_IDEI,
	CHANGE_V4TOS,
	CHANGE_V6TOS,
	CHANGE_ICOS,
	CHANGE_SCOS,
	CHANGE_CCOS,
	CHANGE_CVID,
	CHANGE_SVID,
};

enum					 /* Src field */
{
	RULE_IDEI = 1,
	V4TOS,
	V6TOS,
	RULE_ICOS,
	SCOS,
	CCOS,
	RULE_SCOS,
	RULE_CCOS,
	RULE_TOS,
	V4DSCP,
	V6DSCP,	
};

#pragma pack(1)		/* pragma pack definition */

typedef struct CLASSIFY_ADDR_TBL_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved:12;
	UINT32 bfAddr3:5;
	UINT32 bfAddr2:5;
   	UINT32 bfAddr1:5;
   	UINT32 bfAddr0:5;
#else
   	UINT32 bfAddr0:5;
	UINT32 bfAddr1:5;
	UINT32 bfAddr2:5;
	UINT32 bfAddr3:5;
	UINT32 bfReserved:12;
#endif
}CLASSIFY_ADDR_TBL_t;

typedef struct CLASSIFY_MASK_TBL_s										    
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved:5;
	UINT32 bfValid:1;										/* Bits 218*/
	UINT32 bfImask:1;									/* Bits 217*/
	UINT32 bfEmask:1;									/* Bits 216*/
    UINT32 bfOffset6:4;                                     /* Bits 212-215 */
    UINT32 bfOffset5:4;                                     /* Bits 208-211 */
    UINT32 bfOffset4:4;                                     /* Bits 204-207 */
    UINT32 bfOffset3:4;                                     /* Bits 200-203 */
    UINT32 bfOffset2:4;                                     /* Bits 196-199 */
    UINT32 bfOffset1:4;                                     /* Bits 192-195 */                                /* Bits 256-259 */

    UINT8  Mask[CLASSIFY_KEYVALUE_LENGTH];                  /* Bits 0-191   */
#else
   UINT32 bfOffset1:4;
   UINT32 bfOffset2:4;
   UINT32 bfOffset3:4;
   UINT32 bfOffset4:4;
   UINT32 bfOffset5:4;
   UINT32 bfOffset6:4;
   UINT32 bfEmask:1;
   UINT32 bfImask:1;
   UINT32 bfValid:1;
   UINT32 bfReserved:5;

   UINT8  Mask[CLASSIFY_KEYVALUE_LENGTH];
#endif
} CLASSIFY_MASK_TBL_t;

typedef struct CLASSIFY_RULE_TBL_s
{
#ifdef OPCONN_BIG_ENDIAN
   	UINT32 bfReserved:31;
	UINT32 bfTranActH:1;				/* Bits 224*/
	
	UINT32 bfTranActL:3;				/* Bits 221-223*/
	UINT32 bfEgress:1;					/* Bits 220*/
	UINT32 bfIngress:1;					/* Bits 219*/
	UINT32 bfRmkAct:4;					/* Bits 215-218*/				
	UINT32 bfRmkCos:3;				/* Bits 212-214*/
	UINT32 bfRmkIcos:3;				/* Bits 209-211*/
	UINT32 bfRmkVid:12;				/* Bits 197-208*/
	UINT32 bfMsel:5;						/* Bits 192-196*/
	
   	UINT8  Rule[CLASSIFY_KEYVALUE_LENGTH];                  /* Bits 0-191   */
#else
	UINT32 bfTranActH:1;
	UINT32 bfReserved:31;

	UINT32 bfMsel:5;
	UINT32 bfRmkVid:12;
	UINT32 bfRmkIcos:3;
	UINT32 bfRmkCos:3;
	UINT32 bfRmkAct:4;
	UINT32 bfIngress:1;
	UINT32 bfEgress:1;
	UINT32 bfTranActL:3;

   	UINT8  Rule[CLASSIFY_KEYVALUE_LENGTH];                 
#endif
}CLASSIFY_RULE_TBL_t;

typedef struct CLASSIFY_ADDR_RULEHIT_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved:8;
	UINT32 bfRuleHit:24;

#else
	UINT32 bfRuleHit:24;
	UINT32 bfReserved:8;
#endif
}CLASSIFY_ADDR_RULEHIT_t;
typedef struct CLASSIFY_METER_TBL_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfResv:12;
	UINT32 bfCir: 10;
	UINT32 bfCbs: 10;
#else
	UINT32 bfCbs: 10;
    	UINT32 bfCir: 10;
	UINT32 bfResv:12;
#endif
}CLASSIFY_METER_TBL_t;

typedef struct CLASSIFY_ADDR_ENTRY_s
{
    UINT16 ClasAddr[CLASSIFY_RULE_TABLE_GROUP];
}CLASSIFY_ADDR_ENTRY_t;

typedef struct CLASSIFY_MASK_ENTRY_s										    
{
	UINT32 bfReserved:5;
	UINT32 bfValid:1;										/* Bits 218*/
	UINT32 bfImask:1;									/* Bits 217*/	
	UINT32 bfEmask:1;									/* Bits 216*/
   	UINT32 bfOffset6:4;                                     /* Bits 212-215 */
   	UINT32 bfOffset5:4;                                     /* Bits 208-211 */
   	UINT32 bfOffset4:4;                                     /* Bits 204-207 */
   	UINT32 bfOffset3:4;                                     /* Bits 200-203 */
   	UINT32 bfOffset2:4;                                     /* Bits 196-199 */
   	UINT32 bfOffset1:4;                                     /* Bits 192-195 */

   	UINT8  Mask[CLASSIFY_KEYVALUE_LENGTH];                  /* Bits 0-191   */

	UINT8  maskPri;
   	UINT16 referenceCount;
} CLASSIFY_MASK_ENTRY_t;

typedef struct CLASSIFY_RULE_ENTRY_s
{
   	UINT32 bfReserved:31;												
	UINT32 bfTranActH:1;

	UINT32 bfTranActL:3;				/* Bits 221-223*/
	UINT32 bfEgress:1;					/* Bits 220*/
	UINT32 bfIngress:1;					/* Bits 219*/
	UINT32 bfRmkAct:4;					/* Bits 215-218*/				
	UINT32 bfRmkCos:3;				/* Bits 212-214*/
	UINT32 bfRmkIcos:3;				/* Bits 209-211*/
	UINT32 bfRmkVid:12;				/* Bits 197-208*/
	UINT32 bfMsel:5;						/* Bits 192-196*/
	
   	UINT8  Rule[CLASSIFY_KEYVALUE_LENGTH];                  /* Bits 0-191   */
}CLASSIFY_RULE_ENTRY_t;

typedef struct CLASSIFY_METER_s
{
	UINT8 used;
	UINT8 cbs;
	UINT8 cir;
}CLASSIFY_METER_t;

typedef struct CLASSIFY_RULE_MALLOC_s
{
    UINT8 mallocFlag;
    UINT8 modMselFlag;
}CLASSIFY_RULE_MALLOC_t; 

typedef struct CLASSIFY_OFFSET_SET_PAR_s
{
	UINT8		offsetSeq;			/* Offset windows from 1 to 8*/
	UINT8		searchLoop;			/* Searching offset windows from 1 to 8 is a loop */
	UINT16  	startBit;			/* */
	UINT16  	fieldLength;		    /* */
}CLASSIFY_OFFSET_SET_PAR_t;

typedef struct CLASSIFY_FIELD_ADDRLEN_s
{
    UINT16 	fieldOffset;
    UINT16 	fieldLength;
}CLASSIFY_FIELD_ADDRLEN_t;

typedef struct CLASSIFY_MASK_OFFSET_FIELD_s
{
    UINT8	offsetValue[6];   
}CLASSIFY_MASK_OFFSET_FIELD_t;

typedef union CLASSIFY_FIELD_MAP_s
{
    struct
    {
#ifdef OPCONN_BIG_ENDIAN
        UINT32 bfEnaDestMAC:1;
        UINT32 bfEnaSrcMAC:1;
        UINT32 bfEnaFirstTPID:1;
        UINT32 bfEnaFirstCOS:1;
        UINT32 bfEnaFirstTag:1;    
#ifdef ONLY_SUPPORT_SINGLE_TAG
#else
        UINT32 bfEnaSecondTPID:1;
        UINT32 bfEnaSecondCOS:1;
        UINT32 bfEnaSecondTAG:1;
#endif				
        UINT32 bfEnaLenorType:1;
        UINT32 bfEnaIpVer:1;
        UINT32 bfEnaIpTos:1;
        UINT32 bfEnaIpProType:1;
        UINT32 bfEnaSrcIP:1;
        UINT32 bfEnaDestIP:1;
        UINT32 bfEnaSrcPort:1;
        UINT32 bfEnaDestPort:1;
        UINT32 bfEnaUserDefine1:1;
#ifdef ONLY_SUPPORT_SINGLE_TAG
		 UINT32 :18;
#else
        UINT32 :15;
#endif
#else
#ifdef ONLY_SUPPORT_SINGLE_TAG
		 UINT32 :18;
#else
        UINT32 :15;
#endif
        UINT32 bfEnaUserDefine1:1;
        UINT32 bfEnaDestPort:1;
        UINT32 bfEnaSrcPort:1;
        UINT32 bfEnaDestIP:1;
        UINT32 bfEnaSrcIP:1;
        UINT32 bfEnaIpProType:1;
        UINT32 bfEnaIpTos:1;
        UINT32 bfEnaIpVer:1;
        UINT32 bfEnaLenorType:1;
#ifdef ONLY_SUPPORT_SINGLE_TAG	
#else
        UINT32 bfEnaSecondTAG:1;
        UINT32 bfEnaSecondCOS:1;
        UINT32 bfEnaSecondTPID:1;
#endif				
        UINT32 bfEnaFirstTag:1;    
        UINT32 bfEnaFirstCOS:1;
        UINT32 bfEnaFirstTPID:1;
        UINT32 bfEnaSrcMAC:1;
        UINT32 bfEnaDestMAC:1;
        
#endif
    }EIIField;


    struct
    {
#ifdef OPCONN_BIG_ENDIAN
        UINT32 bfEnaDestMAC:1;
        UINT32 bfEnaSrcMAC:1;
        UINT32 bfEnaFirstTPID:1;
        UINT32 bfEnaFirstCOS:1;
        UINT32 bfEnaFirstTag:1;  
#ifdef ONLY_SUPPORT_SINGLE_TAG		
#else
        UINT32 bfEnaSecondTPID:1;
        UINT32 bfEnaSecondCOS:1;
        UINT32 bfEnaSecondTAG:1;
#endif				
        UINT32 bfEnaLenorType:1;
        UINT32 bfEnaPPPCode:1;       
        UINT32 bfEnaPPPproType:1;
        UINT32 bfEnaLcpIpcp:1;
        UINT32 bfEnaIpVer:1;
        UINT32 bfEnaIpTos:1;
        UINT32 bfEnaIpProType:1;
        UINT32 bfEnaSrcIP:1;
        UINT32 bfEnaDestIP:1;
        UINT32 bfEnaSrcPort:1;
        UINT32 bfEnaDestPort:1;
        UINT32 bfEnaUserDefine1:1;
#ifdef ONLY_SUPPORT_SINGLE_TAG	
		 UINT32 :15;	
#else
        UINT32 :12;
#endif
#else
#ifdef ONLY_SUPPORT_SINGLE_TAG
		  UINT32 :15;
#else
        UINT32 :12;
#endif
        UINT32 bfEnaUserDefine1:1;
        UINT32 bfEnaDestPort:1;
        UINT32 bfEnaSrcPort:1;
        UINT32 bfEnaDestIP:1;
        UINT32 bfEnaSrcIP:1;
        UINT32 bfEnaIpProType:1;
        UINT32 bfEnaIpTos:1;
        UINT32 bfEnaIpVer:1;
        UINT32 bfEnaLcpIpcp:1;
        UINT32 bfEnaPPPproType:1;
        UINT32 bfEnaPPPCode:1;
        UINT32 bfEnaLenorType:1;
#ifdef ONLY_SUPPORT_SINGLE_TAG	
#else
        UINT32 bfEnaSecondTAG:1;
        UINT32 bfEnaSecondCOS:1;
        UINT32 bfEnaSecondTPID:1;
#endif				
        UINT32 bfEnaFirstTag:1;    
        UINT32 bfEnaFirstCOS:1;
        UINT32 bfEnaFirstTPID:1;
        UINT32 bfEnaSrcMAC:1;
        UINT32 bfEnaDestMAC:1;
        
#endif
    }PppoEField;
    

    UINT32 bitMap;
}CLASSIFY_FIELD_MAP_t;

typedef struct CLASSIFY_EII_s
{
#ifdef OPCONN_BIG_ENDIAN
    UINT8  DestMac[6];                         /*6Bytes*/
    UINT8  SrcMac[6];                          /*6Bytes*/
	
    UINT32 bfFirstTpid:             16;        /*2Bytes*/
    UINT32 bfFirstCos:		        3;         /*3Bits*/
    UINT32 bfReser_FirstCFI:        1;
    UINT32 bfFirstVlan:		        12;        /*12Bits*/
#ifdef ONLY_SUPPORT_SINGLE_TAG	
#else
    UINT32 bfSecondTpid:            16;        /*2Byts*/
    UINT32 bfSecondCos:		        3;         /*3Bits  surpport Q-in-Q mode*/
    UINT32 bfReser_SecondCFI:       1;
    UINT32 bfSecondVlan:	        12;        /*12Bits surpport Q-in-Q mode*/
#endif    
    UINT32 bfLenOrType:             16;        /*2Bytes  This is the type or
                                               *lenth field in the L2 head*/
    UINT32 bfIpVersion:	            4;         /*4Bits*/
    UINT32 bfReserved:	            4;
    UINT32 bfIpTos:                 8;         /*1Bytes*/

	UINT16 reser_IpPktLen;
    UINT16 reser_IpIdentify;
    
    UINT16 bfReser_IpSliceFlag:     3;
    UINT16 bfReser_IpSliceOffset:   13;
    
    UINT8  reser_IpTtl;
    UINT8  ipProType;                          /*1Bytes This is protocol type 
                                               *field in the IP head*/
    UINT16 reser_IpCheckSum;
    UINT32 srcIP;                              /*4Bytes*/              
    UINT32 destIP;                             /*4Bytes*/
    UINT16 srcPort;                            /*2Bytes L4 port*/
    UINT16 destPort;                           /*2Bytes L4 port*/
    UINT32 userDefine;
#else

    UINT8  DestMac[6];                         /*6Bytes*/
    UINT8  SrcMac[6];                          /*6Bytes*/

    UINT32 bfFirstVlan:		        12;        /*12Bits*/
    UINT32 bfReser_FirstCFI:        1;
    UINT32 bfFirstCos:		        3;         /*3Bits*/
	UINT32 bfFirstTpid:             16;        /*2Bytes*/
#ifdef ONLY_SUPPORT_SINGLE_TAG
#else
    UINT32 bfSecondVlan:	        12;        /*12Bits surpport Q-in-Q mode*/
    UINT32 bfReser_SecondCFI:       1;
    UINT32 bfSecondCos:		        3;         /*3Bits  surpport Q-in-Q mode*/
    UINT32 bfSecondTpid:            16;        /*2Byts*/
#endif	

    UINT32 bfIpTos:                 8;         /*1Bytes*/ 
    UINT32 bfReserved:	            4;
    UINT32 bfIpVersion:	            4;         /*4Bits*/
	UINT32 bfLenOrType:             16;        /*2Bytes  This is the type or 
                                               *lenth field in the L2 head*/
        
    UINT16 reser_IpPktLen;
    UINT16 reser_IpIdentify;
    
    UINT16 bfReser_IpSliceOffset:   13;
    UINT16 bfReser_IpSliceFlag:     3;   
    
    UINT8  reser_IpTtl;
    UINT8  ipProType;                          /*1Bytes This is protocol type
    											*field in the IP head*/
    UINT16 reser_IpCheckSum;
    UINT32 srcIP;                              /*4Bytes*/              
    UINT32 destIP;                             /*4Bytes*/
    UINT16 srcPort;                            /*2Bytes L4 port*/
    UINT16 destPort;                           /*2Bytes L4 port*/
    UINT32 userDefine;
#endif
}CLASSIFY_EII_t;

typedef struct CLASSIFY_PPPPOE_s
{
#ifdef OPCONN_BIG_ENDIAN
    UINT8  DestMac[6];                         /*6Bytes*/
    UINT8  SrcMac[6];                          /*6Bytes*/

	UINT32 bfFirstTpid:             16;        /*2Bytes*/
    UINT32 bfFirstCos:		        3;         /*3Bits*/
    UINT32 bfReser_FirstCFI:        1;
    UINT32 bfFirstVlan:		        12;        /*12Bits*/
#ifdef ONLY_SUPPORT_SINGLE_TAG
#else
    UINT32 bfSecondCos:		        3;         /*3Bits  surpport Q-in-Q mode*/
    UINT32 bfReser_SecondCFI:       1;
    UINT32 bfSecondVlan:	        12;        /*12Bits surpport Q-in-Q mode*/ 
    UINT32 bfSecondTpid:            16;        /*2Byts*/
#endif	
    UINT16 lenOrType;                          /*2Bytes  This is the type or 
                                               *lenth field in the L2 head*/

    UINT8  reserved;                           /*version,type.1Bytes*/
    UINT8  pppCode;				               /*1Byte*/
	UINT16 sessionId;			
	UINT16 length;              

	UINT32 pppProType:				16;
	UINT32 bfLcpIpcpCodeH:			4;
	UINT32 bfIpVer_lcpIpcpCodeL:	4;
	UINT32 ipTos:					8;

	UINT16 reser_PktLen;
	UINT16 reser_IpIdenty;

	UINT16 bfReser_IpSliceFlag:	3;
	UINT16 bfReser_IpSliceOffset:13;

	UINT8  reser_IpTtl;
	UINT8  ipProType;
	UINT16 reser_IpCheckSum;

    UINT32 srcIP;                              /*4Bytes*/              
    UINT32 destIP;                             /*4Bytes*/
    UINT16 srcPort;                            /*2Bytes L4 port*/
    UINT16 destPort;                           /*2Bytes L4 port*/   

    UINT32 userDefine;
#else
    UINT8  DestMac[6];                         /*6Bytes*/
    UINT8  SrcMac[6];                          /*6Bytes*/

    UINT32 bfFirstVlan:		        12;        /*12Bits*/
    UINT32 bfReser_FirstCFI:        1;
    UINT32 bfFirstCos:		        3;         /*3Bits*/
    UINT32 bfFirstTpid:             16;        /*2Bytes*/
#ifdef ONLY_SUPPORT_SINGLE_TAG	
#else
    UINT32 bfSecondVlan:		    12;        /*12Bits surpport Q-in-Q mode*/
	UINT32 bfReser_SecCFI:		    1;
    UINT32 bfSecondCos:			    3;         /*3Bits  surpport Q-in-Q mode*/    
    UINT32 bfSecondTpid:            16;        /*2Byts*/
#endif
	UINT16 lenOrType;                          /*2Bytes  This is the type or
                                               *lenth field in the L2 head*/

    UINT8  reserved;                           /*version,type.1Bytes*/
    UINT8  pppCode;					           /*1Byte*/
	UINT16 sessionId;
	UINT16 length;
	
	UINT32 ipTos:					8;
	UINT32 bfIpVer_lcpIpcpCodeL:	4;
	UINT32 bfLcpIpcpCodeH:			4;
	UINT32 pppProType:				16;

	UINT16 reser_PktLen;
	UINT16 reser_IpIdenty;

	UINT16 bfReser_IpSliceOffset:13;
	UINT16 bfReser_IpSliceFlag:	3;

	UINT8  reser_IpTtl;
	UINT8  ipProType;
	UINT16 reser_IpCheckSum;

    UINT32 srcIP;                              /*4Bytes*/              
    UINT32 destIP;                             /*4Bytes*/
    UINT16 srcPort;                            /*2Bytes L4 port*/
    UINT16 destPort;                           /*2Bytes L4 port*/   

    UINT32 userDefine;
#endif
    
}CLASSIFY_PPPOE_t;

typedef union
 {
	 CLASSIFY_EII_t 	EIIValue;	/*Values of EII packet fields for rule value usage*/
	 CLASSIFY_PPPOE_t	PPPoeValue; /*Values of PPPoE packet fields for rule value usage*/
 }ClsDataValue_t;

typedef struct CLASSIFY_PARAMETER_s
{
    	UINT8  pktType;
    	UINT8  egressMask;                /* Mask for egress port */
    	UINT8  ingressMask;	              /* Mask for ingress port */
    	UINT8  maskPri;
    	UINT16 cbs;                       /* CBS of meter configuration */
    	UINT16 cir;                        /* CIR of meter configuration */

    	CLASSIFY_FIELD_MAP_t   FieldMap;  /*Indicates each field is classified or not */
	CLASSIFY_RULE_ENTRY_t  RuleEntry; /*Input parameters in rule configuration*/
	ClsDataValue_t PktRuleValue;
    ClsDataValue_t PktUsrMask;

}CLASSIFY_PARAMETER_t;

typedef struct CLASSIFY_MASK_PARAMETER_s
{
    UINT8  pktType;

    UINT16 egressMask;
    UINT16 ingressMask;

    UINT16  maskID;

    CLASSIFY_FIELD_MAP_t   FieldMap;

    union
    {
        CLASSIFY_EII_t   EIIValue;
        CLASSIFY_PPPOE_t PPPoeValue;
    }RuleValue;

	union
	{
        CLASSIFY_EII_t   EIIUsrMask;
        CLASSIFY_PPPOE_t PPPoeUsrMask;
	}UsrMask;

}CLASSIFY_MASK_PARAMETER_t;

typedef struct CLASSIFY_RULE_PARAMETER_s
{
    	UINT8  pktType;
	UINT16 cbs;
	UINT16 cir;
    	UINT16 maskID;
	
	CLASSIFY_RULE_ENTRY_t  RuleEntry;
	
    union
    {
        CLASSIFY_EII_t   EIIValue;
        CLASSIFY_PPPOE_t PPPoeValue;
    }RuleValue;
	
}CLASSIFY_RULE_PARAMETER_t;

typedef struct CLASSIFY_MASKINFO_FOR_RULE_s
{
    CLASSIFY_MASK_OFFSET_FIELD_t MaskOffset;
    UINT16                       ingressMask;
    UINT16                       egressMask;
    UINT32                       fieldMap;
    UINT8                        Mask[CLASSIFY_KEYVALUE_LENGTH];
	
	union
	{
        CLASSIFY_EII_t   EIIUsrMask;
        CLASSIFY_PPPOE_t PPPoeUsrMask;
	}UsrMask;

}CLASSIFY_MASKINFO_FOR_RULE_t;

typedef struct opl_cls_hwtbl_s{
	UINT16 clsHwRuleID;   
	UINT16 clsHwMaskID; 
	UINT16 clsHwAddr;
    UINT16 clsHwOff;
}opl_cls_hwtbl_t;

typedef struct CLASSIFY_ITEM_s
{
	CLASSIFY_PARAMETER_t Parameter;
    	UINT16 classifyRuleID;   
	opl_cls_hwtbl_t shadowtbl;
	UINT8  valid;
}CLASSIFY_ITEM_t;

typedef struct CLS_CONFIG_INFO_s
{
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	UINT8	name[64];	/* classify rule name */
	UINT8	ports[10];		/* port list */
#endif
	UINT8  valid  :1;
	UINT8  t_act  :4;/*transfer action*/
	UINT8  pad    :3;
    UINT8  rulePri;
	UINT8  queueMapId;
	UINT8  cosMapVal;
	UINT8  ingressmask;
	UINT8  bingress;
	UINT16 rmkvidOp:2;
/*add by ltang,for bug 3222 ---start---*/	
#if 1
	UINT16 rmkcoschange:1;
	UINT16 rmkicoschange:1;
#endif
/*add by ltang,for bug 3222 ---end---*/	
	UINT16 rmkvid:12;

    UINT32 dstMacFlag:1;
    UINT32 srcMacFlag:1;
    UINT32 etherPriFlag:1;
    UINT32 vlanIdFlag:1;
    UINT32 lenOrTypeFlag:1;
    UINT32 dstIpFlag:1;
    UINT32 srcIpFlag:1;
    UINT32 ipTypeFlag:1;
    UINT32 ipV4DscpFlag:1;
    UINT32 ipV6PreFlag:1;
    UINT32 srcL4PortFlag:1;
    UINT32 dstL4PortFlag:1;
    UINT32 rsvFlag:21;

	struct {
		UINT8 lowRange[6];
		UINT8 highRange[6];
	}dstMac;

	struct {
		UINT8 lowRange[6];
		UINT8 highRange[6];
	}srcMac;

	struct {
		UINT8 lowRange;
		UINT8 highRange;
	}etherPri;

	struct {
		UINT16 lowRange;
		UINT16 highRange;
	}vlanId;

	struct {
		UINT16 lowRange;
		UINT16 highRange;
	}lenOrType;
	
	struct {
		UINT32 lowRange;
		UINT32 highRange;
	}dstIp;

	struct {
		UINT32 lowRange;
		UINT32 highRange;
	}srcIp;

	struct {
		UINT8 lowRange;
		UINT8 highRange;
	}ipType;

	struct {
		UINT8 lowRange;
		UINT8 highRange;
	}ipV4Dscp;

	struct {
		UINT8 lowRange;
		UINT8 highRange;
	}ipV6Pre;

	struct {
		UINT16 lowRange;
		UINT16 highRange;
	}srcL4Port;

	struct {
		UINT16 lowRange;
		UINT16 highRange;
	}dstL4Port;
	
    UINT8 dstMacOp;
    UINT8 srcMacOp;
    UINT8 etherPriOp;
    UINT8 vlanIdOp;
    UINT8 lenOrTypeOp;
    UINT8 dstIpOp;
    UINT8 srcIpOp;
    UINT8 ipTypeOp;
    UINT8 ipV4DscpOp;
    UINT8 ipV6PreOp;
    UINT8 srcL4PortOp;
    UINT8 dstL4PortOp;
}CLS_CONFIG_INFO_t;

#pragma pack ()

OPL_STATUS clsWindowRangeCheck(UINT8 *pMask);

/*******************************************************************************
*
* clsMaskAdd - Add a specific classification mask entry.
*
* DESCRIPTION:
*       this function used to generate classify mask entry and flow mask entry 
*       accordaning the input parameter, and then malloc the rule and mask entry
*       calculate the address and then write mask and rule into the mask and rule
*       table
*
* INPUT:
*        pstMaskPar - classification and flow parameter.
*
* OUTPUT:
*        MaskID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO: 
*/

OPL_STATUS clsMaskAdd(CLASSIFY_MASK_PARAMETER_t *pMaskPar, UINT16 maskID);

/*******************************************************************************
*
* clsMaskDel - Delete classification mask entry directly.
*
* DESCRIPTION:
*       This function used to delete a specific classification mask entry. Disable
*		the valid bit and clear the data.
*
* INPUT:
*        MaskID       - Classification Mask ID.
*
* OUTPUT:
*        None.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS clsMaskDel(UINT8 maskID);

/*******************************************************************************
*
* clsRuleAdd - configure rule and mask into classify rule and classify mask
*                    table
*
* DESCRIPTION:
*       this function used to generate classify mask entry and flow mask entry 
*       accordanve the input parameter, and then malloc the rule and mask entry
*       calculate the address and then write mask and rule into the mask and rule
*       table
*
* INPUT:
*        pstRulePar - classification and flow parameter.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO: 
*/

OPL_STATUS clsRuleAdd(CLASSIFY_RULE_PARAMETER_t *pRulePar, UINT16 maskID, UINT16 FlowMaskID, UINT16 *pRuleID);

/*******************************************************************************
*
* clsRuleDel - Delete classification rule entry directly.
*
* DESCRIPTION:
*       This function used to delete a specific classification rule entry directly.
*       Calculate the index in hash table and clear it.  
*
* INPUT:
*        MaskID       - Classification Mask ID.
*
* OUTPUT:
*        None.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS clsRuleDel(UINT16 ruleID);

/*******************************************************************************
*
* clsAddrCal		-	Calculate the classify rule entry index.
*
* DESCRIPTION:
*		 Calculate the classify rule entry index.
*
* INPUT:
*		 pRuleEntry	-	Pointer to entry of classify rule table.
*
* OUTPUT:
*		 pAddr		-	Pointer to entry of classify address table.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        OPL_ERR_NULL_POINTER			-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS clsAddrCal(CLASSIFY_RULE_ENTRY_t *pRuleEntry, UINT16 *pAddr);

/*******************************************************************************
*
* clsMaskMalloc	-	Malloc memory for specified classify mask entry.
*
* DESCRIPTION:
*		This function malloc memory for specified classify mask entry. Mainly
*		divided into two situation: specified classify mask entry already
*		exist, or is a new entry. NO matter which situation, the mask table
*		need be adjusted.
*
* INPUT:
*		 pstMaskEntry	-	Pointer to mask entry.
*
* OUTPUT:
*		 pucMaskID		-	Pointer to index of flow mask entry.
*		 pucMaskWr		-	Pointer to parameter indicating write or not write.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        OPL_ERR_NULL_POINTER			-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS clsMaskMalloc(CLASSIFY_MASK_ENTRY_t *pMaskEntry, 
                                      UINT8 *pMaskID);

/*******************************************************************************
*
* clsMaskFree - Free specified entry of classify mask table.
*
* DESCRIPTION:
*		This function free specified entry of classify mask table. If only one
*		classify rule refer to the entry, adjust the entries remain after 
*		free action. If several rules refer to it, just decrease the reference
*		number.
*
* INPUT:
*		 maskID		-	Index of entry of classify mask table.
*
* OUTPUT:
*		 NULL.
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*		 CLASSIFY_FREE_INVALID_MENTRY	-	Mask entry to be freed is invalid.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsMaskFree(UINT8 maskID);


/*******************************************************************************
*
* clsRuleFree - Free the space for specified classify rule ID.
*
* DESCRIPTION:
*		This function free the space for specified classify rule ID. Two factors,
*		free space counter for specified group, and malloc flag.
*
* INPUT:
*		 ruleID		-	Classify rule ID.
*
* OUTPUT:
*		 NULL.
*
* RETURNS:
*        NULL.
*
* SEE ALSO: 
*/
void clsRuleFree(UINT16 ruleID);


/*******************************************************************************
*
* clsRuleMalloc - Malloc the rule ID and address offset for certain 
*								address entry.
*
* DESCRIPTION:
*		This function malloc the rule ID and address offset for certain address
*		entry. Firstly, check if there is any space in the hash address. Then,
*		check which group has the most free ID. Finally, calculate the rule
*		ID and address offset.
*
* INPUT:
*		 address		-	Index of classify address table.
*
* OUTPUT:
*		 pRuleID		-	Pointer to index of entry of classify rule table.
*		 pAddrOff		-	Pointer to offset of certain entry of classify address table.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        OPL_ERR_NULL_POINTER			-	If the input pointer is NULL.
*		 CLASSIFY_ADDR_TABLE_8COLLISION	-	The entry of classify address table is full.
*		 CLASSIFY_RULE_TABLE_FULL		-	All the classify rule table is full.
*
* SEE ALSO: 
*/
OPL_STATUS clsRuleMalloc(UINT16 address, UINT16 *pRuleID ,UINT16 *pAddrOff);

/*******************************************************************************
*
* clsEIIMaskRuleMake - Make classify mask and rule table entry.
*
* DESCRIPTION:
*      	This function mainly divided into two parts. Firstly, dicide the window
*       offset through the field list of inputting parameter. Secondly, set
*       the mask and rule table entry by the data of the field. But you should
*       note that if the field of the parameter is not byte alignment, you 
*       should do some shift operation first before setting.
*
* INPUT:
*        pRulePar -       	Pointer to parameter inputing.
*        
* OUTPUT:
*        pMaskEntry -       Pointer to classify mask table entry.
*        pRuleEntry -       Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK                         -       Operation is successful.
*        OPL_ERR_NULL_POINTER   		-       Input pointer is null.
*		 Others                 		- 		An appropriate specific error code defined
*												for CLT device driver.
*        CLASSIFY_NULL_FIELDMAP    		-       FieldMap is null.
*        CLASSIFY_INVALID_FIELD_TYPE    -       Field type is wrong.
*        CLASSIFY_WINDOWS_OUTOF_RANGE   -       Input index of parameter exceeds
*                                               maximal defined index.
*        CLASSIFY_INVALID_FOFFSET_NUM   -       Invalid offset ID.
*
* SEE ALSO: 
*/
OPL_STATUS clsEIIMaskRuleMake(CLASSIFY_PARAMETER_t *pMaskRulePar,
              CLASSIFY_MASK_ENTRY_t *pMaskEntry, CLASSIFY_RULE_ENTRY_t *pRuleEntry);

/*******************************************************************************
*
* clsPPPoeMaskRuleMake -       Make classify mask and rule table entry.
*
* DESCRIPTION:
*                This function mainly divided into two parts. Firstly, dicide the window
*                       offset through the field list of inputting parameter. Secondly, set
*                       the mask and rule table entry by the data of the field. But you should
*                       note that if the field of the parameter is not byte alignment, you 
*                       should do some shift operation first before setting.
*
* INPUT:
*        pRulePar               -       Pointer to parameter inputing.
*        
* OUTPUT:
*        pstMaskEntry           -       Pointer to classify mask table entry.
*        pstRuleEntry           -       Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK                                                 -       Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL   -       Input pointer is null.
*                CLASSIFY_NULL_FIELDMAP                 -       FieldMap is null.
*                CLASSIFY_INVALID_FIELD_TYPE    -       Field type is wrong.
*                CLASSIFY_WINDOWS_OUTOF_RANGE   -       Input index of parameter exceeds
*                                                                                               maximal defined index.
*                CLASSIFY_INVALID_FOFFSET_NUM   -       Invalid offset ID.
*
* SEE ALSO: 
*/
OPL_STATUS clsPPPoeMaskRuleMake(CLASSIFY_PARAMETER_t *pMaskRulePar,
              CLASSIFY_MASK_ENTRY_t *pMaskEntry, CLASSIFY_RULE_ENTRY_t *pRuleEntry);

/*******************************************************************************
*
* clsMaskRuleSet - Set classify mask and rule entry.
*
* DESCRIPTION:
*        Set classify mask and rule entry. There are mainly three steps. Firstly,
*        get the start bit and total bits of specified field. Secondly, change
*        the start bit and total bits of field into start byte and total bytes
*        of window. For field has 128 bytes totally, and window has 64 bytes 
*        instead. Thirdly, fill in mask and rule entry with 0xff and data
*        by several instances: no tail bits neither header bits, has tail bits
*        but no header bits, no tail bits but has header bits, both tail bits
*        and header bits.
*
* INPUT:
*        pData                - Value to set specified classify entry.
*        fieldMapCount        - Index of the field correspond to the value.
*        pOffset              - Pointer to window offset array.
*        
* OUTPUT:
*        pClassMask         - Pointer to classify mask table.
*        pClassRule         - Pointer to classify rule table.
*
* RETURNS:
*        OPL_OK                	- Operation is successful.
*        OPL_ERR_NULL_POINTER   - Input pointer is null.
*
* SEE ALSO: 
*/
OPL_STATUS clsMaskRuleSet(UINT8 offsetSeq, UINT8 offsetValue, 
                                UINT16 startBit, UINT16 fieldLen, UINT8* pRuleData,
							    UINT8* pUsrMask, UINT8* pMaskValue, UINT8* pRuleValue);

/*******************************************************************************
*
* clsOffsetSet - Set all 8 window offsets according to FieldMap.
*
* DESCRIPTION:
*       This function looks up the FieldMap. It the specific bit is valid, this 
*		function get the start bit and length of the field, the search the offset 
*		table.
* INPUT:
*        pRulePar             	-       Pointer to parameter inputing.
*        
* OUTPUT:
*        pOffset              	-       Pointer to window offset array.
*        pMaskValue          	-       Pointer to mask value.
*        pRuleValue          	-       Pointer to rule value.
*
* RETURNS:
*        OPL_OK                 -       Operation is successful.
*        OPL_ERR_NULL_POINTER   -       Input pointer is null.
*  
*
* SEE ALSO: 
*/
OPL_STATUS clsOffsetSet(CLASSIFY_PARAMETER_t* pMaskRulePar, UINT8* pMaskValue,
                             CLASSIFY_MASK_OFFSET_FIELD_t* pOffset, UINT8* pRuleValue);

/*******************************************************************************
*
* clsOffsetTblSearch - Caculate classify offset value.      
*
* DESCRIPTION:
*       This function search the offset table and find a location according to the input 
*		start bit and length. Then get the mask value and rule value via call sub func.
*                                             
* INPUT:
*			pOffsetSetPar	 - offset set parameter, include offset sequence, loop
*                                 time, start bit and field length
*           pOffset          - each offset value
*           pRulePar         - rule parameter
*        
* OUTPUT:
*           pMaskValue       - get current mask value after set offset
*           pRuleValue       - get current rule value after set offset
*
* RETURNS:
*           OPL_OK              	- Operation is successful.
*	        OPL_ERR_NULL_POINTER   	- Input pointer is null.
*
* SEE ALSO: 
*/
OPL_STATUS	clsOffsetTblSearch(CLASSIFY_OFFSET_SET_PAR_t* pOffsetSetPar, CLASSIFY_MASK_OFFSET_FIELD_t* pOffset,
								CLASSIFY_PARAMETER_t* pMaskRulePar, UINT8* pMaskValue, UINT8* pRuleValue);
 
/*******************************************************************************
*
* clsOffsetHeadTailCal	- Specify the sequence and value of a offset, then get its' head 
*                     and tail byte.
*
* DESCRIPTION:
*       This function calculates the start and tail bits of a specific offset.  
*		Especially calculates the offset which ocuppies the byte 79 and byte 0.	
*			
* INPUT:
*           offsetSeq 	- Offset sequence, from 1 - 8
*			offsetValue - Offset step value, from 1 - 12
*
* OUTPUT:
*			pOffsetHead - Byte number of offset head 
* 			pOffsetTail - Byte number of offset tail
*
* RETURNS:
*           OPL_OK              	- Operation is successful.
*	        OPL_ERR_NULL_POINTER   	- Input pointer is null.
*
* SEE ALSO: 
*/
OPL_STATUS clsOffsetHeadTailCal(UINT8 offsetSeq, UINT8 offsetValue, UINT16* pOffsetHead, UINT16* pOffsetTail);

 
/*******************************************************************************
*
* clsRuleIDSel		-	Select rule ID corresponding to specified rule entry.
*
* DESCRIPTION:
*		 This function select rule ID in classify address table corresponding to
*			specified rule entry inputting through parameter.
*
* INPUT:
*		 pstRuleEntry	-	Pointer to entry of classify rule table.
*		 usAddr			-	Index of classify address table.
*
* OUTPUT:
*		 pAddrOff		-	Pointer to offset of certain entry of classify address table.
*		 pRuleID		-	Pointer to index of entry of classify rule table.
*
* RETURNS:
*        OPL_OK						-	If the operation is successful.
*        OPL_ERR_NULL_POINTER		-	If the input pointer is NULL.
*		 CLASSIFY_RULE_NULL_ITEM	-	No such rule item is find in address table.
*
* SEE ALSO: 
*/
OPL_STATUS clsRuleIDSel(CLASSIFY_RULE_ENTRY_t *pRuleEntry ,UINT16 usAddr 
                                       ,UINT16 *pAddrOff ,UINT16 *pRuleID);
 
/*******************************************************************************
*
* clsMaskSel	-	Select the mask entry ID according to parameter.
*
* DESCRIPTION:
*		 Select the mask entry ID according to parameter. If there's not entry match
*			parameter, return message to indicate no entry selected.
*
* INPUT:
*		 pMaskEntry	-	Pointer to mask entry.
*
* OUTPUT:
*		 pMask		-	Pointer to mask ID.
*
* RETURNS:
*        OPL_OK						-	If the operation is successful.
*        OPL_ERR_NULL_POINTER		-	If the input pointer is NULL.
*		 CLASSIFY_MASKF_SELECT_NULL	-	If the specified mask entry is not exist.
*
* SEE ALSO: 
*/
OPL_STATUS clsMaskSel(CLASSIFY_MASK_ENTRY_t *pMaskEntry, UINT8 *pMask);


/*******************************************************************************
*
* clsMaskTblRd - read a mask entry from the mask table.
*
* DESCRIPTION:
*       read a mask entry from the mask table.
*
* INPUT:
*       maskID  - classification Mask entry ID.
*       tblType - CLASSIFY_LOCAL_TBL               1
*                   CLASSIFY_HW_TBL                  2
*
* OUTPUT:
*        pMask  - classify mask entry.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO: 
*/

OPL_STATUS clsMaskTblRd(CLASSIFY_MASK_ENTRY_t *pMask, UINT16 maskID, UINT8 tblType);

/*******************************************************************************
*
* clsMaskTblEntryAdd	-	Add the specific mask entry to the table.
*
* DESCRIPTION:
*		    This function used to calculate the address, and then allocate the rule 
*			and mask entry and then write mask and rule into the mask and rule table
*			the rule and mask entry is generated by the calling funtion.
*
* INPUT:
*		 pstMaskEntry	-	Pointer to classification Mask entry.
*		 usMaskID	    -	classification mask ID.
*
* OUTPUT:
*		 None.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS clsMaskTblEntryAdd(CLASSIFY_MASK_ENTRY_t *pMaskEntry, UINT16 maskID);

/*******************************************************************************
*
* clsRuleTblEntryAdd	-	Add the specific rule and mask entry to the table.
*
* DESCRIPTION:
*		 This function used to calculate the address, and then allocate the rule 
*			and mask entry and then write mask and rule into the mask and rule table
*			the rule and mask entry is generated by the calling funtion.
*
* INPUT:
*		 RuleEntry	        -	Pointer to classification rule entry.
*
* OUTPUT:
*		 ClassifyRuleID		-	Pointer to classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS clsRuleTblEntryAdd(CLASSIFY_RULE_ENTRY_t *pRuleEntry, UINT16 *pClassifyRuleID);

/*******************************************************************************
* clsEIIMaskMake	-	Make classify mask and entry.
*
* DESCRIPTION:
*		 This function mainly divided into two parts. Firstly, dicide the window
*			offset through the field list of inputting parameter. Secondly, set
*			the mask and rule table entry by the data of the field. But you should
*			note that if the field of the parameter is not byte alignment, you 
*			should do some shift operation first before setting.
*
* INPUT:
*        pstMaskPar			-	Pointer to parameter inputing.
*        
* OUTPUT:
*        pstMaskEntry		-	Pointer to classify mask table entry.
*        pstRuleEntry		-	Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK							-	Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	Input pointer is null.
*		 CLASSIFY_NULL_FIELDMAP			-	FieldMap is null.
*		 CLASSIFY_INVALID_FIELD_TYPE	-	Field type is wrong.
*		 CLASSIFY_WINDOWS_OUTOF_RANGE	-	Input index of parameter exceeds
*												maximal defined index.
*		 CLASSIFY_INVALID_FOFFSET_NUM	-	Invalid offset ID.
*
* SEE ALSO: 
*/
OPL_STATUS clsEIIMaskMake(CLASSIFY_MASK_PARAMETER_t *pMaskPar,
								   CLASSIFY_MASK_ENTRY_t *pMaskEntry);
/*******************************************************************************
*
* clsEIIRuleMake	-	Make classify mask and rule table entry.
*
* DESCRIPTION:
*		 This function mainly divided into two parts. Firstly, dicide the window
*			offset through the field list of inputting parameter. Secondly, set
*			the mask and rule table entry by the data of the field. But you should
*			note that if the field of the parameter is not byte alignment, you 
*			should do some shift operation first before setting.
*
* INPUT:
*        pstRulePar			-	Pointer to parameter inputing.
*        
* OUTPUT:
*        pstMaskEntry		-	Pointer to classify mask table entry.
*        pstRuleEntry		-	Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK							-	Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	Input pointer is null.
*		 CLASSIFY_NULL_FIELDMAP			-	FieldMap is null.
*		 CLASSIFY_INVALID_FIELD_TYPE	-	Field type is wrong.
*		 CLASSIFY_WINDOWS_OUTOF_RANGE	-	Input index of parameter exceeds
*												maximal defined index.
*		 CLASSIFY_INVALID_FOFFSET_NUM	-	Invalid offset ID.
*
* SEE ALSO: 
*/
OPL_STATUS clsEIIRuleMake(CLASSIFY_RULE_PARAMETER_t *pRulePar,
								   CLASSIFY_RULE_ENTRY_t *pRuleEntry, UINT16 maskID);

/*******************************************************************************
*
* clsPPPoeMaskMake	-	Make classify mask and rule table entry.
*
* DESCRIPTION:
*		 This function mainly divided into two parts. Firstly, dicide the window
*			offset through the field list of inputting parameter. Secondly, set
*			the mask and rule table entry by the data of the field. But you should
*			note that if the field of the parameter is not byte alignment, you 
*			should do some shift operation first before setting.
*
* INPUT:
*        pstRulePar			-	Pointer to parameter inputing.
*        
* OUTPUT:
*        pstMaskEntry		-	Pointer to classify mask table entry.
*        pstRuleEntry		-	Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK							-	Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	Input pointer is null.
*		 CLASSIFY_NULL_FIELDMAP			-	FieldMap is null.
*		 CLASSIFY_INVALID_FIELD_TYPE	-	Field type is wrong.
*		 CLASSIFY_WINDOWS_OUTOF_RANGE	-	Input index of parameter exceeds
*												maximal defined index.
*		 CLASSIFY_INVALID_FOFFSET_NUM	-	Invalid offset ID.
*        CLASSIFY_INVALID_FIELD_NUM     -   Invalid field number
*
* SEE ALSO: 
*/
OPL_STATUS clsPPPoeMaskMake(CLASSIFY_MASK_PARAMETER_t *pMaskPar,
								   CLASSIFY_MASK_ENTRY_t *pMaskEntry);

/*******************************************************************************
*
* clsPPPoeRuleMake	-	Make classify mask and rule table entry.
*
* DESCRIPTION:
*		 This function mainly divided into two parts. Firstly, dicide the window
*			offset through the field list of inputting parameter. Secondly, set
*			the mask and rule table entry by the data of the field. But you should
*			note that if the field of the parameter is not byte alignment, you 
*			should do some shift operation first before setting.
*
* INPUT:
*        pstRulePar			-	Pointer to parameter inputing.
*        
* OUTPUT:
*        pstMaskEntry		-	Pointer to classify mask table entry.
*        pstRuleEntry		-	Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK							-	Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	Input pointer is null.
*		 CLASSIFY_NULL_FIELDMAP			-	FieldMap is null.
*		 CLASSIFY_INVALID_FIELD_TYPE	-	Field type is wrong.
*		 CLASSIFY_WINDOWS_OUTOF_RANGE	-	Input index of parameter exceeds
*												maximal defined index.
*		 CLASSIFY_INVALID_FOFFSET_NUM	-	Invalid offset ID.
*        CLASSIFY_INVALID_FIELD_NUM     -   Invalid field number
*
* SEE ALSO: 
*/
OPL_STATUS clsPPPoeRuleMake(CLASSIFY_RULE_PARAMETER_t *pRulePar,
								   CLASSIFY_RULE_ENTRY_t *pRuleEntry, UINT16 maskID);


/*******************************************************************************
*
* clsRuleAddrGet	-	Allocate the rule ID and address offset for certain 
*								address entry.
*
* DESCRIPTION:
*		 This function allocates the rule ID and address offset for certain address
*			entry. Firstly, check if there is any space in the hash address. Then,
*			check which group has the most free ID. Finally, calculate the rule
*			ID and address offset.
*
* INPUT:
*		 RuleID		    -	classify rule ID.
*
* OUTPUT:
*		 RuleAddr		-	Pointer to index of entry of classify hash address table.
*		 AddrOff		-	Pointer to offset of certain entry of classify address table.
*
* RETURNS:
*        OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 CLASSIFY_ADDR_TABLE_8COLLISION	-	The entry of classify address table is full.
*		 CLASSIFY_RULE_TABLE_FULL		-	All the classify rule table is full.
*
* SEE ALSO: 
*/
OPL_STATUS clsRuleAddrGet(UINT16 RuleID, UINT16 *RuleAddr, UINT16 *AddrOff);

 /*******************************************************************************
 *
 * candFfieldAddrLenGet - Get the start bit and total bits of specified parameter.
 *
 * DESCRIPTION:
 *        First distinguish the type of received packet, then get the start bit
 *        and total bits of specified parameter directly from defined global array.
 *
 * INPUT:
 *        fieldNum             -       Index of parameter in FieldMap.
 *        ucPktType            -       Type of the packet received.
 *        
 * OUTPUT:
 *        pFieldStartAddr      -       Start bit of specified parameter in packet.
 *        pFieldLen            -       Total bits of specified parameter in packet.
 *
 * RETURNS:
 *        OPL_OK                        - Operation is successful.
 *        OPL_ERR_NULL_POINTER   		- Input pointer is null.
 *        CLASSIFY_WINDOWS_OUTOF_RANGE  - Input index of parameter exceeds                                                                            maximal defined index.
 *        CLASSIFY_INVALID_PKT_TYPE     - Not defined packet type.
 *
 * SEE ALSO: 
 */
 OPL_STATUS candFfieldAddrLenGet(UINT8 fieldNum, 
               UINT16 *pFieldStartAddr, UINT16 *pFieldLen, UINT8 pktType);
 

/*******************************************************************************
*
* clsAddrMapToTbl	- 	Map classify rule ID to address table.
*
* DESCRIPTION:
*        This function map classify rule ID to address table.
*
* INPUT:
*		 offset		-	Offset of classify address table.
*		 ruleID			-	Classify rule ID.
*        
* OUTPUT:
*		 pulBuf			-	Pointer to address buffer.
*
* RETURNS:
*        OPL_OK			-	Operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS clsAddrMapToTbl(UINT16 offset 
                  ,UINT16 ruleID ,CLASSIFY_ADDR_TBL_t *pBuf);

/*******************************************************************************
*
* clsTblMapToAddr	- 	Read classify rule ID from address table.
*
* DESCRIPTION:
*        This function read classify rule ID from address table.
*
* INPUT:
*		 offset		-	Offset of classify address table.
*		 pBuf			-	Pointer to address buffer.
*        
* OUTPUT:
*		 pRule		-	Pointer to classify rule ID.
*
* RETURNS:
*        OPL_OK		-	Operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS clsTblMapToAddr(UINT16 offset 
                  ,UINT16 *pRule ,CLASSIFY_ADDR_TBL_t *pBuf);
 
 /*******************************************************************************
 *
 * clsMselModByMask	-	Move the mask entry from source to destination.
 *
 * DESCRIPTION:
 *		 Move the mask entry from source to destination.
 *
 * INPUT:
 *		 ucSrcMsel			-	Source mask index.
 *		 ucDestMsel		-	Destination mask index.
 *
 * OUTPUT:
 *		 NULL.
 *
 * RETURNS:
 *        OPL_OK						-	If the operation is successful.
 *        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
 *
 * SEE ALSO: 
 */
 OPL_STATUS clsMselModByMask(UINT8 srcMsel, UINT8 destMsel);
 
 
 /*******************************************************************************
 *
 * clsMselFlagSet	-	Make flag to selected mask entry.
 *
 * DESCRIPTION:
 *		 Make flag to selected mask entry. Flag indicates the entry is malloced
 *		 or cleared.
 *
 * INPUT:
 *		 msel		-	Selected mask ID.
 *		 opType		-	Action type which is select or clear.
 *
 * OUTPUT:
 *		 NULL.
 *
 * RETURNS:
 *		 NULL.
 *
 * SEE ALSO: 
 */
 void clsMselFlagSet(UINT8 msel, UINT8 opType);
 
 
/*******************************************************************************
*
* clsMselModByFlag	-	Change the reference mask of the rule.
*
* DESCRIPTION:
*		 Change the reference mask of the rule.
*
* INPUT:
*		 msel			-	Selected mask ID.
*
* OUTPUT:
*		 NULL.
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsMselModByFlag(UINT8 msel);

/*******************************************************************************
*
* clsInit	-	Change the reference mask of the rule.
*
*
* SEE ALSO: 
*/
OPL_STATUS clsInit(void);

/*******************************************************************************
*
* clsRuleEntryHwWrite	-	write a rule to the classify rule table.
*
* DESCRIPTION:
*		 write a rule to the classify rule table.
*
* INPUT:
*		 entryId: rule entryId
*        pClsRuleEntry: value
*
* OUTPUT:
*		 NULL.
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsRuleEntryHwWrite(UINT32 entryId,CLASSIFY_RULE_TBL_t *pClsRuleEntry);

/*******************************************************************************
*
* clsRuleEntryHwRead	-	read a rule from the classify rule table.
*
* DESCRIPTION:
*		 write a rule to the classify rule table.
*
* INPUT:
*		 entryId: rule entryId
*
* OUTPUT:
*		   pClsRuleEntry: value
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsRuleEntryHwRead(UINT32 entryId,CLASSIFY_RULE_TBL_t *pClsRuleEntry);

/*******************************************************************************
*
* clsAddrEntryHwWrite	-	write a addr to the classify addr table.
*
* DESCRIPTION:
*		 write a addr to the addr table.
*
* INPUT:
*		 entryId: rule entryId
*        pClsAddrEntry: value
*
* OUTPUT:
*		 NULL.
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsAddrEntryHwWrite(UINT32 entryId,CLASSIFY_ADDR_TBL_t *pClsAddrEntry);

/*******************************************************************************
*
* clsAddrEntryHwRead	-	read a addr from the classify addr table.
*
* DESCRIPTION:
*		 write a addr to the addr table.
*
* INPUT:
*		 entryId: rule entryId
*        pClsAddrEntry: value
*
* OUTPUT:
*		 NULL.
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsAddrEntryHwRead(UINT32 entryId,CLASSIFY_ADDR_TBL_t *pClsAddrEntry);

/*******************************************************************************
*
* clsMaskEntryHwWrite	-	write a mask to the classify mask table.
*
* DESCRIPTION:
*		 write a addr to the addr table.
*
* INPUT:
*		 entryId: rule entryId
*        pClsMaskEntry: value
*
* OUTPUT:
*		 NULL.
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsMaskEntryHwWrite(UINT32 entryId,CLASSIFY_MASK_TBL_t *pClsMaskEntry);

/*******************************************************************************
*
* clsMaskEntryHwRead	-	read a mask from the classify mask table.
*
* DESCRIPTION:
*		 write a mask to the mask table.
*
* INPUT:
*		 entryId: rule entryId
*       
*
* OUTPUT:
*		 pClsMaskEntry: value
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsMaskEntryHwRead(UINT32 entryId,CLASSIFY_MASK_TBL_t *pClsMaskEntry);
	
/*******************************************************************************
*
* clsRuleEntryShow	-	show classify rule table in asic.
*
* DESCRIPTION:
*		 show classify rule table in asic.
*
* INPUT:
*		startId:
*		endId:
*       
* OUTPUT:
*		
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsRuleEntryShow(UINT32 startId,UINT32 endId);

/*******************************************************************************
*
* clsAddrEntryShow	-	show classify address table in asic.
*
* DESCRIPTION:
*		 show classify address table in asic.
*
* INPUT:
*		startId:
*		endId:
*       
* OUTPUT:
*		
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsAddrEntryShow(UINT32 startId,UINT32 endId);

/*******************************************************************************
*
* clsMaskEntryShow	-	show classify mask table in asic.
*
* DESCRIPTION:
*		 show classify mask table in asic.
*
* INPUT:
*		startId:
*		endId:
*       
* OUTPUT:
*		
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsMaskEntryShow(UINT32 startId,UINT32 endId);

/*******************************************************************************
*
* clsCounterEntryShow	-	show classify counter table in asic.
*
* DESCRIPTION:
*		 show classify counter table in asic.
*
* INPUT:
*		startId:
*		endId:
*       
* OUTPUT:
*		
*
* RETURNS:
*		 OPL_OK							-	If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL	-	If the input pointer is NULL.
*		 Others                 		-   an appropriate specific error code defined
*											for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS clsCounterEntryShow(UINT32 startId,UINT32 endId);

OPL_STATUS clsLastWindowCreate(
																UINT32 comVal,
																UINT32 mask,
																UINT8 imask,
																UINT8 emask,
																UINT8 tranAct,
																UINT8 egressPort,
																UINT8 ingressPort,
																UINT8 remarkAct,
																UINT8 remarkCos,
																UINT8 remarkIcos,
																UINT8 remarkVid,
																UINT8 enable);


OPL_STATUS clsFirstWindowCreate(
																UINT32 comVal,
																UINT32 mask,
																UINT8 imask,
																UINT8 emask,
																UINT8 tranAct,
																UINT8 egressPort,
																UINT8 ingressPort,
																UINT8 remarkAct,
																UINT8 remarkCos,
																UINT8 remarkIcos,
																UINT8 remarkVid,
																UINT8 enable);


OPL_STATUS cls72Start4ByteCreate(
																UINT32 comVal,
																UINT32 mask,
																UINT8 imask,
																UINT8 emask,
																UINT8 tranAct,
																UINT8 egressPort,
																UINT8 ingressPort,
																UINT8 remarkAct,
																UINT8 remarkCos,
																UINT8 remarkIcos,
																UINT8 remarkVid,
																UINT8 enable);


OPL_STATUS cls74To77Create(
																UINT32 comVal,
																UINT32 mask,
																UINT8 imask,
																UINT8 emask,
																UINT8 tranAct,
																UINT8 egressPort,
																UINT8 ingressPort,
																UINT8 remarkAct,
																UINT8 remarkCos,
																UINT8 remarkIcos,
																UINT8 remarkVid,
																UINT8 enable);

OPL_STATUS clsMaskRuleMake2ONU(CLASSIFY_PARAMETER_t *pMaskRulePar,
                               CLASSIFY_MASK_ENTRY_t *pMaskEntry,
                               CLASSIFY_RULE_ENTRY_t *pRuleEntry);

#endif
