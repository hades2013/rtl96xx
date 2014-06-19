/*************************************************************************
*
* Copyright 2003~2030 OPL, All Rights Reserved
*
* @file opapi_comm_list.c
*
* DESCRIPTION:
*
* Date Created:
*
* Authors(optional):
*
* Reviewed by (optional):
*
* Edit History:
*
*************************************************************************/

/***** INCLUDE FILES *****/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vos_alloc.h>

#include "opapi_comm_list.h"
#include "opapi_comm_tree.h"

/***** LOCAL DEFINES and ENUM *****/

/***** LOCAL STRUCTURES and UNIONS *****/

/***** LOCAL TYPEDEFS ****/

/***** LOCAL (PRIVATE) PROTOTYPES *****/

/***** DATA ALLOCATION *****/

/***** PUBLIC FUNCTIONS ****/

/**
* @par Description
* This function gets the last node.
* @param list - head pointer
*
* @return list - the last node, else return NULL
* @see
*/
clList * clListLast(clList *list)
{
  if (list) {
    while (list->next) {
      list = list->next;
    }
  }

  return list;
}

/**
* @par Description
* This function adds a node to the head of the list.
* @param list - head pointer
* @param data - data pointer
*
* @return list - the new head pointer
* @see
*/
clList * clListInsert(clList *list, void *data)
{
  clList *new_list;

  new_list = (clList *) vosAlloc(sizeof(clList));
  memset(new_list, 0, sizeof(clList));
  new_list->data = data;

  if (list) {
    new_list->next = list;
    list->prev = new_list;
    return new_list;
  }
  else {
    return new_list;
  }
}

/**
* @par Description
* This function adds a node to the tail of the list.
* @param list - head pointer
* @param data - data pointer
*
* @return list - the new head pointer
* @see
*/
clList * clListAppend(clList *list, void *data)
{
  clList *new_list;
  clList *last;

  new_list = (clList *) vosAlloc(sizeof(clList));
  memset(new_list, 0, sizeof(clList));
  new_list->data = data;

  if (list) {
    last = clListLast(list);
    last->next = new_list;
    new_list->prev = last;
    return list;
  }
  else {
    return new_list;
  }
}

/**
* @par Description
* This function removes a node from list.
* @param list - head pointer
* @param data - data pointer
*
* @return list - the new head pointer
* @see
*/
clList * clListRemove(clList *list, const void *data)
{
  clList *tmp;

  tmp = list;
  while (tmp) {
    if (tmp->data != data) {
      tmp = tmp->next;
    }
    else {
      if (tmp->prev) {
        tmp->prev->next = tmp->next;
      }
      if (tmp->next) {
        tmp->next->prev = tmp->prev;
      }

      if (list == tmp) {
        list = list->next;
      }

      vosFree(tmp);

      break;
    }
  }

  return list;
}

/**
* @par Description
* This function gets the nth node.
* @param list - head pointer
* @param n - the position of the node, from 0 to max-1
*
* @return list - the nth node or NULL
* @see
*/
clList * clListNth(clList *list, int n)
{
  while ((n-- > 0) && list) {
    list = list->next;
  }

  return list;
}

/**
* @par Description
* This function gets the data field of the nth node.
* @param list - head pointer
* @param n - the position of the node, from 0 to max-1
*
* @return data - the data field
* @see
*/
void * clListNthData(clList *list, int n)
{
  clList *tmp;

  if (list == NULL) {
    return NULL;
  }

  tmp = clListNth(list, n);

  return tmp->data;
}

/**
* @par Description
* This function frees the whole list.
* @param list - head pointer
*
* @return
* @see
*/
void clListFree(clList *list)
{
  clList *last;

  while (list) {
    last = list;
    list = list->next;
    vosFree(last);
  }

  return;
}

/**
* @par Description
* This function gets the position of the appointed node.
* @param list - head pointer
* @param link - the appointed node
*
* @return i - the position of the node, from 0 to max-1
* @see
*/
int clListPosition(clList *list, clList *link)
{
  int i;

  i = 0;
  while (list) {
    if (list == link) {
      return i;
    }
    i++;
    list = list->next;
  }

  return -1;
}

int clListLen(clList *list)
{
  clList *tmp;
  int i;

  i = 0;
  tmp = list;
  while (tmp) {
    i++;
    tmp = tmp->next;
  }

  return i;
}

/**
* @par Description
* This function sorts the list, the big node is on the bottom, the little one is on the top.
* @param list - head pointer
*
* @return
* @see
*/
void clListBubbleSort(clList *list, clCompareFunc key_compare)
{
  clList *node;
  int i, j, len;
  int index;
  void *data;

  if (list == NULL || key_compare == NULL) {
    return;
  }

  node = NULL;
  len = 0;
  data = NULL;

  len = clListLen(list);

  if (len < 2) {
    return;
  }

  node = list;
  for (i = len; i > 0; i--) {
    for (j = 0; j < i - 1; j++) {
      if (key_compare(node->data, node->next->data) > 0) {
        data = node->next->data;
        node->next->data = node->data;
        node->data = data;
      }
      node = node->next;
    }
    node = list;
  }

  return;
}

void clListPrint(clList *list)
{
  clList *tmp;
  int i;

  i = 0;
  tmp = list;
  while (tmp) {
    printf("node %d: node=%08x, next=%08x, prev=%08x, data=%08x\n",
           i,
           (unsigned int) tmp,
           (unsigned int) tmp->next,
           (unsigned int) tmp->prev,
           (unsigned int) tmp->data);
    i++;
    tmp = tmp->next;
  }
}

/***** PRIVATE FUNCTIONS *****/

/**************************************************
 *                                                *
 * test functions                                 *
 *                                                *
 **************************************************/


/* END FILE */

