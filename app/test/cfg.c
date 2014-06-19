#include <stdio.h>
#include <string.h>
#include <lw_type.h>
#include <lw_if_pub.h>
#include <lw_config_api.h>
#include <lw_config_oid.h>

extern int lwConfigName2Oid(char * name,unsigned int *oid);
extern int ipc_cfg_set(ifindex_t ifindex,unsigned uioid, char *value);

int cfg_main(int argc, char **argv)
{
    ifindex_t ifindex;
    IF_RET_E ret;
    UINT oid;
    char buf[1024];       
   
    memset(buf,0,sizeof(buf));
    if (argc <= 1) 
    {    
         printf("%s %d\n",__FUNCTION__,__LINE__);
         return -1;
    }
    printf("%s %d\n",__FUNCTION__,__LINE__);
    if(strcmp("backup",argv[1])==0)
    {
        printf("%s %d\n",__FUNCTION__,__LINE__);
       ipc_cfg_backup("/var/run/.cfg");
    }
    else if(strcmp("set",argv[1])==0)
    {
        if (argc < 5) 
        {    
            printf("%s %d\n",__FUNCTION__,__LINE__);
            
            return -1;
        }
        ret=IF_GetByIFName(argv[2],&ifindex);
        if(ret!=IF_OK)
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            
            return -1;
        }
        if(lwConfigName2Oid(argv[3],&oid)!=0)
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
             
            return -1;
        }     
        if(ipc_cfg_set(ifindex,oid,argv[4])!=0)
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);             
            return -1;
        }
        
    }else if(strcmp("get",argv[1])==0)
    {
        if (argc < 4) 
        {    
            printf("%s %d\n",__FUNCTION__,__LINE__);
            
            return -1;
        }
        ret=IF_GetByIFName(argv[2],&ifindex);
        if(ret!=IF_OK)
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            
            return -1;
        }
        if(lwConfigName2Oid(argv[3],&oid)!=0)
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
             
            return -1;
        }        
        if(cfg_getval(ifindex,oid,buf,NULL,sizeof(buf))!=0)
        {
             printf("%s val: %s",argv[3],buf);
            
            return -1;
        }
        printf("%s val: %s",argv[3],buf);
    } else if(strcmp("del",argv[1])==0)
    {
         if (argc < 4) 
        {    
            printf("%s %d\n",__FUNCTION__,__LINE__);
            
            return -1;
        }
         ret=IF_GetByIFName(argv[2],&ifindex);
        if(ret!=IF_OK)
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            
            return -1;
        }
        if(lwConfigName2Oid(argv[3],&oid)!=0)
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
             
            return -1;
        }        
        ipc_cfg_del(ifindex,oid);
    }
    return 0;
}
