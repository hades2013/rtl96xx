/*
 * Include Files
 */
#include <common.h>
#include <spi_flash.h>
#include <soc.h>
#include <pblr.h>
#include "flash_spi.h"
#include "memctl_err_handle.h"

static unsigned long flash_size;
static u32_t src_data_addr;
static struct spi_flash *sf;

#define _cache_flush	pblr_dc_flushall
#define	FLASH_TOP_ADDRESS	    (FLASHBASE+flash_size-1)
#define	TEST_SIZE_PER_PATTREN	(0x10000)  //64KB
#define JFFS2_START             (0x300000) //3MB

extern unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);

/*
 * Data Declaration
 */
DECLARE_GLOBAL_DATA_PTR;


const u32_t flash_patterns[] = {
					0x00000000,
					0xff00ff00,
					0x00ff00ff,
					0x0000ffff,
					0xffff0000,
					0xffffffff,
					0x5a5aa5a5,
					0xa5a5a5a5,
					0x55555555, 
					0xaaaaaaaa, 
					0x01234567, 
					0x76543210, 
					0x89abcdef,
					0xfedcba98,
			};

/*
 * Function Declaration
 */
int flash_sect_erase (ulong addr_first, ulong addr_last)
{
	u32_t sector=0, size = addr_last - addr_first;
	
	if(!sf) {
        sf = spi_flash_probe(0, 0, 0, 0);
	    if(!sf) {
	        printf("spi flash probe failed\n");
	        return 0;
	    }
	}
    if (size > CONFIG_ENV_SECT_SIZE) {
        sector = size / CONFIG_ENV_SECT_SIZE;
        if (size % CONFIG_ENV_SECT_SIZE)
            sector++;
    }    
    
	return spi_flash_erase(sf, addr_first, sector * CONFIG_ENV_SECT_SIZE);
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 * Make sure all target addresses are within Flash bounds,
 * and no protected sectors are hit.
 * Returns:
 * ERR_OK          0 - OK
 * ERR_TIMOUT      1 - write timeout
 * ERR_NOT_ERASED  2 - Flash not erased
 * ERR_PROTECTED   4 - target range includes protected sectors
 * ERR_INVAL       8 - target address not in Flash memory
 * ERR_ALIGN       16 - target address not aligned on boundary
 *			(only some targets require alignment)
 */
int
flash_write (char *src, ulong addr, ulong cnt)
{
	if(!sf) {
        sf = spi_flash_probe(0, 0, 0, 0);
        if(!sf) {
            printf("spi flash probe failed\n");
            return 0;
        }
    }
	return spi_flash_write(sf, addr, cnt, src);
}
 
/* Function Name: 
 * 	nor_normal_patterns
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
int flash_normal_patterns(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *src_start;

	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < (sizeof(flash_patterns)/sizeof(u32_t)); i++)
	{
		_cache_flush();
		
		/* write pattern*/
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = flash_patterns[i];
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) setting pass\n", i, flash_patterns[i]);
		
		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check flash data sequentially. Uncached address */
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		
		/* check flash data interlevelingly. Uncached address */
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			
			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		

		/* check flash data sequentially. Cached address */
		src_start = (u32_t *)(TO_CACHED_ADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		
		/* check flash data interlevelingly. Uncached address */
		src_start = (u32_t *)(TO_CACHED_ADDR((u32_t)flash_start));
		for(j=0; j < (test_size_per_pattern/2); j=j+4)
		{
			start_value = (*src_start);
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					(u32_t)src_start , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			
			start_value = *(src_start + ((unsigned int)test_size_per_pattern/8));
			if(start_value != flash_patterns[i])
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)(src_start + ((unsigned int)test_size_per_pattern/8)) , start_value, flash_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}

		printf("Flash: pattern[%d](0x%x) 0x%x pass\n", i, flash_patterns[i], flash_start);
		printf("pattern[%d](0x%x) completed\n", i, flash_patterns[i]);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_walking_of_1(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t walk_pattern;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();
		
		/* generate pattern */
		walk_pattern = (1 << i);
		
		/* write pattern*/
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) setting passed\n", i, walk_pattern);

		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);


		/* check data */  
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_walking_of_0(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t walk_pattern;
	u32_t flash_start;
	volatile u32_t *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();
		
		/* generate pattern */
		walk_pattern = ~(1 << i);
		
		/* write pattern*/
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = walk_pattern;
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) setting passed\n", i, walk_pattern);
		
		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != walk_pattern)
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash:pattern[%d](0x%x) 0x%x passed\n", i, walk_pattern, flash_start);
		printf("pattern[%d](0x%x) completed\n", i, walk_pattern);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_addr_rot(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *_dram_start;
	volatile u32_t *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();
		
		/* write pattern*/
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ((u32_t)src_start << i);
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((u32_t)src_start) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				      (u32_t)src_start , start_value, (((u32_t)src_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: rotate %d setting passed\n", i);
		

		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);

		/* check data */  
		_dram_start = (u32_t *) src_data_addr;
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (((u32_t)_dram_start) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, (((u32_t)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}
		printf("Flash: rotate %d 0x%x passed\n", i, flash_start);
		
		printf("rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int flash_com_addr_rot(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t flash_start;
	volatile u32_t *_dram_start;
	volatile u32_t *src_start;
	
	printf("=======start %s test=======\n", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	{
		_cache_flush();
		
		/* write pattern*/
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			*src_start = ~(((u32_t)src_start) << i);
			src_start++;
		}
		
		_cache_flush();
		
		/* check data */  
		src_start = (u32_t *)src_data_addr;
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((u32_t)src_start << i)))
			{
				 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				        (u32_t)src_start , start_value, ~(((u32_t)src_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
		}
		printf("Flash: ~rotate %d setting passed\n", i);
		
		_cache_flush();
		
		src_start = (u32_t *)src_data_addr;
		flash_start = flash_start_addr + ((i*test_size_per_pattern)%flash_test_size);
		flash_sect_erase(flash_start, (flash_start+test_size_per_pattern-1));
		flash_write((char *)src_start, flash_start, test_size_per_pattern);
		
		/* check data */  
		_dram_start = (u32_t *) src_data_addr;
		src_start = (u32_t *)(TO_UNCACHED_ADDR((u32_t)flash_start));
		for(j=0; j < test_size_per_pattern; j=j+4)
		{
			start_value = (*src_start);
			if(start_value != (~((u32_t)_dram_start << i)))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				(u32_t)src_start , start_value, ~(((u32_t)_dram_start) << i),  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			src_start++;
			_dram_start++;
		}
		printf("Flash: ~rotate %d 0x%x passed\n", i, flash_start);
		
		printf("~rotate %d completed\n", i);
	}
	printf("%s test succeed.\n", __FUNCTION__);
	return MT_SUCCESS;
}

int _flash_test(u32_t flash_start_addr, u32_t test_size_per_pattern, u32_t flash_test_size)
{
	/* partial range */
	if(MT_SUCCESS != flash_normal_patterns( flash_start_addr, test_size_per_pattern, flash_test_size)){
		HANDLE_FAIL;
	}
	if(MT_SUCCESS != flash_walking_of_1( flash_start_addr, test_size_per_pattern, flash_test_size)){
		HANDLE_FAIL;
	}
	if(MT_SUCCESS != flash_walking_of_0( flash_start_addr, test_size_per_pattern, flash_test_size)){
		HANDLE_FAIL;
	}
	if(MT_SUCCESS != flash_addr_rot( flash_start_addr, test_size_per_pattern, flash_test_size)){
		HANDLE_FAIL;
	}
	if(MT_SUCCESS != flash_com_addr_rot( flash_start_addr, flash_test_size, flash_test_size)){
		HANDLE_FAIL;
	}
	return MT_SUCCESS;

}

extern void spi_flash_init(void);

int flash_test(int flag, int argc, void* argv[])
{
	volatile u32_t *bootcode;
	volatile u32_t *bk_buffer;
	int32 retcode=MT_SUCCESS;;
	u32_t i,j,testLoops=1;
	u32_t flash_backup_addr;

	if(argc>1) {
		testLoops = simple_strtoul(argv[1], NULL, 10);
	}

	if(parameters.flash_init_result == INI_RES_UNINIT) {
		spi_flash_init();
    }
	flash_size = 1 << para_flash_info.size_per_chip;
    
	if(!sf)	{
        sf = spi_flash_probe(0, 0, 0, 0);
        if(!sf)	{
            printf("spi flash probe failed\n");
            return MT_FAIL;
        }
    }

	if(((CONFIG_SYS_TEXT_BASE&0x0FFFFFFF)-JFFS2_START) > (flash_size+TEST_SIZE_PER_PATTREN)){
		flash_backup_addr = (0x80000000);
		src_data_addr = flash_backup_addr+flash_size;
	}else if((initdram(0)-((CONFIG_SYS_TEXT_BASE&0x0FFFFFFF)+JFFS2_START)) > (flash_size+TEST_SIZE_PER_PATTREN)){
		flash_backup_addr = (0x80000000+((CONFIG_SYS_TEXT_BASE&0x0FFFFFFF)+JFFS2_START));
		src_data_addr = flash_backup_addr+flash_size;
	}else{
		printf("There is no enough space to back up the SPI-F data, please check your u-boot base address:\n");
		printf("DRAM size: %dMB\n",initdram(0)/(0x100000));
		printf("U-Boot Base: 0x%x\n",CONFIG_SYS_TEXT_BASE);
		return MT_FAIL;
	}

    printf("flash size = %ldMB\n", flash_size / (0x100000));
	printf("FLASH_BACKUP_ADDR: 0x%x\n",flash_backup_addr);
	printf("U-Boot Base: 0x%x\n",CONFIG_SYS_TEXT_BASE);
	printf("src_data_addr: 0x%x\n",src_data_addr);


	for(j=0;j<testLoops;j++) {
		/* 0. Back up and verify whole flash data. */
		printf("Backing up %ldMB flash data....\n",flash_size / (0x100000));
		bk_buffer = (u32_t *)flash_backup_addr;
		bootcode = (u32_t *)FLASHBASE;
		for(i=0; i<flash_size; i=i+4) {
			*bk_buffer = *bootcode;
			bk_buffer++;
			bootcode++;
		}
		bk_buffer = (u32_t *)flash_backup_addr;
		bootcode = (u32_t *)FLASHBASE;
		for(i=0; i<flash_size; i=i+4) {
			if(*bk_buffer != *bootcode) {
			printf("#Back up flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n",\
				(u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
			}

			bk_buffer++;
			bootcode++;
		}

		/* 1. Non Boot loader area, in case of unrecoverable operation. */
		retcode = _flash_test((FLASHBASE+JFFS2_START),TEST_SIZE_PER_PATTREN, (flash_size - JFFS2_START));
		if(retcode < 0) {
			printf("No recover other data\n");
			return MT_FAIL;
		}
		
		/* 2. Boot loader area. */
		retcode = _flash_test((FLASHBASE), TEST_SIZE_PER_PATTREN, JFFS2_START);

		/* 
		 * 3. Copy back and verify data into the flash. 
		 */
		/* 3.1 Loader code */
		printf("Recover loader code:\n");
		flash_sect_erase( FLASHBASE, (FLASHBASE+JFFS2_START-1));
		flash_write((char *)flash_backup_addr, FLASHBASE, JFFS2_START);

		/* 3.2 Other data */
		printf("Recover data other than loader code:\n");
		flash_sect_erase((FLASHBASE+JFFS2_START), FLASH_TOP_ADDRESS);
		flash_write((char *)(flash_backup_addr+JFFS2_START), (FLASHBASE+JFFS2_START), (flash_size-JFFS2_START));

		/* 3.3 Verify data */
		printf("Verify recovered data: ");
		bk_buffer = (u32_t *)flash_backup_addr;
		bootcode = (u32_t *)FLASHBASE;
		for(i=0; i<flash_size; i=i+4) {
			if(*bk_buffer != *bootcode) {
			printf("#Recover flash data error: bk_addr(0x%08x):0x%08x != flash_addr(0x%08x):0x%08x\n",\
				(u32_t)bk_buffer, *bk_buffer, (u32_t)bootcode, *bootcode);
				HANDLE_FAIL;
			}
			bk_buffer++;
			bootcode++;
		}
		if(i==flash_size) {
			printf("Verify OK.\n");
		}
	}//for(j=0;j<testLoops;j++)
	return retcode;
}


