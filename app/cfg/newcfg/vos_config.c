/*************************************************************************

**************************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#include <lw_list.h>

#include <vos_hash.h>
//#include <config_oid.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
//#include <mw_type.h>
#include <vos_types.h>

//#include <mw_config.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <lw_type.h>    
#include <lw_config_api.h>
#include <lw_config_oid.h>
#include "vos_config.h"
#include <lw_if_pub.h>
#include <lw_if_api.h>


#undef DEBUG
#define DEBUG
#ifdef DEBUG
#define	DBG_PRINTF(fmt, ...) do { fprintf(stderr, "DBG [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)

#else
#define DBG_PRINTF
#endif

#undef ASSERT
#define ASSERT
int lwConfigName2Oid(char * name,unsigned int *oid);
void cfg_setupif(CFG_DOMAIN_E domain);


/*oid table */
static CFG_OID_REGINFO_S  g_cfg_oid_tbl[]=
{
    CONFIG_PLAT_OIDTABLE
};
extern void pool_create( unsigned char *base, int size);
extern unsigned char  *pool_alloc( size_t bytes );
extern void pool_free( unsigned char *mem);
int master_flag=0;
 void * shmalloc(size_t size)
{
	 if(master_flag==MASTER)
     return (void *)pool_alloc(size);
	 else
	 return (void *)malloc(size);
}

 void shmfree(void *p)
{   
	if(master_flag==MASTER)
	pool_free((unsigned char *)p);
	else
	free((unsigned char *)p);
}

#define vosAlloc shmalloc
#define vosFree  shmfree
int g_iLockfd;
HASH_TABLE_t * g_pstOidRegHashTbl=NULL;//for name iterator


CFG_DATABASE_S * g_pstcfgdb=NULL;

LIST_HEAD(g_OidRegList);//for oid iterator
int isMaster(void)
{
	return master_flag==MASTER?1:0;
}

static void __cfg_shm_lock( int ifd, short stype )
{
    struct flock stlock;
    stlock.l_type = stype;
    stlock.l_whence = SEEK_SET;
    stlock.l_start = 0;
    stlock.l_len = 0;

    if(fcntl(ifd, F_SETLKW, &stlock) < 0)
    {
        
    }
    return;
}

/*****************************************************************************
                                                                            
*****************************************************************************/
static void __cfg_shm_unlock( int ifd )
{
    struct flock stlock;
    stlock.l_type = F_UNLCK;
    stlock.l_whence = SEEK_SET;
    stlock.l_start = 0;
    stlock.l_len = 0;
    if(fcntl(ifd, F_SETLK, &stlock) < 0)
    {
         
    }
    return;

}

static void __cfg_read_lock( int * pilockfd )
{
    {
        __cfg_shm_lock(*pilockfd, F_RDLCK);
    }
     return;
}

static void __cfg_read_unlock( int ilockfd )
{
    if(ilockfd !=-1)
    {
        __cfg_shm_unlock(ilockfd);        
    }
    else
    {
      	
    }
    return;

}

static void __cfg_write_lock( int  *piPlockfd )
{
    {
        __cfg_shm_lock(*piPlockfd, F_WRLCK);
    }   
    return;
 
}

/*****************************************************************************

*****************************************************************************/
static void __cfg_write_unlock( int ilockfd )
{
    if(ilockfd !=-1)
    {
        __cfg_shm_unlock(ilockfd);       
    }
    else
    {
        
    }
    return;
}



/*--------------local  variable declaration and definition------------- */

 void lwConfigModuleDestroy(void *key, void *value);
 void lwConfigSectionDestroy(void *key, void *value);
 void lwConfigKeyDestroy(void *key, void *value);
static char *trim(const char *str);
static int lwConfigFlushKeyReset(void * key,void * value);
static int lwConfigFlushSectionReset(void * key,void * value);
static unsigned int lwConfigFlushKeyAppend(void * key,void * value, FILE *stream);
static unsigned int lwConfigFlushSectionAppend(void * key,void * value, FILE *stream);
//static int vosConfigFlushForEachModule(void * key,void * value);
static int lwConfigShowForEachKey(void * key,void * value, int fd);
static int lwConfigShowForEachSection(void * key,void * value, int fd);
static int lwConfigShowForEachModule(void * key,void * value, int fd);


void lwConfigIfDestroy(void *key, void *value)
{
    char *k = key;
    CFG_IFM_DATA_t *v = value;
    vosFree(k);
    lwHashFree(v->modules);
    vosFree(v);
}

void lwConfigModuleDestroy(void *key, void *value)
{
    char *k = key;
    CFG_MODULE_DATA_t *v = value;
    vosFree(k);
    lwHashFree(v->sections);
    vosFree(v);
}

void lwConfigSectionDestroy(void *key, void *value)
{
    char *k = key;
    CFG_SECTION_DATA_t *v = value;
    vosFree(k);
    lwHashFree(v->keys);
    vosFree(v);
}

void lwConfigKeyDestroy(void *key, void *value)
{
    char *k = key;
    CFG_KEY_DATA_t *v = value;
    vosFree(k);
    vosFree(v->data);
    vosFree(v);
}

/*******************************************************************************
*
* lwConfigDestroy:	 destroy config database, but dont delete config files.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	void
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	less than zero - error
*
* SEE ALSO: 
*/
int lwConfigDestroy(void)
{
	int i=0;
	__cfg_write_lock(&g_iLockfd);
	for(i=CONFIG_NORMAL;i<CONFIG_MAX_DOMAINS;i++)
	{
	    if (g_pstcfgdb->pstConfigDatabase[i]) {
	        lwHashFree(g_pstcfgdb->pstConfigDatabase[i]);
	    }
	}
	__cfg_write_unlock(g_iLockfd);
    return 0;
}
#if 0
static char * trim(const char *str)
{
    const char *p = str;
    char *result;
    size_t len;

    assert(p != NULL);

    while (*p == ' ' || *p == '\t')
        p++;

    len = vosStrLen(p) + 1;
    result = (char *)vosAlloc(len);
   
    if (NULL == result)
    {
        return NULL;
    }
 
    result[0] = 0;
    vosSafeStrNCpy(result, p, len);

    while (result[vosStrLen(result) - 1] == ' ' ||
        result[vosStrLen(result) - 1] == '\t' ||
        result[vosStrLen(result) - 1] == '\r') {
        result[vosStrLen(result) - 1] = '\0';
    }

  return result;
}
#endif
static int lwConfigFlushKeyReset(void * key,void * value)
{
    CFG_KEY_DATA_t *tkey = (CFG_KEY_DATA_t *)value;
    if (tkey) {
        tkey->flag = CFG_NOT_FLUSHED;
    }
    return 0;
}

static int lwConfigFlushSectionReset(void * key,void * value)
{
    CFG_SECTION_DATA_t *tsection = (CFG_SECTION_DATA_t *)value;
    if (tsection) {
        tsection->flag = CFG_NOT_FLUSHED;
        lwHashForEachDo(tsection->keys, lwConfigFlushKeyReset);
    }
    return 0;
}
static int lwConfigFlushModuleReset(void * key,void * value)
{
    CFG_MODULE_DATA_t *tmodule = (CFG_MODULE_DATA_t *)value;
    if (tmodule) {
        tmodule->flag = CFG_NOT_FLUSHED;
        lwHashForEachDo(tmodule->sections, lwConfigFlushSectionReset);
    }
    return 0;
}
static int lwConfigFlushIfReset(void * key,void * value)
{
    CFG_IFM_DATA_t *tifm = (CFG_IFM_DATA_t *)value;
    if (tifm) {
        tifm->flag = CFG_NOT_FLUSHED;
        lwHashForEachDo(tifm->modules, lwConfigFlushModuleReset);
    }
    return 0;
}
static int lwConfigFlushReset(CFG_DOMAIN_E domain)	
{    
    if (g_pstcfgdb->pstConfigDatabase[domain]) 
	{      
        lwHashForEachDo(g_pstcfgdb->pstConfigDatabase[domain], lwConfigFlushIfReset);
    }
    return 0;
}

static unsigned int lwConfigFlushKeyAppend(void * key,void * value, FILE *stream)
{
    CFG_KEY_DATA_t *tkey = (CFG_KEY_DATA_t *)value;
    if (tkey && tkey->flag == CFG_NOT_FLUSHED) {
        fprintf(stream, "    %s=%s\n", (char *)key, (char *)tkey->data);
        tkey->flag = CFG_FLUSHED;
    }
    return 0;
}

static unsigned int lwConfigFlushSectionAppend(void * key,void * value, FILE *stream)
{
    CFG_SECTION_DATA_t *data = (CFG_SECTION_DATA_t *)value;
    if (data && data->flag == CFG_NOT_FLUSHED) {
        data->flag = CFG_FLUSHED;
        fprintf(stream, "  #%s\n", (char *)key);
        lwHashForEachDoCb(data->keys,
            (FUNCPTR)lwConfigFlushKeyAppend, (unsigned int)stream, 4, 5, 6, 7, 8);
    }
    return 0;
}
static unsigned int lwConfigFlushModuleAppend(void * key,void * value, FILE *stream)
{
	CFG_MODULE_DATA_t *data = (CFG_MODULE_DATA_t *)value;
	if (data && data->flag == CFG_NOT_FLUSHED) 
	{
        data->flag = CFG_FLUSHED;
		fprintf(stream, " @%s\n", (char *)key);
		lwHashForEachDoCb(data->sections,
            (FUNCPTR)lwConfigFlushSectionAppend, (unsigned int)stream, 4, 5, 6, 7, 8);
	}
    return 0;
}
static unsigned int lwConfigFlushIfmAppend(void * key,void * value, FILE *stream)
{
	CFG_IFM_DATA_t *data = (CFG_IFM_DATA_t *)value;
	if (data && data->flag == CFG_NOT_FLUSHED) 
	{
	    if(strcmp(key,IF_TMP_NAME)!=0)
        {   
            data->flag = CFG_FLUSHED;
    		fprintf(stream, "$%s\n", (char *)key);
    		lwHashForEachDoCb(data->modules,
                (FUNCPTR)lwConfigFlushModuleAppend, (unsigned int)stream, 4, 5, 6, 7, 8);
       }
	}
    return 0;
}
static int __append_md5(char *orgfile,char *md5file)
{
        char cmd[64] = {0};
        int ret = 0;
        sprintf(cmd,"md5sum %s > %s",orgfile, md5file);
        if(0 > system(cmd))
        {
                return -1;
        }
        sprintf(cmd,"cat %s >> %s",orgfile,md5file );
       
        ret = system(cmd);

        if((ret>=0) && WIFEXITED(ret))
        {
                if(EXIT_SUCCESS == WEXITSTATUS(ret))
                {
                        return 0;
                }
        }        
        remove(md5file);
        return -1;
}

int lwConfigFlush(CFG_DOMAIN_E enDomain,CHAR *pPath)
{
	FILE *fp=NULL;
	int iRet=0;
	unsigned char szCmd[1024];
	if(pPath==NULL)
	{		
		return -1;
	}
	if((fp=fopen(pPath,"w+"))==NULL)
	{		
		return -1;
	}
	fclose(fp);
	fp=fopen(CONFIG_TMP_FILE,"w+");
    if(fp==NULL)
    {    	
        return -1;
    }
    fprintf(fp,"%s/%s\n",PRODUCT_NAME,CONFIG_CUST_DEVELOP_VER_STR);
	__cfg_read_lock(&g_iLockfd);
	lwConfigFlushReset(enDomain);
	if (g_pstcfgdb->pstConfigDatabase[enDomain])
    {
        lwHashForEachDoCb(g_pstcfgdb->pstConfigDatabase[enDomain],
            (FUNCPTR)lwConfigFlushIfmAppend, (unsigned int)fp, 4, 5, 6, 7, 8);
    }	
	if(strcmp(pPath,CONFIG_MNT_START)==0)
	g_pstcfgdb->commitflag[enDomain]=0;
	__cfg_read_unlock(g_iLockfd);
	fclose(fp);
	
    iRet=__append_md5(CONFIG_TMP_FILE,CONFIG_TMP2_FILE);
    if(0 != iRet)
    {
        remove(CONFIG_TMP_FILE);		
        return -1;
    }
	
	sprintf((char *)szCmd,"dos2unix %s",CONFIG_TMP2_FILE);
	system((char *)szCmd);
	sprintf((char *)szCmd,"cp -rf %s %s",CONFIG_TMP2_FILE,pPath);
	system((char *)szCmd);

	remove(CONFIG_TMP2_FILE);
    remove(CONFIG_TMP_FILE);
	return 0;
	
}
/*print to console or terminal*/
static int lwConfigShowForEachKey(void *key, void *value, int fd)
{
    CFG_KEY_DATA_t *tkey = (CFG_KEY_DATA_t *)value;
    if (tkey && tkey->data)
    {
        printf("    %-20s : %s\r\n", (char *)key, (char *)tkey->data);
    }
    return 0;
}


static int lwConfigShowForEachSection(void *key, void *value, int fd)
{
    CFG_SECTION_DATA_t *tsection = (CFG_SECTION_DATA_t *)value;
    printf("  #%s\r\n", (char *)key);
    if (tsection && tsection->keys) {
        lwHashForEachDoCb(tsection->keys,
            (FUNCPTR)lwConfigShowForEachKey, (unsigned int)fd, 4, 5, 6, 7, 8);
    }
    return 0;
}

static int lwConfigShowForEachModule(void *key, void *value, int fd)
{
    CFG_MODULE_DATA_t *tmodule = (CFG_MODULE_DATA_t *)value;

    /* strip the default path */
    printf("@%s\r\n", ((char *)key));

    if (tmodule && tmodule->sections) {
        lwHashForEachDoCb(tmodule->sections,
            (FUNCPTR)lwConfigShowForEachSection, (unsigned int)fd, 4, 5, 6, 7, 8);
    }
    return 0;
}
static void lwConfigShowForEachIf(void *key, void *value, int fd)
{
	(void)fd;
	CFG_IFM_DATA_t  *tifm=(CFG_IFM_DATA_t*)value;
	
	printf("$%s\r\n", ((char *)key));

    if (tifm && tifm->modules)
    {
        lwHashForEachDoCb(tifm->modules,
        (FUNCPTR)lwConfigShowForEachModule,(unsigned int)fd, 4, 5, 6, 7, 8);
    }	
	return ;
}
/*******************************************************************************
*
* lwConfigShow:	 Display current configuration variables for all modules.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fd - file descriptor on which to write
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void lwConfigShow(CFG_DOMAIN_E enDomain,int fd)
{
	__cfg_read_lock(&g_iLockfd);
	if (g_pstcfgdb->pstConfigDatabase[enDomain])
    {
        lwHashForEachDoCb(g_pstcfgdb->pstConfigDatabase[enDomain],
            (FUNCPTR)lwConfigShowForEachIf, (unsigned int)fd, 4, 5, 6, 7, 8);
    }
	__cfg_read_unlock(g_iLockfd);

}
int lwConfigValueGet(
	CFG_DOMAIN_E enDomain,
	const unsigned char *ifm,
    const unsigned char *fname,
    const unsigned char *section,
    const unsigned char *key,
    unsigned char *default_value,
    void *result,
    int retlen)
{
	CFG_IFM_DATA_t  *tifm;
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;
    char *value = (char *)default_value;
	int rv=0;
   // vosMutexTake(&g_pstConfigLock);
	__cfg_read_lock(&g_iLockfd);
	if(enDomain >=CONFIG_MAX_DOMAINS)
	{
		DBG_PRINTF("domain error, too big!");
		rv=-1;
		goto exit_label;
	}
    if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
    {
    	DBG_PRINTF("domain data is empty!");
    	rv=-1;
        goto exit_label;
    }
	tifm = (CFG_IFM_DATA_t *)lwHashValue((void *)ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
    if (tifm == NULL)
    {
    	DBG_PRINTF("hash ifm[%d][%s] error!",enDomain,ifm);
    	rv=-1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)lwHashValue((void *)fname, tifm->modules);
    if (tmodule == NULL)
    {
    	DBG_PRINTF("hash fname error!");
    	rv=-1;
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)lwHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
    	DBG_PRINTF("hash section error!");
    	rv=-1;
        goto exit_label;
    }

    tkey = (CFG_KEY_DATA_t *)lwHashValue((void *)key, tsection->keys);
    if (tkey == NULL)
    {
    	DBG_PRINTF("hash key (%s)error!",key);
    	rv=-1;
        goto exit_label;
    }

    if (tkey->data == NULL)
    {
    	DBG_PRINTF("tkey->data is NULL!");
    	rv=-1;
        goto exit_label;
    }

    value = tkey->data;
	
exit_label:
	if((value!=NULL)&&(strlen(value)<=(retlen-1)))
	{		
		strcpy(result,value);
	}
	else
	{
		rv=-1;
	}
    __cfg_read_unlock(g_iLockfd);
    return rv;
}

int lwConfigValueSet(
	CFG_DOMAIN_E enDomain,
	const unsigned char *ifm,
    const unsigned char *fname,
    const unsigned char *section,
    const unsigned char *key,
    const unsigned char *value)
{
	CFG_IFM_DATA_t  *tifm;
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
    CFG_KEY_DATA_t *tkey;
    int rv = 0;
	void *hashkey=NULL;
   // vosMutexTake(&g_pstConfigLock);
    
	__cfg_write_lock(&g_iLockfd);
	if(enDomain >=CONFIG_MAX_DOMAINS)
	{
		rv = -1;		
		goto exit_label;
	}	
	
	if((isMaster()!=TESTER)&&(isMaster()!=MASTER))
	{
			rv = -1;		
		goto exit_label;
	}
	
	if(ifm==NULL || fname==NULL || section == NULL
		 || key==NULL)
	{
		rv = -1;		
		goto exit_label;
	}
    if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
    {
        rv = -1;		
        goto exit_label;
    }
	tifm = (CFG_IFM_DATA_t *)lwHashValue((void*)ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	if(tifm==NULL)
	{
		
		tifm = vosAlloc(sizeof(CFG_IFM_DATA_t));
//        ASSERT(tifm != NULL, "Out of memory!!!");
        if (NULL == tifm)
        {
           rv = -1;		   
           goto exit_label;         
        }
        tifm->modules=lwHashCreate(vosLwStringHash,vosLwEqualString,lwConfigModuleDestroy,0,HS_MODULE);  
		hashkey=vosAlloc(strlen((char *)ifm)+1);
		if(hashkey==NULL)
		{
			rv = -1;		   
			goto exit_label; 
		}
		strcpy((char *)hashkey, (const char *)ifm);
        lwHashInsert(hashkey, tifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	}
	
    tmodule = (CFG_MODULE_DATA_t *)lwHashValue((void*)fname, tifm->modules);
    if (tmodule == NULL) {
        tmodule = vosAlloc(sizeof(CFG_MODULE_DATA_t));
     //   ASSERT(tmodule != NULL, "Out of memory!!!");
        if (NULL == tmodule)
        {
           rv = -1;		   
           goto exit_label;         
        }
        tmodule->sections = lwHashCreate(vosLwStringHash, vosLwEqualString, lwConfigSectionDestroy, 0,HS_SECTION);

		hashkey=vosAlloc(strlen((char *)fname)+1);
		if(hashkey==NULL)
		{
			rv = -1;		  
           goto exit_label; 
		}
		strcpy((char *)hashkey,(const char *)fname);
		lwHashInsert(hashkey, tmodule, tifm->modules);
    }

    tsection = (CFG_SECTION_DATA_t *)lwHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
        tsection = vosAlloc(sizeof(CFG_SECTION_DATA_t));
    //    ASSERT(tsection != NULL, "Out of memory!!!");
       
        if (NULL == tsection)
        {
           rv = -1;		            
           goto exit_label;         
        }        
       
        tsection->keys = lwHashCreate(vosLwStringHash, vosLwEqualString, lwConfigKeyDestroy, 0,HS_KEY);
        tsection->flag = CFG_NOT_FLUSHED;
		hashkey=vosAlloc(strlen((char *)section)+1);
		if(hashkey==NULL)
		{
			rv = -1;		  
           goto exit_label; 
		}
		strcpy((char *)hashkey,(const char *)section);
        lwHashInsert(hashkey, tsection, tmodule->sections);
    }
	tkey = (CFG_KEY_DATA_t *)lwHashValue((void *)key, tsection->keys);
	if(tkey==NULL)
	{
	    tkey = vosAlloc(sizeof(CFG_KEY_DATA_t));
	  //  ASSERT(tsection != NULL, "Out of memory!!!");	   
	    if (NULL == tkey)
	    {
	        rv = -1;			       
	        goto exit_label;         
	    }	   
		hashkey=vosAlloc(strlen((char *)value)+1);
		if(hashkey==NULL)
		{
			rv = -1;		   
	        goto exit_label; 
		}
		strcpy((char *)hashkey,(const char *)value);
	    tkey->data = hashkey;
	    tkey->flag = CFG_NOT_FLUSHED;
		hashkey=vosAlloc(strlen((char *)key)+1);
		if(hashkey==NULL)
		{
			rv = -1;		   
	       goto exit_label; 
		}
		strcpy((char *)hashkey,(const char *)key);
    	lwHashInsert(hashkey, tkey, tsection->keys);
	}
	else
	{
		if(tkey->data!=NULL)
		{
			vosFree(tkey->data);
		}
		hashkey=vosAlloc(strlen((char *)value)+1);
		if(hashkey==NULL)
		{
		   rv = -1;		   
	       goto exit_label; 
		}
		strcpy((char *)hashkey,(const char *)value);
		tkey->data=hashkey;
	}		
	g_pstcfgdb->commitflag[enDomain]=1;
exit_label:
    __cfg_write_unlock(g_iLockfd);
    return rv;
}

int lwConfigDomainDelete(CFG_DOMAIN_E enDomain)
{
	__cfg_write_lock(&g_iLockfd);
	lwHashFree(g_pstcfgdb->pstConfigDatabase[enDomain]);
	g_pstcfgdb->pstConfigDatabase[enDomain]=lwHashCreate(
						   vosLwStringHash,
						   vosLwEqualString,
						   lwConfigIfDestroy,
						   0,HS_INTF);		        
	 __cfg_write_unlock(g_iLockfd);
     return 0;
}

int lwConfigIfDelete(CFG_DOMAIN_E enDomain,unsigned char* ifm)
{
	//CFG_IFM_DATA_t *tifm=NULL;
	int rv = 0;

	__cfg_write_lock(&g_iLockfd);
	
	if(enDomain>=CONFIG_MAX_DOMAINS)
	{
		rv = -1;
		goto exit_label;
	}
		
	if( ifm==NULL)
	{
		rv = -1;
		goto exit_label;
	}
	if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
	{
		rv = -1;
		goto exit_label;
	}	 
	lwHashRemove((void *)ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	exit_label:	
	 __cfg_write_unlock(g_iLockfd);
	return rv;
}
/** description: delete config module.
 *  @param fname: the file name of the module, which include full path
 *  @return success: 0
 *          fail:    -1

 *  @see
 *  @deprecated
 */
int lwConfigModuleDelete(CFG_DOMAIN_E enDomain,unsigned char* ifm,unsigned char *fname)
{
	CFG_IFM_DATA_t *tifm=NULL;
   // CFG_MODULE_DATA_t *tmodule;
    int rv = 0;

	__cfg_write_lock(&g_iLockfd);
   
    if(enDomain>=CONFIG_MAX_DOMAINS)
	{
		rv = -1;
        goto exit_label;
	}
	
	if(fname==NULL || ifm==NULL)
	{
		rv = -1;
        goto exit_label;
	}
    if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
    {
        rv = -1;
        goto exit_label;
    }    

	
	tifm = (CFG_IFM_DATA_t *)lwHashValue(ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	if (tifm == NULL)
	{
		rv = -1;
		goto exit_label;
	}
	lwHashRemove((void *)fname, tifm->modules);    

exit_label:    
    __cfg_write_unlock(g_iLockfd);
    return rv;
}

int lwConfigSectionDelete(CFG_DOMAIN_E enDomain,const unsigned char* ifm,const unsigned char *fname, const unsigned char *section)
{
	CFG_IFM_DATA_t *tifm=NULL;
    CFG_MODULE_DATA_t *tmodule;
  //  CFG_SECTION_DATA_t *tsection;
    int rv = 0;
   
	__cfg_write_lock(&g_iLockfd);
   
	if(enDomain>=CONFIG_MAX_DOMAINS)
	{
		rv = -1;
        goto exit_label;
	}
	
	if(fname==NULL || section==NULL || ifm==NULL)
	{
		rv = -1;
        goto exit_label;
	}
    if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
    {
        rv = -1;
        goto exit_label;
    }    
	tifm = (CFG_IFM_DATA_t *)lwHashValue((void *)ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
    if (tifm == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)lwHashValue((void *)fname, tifm->modules);
    if (tmodule == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    lwHashRemove((void *)section, tmodule->sections);

exit_label:
    __cfg_write_unlock(g_iLockfd);
    return rv;
}

int lwConfigKeyDelete(CFG_DOMAIN_E enDomain,unsigned char* ifm,unsigned char *fname, unsigned char *section, unsigned char *key)
{
	CFG_IFM_DATA_t *tifm=NULL;
    CFG_MODULE_DATA_t *tmodule=NULL;
    CFG_SECTION_DATA_t *tsection=NULL;
    int rv = 0;

    __cfg_write_lock(&g_iLockfd);

	if(enDomain>=CONFIG_MAX_DOMAINS)
	{
		rv = -1;
        goto exit_label;
	}
	if(fname==NULL || section==NULL || key==NULL||ifm==NULL)
	{
		rv = -1;
        goto exit_label;
	}
    if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
    {
        rv = -1;
        goto exit_label;
    }
	tifm = (CFG_IFM_DATA_t *)lwHashValue(ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
    if (tifm == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tmodule = (CFG_MODULE_DATA_t *)lwHashValue(fname, tifm->modules);
    if (tmodule == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)lwHashValue(section, tmodule->sections);
    if (tsection == NULL)
    {
        rv = -1;
        goto exit_label;
    }

    lwHashRemove((void *)key, tsection->keys);

exit_label:
    __cfg_write_unlock(g_iLockfd);
    return rv;
}
int lwConfigSectionCreate(
	CFG_DOMAIN_E enDomain,
	char *ifm,
    char *fname,
    const char *section)
{
	CFG_IFM_DATA_t  *tifm=NULL;
    CFG_MODULE_DATA_t *tmodule=NULL;
    CFG_SECTION_DATA_t *tsection=NULL;
    //CFG_KEY_DATA_t *tkey=NULL;
    int rv = 0;
	void *hashkey=NULL;
    __cfg_write_lock(&g_iLockfd);

	if(enDomain >=CONFIG_MAX_DOMAINS)
	{
		rv = -1;
		goto exit_label;
	}
	if(ifm==NULL || fname==NULL || section == NULL)
	{
		rv = -1;
		goto exit_label;
	}
    if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
    {
        rv = -1;
        goto exit_label;
    }
	tifm = (CFG_IFM_DATA_t *)lwHashValue(ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	if(tifm==NULL)
	{
		tifm = vosAlloc(sizeof(CFG_IFM_DATA_t));
       // ASSERT(tifm != NULL, "Out of memory!!!");
        if (NULL == tifm)
        {
           rv = -1;
           goto exit_label;         
        }
        tifm->modules=lwHashCreate(vosLwStringHash,vosLwEqualString,lwConfigModuleDestroy,0,HS_MODULE);  
		hashkey=vosAlloc(strlen(ifm)+1);
		if(hashkey==NULL)
		{
			rv = -1;		   
	       goto exit_label; 
		}
		strcpy(hashkey,ifm);
        lwHashInsert(hashkey, tifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	}

    tmodule = (CFG_MODULE_DATA_t *)lwHashValue(fname, tifm->modules);
    if (tmodule == NULL) {
        tmodule = vosAlloc(sizeof(CFG_MODULE_DATA_t));
      //  ASSERT(tmodule != NULL, "Out of memory!!!");

        if (tmodule == NULL) {
            rv = -1;
            goto exit_label;
        }
        tmodule->sections = lwHashCreate(vosLwStringHash, vosLwEqualString, lwConfigSectionDestroy, 0,HS_SECTION);
		hashkey=vosAlloc(strlen(fname)+1);
		if(hashkey==NULL)
		{
			rv = -1;		   
	       goto exit_label; 
		}
		strcpy(hashkey,fname);
		lwHashInsert(hashkey, tmodule, tifm->modules);
    }

    tsection = (CFG_SECTION_DATA_t *)lwHashValue((void *)section, tmodule->sections);
    if (tsection == NULL)
    {
        tsection = vosAlloc(sizeof(CFG_SECTION_DATA_t));
      //  ASSERT(tsection != NULL, "Out of memory!!!");

        if (tsection == NULL) {
            rv = -1;           
            goto exit_label;
        }
        tsection->keys = lwHashCreate(vosLwStringHash, vosLwEqualString, lwConfigKeyDestroy, 0,HS_KEY);
        tsection->flag = CFG_NOT_FLUSHED;
		hashkey=vosAlloc(strlen(section)+1);
		if(hashkey==NULL)
		{
			rv = -1;		   
	       goto exit_label; 
		}
		strcpy(hashkey,section);
        lwHashInsert(hashkey, tsection, tmodule->sections);
    }

exit_label:
     __cfg_write_unlock(g_iLockfd);
    return rv;
}


int lwConfigSectionGetByIndex(	CFG_DOMAIN_E enDomain,char *ifm,char *fname, int index, char **section)
{
	CFG_IFM_DATA_t *tifm;
    CFG_MODULE_DATA_t *tmodule;
    void *tsection = NULL;
    int rv = -1;

    
	__cfg_read_lock(&g_iLockfd);
	if(enDomain >=CONFIG_MAX_DOMAINS)
	{
		goto exit_label;
	}
	if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
	{
		goto exit_label;
	}
	tifm = (CFG_IFM_DATA_t *)lwHashValue(ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	if (tifm == NULL)
	{
		goto exit_label;
	}

	tmodule = (CFG_MODULE_DATA_t *)lwHashValue(fname, tifm->modules);
	if (tmodule == NULL)
	{
		goto exit_label;
	}   

    rv = lwHashValueGetByIndex(index, tmodule->sections, (void **)section,(void **)(&tsection));
   // rv = lwHashValueGetByIndex(index, tmodule->sections, (void **)section, NULL);

exit_label:
     __cfg_read_unlock(g_iLockfd);
    return rv;
}

int lwConfigKeyGetByIndex(CFG_DOMAIN_E enDomain,char *ifm,
    char *fname,
    char *section,
    int index,
    char **key,
    char **value)
{
	CFG_IFM_DATA_t *tifm;
    CFG_MODULE_DATA_t *tmodule;
    CFG_SECTION_DATA_t *tsection;
//    CFG_KEY_DATA_t *tkey;
    int rv = -1;

    __cfg_read_lock(&g_iLockfd);
    
	if(enDomain >=CONFIG_MAX_DOMAINS)
	{
		goto exit_label;
	}
	if (g_pstcfgdb->pstConfigDatabase[enDomain] == NULL)
	{
		goto exit_label;
	}
	tifm = (CFG_IFM_DATA_t *)lwHashValue(ifm, g_pstcfgdb->pstConfigDatabase[enDomain]);
	if (tifm == NULL)
	{
		goto exit_label;
	}
    

    tmodule = (CFG_MODULE_DATA_t *)lwHashValue(fname, tifm->modules);
    if (tmodule == NULL)
    {
        goto exit_label;
    }

    tsection = (CFG_SECTION_DATA_t *)lwHashValue(section, tmodule->sections);
    if (tsection == NULL)
    {
        goto exit_label;
    }

    rv = lwHashValueGetByIndex(index, tsection->keys, (void **)key, (void **)value);

exit_label:
    __cfg_read_unlock(g_iLockfd);
    return rv;
}
void lwConfigOidDestroy(void *key, void *value)
{
    char *k = key;  
	CFG_OID_REG_t *v=value;
    vosFree(k);
	//vosFree(v->data);
    vosFree(v);
}
#define ISHIGHER(a,b) ((a)<(b))
#define ISLOWER(a,b) ((a)>(b))  
#define ISEQUAL(a,b)  ((a)==(b))

int vosconfigSortOidTbl(void)
{
	int i =0;
	int rv=0;
	CFG_OID_REG_t *tOid=NULL;
    CFG_OID_REG_t *pstPos = NULL;
    CFG_OID_REG_t *pstNext = NULL; 
	void *hashkey=NULL;
	/*not in share mem*/
	if(g_pstOidRegHashTbl==NULL)
	g_pstOidRegHashTbl=lwHashCreate(vosLwStringHash,vosLwEqualString,lwConfigOidDestroy,0,HS_OID);
	
	for(i=0;i<(sizeof(g_cfg_oid_tbl)/sizeof(g_cfg_oid_tbl[0]));i++)
	{

	
		tOid = malloc(sizeof(CFG_OID_REG_t));
//        ASSERT(tOid != NULL, "Out of memory!!!");
		/*oid name hash tbl*/
        if (tOid == NULL) {
            rv = -1;           
            goto exit_label;
        }        
        tOid->data=&g_cfg_oid_tbl[i];		
		hashkey=malloc(strlen(g_cfg_oid_tbl[i].name)+1);
		if(hashkey==NULL)
		{
			rv = -1;		   
	       goto exit_label; 
		}
		strcpy(hashkey,g_cfg_oid_tbl[i].name);
        lwHashInsert(hashkey, tOid, g_pstOidRegHashTbl);
		tOid = malloc(sizeof(CFG_OID_REG_t));
		if (tOid == NULL) {
            rv = -1;           
            goto exit_label;
        }    
		/*oid list tbl*/
		tOid->data=&g_cfg_oid_tbl[i];
        
		if(list_empty(&g_OidRegList))
	    {	        
	        list_add_tail(&(tOid->list),&g_OidRegList);	        
	    }
		else
		{
			list_for_each_entry_safe(pstPos,pstNext,&g_OidRegList,list)
    		{
		        if(ISLOWER(tOid->data->uiCmoid,pstPos->data->uiCmoid)||ISEQUAL(tOid->data->uiCmoid,pstPos->data->uiCmoid))
		        {
		   		    if( &pstNext->list != &g_OidRegList)
		            {
		                continue;
		            }
		            else
		            {  /*已经是最后一个节点，把插入节点放之于后*/                 
		               list_add(&(tOid->list),&(pstPos->list));                     
                       break;
		            }
		        }
		        else if(ISHIGHER(tOid->data->uiCmoid,pstPos->data->uiCmoid))
		        { 
		            /*插入到pstPos位置前面*/
		            list_add_tail(&(tOid->list),&(pstPos->list));                     
                    break;
		        }   
	    	}  
		}
	}	
	
exit_label:	
	return rv;
}

extern char g_szTestShareBuf[];
// char g_szTestShareBuf[];
int Cfginit(int flag)
{
	int iShmId = 0;
    void *pStart = ( void *)g_szTestShareBuf;
	master_flag=flag;
	g_iLockfd=open(VOSCONFIG_SHMLOCKFILE, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    if(g_iLockfd<0)
    {
        return -1;
    }

    iShmId = shmget(VOSCFG_SHM_KEY,VOSCFG_SHM_SIZE,IPC_CREAT);
    if(-1 == iShmId)
    { 
        iShmId = shmget(VOSCFG_SHM_KEY,0,0);
        if(-1 == iShmId)
        {
        	close(g_iLockfd);
            return -1;
        }       
    }     
	__cfg_write_lock(&g_iLockfd);
	g_pstcfgdb =(CFG_DATABASE_S *)shmat(iShmId,pStart,0);
	if(isMaster()==MASTER||isMaster()==TESTER)
	{
		int i=0;
		memset(pStart,0,VOSCFG_SHM_SIZE);
		pool_create((unsigned char *)((unsigned char*)pStart+sizeof(CFG_DATABASE_S)),(VOSCFG_SHM_SIZE-sizeof(CFG_DATABASE_S)));
		for(i=CONFIG_NORMAL;i<CONFIG_MAX_DOMAINS;i++)
		{
		    if (g_pstcfgdb->pstConfigDatabase[i]==NULL) 
			{		        
				g_pstcfgdb->pstConfigDatabase[i] = lwHashCreate(
						   vosLwStringHash,
						   vosLwEqualString,
						   lwConfigIfDestroy,
						   0,HS_INTF);		        
		    }
		}
	
	}
	__cfg_write_unlock(g_iLockfd);
	
	vosconfigSortOidTbl();	
//    printf("success\n");
	return 0;
}
#undef CONFIG_CHECK_VER


static int cfg_cfgfile_check_md5(char *infile, char *outfile)
{
        FILE* fpin = NULL;
        FILE* fpout = NULL;
        char hash_val[64]= {0};
        char hash_val_new[64]={0};
        char buf[2048] = {0};
        char cmd[64] = {0};

        fpin = fopen(infile,"r");
        if(NULL == fpin)
        {
            return -1;
        }
        fpout = fopen(outfile,"w");
        if(NULL == fpout)
        {
            fclose(fpin);
            return -1;
        }
        /*skip first line md5*/
        while(NULL != fgets(hash_val,64,fpin))
        {
            if((hash_val[0] != '\0') && (hash_val[0] != '\r') && (hash_val[0] != '\n'))
            {
                break;
            }

        }
        while(NULL != fgets(buf,2048,fpin))
        {
            if(EOF == fputs(buf,fpout))
            {
                fclose(fpin);
                fclose(fpout);
                return -1;
            }
        }
        fclose(fpin);
        fclose(fpout);
        sprintf(cmd,"md5sum %s > "CONFIG_MD5_TMPFILE,outfile);
        if(0 > system(cmd))
        {
            unlink(CONFIG_MD5_TMPFILE);
            return -1;
        }
        fpin = fopen(CONFIG_MD5_TMPFILE,"r");
        if(NULL == fpin)
        {
            unlink(CONFIG_MD5_TMPFILE);
            return -1;
        }
        fgets(hash_val_new,64,fpin);
        fclose(fpin);
        unlink(CONFIG_MD5_TMPFILE);
       // printf("hash_val=%s,hash_val_new=%s\n",hash_val,hash_val_new);
        if(0 != strncmp(hash_val,hash_val_new, MD5_LEN))
        {
            return -1;
        }
        return 0;
}

static  int __cfg_checkfile( char * pPath)
{
    FILE *fp=NULL;
    char szBuf[1024];
    char *p=NULL;
    char szMd5[256];
    int i=0;
    char *pcTemp = NULL;
    #ifdef CONFIG_CHECK_VER
    char *tmp=NULL;
    #endif
    if(pPath==NULL)
    {       
        return -1;
    }
    /*出厂文件不做检查*/
    if(strcmp(pPath,CONFIG_DEFAULTFILE)==0)
    {
        return 0;
    }
    fp=fopen(pPath,"r");
    if(fp==NULL)
    {       
        return -1;
    }
    memset(szBuf,0,sizeof(szBuf));

    p=fgets(szBuf,sizeof(szBuf),fp);
    if(p==NULL)
    {       
        fclose(fp);
        return -1;
    }
    memset(szMd5,0,sizeof(szMd5));
    while(szBuf[i]==' '||szBuf[i]==TAB)
    {
        i++;
    }
    pcTemp=szBuf+i;
    while( *pcTemp!='\r' && *pcTemp!= '\n' && *pcTemp!='\0' )
    {
        pcTemp++;
    }
    *pcTemp='\0';
    strcpy(szMd5, szBuf+i);
    memset(szBuf,0,sizeof(szBuf));
    p=fgets(szBuf,sizeof(szBuf),fp);
    if(p==NULL)
    {
        
        fclose(fp);
        return -1;
    }

    p=strchr(szBuf,'/');
    if(p==NULL)
    {        
        fclose(fp);
        return -1;
    }
    fclose(fp);
    if((0 != strcmp(szMd5,"nomd5"))&&(0!=cfg_cfgfile_check_md5(pPath,CONFIG_TMP_FILE)))
    {        
        remove(CONFIG_TMP_FILE);
        return -1;
    }
    remove(CONFIG_TMP_FILE);
    *p='\0';
    p++; 
    #ifdef CONFIG_CHECK_VER
    if(strcmp(PRODUCT_NAME,szBuf)!=0)
    {       
        return -1;
    }   
    tmp=p;
    while( *tmp!='\r' && *tmp!= '\n' && *tmp!='\0' )
    {
        tmp++;
    }
   *tmp='\0';

    if(0<__cfgver_cmp(p,CONFIG_CUST_DEVELOP_VER_STR))
    {
        
        return -1;
    }
    #endif
    return 0;
}

int lwConfigParserFile(char * pPath,CFG_DOMAIN_E enDomain)
{
    FILE *fp=NULL;
    char szBuf[2048]={0};
    char szVal[1024]={0};
    char *pcRet;
  //  int iRet=-1; 
    int findmodule=FALSE;
    int findsection=FALSE;
//    int  freeindex=-1;
    unsigned int curkeyoid=0;
	unsigned int cursecoid=0;
	unsigned int curmidoid=0;
    char *tmp=NULL;
    int i=0;
    char curif[128]={0};
	char curmodule[128]={0};
	char cursection[128]={0};
	char curkey[128]={0};
	
    if(pPath == NULL)
    {
    	DBG_PRINTF("%s %d\n",__FUNCTION__,__LINE__);
        return -1;
    }
    
    if(0 !=__cfg_checkfile(pPath))
    {
    	DBG_PRINTF("check %s faild!\n", pPath);
        return -1;
    }
	
	
    fp=fopen(pPath,"r");
    if(fp == NULL)
    {  
    	DBG_PRINTF("open %s faild!\n", pPath);
        return -1;
    }
    memset(szBuf,0,sizeof(szBuf));
    memset(szVal,0,sizeof(szVal));
    /*读取md5行*/
    pcRet=fgets(szBuf,sizeof(szBuf),fp);
    if(pcRet==NULL)
    {    
    	DBG_PRINTF("get md5  faild!\n");
        fclose(fp);
        return -1;
    }
    /*读取版本行*/
    pcRet=fgets(szBuf,sizeof(szBuf),fp);
    if(pcRet==NULL)
    {      
    	DBG_PRINTF("get version  faild!\n");
        fclose(fp);
        return -1;
    }
    pcRet=fgets(szBuf,sizeof(szBuf),fp);
    
    while(pcRet!=NULL)
    {
        i=0;
        while(szBuf[i]==' '||szBuf[i]==TAB)
        {
            i++;
        }
        switch(szBuf[i])
        {
            case '$':
                tmp=szBuf+i+1;
                while( *tmp!='\r' && *tmp!= '\n' && *tmp!='\0' )
                {
                    tmp++;
                }
                *tmp='\0';			
				memset(curif,0,sizeof(curif));
				strcpy(curif,(szBuf+i+1));
				findsection=FALSE;
				findmodule=FALSE;
                break;
            case '@':
                tmp=szBuf+i+1;
                while( *tmp!='\r' && *tmp!= '\n' && *tmp!='\0' )
                {
                    tmp++;
                }
                *tmp='\0';				
				memset(curmodule,0,sizeof(curmodule));
				strcpy(curmodule,(szBuf+i+1));
				if(-1==lwConfigName2Oid(curmodule,&curmidoid))
				{
					break;
				}
				findmodule=TRUE;
                break;
            case '#':                
				tmp=szBuf+i+1;
                while( *tmp!='\r' && *tmp!= '\n' && *tmp!='\0' )
                {
                    tmp++;
                }
                *tmp='\0';
				memset(cursection,0,sizeof(cursection));
				strcpy(cursection,(szBuf+i+1));
				if(-1==lwConfigName2Oid(cursection,&cursecoid))
				{
					break;
				}
				if(CONFIG_MID(cursecoid)!=curmidoid)
				{
					DBG_PRINTF("module oid unmatched!\n");
					fclose(fp);
					return -1;
				}
                findsection=TRUE;				
                break;
            default:               
                if(findsection==FALSE || findmodule==FALSE)
                {
                	DBG_PRINTF("module or section not found!\n");
                    fclose(fp);
                    return -1;
                }
                else
                {
                    char *p=NULL;                
                    p=strchr((szBuf+i),'=');
                    if(p==NULL)
                    {    
                    	DBG_PRINTF("miss keyname at the left of = !\n");
                        fclose(fp);
                        return -1;
                    }
                    *p='\0';
					if(-1==lwConfigName2Oid((szBuf+i),&curkeyoid))
					{
						break;
					}
					strcpy(curkey,(szBuf+i));
					if(CONFIG_MID_SECTION(curkeyoid)!=CONFIG_MID_SECTION(cursecoid))
					{
						DBG_PRINTF("section[%s] oid unmatched! [%02x:%02x]\n",(szBuf+i),curkeyoid,cursecoid);
						fclose(fp);
						return -1;
					}					
                    p++;              
                    tmp=p;
                    while( *tmp!='\r' && *tmp!= '\n' && *tmp!='\0' )
                    {
                        tmp++;
                    }
                    *tmp='\0';
                    memset(szVal,0,sizeof(szVal));
                    strcpy(szVal,p);					
					lwConfigValueSet(enDomain,(const unsigned char *)curif,(const unsigned char *) curmodule,(const unsigned char *) cursection,(const unsigned char *) curkey,(const unsigned char *) szVal);
                }               
                break;

        }
        memset(szBuf,0,sizeof(szBuf));
        pcRet=fgets(szBuf,sizeof(szBuf),fp);
    }    
    fclose(fp);    
    return 0;
}


int lwConfigName2Oid(char * name,unsigned int *oid)
{
	CFG_OID_REG_t *tOid=NULL;
	int iLen=0;
	int iDigit=0,i=0;
	unsigned int uiNum=0;
	unsigned char tmpname[128];
	if(name==NULL || oid==NULL)
	{
		return -1;
	}
	iLen = (int)strlen(name);
    if(0 == iLen )
    {   	
        return -1;
    }
     /*now:the last CONFIG_IDX_HEXBIT bits*/
    for(iDigit=0,i=1;i<=CONFIG_IDX_HEXBIT;i++,iDigit++)
    {
        if (((iLen-i)<0) || (!isdigit(name[iLen-i])))
        {
            break;
        }
    }
    if((iLen-i)<=0)
    {      	
        return -1;
    }
	if(iDigit>0)
	{
		uiNum = (UINT)atoi(name+iLen-iDigit);
		if( (uiNum < CONFIG_MIN_INDEX) || (uiNum > CONFIG_MAX_INDEX))
		{			
			return -1;
		}
	}
	memset(tmpname,0,sizeof(tmpname));
	strncpy((char *)tmpname,name,iLen-iDigit);
	tOid=(CFG_OID_REG_t *)lwHashValue(tmpname,g_pstOidRegHashTbl);
	if(tOid!=NULL)
	{
		if((iDigit>0)&&(tOid->data->enNode_type!=CFG_OID_TYPE_TABLE))
		{
			return -1;
		}
		*oid=tOid->data->uiCmoid+uiNum;		
		return 0;
	}
	return -1;
}
CFG_OID_REG_t * lwConfigOid2name(unsigned int oid,unsigned char * name,unsigned int namelen)
{
	CFG_OID_REG_t *pstPos = NULL;
	if(name==NULL || namelen==0)
	{
		return NULL;
	}
	list_for_each_entry(pstPos,&g_OidRegList,list)
    {      
		if(pstPos->data->uiCmoid==oid)
		{
			if(strlen(pstPos->data->name)<namelen)
			{
				strcpy((char *)name,pstPos->data->name);
				return pstPos;
			}
		}
	}
	return NULL;
}

/*external api*/
/*depend on ifm ,just stub*/
//#define IF_DOMAIN_MASK          0x7

//#define IF_DOMAIN(ifidx)        (CFG_DOMAIN_E)(((ifidx)>>21)&IF_DOMAIN_MASK)
//#define IF_DOMAIN(ifidx)        ifidx

int ifindex2ifname(int ifindex,unsigned char *name,int namelen)
{
    char buf[BUF_SIZE64];
	int ret=0;
	if(namelen==0||name==NULL)
	{
		return -1;
	}
	/*depend on ifm,now just return root*/
    memset(buf,0,sizeof(buf));
    ret=IF_GetAttr(ifindex,IF_NAME,buf,sizeof(buf));	
	strcpy((char *)name,buf);
	return 0;
}
/*master api*/
int master_cfg_setval(int ifindex,unsigned int oid,void *val)
{
	unsigned char ifname[64]={0};
	unsigned char midname[128]={0};
	unsigned char secname[128]={0};
	unsigned char tmpsecname[128]={0};
	unsigned char keyname[128]={0};
	CFG_OID_REG_t *pstSecinfo = NULL;
	CFG_OID_REG_t *pstKeyinfo = NULL;
	unsigned int val_len=0;
	unsigned char buf[OP_VOS_CONFIG_STR_MAX]={0};
	
	if((isMaster()!=TESTER)&&(isMaster()!=MASTER))
	{
		return -1;
	}
	if(val==NULL)
	{
		return -1;
	}
	if(0!=ifindex2ifname(ifindex,ifname,sizeof(ifname)))
	{	
		return -1;
	}
	if(lwConfigOid2name(CONFIG_MID(oid),midname,sizeof(midname))==NULL)
	{		
		return -1;
	}
	if((pstSecinfo=lwConfigOid2name(CONFIG_MID_SECTION(oid),tmpsecname,sizeof(tmpsecname)))==NULL)
	{		
		return -1;
	}
	if((pstKeyinfo=lwConfigOid2name(CONFIG_MID_SECTION_KEY(oid),keyname,sizeof(keyname)))==NULL)
	{		
		return -1;
	}
	if((pstSecinfo->data->enNode_type==CFG_OID_TYPE_TABLE)&&((CONFIG_INDEX(oid)<CONFIG_MIN_INDEX)||(CONFIG_INDEX(oid)>CONFIG_MAX_INDEX)))
	{		
		return -1;
	}
	if(pstSecinfo->data->enNode_type==CFG_OID_TYPE_TABLE)
	sprintf((char *)secname,"%s%d",tmpsecname,CONFIG_INDEX(oid));
	else
	strcpy((char *)secname,(const char *)tmpsecname);
	/*convert value to str*/
	
	switch ( pstSecinfo->data->type)
    {
	   case CFG_TYPE_STR:
				val_len = strlen(val);	
				if(val_len>OP_VOS_CONFIG_STR_MAX)
				{					
					return -1;
				}
				strncpy((char *)buf,val,OP_VOS_CONFIG_STR_MAX);
				break;			 
	   case CFG_TYPE_INT:
	   			sprintf((char *)buf,"%u",*(unsigned int *)val);
				break;
	   case CFG_TYPE_IPV4:			   			
				inet_ntop(AF_INET,val,(char *)buf,sizeof(buf));
				break;	   
	   case CFG_TYPE_IPV6:					    
				inet_ntop(AF_INET6,val,(char *)buf,sizeof(buf));
			    break;	 
		default:
			{			
	    return  -1;
			}
	}	
	return lwConfigValueSet(IF_DOMAIN(ifindex),ifname,midname,secname,keyname,buf);
	
	
}
int master_cfg_del(int ifindex,unsigned int oid)
{
	unsigned char ifname[64]={0};
	unsigned char midname[128]={0};
	unsigned char secname[128]={0};
	unsigned char tmpsecname[128]={0};
	CFG_OID_REG_t *pstSecinfo = NULL;
	unsigned char keyname[128]={0};
	CFG_OID_REG_t *pstKeyinfo = NULL;

	if((isMaster()!=TESTER)&&(isMaster()!=MASTER))
	{
		return -1;
	}
	if(0!=ifindex2ifname(ifindex,ifname,sizeof(ifname)))
	{
		return -1;
	}
	if(IS_ROOTCONFIG(oid))
	{	   
		lwConfigIfDelete(IF_DOMAIN(ifindex),ifname);
		return 0;
	}
	if(lwConfigOid2name(CONFIG_MID(oid),midname,sizeof(midname))==NULL)
	{
		return -1;
	}
	if(IS_MIDCONFIG(oid))
	{	    
		lwConfigModuleDelete(IF_DOMAIN(ifindex),ifname,midname);
		return 0;
	}
	if((pstSecinfo=lwConfigOid2name(CONFIG_MID_SECTION(oid),tmpsecname,sizeof(tmpsecname)))==NULL)
	{
		return -1;
	}
	if((pstSecinfo->data->enNode_type==CFG_OID_TYPE_TABLE)&&((CONFIG_INDEX(oid)<CONFIG_MIN_INDEX)||(CONFIG_INDEX(oid)>CONFIG_MAX_INDEX)))
	{
		return -1;
	}
	if(pstSecinfo->data->enNode_type==CFG_OID_TYPE_TABLE)
	sprintf((char *)secname,"%s%d",tmpsecname,CONFIG_INDEX(oid));
	else
	strcpy((char *) secname,(const char *)tmpsecname);

	
	if(IS_SECTIONCONFIG(oid))
	{	   
		lwConfigSectionDelete(IF_DOMAIN(ifindex),ifname,midname,secname);
		return 0;
	}
	if((pstKeyinfo=lwConfigOid2name(CONFIG_MID_SECTION_KEY(oid),keyname,sizeof(keyname)))==NULL)
	{
		return -1;
	}    
	return lwConfigKeyDelete(IF_DOMAIN(ifindex),ifname,midname,secname,keyname);   
}

int cfg_getval(int ifindex,unsigned int oid,void *val,void* default_val,unsigned retlen)
{
	unsigned char ifname[64]={0};
	unsigned char midname[128]={0};
	unsigned char secname[128]={0};
	unsigned char tmpsecname[128]={0};
	unsigned char keyname[128]={0};
	CFG_OID_REG_t *pstSecinfo = NULL;
	CFG_OID_REG_t *pstKeyinfo = NULL;
//	int ret;
	unsigned char buf[OP_VOS_CONFIG_STR_MAX]={0};
	
	if(val==NULL)
	{
		return -1;
	}
	
	if(0!=ifindex2ifname(ifindex,ifname,sizeof(ifname)))
	{
		DBG_PRINTF("ifindex2ifname faild!");
		return -1;
	}
	if(lwConfigOid2name(CONFIG_MID(oid),midname,sizeof(midname))==NULL)
	{
		DBG_PRINTF("lwConfigOid2name faild!");
		return -1;
	}
	if((pstSecinfo=lwConfigOid2name(CONFIG_MID_SECTION(oid),tmpsecname,sizeof(tmpsecname)))==NULL)
	{
		DBG_PRINTF("lwConfigOid2name faild!");
		return -1;
	}
	if((pstKeyinfo=lwConfigOid2name(CONFIG_MID_SECTION_KEY(oid),keyname,sizeof(keyname)))==NULL)
	{	     
		DBG_PRINTF("lwConfigOid2name faild %02x!",CONFIG_MID_SECTION_KEY(oid));
		return -1;
	}
	if((pstSecinfo->data->enNode_type==CFG_OID_TYPE_TABLE)&&((CONFIG_INDEX(oid)<CONFIG_MIN_INDEX)||(CONFIG_INDEX(oid)>CONFIG_MAX_INDEX)))
	{
		DBG_PRINTF("CFG_OID_TYPE_TABLE index faild!");
		return -1;
	}
    
	if(pstSecinfo->data->enNode_type==CFG_OID_TYPE_TABLE)
	sprintf((char *)secname,"%s%d",tmpsecname,CONFIG_INDEX(oid));
	else
	strcpy((char *)secname,(const char *)tmpsecname);

	//printf("midname = %s secname= %s keyname = %s \n", midname, secname, keyname);
	if(0!=lwConfigValueGet(IF_DOMAIN(ifindex),(const unsigned char *)ifname,(const unsigned char *)midname,(const unsigned char *)secname,(const unsigned char *)keyname,default_val,buf,sizeof(buf)))
	{
		DBG_PRINTF("lwConfigValueGet %s %s %s %s faild!",ifname,midname,secname,keyname);        
		return -1;
	}
	
	switch ( pstSecinfo->data->type)
    {
    	case CFG_TYPE_STR:
			if(retlen<=strlen((char *)buf))
			{
				DBG_PRINTF("CFG_TYPE_STR faild!");
				return -1;
			}
			strcpy((char *)val,(const char *)buf);
           
			break;
		case CFG_TYPE_INT:
			if(retlen<sizeof(int))
			{
				DBG_PRINTF("CFG_TYPE_IPV4 faild!");
				return -1;
			}
			int idata;
			idata = atoi((const char *)buf);
			memcpy(val, &idata, sizeof(int));
			break;
		case CFG_TYPE_IPV4:	
			if(retlen<4)
			{
				DBG_PRINTF("CFG_TYPE_IPV4 faild!");
				return -1;
			}
			inet_pton(AF_INET,(char *)buf,val);
			break;
		case CFG_TYPE_IPV6:	
			if(retlen<4)
			{
				DBG_PRINTF("CFG_TYPE_IPV6 faild!");
				return -1;
			}
			inet_pton(AF_INET6,(char *)buf,val);
			break;
		default:
			return -1;
	}
	return 0;
}
static int lwConfigBackupForEachKey(void *key, void *value, int dst,void *ifm,void *mid,void *section)
{
	unsigned int uiCmoid=0;
	unsigned char name[128];
    CFG_KEY_DATA_t *tkey = (CFG_KEY_DATA_t *)value;
	CFG_OID_REG_t * tOidifo=NULL;
	
    if (tkey && tkey->data)
    {        
        lwConfigName2Oid(key,&uiCmoid);
		tOidifo=lwConfigOid2name(CONFIG_MID_SECTION_KEY(uiCmoid),name,sizeof(name));
		if(tOidifo->data->iRestore==1)
        lwConfigValueSet((CFG_DOMAIN_E)dst,(const unsigned char  *)ifm,(const unsigned char  *)mid,(const unsigned char  *)section,(const unsigned char  *)key,(const unsigned char *)(tkey->data));
    }
    return 0;
}

static int lwConfigBackupForEachSection(void *key, void *value, int dst,void *ifm,void *mid)
{
    CFG_SECTION_DATA_t *tsection = (CFG_SECTION_DATA_t *)value;   
    if (tsection && tsection->keys) {
        lwHashForEachDoCb(tsection->keys,
            (FUNCPTR)lwConfigBackupForEachKey, (unsigned int)dst, (unsigned int)ifm,(unsigned int) mid,(unsigned int) key, 7, 8);
    }
    return 0;
}

static int lwConfigBackupForEachModule(void *key, void *value, int dst,void *ifm)
{
    CFG_MODULE_DATA_t *tmodule = (CFG_MODULE_DATA_t *)value;

    /* strip the default path */
   
    if (tmodule && tmodule->sections) {
        lwHashForEachDoCb(tmodule->sections,
            (FUNCPTR)lwConfigBackupForEachSection, (unsigned int)dst, (unsigned int)ifm, (unsigned int)key, 6, 7, 8);
    }
    return 0;
}

static void lwConfigBackupForEachIf(void *key, void *value, int dst)
{	
	CFG_IFM_DATA_t  *tifm=(CFG_IFM_DATA_t*)value;	

    if (tifm && tifm->modules)
    {
        lwHashForEachDoCb(tifm->modules,
        (FUNCPTR)lwConfigBackupForEachModule,(unsigned int)dst,(unsigned int)key, 5, 6, 7, 8);
    }	
	return ;
}

static void lwConfigbackup(CFG_DOMAIN_E dst,CFG_DOMAIN_E src)
{
	__cfg_write_lock(&g_iLockfd);
	if (g_pstcfgdb->pstConfigDatabase[src])
    {
        lwHashForEachDoCb(g_pstcfgdb->pstConfigDatabase[src],
            (FUNCPTR)lwConfigBackupForEachIf, (unsigned int)dst, 4, 5, 6, 7, 8);
    }	
	__cfg_write_unlock(g_iLockfd);
}

/*spec1: restore default 
 *spec2: backup no-factory config to flash and restore default
 * use macro to split spec1 and spec2
 */
int CfgRestoreFactory(char *file)
{
	char cmd[256] = {0};
	
	if((isMaster()!=TESTER)&&(isMaster()!=MASTER))
	{
		return -1;
	}
	
	lwConfigDomainDelete(CONFIG_VIRTUAL);
	
	lwConfigParserFile(file,CONFIG_VIRTUAL);

	lwConfigbackup(CONFIG_VIRTUAL,CONFIG_NORMAL);
	
	lwConfigFlushReset(CONFIG_VIRTUAL);
	lwConfigFlush(CONFIG_VIRTUAL,CONFIG_DEFAULTSTART);
	
	
	sprintf(cmd, "cp %s %s", CONFIG_DEFAULTSTART, CONFIG_MNT_START);
	if(system(cmd) < 0)
	{
		DBG_PRINTF("copy to mnt faild!\n");
		return -1;
	}

	//reboot();
	return 0;
}
/* master start to load config.
* if load config fail,just restore to factory
* if 
*/

int CfgStart(void)
{
	char cmd[256] = {0};
	
	if((isMaster()!=TESTER)&&(isMaster()!=MASTER))
	{
		return -1;
	}
	
	/*配置加载失败，则加载默认配置*/
	if(lwConfigParserFile(CONFIG_MNT_START, CONFIG_VIRTUAL) != 0)
	{
		DBG_PRINTF("load file %s to CONFIG_NORMAL faild! \nnow to load factory config!\n", CONFIG_MNT_START);
		if(lwConfigParserFile(CONFIG_DEFAULTFILE, CONFIG_VIRTUAL) != 0)
		{
			DBG_PRINTF("load file %s to CONFIG_NORMAL faild!\n", CONFIG_DEFAULTFILE);
			return -1;
		}
		else
		{
			if(lwConfigParserFile(CONFIG_DEFAULTFILE, CONFIG_NORMAL) != 0)
			{
				DBG_PRINTF("load file %s to CONFIG_VIRTUAL faild!\n", CONFIG_DEFAULTFILE);
				return -1;
			}
		}

		lwConfigFlushReset(CONFIG_VIRTUAL);
		if(lwConfigFlush(CONFIG_VIRTUAL, CONFIG_DEFAULTSTART) < 0)
		{
			DBG_PRINTF("save config to file %s faild!\n", CONFIG_DEFAULTSTART);
			return -1;
		}
		
		sprintf(cmd, "cp %s %s", CONFIG_DEFAULTSTART, CONFIG_MNT_START);
		if(system(cmd) < 0)
		{
			DBG_PRINTF("copy to mnt faild!\n");
			return -1;
		}
        lwConfigDomainDelete(CONFIG_VIRTUAL);
	}
	else
	{
		if(lwConfigParserFile(CONFIG_MNT_START, CONFIG_NORMAL) != 0)
		{
			DBG_PRINTF("load file %s to CONFIG_VIRTUAL faild!\n", CONFIG_MNT_START);
			return -1;
		}
		lwConfigDomainDelete(CONFIG_VIRTUAL);
	}
	cfg_setupif(CONFIG_NORMAL);
	__cfg_write_lock(&g_iLockfd);
	g_pstcfgdb->commitflag[CONFIG_NORMAL]=0;
	__cfg_write_unlock(g_iLockfd);
	return 0;
}
static void lwConfigSetUpForEachIf(void *key, void *value, CFG_DOMAIN_E dst)
{	
	CFG_IFM_DATA_t  *tifm=(CFG_IFM_DATA_t*)value;	
    IF_INFO_S stInfo;
    IF_RET_E ret;
    ifindex_t ifindex;
    if(key==NULL)
    {
        return ;
    }
    memset(&stInfo,0,sizeof(IF_INFO_S));
    if(strncmp("root",key,strlen("root"))==0||strncmp("sys",key,strlen("sys"))==0)
    {       
        strcpy(stInfo.szIfName,"root");
        strcpy(stInfo.szPseudoName,"root");
        stInfo.enSubType=IF_SUB_ROOT;
        stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,dst,0);
        stInfo.uiSpecIndex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,dst,0);        
        ret=IF_CreatInterface(&stInfo,&ifindex);
        if(ret!=IF_OK)
        {
            printf("fatal error ,create root if failed\n");
        }
    }else if(strncmp(IF_L3VLAN_NAMEPREFIX,key,strlen(IF_L3VLAN_NAMEPREFIX))==0)
    {
    
        strcpy(stInfo.szIfName,key);
        strcpy(stInfo.szPseudoName,key);
        stInfo.enSubType=IF_SUB_L3VLAN;
        stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,dst,0);
        stInfo.uiSpecIndex=IF_INVALID_IFINDEX;
        
        ret=IF_CreatInterface(&stInfo,&ifindex);
        if(ret!=IF_OK)
        {
            printf("fatal error ,create %s if failed\n",key);
        }
    }
    else if((strncmp(IF_ETH_NAMEPREFIX,key,strlen(IF_ETH_NAMEPREFIX))==0)&&(strncmp(IF_ETHPORT_NAMEPREFIX,key,strlen(IF_ETHPORT_NAMEPREFIX))!=0))
    {
        strcpy(stInfo.szIfName,key);
        strcpy(stInfo.szPseudoName,key);
        stInfo.enSubType=IF_SUB_ETH;
        stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,dst,0);
        stInfo.uiSpecIndex=IF_INVALID_IFINDEX;
        
        ret=IF_CreatInterface(&stInfo,&ifindex);
        if(ret!=IF_OK)
        {
            printf("fatal error ,create %s if failed\n",key);
        }
    }
	else if(strncmp(IF_ETHPORT_NAMEPREFIX,key,strlen(IF_ETHPORT_NAMEPREFIX))==0)
    {
        strcpy(stInfo.szIfName,key);
        strcpy(stInfo.szPseudoName,key);
        stInfo.enSubType=IF_SUB_ETHPORT;
        stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,dst,0);
        stInfo.uiSpecIndex=IF_INVALID_IFINDEX;
        
        ret=IF_CreatInterface(&stInfo,&ifindex);
        if(ret!=IF_OK)
        {
            printf("fatal error ,create %s if failed\n",key);
        }
    }
	else if(strncmp(IF_AGGR_NAMEPREFIX,key,strlen(IF_AGGR_NAMEPREFIX))==0)
    {
        strcpy(stInfo.szIfName,key);
        strcpy(stInfo.szPseudoName,key);
        stInfo.enSubType=IF_SUB_LAG;
        stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,dst,0);
        stInfo.uiSpecIndex=IF_INVALID_IFINDEX;
        
        ret=IF_CreatInterface(&stInfo,&ifindex);
        if(ret!=IF_OK)
        {
            printf("fatal error ,create %s if failed\n",key);
        }
    }
	else 
    {
         printf("fatal error ,unkown  ..if [%s]\n",key);
    }
	return ;
}

void cfg_setupif(CFG_DOMAIN_E domain)
{
   // __cfg_write_lock(&g_iLockfd);
    __cfg_read_lock(&g_iLockfd);
    if (g_pstcfgdb->pstConfigDatabase[domain])
    {
        lwHashForEachDoCb(g_pstcfgdb->pstConfigDatabase[domain],
            (FUNCPTR)lwConfigSetUpForEachIf, domain, 4, 5, 6, 7, 8);
    }	
    //__cfg_write_unlock(g_iLockfd);
     __cfg_read_unlock(g_iLockfd);
}
void save_config_to_file()
{
	char cmd[256] = {0};

	lwConfigFlushReset(CONFIG_VIRTUAL);
	remove(CONFIG_DEFAULTSTART);
	if(lwConfigFlush(CONFIG_VIRTUAL, CONFIG_DEFAULTSTART) < 0)
	{
		DBG_PRINTF("save to file %s faild!\n", CONFIG_DEFAULTSTART);
		return;
	}
	
	sprintf(cmd, "cp %s %s", CONFIG_DEFAULTSTART, CONFIG_MNT_START);
	if(system(cmd) < 0)
	{
		DBG_PRINTF("copy to mnt faild!\n");
		return;
	}
	/*保持NORMAL 域中的配置与flash 中的配置相同*/
	lwConfigDomainDelete(CONFIG_NORMAL);
	lwConfigDomainDelete(CONFIG_VIRTUAL);
	lwConfigParserFile(CONFIG_DEFAULTSTART, CONFIG_NORMAL);
	remove(CONFIG_DEFAULTSTART);
	return;
}


int master_cfg_backup(const char *file);
void master_cfg_do_commit(void)
{
	//save_config_to_file();
	master_cfg_backup(CONFIG_MNT_START);
	return;
}

/*import a config file to config domain*/
int cfg_restore_for_file(char *file)
{
	int ret=0;

	ret=lwConfigDomainDelete(CONFIG_VIRTUAL);
	if(ret!=0)
	{
	   return ret;
	}
	ret=lwConfigParserFile(file,CONFIG_VIRTUAL);
	if(ret!=0)
	{
	   return ret;
	}

	save_config_to_file();
	return ret;	
	
}

/*save current config to file*/
int master_cfg_backup(const char *file)
{
	lwConfigFlushReset(CONFIG_NORMAL);
	if(lwConfigFlush(CONFIG_NORMAL, (char *)file) < 0)
	{
		DBG_PRINTF("save to file %s faild!\n", file);
		return -1;
	}

	return 0;
}
int master_cfg_getcommitflag(ifindex_t  ifindex)
{
	int flag=0;
	__cfg_read_lock(&g_iLockfd);
	flag=g_pstcfgdb->commitflag[IF_DOMAIN(ifindex)];
	 __cfg_read_unlock(g_iLockfd);
	 return flag;
}
