
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
* FILENAME:  macros.h
*
* DESCRIPTION: 
*	Header file containing standard macros.
*
* Date Created: Apr 21, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_macros.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __MACROS_H_
#define __MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif
 
#include <stdio.h>

/******************************************************************************
 * PUBLIC MACRO
 *
 *    ASSERT()
 *
 * DESCRIPTION
 *
 *    Print a message if boolean condition is true.
 *
 * RETURNS
 *
 *    None
 *
 * ARGUMENTS
 *
 *    e   Boolean expression used to determine if message is
 *        displayed or not.
 *    s   String to display as part of ASSERT message
 *
 * SIDE EFFECTS
 *
 *    None
 *
 * IMPLEMENTATION NOTES
 *
 *     The message printed out is the file name, function name,
 *     line number, and user supplied string.
 *
 *     The macro is defined to NULL if DEBUG not defined
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 ****/
#ifdef DEBUG

   #define ASSERT(e,s) ((!(e)) ?  \
           printf("**ASSERT!: FILE: %s FUNC: %s  Ln: %4d  <"#e">.\r\n%s\r\n", \
                  __FILE__, __FUNCTION__, __LINE__,(s)): (void) 0)

#else

   #define ASSERT(e,s)

#endif

/******************************************************************************
 * PUBLIC MACRO
 *
 *     WARNMSG()
 *
 * DESCRIPTION
 *
 *     Print a warning message.
 *
 * RETURNS
 *
 *     None
 *
 * ARGUMENTS
 *
 *     s   String to display as part of WARNMSG() message
 *
 * SIDE EFFECTS
 *
 *     None
 *
 * IMPLEMENTATION NOTES
 *
 *     The message printed is the filename, function name,
 *     line number, and user supplied string.
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *      None
 *
 ****/
#define WARNMSG(s) printf("****WARN!: FILE: %s FUNC: %s  Ln: %4d  <%s>.\r\n",  __FILE__, __FUNCTION__, __LINE__, s)

/******************************************************************************
 * PUBLIC MACRO
 *
 *    DIM()
 *
 * DESCRIPTION
 *
 *    Calculate the dimensions of an array
 *
 * RETURNS
 *
 *    Number of elements in (dimension of) array
 *
 * ARGUMENTS
 *
 *    Array - Array to calculate the dimensions of
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *    This macro does not function on pointers to arrays.
 *    The calculation is done at compile time.
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#define DIM(Array) ((sizeof((Array)) / sizeof((Array)[0])))

/******************************************************************************
 * PUBLIC MACRO
 *
 *     LASTINDEX()
 *
 * DESCRIPTION
 *
 *     Provides index number of last element of an array
 *
 * RETURNS
 *
 *     Index of last element in the array
 *
 * ARGUMENTS
 *
 *     array - array to calculate the last element index of
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *      This macro does not function on pointers to arrays.  The
 *       calculation is done at compile time.
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#ifndef LASTINDEX
   #define LASTINDEX(array) (sizeof((array))/sizeof((array[0]))-1)
#endif

/******************************************************************************
 * PRIVATE MACRO
 *
 *     NULLCHAR
 *
 * DESCRIPTION
 *
 *     sets NULLCHAR to NULL character  if not already defined
 *
 ****/
#ifndef NULLCHAR
   #define NULLCHAR '\0'
#endif

/******************************************************************************
 * PRIVATE MACRO
 *
 *     SAME
 *
 * DESCRIPTION
 *
 *     #define used for string compare return values
 *
 *     Used for strcmp etc.  For example:
 *
 *               if(strcmp(str1, str2) == SAME)
 *
 *                       or
 *
 *               if (strcmp(str1, str2) != SAME)
 *
 ****/
#define SAME 0

/******************************************************************************
 * PUBLIC MACRO
 *
 *     LASTCHAR()
 *
 * DESCRIPTION
 *
 *     Returns index value of last character in a string
 *
 * RETURNS
 *
 *     Index value of last character in the string
 *
 * ARGUMENTS
 *
 *     s   - pointer to string to search for last character
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#ifndef LASTCHAR
   #define LASTCHAR(s) ((s)[strlen((s))-1])
#endif

/******************************************************************************
 * PUBLIC MACRO
 *
 *      ISMTSTRING()
 *
 * DESCRIPTION
 *
 *      Determines if a string is empty
 *
 * RETURNS
 *
 *      TRUE if string is empty, FALSE if not
 *
 * ARGUMENTS
 *
 *      s   - pointer to possible empty string
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#ifndef ISMTSTRING
   #define ISMTSTRING(s) (s)[0] == NULLCHAR ? TRUE: FALSE
#endif

/******************************************************************************
 * PUBLIC MACRO
 *
 *     MAX() & MIN()
 *
 * DESCRIPTION
 *
 *     Find the maximum and minimum of two values
 *
 * RETURNS
 *
 *     The maximum (MAX()) or minimum (MIN()) of 'a' and 'b'
 *
 * ARGUMENTS
 *
 *     a  - first value
 *     b  - second value
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#ifndef MAX
   #define MAX(a,b)    (((a) > (b)) ? (a) : (b))
   #define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

/******************************************************************************
 * PUBLIC MACRO
 *
 *     LWRLIM()
 *
 * DESCRIPTION
 *
 *     Determine if a value is above or equal a lower limit
 *
 * RETURNS
 *
 *     TRUE if 'ival' >= 'imin" else FALSE
 *
 * ARGUMENTS
 *
 *     ival - value to be evaluated
 *     imin - lower limit to compare against
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#define LWRLIM(ival, imin)        ((ival) >= (imin)) ? TRUE : FALSE

/******************************************************************************
 * PUBLIC MACRO
 *
 *     UPRLIM()
 *
 * DESCRIPTION
 *
 *     Determine if a value is below or equal an upper limit
 *
 * RETURNS
 *
 *     TRUE if 'ival' <= 'imin" else FALSE
 *
 * ARGUMENTS
 *
 *     ival - value to be evaluated
 *     imax - upper limit to compare against
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#define UPRLIM(ival, imax)        ((ival) <= (imax)) ? TRUE : FALSE

/******************************************************************************
 * PUBLIC MACRO
 *
 *     INRANGE()
 *
 * DESCRIPTION
 *
 *     Determine if a value is within a given range
 *
 * RETURNS
 *
 *     TRUE if 'imin' <= 'ival' <= 'imax" else FALSE
 *
 * ARGUMENTS
 *
 *     ival - value to be evaluated
 *     imin - lower limit of range
 *     imax - upper limit of range
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#define INRANGE(ival, imin, imax) LWRLIM(ival, imin) && UPRLIM(ival, imax)

/******************************************************************************
 * PUBLIC MACRO
 *
 *     CONSTRAIN()
 *
 * DESCRIPTION
 *
 *     constrain a value to a given range
 *
 * RETURNS
 *
 *      'top' if 'val' > 'top',
 *      'bot' if 'val' < 'bot',  OR
 *      'val' if  'top' >= 'val' >= 'bot'
 *
 * ARGUMENTS
 *
 *     val - value to be constrained
 *     bot - lower limit of range
 *     top - upper limit of range
 *
 * SIDE EFFECTS
 *
 *
 *
 * IMPLEMENTATION NOTES
 *
 *
 *
 * SPECIAL REQUIREMENTS & NOTES
 *
 *
 *
 ****/
#define CONSTRAIN(val, bot, top) {val = MAX(val, bot); val = MIN(val, top);}
 
#ifdef __cplusplus
}
#endif

#endif /* __MACROS_H */

