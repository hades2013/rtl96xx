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
* FILENAME:  utils.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/utils.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __UTILS_H_
#define __UTILS_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <vos.h>

unsigned int utilSwap32(unsigned int i);
unsigned short utilSwap16(unsigned short i);

/* for bit reversion */
uint8 utilBitRev8(uint8 n);
uint16 utilBitRev16(uint16 n);
uint32 utilBitRev32(uint32 n);
uint32 utilBitRevByByteWord32(uint32 n);
/* for string operations */
const char *utilStrCaseIndex(const char *s, const char *sub);
char *utilStrReplace(char *s, int start, int len, const char *repstr);
/* operating conversion between digit with character */
int utilDigitToHexChar(int digit);
int utilHexCharToDigit(int digit);

/*  */
int utilIsInt(char *s);
uint32 utilParseInteger(char *str);
int utilParseRand(char *str, uint32 *val);


char *utilBaseName(char* path);
char *utilStrChomp(char *s);
char *utilStrTrim(char *s);
char *utilStrLeftTrim(char *s);
char *utilStrRightTrim(char *s);
void utilStrTrimQuotes(char *s);
char *utilStrReplaceChar(char *s, char old, char new);
int utilStrWordsCount(char *s, const char *word);
int utilStrStartWith(const char *a, const char *b);
uint32 utilStrSplit(const char *sep, const char *str, char ***result);

uint8 utilStrIsTimeZone(const char *timezone);
int utilStrToTimeZone(const char *timezone);
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __UTILS_H_ */
 
