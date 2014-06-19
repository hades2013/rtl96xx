
/*
 * Include Files
 */
//#include <common.h>
//#include <exports.h>
//#include "board_common.h"
#include <soc.h>
#include <pblr.h>
#include "memctl.h"
#define _cache_flush	pblr_dc_flushall


/*Platform independent parameter*/
#ifdef CONFIG_MT_ERR_HANDLE
#define HANDLE_FAIL return -1;
#else
#define HANDLE_FAIL while(1);
#endif

/*Cases dependent parameter*/
#define TAREA_MAX		(2)
#define TAREA1_START	(0x80000000)
#define TAREA1_END		(CONFIG_SYS_TEXT_BASE - 0x300000)
#define TAREA2_START	(CONFIG_SYS_TEXT_BASE + 0x300000)
#define TAREA1_SIZE		(unsigned int)((TAREA1_END&0x1FFFFFFF))	// offset 0x0~TEXT BASE-0x300000
#define TAREA2_SIZE		(unsigned int)(initdram(0)-(TAREA2_START&0x1FFFFFFF))	// offset+0x4000000~end
#define CFG_DCACHE_SIZE (0x20)

#define MEMCTL_DEBUG_PRINTF printf
//#define MEMCTL_DEBUG_PRINTF(...)

const unsigned int dram_patterns[] =	{
					0x00000000,
					0xffffffff,
					0x55555555,
					0xaaaaaaaa,
					0x01234567,
					0x76543210,
					0x89abcdef,
					0xfedcba98,
					0xA5A5A5A5,
					0x5A5A5A5A,
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x0000FFFF,
					0xFFFF0000,
					0x00FFFF00,
					0xFF0000FF,
					0x5A5AA5A5,
					0xA5A55A5A,
				};

const unsigned int line_toggle_pattern[] = {
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x00000000,
					0xFFFFFFFF,
					0x5A5A5A5A,
					0xA5A5A5A5
				};

const unsigned int toggle_pattern[] = {
					0xA5A5A5A5,
					0x5A5A5A5A,
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x0000FFFF,
					0xFFFF0000,
					0x00FFFF00,
					0xFF0000FF,
					0x5A5AA5A5,
					0xA5A55A5A,
				};

/*
 * Function Declaration
 */
extern unsigned int board_DRAM_freq_mhz(void);

void (*f)(void) = (void *) 0xbfc00000;

int dram_normal_patterns(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    volatile u32_t *read_start;
    volatile u32_t *start;


    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = dram_patterns[i];
            start++;
        }

	_cache_flush();

        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
	    start_value = (*read_start);
            if(start_value != dram_patterns[i])
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);

    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}



int dram_walking_of_1(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t walk_pattern;
    u32_t start_value;
    volatile u32_t *read_start;
    volatile u32_t *start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i++)
    {

	_cache_flush();

        /* generate pattern */
        walk_pattern = (1 << i);

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = walk_pattern;
            start++;
        }

	_cache_flush();

        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
	    start_value = (*read_start);
            if(start_value != walk_pattern)
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}



int dram_walking_of_0(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t walk_pattern;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i++)
    {

	_cache_flush();

        /* generate pattern */
        walk_pattern = ~(1 << i);

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = walk_pattern;
            start++;
        }

	_cache_flush();

        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != walk_pattern)
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

int dram_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    //for (i=0; i < 4; i=i+4)
    {

	_cache_flush();
        /* write pattern*/
        start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
        read_start 	= (u32_t *)((u32_t)start + dram_size-4);
        read_start_addr = ((u32_t)start);
        start = (u32_t *)(UADDR(((u32_t)start)));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = (read_start_addr << i);
            start++;
            read_start_addr = read_start_addr + 4 ;
        }

	_cache_flush();
	read_start_addr = ((u32_t)read_start);
        /* check data reversing order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != ((read_start_addr) << i))
            {
                 printf("decr addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ((read_start_addr) << i), 
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr - 4;
            read_start--;
        }

	read_start_addr += 4;
        read_start++;

        /* check data sequential order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != ((read_start_addr) << i))
            {
                 printf("seq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ((read_start_addr) << i), \
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr + 4;
            read_start++;
        }

	MEMCTL_DEBUG_PRINTF("rotate %d 0x%x passed\r", i, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

int dram_com_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    //for (i=0; i < 4; i=i+4)
    {

	_cache_flush();
        /* write pattern*/
        start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
        read_start 	= (u32_t *)((u32_t)start + dram_size-4);
        read_start_addr = ((u32_t)start);
        start = (u32_t *)(UADDR(((u32_t)start)));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ~(read_start_addr << i);
            start++;
            read_start_addr = read_start_addr + 4 ;
        }

	_cache_flush();
	read_start_addr = ((u32_t)read_start);
        /* check data reversing order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("decr addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ~((read_start_addr) << i),
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr - 4;
            read_start--;
        }

	read_start_addr += 4;
        read_start++;

        /* check data sequential order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("seq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ~((read_start_addr) << i), 
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr + 4;
            read_start++;
        }

	MEMCTL_DEBUG_PRINTF("~rotate %d 0x%x passed\r", i, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}
#if 0
void dram_com_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    {

	_cache_flush();

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start_addr = (u32_t)start + dram_size - 4;
        read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ~(((u32_t)start) << i);
            start++;
        }

	_cache_flush();

        /* check data sequential order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        read_start , start_value, ~((read_start_addr) << i),\
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
            read_start_addr = read_start_addr - 4;
        }

	read_start_addr += 4;
        read_start++;
        /* check data reversing order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        read_start , start_value, ~((read_start_addr) << i),\
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start++;
            read_start_addr = read_start_addr + 4;
        }

	printf("~rotate %d 0x%x passed\r", i, start);

    }
    printf("%s test completed.\r", __FUNCTION__);
    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}
#endif
/*
 * write two half-words and read word.
 */
int dram_half_word_access(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u16_t h_word;
    u32_t start_value;
    volatile u16_t *start_h;
    volatile u32_t *start_w;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++)
    {
	_cache_flush();

        /* write half-word pattern*/
        start_h = (u16_t *)(dram_start+ (i*dram_size)%(area_size));
        start_w = (u32_t *)(UADDR(dram_start+ (i*dram_size)%(area_size)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            h_word = (u16_t)(dram_patterns[i]);
            *(start_h+1) = h_word;
            h_word = (u16_t)(dram_patterns[i] >> 16);
            *start_h = h_word;
            start_h+=2;
        }

	_cache_flush();

        /* read word and check data */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*start_w);
            if(start_value != dram_patterns[i])
            {
                 printf("addr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",
                       (u32_t)start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            start_w--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start_h);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;

}

int dram_byte_access(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u8_t byte;
    u32_t start_value;
    volatile u8_t *start_b;
    volatile u32_t *start_w;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++)
    {

	_cache_flush();

        /* write byte pattern*/
        start_w = (u32_t *)(UADDR(dram_start+(i*dram_size)%(area_size)+dram_size-4));
        start_b = (u8_t *)(dram_start+(i*dram_size)%(area_size));
        for(j=0; j < dram_size; j=j+4)
        {
            byte = (u8_t)(dram_patterns[i]);
            *(start_b+3) = byte;
            byte = (u8_t)(dram_patterns[i] >> 8);
            *(start_b+2) = byte;
            byte = (u8_t)(dram_patterns[i] >> 16);
            *(start_b+1) = byte;
            byte = (u8_t)(dram_patterns[i] >> 24);
            *(start_b) = byte;
            start_b+=4;
        }

	_cache_flush();

        /* read word and check data */  
        for (j=0; j < dram_size; j=j+4)
        {
            start_value = *start_w;
            if (start_value != dram_patterns[i])
            {
                 printf("addr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",
                       (u32_t)start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            start_w--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start_b);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

int memcpy_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    {

	_cache_flush();

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start_addr = ((u32_t)start + (dram_size-4));
        read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ((u32_t)start << i);
            start++;
        }


	memcpy((char *)(dram_start+dram_size), (char *)(dram_start + (i*dram_size)%(area_size)), dram_size);

	_cache_flush();

        /* check uncached data */  
        read_start = (u32_t *)(dram_start+dram_size+dram_size-4);
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != ((read_start_addr) << i))
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ((read_start_addr) << i), 
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start = read_start - 1;
            read_start_addr = read_start_addr - 4;
        }
	MEMCTL_DEBUG_PRINTF("memcpy %d 0x%x passed\r", i, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}


#pragma pack(1)
/* Data structures used for testing unaligned load/store operations. */
typedef struct{
	unsigned char c1;
	unsigned int w;
} t_off_1;

typedef struct{
	unsigned char c1;
	unsigned char c2;
	unsigned int w;
} t_off_2;
typedef struct{
	unsigned char c1;
	unsigned char c2;
	unsigned char c3;
	unsigned int w;
} t_off_3;

#pragma pack(4)
int unaligned_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	unsigned int i, j;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;

	off1 = (t_off_1 *)dram_start;
	off2 = (t_off_2 *)dram_start;
	off3 = (t_off_3 *)dram_start;


	/* patterns loop */
	for(j=0; j<sizeof(dram_patterns)/sizeof(unsigned int);j++)
	{
		/* offset 1 bytes */
         	off1 = (t_off_1 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			off1[i].c1 = 0xcc;
			off1[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			if((off1[i].w != dram_patterns[j]) || (off1[i].c1 != 0xcc)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 1 error:addr(0x%x) write 0x%x, read 0x%x, c1(%02x)\n", 
					(u32_t)&off1[i], dram_patterns[j], off1[i].w, off1[i].c1);
				goto test_fail;
			}
		}
	
	
		/* offset 2 bytes */
         	off2 = (t_off_2 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			if(off2[i].w != dram_patterns[j] || (off2[i].c1 != 0xcc) || (off2[i].c2 != 0xdd)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 2 error:addr(0x%x) write 0x%x, read 0x%x, c1(0x%x), c2(0x%x)\n", 
					(u32_t)&off2[i], dram_patterns[j], off2[i].w, off2[i].c1, off2[i].c2);
				printf("&dram_pattern[%d](0x%p) = 0x%x\r", j, &dram_patterns[j], dram_patterns[j]);
				goto test_fail;
			}
		}

		/* offset 3 bytes */
         	off3 = (t_off_3 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			if(off3[i].w != dram_patterns[j] ||(off3[i].c1 != 0xcc) || (off3[i].c2 != 0xdd)\
			 || (off3[i].c3 != 0xee)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 3 error:addr(0x%x) write 0x%x, "
					"read 0x%x, c1(%02x), c2(%02x), c3(%02x)\n", \
					(u32_t)&off1[i], dram_patterns[j], off3[i].w, off3[i].c1, 
					off3[i].c2, off3[i].c3);
				goto test_fail;
			}
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", j, dram_patterns[j], (u32_t)off3);

	}
   	 MEMCTL_DEBUG_PRINTF("%s test passed.\r", __FUNCTION__);
	 return 0;
test_fail:
   	 printf("%s test failed.\n", __FUNCTION__);
	 puts("test failed\n");
	 HANDLE_FAIL;

}



unsigned int disable_DRAM_prefech(unsigned int side_id)
{
	unsigned int old_mcr;

	old_mcr = REG32(MCR);
	if( side_id & MCR_PREFETCH_INS_SIDE )
		REG32(MCR) = REG32(MCR) & ((unsigned int)MCR_PREFETCH_DIS_IMASK);
	if( side_id & MCR_PREFETCH_DATA_SIDE)
		REG32(MCR) = REG32(MCR) & ((unsigned int)MCR_PREFETCH_DIS_DMASK);

	return old_mcr;
}

unsigned int enable_DRAM_prefech(unsigned int mode, unsigned int side_id)
{
	unsigned int old_mcr;
	unsigned int tmp_mode;

	tmp_mode = 0;
	old_mcr = REG32(MCR);


	if( side_id & MCR_PREFETCH_INS_SIDE ){
		disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE);
		if( mode == MCR_PREFETCH_OLD_MECH)
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_IOLD | MCR_PREFETCH_ENABLE_INS;
		else
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_INEW | MCR_PREFETCH_ENABLE_INS;

		
	}
	if( side_id & MCR_PREFETCH_DATA_SIDE ){
		disable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
		if( mode == MCR_PREFETCH_OLD_MECH)
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_DOLD | MCR_PREFETCH_ENABLE_DATA;
		else
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_DNEW | MCR_PREFETCH_ENABLE_DATA;
	}

	return old_mcr;
}

/*Cases dependent parameters*/
#define INIT_VALUE (0x5A5AA5A5)
#define BACKGROUND_VALUE (0xDEADDEAD)
#define GET_SEED 1
#define SET_SEED 0
#define RANDOM_TEST
#define TEST_TIMES (0x1)
//#define DIFF_ROWS
//#define USE_BYTESET /* exclusive with DIFF_ROWS */
/*
        get_or_set = GET_SEED: get seed
        get_or_set = SET_SEED: set seed
*/
static void __srandom32(int *a1, int *a2, int *a3, int get_or_set)
{
        static int s1, s2, s3;
        if(GET_SEED==get_or_set){
                *a1=s1;
                *a2=s2;
                *a3=s3;
        }else{
                s1 = *a1;
                s2 = *a2;
                s3 = *a3;
        }
}

static unsigned int __random32(void)
{
#define TAUSWORTHE(s,a,b,c,d) ((s&c)<<d) ^ (((s <<a) ^ s)>>b)
        int s1, s2, s3;
        __srandom32(&s1, &s2, &s3, GET_SEED);

        s1 = TAUSWORTHE(s1, 13, 19, 4294967294UL, 12);
        s2 = TAUSWORTHE(s2, 2, 25, 4294967288UL, 4);
        s3 = TAUSWORTHE(s3, 3, 11, 4294967280UL, 17);

        __srandom32(&s1, &s2, &s3, SET_SEED);

        return (s1 ^ s2 ^ s3);
}

int cache_flush_adj_addr(unsigned int addr_base, unsigned int run_times, \
			 unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		MEMCTL_DEBUG_PRINTF("\rADDRESS WORDSET addr_base: 0x%08x times: %d, pattern: Address ", addr_base, test_times);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = (unsigned int)pdata;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != ((unsigned int)pdata))
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , ((unsigned int)pdata));
				retcode = -1;
			}
			pdata++;
		}
	}
    
	return (retcode);
}

int cache_flush_adjacent(unsigned int addr_base, unsigned int run_times, \
			 unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode, a, b, c;
	unsigned char vbyte;

	retcode = 0;
	vbyte = 0;

	write_value = INIT_VALUE;
	a=0x13243;b=0xaaa0bdd;c=0xfffbda0;
	__srandom32(&a, &b, &c, SET_SEED);
	for(test_times = 0; test_times < run_times; test_times++)
	{
		if(random == 1){
			MEMCTL_DEBUG_PRINTF("\rRANDOM ");
			write_value = __random32();
		}
		else
			MEMCTL_DEBUG_PRINTF("\rFIXED  ");
		
		if(byteset == 1){
			MEMCTL_DEBUG_PRINTF("BYTESET ");
			vbyte = (unsigned char)write_value;
			write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) \
			| (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
		}
		else
			MEMCTL_DEBUG_PRINTF("WORDSET ");
	
		MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		if(byteset == 1){
			memset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
		}
		else{

			for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
			{
				*pdata = write_value;
				pdata++;
			}
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		}
	}

	return (retcode);
}


int cache_flush_dispersed (unsigned int addr_base, unsigned int run_times,\
			   unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode, a, b, c;
	unsigned char vbyte;

	retcode = 0;
	vbyte = 0;

	/* 
	 * 8198 CPU configuraton:
	 * Dcache size 8KB
	 * ICache 16KB
	 * No L2 Cache
	 * Cache line 8 words
	 */
	write_value = INIT_VALUE;
	a=0x13243;b=0xaaa0bdd;c=0xfffbda0;
	__srandom32(&a, &b, &c, SET_SEED);
	for(test_times = 0; test_times < run_times; test_times++)
	{

		if(random == 1)	{
			MEMCTL_DEBUG_PRINTF("\rRANDOM ");
			write_value = __random32();
		}
		else
			MEMCTL_DEBUG_PRINTF("\rFIXED  ");
			
		if(byteset == 1){
			MEMCTL_DEBUG_PRINTF("BYTESET ");
			vbyte = (unsigned char)write_value;
			write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) \
			| (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
		}
		else
			MEMCTL_DEBUG_PRINTF("WORDSET ");
		
		MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}
	

		pdata = (unsigned int *)(CADDR(addr_base));
		if(byteset == 1){
			memset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
		}
		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}
		/* write back and invalidate DCache */
		_cache_flush();


		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}
	}


	return (retcode);
}




int cache_flush_adjacent_toggle_word(unsigned int addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
	
		MEMCTL_DEBUG_PRINTF("\raddr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = write_value;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		}
	}


	return (retcode);
}


int cache_flush_dispersed_toggle_word (unsigned int addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		MEMCTL_DEBUG_PRINTF("\raddr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}
	

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
#if 0
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}

			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}

			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}

			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}
#else
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}

#endif
	}


	return (retcode);
}


int cache_flush_adjacent_toggle_line128(unsigned int addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value[4];
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		write_value[0] = line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[1] = line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[2] = ~line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[3] = ~line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		MEMCTL_DEBUG_PRINTF("\raddr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value[0]);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+16)
		{
			*pdata++ = write_value[0];
			*pdata++ = write_value[1];
			*pdata++ = write_value[2];
			*pdata++ = write_value[3];
		}

		/* write back and invalidate DCache */
		_cache_flush();


		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+16)
		{
			data = *pdata;
			if(data != write_value[0])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[0]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[1])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[1]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[2])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[2]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[3])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[3]);
				retcode = -1;
			}
			pdata++;
		}
	}

	return (retcode);
}

int cache_flush_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	int retcode;
	unsigned int addr_base;
	unsigned int test_times;
	test_times = TEST_TIMES;

	MEMCTL_DEBUG_PRINTF("test area size = 0x%08x\n",area_size);

	for(addr_base = dram_start; addr_base < (dram_start + area_size) ;\
		addr_base = addr_base + dram_size)
	//for(addr_base = dram_start+0x6c00000; addr_base < (dram_start + area_size) ;
	//	addr_base = addr_base + dram_size)
	{
		retcode = cache_flush_adjacent(addr_base , test_times, 0, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
#if 1
		retcode = cache_flush_adjacent(addr_base, test_times, 1, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = cache_flush_dispersed(addr_base, test_times, 0, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
#endif
#if 1
		retcode = cache_flush_dispersed(addr_base, test_times, 1, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = cache_flush_adjacent(addr_base, test_times, 0, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = cache_flush_adjacent(addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = cache_flush_dispersed(addr_base, test_times, 0, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = cache_flush_dispersed(addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = cache_flush_adj_addr(addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = cache_flush_adjacent_toggle_word(addr_base, test_times);
		if(retcode < 0){
			printf("cache_flush_adjacent_toggle_word error\n");
			goto test_fail;
		}
		retcode = cache_flush_dispersed_toggle_word(addr_base, test_times);
		if(retcode < 0){
			printf("cache_flush_dispersed_toggle_word error\n");
			goto test_fail;
		}
		retcode = cache_flush_adjacent_toggle_line128(addr_base, test_times);
		if(retcode < 0){
			printf("cache_flush_adjacent_toggle_line128 error\n");
			goto test_fail;
		}
#endif
	}
	MEMCTL_DEBUG_PRINTF("\n");
	return 0;
test_fail:
	HANDLE_FAIL;
}

int _dram_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    if( (0==unaligned_test( dram_start, dram_size, area_size)) 
    	&& (0==dram_addr_rot(dram_start, dram_size, area_size))
    	&& (0==dram_com_addr_rot(dram_start, dram_size, area_size))
  	  	&& (0==dram_byte_access(dram_start, dram_size, area_size))
  	  	&& (0==dram_half_word_access(dram_start, dram_size, area_size))
  	  	&& (0==cache_flush_test( dram_start, dram_size, area_size))
  	  	&& (0==dram_normal_patterns(dram_start, dram_size, area_size))
  	  	&& (0==dram_walking_of_1(dram_start, dram_size, area_size))
  	  	&& (0==dram_walking_of_0(dram_start, dram_size, area_size))
  	  	&& (0==memcpy_test(dram_start, dram_size, area_size))
  	  	&& (0==dram_addr_rot(dram_start, area_size, area_size))
  	  	&& (0==dram_com_addr_rot(dram_start, area_size, area_size)))
	{	return 0;     }
    return -1;
}

int dram_test (int flag, int argc, char *argv[])
{
	u32_t start_addr, test_area_size;
    u32_t dram_size, zidx=1;
    u32_t l, testLoops;
    volatile u32_t *mcr, oldmcr;

	if(argc > 1)
        testLoops = simple_strtoul(argv[1], NULL, 10);
    else
        testLoops = 1;
    
	/*back the value of mcr*/
	mcr = (u32_t *)MCR;
    oldmcr = *mcr;
    dram_size = 0x10000;

	for(l=1; l<=testLoops; l++) {
    	if(testLoops>1) MEMCTL_DEBUG_PRINTF("\n[Round %d]\n", l);
ZONE_START:
		if(zidx==2) {
	        start_addr = TAREA2_START;
	        test_area_size = TAREA2_SIZE;
		} else {
		    start_addr = TAREA1_START;
			test_area_size = TAREA1_SIZE;
	    }
		if((0 >= test_area_size)||(initdram(0)<test_area_size)) goto IDX_CHECK;

		/* disable prefetch mechanism */
		disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
		_cache_flush();
		MEMCTL_DEBUG_PRINTF("Range [0x%x~0x%x]\n", start_addr, start_addr+test_area_size-1);

		/*1. Uncached range */
	    MEMCTL_DEBUG_PRINTF("No prefetch, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
				    UADDR(start_addr), dram_size, *mcr);
	    if(-1 == _dram_test(UADDR(start_addr), dram_size, test_area_size))
            return -1;

		/*2. Cached range without prefetch */
	    MEMCTL_DEBUG_PRINTF("\nNo prefetch, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	                CADDR(start_addr), dram_size, *mcr);
	    if(-1 == _dram_test(CADDR(start_addr), dram_size, test_area_size))
            return -1;

		disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

		/*3. Cached range with data prefetch mechanism */
		enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_DATA_SIDE);
		MEMCTL_DEBUG_PRINTF("\nEnable MCR_PREFETCH_DATA_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	    		    CADDR(start_addr), dram_size, *mcr);
	   if(-1 == _dram_test(CADDR(start_addr), dram_size, test_area_size))
        	return -1;

		disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

		/*4. Cached range with instruction prefetch mechanism */
		enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_INS_SIDE);
	    MEMCTL_DEBUG_PRINTF("\nEnable MCR_PREFETCH_INS_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	               CADDR(start_addr), dram_size, *mcr);
	    if(-1 == _dram_test(CADDR(start_addr), dram_size, test_area_size))
            return -1;

		disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );


		/*5. Cached range with instruction/data prefetch mechanism */
		enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_DATA_SIDE | MCR_PREFETCH_INS_SIDE);
	    MEMCTL_DEBUG_PRINTF("\nEnable MCR_PREFETCH_DATA_SIDE/MCR_PREFETCH_INS_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	               CADDR(start_addr), dram_size, *mcr);
	    if(-1 == _dram_test(CADDR(start_addr), dram_size, test_area_size))
           return -1;

		disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

IDX_CHECK:
		if((++zidx)<=TAREA_MAX)
	        goto ZONE_START;
        else
            zidx=1;
	}
	/* restore memory controller register */
    *mcr = oldmcr;
	_cache_flush();
    MEMCTL_DEBUG_PRINTF("\n\n");
	return 0;
}
