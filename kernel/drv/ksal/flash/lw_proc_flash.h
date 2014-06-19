#include <linux/autoconf.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/stddef.h>
#include "drv_manufacture.h"
#include "lw_config.h"
#include <linux/version.h>

#define ETH_ADDR "ethaddr"
#define BOOT_FLAG "bootflag"
#define RESTORE_FLAG "restore_default"
#define HARD_VERSION  "hardversion"
#define BOOT_VERSION  "bootversion" 
#define MANU_NUM   "SN"  //serial number    
#define PROC_MANUFACTORY  "manufactory"

/*Begin add by huangmingjian 2014-01-08 */ 
#ifdef CONFIG_CONSOLE_DEBUG
#define CONSOLE_DEBUG "console_debug"
#endif
/*End add by huangmingjian 2014-01-08 */ 

#define BUFSIZE64     64
#define BUFSIZE32     32
#define BUFSIZE4      4
/*begin by leijinbao 2013/12/20 for mac Encryption*/
#define BUFSIZE88     89 
#define ETHADDR_SIZE BUFSIZE88
/*end by leijinbai 2013/12/20 for for mac Encryption*/
#define HARDVERSION_SIZE BUFSIZE64
#define BOTVERSION_SIZE BUFSIZE64
#define MANUCODE_SIZE BUFSIZE64
#define BOOTFLAG_SIZE BUFSIZE4
#define RESTOREFLAG_SIZE BUFSIZE4

/*Begin modify by dengjian 2012-11-28 for EPN204QID0015*/
#ifdef CONFIG_BOSA
#define MODSETVAL  "modsetval"
#define APCSETVAL  "apcsetval"
#define BUFSIZE5      5
#define MODSETVAL_SIZE BUFSIZE5
#define APCSETVAL_SIZE BUFSIZE5
#endif
/*End modify by dengjian 2012-11-28 for EPN204QID0015*/

#ifdef CONFIG_BOSA  //add by zhouguanhua 2013/6/14
#define FTOVERFLAG "FToverflag"
#define FTOVERFLAG_SIZE BUFSIZE5
#endif 



typedef struct _proc_item {
	const char *name;
	int (*read_func) (struct seq_file *m,void *v);
	int (*write_func) (char *buffer, unsigned long count);
} proc_item;

typedef struct factoryinfo{
    char ethaddr [ETHADDR_SIZE];           //MAC地址
    char hardversion[HARDVERSION_SIZE];       // 硬件版本号
	char bootversion[BOTVERSION_SIZE];       // boot版本号
	char manucode [MANUCODE_SIZE];        // 生产序列号
    char bootflag[BOOTFLAG_SIZE];          //加载主/备APP标记
    char restore_default[RESTOREFLAG_SIZE];   //APP是否恢复默认配置标记
    #ifdef CONFIG_BOSA
    char modsetval[MODSETVAL_SIZE];    //调制电流寄存器值
    char apcsetval[APCSETVAL_SIZE];    //偏置电流寄存器值
    char ftoverflag[FTOVERFLAG_SIZE];
    #endif
/*Begin add by huangmingjian 2014-01-08 */ 	
#ifdef CONFIG_CONSOLE_DEBUG
    char console_debug_flag[BUFSIZE4];  /*console debug enable or disable*/
#endif
/*End add by huangmingjian 2014-01-08 */ 
}factory_data_s;

