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
* FILENAME:  utils.c
*
* DESCRIPTION: 
*	
*
* Date Created: Nov 03, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/utils.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <utils.h>
#include <vos.h>


/*******************************************************************************
*
* utilSwap32:	 swap the bytes in a 32-bit word.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	
*
* SEE ALSO: 
*/
unsigned int utilSwap32(unsigned int i)
{
    i = (i << 16) | (i >> 16);
    return (i & 0xff00ffff) >> 8 | (i & 0xffff00ff) << 8;
}

/*******************************************************************************
*
* utilSwap16:	 swap the bytes in a 16-bit half-word.
*
* DESCRIPTION:
* 	
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
unsigned short utilSwap16(unsigned short i)
{
    return i << 8 | i >> 8;
}

/*******************************************************************************
*
* utilBitRev8:	 Reverse the bits in an 8-bit.
*
* DESCRIPTION:
* 	
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
uint8 utilBitRev8(uint8 n)
{
    n = (((n & 0xaa) >> 1) | ((n & 0x55) << 1));
    n = (((n & 0xcc) >> 2) | ((n & 0x33) << 2));
    n = (((n & 0xf0) >> 4) | ((n & 0x0f) << 4));
    return n;
}

/*******************************************************************************
*
* utilBitRev16:	 Reverse the bits in an 16-bit.
*
* DESCRIPTION:
* 	
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
uint16 utilBitRev16(uint16 n)
{
    n = (((n & 0xaaaa) >> 1) | ((n & 0x5555) << 1));
    n = (((n & 0xcccc) >> 2) | ((n & 0x3333) << 2));
    n = (((n & 0xf0f0) >> 4) | ((n & 0x0f0f) << 4));
    n = (((n & 0xff00) >> 8) | ((n & 0x00ff) << 8));
    return n;
}

/*******************************************************************************
*
* utilBitRev32:	 Reverse the bits in an 32-bit.
*
* DESCRIPTION:
* 	
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
uint32 utilBitRev32(uint32 n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    n = (((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8));
    return (n >> 16) | (n << 16);
}

/*******************************************************************************
*
* utilBitRevByByteWord32:	Reverse the bits in each byte of 32bit long. 
*
* DESCRIPTION:
* 	
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
uint32 utilBitRevByByteWord32(uint32 n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    return n;
}

/* for some string operations */

/*******************************************************************************
*
* utilStrCaseIndex:	 to find the sub string index in the string.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	s - the string,the index is in this string.
*   sub - it is the substring, which should be found.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	the index from the string, which is the point to remains
*
* SEE ALSO: 
*/
const char *utilStrCaseIndex(const char *s, const char *sub)
{
    int len;
    for (len = vosStrLen(sub); *s; s++) {
        if (vosStrNCaseCmp(s, sub, len) == 0) {
            return s;
        }
    }
    return 0;
}

/*******************************************************************************
*
* utilStrReplace:	 to replace the string in anywhere
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	s - the string will be modified.
*   start - the start offset relation to s.
*   len - how many bytes will be replaced.
*   repstr - the replace string.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	the new string pointer.
*
* SEE ALSO: 
*/
char *utilStrReplace(char *s, int start, int len, const char *repstr)
{
    memmove(s + start + strlen(repstr),
        s + start + len,
        strlen(s + start + len) + 1);
    memmove(s + start, repstr, strlen(repstr));
    return s;
}


/*******************************************************************************
*
* utilDigitToHexChar:	 convert digit to hex character. 
*
* DESCRIPTION:
* 	
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
int utilDigitToHexChar(int digit)
{
    digit &= 0xf;
    return (digit > 9) ? digit - 10 + 'a' : digit + '0';
}

/*******************************************************************************
*
* utilHexCharToDigit:	 Convert hex character to digit
*
* DESCRIPTION:
* 	
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
int utilHexCharToDigit(int digit)
{
    if (digit >= '0' && digit <= '9') return (digit - '0'     );
    if (digit >= 'a' && digit <= 'f') return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F') return (digit - 'A' + 10);
    return 0;
}

/** description: to get the rand value.
 *  @param str - string may be rand(x),or rand(x,y)
 *  @param val - when str is rand(x),val are in (0,x-1),and if rand(x, y),val are in (x,y)
 *  @return if the string is of the form rand(x) or rand(x,y),return non-zero,else return zero.
 */
int utilParseRand(char *str, uint32 *val)
{
  uint32              min, max, rnd32;
  
  if (vosStrNCaseCmp(str, "rand(", 5) != 0) {
    return 0;
  }

  min = vosStrToInt(str + 5, &str);
  if (*str == ',') {
    max = vosStrToInt(str + 1, &str);
  } else {
    max = min - 1;
    min = 0;
  }
  
  if (*str != ')') {
    return 0;
  }
  
  rnd32 = (vosRandom() << 17 ^
	   vosRandom() << 13 ^
	   vosRandom() << 5 ^
	   vosRandom() >> 3);
  
  if (max - min + 1 == 0) {
    *val = 0;
  } else {
    *val = min + rnd32 % (max - min + 1);
  }
  return 1;
}

/* return true if a constant is a well-formed integer of the type supported by utilParseInteger. */
int utilIsInt(char *s)
{
  int base;
  uint32 dummy;
  
  if (utilParseRand(s, &dummy)) {
    return 1;
  }
  
  if (s == NULL) {
    return 0;
  }
  
  if (*s == '-') {
    s++;
  }
  
  if (*s == '0') {
    if (s[1] == 'b' || s[1] == 'B') {
      base = 2;
      s += 2;
    } else if (s[1] == 'x' || s[1] == 'X') {
      base = 16;
      s += 2;
    } else
      base = 8;
  } else {
    base = 10;
  }

  do {
    if (!isxdigit((unsigned) *s) || utilHexCharToDigit(*s) >= base) {
      return(0);
    }
  } while (*++s);
  
  return(1);
}
/** description: read a integer.
 *  @param str - the respresentation in string.
 *  @return return unsigned representation
 *  @Note: 
 *       [-]0x[0-9|A-F|a-f]+   -hexadecimal if the string begins with "0x"
 *       [-][0-9]+             -decimal integer
 *       [-]0[0-7]+            -octal integer
 *       [-]0b[0-1]+           -binary if the string begins with "0b"
 */
uint32 utilParseInteger(char *str)
{
  uint32  rnd;
  if (utilParseRand(str, &rnd)) {
    return rnd;
  }
  /*if (!utilIsInt(str)) {
    vosPrintk("WARNING: truncated malformed integer \"%s\"\r\n", str);
  }*/
  return vosStrToUInt(str, NULL);
}

/**
 * Return only the filename with leading directory components
 * removed. This function does not modify the path string.
 * @param path A file path string
 * @return A pointer to the basename in path
 */
char *utilBaseName(char* path) {
  
  char *fname;

  if (!path) {
    return NULL;
  }

  fname= strrchr(path, '/');
  
  return(fname ? ++fname : path);
  
}


/**
  * Removes everything from the first line break or newline (CR|LF)
  * @param s A string to be chomped
  * @return The chomped string
  */
char *utilStrChomp(char *s) {

  if (!s) {
    return NULL;
  }
  
  for (; *s; s++) {
    if (('\r' == *s) || ('\n' == *s)) {
      *s= 0; break;
    }
  }

  return s;
  
}


/**
 * Remove leading and trailing space from the string
 * @param s A string
 * @return s with leading and trailing spaces removed 
 */
char *utilStrTrim(char *s) {

  if (!s) {
    return NULL;
  }
  
  utilStrLeftTrim(s);
  utilStrRightTrim(s);

  return s;
  
}


/**
 * Remove leading white space [ \t\r\n] from the string.
 * @param s A string
 * @return s with leading spaces removed
 */
char *utilStrLeftTrim(char *s) {
  
  char *t= s;

  if (!s) {
    return NULL;
  }

  while(*t==' ' || *t=='\t' || *t=='\r' || *t=='\n') t++;
  if(t!=s) {
    char *r= s;
    do {
      *r++= *t;
    } while(*t++);
  }

  return s;

}


/**
 * Remove trailing white space [ \t\r\n] from the string
 * @param s A string
 * @return s with trailing spaces removed
 */
char *utilStrRightTrim(char *s) {

  char *t= s;

  if (!s) {
    return NULL;
  }

  while(*s) s++;
  while(*--s==' ' || *s=='\t' || *s=='\r' || *s=='\n') *s= 0;
  
  return t;

}

/**
 * Remove any enclosing quotes ["'] from the string
 * @param s A string
 */
void utilStrTrimQuotes(char *s) {

  char *t= s;
  char tmp=0;

  if (!s) {
    return;
  }

  if(*t==39 || *t==34 ) {

    tmp=*t;
    t++;

  } else {

    return;
    
  }

  while ( *t != tmp && *t != '\0' ) {
    *(t-1) = *t;
    t++;
  }

  *(t-1) = '\0';
  
  return;

}


/**
 * Replace all occurrences of the old char in the string
 * s with the new char.
 * @param s A string
 * @param old The old char
 * @param new The new char
 * @return s where all occurrence of old are replaced with new
 */
char *utilStrReplaceChar(char *s, char old, char new) {

  char *t= s;

  while (s&&*s) { if(*s==old) *s=new; s++; }

  return (t);

}

/**
 * Count the number the sub-string word occurs in s.
 * @param s The String to search for word in
 * @param word 	The sub-string to count in s
 */
int utilStrWordsCount(char *s, const char *word) {

  int i= 0;
  char *p= s;

  if (!s) {
    return i;
  }
  if (!word) {
    return i;
  }
  
  while((p= strstr(p, word))) { i++;  p++; }

  return i;

}


/**
 * Return TRUE if the string a starts with the string
 * b. The test is case-insensitive but depends on that
 * all characters in the two strings can be translated in the current
 * locale.
 * @param a The string to search for b in
 * @param b The sub-string to test a against
 * @return TRUE if a starts with b, otherwise FALSE
 */
int utilStrStartWith(const char *a, const char *b) {

  if((!a || !b) || toupper((int)*a)!=toupper((int)*b)) return FALSE;

  while(*a && *b) {
    
    if(toupper((int)*a++) != toupper((int)*b++)) return FALSE;
    
  }

  return TRUE;

}

/*
 * split the string into pieces, which must be freed by the caller.
 */
uint32 utilStrSplit(const char *sep, const char *str, char ***result) 
{
    char *p;
    char *src;
    char *buf;
    char *saveptr;
    uint32 c = 0;

    if (!str) return (c);
    
    src = strdup(str);
    buf = src;

    p = strtok_r(buf, sep, &saveptr);
    while (p) {
        c++;
        *result = vosReAlloc(*result, sizeof(char *) * c);
        (*result)[c - 1] = vosStrDup(p);
        p = strtok_r(NULL, sep, &saveptr);
    }

    free(src);
    
    return(c);
}

uint8 utilStrIsTimeZone(const char *timezone)
{
    enum { MAX_TIMEZONE_LEN = 6 };
    char str[MAX_TIMEZONE_LEN+1], *ptr;
    int off_hour=0, off_min=0;
    uint8 negative = FALSE;
    int max_off_hour = 13;

    strncpy(str, timezone, MAX_TIMEZONE_LEN);
    str[MAX_TIMEZONE_LEN] = '\0';

    ptr = str;

    if (str[0] == '-')
    {
        ptr++;
        negative = TRUE;
        max_off_hour = 12;
    }
    else if (str[0] == '+')
    {
        ptr++;
    }

    if (strchr(ptr, ':') == NULL)
    {
        return FALSE;
    }
    
    if (*ptr != 0)
    {
        sscanf(ptr, "%d:%d", &off_hour, &off_min);
        printf("timezone %d:%d\n", off_hour, off_min);
    }

    if (off_hour > max_off_hour || off_min > 60)
    {
        return FALSE;
    }

    return TRUE;
}

int utilStrToTimeZone(const char *timezone)
{
    enum { MAX_TIMEZONE_LEN = 6 };
    char str[MAX_TIMEZONE_LEN+1], *ptr;
    int off_hour=0, off_min=0;
    uint8 negative = FALSE;
    int max_off_hour = 13;

    strncpy(str, timezone, MAX_TIMEZONE_LEN);
    str[MAX_TIMEZONE_LEN] = '\0';

    ptr = str;

    if (str[0] == '-')
    {
        ptr++;
        negative = TRUE;
        max_off_hour = 12;
    }
    else if (str[0] == '+')
    {
        ptr++;
    }

    if (strchr(ptr, ':') == NULL)
    {
        return 0;
    }
    
    if (*ptr != 0)
    {
        sscanf(ptr, "%d:%d", &off_hour, &off_min);
        /* printf("timezone %d:%d\n", off_hour, off_min); */
    }

    if (off_hour > max_off_hour || off_min > 60)
    {
        return 0;
    }

    if (TRUE == negative)
    {
        return 0 - (off_hour * 3600 + off_min * 60);
    }
    else
    {
        return off_hour * 3600 + off_min * 60;
    }
}
/*caculate the string digit count*/
int utilStrDigitsCount(const char *src, const char *step)
{
   int  idx = 0;
   char *tmp = NULL;
   char *tmp1 = NULL;
   char *saveptr;
   
   //tmp = (char *)vosAlloc(strlen(src));
   tmp = (char *)malloc(strlen(src));
   if (tmp == NULL) return 0;
   tmp1 = tmp;
   strcpy(tmp,src);
   char *dig = strtok_r(tmp,step,&saveptr);
   while(dig){
           dig = strtok_r(NULL,step,&saveptr);
           idx++;
       }
   //vosFree(tmp1);
   free(tmp);
   return (idx);
}

/*get the string digit value*/
int utilStrDigitsGet(const char *src,char *step,int num)
{
    int  idx = 0;
    char *tmp = NULL;
    char *tmp1 = NULL;
    char *saveptr;

    //tmp = (char *)vosAlloc(strlen(src));
    tmp = (char *)malloc(strlen(src));
    if (tmp == NULL) return -1;
    tmp1 = tmp;
    strcpy(tmp,src);
    char *dig = strtok_r(tmp,step,&saveptr);
    while(dig){
        if (idx == num) return (strtoul(dig,NULL,0));
        dig = strtok_r(NULL,step,&saveptr);
        idx++;
    }
    //vosFree(tmp1);
    free(tmp);
    return (-1);
}

/*find the string digit value*/
int utilStrDigitsFind(const char *src,char *step,int digit)
{
    int  idx = 0;
    char *tmp = NULL;
    char *tmp1 = NULL;
    char *saveptr;
   
    //tmp = (char *)vosAlloc(strlen(src));
    tmp = (char *)malloc(strlen(src));
    if (tmp == NULL) return -1;
    tmp1 = tmp;
    strcpy(tmp,src);
    char *dig = strtok_r(tmp,step,&saveptr);
    while(dig){
        if (strtoul(dig,NULL,0) == (unsigned long)digit) return 0;
        dig = strtok_r(NULL,step,&saveptr);
        idx++;
    }
    //vosFree(tmp1);
    free(tmp);
    return (-1);
}

void utilStrDigitsDelete(char *portstr,const char *step,unsigned int digit)
{   
    
    int n = strlen(portstr);
    int m = utilStrDigitsCount(portstr,step);
    char *dig  = NULL;
    char *tmp1  = NULL;
    char *tmp2  = NULL;
    int idx    = 0;
    char *saveptr;

//    tmp1 = (char *)vosAlloc(strlen(portstr));
//    tmp2 = (char *)vosAlloc(strlen(portstr));
    tmp1 = (char *)malloc(strlen(portstr));
    if (tmp1 == NULL) 
    {
        return;
    }
    tmp2 = (char *)malloc(strlen(portstr));
    if (tmp2 == NULL) 
    {
        free(tmp1);
        return;
    }
    strcpy(tmp2,portstr);
    memset(tmp1,0,n);    
    dig = strtok_r(tmp2,step,&saveptr);
    while (dig)
    {
        if (strtoul(dig,NULL,0) != digit)
        {
            strcat(tmp1,dig);               
            idx++;
            if (idx >= m) goto exit;
            if (idx < (m -1))
            strcat(tmp1,step);
        }
        dig = strtok_r(NULL,step,&saveptr);
    }
    strcpy(portstr,tmp1);
exit:
    free(tmp1);
    free(tmp2);
//    vosFree(tmp1);
//    vosFree(tmp2);
    return ;
}
#define NULL_STRING(x)          ((NULL == x) || ('\0' == *x))
#define IS_SPACE(ch)            ((ch == ' ' ) ? 1 : 0)

static char utilHexToDec(char HexDigit)
{
    if (('0' <= HexDigit) && (HexDigit <= '9'))
        return (HexDigit - '0');

    if (('a' <= HexDigit) && (HexDigit <= 'f'))
        return (HexDigit - 'a' + 10);

    if (('A' <= HexDigit) && (HexDigit <= 'F'))
        return (HexDigit - 'A' + 10);

    /* illegal digit */
    return -1;
}

int utilStrToMacAddr(char *pString, void *pMacAddr)
{
    short   hiDigit;
    short   loDigit;
    short   tempDigit;
    int     index;
    char    MacAddr[6];

    if (NULL_STRING(pString))
        return -1;

	if( 17 != vosStrLen( pString ))
		return -1;

    for (index = 0; index < 6; index++)
    {
        hiDigit = utilHexToDec(*(pString++));

        if (('\0' != *pString) && (':' != *pString))
            loDigit = utilHexToDec(*(pString++));
        else
        {
            loDigit = hiDigit;
            hiDigit = 0;
        }
        
        if ((0 > hiDigit) || (0 > loDigit))
            return -1;

        tempDigit = (hiDigit << 4) + loDigit;
        if ((0 > tempDigit) || (tempDigit > 255))
            return -1;

        if ((index < 5) && (':' != *pString))
            return -1;

        pString++;

        MacAddr[index] = (char) tempDigit;
    }

/* endian conversion may be necessary here */
    memcpy(pMacAddr, MacAddr, 6);
    return 0;
}   /* utilStrToMacAddr */


unsigned int utilStrToIpAddr(char *inetString    /* string inet address */ )
{
#define MAX_PARTS 4 	/* Maximum number of parts in an IP address. */

    unsigned int val, base, n;
    char   c;
    unsigned int parts[MAX_PARTS], *pp = parts;

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

    if (*inetString && !IS_SPACE (*inetString)) 
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

int utilParseStrToPortList(
    const char *list,
    const char *single,
    const char *mulit,
    UINT8 maxportnum,
    UINT32 *diglist,
    UINT32 *dignum)
{
    return 0;
}

int utilStrComprise(const char *src,const char *comp)
{
    if ((NULL == src)||(NULL == comp))
        return -1;

    for (; *comp != '\0'; src++, comp++)
    {
        if ((*src == '\0')||(*src != *comp))
        return -1;
    }

    return 0;
}

