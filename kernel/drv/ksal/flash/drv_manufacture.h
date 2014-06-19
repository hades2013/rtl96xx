#ifdef  __cplusplus
extern "C"{
#endif

#ifndef DRV_MANUFACTURE_H
#define DRV_MANUFACTURE_H

//#include <common/type.h>

#define ENV_VALID_FLAG   0xAA55AA55 //modify by dengjian 2012-08-29 for QID0013
#define ENV_INVALID_FLAG 0x0

#define ENV_HAVE_CFG   1
#define ENV_NO_CFG     0

#define MAX_BOOT_CONFIG_SIZE   0x1000

#define ENV_HEADER_SIZE	       (2*sizeof(unsigned long)) //modify by dengjian 2012-08-29 for QID0013
#define ENV_VALID_FLAGS_OFFSET (sizeof(unsigned long)) //modify by dengjian 2012-08-29 for QID0013
#define ENV_VALID_FLAGS_LEN    (sizeof(unsigned long))

#define ENV_SIZE (MAX_BOOT_CONFIG_SIZE - ENV_HEADER_SIZE)

typedef	struct environment_s {
	unsigned long	crc;		    /* CRC32 over data bytes	*/
	//unsigned long	data_len;	    /* the data length,	must be 4bytes align*/  //modify by dengjian 2012-08-13 
    unsigned long   valid_flags;    /* 标志当前的data是否有效，0xAA55AA55表示有效， 0表示无效 */
	unsigned char	data[ENV_SIZE]; /* Environment data		*/
} env_t;

//extern int printk(const char *s, ...);

//#define DBG_MANUFACTURE
#ifdef DBG_MANUFACTURE
#define DBG_MANUFACTURE_PRINT printk
#else
#define DBG_MANUFACTURE_PRINT(fmt,arg...)
#endif

bool GetBootCfg(env_t *pstBootCfg);
bool SaveBootCfg(env_t *pstBootCfg);

extern int zj_debug(struct seq_file *m, void *v);

#endif //DRV_MANUFACTURE_H
#ifdef  __cplusplus
}
#endif

