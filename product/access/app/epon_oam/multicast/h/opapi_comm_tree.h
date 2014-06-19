/*************************************************************************
*
* Copyright 2003~2030 OPL, All Rights Reserved
*
* @file opapi_comm_tree.h
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

#ifndef OPAPI_COMM_TREE_H
#define OPAPI_COMM_TREE_H

/***** INCLUDE FILES *****/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/***** DEFINES and ENUMS *****/
typedef int (*clTraverseFunc) (void *key, void *data);
typedef int (*clCompareFunc) (void *key, void *data);
typedef void (*clDestroyFunc) (void *key);

struct clTree {
  struct clTreeNode *root;
  clCompareFunc key_compare;
  clDestroyFunc key_destroy;
};

struct clTreeNode {
  void *key;
  struct clTreeNode *left;
  struct clTreeNode *right;
};

/***** STRUCTURES and UNIONS *****/

/***** TYPEDEFS *****/

/***** EXTERNS *****/
extern struct clTree *clTreeNew(clCompareFunc key_compare,
                                clDestroyFunc key_destroy);
extern struct clTree *clTreeNew(clCompareFunc key_compare,
                                clDestroyFunc key_destroy);
extern void clTreeInsert(struct clTree *tree, void *key);
extern int clTreeRemove(struct clTree *tree, void *key);
extern void clTreeTraverse(struct clTree *tree,
                           clTraverseFunc traverseFunc,
                           void *userData);
extern void *clTreeSearch(struct clTree *tree, void *userData);
extern int clTreeDestroy(struct clTree *tree);


/***** PROTOTYPES *****/

#endif /* OPAPI_COMM_TREE_H */

/* END FILE */

