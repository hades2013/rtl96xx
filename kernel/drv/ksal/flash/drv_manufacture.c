#ifdef  __cplusplus
extern "C"{
#endif
#include <linux/autoconf.h>
#include "linux/string.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include "drv_flash.h"
#include "drv_manufacture.h"

#define BOOT_CFG_SIZE   0x10000  /* 64kb*/
#define PHY_ADDR_TO_FLASH_ADDR(flash_sect_addr) ((uint32)flash_sect_addr - (uint32)pucBootEnv)

static uint32 crc32(uint32 crc, const uint8 *buf, uint len);

const uint32 crc_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
static uint32 crc32(crc, buf, len)
    uint32 crc;
    const uint8 *buf;
    uint len;
{
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

/*****************************************************************************
    Func Name: GetBootCfg
  Description: 获取boot配置块
        Input: env_t *pstBootCfg
       Output:
       Return: bool
      Caution:
------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------

*****************************************************************************/
bool GetBootCfg(env_t *pstBootCfg)
{

    uint32   ulHaveValidCfg = ENV_NO_CFG;
	env_t  *tem_env_ptr    = NULL;
    uint8 *pucBootEnv = NULL;
    int    rc = 0;

    if(NULL == pstBootCfg)
    {
        DBG_MANUFACTURE_PRINT ("(NULL == pstBootCfg)\n");
        return false;
    }

    pucBootEnv = (uint8 *)kmalloc(BOOT_CFG_SIZE, GFP_KERNEL);
    memset(pucBootEnv, 0, BOOT_CFG_SIZE);

    rc = drv_flash_read(pucBootEnv, 0, BOOT_CFG_SIZE);
	if (rc != 0) {
        DBG_MANUFACTURE_PRINT("drv_flash_read pucBootEnv error\n");
        kfree(pucBootEnv);
        return false;
	}

    //memset(pstBootCfg, 0, MAX_BOOT_CONFIG_SIZE);

    /*CFG_ENV_ADDR为boot cfg的flash基地址*/
    tem_env_ptr = (env_t *)(pucBootEnv);
	
	/*Begin modify by dengjian 2012-08-29 for QID0013*/
    /*遍历存放boot cfg的flash*/
	while(ENV_VALID_FLAG == tem_env_ptr->valid_flags)
	{
	    //DBG_MANUFACTURE_PRINT ("tem_env_ptr=%x\n",tem_env_ptr);
	    /*当前的cfg块超过了boot配置flash块*/
	    if(((uint32)tem_env_ptr + MAX_BOOT_CONFIG_SIZE) > ((uint32)pucBootEnv + BOOT_CFG_SIZE))
	    {
	        DBG_MANUFACTURE_PRINT ("(((uint32)tem_env_ptr + ENV_HEADER_SIZE + tem_env_ptr->data_len) > (pucBootEnv + BOOT_CFG_SIZE))\n");
	        break;
	    }

	    /*如果flags为0xAA55AA55，表示有效*/
	    if(ENV_VALID_FLAG == tem_env_ptr->valid_flags)
	    {
	        /*校验crc*/
	        if(crc32(0, tem_env_ptr->data, ENV_SIZE) == tem_env_ptr->crc)
	        {
	            DBG_MANUFACTURE_PRINT ("ulHaveValidCfg = ENV_HAVE_CFG;\n");
	            /*找到有效的boot cfg*/
	            ulHaveValidCfg = ENV_HAVE_CFG;
	        }
	        else
	        {
	            DBG_MANUFACTURE_PRINT ("ulHaveValidCfg = ENV_NO_CFG;\n");
	            /*没有找到有效的boot cfg*/
	            ulHaveValidCfg = ENV_NO_CFG;
	        }
            //break;
	    }

	    /*获取下一块boot cfg的地址*/
	    tem_env_ptr = (env_t *)((uint32)tem_env_ptr + MAX_BOOT_CONFIG_SIZE);
	    /*判断地址是否合法*/
	    if((uint32)tem_env_ptr > ((uint32)pucBootEnv + BOOT_CFG_SIZE))
	    {
	        DBG_MANUFACTURE_PRINT ("((uint32)tem_env_ptr > ((pucBootEnv + BOOT_CFG_SIZE) - ENV_HEADER_SIZE))\n");
	        break;
	    }
	}
    tem_env_ptr = (env_t *)((uint32)tem_env_ptr - MAX_BOOT_CONFIG_SIZE);
    if(ENV_NO_CFG == ulHaveValidCfg)
    {
        DBG_MANUFACTURE_PRINT("(ENV_NO_CFG == ulHaveValidCfg)\n");
        kfree(pucBootEnv);
        return false;
    }
    memcpy(pstBootCfg, tem_env_ptr, MAX_BOOT_CONFIG_SIZE);
	/*End modify by dengjian 2012-08-29 for QID0013*/
    kfree(pucBootEnv);
    return true;
}
/*****************************************************************************
    Func Name: SaveBootCfg
  Description: 获取或保存boot配置块调试打印信息
        Input: env_t *pstBootCfg
       Output:
       Return: bool
      Caution:
------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------

*****************************************************************************/
void debug_GetSaveBootCfg(void)
{
    env_t *p = NULL;
    int i = 0;

    p = (env_t *)kmalloc(MAX_BOOT_CONFIG_SIZE, GFP_KERNEL);
    if(NULL == p)
    {
        printk ("boot config kmalloc error!\n");
        return;
    }

    memset(p, 0, MAX_BOOT_CONFIG_SIZE);

    if(true != GetBootCfg(p))
    {
        printk ("Read boot config error!\n");
        kfree(p);
        return;
    }
    else
    {
        for(i=0;i<400;i++)
        {
            printk ("%c", *(p->data+i));
        }
        printk ("\n");
        if(true != SaveBootCfg(p))
        {
            printk ("save boot config error!\n");
        }
    }

    kfree(p);
    return;

}
/*****************************************************************************
    Func Name: SaveBootCfg
  Description: 保存boot配置块
        Input: env_t *pstBootCfg
       Output:
       Return: bool
      Caution:
------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------

*****************************************************************************/

bool SaveBootCfg(env_t *pstBootCfg)
{
	uint    len = 0;
	uint8  *env = NULL;
	uint8  *nxt = NULL;
	uint32	end_addr = 0;
	uint32	flash_sect_addr = 0;
	uint8   env_invalid_flags[ENV_VALID_FLAGS_LEN] = {0,0,0,0};
	env_t  *tem_env_ptr = NULL;
	uint32  ulHaveValidCfg = ENV_NO_CFG;
	uint8  *pcNextFreeAddr = NULL;
	uint    cur_save_len = 0;
	uint8  *pcCur = 0;
	int	    rc = 0;
	int     rcode = 0;
    unsigned long i = 0;
    uint8 *pucBootEnv = NULL;

	if(NULL == pstBootCfg)
    {
        return false;
    }
    
	/*Begin modify by dengjian 2012-08-29 for QID0013*/
    #if 0
    /*计算data_len*/
	for (env=pstBootCfg->data; *env; env=nxt+1)
	{
		for (nxt=env; *nxt; ++nxt)
		{
		    len++;
		}
		len++;
	}
	len++;
    #endif
    
    /* the data length,	must be 4bytes align*/
	//pstBootCfg->data_len = (len+0x3) & (~0x3);
	pstBootCfg->crc = crc32(0, pstBootCfg->data, ENV_SIZE);
    pstBootCfg->valid_flags = ENV_VALID_FLAG;
    
    DBG_MANUFACTURE_PRINT("data:\n");
    for(i=0; i<ENV_SIZE; i++)
    {
        DBG_MANUFACTURE_PRINT ("%c",pstBootCfg->data[i]);
    }

    #if 0
	cur_save_len    = (ENV_HEADER_SIZE + pstBootCfg->data_len); /*要保存的cfg块长度*/

    if(cur_save_len >= MAX_BOOT_CONFIG_SIZE)
    {
        return false;
    }
    #endif
    DBG_MANUFACTURE_PRINT("\n");

    pucBootEnv = (uint8 *)kmalloc(BOOT_CFG_SIZE, GFP_KERNEL);
    memset(pucBootEnv, 0, BOOT_CFG_SIZE);
    rc = drv_flash_read(pucBootEnv, 0, BOOT_CFG_SIZE);
	if (rc != 0) {
        DBG_MANUFACTURE_PRINT("drv_flash_read pucBootEnv error\n");
        kfree(pucBootEnv);
        return false;
	}

	flash_sect_addr = (uint32)pucBootEnv;         /*"UBoot-env"的起始地址*/
	end_addr        = flash_sect_addr + BOOT_CFG_SIZE - 1; /*"UBoot-env"的结束地址*/

    /*CFG_ENV_ADDR为boot cfg的flash基地址*/
    tem_env_ptr = (env_t *)pucBootEnv;

    /*遍历存放boot cfg的flash*/
	while(ENV_VALID_FLAG == tem_env_ptr->valid_flags)
	{
	    /*当前的cfg块超过了boot配置flash块*/
	    if(((uint32)tem_env_ptr + MAX_BOOT_CONFIG_SIZE) > ((uint32)pucBootEnv + BOOT_CFG_SIZE))
	    {
	        break;
	    }

	    /*如果flags为0xAA55AA55，表示有效*/
	    if(ENV_VALID_FLAG == tem_env_ptr->valid_flags)
	    {
	        /*校验crc*/
	        if(crc32(0, tem_env_ptr->data, ENV_SIZE) == tem_env_ptr->crc)
	        {
	            /*找到有效的boot cfg*/
	            ulHaveValidCfg = ENV_HAVE_CFG;
	        }
	        else
	        {
	            /*没有找到有效的boot cfg*/
	            ulHaveValidCfg = ENV_NO_CFG;
	        }
            //break;
	    }

	    /*获取下一块boot cfg的地址*/
	    tem_env_ptr = (env_t *)((uint32)tem_env_ptr + MAX_BOOT_CONFIG_SIZE);
	    /*判断地址是否合法*/
	    if((uint32)tem_env_ptr > ((uint32)pucBootEnv + BOOT_CFG_SIZE))
	    {
	        //printk("0x%08lX\n",PHY_ADDR_TO_FLASH_ADDR(tem_env_ptr));
	        break;
	    }
	}

	/*没有找到有效的boot cfg*/
	if(ENV_NO_CFG == ulHaveValidCfg)
	{
        /*擦除整块64K的flash，并将当前配置保存到第1块boot cfg块中*/
        goto erase_and_write_first_block;
	}


    /*下一个空闲cfg块的起始地址*/
    pcNextFreeAddr = (uint8 *)((uint32)tem_env_ptr);

    #if 0
    /*must be 4bytes align*/
    if((uint32)pcNextFreeAddr & 0x3)
    {
        /*擦除整块64K的flash，并将当前配置保存到第1块boot cfg块中*/
        goto erase_and_write_first_block;
    }
    #endif
    
    /*检查保存下一个cfg块是否会越界*/
    if(((uint32)pcNextFreeAddr + MAX_BOOT_CONFIG_SIZE) > ((uint32)pucBootEnv + BOOT_CFG_SIZE))
    {
        /*擦除整块64K的flash，并将当前配置保存到第1块boot cfg块中*/
        goto erase_and_write_first_block;
    }

    /*检查保存下一个cfg块是否为全0xFF*/
    if (0xf000 == PHY_ADDR_TO_FLASH_ADDR(pcNextFreeAddr))
    {
	    for(pcCur=pcNextFreeAddr; pcCur<(pcNextFreeAddr + MAX_BOOT_CONFIG_SIZE-0x10); pcCur++)
	    {
	        if(0xFF != *pcCur)
	        {
                /*擦除整块64K的flash，并将当前配置保存到第1块boot cfg块中*/
                goto erase_and_write_first_block;
	        }
	    }
    }
    else
    {
        for(pcCur=pcNextFreeAddr; pcCur<(pcNextFreeAddr + MAX_BOOT_CONFIG_SIZE); pcCur++)
	    {
	        if(0xFF != *pcCur)
	        {
                /*擦除整块64K的flash，并将当前配置保存到第1块boot cfg块中*/
                goto erase_and_write_first_block;
	        }
	    }
    }
	/*将当前配置保存到pcNextFreeAddr指向的块中*/
	DBG_MANUFACTURE_PRINT ("Writing to Flash... ");
    rc = drv_flash_write((uint8 *)pstBootCfg, (uint32)PHY_ADDR_TO_FLASH_ADDR(pcNextFreeAddr), MAX_BOOT_CONFIG_SIZE);

	if (rc != 0) {
		rcode = 1;
        DBG_MANUFACTURE_PRINT ("\n将当前配置保存到pcNextFreeAddr指向的块中 failed\n");
	} else {
		DBG_MANUFACTURE_PRINT ("done\n");
	}

    #if 0
    DBG_MANUFACTURE_PRINT("(uint32)tem_env_ptr + ENV_VALID_FLAGS_OFFSET=0x%p\n",(uint32)PHY_ADDR_TO_FLASH_ADDR(tem_env_ptr) + ENV_VALID_FLAGS_OFFSET);
	/*并将原来的boot cfg块flags设置为0，表示该块无效*/
	if(0 == rcode)
	{
    	DBG_MANUFACTURE_PRINT ("Writing to Flash... ");
    	rc = drv_flash_write((uint8 *)env_invalid_flags, (uint32)PHY_ADDR_TO_FLASH_ADDR(tem_env_ptr) + ENV_VALID_FLAGS_OFFSET, ENV_VALID_FLAGS_LEN);
    	if (rc != 0) {
    		rcode = 1;
            DBG_MANUFACTURE_PRINT ("\n并将原来的boot cfg块flags设置为0，表示该块无效 failed\n");
    	} else {
    		DBG_MANUFACTURE_PRINT ("done\n");
    	}
	}
    #endif
    
	if(1 == rcode)
	{
        kfree(pucBootEnv);
        return false;
	}

    kfree(pucBootEnv);
	return true;

/*擦除整块64K的flash，并将当前配置保存到第1块boot cfg块中*/
erase_and_write_first_block:
    DBG_MANUFACTURE_PRINT ("erase_and_write_first_block\n");
	DBG_MANUFACTURE_PRINT ("Protect off %08lX ... %08lX\n",
                            (uint32)PHY_ADDR_TO_FLASH_ADDR(flash_sect_addr), PHY_ADDR_TO_FLASH_ADDR(end_addr));

	DBG_MANUFACTURE_PRINT ("Erasing Flash...");
	if (drv_flash_sect_erase (PHY_ADDR_TO_FLASH_ADDR(flash_sect_addr), PHY_ADDR_TO_FLASH_ADDR(end_addr)))
	{
        DBG_MANUFACTURE_PRINT ("drv_flash_sect_erase failed\n");
        kfree(pucBootEnv);
		return false;
	}

    DBG_MANUFACTURE_PRINT ("done\n");

	DBG_MANUFACTURE_PRINT ("Writing to Flash... ");
	rc = drv_flash_write((uint8 *)pstBootCfg, PHY_ADDR_TO_FLASH_ADDR(flash_sect_addr), MAX_BOOT_CONFIG_SIZE);
	if (rc != 0) {
        DBG_MANUFACTURE_PRINT ("drv_flash_sect_erase failed\n");
		rcode = 1;
	} else {
	    rcode = 0;
	}
	/*End modify by dengjian 2012-08-29 for QID0013*/
	if(1 == rcode)
	{
        kfree(pucBootEnv);
        return false;
	}

    kfree(pucBootEnv);

    return true;
}

#ifdef  __cplusplus
}
#endif



