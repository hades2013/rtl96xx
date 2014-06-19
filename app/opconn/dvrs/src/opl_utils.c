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
* FILENAME: opl_.c
*
* DESCRIPTION: Opulan privated defined data type and constants. this
*	file is supposed to be included by driver Module source files of 
*	Opulan chip products.
*
* Date Created: 17/05/2007
*
* Authors(optional): yzhang
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/dvrs/opconn/src/opl_utils.c#1 $
* $Log: opl_utils.c,v $
* Revision 1.2  2007/09/14 11:51:14  yzhang
* Add Auto Bind operation for EOC module; add system counter to CLT driver
*
* Revision 1.1.1.1  2007/08/28 09:36:49  schen
* ADD FOR PURE USER SPACE
*
* Revision 1.6  2007/07/25 06:11:05  schen
* Move CRC16BitShift(),CRC32BitShift() here from commoninfo.c
*
* Revision 1.5  2007/06/22 07:24:21  rzhou
* Add header file ctype.h
*
* Revision 1.4  2007/06/21 06:57:29  schen
* Change naming cltString2Ip()
*
* Revision 1.3  2007/06/19 02:58:46  schen
* Move IPMUX_inet_addr() and String2Mac() to this file from stripandmac.c
*
* Revision 1.2  2007/06/01 05:29:03  rzhou
* clt-mpw code clean up
*
* Revision 1.1.1.1  2007/05/30 08:38:11  rzhou
* Reimport clt-mpw project
*
* Revision 1.1.1.1  2007/05/29 09:10:57  schen
* create clt project
*
**************************************************************************/

/***** INCLUDE FILES *****/
#include<ctype.h>
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include <time.h>

/*******************************************************************************
*
* atohex		- convert 2 Bytes Hexidecimal String to hexadecimal value.
*
* DESCRIPTION:
* 	This function accept string input and convert it into hexadecimal. the length
*	of the string is limited as 2 chars, and the result value is output as
*	function returned value.
*	
* INPUTS:
*	pStr	- pointer to the ASCII string with "a/A - f/F", "0-9".
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	the output hexadecimal values.
*	if invalid string passed, the value is not reliable.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
UINT8 atohex(INT8 *pStr)
{
	UINT8 hexCharInStr[2];
	INT32 iiii;

	for (iiii = 0; iiii < 2; iiii++)
	{
			/* input validation */
		if (('0' <= ((UINT8 *)pStr)[iiii]) && ('9' >= ((UINT8 *)pStr)[iiii]))
		{
			hexCharInStr[iiii] = ((UINT8 *)pStr)[iiii] - (UINT8)'0';
			/* OPL_PRINTF("hexCharInStr[%d]=%x, ((UINT8 *)pStr)[%d]=%x\n", iiii, hexCharInStr[iiii], iiii, ((UINT8 *)pStr)[iiii]); */
		}
		else
		{
			if (('a' <= ((UINT8 *)pStr)[iiii]) && ('f' >= ((UINT8 *)pStr)[iiii]))
			{
				hexCharInStr[iiii] = ((UINT8 *)pStr)[iiii] - (UINT8)'a' + 10;
				/* OPL_PRINTF ("hexCharInStr[%d]=%x, (UINT8 *)pStr)[%d]=%x\n", iiii, hexCharInStr[iiii], iiii, ((UINT8 *)pStr)[iiii]); */
			}
			else
			{
				if (('A' <= ((UINT8 *)pStr)[iiii]) && ('F' >= ((UINT8 *)pStr)[iiii]))
				{
					hexCharInStr[iiii] = ((UINT8 *)pStr)[iiii] - (UINT8)'A' + 10;
					/* OPL_PRINTF ("hexCharInStr[%d]=%x, (UINT8 *)pStr)[%d]=%x\n", iiii, hexCharInStr[iiii], iiii, ((UINT8 *)pStr)[iiii]);*/
				}
				else
				hexCharInStr[iiii] = 0;
			}
		}
	}

	return hexCharInStr[0] * 16 + hexCharInStr[1];
}

void oplMacAddrToStr(UINT8 *pMac, UINT8 *pStr, UINT8 cDelimiter)
{
	int ii, jj; 
	for (ii = OPL_ZERO, jj = OPL_ZERO; jj < OPL_MAC_LEN_IN_BYTES; ii += 3, jj++) 
	{ 
		sprintf ((char *)((int)pStr + ii), "%02x%c", ((UINT8 *)pMac)[jj], cDelimiter); 
	} 
	((char *)pStr)[ii - 1] = '\0'; 
}
  
void oplStrToMacAddr(UINT32 *pMac, UINT8 *pStr)
{
	int ii, jj; 
	for (ii = OPL_ZERO, jj = OPL_ZERO; jj < OPL_MAC_LEN_IN_BYTES; ii += 3, jj++) 
	{ 
		((char *)pStr)[ii + 2] = '\0'; 
		((UINT8 *)pMac)[jj] = atohex ((char *)((int)pStr + ii)); 
	} 
}
void oplMacAddrCpy(UINT8 *pMacSrc, UINT8 *pMacDst)	
{
	int ii; 
	for (ii = OPL_ZERO; ii < OPL_MAC_LEN_IN_BYTES; ii++)
	{ 
		((UINT8 *)pMacDst)[ii] = ((UINT8 *)pMacSrc)[ii]; 
	} 
}

OPL_BOOL oplMacAddrEqual(UINT8 *pMac0, UINT8 *pMac1)
{
	return (((pMac0)[0] == (pMac1)[0] && (pMac0)[1] == (pMac1)[1] && (pMac0)[2] == (pMac1)[2] && (pMac0)[3] == (pMac1)[3] &&
											(pMac0)[4] == (pMac1)[4] && (pMac0)[5] == (pMac1)[5]) ? OPL_TRUE : OPL_FALSE);
}


UINT32 string2Ip(char *inetString    /* string inet address */ )
{
#define MAX_PARTS 4 	/* Maximum number of parts in an IP address. */

    UINT32 val, base, n;
    char   c;
    UINT32 parts[MAX_PARTS], *pp = parts;

    /* check for NULL pointer */

    if (inetString == (char *) NULL)
	{
	    return (-1);
	}

again:

    /* Collect number up to ``.''.  Values are specified as for C:
     * 0x=hex, 0=octal, other=decimal. */

    val = 0; base = 10;
    if (*inetString == '0')
	    base = 8, inetString++;
	
    if (*inetString == 'x' || *inetString == 'X')
	    base = 16, inetString++;
	    
    while ((c = *inetString))
	{
    	if (isdigit (c))
    	{
    	    val = (val * base) + (c - '0');
    	    inetString++;
    	    continue;
    	}

    	if (base == 16 && isxdigit (c))
    	{
    	    val = (val << 4) + (c + 10 - (islower (c) ? 'a' : 'A'));
    		inetString++;
    		continue;
    	}
	    break;
	} /* while */

    if (*inetString == '.')
	{
    	/*
    	 * Internet format:
    	 *	a.b.c.d
    	 *	a.b.c	(with c treated as 16-bits)
    	 *	a.b	(with b treated as 24 bits)
    	 * Check each value for greater than 0xff for each part of the IP addr.
    	 */

	    if ((pp >= parts + (MAX_PARTS - 1)) || val > 0xff)
	    {
	        return (-1);
	    }
	    
	    *pp++ = val, inetString++;
	    goto again;
	}

    /* Check for trailing characters */

    if (*inetString && !isspace (*inetString)) 
	{
	    return (-1);
	}
    *pp++ = val;

    /* Concoct the address according to the number of parts specified. */

    n = pp - parts;
    switch ((int) n)
	{
    	case 1:				/* a -- 32 bits */
    	    val = parts[0];
    	    break;
    
    	case 2:				/* a.b -- 8.24 bits */
    	    if (val > 0xffffff)
    	    {
    		    return (-1);
    		}
    
    	    val = (parts[0] << 24) | parts[1];
    	    break;
    
    	case 3:				/* a.b.c -- 8.8.16 bits */
    	    if (val > 0xffff)
    	    {
    		    return (-1);
    		}
    
    	    val = (parts[0] << 24) | (parts[1] << 16) | parts[2];
    	    break;
    
    	case 4:				/* a.b.c.d -- 8.8.8.8 bits */
    	    if (val > 0xff)
    	    {
    		    return (-1);
    		}
    
    	    val = (parts[0] << 24) | (parts[1] << 16) |
    		  (parts[2] << 8) | parts[3];
    	    break;
    
    	default:
    	    return (-1);
	}

    return (val);
}

int string2mac(UINT8 *macString, UINT8 *mac)
{
	UINT32	strLen = 0;
	UINT32	i = 0;
	int  	macAddr[6];
	int		nVaildNum;

	if ((NULL == macString) || (NULL == mac))
	{
		return -1;
	}
	strLen = strlen(macString);
	if ( strLen != MAC_STRING_LENG)
	{
		return -1;
	}
	
	/*convert string to value*/
	nVaildNum = sscanf(macString,"%02x:%02x:%02x:%02x:%02x:%02x",
		&macAddr[0],&macAddr[1],&macAddr[2],&macAddr[3],&macAddr[4],&macAddr[5]);

	if (nVaildNum != 6)
	{
		return -1;
	}

#if 1
	for (i = 0; i < 6; i ++)
	{
		mac[i] = (char)macAddr[i];
	}
#else	
	for (i = 0; i < 6; i ++)
	{
		macTmp[i] = (char)macAddr[i];
	}
	MAC_SWAP(mac, macTmp);
#endif
	return 0;
}

OPL_STATUS TableAddrCal8(UINT8 *pucValue, UINT32 ulValueLen, UINT8 usAddrMask, UINT8 *pusAddress)
{
	UINT8     usCrcValue = 0;
	OPL_STATUS iStatus;

	if (NULL == pucValue)
	{
		return OPL_ERR_NULL_POINTER;
	}

	iStatus = CRC8BitShift(pucValue, ulValueLen, &usCrcValue);
	if (OPL_OK != iStatus)
	{
		return iStatus ;
	}

	*pusAddress = usAddrMask & usCrcValue;
	
	return OPL_OK;
}


OPL_STATUS TableAddrCal16(UINT8 *pucValue, UINT32 ulValueLen, UINT16 usAddrMask, UINT16 *pusAddress)
{
	UINT16     usCrcValue = 0;
	OPL_STATUS iStatus;

	if (NULL == pucValue)
	{
		return OPL_ERR_NULL_POINTER;
	}

	iStatus = CRC16BitShift(pucValue, ulValueLen, &usCrcValue);
	if (OPL_OK != iStatus)
	{
		return iStatus ;
	}

	*pusAddress = usAddrMask & usCrcValue;
	
	return OPL_OK;
}



OPL_STATUS TableAddrCal32(UINT8 *pucValue,UINT32 ulValueLegnth
                                   ,UINT32 ulAddrMask,UINT32 *pulAddress)
{
    UINT32 ulCrcValue = 0;
    OPL_STATUS iStatus;


    if(NULL == pucValue)
    {
        return OPL_ERR_NULL_POINTER;
    }

    iStatus = CRC32BitShift(pucValue,ulValueLegnth,&ulCrcValue);
    if(OPL_OK != iStatus)
    {
        return iStatus;
    }

    *pulAddress = ulAddrMask & ulCrcValue;
    return OPL_OK;
}



OPL_STATUS CRC16BitShift(UINT8 *pucValue,UINT32 ulValueLegnth
                                                       ,UINT16 *pusCrcValue)
{
	UINT8  ucCharBit  = 0;
	UINT16 usCrc     = 0xFFFF;

	if (NULL == pucValue)
	{
		return OPL_ERR_NULL_POINTER;
	}

	while(0 != ulValueLegnth--)
	{
		for (ucCharBit = CRC_BYTE_BIT_COUNT; 0 != ucCharBit; ucCharBit /= 2)
		{
			if (0 != (usCrc & CRC16_MOST_SIGNIFICIANT_BIT))
			{
				usCrc *= 2;
				usCrc ^= CRC16_POLYNOMIAL_CCITT;
			}
			else
			{
				usCrc *= 2;
			}

			if (((*pucValue) & ucCharBit) != 0)
			{
				usCrc ^= CRC16_POLYNOMIAL_CCITT;
			}
		}

		pucValue++;
	}

	*pusCrcValue = usCrc;

	return(OPL_OK);
}

OPL_STATUS CRC8BitShift(UINT8 *pucValue,UINT32 ulValueLegnth
                                                       ,UINT8 *pusCrcValue)
{
	UINT8  ucCharBit  = 0;
	UINT8  usCrc     = 0XFF;

	if (NULL == pucValue)
	{
		return OPL_ERR_NULL_POINTER;
	}

	while(0 != ulValueLegnth--)
	{
		for (ucCharBit = CRC_BYTE_BIT_COUNT; 0 != ucCharBit; ucCharBit /= 2)
		{
			if (0 != (usCrc & CRC8_MOST_SIGNIFICIANT_BIT))
			{
				usCrc *= 2;
				usCrc ^= CRC8_POLYNOMIAL_CCITT;
			}
			else
			{
				usCrc *= 2;
			}

			if (((*pucValue) & ucCharBit) != 0)
			{
				usCrc ^= CRC8_POLYNOMIAL_CCITT;
			}
		}

		pucValue++;
	}

	*pusCrcValue = usCrc;

	return(OPL_OK);
}




OPL_STATUS CRC32BitShift(UINT8 *pucValue,UINT32 ulValueLegnth
                                                  ,UINT32 *pulCrcValue)
{
    UINT8  ucCharBit = 0;
	UINT32 ulCrc     = 0;

	if(NULL == pucValue)
	{
	    return OPL_ERR_NULL_POINTER;
	}

	while(0 != ulValueLegnth--)
	{
		for(ucCharBit = CRC_BYTE_BIT_COUNT; 0 != ucCharBit; ucCharBit /= 2)
		{
			if(0 != (ulCrc&CRC32_MOST_SIGNIFICIANT_BIT))
			{
				ulCrc *= 2;
				ulCrc ^= CRC32_POLYNOMIAL;
			}
			else
			{
				ulCrc *= 2;
			}

			if(((*pucValue)&ucCharBit) != 0)
			{
				ulCrc ^= CRC32_POLYNOMIAL;
			}
		}

		pucValue++;
	}

    *pulCrcValue = ulCrc;
	return(OPL_OK);
}

unsigned int string2uint(unsigned char *input)
{
    unsigned int val = 0;
    unsigned int len;
    unsigned int tmp;
    unsigned int count;
    
    len = strlen(input);

    if(len <= 2)
    {
        return atoi(input);
    }
    
    if(input[1] == 'x'||input[1] == 'X')
    {
        for(count = len - 1; count >= 2; count--)
        {
            switch(input[count])
            {
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                    tmp = input[count] - 'a' + 10;
                    val = val + (tmp<<((len - 1 - count)*4));
                    break;
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                    tmp = input[count] - 'A' + 10;
                    val = val + (tmp<<((len - 1 - count)*4));
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    tmp = input[count] - '0';
                    val = val + (tmp<<((len - 1 - count)*4));
                    break;
                default:
                    printf("input string error.\n");
                    break;
            }
        }
    }else
    {
        val = atoi(input);
    }
    
    return val;
}

OPL_STATUS string2minuscule(UINT8 *pBuff)
{
	UINT32 len;
	UINT32 index;

	if(OPL_NULL == pBuff)
	{
		return OPL_ERR_NULL_POINTER;
	}

	len = OPL_STRLEN(pBuff);

	for(index = OPL_ZERO; index < len; index++)
	{
		if('A' <= pBuff[index] && 'Z' >= pBuff[index])
		{
			pBuff[index] = pBuff[index] + 32;
		}
	}
	return OPL_OK;
}

OPL_STATUS stringIsNum(UINT8 *pBuff)
{
    unsigned int len;
    unsigned int count;

	if(OPL_NULL == pBuff)
	{
		return 0;
	}
    
    len = strlen(pBuff);

    if(len > 2 && (pBuff[1] == 'x'||pBuff[1] == 'X')&& pBuff[0] == '0')
    	{
    		for(count = 2; count < len; count++)
		{
			switch(pBuff[count])
			{
				case 'a':
                	case 'b':
                	case 'c':
                	case 'd':
                	case 'e':
                	case 'f':
				case 'A':
                	case 'B':
                	case 'C':
               	case 'D':
                	case 'E':
                	case 'F': 
				case '0':
                	case '1':
                	case '2':
                	case '3':
                	case '4':
                	case '5':
                	case '6':
                	case '7':
                	case '8':
                	case '9':
					break;
				default:
					return 0;
			}
		}
		return 1;		
    	}

	for(count = OPL_ZERO; count < len; count++)
	{
		switch(pBuff[count])
		{
			case '0':
             	case '1':
             	case '2':
             	case '3':
             	case '4':
             	case '5':
             	case '6':
             	case '7':
             	case '8':
            	case '9':
				break;
			default:
				return 0;
		}
	}
	return 1;
}

OPL_STATUS getSytemDate(UINT8 *pDate)
{
	UINT8 *wday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sta",OPL_NULL};
	time_t timeep;
	struct tm *p;

	time(&timeep);
	p = gmtime(&timeep);

	sprintf(pDate,"%d_%d_%d_%s_%d_%d_%d",
		(1900+p->tm_year),
		(1+p->tm_mon),
		p->tm_mday,
		wday[p->tm_wday],
		p->tm_hour,
		p->tm_min,
		p->tm_sec);
	return OPL_OK;
}

int convertFirstStringType(unsigned char *listString,unsigned int portArry[],unsigned int numOfPorts)
{
    unsigned tmpPortId = atoi(listString);

    if(tmpPortId < numOfPorts)
    {
        portArry[tmpPortId] = 1;
        return 0;
    }else
    {
        OPL_UTILS_LOG_TRACE();
        return -1;
    }
}

int convertSecondStringType(unsigned char *listString,unsigned int portArry[],unsigned int numOfPorts)
{
    unsigned char *p = listString;
    unsigned int portEndId;
    unsigned int portStartId = atoi(listString);
    unsigned int index;
    
    for(p = listString; *p != '-'; p++)
    {

    }
    portEndId = atoi((unsigned char *)&p[1]);

    if(portStartId > portEndId)
    {
        index = portStartId;
        portStartId = portEndId;
        portEndId = portStartId;
    }
    
    if(portEndId >= numOfPorts)
    {
        OPL_UTILS_LOG_TRACE();
        return -1;
    }
    
    for(index = portStartId; index <= portEndId; index++)
    {
        portArry[index] = 1;
    }
    return 0;
}

/*******************************************************************************
* convertPortListString2PortArry	- this function is used to convert port list string to portArry
*
* DESCRIPTION:
*	
* INPUT:
*		 listString: such as 1,4,50-58,90-110,125
*		 numOfPorts: the total of ports in the system
* OUTPUT:
*		 portArry: if the port 1 is in the liststring ,than portArry[1] = 1 else ,portArry[1] = 0
* RETURNS:
*    	 -1: input error.
* SEE ALSO: 
*/
int convertPortListString2PortArry(unsigned char *listString,unsigned int *portArry,unsigned int numOfPorts)
{
    unsigned char *p;
    unsigned char tmpBuff[256];
    unsigned int len;
    unsigned int pIndex;
    unsigned int index = 0;
    unsigned int mode = 0;
    
    int retVal = 0;
    
    p = listString;
    len = strlen(listString);


    /* begin added by jiangmingli for bug 2813 */
    memset(portArry, 0, numOfPorts * sizeof(unsigned int));
    /* end added by jiangmingli for bug 2813 */
        
    for(pIndex = 0; pIndex < len; pIndex++)
    {    
        switch(p[pIndex])
        {
            case ',':
                if(pIndex == 0)
                {
                    OPL_UTILS_LOG_TRACE();
                    return -1;
                }else
                {
                    if(p[pIndex - 1] < '0' || p[pIndex - 1] > '9')
                    {
                        OPL_UTILS_LOG_TRACE();
                        return -1;
                    }
                }
                
                if(pIndex + 1 >= len)
                {
                    OPL_UTILS_LOG_TRACE();
                    return -1;
                }
                if(p[pIndex + 1] < '0' || p[pIndex + 1] > '9')
                {
                    OPL_UTILS_LOG_TRACE();
                    return -1;
                }
                
                tmpBuff[index] = '\0';

                if(1 == mode)  /* include '-'*/
                {
                    retVal = convertSecondStringType(&tmpBuff[0],portArry,numOfPorts);
                    if(0 != retVal)
                    {
                        OPL_UTILS_LOG_TRACE();
                        return retVal;
                    }
                }else
                {
                  retVal = convertFirstStringType(&tmpBuff[0],portArry,numOfPorts);
                  if(0 != retVal)
                  {
                    OPL_UTILS_LOG_TRACE();
                    return retVal;
                  }
                }
                index = 0;
                mode = 0;
                memset(&tmpBuff[0],0,256);
                break;
            case '-':
                if(1 == mode)
                {
                    OPL_UTILS_LOG_TRACE();
                    return -1;
                }
                
                if(pIndex == 0)
                {
                    OPL_UTILS_LOG_TRACE();
                    return -1;
                }else
                {
                    if(p[pIndex-1] < '0'|| p[pIndex-1] > '9')
                    {
                        OPL_UTILS_LOG_TRACE();
                        return -1;
                    }
                }
                
                if(pIndex + 1 >= len)
                {
                    OPL_UTILS_LOG_TRACE();
                    return -1;
                }
                
                if(p[pIndex + 1] < '0' || p[pIndex + 1] > '9')
                {
                    OPL_UTILS_LOG_TRACE();
                    return -1;
                }
                mode = 1;
                tmpBuff[index] = p[pIndex];
                index++;
                break;
            case '0':
                if(pIndex == 0 && (pIndex+1) < len)
                {
                    if(p[pIndex + 1] > '0' && p[pIndex + 1] < '9')
                    {
                        OPL_UTILS_LOG_TRACE();
                        return -1;
                    }
                }
                
                if(pIndex > 0 && (pIndex + 1) < len)
                {
                    if((p[pIndex - 1] < '0' || p[pIndex - 1] > '9')&&
                    (p[pIndex + 1] > '0' && p[pIndex + 1] < '9'))
                    {
                        OPL_UTILS_LOG_TRACE();
                        return -1;
                    }
                }
                tmpBuff[index] = p[pIndex];
                index++;
                if(pIndex+1 == len)
                {
                    tmpBuff[index] = '\0';
                    if(mode == 1)
                    {
                        retVal = convertSecondStringType(&tmpBuff[0],portArry,numOfPorts);
                    }else
                    {
                        retVal = convertFirstStringType(&tmpBuff[0],portArry,numOfPorts);
                    }
                    if(0 != retVal)
                    {
                        OPL_UTILS_LOG_TRACE();
                        return retVal;
                    }
                    return 0;
                }
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                tmpBuff[index] = p[pIndex];
                index++;
                if(pIndex+1 == len)
                {
                    tmpBuff[index] = '\0';
                    if(mode == 1)
                    {
                        retVal = convertSecondStringType(&tmpBuff[0],portArry,numOfPorts);
                    }else
                    {
                        retVal = convertFirstStringType(&tmpBuff[0],portArry,numOfPorts);
                    }
                    if(0 != retVal)
                    {
                        OPL_UTILS_LOG_TRACE();
                        return retVal;
                    }
                    return 0;
                }
                break;
            case ' ':
                tmpBuff[index] = '\0';
                if(1 == mode)  /* include '-'*/
                {
                    retVal = convertSecondStringType(&tmpBuff[0],portArry,numOfPorts);
                    if(0 != retVal)
                    {
                        OPL_UTILS_LOG_TRACE();
                        return retVal;
                    }
                }else
                {
                    retVal = convertFirstStringType(&tmpBuff[0],portArry,numOfPorts);
                    if(0 != retVal)
                    {
                        OPL_UTILS_LOG_TRACE();
                        return retVal;
                    }
                }
                index = 0;
                mode = 0;
                memset(&tmpBuff[0],0,256);
                return 0;
                break;
            default:
                OPL_UTILS_LOG_TRACE();
                return -1;
                break;

        }
    }
}