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
* FILENAME:  hash.c
*
* DESCRIPTION: 
*	Non-thread-safe hash table
*
* Date Created: Apr 30, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_hash.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "vos_hash.h"


#define vosStrLen 		strlen
#define vosSafeStrNCpy 	strncpy
#define vosPrintk       printf
#define vosPrintf
#define vosStrCmp       strcmp
#define DIM(Array) ((sizeof((Array)) / sizeof((Array)[0])))

#undef ASSERT

#define ASSERT 

extern void * shmalloc(size_t size);


extern void shmfree(void *p);



#define vosAlloc shmalloc
#define vosFree  shmfree

#ifndef __USE_ISOC99
#define inline
#endif


/* Maximum allowed fullness: when hash table's fullness exceeds this
   value, the table is resized. */
#define HASH_MAX_FULLNESS 0.75

/* The hash table size is multiplied by this factor (and then rounded
   to the next prime) with each resize. This guarantees infrequent resizes. */
#define HASH_RESIZE_FACTOR 2

#if 0

struct HASH_ENTRY_s
{
    void             *key;
    void             *data;
    struct HASH_ENTRY_s *next;
};

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
    struct HASH_ENTRY_s **hashList;
};
#endif

#define HASH_INDEX(key, tab) ((vosLwStringHash)(key) % (tab->hashSize -1))

#define IS_EMPTY(entry) ((entry) == NULL)


/* Find a prime near, but greather than or equal to SIZE.  The primes
   are looked up from a table with a selection of primes convenient
   for this purpose.

   PRIME_OFFSET is a minor optimization: it specifies start position
   for the search for the large enough prime.  The final offset is
   stored in the same variable.  That way the list of primes does not
   have to be scanned from the beginning each time around.  */

static int primeSize(int size, int *primeOffset)
{
  static const int primes[] = {
    13, 19, 29, 41, 59, 79, 107, 149, 197, 263, 347, 457, 599, 787, 1031,
    1361, 1777, 2333, 3037, 3967, 5167, 6719, 8737, 11369, 14783,
    19219, 24989, 32491, 42257, 54941, 71429, 92861, 120721, 156941,
    204047, 265271, 344857, 448321, 582821, 757693, 985003, 1280519,
    1664681, 2164111, 2813353, 3657361, 4754591, 6180989, 8035301,
    10445899, 13579681, 17653589, 22949669, 29834603, 38784989,
    50420551, 65546729, 85210757, 110774011, 144006217, 187208107,
    243370577, 316381771, 411296309, 534685237, 695090819, 903618083,
    1174703521, 1527114613, 1837299131, 2147483647
  };
  int i;

  for (i = *primeOffset; (unsigned int)i < DIM(primes); i++)
    if (primes[i] >= size) {
      *primeOffset = i + 1;
      return primes[i];
    }

  abort();
}


unsigned int vosLwStringHash(void *src)
{
    int i, l;
    unsigned int ret = 0;
    unsigned short *s;
    char *str = (char *)src;
    if (str == NULL)
        return(0);
    l = (vosStrLen(str) + 1) / 2;
    s = (unsigned short *)str;

    for (i = 0; i < l; i++) 
        ret ^= s[i]<<(i&0x0f);

    return(ret);
}

int vosLwEqualString(void *k1, void *k2)
{
    return (0 == vosStrCmp((char *)k1, (char *)k2));
}

inline struct HASH_ENTRY_s *lwhashEntryNew(void *key, void *data)
{
    struct HASH_ENTRY_s *new = vosAlloc(sizeof(struct HASH_ENTRY_s));
    new->key = key;
    new->data = data;
    new->next = NULL;
    return new;
}

void voslwHashEntryFree(void *key, void *value)
{
    vosFree(key);
    vosFree(value);
}
extern void lwConfigOidDestroy(void *key, void *value);
extern void lwConfigIfDestroy(void *key, void *value);
extern void lwConfigKeyDestroy(void *key, void *value);
extern void lwConfigModuleDestroy(void *key, void *value);
extern void lwConfigSectionDestroy(void *key, void *value);

static void lwhashdestroy(HASH_TABLE_t *tab,void *key,void *value)
{
		if( tab->type==HS_OID)				
		lwConfigOidDestroy(key, value);
		if( tab->type==HS_INTF)				
		lwConfigIfDestroy(key, value);
		if( tab->type==HS_KEY)				
		lwConfigKeyDestroy(key, value);
		if( tab->type==HS_MODULE)				
		lwConfigModuleDestroy(key, value);
		if( tab->type==HS_SECTION)				
		lwConfigSectionDestroy(key, value);
		return ;
}
static int lwhashcompare(void *key, void *newkey)
{
	return vosLwEqualString(key,newkey);
}
static int lwhashListUpdate(struct HASH_ENTRY_s *root, void *key, 
    void *data, HASH_TABLE_t *tab)
{
    struct HASH_ENTRY_s *pos;
    for(pos = root; pos != NULL; pos = pos->next ) {
        if ( lwhashcompare(key, pos->key) ) {
			lwhashdestroy(tab,pos->key,pos->data);
            pos->key = key;
            pos->data = data;
            return 0;
        }
    }
    return -1;
}

HASH_TABLE_t *lwHashCreate(unsigned int (*keyfunc)(void *),
                       int (*comparefunc)(void *, void *),
                       void (*destroyfunc)(void *, void *),
                       int size,int type)
{
    int i, primesize;
    int len; 

    HASH_TABLE_t *tab = vosAlloc( sizeof(HASH_TABLE_t) );	
//    ASSERT(tab != NULL, "Out of memory!!!");	
    tab->primeOffset = 0;
    primesize = 1 + size / HASH_MAX_FULLNESS;
    primesize = primeSize(primesize, &tab->primeOffset);
    tab->hashSize = primesize;
    //printf("primesize=%d, primesize * HASH_MAX_FULLNESS = %d\n", primesize, (int)((primesize*75)/100));
    tab->resizeThreshold = (primesize * 75)/100;	
    len = sizeof(struct HASH_ENTRY_s *) * primesize;

    tab->hashList = vosAlloc(len);
   // ASSERT(tab->hashList != NULL, "Out of memory!!!");	
    for (i = 0; i < primesize; i++)
        tab->hashList[i] = NULL ;

    tab->compare = comparefunc;
    tab->getHash = keyfunc;
    tab->entryDestroy = destroyfunc;
    tab->items = tab->count = 0;
    return tab;
}

static void hashResize(HASH_TABLE_t *tab)
{
    int               i, newIndex;
    int               oldsize = tab->hashSize;
    struct HASH_ENTRY_s  **old_entries = tab->hashList;
    struct HASH_ENTRY_s  *pos, *cur_entry, *old_entry;
    int newsize;

    newsize = primeSize(tab->hashSize * HASH_RESIZE_FACTOR, &tab->primeOffset);
    tab->hashSize = newsize;
    //tab->resizeThreshold = newsize * HASH_MAX_FULLNESS;
    tab->resizeThreshold = (newsize * 75)/100;
    tab->hashList = vosAlloc(sizeof(struct HASH_ENTRY_s *) * newsize);
  //  ASSERT(tab->hashList != NULL, "Out of memory!!!");
    tab->count = tab->items = 0;
    for (i = 0; i < newsize; i++)
        tab->hashList[i] = NULL ;

    for (i = 0; i < oldsize; i++) {
        cur_entry = old_entries[i];
        if (IS_EMPTY(cur_entry))
            continue;

        pos = cur_entry->next;
        while (pos != NULL) {
            old_entry = pos;
            pos = pos->next;
            lwHashInsert(old_entry->key, old_entry->data, tab);
            vosFree(old_entry);
        }

        lwHashInsert(cur_entry->key, cur_entry->data, tab);
        vosFree(cur_entry);
        old_entries[i] = NULL;
    }
    
    vosFree(old_entries);
}

void lwHashClear(HASH_TABLE_t *tab)
{
    int i;
    struct HASH_ENTRY_s *pos, *cur_entry, *old_entry;

    if (tab == NULL)
        return;

    if (tab->count == 0)
        return;

    for (i = 0; i < tab->hashSize; i++) {
        cur_entry = tab->hashList[i];
        if (IS_EMPTY(cur_entry))
            continue;

        pos = cur_entry->next;
        while (pos != NULL) {
            old_entry = pos;
            pos = pos->next;
         //   tab->entryDestroy(old_entry->key, old_entry->data);
         	lwhashdestroy(tab,old_entry->key,old_entry->data);
            vosFree(old_entry);
        }

      //  tab->entryDestroy(cur_entry->key, cur_entry->data);
      	lwhashdestroy(tab,cur_entry->key,cur_entry->data);
        vosFree(cur_entry);
        tab->hashList[i] = NULL;
    }

    tab->count = tab->items = 0;
}


void lwHashFree(HASH_TABLE_t *tab)
{
    lwHashClear(tab);
    vosFree(tab->hashList);
    vosFree(tab);
    tab =NULL;
}

void lwHashInsert(void *key, void *data, HASH_TABLE_t *tab)
{
    unsigned int index = HASH_INDEX(key, tab);
    struct HASH_ENTRY_s *l, *pos;

    /* Check to see if the key is in hash table. 
     * If the key existed then replace the value. 
     * Otherwise, append the new HASH_ENTRY_s of the key to the list 
     */
    if ( lwhashListUpdate(tab->hashList[index], key, data, tab ) != 0 ) { 
        /* If adding the item would make the table exceed max. fullness,
           resize the table first.  */
        if (tab->items >= tab->resizeThreshold) {
            hashResize(tab);
            index = HASH_INDEX(key, tab);
        }

        /* append the new hash entry to hash table */
        l = lwhashEntryNew(key, data);
        if (tab->hashList[index] == NULL) {
            tab->hashList[index] = l;
            tab->items++;
        } else {
            for(pos = tab->hashList[index]; pos->next != NULL; pos = pos->next);
            pos->next = l;
        }

        tab->count++;
    }
}
 
void lwHashRemove(void *key, HASH_TABLE_t *tab)
{
    unsigned int index = HASH_INDEX(key, tab);
    struct HASH_ENTRY_s *pos ,*prev, *tmp;

    if (NULL == tab->hashList[index]) return;

    pos = tab->hashList[index];
    if (lwhashcompare(pos->key, key)) {
        tmp = pos->next;
        if (tmp == NULL) {
            tab->hashList[index] = NULL;
        } else {
            tab->hashList[index] = tmp;
        }
       // tab->entryDestroy(pos->key, pos->data);
		lwhashdestroy(tab,pos->key,pos->data);
        vosFree(pos);
        --tab->items;
        --tab->count;
        return;
    }

    prev = tab->hashList[index];
    for (pos = prev->next; NULL != pos; pos = pos->next) {
        if (lwhashcompare(pos->key, key)) {
            prev->next = pos->next;
            //tab->entryDestroy(pos->key, pos->data);
			lwhashdestroy(tab,pos->key,pos->data);
            vosFree(pos);
            --tab->count;
            return;
        }
        prev = pos;
    }
}

void *lwHashValue(void *key, HASH_TABLE_t *tab)
{
    struct HASH_ENTRY_s *pos;
    unsigned int index = HASH_INDEX(key, tab);
    for (pos = tab->hashList[index]; NULL != pos; pos = pos->next) {
        if (lwhashcompare(key, pos->key)) {
            return (pos->data);
        }
    }
    return NULL;
}

/*******************************************************************************
*
* lwHashValueGetByIndex:	 
*
* DESCRIPTION:
* 	hash value get by index, the performance of this function is bad
*
* INPUTS:
*   index - index of the HASH_TABLE_t, zero base
*   tab   - HASH_TABLE_t
*   key   - the key of current index
*   data  - the data of current index
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int lwHashValueGetByIndex(unsigned int index, HASH_TABLE_t *tab, void **key, void **data)
{
    int i, current = 0;
    struct HASH_ENTRY_s *pos;

    if (index >= (unsigned int)(tab->count))
        return -1;
    
    for (i = 0; i < tab->hashSize; i++) {
        for (pos = tab->hashList[i]; NULL != pos; pos = pos->next ) {
            if (index == (unsigned int)current)
            {
                *key = pos->key;
                *data = pos->data;
                return 0;
            }
            current++;
        }
    }
    return -1;
}

void lwHashForEachDo(HASH_TABLE_t *tab, int(cb)(void *, void *))
{
    int i = 0;
    struct HASH_ENTRY_s *pos;
    for (i = 0; i < tab->hashSize; i++) {
        for (pos = tab->hashList[i]; NULL != pos; pos = pos->next ) {
            cb(pos->key, pos->data);
        }
    }
}

void lwHashForEachDoCb(
    HASH_TABLE_t *tab, 
    FUNCPTR pCallback, 
    unsigned int ulArg3, 
    unsigned int ulArg4, 
    unsigned int ulArg5, 
    unsigned int ulArg6, 
    unsigned int ulArg7, 
    unsigned int ulArg8)
{
    int i = 0;
    struct HASH_ENTRY_s *pos;
    for (i = 0; i < tab->hashSize; i++) {
        for (pos = tab->hashList[i]; NULL != pos; pos = pos->next ) {
            if (pCallback)
            {
                pCallback((unsigned int)(pos->key), 
                    (unsigned int)(pos->data), 
                    ulArg3, 
                    ulArg4, 
                    ulArg5, 
                    ulArg6, 
                    ulArg7, 
                    ulArg8);
            }
        }
    }
}

inline int lwHashSize(HASH_TABLE_t *tab)
{
    return tab->hashSize;
}

inline int lwHashCount(HASH_TABLE_t *tab)
{
    return tab->count;
}

inline int lwHashItems(HASH_TABLE_t *tab)
{
    return tab->items;
}


