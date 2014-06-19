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
#include <vos_types.h>

#ifndef __USE_ISOC99
#define inline
#endif

#define vosHashTableCreate(hSize) \
         vosHashCreate(vosStringHash, vosEqualString, vosHashEntryFree, hSize)

struct HASH_ENTRY_s;
struct HASH_TABLE_s;
typedef struct HASH_TABLE_s HASH_TABLE_t;

uint32 vosStringHash(void *src);
int vosEqualString(void *k1, void *k2);
void vosHashEntryFree(void *key, void *value);

HASH_TABLE_t *vosHashCreate(uint32 (*keyfunc)(void *),
                       int (*comparefunc)(void *, void *),
                       void (*entrydestroyfunc)(void *, void *),
                       int size);
void vosHashClear(HASH_TABLE_t *tab);
void vosHashFree(HASH_TABLE_t *tab);
void vosHashInsert(void *key, void *data, HASH_TABLE_t *tab);
void vosHashRemove(void *key, HASH_TABLE_t *tab);
void *vosHashValue(void *key, HASH_TABLE_t *tab);
int vosHashValueGetByIndex(uint32 index, 
                            HASH_TABLE_t *tab, 
                            void **key, 
                            void **data);
void vosHashForEachDo(HASH_TABLE_t *tab, int (cb)(void *, void *));
void vosHashForEachDoCb(HASH_TABLE_t *tab, 
                         FUNCPTR pCallback, 
                         uint32 ulArg3, 
                         uint32 ulArg4, 
                         uint32 ulArg5, 
                         uint32 ulArg6, 
                         uint32 ulArg7, 
                         uint32 ulArg8);

int vosHashSize(HASH_TABLE_t *tab);
int vosHashCount(HASH_TABLE_t *tab);
int vosHashItems(HASH_TABLE_t *tab);

#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_HASH_H_ */

