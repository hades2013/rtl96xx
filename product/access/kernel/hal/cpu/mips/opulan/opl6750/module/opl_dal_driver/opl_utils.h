/*************************************************************************
*
*  COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential                       
*
* 	This software is made available only to customers and prospective        
* 	customers of Opulan Technologies Corporation under license and may be
*	 used only with Opulan semi-conductor products.                           
*
* FILENAME: opl_utils.h
*
* DESCRIPTION: Opulan privated defined utility functions. this
*	file is supposed to be included by driver Module source files of 
*	Opulan chip products for reusable code to saving development time.
*
* Date Created: 07/04/2005
*
* Authors(optional): yzhang
*
**************************************************************************/

#ifndef OPL_UTILS_H
#define OPL_UTILS_H

/***** INCLUDE FILES *****/

/***** DEFINES and ENUMS *****/
	
	/* Byte Order related Utility definitons */

#ifdef OPL_BIG_ENDIANESS

#define	opl_ntohl(x)	(x)
#define	opl_ntohs(x)	(x)
#define	opl_htonl(x)	(x)
#define	opl_htons(x)	(x)

#else /* BYTE ORDER is of LITTLE_ENDIANESS */

#define opl_ntohl(x)	((((x) & 0x000000ff) << 24) | \
			 (((x) & 0x0000ff00) <<  8) | \
			 (((x) & 0x00ff0000) >>  8) | \
			 (((x) & 0xff000000) >> 24))

#define opl_htonl(x)	((((x) & 0x000000ff) << 24) | \
			 (((x) & 0x0000ff00) <<  8) | \
			 (((x) & 0x00ff0000) >>  8) | \
			 (((x) & 0xff000000) >> 24))

#define opl_ntohs(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))

#define opl_htons(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))

#endif	

#define OPL_UTILS_LOG_TRACE() if(0)OPL_DRV_PRINTF(("%s,%s,%d.\n",__FILE__,__FUNCTION__,__LINE__))

#define OPL_NTOHL(x)	(x) = opl_ntohl((UINT32)(x))
#define OPL_NTOHS(x)	(x) = opl_ntohs((UINT16)(x))
#define OPL_HTONL(x)	(x) = opl_htonl((UINT32)(x))
#define OPL_HTONS(x)	(x) = opl_htons((UINT16)(x))

	/* CRC calculate */
#define CRC_BYTE_BIT_COUNT           			0x80
#define CRC8_POLYNOMIAL_CCITT       			0x07
#define CRC8_MOST_SIGNIFICIANT_BIT  		0x80

#define CRC16_POLYNOMIAL_AMERICAN    	0x8005
#define CRC16_POLYNOMIAL_CCITT       		0x1021
#define CRC16_RESULT_LENGTH          			0x0000FFFF
#define CRC16_MOST_SIGNIFICIANT_BIT  	0x00008000
#define CRC32_POLYNOMIAL             				0x04C11DB7
#define CRC32_RESULT_LENGTH          			0xFFFFFFFF
#define CRC32_MOST_SIGNIFICIANT_BIT  		0x80000000

	/* MAC Address related definitons */
#define OPL_MAC_LEN_IN_BYTES					6
#define OPL_MAC_STR_LEN_IN_BYTES			18
#define CLI_MAC_STRING_LEN	                	17   

#define MAC_SWAP(dest, src) \
			dest[0] = src[5]; \
			dest[1] = src[4]; \
			dest[2] = src[3]; \
			dest[3] = src[2]; \
			dest[4] = src[1]; \
			dest[5] = src[0]

#define OPL_MAC_ADDR_TO_STR oplMacAddrToStr

#define OPL_STR_TO_MAC_ADDR oplStrToMacAddr

#define OPL_MAC_ADDR_CPY oplMacAddrCpy 

#define OPL_MAC_ADDR_EQUAL oplMacAddrEqual

#ifndef NULL
#define NULL 0
#endif

UINT8 atohex(INT8 *pStr);

UINT32 string2Ip(char *inetString    /* string inet address */ );

int string2mac(UINT8 *macString, UINT8 *mac);

OPL_STATUS TableAddrCal8(UINT8 *pucValue, UINT32 ulValueLen, UINT8 usAddrMask, UINT8 *pusAddress);

OPL_STATUS TableAddrCal16(UINT8 *pucValue, UINT32 ulValueLen, UINT16 usAddrMask, UINT16 *pusAddress);

OPL_STATUS TableAddrCal32(UINT8 *pucValue,UINT32 ulValueLegnth,UINT32 ulAddrMask,UINT32 *pulAddress);

OPL_STATUS CRC8BitShift(UINT8 *pucValue,UINT32 ulValueLegnth ,UINT8 *pusCrcValue);

OPL_STATUS CRC16BitShift(UINT8 *pucValue,UINT32 ulValueLegnth ,UINT16 *pusCrcValue);

OPL_STATUS CRC32BitShift(UINT8 *pucValue,UINT32 ulValueLegnth ,UINT32 *pulCrcValue);

unsigned int string2uint(unsigned char *input);

OPL_STATUS string2minuscule(UINT8 *pBuff);

OPL_STATUS stringIsNum(UINT8 *pBuff);

//OPL_STATUS getSytemDate(UINT8 *pDate);

int convertFirstStringType(unsigned char *listString,unsigned int portArry[],unsigned int numOfPorts);

int convertSecondStringType(unsigned char *listString,unsigned int portArry[],unsigned int numOfPorts);

int convertPortListString2PortArry(unsigned char *listString,unsigned int *portArry,unsigned int numOfPorts);

OPL_STATUS oplMacToStr(unsigned char *pMac, unsigned char *pMacStr);
int convertFirstStringType(unsigned char *listString,unsigned int portArry[],unsigned int numOfPorts);

int convertSecondStringType(unsigned char *listString,unsigned int portArry[],unsigned int numOfPorts);

int convertPortListString2PortArry(unsigned char *listString,unsigned int *portArry,unsigned int numOfPorts);

OPL_STATUS oplEthAddrGetByName(char *name, UINT8 *mac);

#endif 

