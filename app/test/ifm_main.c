#include <stdio.h>
#include <string.h>
#include <lw_type.h>
#include <lw_if_pub.h>

int ifm_main(int argc, char **argv)
{
    IF_INFO_S stInfo;
    ifindex_t ifindex=IF_INVALID_IFINDEX;
    ifindex_t pifindex=IF_INVALID_IFINDEX;
    IF_RET_E ret;
    if (argc <= 2) 
    {    
         printf("%s %d\n",__FUNCTION__,__LINE__);
        return -1;
    }
    if(strcmp(argv[1],"create")==0)
    {
        if (argc != 3) 
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            return -1;
        }
        strcpy(stInfo.szIfName,argv[2]);
        strcpy(stInfo.szPseudoName,argv[2]);
        if(strncmp("l3vlan",argv[2],strlen("l3vlan"))==0)
        {
            stInfo.enSubType=IF_SUB_L3VLAN;
            stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_NORMAL,0);
            stInfo.uiSpecIndex=IF_INVALID_IFINDEX;
             printf("%s %d %02x\n",__FUNCTION__,__LINE__, stInfo.uiParentIfindex);
        }
        else if(strncmp("eth",argv[2],strlen("eth"))==0)
        {
            stInfo.enSubType=IF_SUB_ETH;
            stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_NORMAL,0);
            stInfo.uiSpecIndex=IF_INVALID_IFINDEX;
        }
        else if(strncmp("ethport",argv[2],strlen("ethport"))==0)
        {
            stInfo.enSubType=IF_SUB_ETHPORT;
            stInfo.uiParentIfindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_NORMAL,0);
            stInfo.uiSpecIndex=IF_INVALID_IFINDEX;
        }
        ret=IF_CreatInterface(&stInfo,&ifindex);
        if(ret!=IF_OK)
        {
            printf("create ifm [%s] fail.. %d\n",argv[2],ret);
            return -1;
        }
        if(ifindex==IF_INVALID_IFINDEX)
        {
            printf(" ifm index %s is invalid\n",argv[2]);
            return -1;
        }
        printf(" ifm [%s] create success\n",argv[2]);
        return 0;
    }
    else if(strcmp(argv[1],"delete")==0)
    {
        if (argc != 3) 
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            return -1;
        }
        ret=IF_GetByIFName(argv[2],&ifindex);
        if(ret!=IF_OK)
        {
            printf("ifm [%s] does not  exists \n",argv[2]);
            return -1;
        }
        ret=IF_DeleteInterface(ifindex);
        if(ret!=IF_OK)
        {
            printf("ifm delete [%s] fail\n",argv[2]);
            return -1;
        }

         printf(" ifm [%s] delete success\n",argv[2]);
    }
    else if(strcmp(argv[1],"dump")==0)
    {
        unsigned char buf[128];
        if (argc != 3) 
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            return -1;
        }
        
        if (strcmp("all",argv[2])!=0)
        {
        ret=IF_GetByIFName(argv[2],&ifindex);
        if(ret!=IF_OK)
        {
            printf("ifm [%s] does not  exists \n",argv[2]);
            return -1;
        }
        IF_GetAttr(ifindex,IF_NAME,buf,sizeof(buf));
         printf(" ifm [%s] : ifindex %02x \n",buf,ifindex);
         printf("             ifname %s \n",buf);
         IF_GetAttr(ifindex,IF_PSEUDONAME,buf,sizeof(buf));
         printf("             psdname %s \n",buf);
        }
        else
        {
            ret=IF_GetFirstIfindex(IF_SUB_PHY_ALL,&ifindex);
            while(ret==IF_OK)
            {
                 IF_GetAttr(ifindex,IF_NAME,buf,sizeof(buf));
                 printf(" ifm [%s] : ifindex %02x \n",buf,ifindex);
                 printf("             ifname %s \n",buf);
                 IF_GetAttr(ifindex,IF_PSEUDONAME,buf,sizeof(buf));
                 printf("             psdname %s \n",buf);
                  printf("             \r\n");
                 ret=IF_GetNextIfindex(IF_SUB_PHY_ALL,&ifindex);
            }            
        }
    }else if(strcmp(argv[1],"attach")==0)
    {
        if (argc != 4) 
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            return -1;
        }
        ret=IF_GetByIFName(argv[2],&ifindex);
        if(ret!=IF_OK)
        {
            printf("ifm [%s] does not  exists \n",argv[2]);
            return -1;
        }
        ret=IF_GetByIFName(argv[3],&pifindex);
        if(ret!=IF_OK)
        {
            printf("ifm [%s] does not  exists \n",argv[3]);
            return -1;
        }
        ret=IF_AttachParent(ifindex,pifindex);
        if(ret!=IF_OK)
        {
            printf("ifm attach [%s] to [%s] fail \n",argv[2],argv[3]);
            return -1;
        }
        printf("ifm attach [%02x] to [%02x] success \n",ifindex,pifindex);
        ret=IF_GetAttr(ifindex,IF_PARENT,&pifindex,sizeof(ifindex_t));
        if(ret!=IF_OK)
        {
            return -1;
        }
        printf("ifm [%02x] parent is %02x\n",ifindex,pifindex);
    }else if(strcmp(argv[1],"test")==0)
    {
        if (argc != 3) 
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            return -1;
        }
        ret=IF_TestIfExist(atoi(argv[2]));
        if(ret!=IF_OK)
        {
            printf("ifm [%02x] does not exists\n",atoi(argv[2]));
            return -1;
        }
    }else if (strcmp(argv[1],"setname")==0)
    {
        unsigned char buf[128];
        if (argc != 4) 
        {
            printf("%s %d\n",__FUNCTION__,__LINE__);
            return -1;
        }
        ret=IF_GetByIFName(argv[2],&ifindex);
        if(ret!=IF_OK)
        {
            printf("ifm [%s] does not  exists \n",argv[2]);
            return -1;
        }
        ret=IF_SetAttr(ifindex,IF_PSEUDONAME,argv[3],strlen(argv[3]));
        if(ret!=IF_OK)
        {
            printf("ifm [%s] setname error \n",argv[2]);
            return -1;
        }
        ret=IF_GetAttr(ifindex,IF_PSEUDONAME,buf,sizeof(buf));
        if(ret!=IF_OK)
        {
            printf("ifm [%s] setname error \n",argv[2]);
            return -1;
        }
        printf("ifm [%s] new psdname is %s\n",argv[2],buf);
    }
    
    return 0;
}

