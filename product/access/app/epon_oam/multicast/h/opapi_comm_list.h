/*************************************************************************
*
* Copyright 2003~2030 OPL, All Rights Reserved
*
* @file opapi_comm_list.h
*
* DESCRIPTION: Defines the API to the 'example'
*
* Date Created: 3/20/2007
*
* Authors(optional):
*
* Reviewed by (optional):
*
**************************************************************************/

#ifndef OPAPI_COMM_LIST_H
#define OPAPI_COMM_LIST_H

/***** INCLUDE FILES *****/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/***** DEFINES and ENUMS *****/
typedef struct _clList clList;

struct _clList {
  void *data;
  clList *next;
  clList *prev;
};

/***** STRUCTURES and UNIONS *****/

/***** TYPEDEFS *****/

/***** EXTERNS *****/
/* Doubly linked lists
 */
extern clList *clListLast(clList *list);
extern clList *clListAppend(clList *list, void *data);
extern clList *clListRemove(clList *list, const void *data);
extern clList *clListNth(clList *list, int n);
extern void clListFree(clList *list);
extern int clListPosition(clList *list, clList *link);
extern void clListPrint(clList *list);



/***** PROTOTYPES *****/

#endif /* OPAPI_COMM_LIST_H */

/* END FILE */

