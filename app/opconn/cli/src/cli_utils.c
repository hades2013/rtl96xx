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
* FILENAME:  cli_utils.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 08, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_utils.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include <math.h>

/*****************************************************************************
 *cliByteGet - Get one byte which is inputed from telnet client or console.
 *DESCRIPTION
 *This routine to get one byte which is inputed from telnet client or console.
 *Input:pstEnv,pbyAddr,byMinVal,byMaxVal,pulRetVal.
 *Output:pbyAddr,pulRetVal.
 *Return:STATUS.

 *****************************************************************************/
STATUS cliByteGet(ENV_t *pstEnv, unsigned char *pbyAddr, unsigned char byMinVal,
                        unsigned char byMaxVal, unsigned short *pulRetVal)
{
    char achLine[MAX_POOL_LENGTH];
    unsigned long  ulStrLen;
    unsigned long  ulTemp;
    unsigned short usIsCtrlCLine = FALSE;

    
    *pulRetVal = 0;
    if (NULL == pstEnv || NULL == pbyAddr)
    {
        *pulRetVal = NULLPTRERR;
        return ERROR;
    }
    if (byMinVal > byMaxVal)
    {
        *pulRetVal = RANGEERR;
        return ERROR;
    }
    ulStrLen = cliLineRead(pstEnv, achLine,&usIsCtrlCLine);
    if ((ULONG)ERROR == ulStrLen)
    {
        *pulRetVal = TERMINALERR;
        return ERROR;
    }    
    if (TRUE == usIsCtrlCLine)
    {
        *pulRetVal = CTRLCHITED;
        return OK;
    }
    if (FALSE == cliIsDigits(achLine))
    {
        *pulRetVal = NOTALLDIGITAL;
        return ERROR;
    }    
    if (ulStrLen>0)
    {
        ulTemp = strtoul(achLine, NULL, 10);
        if (ulTemp < byMinVal)
        {
            *pulRetVal = TOOSMALL;
            return ERROR;
        }
        if (ulTemp > byMaxVal)
        {
            *pulRetVal = TOOLARGE;
            return ERROR;            
        }        
        *(unsigned char *)pbyAddr = ulTemp;        
    }
    else
    {
        *pulRetVal = ENTERHITED;        
    }
    
    return OK;
}

/*****************************************************************************
 *cliWordGet - Get one word which is inputed from telnet client or console.
 *DESCRIPTION
 *This routine to get one word which is inputed from telnet client or console.
 *Input:pstEnv,pusAddr,usMinVal,usMaxVal,pulRetVal.
 *Output:pusAddr,pulRetVal.
 *Return:STATUS.
 
 ****************************************************************************/
STATUS cliWordGet(ENV_t *pstEnv, unsigned short *pusAddr, unsigned short usMinVal,
                         unsigned short usMaxVal, unsigned short *pulRetVal)
{
    char achLine[MAX_POOL_LENGTH];
    unsigned long  ulStrLen;
    unsigned long  ulTemp;
    unsigned short usIsCtrlCLine = FALSE;


    *pulRetVal = 0;
    if (NULL == pstEnv || NULL == pusAddr)
    {
        *pulRetVal = NULLPTRERR;
        return ERROR;
    }
    if (usMinVal > usMaxVal)
    {
        *pulRetVal = RANGEERR;
        return ERROR;
    }
    ulStrLen = cliLineRead(pstEnv, achLine,&usIsCtrlCLine);
    if ((ULONG)ERROR == ulStrLen)
    {
        *pulRetVal = TERMINALERR;
        return ERROR;
    }    
    if (TRUE == usIsCtrlCLine)
    {
        *pulRetVal = CTRLCHITED;
        return OK;
    }
    if (FALSE == cliIsDigits(achLine))
    {
        *pulRetVal = NOTALLDIGITAL;
        return ERROR;
    }    
    if (ulStrLen>0)
    {
        ulTemp = strtoul(achLine, NULL, 10);
        if (ulTemp < usMinVal)
        {
            *pulRetVal = TOOSMALL;
            return ERROR;
        }
        if (ulTemp > usMaxVal)
        {
            *pulRetVal = TOOLARGE;
            return ERROR;            
        }        
        *(unsigned short *)pusAddr = ulTemp;        
    }
    else
    {
        *pulRetVal = ENTERHITED;        
    }
    
    return OK;        
}

/*****************************************************************************
 *cliLongGet - Get a long integer(4Bytes) which is inputed from telnet client
                or console.
 *DESCRIPTION
 *This routine to get a long integer which is inputed from telnet client or 
 *console.
 *Input:pstEnv,pulAddr,ulMinVal,ulMaxVal,pulRetVal.
 *Output:pulAddr,pulRetVal.
 *Return:STATUS.
         OK    -- If successfully.
         ERROR -- If faild.
 ****************************************************************************/
STATUS cliLongGet(ENV_t *pstEnv, unsigned long *pulAddr, unsigned long ulMinVal,
                         unsigned long ulMaxVal, unsigned short *pulRetVal)
{
    char achLine[MAX_POOL_LENGTH];
    unsigned long ulStrLen;
    unsigned long ulTemp;
    unsigned short usIsCtrlCLine = FALSE;


    *pulRetVal = 0;
    if (NULL == pstEnv || NULL == pulAddr)
    {
        *pulRetVal = NULLPTRERR;
        return ERROR;
    }
    if (ulMinVal > ulMaxVal)
    {
        *pulRetVal = RANGEERR;
        return ERROR;
    }
    ulStrLen = cliLineRead(pstEnv, achLine,&usIsCtrlCLine);
    if ((ULONG)ERROR == ulStrLen)
    {
        *pulRetVal = TERMINALERR;
        return ERROR;
    }    
    if (TRUE == usIsCtrlCLine)
    {
        *pulRetVal = CTRLCHITED;
        return OK;
    }
    if (FALSE == cliIsDigits(achLine))
    {
        *pulRetVal = NOTALLDIGITAL;
        return ERROR;
    }    
    if (ulStrLen>0)
    {
        ulTemp = strtoul(achLine, NULL, 10);
        if (ulTemp < ulMinVal)
        {
            *pulRetVal = TOOSMALL;
            return ERROR;
        }
        if (ulTemp > ulMaxVal)
        {
            *pulRetVal = TOOLARGE;
            return ERROR;            
        }        
        *(unsigned long *)pulAddr = ulTemp;        
    }
    else
    {
        *pulRetVal = ENTERHITED;        
    }
    
    return OK;
}

/*****************************************************************************
 *cliIntegerGet - Get a integer which is inputed from telnet client.
 *DESCRIPTION
 *This routine to get a integer which is inputed from telnet client.
 *Input:pstEnv,pnAddr,nMinVal,nMaxVal,pulRetVal.
 *Output:pnAddr,pulRetVal.
 *Return:STATUS.
         OK    -- If successfully.
         ERROR -- If faild.
 ****************************************************************************/
STATUS cliIntegerGet(ENV_t *pstEnv, int *pnAddr, int nMinVal, int nMaxVal,
                     unsigned short *pulRetVal)
{
    int  nTemp;
    char achLine[MAX_POOL_LENGTH];    
    unsigned long  ulStrLen;    
    unsigned short usIsCtrlCLine = FALSE;


    *pulRetVal = 0;
    if (NULL == pstEnv || NULL == pnAddr)
    {
        *pulRetVal = NULLPTRERR;
        return ERROR;
    }
    if (nMinVal > nMaxVal)
    {
        *pulRetVal = RANGEERR;
        return ERROR;
    }
    ulStrLen = cliLineRead(pstEnv, achLine,&usIsCtrlCLine);
    if ((ULONG)ERROR == ulStrLen)
    {
        *pulRetVal = TERMINALERR;
        return ERROR;
    }    
    if (TRUE == usIsCtrlCLine)
    {
        *pulRetVal = CTRLCHITED;
        return OK;
    }
    if (FALSE == cliIsDigits(achLine))
    {
        *pulRetVal = NOTALLDIGITAL;
        return ERROR;
    }    
    if (ulStrLen>0)
    {
        nTemp = strtol(achLine, NULL, 10);
        if (nTemp < nMinVal)
        {
            *pulRetVal = TOOSMALL;
            return ERROR;
        }
        if (nTemp > nMaxVal)
        {
            *pulRetVal = TOOLARGE;
            return ERROR;            
        }        
        *(int *)pnAddr = nTemp;        
    }
    else
    {
        *pulRetVal = ENTERHITED;        
    }
    
    return OK;
}

/*****************************************************************************
 *cliStringGet - Get strings which is inputed from telnet client or console.
 *DESCRIPTION
 *This routine to get strings which is inputed from telnet client or console.
 *Input:pstEnv,pchAddr,ulMaxStrLen,pulRetVal.
 *Output:pchAddr,pulRetVal.
 *Return:STATUS.

 ****************************************************************************/
STATUS cliStringGet(ENV_t *pstEnv, char *pchAddr, unsigned long ulMaxStrLen,
                          unsigned short *pulRetVal)
{
    char achLine[MAX_POOL_LENGTH];
    unsigned long ulStrLen;
    unsigned long ulCopyLen;
    unsigned short usIsCtrlCLine = FALSE;


    *pulRetVal = 0;
    if (NULL == pstEnv || NULL == pchAddr)
    {
        *pulRetVal = NULLPTRERR;
        return ERROR;
    }
    ulStrLen = cliLineRead(pstEnv, achLine,&usIsCtrlCLine);
    if (ERROR == ulStrLen)
    {
        *pulRetVal = TERMINALERR;
        return ERROR;
    }
    
    if (TRUE == usIsCtrlCLine)
    {
        *pulRetVal = CTRLCHITED;
        return OK;
    }
        
    ulCopyLen = ulStrLen<ulMaxStrLen?ulStrLen:ulMaxStrLen;    
    memcpy(pchAddr, achLine, ulCopyLen);
    pchAddr[ulCopyLen] = '\0';    
    
    return ulCopyLen;
}

/*****************************************************************************
 *cliIsDigits - check if the string is composed all of digit.
 *DESCRIPTION
 *This routine mainly to check one string to see which is
 *all digits.
 *Input:pcDigits.
 *Output:N/A.
 *Return:BOOL.
         TRUE   ---If all digits.
         FALSE  ---If not all digits.
 ****************************************************************************/
BOOL cliIsDigits(char *pcDigits)
{
    int i = 0;
    
    
    if (*pcDigits != '\0')
    {     
        while ( pcDigits[i] )
        {                        
            if (pcDigits[i] < '0' || pcDigits[i] > '9') 
            {          
                i = 0;
                return FALSE;
            } 
            i++;
        }
    } 
    return TRUE;
}

/*********************************************************
 *cliIsInteger - check if the string is a integer string.
 *DESCRIPTION

 *Input:acInput.
 *Output:N/A.
 *Return:BOOL.
         TRUE   ---If it is a integer string.
         FALSE  ---If it is not a integer string.
 *********************************************************/
BOOL cliIsInteger(char acInput[])
{
    int i = 0;    
    
    if (acInput[0] != '\0')
    { 
        if ('-' == acInput[i] || '+' == acInput[i])
        {
            i++;
        }
        while ( acInput[i] )
        {                         
            if (acInput[i] < '0' || acInput[i] >'9') 
            {           
                i = 0;
                return FALSE;
            } 
            i++;
       }
    } 
    
    return TRUE;
}

/*********************************************************
 *cliIsValidIpAddress -  validate the legality of input ip address.
 *DESCRIPTION
 *This routine mainly to  validate the legality of input ip address .
 *Input:str_ip.
 *Output:N/A.
 *Return:BOOL.
         TRUE     ---ip address is right.
         FALSE  ---ip address is wrong.
 *********************************************************/
BOOL  cliIsValidIpAddress(char *pchStrIP)
{
    int i = 0;
    int j = 0;
    int n[4];
    int len = 0;
        
    if (NULL == pchStrIP) 
    {
        return FALSE;
    }
    
    len = strlen(pchStrIP);
    if (0 == len) 
    {
        return FALSE;
    }
    for (i = 0; i < len; i++)
    {
        if (pchStrIP[i] == '.' || (pchStrIP[i] >= '0' && pchStrIP[i] <= '9'))
        {
            continue;
        }
        else
        {
            break;
        }
    }
    if (i < len) 
    {
        return FALSE;
    }
    
    len = 0;/*len is to supervisor if number of dot is less than 3*/
    for (i = 0; i < 4; i++) 
    {
        n[i] = 0;
    }
    for (i = 0; i < 4; i++)
    {
        if (pchStrIP[0] == '.') 
         {
             return FALSE;
         }
        if (pchStrIP[j] == '.' && (pchStrIP[j+1]=='.' || pchStrIP[j+1]=='\0')) 
         {
             return FALSE;
         }
        if (pchStrIP[j] == '.') 
         {
             j++;
         }
        while (pchStrIP[j]!= '.' && pchStrIP[j] != '\0')
         {
            n[i] = n[i]*10+(pchStrIP[j]-'0');
             j++;
         }
         len++;
    }
    if (pchStrIP[j] != '\0') 
    {
        return FALSE;
    }
    if (len < 4) 
    {
        return FALSE;
    }
    for (i = 0; i < 4; i++)
    {
        if (n[i] > 255) 
        {
            return FALSE;   
        }
    }
    if (n[3] == 0 ||n[0] == 0)
    {
        return FALSE;
    }
    
    return TRUE;

}

/*********************************************************
 *cliIsValidNetMask -  validate the legality of input ip address.
 *DESCRIPTION
 *This routine mainly to  validate the legality of input ip address .
 *Input:str_ip.
 *Output:N/A.
 *Return:BOOL.
         TRUE     ---ip address is right.
         FALSE  ---ip address is wrong.
 *********************************************************/
BOOL  cliIsValidNetMask(char str_ip[])
{
    int len = 0;
    int i = 0;
    int j = 0;
    int n[4];
        
    if (NULL == str_ip) 
    {
        return FALSE;
    }
    
    len = strlen(str_ip);
    if (0 == len) 
    {
        return FALSE;
    }
    for (i = 0; i < len; i++)
    {
        if ('.' == str_ip[i] || (str_ip[i] >= '0' && str_ip[i] <= '9'))
        {
            continue;
        }
        else
        {
            break;
        }
    }
    if (i < len) 
    {
        return FALSE;
    }
    
    len = 0;/*len is to supervisor if number of dot is less than 3*/
    for (i = 0; i < 4; i++) 
    {
        n[i] = 0;
    }
    for (i = 0; i < 4; i++)
    {
         if ('.' == str_ip[0]) 
         {
             return FALSE;
         }
         if ('.' == str_ip[j] && ('.' == str_ip[j+1] || '\0' == str_ip[j+1])) 
         {
             return FALSE;
         }
         if ('.' == str_ip[j]) 
         {
             j++;
         }
         while (str_ip[j] != '.' && str_ip[j] != '\0')
         {
             n[i] = n[i]*10+(str_ip[j]-'0');
             j++;
         }
         len++;
    }
    if (str_ip[j] != '\0') 
    {
        return FALSE;
    }
    if (len < 4) 
    {
        return FALSE;
    }
    for (i = 0; i < 4; i++)
    {
        if (n[i] > 255) 
        {
            return FALSE;   
        }
    }
  /*  if (0 == n[3] || 0 == n[0])
    {
        return FALSE;
    }*/
    
    return TRUE;

}

/*********************************************************
 *cliIpToStr -  convert to dot format ip string.
 *DESCRIPTION
 *This routine mainly to  convert to dot format ip string.
 *NOTE: The buffer must big enough to contain the ip string.
 *Input:ip_addr.
 *Output:ip_string.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 *********************************************************/
STATUS cliIpToStr(ULONG ip_addr, char *ip_string)
{
    int i;
    unsigned char ip_buffer[4];
    unsigned char temp;
    BOOL flag;
    
    for (i = 0; i < 4; i++)
    {
        ip_buffer[3-i] = (unsigned char) ((ip_addr >> 8*i) & 0xFF);
    }
    
    temp = ip_buffer[0];
    flag = FALSE;
    
    if (temp >= 100) 
    { 
        *ip_string = temp/100+'0'; 
        ip_string++; 
        temp = temp%100; 
        flag = TRUE; 
    }
    if (flag == TRUE || temp >= 10) 
    {
        *ip_string = temp/10+'0';
        ip_string++; 
        temp = temp%10; 
    }
    *ip_string = temp+'0'; 
    ip_string++;
    
    *ip_string = '.';
    ip_string++;
    
    temp = ip_buffer[1];
    flag = FALSE;
    if (temp >= 100) 
    { 
        *ip_string = temp/100+'0';
        ip_string++;
        temp = temp%100; 
        flag = TRUE; 
    }
    if (flag == TRUE || temp >= 10) 
    { 
        *ip_string = temp/10+'0'; 
        ip_string++; 
        temp = temp%10; 
    }
    *ip_string = temp+'0'; 
    ip_string++;
    
    *ip_string = '.';
    ip_string++;
    
    temp = ip_buffer[2];
    flag = FALSE;
    if (temp >= 100) 
    { 
        *ip_string = temp/100+'0'; 
        ip_string++; 
        temp = temp%100; 
        flag = TRUE; 
    }
    if (flag == TRUE || temp >= 10) 
    { 
        *ip_string = temp/10+'0';
        ip_string++; 
        temp = temp%10; 
    }
    *ip_string = temp+'0'; 
    ip_string++;
    
    *ip_string = '.';
    ip_string++;
    
    temp = ip_buffer[3];
    flag = FALSE;
    if (temp >= 100) 
    { 
        *ip_string = temp/100+'0'; 
        ip_string++; 
        temp = temp%100; 
        flag = TRUE; 
    }
    if (flag == TRUE || temp >= 10) 
    { 
        *ip_string = temp/10+'0'; 
        ip_string++; 
        temp = temp%10; 
    }
    *ip_string = temp+'0'; 
    ip_string++;
    
    *ip_string = '\0';
    
    return(OK);
}

/*****************************************************************************
 *cliStrToIp -  convert string to interger ip.
 *DESCRIPTION
 *
 *Input:pchDotIPStr,pulIP.            
 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliStrToIp(char *pchDotIPStr,ULONG *pulIP)
{   
    int i = 0;
    int j = 0;
    int nTemp[4];
    int nLen = 0;

    
    if (NULL == pchDotIPStr || NULL == pulIP) 
    {
        return ERROR;
    }

    nLen = strlen(pchDotIPStr);
    if (0 == nLen) 
    {
        return ERROR;
    }
    for (i = 0; i < nLen; i++)
    {
        if ('.' == pchDotIPStr[i] || (pchDotIPStr[i] >= '0' 
            && pchDotIPStr[i] <= '9'))
        {
            continue;
        }
        else
        {
            break;
        }
    }
    if (i < nLen) 
    {
        return ERROR;
    }

    nLen = 0;/*nLen is to supervisor if number of dot is less than 3*/
    for ( i = 0; i < 4; i++) 
    {
        nTemp[i] = 0;
    }
    for ( i = 0; i < 4; i++)
    {
        if ('.' == pchDotIPStr[0]) 
        {
            return ERROR;
        }
        if ('.' == pchDotIPStr[j] && ('.' == pchDotIPStr[j+1] || '\0' == pchDotIPStr[j+1])) 
        {
            return ERROR;
        }
        if ('.' == pchDotIPStr[j]) 
        {
            j++;
        }
        while (pchDotIPStr[j]!= '.' && pchDotIPStr[j] != '\0')
        {
            nTemp[i] = nTemp[i]*10+(pchDotIPStr[j]-'0');
            j++;
        }
        nLen++;
    }
    if (pchDotIPStr[j] != '\0') 
    {
        return ERROR;
    }
    if (nLen < 4) 
    {
        return ERROR;
    }
    for (i = 0; i < 4; i++)
    {
        if (nTemp[i] > 255) 
        {
            return ERROR;
        }
    }
    *pulIP = nTemp[0]*256*256*256+nTemp[1]*256*256+nTemp[2]*256+nTemp[3];

    return OK;
}

int cliDigitToHexChar(int digit)
{
    digit &= 0xf;
    return (digit > 9) ? digit - 10 + 'a' : digit + '0';
}

/* begin modified by jiangmingli, 20080905 */
STATUS cliMacToStr(unsigned char *pMac, unsigned char *pMacStr)
{
    unsigned char chTemp;
    int i;
   /* if (strlen(pMac) < 6)
    {
		vosPrintf(pstEnv->nWriteFd, " Mac address syntax error!\r\n");
    	return ERROR;
    }*/

    for (i = 0; i < 6; i++)
    {
    	chTemp = pMac[i];
    	pMacStr[i*3] = cliDigitToHexChar(chTemp / 16);
    	pMacStr[i*3 + 1] = cliDigitToHexChar(chTemp % 16);
    	if (5 == i)
    		pMacStr[i*3 + 2] = '\0';
    	else
    		pMacStr[i*3 + 2] = ':';
    }

    return TRUE;
}
/* end modified by jiangmingli, 20080905 */

/*****************************************************************************
 *cliStrToDate -  Convert string to Date.
 *Input:sprDate,pnYear.
 *Output:N/A.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliStrToDate(char *sprDate, int *pnYear, int *pnMon, int *pnDay)
{
    int i = 0;
    int j;
    int nDateLen = 0;

    if (sprDate == NULL || pnYear == NULL || pnMon == NULL || pnDay == NULL) 
    {
        return ERROR;
    }
    
    nDateLen = strlen(sprDate);
    if (nDateLen == 0) 
    {
        return ERROR;
    }
    
    for (i = 0;i < nDateLen;i++)
    {
        if ((sprDate[i] == '-') || (sprDate[i] >= '0' && sprDate[i] <= '9'))
        {/*don't allow enter space*/
            continue;
        }
        else
        {
            break;
        }
    }
    
    if (i < nDateLen) 
    {
        return ERROR;
    }
    
    nDateLen = 0;/*nDateLen is to supervisor if number of dot is less than 3*/
    
    *pnYear = 0;
    *pnMon = 0;
    *pnDay = 0;
   
    for (i = 0,j=0;i < 3;i++)
    {
        if (sprDate[0] == '-' ) 
        {
            return ERROR;
        }
        
        if (sprDate[j] == '-' && (sprDate[j+1] == '-' || sprDate[j+1] == '\0')) 
        {
            return ERROR;
        }
        
        if (sprDate[j] == '-' ) 
        {
            j++;
        }
        
        while (sprDate[j] != '-' && sprDate[j] != '\0')
        {
            if (i==0)/*Year*/
            {
                *pnYear = *pnYear*10+(sprDate[j]-'0');
            }
            if (i==1)/*Month*/
            {
                *pnMon = *pnMon*10+(sprDate[j]-'0');
            }
            if (i==2)/*Day*/
            {
                *pnDay = *pnDay*10+(sprDate[j]-'0');
            }
            
            j++;
            
        }/*while*/
        nDateLen++;
     }
    
    if (sprDate[j] != '\0') 
    {
        return ERROR;
    }
    if (nDateLen < 3) 
    {
        return ERROR;
    }
    
    if (*pnYear < 2000 || *pnYear > 2099) 
    {
        return ERROR;       
    }
    if (*pnMon <1 || *pnMon >12)
    {
        return ERROR;
    }
    if (*pnDay <1 || *pnDay >31)
    {
        return ERROR;
    }
    
    return OK;
}

/*****************************************************************************
 *cliStrToTime -  Convert string to Time.
 *Input:sprDate,pnYear.
 *Output:N/A.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliStrToTime(char *sprTime, int *pnHour, int *pnMin, int *pnSec)
{
    int i = 0;
    int j;
    int nTimeLen = 0;

    
    if (sprTime == NULL || pnHour == NULL || pnMin == NULL || pnSec == NULL) 
    {
        return ERROR;
    }
    
    nTimeLen = strlen(sprTime);
    if (nTimeLen == 0) 
    {
        return ERROR;
    }
    
    for (i = 0;i < nTimeLen;i++)
    {
        if ((sprTime[i] == ':') || (sprTime[i] >= '0' && sprTime[i] <= '9'))
        {
            continue;
        }
        else
        {
            break;
        }
    }
    
    if (i < nTimeLen) 
    {
        return ERROR;
    }
    
    nTimeLen = 0;/*nDateLen is to supervisor if number of dot is less than 3*/
    
    *pnHour = 0;
    *pnMin = 0;
    *pnSec = 0;
   
    for (i = 0,j=0;i < 3;i++)
    {
        if (sprTime[0] == ':' ) 
        {
            return ERROR;
        }
        
        if (sprTime[j] == ':' && (sprTime[j+1] == ':' || sprTime[j+1] == '\0')) 
        {
            return ERROR;
        }
        
        if (sprTime[j] == ':' ) 
        {
            j++;
        }
        
        while (sprTime[j] != ':' && sprTime[j] != '\0')
        {
            if (i==0)/*Hour*/
            {
                *pnHour = *pnHour*10+(sprTime[j]-'0');
            }
            if (i==1)/*Minute*/
            {
                *pnMin = *pnMin*10+(sprTime[j]-'0');
            }
            if (i==2)/*Second*/
            {
                *pnSec = *pnSec*10+(sprTime[j]-'0');
            }
            
            j++;
            
        }/*while*/
        nTimeLen++;
     }
    
    if (sprTime[j] != '\0') 
    {
        return ERROR;
    }
    if (nTimeLen < 3) 
    {
        return ERROR;
    }
    
    if (*pnHour < 0 || *pnHour > 23) 
    {
        return ERROR;       
    }
    if (*pnMin <0 || *pnMin >59)
    {
        return ERROR;
    }
    if (*pnSec <0 || *pnSec >59)
    {
        return ERROR;
    }
   
    return OK;
}

/*****************************************************************************
 *cliStrLeftTrim - Delete the head space of the source string.
                     
 *DESCRIPTION
 *
 *Input:ppchString.            
 *Output:ppchString.           
 *Return:STATUS.
 ****************************************************************************/
STATUS cliStrLeftTrim(char **ppchString)
{
    if ((NULL == ppchString)||(NULL == *ppchString))
    {
        return ERROR;
    }
    
    while (' ' == **ppchString || '\t' == **ppchString) 
    {
        (*ppchString)++;
    }
    return OK;
}

/*****************************************************************************
 *cliStrRightTrim - Delete the tail space of the source string.
                     
 *DESCRIPTION
 *
 *Input:pchString.            
 *Output:ppchString.           
 *Return:N/A.
 ****************************************************************************/
STATUS cliStrRightTrim(char *pchString)
{
    char *s = pchString;

    if (!s) {
        return ERROR;
    }

    while (*s) s++;
    while (*--s == ' ' || *s == '\t') *s = 0;

    return OK;
}

STATUS cliStrToLower(char *pchInput)
{
    int i;
    
    if (strlen(pchInput)<=0)
    {
        return ERROR;
    }

    for (i=0; i<strlen(pchInput); i++)
    {
        if (pchInput[i]>='A' && pchInput[i]<='Z')
        {
            pchInput[i] = pchInput[i] -'A' +'a';
        }
    }
    return OK;
}

BOOL cliIsInputYes(char *pchInput)
{
    if (strlen(pchInput)<=0)
    {
        return FALSE;
    }

    cliStrLeftTrim(&pchInput);
    cliStrRightTrim(pchInput);
    cliStrToLower(pchInput);

    if (0 == strcmp(pchInput,"yes") || 0 == strcmp(pchInput,"ye") ||0 == strcmp(pchInput,"y"))
    {
        return TRUE;
    }
    
    return FALSE;
}

BOOL cliIsInputNo(char *pchInput)
{
    if (strlen(pchInput)<=0)
    {
        return FALSE;
    }

    cliStrLeftTrim(&pchInput);
    cliStrRightTrim(pchInput);
    cliStrToLower(pchInput);

    if (0 == strcmp(pchInput,"no") || 0 == strcmp(pchInput,"n"))
    {
        return TRUE;
    }
    
    return FALSE;
}

/* begin added by jiangmingli, 2008-08-26 */
/*******************************************************************************
* cliGetPortlist
*
* DESCRIPTION:
*		detrieve the portid list from the symbol array of Portlist from CLI
*
*	INPUTS:
*		pcPortlist		symbol array of Portlist from CLI
*		uiMaxPortNum	 	the max number of portlist
*		pucPortId		the Port List retrieved from pcPortlist
*		pusPortNum	the Port Number retrieved from pcPortlist
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR							success
* 		ERR_NULL_POINTER					NULL point
*		ERR_INVALID_PARAMETERS				parameter invalid		
* 	SEE ALSO: 
*/
STATUS cliGetPortlist(INT8 *pcPortlist, UINT32 uiMaxPortNum, UINT32 *puiPortId, UINT32 *puiPortNum)
{
	UINT16 usPortlistLen;
	UINT16 i;
	UINT16 j;
	BOOL bPrevWordNumber;
	UINT32 uiPortNum;
	BOOL bPrevLine;
	UINT32 uiSecondNum;
	
	if ((NULL == pcPortlist) || (NULL == puiPortNum) || (NULL == puiPortId)) 
    {
        return ERR_NULL_POINTER;
    }
    
    usPortlistLen = strlen(pcPortlist);
    if (0 == usPortlistLen) 
    {
        return ERR_INVALID_PARAMETERS;
    }

	/* parse portlist */
	uiPortNum = 0;
	puiPortId[0] = 0;
	bPrevWordNumber = FALSE;
	bPrevLine = FALSE;
	uiSecondNum = 0;
    for (i = 0; i < usPortlistLen; i++)
	{
		/* only symbol of '0'-'9' is valid for portid, ohter symbol is regarded as seperate symbol */
		switch (pcPortlist[i])
		{
			case ',':
			{
				if (FALSE == bPrevWordNumber)
				{
					return ERR_INVALID_PARAMETERS;
				}

				bPrevWordNumber = FALSE;
				break;
			}
			case '-':
			{
				if (FALSE == bPrevWordNumber)
				{
					return ERR_INVALID_PARAMETERS;
				}
				
				bPrevLine = TRUE;
				bPrevWordNumber = FALSE;
				uiSecondNum = 0;
				break;
			}
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
			{
				if (FALSE == bPrevLine)
				{
					if (FALSE == bPrevWordNumber)
					{
						uiPortNum++;
						
						if (uiPortNum > uiMaxPortNum)
						{
							return ERR_INVALID_PARAMETERS;
						}
						
						puiPortId[uiPortNum - 1] = pcPortlist[i] - '0';
						if (puiPortId[uiPortNum - 1] > uiMaxPortNum)
						{
							return ERR_INVALID_PARAMETERS;
						}
					}
					else
					{
						puiPortId[uiPortNum - 1] = 10 * puiPortId[uiPortNum - 1] + (pcPortlist[i] - '0');
						if (puiPortId[uiPortNum - 1] > uiMaxPortNum)
						{
							return ERR_INVALID_PARAMETERS;
						}
					}

					bPrevWordNumber = TRUE;
				}
				else
				{
					if ((i < (usPortlistLen - 1)) && ('-' == pcPortlist[i + 1]))
					{
						return ERR_INVALID_PARAMETERS;
					}
					
					if ((i == (usPortlistLen - 1)) || (',' == pcPortlist[i + 1]))
					{
						uiSecondNum = 10 * uiSecondNum + (pcPortlist[i] - '0');
						if (uiSecondNum > uiMaxPortNum)
						{
							return ERR_INVALID_PARAMETERS;
						}
						
						if (uiSecondNum < puiPortId[uiPortNum - 1])
						{
							return ERR_INVALID_PARAMETERS;
						}

						for (j = puiPortId[uiPortNum - 1] + 1; j <= uiSecondNum; j++)
						{
							uiPortNum++;
							puiPortId[uiPortNum - 1] = j;
						}

						bPrevLine = FALSE;
					}
					else
					{
						uiSecondNum = 10 * uiSecondNum + (pcPortlist[i] - '0');
						if (uiSecondNum > uiMaxPortNum)
						{
							return ERR_INVALID_PARAMETERS;
						}
						
						bPrevWordNumber = TRUE;
					}
				}
				
				break;
			}
			default:
			{
				return ERR_INVALID_PARAMETERS;
			}
		}
	}

	*puiPortNum = uiPortNum;

	return NO_ERROR;
}
/* end added by jiangmingli, 2008-08-26 */

/* Function Definitions */
 /*****************************************************************************
 *
 * cliIsLeapYear: Check whether the year is leap year or not.
 *
 * DESCRIPTION: 
 *
 *   This routine checks whether the year is leap year or not.
 *
 * Input:
 *   unYear - year number
 *
 * Output:
 *   N/A.
 *
 * Return:
 *   TRUE if current year is leap year; otherwise, return FALSE
 *
 ****************************************************************************/
int cliIsLeapYear(unsigned int unYear)
{
    int nRc = FALSE;
    
    if ((unYear % 100) == 0)
    {
        if ((unYear % 400) == 0)
        {
            nRc = TRUE;
        }
    }
    else
    {
        if ((unYear % 4) == 0)
        {
            nRc = TRUE;
        }
    }

    return nRc;
}


