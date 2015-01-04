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
* FILENAME:  vos_hash.h
*
* DESCRIPTION: 
*	
*
* Date Created: Apr 30, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_hash.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_HASH_H_
#define __VOS_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif
 
#include <string.h>
//#include <vos_types.h>

#ifndef __USE_ISOC99
#define inline
#endif

#ifdef __cplusplus
typedef int             (*FUNCPTR) (...);     /* ptr to function returning int */
#else
typedef int             (*FUNCPTR) ();	   /* ptr to function returning int */
#endif

#define lwHashTableCreate(hSize) \
         lwHashCreate(vosStringHash, vosEqualString, vosHashEntryFree, hSize)

#if 1
struct HASH_ENTRY_s
{
    void             *key;
    void             *data;
    struct HASH_ENTRY_s *next;
};
#define HS_INTF 1
#define HS_MODULE 2
#define HS_SECTION 3
#define HS_KEY     4
#define HS_OID     5

struct HASH_TABLE_s
{
    unsigned int            (*getHash)(void *);
    int               (*compare)(void *, void *);
    void              (*entryDestroy)(void *, void *);
    int               items;             /* size of the hash table used items. */
    int               hashSize;          /* size of the hash table array. */
    int               count;             /* number of non-empty entries. */
    int               resizeThreshold;  /* after size exceeds this number of entries,
                                            resize the hash table. */
    int               primeOffset;      /* the offset of the current prime
                                            in the prime table. */
    int    type;
    struct HASH_ENTRY_s **hashList;
};
#endif

//struct HASH_ENTRY_s;
//struct HASH_TABLE_s;
typedef struct HASH_TABLE_s HASH_TABLE_t;

extern unsigned int vosLwStringHash(void *src);
int vosLwEqualString(void *k1, void *k2);
void lwHashEntryFree(void *key, void *value);

HASH_TABLE_t *lwHashCreate(unsigned int (*keyfunc)(void *),
                       int (*comparefunc)(void *, void *),
                       void (*entrydestroyfunc)(void *, void *),
                       int size,int type);
void lwHashClear(HASH_TABLE_t *tab);
void lwHashFree(HASH_TABLE_t *tab);
void lwHashInsert(void *key, void *data, HASH_TABLE_t *tab);
void lwHashRemove(void *key, HASH_TABLE_t *tab);
void *lwHashValue(void *key, HASH_TABLE_t *tab);
int lwHashValueGetByIndex(unsigned int index, 
                            HASH_TABLE_t *tab, 
                            void **key, 
                            void **data);
void lwHashForEachDo(HASH_TABLE_t *tab, int (cb)(void *, void *));
void lwHashForEachDoCb(HASH_TABLE_t *tab, 
                         FUNCPTR pCallback, 
                         unsigned int ulArg3, 
                         unsigned int ulArg4, 
                         unsigned int ulArg5, 
                         unsigned int ulArg6, 
                         unsigned int ulArg7, 
                         unsigned int ulArg8);

int lwHashSize(HASH_TABLE_t *tab);
int lwHashCount(HASH_TABLE_t *tab);
int lwHashItems(HASH_TABLE_t *tab);

#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_HASH_H_ */

