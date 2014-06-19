#ifndef __LW_CONFIG_API_H_
#define __LW_CONFIG_API_H_
 
#ifdef __cplusplus
extern "C" {
#endif

extern int  Cfginit(int flag);

extern int cfg_getval(int ifindex,unsigned int oid,void *val,void* default_val,unsigned retlen);

#define CFG_OK  0
#define CFG_ERR  -1

#define MASTER           1
#define SLAVER           0
#define TESTER           0xff

#define CONFIG_DEFAULTFILE "/etc/config/startup"
//#define CONFIG_MNT_START		"/mnt/startup"
extern int isMaster(void);


#ifdef __cplusplus
}
#endif
#endif

