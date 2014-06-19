#ifdef  __cplusplus
    extern "C"{
#endif
#include "lw_proc_flash.h"

extern unsigned int s_ui_management_vlan;
extern g_mac_write_flag;

struct proc_dir_entry *lw_procmanu_dir = NULL;



/*****************************************************************************
    Func Name: lw_setfactoyrinfo
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
int lw_setfactoyrinfo(char *name,char *val)
{
    char *p = NULL;
    char *tmp = NULL;
    char *n = NULL;
    char *nxt = NULL;
    char *m = NULL;
    env_t *pcBootCfg = NULL;
    
    pcBootCfg = kmalloc(MAX_BOOT_CONFIG_SIZE, GFP_KERNEL);
    if(NULL == pcBootCfg)
    {
        return EINVAL;
    }
    /*获取uboot配置块*/
    memset(pcBootCfg, 0, MAX_BOOT_CONFIG_SIZE);
    if(true != GetBootCfg(pcBootCfg))
    {
        kfree(pcBootCfg);
        return EINVAL;
    }
    /*查找需要设置的配置项*/
    p = (char *)pcBootCfg->data;
    while(*p != '\0')
    {
        tmp = strchr(p,'=');
        if(NULL == tmp)
        {
            kfree(pcBootCfg);
            return EINVAL;
        }
        *tmp  = '\0';
        nxt = tmp+1;
        while(*nxt != '\0')
        {
            nxt++;
        }
        if(strcmp(p,name)==0)
        {
            *tmp  = '=';
            m = nxt+1;
            break;
        }
        *tmp  = '=';
        p = nxt+1;
    }
    /*在原有配置项中没有找到，在配置项后面添加*/
    if('\0' == *p)
    {
        while (*name != '\0')
        {
            *p = *name;
            p++;
            name++;
        }
        *p = '=';
        while (*val != '\0')
        {
            p++;
            *p = *val;
            val++;
        }
        *p = '\0';
        /*\0\0表示配置项已经完全结束*/
        *(++p) = '\0';
    }
    else
    {
        /*在最后一项中找到，直接修改*/
        if(tmp&&m&&('\0' == *m))
        {
            n = tmp+1; 
            while('\0' != *val)
            {
                *n = *val;
                n++;
                val++;
            }
            *n = '\0';
             /*\0\0表示配置项已经完全结束*/
            *++n = '\0';
        }
        /*在中间的配置项中找到，先删除然后在最后添加*/
        else
        {
            while(m&&(m!=(char *)&(pcBootCfg->data[ENV_SIZE-1])))
            {
                *p = *m++;
				if ((*p == '\0') && (*m == '\0'))
				{
					break;
				}
				++p;
			}
	        while (*name != '\0')
            {
                p++;
                *p = *name;
                name++;
            }
            *++p = '=';
            while (*val != '\0')
            {
                p++;
                *p = *val;
                val++;
            }
            *(++p) = '\0';
             /*\0\0表示配置项已经完全结束*/
            *(++p) = '\0';
        }
    }
    if(true != SaveBootCfg(pcBootCfg))
    {
        kfree(pcBootCfg);
        return EINVAL;
    }
    kfree(pcBootCfg);
    return 0;
}
/*****************************************************************************
    Func Name: lw_getfatoryinfo
  Description: 获取厂测数据
        Input: factory_data_s **factoryinfo         
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

int lw_getfatoryinfo(factory_data_s *factoryinfo)
{
    char *p = NULL; 
    char *temp = NULL;
    char *a= NULL;
    env_t * pcBootCfg = NULL;

	if(!factoryinfo)
	{
        printk( "factoryinfo is null\n");
        return EINVAL;
	}
    pcBootCfg = kmalloc(MAX_BOOT_CONFIG_SIZE, GFP_KERNEL);
    if(NULL == pcBootCfg)
    {
        printk( "lw_getfatoryinfo kmalloc error\n");
        return EINVAL;
    }

    memset(pcBootCfg, 0, MAX_BOOT_CONFIG_SIZE);

    if(true != GetBootCfg(pcBootCfg))
    {
        kfree(pcBootCfg);
        return EINVAL;
    }
    memset(factoryinfo, 0, sizeof(factory_data_s));
    p = (char *)pcBootCfg->data;
    while(*p != '\0')
    {
        temp = strchr(p,'=');
        if(NULL == temp)
        {
            kfree(pcBootCfg);
            DBG_MANUFACTURE_PRINT("NULL == temp\n");
            return EINVAL;
        }
        *temp  = '\0';
        a = temp+1;
        while(*a != '\0')
        {
            a++;
        }
        if(0 == strcmp(p,ETH_ADDR))
        {
            strncpy(factoryinfo->ethaddr,temp+1,ETHADDR_SIZE);
            factoryinfo->ethaddr[ETHADDR_SIZE-1] = '\0';
        }
     
        if(0 == strcmp(p,BOOT_FLAG))
        {
            strncpy(factoryinfo->bootflag,temp+1,BOOTFLAG_SIZE);
            factoryinfo->bootflag[BOOTFLAG_SIZE-1] = '\0';
        }
        
        if(0 == strcmp(p,RESTORE_FLAG))
        {
            strncpy(factoryinfo->restore_default,temp+1,RESTOREFLAG_SIZE);
            factoryinfo->restore_default[RESTOREFLAG_SIZE-1] = '\0';
        } 
        if(0 == strcmp(p,HARD_VERSION))
        {
            strncpy(factoryinfo->hardversion,temp+1,HARDVERSION_SIZE);
            factoryinfo->hardversion[HARDVERSION_SIZE-1] = '\0';
        } 
        if(0 == strcmp(p,BOOT_VERSION))
        {
            strncpy(factoryinfo->bootversion,temp+1,BOTVERSION_SIZE);
            factoryinfo->bootversion[BOTVERSION_SIZE-1] = '\0';
        } 
        if(0 == strcmp(p,MANU_NUM))
        {
            strncpy(factoryinfo->manucode,temp+1,MANUCODE_SIZE);
            factoryinfo->manucode[MANUCODE_SIZE-1] = '\0';
        } 
		/*Begin add by dengjian 2012-11-28 for EPN204QID0015*/
        #ifdef CONFIG_BOSA
        if(0 == strcmp(p,MODSETVAL))
        {
            strncpy(factoryinfo->modsetval,temp+1,MODSETVAL_SIZE);
            factoryinfo->modsetval[MODSETVAL_SIZE-1] = '\0';
        }
        if(0 == strcmp(p,APCSETVAL))
        {
            strncpy(factoryinfo->apcsetval,temp+1,APCSETVAL_SIZE);
            factoryinfo->apcsetval[APCSETVAL_SIZE-1] = '\0';
        }
        /*Begin added by feihuaxin for bugid 103 2013-6-6 */
        if(0 == strcmp(p,FTOVERFLAG))
        {
            strncpy(factoryinfo->ftoverflag,temp+1,MODSETVAL_SIZE);
            factoryinfo->ftoverflag[MODSETVAL_SIZE-1] = '\0';
        }
        /*End added by feihuaxin for bugid 103  2013-6-6 */
        #endif
		/*End add by dengjian 2012-11-28 for EPN204QID0015*/
        p = a+1;
    }
    
    kfree(pcBootCfg);
    return 0;
}

/*****************************************************************************
    Func Name: print_factoryinfo
       Author: guguiyuan
  Description: 显示厂测数据内容
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
/*lint -e{715} */
int print_factoryinfo(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }

    if('\0' == factoryinfo->bootversion[0])
    {
        strcpy(factoryinfo->bootversion,"100");
    }

    if('\0' == factoryinfo->hardversion[0])
    {
        strcpy(factoryinfo->hardversion,"VER.A");
    }
    seq_printf(m,"%s=%s\n",ETH_ADDR, factoryinfo->ethaddr);
    seq_printf(m,"%s=%s\n",HARD_VERSION, factoryinfo->hardversion);
    seq_printf(m,"%s=%s\n",BOOT_VERSION, factoryinfo->bootversion);
    seq_printf(m,"%s=%s\n",MANU_NUM, factoryinfo->manucode);
    kfree(factoryinfo);
    return 0;
}
/*****************************************************************************
    Func Name: print_bootflag
       Author: guguiyuan
  Description: 显示bootflag
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
/*lint -e{715} */
int print_bootflag(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->bootflag[0])
    {
        strcpy(factoryinfo->bootflag,"0");
    }
    seq_printf(m,"%s\n",factoryinfo->bootflag);
    kfree(factoryinfo);
    return 0;
}
/*****************************************************************************
    Func Name: print_restoreflag
 Date Created: 2011/8/24
       Author: guguiyuan
  Description: 显示restoreflag
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

/*lint -e{715} */
int print_restoreflag(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->restore_default[0])
    {
        strcpy(factoryinfo->restore_default,"0");
    }
    seq_printf(m,"%s\n",factoryinfo->restore_default);
    kfree(factoryinfo);
    return 0;
}

/*****************************************************************************
    Func Name: write_bootflag
       Author: guguiyuan
  Description: 用户态设置bootflag
        Input: char *buffer   
               unsigned long count
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

/*lint -e{715} */
int write_bootflag(char *buffer, unsigned long count)
{ 
    if(NULL == buffer)
    {
        return EINVAL;
    }
    if(('0' != *buffer)&&('1' != *buffer))
    {
        return EINVAL;
    }
    *(buffer+1) = '\0';

    return lw_setfactoyrinfo(BOOT_FLAG, buffer);
}
/*****************************************************************************
    Func Name: write_bootflag
       Author: guguiyuan
  Description: 用户态设置restoreflag
        Input: char *buffer   
               unsigned long count
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
/*lint -e{715} */
int write_restoreflag(char *buffer, unsigned long count)
{
    if(NULL == buffer)
    {
        return EINVAL;
    }
    
    if(('0' != *buffer)&&('1' != *buffer))
    {
        return EINVAL;
    }
    *(buffer+1) = '\0';
    
    return lw_setfactoyrinfo(RESTORE_FLAG, buffer);
}
/*****************************************************************************
    Func Name: print_bootversion
 Date Created: 2013/6/5
       Author: zhouguanhua
  Description: 显示bootversion
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
/*lint -e{715}*/
int print_bootversion(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->bootversion[0])
    {
        strcpy(factoryinfo->bootversion,"100");
    }
    seq_printf(m,"%s\n",factoryinfo->bootversion);
    kfree(factoryinfo);
    return 0;
} 
/*****************************************************************************
    Func Name: print_hardversion
 Date Created: 2013/6/5
       Author: zhouguanhua
  Description: 显示hardversion
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

/*lint -e{715} */
int print_hardversion(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->hardversion[0])
    {
        strcpy(factoryinfo->hardversion,"VER.A");
    }
    seq_printf(m,"%s\n",factoryinfo->hardversion);
    kfree(factoryinfo);
    return 0;
}
/*****************************************************************************
    Func Name: print_manucode
 Date Created: 2013/5/15
       Author: zhouguanhua
  Description: 显示manucode
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

/*lint -e{715} */
int print_manucode(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->manucode[0])
    {
        strcpy(factoryinfo->manucode,"");
    }
    seq_printf(m,"%s\n",factoryinfo->manucode);
    kfree(factoryinfo);
    return 0;
}

/*****************************************************************************
    Func Name: write_manucode
       Date Created: 2013/5/15
             Author: zhouguanhua
        Input: char *buffer   
               unsigned long count
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

/*lint -e{715} */
int write_manucode(char *buffer, unsigned long count)
{ 
    int len=0;
    if(NULL == buffer)
    {
        return EINVAL;
    }
    len=strlen(buffer);
    *(buffer+len)='\0';
    return lw_setfactoyrinfo(MANU_NUM, buffer);
}

/*****************************************************************************
  函 数 名: write_mac
  函数描述: 写mac
  输入参数: struct seq_file *m
               void *v
  输出参数: 无
    返回值:
    注意点:
------------------------------------------------------------------------------
    修改历史
    日期        姓名             描述
  --------------------------------------------------------------------------

*****************************************************************************/
/*lint -e{715} */
int write_ethaddr(char *buffer, unsigned long count)
{	
    int len=0;
    if(NULL == buffer)
    {
        return EINVAL;
    }
	
	/*Begin modified by huangmingjian 2014/04/04 for Bug 500*/
	if(1 == g_mac_write_flag)
	{
		len=strlen(buffer);
		*(buffer+len)='\0';
		return lw_setfactoyrinfo(ETH_ADDR, buffer);
	}
	else
	{
		return 0;
	}
	/*End modified by huangmingjian 2014/04/04 for Bug 500*/
}

/*Begin add by zhouguanhua 2013-5-15 for bug id 52*/ 
static int print_ethaddr(struct seq_file *m,void *v)
{ 
   factory_data_s *factoryinfo;
   
   factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
   if(0 != lw_getfatoryinfo(factoryinfo))
   {
       seq_printf(m,"can not get factoryinfo\n");
       kfree(factoryinfo);
       return 0;
   }
   if('\0' == factoryinfo->ethaddr[0])
   {
       strcpy(factoryinfo->ethaddr,"");
   }
   seq_printf(m,"%s\n",factoryinfo->ethaddr);
   kfree(factoryinfo);
   
   return 0;
}
/*End add by zhouguanhua 2013-5-15 for bug id 52*/

#if 0 
/*lint -e{715} */
static int read_ethaddr(struct seq_file *m,void *v)
{
    seq_printf(m, "alse see factoyrinfo\n");
	return 0;
}
#endif 
/*lint -e{715} */
int read_mvlan(struct seq_file *m,void *v)
{
    seq_printf(m, "%ld\n", s_ui_management_vlan);
	return 0;
}

int write_mvlan(const char *buffer, unsigned long count)
{
	unsigned int value;
	int remain = (int)count;

	while (remain) {
		if (sscanf(buffer, "%ld", &value) == 1)
		{
            value &= 0xffff;
            if (value > 4094) {
			    return EFAULT;
		    }

			s_ui_management_vlan = value;
		}
		else
			return EFAULT;
		/* advance to one character past the next ; */
		do {
			++buffer;
			--remain;
		}
		while (remain && *(buffer - 1) != ';');
	}

	return (int)count;
}

/*Begin add by dengjian 2012-11-28 for EPN204QID0015*/
#ifdef CONFIG_BOSA
/*****************************************************************************
    Func Name: print_modsetval
       Author: dengjian
  Description: 显示modsetval
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
int print_modsetval(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->modsetval[0])
    {
        strcpy(factoryinfo->modsetval,"0");
    }
    seq_printf(m,"%s\n",factoryinfo->modsetval);
    kfree(factoryinfo);
    return 0;
}

/*****************************************************************************
    Func Name: write_modsetval
       Author: dengjian
  Description: 用户态设置调制电流值
        Input: char *buffer   
               unsigned long count
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
int write_modsetval(char *buffer, unsigned long count)
{ 
    if(NULL == buffer)
    {
        return EINVAL;
    }
    if(('0' != *buffer)&&('1' != *buffer))
    {
        return EINVAL;
    }
    *(buffer+1) = '\0';

    return lw_setfactoyrinfo(MODSETVAL, buffer);
}

/*****************************************************************************
    Func Name: print_apcsetval
       Author: dengjian
  Description: 显示偏置电流寄存器值
        Input: struct seq_file *m   
                void *v
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
int print_apcsetval(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->apcsetval[0])
    {
        strcpy(factoryinfo->apcsetval,"0");
    }
    seq_printf(m,"%s\n",factoryinfo->apcsetval);
    kfree(factoryinfo);
    return 0;
}

/*****************************************************************************
    Func Name: write_apcsetval
       Author: dengjian
  Description: 用户态设置偏置电流值
        Input: char *buffer   
               unsigned long count
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
int write_apcsetval(char *buffer, unsigned long count)
{ 
    if(NULL == buffer)
    {
        return EINVAL;
    }
    if(('0' != *buffer)&&('1' != *buffer))
    {
        return EINVAL;
    }
    *(buffer+1) = '\0';

    return lw_setfactoyrinfo(APCSETVAL, buffer);
}
#endif
/*End add by dengjian 2012-11-28 for EPN204QID0015*/
/*Begin added by feihuaxin for bugid 103  2013-6-6 */
#ifdef CONFIG_BOSA //add CONFIG_BOSA by zhouguanhua 2013/6/14
int print_FToverflag(struct seq_file *m, void *v)
{
    factory_data_s *factoryinfo;
    
    factoryinfo = kmalloc(sizeof(factory_data_s), GFP_KERNEL);
    if(0 != lw_getfatoryinfo(factoryinfo))
    {
        seq_printf(m,"can not get factoryinfo\n");
        kfree(factoryinfo);
        return 0;
    }
    if('\0' == factoryinfo->ftoverflag[0])
    {
        strcpy(factoryinfo->ftoverflag,"0");
    }
    seq_printf(m,"%s\n",factoryinfo->ftoverflag);
    kfree(factoryinfo);
    return 0;

}

int write_FToverflag(char *buffer, unsigned long count)
{
    if(NULL == buffer)
    {
        return EINVAL;
    }
    if(('0' != *buffer)&&('1' != *buffer))
    {
        return EINVAL;
    }
    *(buffer+1) = '\0';

    return lw_setfactoyrinfo(FTOVERFLAG, buffer);

}
#endif
/*End add by feihuaxin for bugid 103  2013-6-6 */

/*****************************************************************************
    Func Name: flash_dispatch_state_seq_show
       Author: guguiyuan
  Description: 挂到文件open func的函数
        Input: struct seq_file *m  
                void *v             
       Output: 
       Return: static int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
/*lint -e{715} */
static int
flash_dispatch_state_seq_show (struct seq_file *m, void *v)
{
	proc_item * item;
	item = (proc_item *)m->private;
	
	if (( item != NULL)&&(item->read_func != NULL ))
	{
		/*FIXME may need sem lock*/
		return item->read_func(m,item);
	}
	return 0;
}
/*****************************************************************************
    Func Name: flash_dispatch_open_fs
       Author: guguiyuan
  Description: 打开文件
        Input: struct inode *inode
                struct file *file             
       Output: 
       Return: static int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static int
flash_dispatch_open_fs(struct inode *inode, struct file *file)
{
    return single_open(file, flash_dispatch_state_seq_show, PDE(inode)->data);
}
/*****************************************************************************
    Func Name: flash_dispatch_write
       Author: guguiyuan
  Description: 将用户输入写入文件
        Input: struct file *file
               const char __user * buffer 
               size_t  count
               loff_t * data
       Output: 
       Return: static int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

/*lint -e{715} */
static ssize_t
flash_dispatch_write(struct file *file, const char __user * buffer,
	       size_t  count, loff_t * data)
{
	int result = -EFAULT;
	char *tmp_buffer;
	proc_item * item ;

        struct seq_file *m = (struct seq_file *)file->private_data;    
        item = (proc_item *)m->private;

	/* Arg buffer points to userspace memory, which can't be accessed
	 * directly.  Since we're making a copy, zero-terminate the
	 * destination so that sscanf can be used on it safely.
	 */
	tmp_buffer = kmalloc(count + 1, GFP_KERNEL);
	if (!tmp_buffer)
		return -ENOMEM;

	if (copy_from_user(tmp_buffer, buffer, count)) {
        kfree(tmp_buffer);
		result = -EFAULT;
	} else {
		tmp_buffer[count] = 0;

		/*FIXME may need sem lock*/
		if (( item !=NULL)&&(item->write_func!= NULL ))
		{
			result = item->write_func(tmp_buffer, count);          
            if(result == 0)
            {
                result =(int)count;
            }
		}
	}
	kfree(tmp_buffer);
	return result;
}
/*lint -save -e785*/
static struct file_operations lw_drv_flash = {
	.open = flash_dispatch_open_fs,
	.read = seq_read,
	.write = flash_dispatch_write,
	.llseek = seq_lseek,
	.release = single_release,
};
/*lint -restore*/
static proc_item lw_proc_items[] = {
    {"factoryinfo",print_factoryinfo,NULL},
    {"bootflag",print_bootflag,write_bootflag},
    {"restoreflag",print_restoreflag,write_restoreflag},
    {"zj_debug",zj_debug,NULL},
    /*Begin add by zhouguanhua 2013-5-15 for bug id 52*/ 
    {"SN", print_manucode, write_manucode},
    {"ethaddr", print_ethaddr, write_ethaddr},
   /*End add by zhouguanhua 2013-5-15 for bug id 52*/   
    {"mvlan", read_mvlan, write_mvlan},
    /*Begin add by zhouguanhua 2013-5-6 for bug id 36*/
    {"bootversion", print_bootversion, NULL},
    {"hardversion", print_hardversion, NULL},
    /*End add by zhouguanhua 2013-5-6 for bug id 36*/
	/*Begin add by dengjian 2012-11-28 for EPN204QID0015*/
    #ifdef CONFIG_BOSA
    {"modsetval",print_modsetval,write_modsetval},
    {"apcsetval",print_apcsetval,write_apcsetval},
    
	/*End add by dengjian 2012-11-28 for EPN204QID0015*/
    {"FToverflag", print_FToverflag, write_FToverflag}, 
    #endif
	{NULL}
};
/*****************************************************************************
    Func Name: lw_flash_init
       Author: guguiyuan
  Description: 在指定目录创建文件
        Input:  
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static int  add_device(void)
{
	struct proc_dir_entry *proc;
	proc_item *item;

	for (item = lw_proc_items; item->name; ++item) {

		proc = create_proc_entry(item->name,
					      S_IFREG | S_IRUGO | S_IWUSR,
					      lw_procmanu_dir);
		if (proc)
		{
			
			#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
		  	proc->owner = THIS_MODULE;
			#endif
			
			proc->data = item;
			proc->proc_fops = &lw_drv_flash;
		} else {
			printk("%s creat proc entry err\n",item->name );
		}
	}

	return 0;
}

/*****************************************************************************
    Func Name: lw_flash_init
       Author: guguiyuan
  Description: 文件创建初始化接口
        Input:  
       Output: 
       Return: int
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
int lw_flash_init(void)
{
	int status = 0;
	lw_procmanu_dir = proc_mkdir(PROC_MANUFACTORY, NULL);
    
	if (!lw_procmanu_dir)
    {
        printk("create dir %s failed\n", PROC_MANUFACTORY);
        status = 1;
	}
    else 
	{
#ifdef MODULE	
		lw_procmanu_dir->owner = THIS_MODULE;
#endif
		status = add_device(); 
    }
	return status;
}
static int  lw_flash_exit(void)
{
    proc_item *item;

	for (item = lw_proc_items; item->name; ++item)
		remove_proc_entry(item->name, lw_procmanu_dir);
	return 0;
}
module_init(lw_flash_init);
module_exit(lw_flash_exit);
/*Begin added by feihuaxin for bugid 103 2013-6-8*/
EXPORT_SYMBOL(lw_getfatoryinfo);
EXPORT_SYMBOL(lw_setfactoyrinfo);
/*End added by feihuaxin for bugid 103 2013-6-8*/

#ifdef  __cplusplus
}
#endif

