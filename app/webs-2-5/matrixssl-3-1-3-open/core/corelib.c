/*
 *	corelib.c
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 *
 *	Open and Close APIs and utilities for PeerSec core library
 */
/*
 *	Copyright (c) PeerSec Networks, 2002-2010. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#include "coreApi.h"

static int32	psCoreIsOpen = PS_FALSE;

/******************************************************************************/
int32 psCoreOpen(void)
{
	if (psCoreIsOpen) {
		return PS_CORE_IS_OPEN;
	}	
		

	if (osdepTimeOpen() < 0) {
		psTraceCore("osdepTimeOpen failed\n");
		return PS_FAILURE;
	}
	if (osdepEntropyOpen() < 0) {
		psTraceCore("osdepEntropyOpen failed\n");
		osdepTimeClose();
		return PS_FAILURE;
	}
	
#ifdef USE_MULTITHREADING
	if (osdepMutexOpen() < 0) {
		psTraceCore("osdepMutexOpen failed\n");
		osdepEntropyClose();
		osdepTimeClose();
		return PS_FAILURE;
	}
#endif /* USE_MULTITHREADING */


	psCoreIsOpen = PS_TRUE;
	return PS_SUCCESS;
}

/******************************************************************************/
int32 psCoreClose(void)
{
	int32	err = 0;
	
	if (psCoreIsOpen == PS_FALSE) {
		return PS_SUCCESS;
	}
	psCoreIsOpen = PS_FALSE;
	

#ifdef USE_MULITHREADING
	if (osdepMutexClose() < 0) {
		err = 1;
	}
#endif /* USE_MULITHREADING */

	if (osdepEntropyClose() < 0) {
		err = 1;
	}

	if (osdepTimeClose() < 0) {
		err = 1;
	}
	
	if (err) {
		return PS_FAILURE;
	}
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	ERROR FUNCTIONS
	Tap into platform trace and break execution if DEBUG compile
	
	Modules should tie themselves to these low levels
	with compile-time defines
*/
void _psError(char *msg)
{
	_psTrace(msg);
#ifdef HALT_ON_PS_ERROR	
	osdepBreak();
#endif	
} 
void _psErrorInt(char *msg, int32 val)
{
	_psTraceInt(msg, val);
#ifdef HALT_ON_PS_ERROR		
	osdepBreak();
#endif
} 
void _psErrorStr(char *msg, char *val)
{
	_psTraceStr(msg, val);
#ifdef HALT_ON_PS_ERROR		
	osdepBreak();
#endif	
} 

/******************************************************************************/
/*
	Creates a simple linked list from a given stream and separator char
	
	Memory info:
	Callers do not have to free 'items' on function failure.
*/
int32 psParseList(psPool_t *pool, char *list, const char separator,
		psList_t **items)
{
	psList_t	*litems, *start, *prev;
	uint32		itemLen, listLen;
	char		*tmp;

	*items = NULL;
	
	listLen = (int32)strlen(list) + 1;
	if (listLen == 1) {
		return PS_ARG_FAIL;
	}
	start = litems = psMalloc(pool, sizeof(psList_t));
	if (litems == NULL) {
		return PS_MEM_FAIL;
	}
	memset(litems, 0, sizeof(psList_t));

	while (listLen > 0) {
		itemLen = 0;
		tmp = list;
		if (litems == NULL) {
			litems = psMalloc(pool, sizeof(psList_t));
			if (litems == NULL) {
				psFreeList(start);
				return PS_MEM_FAIL;
			}
			memset(litems, 0, sizeof(psList_t));
			prev->next = litems;
			
		}
		while (*list != separator && *list != '\0') {
			itemLen++;
			listLen--;
			list++;
		}
		litems->item = psMalloc(pool, itemLen + 1);
		if (litems->item == NULL) {
			psFreeList(start);
			return PS_MEM_FAIL;
		}
		litems->len = itemLen;
		memset(litems->item, 0x0, itemLen + 1);
		memcpy(litems->item, tmp, itemLen);
		list++;
		listLen--;
		prev = litems;
		litems = litems->next;
	}
	*items = start;
	return PS_SUCCESS;
}

void psFreeList(psList_t *list)
{
	psList_t	*next, *current;

	if (list == NULL) {
		return;
	}
	current = list;
	while (current) {
		next = current->next;
		if (current->item) {
			psFree(current->item);
		}
		psFree(current);
		current = next;
	}	
}

/******************************************************************************/
/*
	Clear the stack deeper than the caller to erase any potential secrets
	or keys.
*/
void psBurnStack(uint32 len)
{
	unsigned char buf[32];
	
	memset(buf, 0x0, sizeof(buf));
	if (len > (uint32)sizeof(buf)) {
		psBurnStack(len - sizeof(buf));
	}
}

/******************************************************************************/

