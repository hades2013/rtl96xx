/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_match.c
*
* DESCRIPTION: 
*   
*
* Date Created: Jul 22, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_match.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos_match.h>
#include <vos_types.h>
#include <lw_type.h>

#define ASTERISK '*'        /* The '*' metacharacter */
#define QUESTION '?'        /* The '?' metacharacter */
#define LEFT_BRACKET '['    /* The '[' metacharacter */
#define RIGHT_BRACKET ']'   /* The ']' metacharacter */

#define IS_OCTAL(ch) (ch >= '0' && ch <= '7')

#define EOS '\000'

static int doList (char *string, char *pattern);
static char nextChar (char **patp);
static void listParse (char **patp, char *lowp, char *highp);


/*
 *  FUNCTION
 *
 *  vosMatch   test string for wildcard match
 *
 *  SYNOPSIS
 *
 *  int vosMatch(char *string, char *pattern)
 *
 *  DESCRIPTION
 *
 *  Test string for match using pattern.  The pattern may
 *  contain the normal shell metacharacters for pattern
 *  matching.  The '*' character matches any string,
 *  including the null string.  The '?' character matches
 *  any single character.  A list of characters enclosed
 *  in '[' and ']' matches any character in the list.
 *  If the first character following the beginning '['
 *  is a '!' then any character not in the list is matched.
 *
 */


/*
 *  PSEUDO CODE
 *
 *  Begin vosMatch
 *      Switch on type of pattern character
 *      Case ASTERISK:
 *          Attempt to match asterisk
 *          Break
 *      Case QUESTION MARK:
 *          Attempt to match question mark
 *          Break
 *      Case EOS:
 *          Match is result of EOS on string test
 *          Break
 *      Case default:
 *          If explicit match then
 *          Match is result of submatch
 *          Else
 *          Match is FALSE
 *          End if
 *          Break
 *      End switch
 *      Return result of match test
 *  End vosMatch
 *
 */

int vosMatch(char *string, char *pattern)
{
    int ismatch;

    ismatch = FALSE;
    switch (*pattern) {
    case ASTERISK:
        pattern++;
        do {
            ismatch = vosMatch (string, pattern);
        } while (!ismatch && *string++ != EOS);
        break;
    case QUESTION:
        if (*string != EOS) {
            ismatch = vosMatch (++string, ++pattern);
        }
        break;
    case EOS:
        if (*string == EOS) {
            ismatch = TRUE;
        }
        break;
    case LEFT_BRACKET:
        if (*string != EOS) {
            ismatch = doList (string, pattern);
        }
        break;
    default:
        if (*string++ == *pattern++) {
            ismatch = vosMatch (string, pattern);
        } else {
            ismatch = FALSE;
        }
        break;
    }
    return (ismatch);
}


/*
 *  FUNCTION
 *
 *  doList    process a list and following substring
 *
 *  SYNOPSIS
 *
 *  static int doList (string, pattern)
 *  char *string;
 *  char *pattern;
 *
 *  DESCRIPTION
 *
 *  Called when a list is found in the pattern.  Returns
 *  TRUE if the current character matches the list and
 *  the remaining substring matches the remaining pattern.
 *
 *  Returns FALSE if either the current character fails to
 *  match the list or the list matches but the remaining
 *  substring and subpattern's don't.
 *
 *  RESTRICTIONS
 *
 *  The mechanism used to match characters in an inclusive
 *  pair (I.E. [a-d]) may not be portable to machines
 *  in which the native character set is not ASCII.
 *
 *  The rules implemented here are:
 *
 *      (1) The backslash character may be
 *          used to quote any special character.
 *          I.E.  "\]" and "\-" anywhere in list,
 *          or "\!" at start of list.
 *
 *      (2) The sequence \nnn becomes the character
 *          given by nnn (in octal).
 *
 *      (3) Any non-escaped ']' marks the end of list.
 *
 *      (4) A list beginning with the special character
 *          '!' matches any character NOT in list.
 *          The '!' character is only special if it
 *          is the first character in the list.
 *
 */


/*
 *  PSEUDO CODE
 *
 *  Begin doList
 *      Default result is no match
 *      Skip over the opening left bracket
 *      If the next pattern character is a '!' then
 *      List match gives FALSE
 *      Skip over the '!' character
 *      Else
 *      List match gives TRUE
 *      End if
 *      While not at closing bracket or EOS
 *      Get lower and upper bounds
 *      If character in bounds then
 *          Result is same as sense flag.
 *          Skip over rest of list
 *      End if
 *      End while
 *      If match found then
 *      If not at end of pattern then
 *          Call match with rest of pattern
 *      End if
 *      End if
 *      Return match result
 *  End doList
 *
 */

static int doList (char *string, char *pattern)
{
    int ismatch;
    int if_found;
    int if_not_found;
    auto char lower;
    auto char upper;

    pattern++;
    if (*pattern == '!') {
        if_found = FALSE;
        if_not_found = TRUE;
        pattern++;
    } else {
        if_found = TRUE;
        if_not_found = FALSE;
    }
    ismatch = if_not_found;
    while (*pattern != ']' && *pattern != EOS) {
        listParse (&pattern, &lower, &upper);
        if (*string >= lower && *string <= upper) {
            ismatch = if_found;
            while (*pattern != ']' && *pattern != EOS) {pattern++;}
        }
    }
    if (*pattern++ != ']') {
        fprintf (stderr, "warning - character class error\n");
    } else {
        if (ismatch) {
            ismatch = vosMatch (++string, pattern);
        }
    }
    return (ismatch);
}


/*
 *  FUNCTION
 *
 *  listParse    parse part of list into lower and upper bounds
 *
 *  SYNOPSIS
 *
 *  static void listParse (char **patp, char *lowp, char *highp)
 *
 *  DESCRIPTION
 *
 *  Given pointer to a pattern pointer (patp), pointer to
 *  a place to store lower bound (lowp), and pointer to a
 *  place to store upper bound (highp), parses part of
 *  the list, updating the pattern pointer in the process.
 *
 *  For list characters which are not part of a range,
 *  the lower and upper bounds are set to that character.
 *
 */

static void listParse (char **patp, char *lowp, char *highp)
{
    *lowp = nextChar (patp);
    if (**patp == '-') {
        (*patp)++;
        *highp = nextChar (patp);
    } else {
        *highp = *lowp;
    }
}


/*
 *  FUNCTION
 *
 *  nextChar    determine next character in a pattern
 *
 *  SYNOPSIS
 *
 *  static char nextChar (char **patp)
 *
 *  DESCRIPTION
 *
 *  Given pointer to a pointer to a pattern, uses the pattern
 *  pointer to determine the next character in the pattern,
 *  subject to translation of backslash-char and backslash-octal
 *  sequences.
 *
 *  The character pointer is updated to point at the next pattern
 *  character to be processed.
 *
 */

static char nextChar (char **patp)
{
    char ch;
    char chsum;
    int count;

    ch = *(*patp)++;
    if (ch == '\\') {
        ch = *(*patp)++;
        if (IS_OCTAL (ch)) {
            chsum = 0;
            for (count = 0; count < 3 && IS_OCTAL (ch); count++) {
                chsum *= 8;
                chsum += ch - '0';
                ch = *(*patp)++;
            }
            (*patp)--;
            ch = chsum;
        }
    }
    return (ch);
}

