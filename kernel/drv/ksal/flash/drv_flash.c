#ifdef  __cplusplus
extern "C"{
#endif

#include "drv_flash.h"

/* addr: 0~8M
*/
int 
drv_flash_sect_erase (uint32 addr_first, uint32 addr_last)
{
    #if 0
    uint32 len = addr_last - addr_first;
    len = len+1;
    return rtk_flash_direct_erase(addr_first, len);
    #endif

    int rc;
    uint32 uiLen;
    struct erase_info stEraseInof;
    struct mtd_info *pstMtd = NULL;

    if (addr_last < addr_first)
    {
        printk( "drv_flash_read src null\n");
        return 1;
    }
	/*Begin modify by dengjian 2012-08-29 for QID0013*/
    memset(&stEraseInof, 0, sizeof(stEraseInof));

    uiLen = addr_last - addr_first;
    uiLen = uiLen + 1;

    pstMtd = get_mtd_device_nm(FLASH_DEV_NAME);
    
    stEraseInof.addr = addr_first;
    stEraseInof.len = uiLen;
    stEraseInof.callback = NULL;
    stEraseInof.mtd = pstMtd;
	/*End modify by dengjian 2012-08-29 for QID0013*/
    
    if (IS_ERR(pstMtd))
    {
        printk( "drv_flash_read get_mtd_device_nm ERROR\n");
        return 1;
    }

    rc = pstMtd->erase(pstMtd, &stEraseInof);

    return rc;

}

/* addr: 0~8M
*/
int
drv_flash_read(uint8 * src, uint32 addr, uint32 cnt)
{
    #if 0
    return rtk_flash_direct_read(addr, cnt, src);
    #endif

    size_t retLen;
    size_t readLen;
    loff_t from;
    int rc;

    struct mtd_info *pstMtd = NULL;

    if (NULL == src)
    {
        printk( "drv_flash_read src null\n");
        return 1;
    }

    retLen = cnt;
    readLen = cnt;
    from = addr;

    pstMtd = get_mtd_device_nm(FLASH_DEV_NAME);

    if (IS_ERR(pstMtd))
    {
        printk( "drv_flash_read get_mtd_device_nm ERROR\n");
        return 1;
    }

    rc = pstMtd->read(pstMtd, from, readLen, &retLen, (u_char *)src);

    return rc;

}

/* addr: 0~8M
*/
int
drv_flash_write (uint8 *src, uint32 addr, uint32 cnt)
{
    #if 0
    return rtk_flash_direct_write(addr, cnt, src);
    #else

    size_t retLen;
    size_t readLen;
    loff_t from;
    int rc;

    struct mtd_info *pstMtd = NULL;

    if (NULL == src)
    {
        printk( "drv_flash_read src null\n");
        return 1;
    }

    retLen = cnt;
    readLen = cnt;
    from = addr;

    pstMtd = get_mtd_device_nm(FLASH_DEV_NAME);

    if (IS_ERR(pstMtd))
    {
        printk( "drv_flash_read get_mtd_device_nm ERROR\n");
        return 1;
    }

    //printk( "pstMtd->write %x\n", pstMtd->write);
    //printk( "from %llx readLen %x retLen %x\n", from, readLen, retLen);

    rc = pstMtd->write(pstMtd, from, readLen, &retLen, (u_char *)src);

    return rc;


    #endif
}

#ifdef  __cplusplus
}
#endif
