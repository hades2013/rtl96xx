#ifndef __HEXICOMTC_H__
#define __HEXICOMTC_H__


#include "hexicom.h"
/*
SIZE 
*/

#define HC_MAC_ADDRESS_SIZE 6
#define HC_IP_ADDRESS_SIZE  4

#define HC_SYS_MODEL_SIZE 64
#define HC_SYS_VERSION_SIZE 64
#define HC_SYS_NAME_SIZE 64
#define HC_SYS_DESC_SIZE 128
#define HC_MIB_VERSION_SIZE 64
#define HC_HOSTNAME_SIZE 64

#define HC_ADMIN_SIZE 16
#define HC_IFNAME_SIZE 64

#define HC_VIDLIST_SIZE 4096

typedef unsigned char vlan_portlist_t;

#define HC_VLAN_PORT_SIZE  sizeof(vlan_portlist_t)


#define HC_URL_SIZE 128

#define HC_INTEGER_SIZE  sizeof(long)

/*
ENUM
*/
typedef enum {
	HC_VENDOR_HEXICOM = 1,
	HC_VENDOR_HUANSHUN	
}
hc_oemid_t;



/*
CONST
*/

#ifdef FOR_HUANSHUN
#define HC_OEMID  HC_VENDOR_HUANSHUN
#endif 

#ifndef HC_OEMID
#define HC_OEMID HC_VENDOR_HEXICOM
#endif 

#define HC_CACHE_TIMEOUT  	  5

#define HC_ROOT_OID 1,3,6,1,4,1,36368
#define HC_CLT_OBJ_OID HC_ROOT_OID,1,1,1,1

#define HC_CABLE_NUM  2
#define HC_MAX_TMPL_NUM  EOC_MAX_TMPL_NUMS
#define HC_MAX_USER_NUM  EOC_MAX_USER_NUMS
#define HC_ETHERNET_NUM  2

#define HC_PORD_MODEL PROD_MODEL

#define HC_MIB_VERSION "201106021300Z"

#define HC_MAX_TMPL_NAME_SIZE 31 // sync with web
#define HC_MAX_TMPL_DESC_SIZE 63 // sync with web

#define HC_MAX_USER_NAME_SIZE 31 // sync with web
#define HC_MAX_USER_DESC_SIZE 63 // sync with web


#define FLOAT_MULTIPLE  100

#define f2ul(f) (unsigned long)(f * FLOAT_MULTIPLE)

/*
OTHERS
*/

#define LOCAL_COMMIT_DECLARE() \
static int __local_valid = 0; \
static int __commit_pending = 0; \
static int __local_commit(int ms, SNMPAlarmCallback * cb, void *cd) \
{ \
	struct timeval t; \
	static int commit_id = 0; \
	__commit_pending = 1; \
	t.tv_sec = 0; \
	t.tv_usec = ms * 1000; \
	if (commit_id){ \
		snmp_alarm_unregister(commit_id); \
		commit_id = 0; \
	} \
	if (ms == 0){ \
		__commit_pending = 0; \
		cb(0, cd); \
	}else { \
		commit_id = snmp_alarm_register_hr( t, 0, cb, cd); \
		if (commit_id == 0){ \
			__commit_pending = 0; \
			snmp_log(LOG_ERR,"could not register alarm\n"); \
			return 0;	 \
		} \
	} \
	return 1; \
} 

#define LOCAL_COMMIT __local_commit
#define SET_VALID() (__local_valid = 1)
#define CLR_VALID() (__local_valid = 0)

#define LOCAL_VALID() (__local_valid)

#define COMMIT_CLEAR_PENDING() (__commit_pending = 0)

#define COMMIT_PENDING_ASSERT() do {\
								if (__commit_pending == 1){ \
									snmp_log(LOG_INFO,"Commit is pending..\n"); \
									return 0;\
								}\
							}while(0) 

#define HC_VALID_ASSERT() do {\
								if (!__local_valid){\
									snmp_log(LOG_ERR, "Local data invalid\n"); \
									return ;\
								}\
							}while(0) 



#define HC_DBG_OID(vb) \
DEBUGMSGTL(( DBGID, "oid: ")); \
DEBUGMSGOID((DBGID, (vb)->name, (vb)->name_length)); \
DEBUGMSG((DBGID, "\n"))

#define HC_DBG_MAC 0x0,0x0,0x21,0x01,0x14,0x14 

extern int hc_max_supported_device_num;

#define vb_strdup(vb) snmp_strdup((vb)->val.string, (vb)->val_len)
/*#define hc_array_set(vb, a) \
	memset(a, 0, sizeof(a)); \
	memcpy(a, (vb)->val.string, (vb)->val_len)
*/		
#define vb_str_set(vb, a) \
	memcpy(a, (vb)->val.string, (vb)->val_len); \
	a[(vb)->val_len] = 0

#define vb_int_set(vb, i) ((i) = *(vb)->val.integer)
#define vb_int(vb) (*(vb)->val.integer)


#define DBGID "hexicom"


/*
Here are some definition of SNMP textual convention
*/
enum{
	ConfigAction_restore = 1,
	ConfigAction_save 	
};


enum {
	RowStatus_active = 1,
	RowStatus_notInService,	
	RowStatus_notReady,
	RowStatus_createAndGo,
	RowStatus_createAndWait,
	RowStatus_destroy
};

enum {
	TruthValue_true = 1,
	TruthValue_false 
};

enum {
	VLANTAG_untagged = 1,
	VLANTAG_tagged
};

enum {
	IPMode_static = 0,
	IPMode_dynamic
};


enum {
	AdminStatus_enabled = 1,
	AdminStatus_disabled
};


enum {
	IFType_cable = 1,
	IFType_ethernet
};

enum {
	IFLink_on = 1,
	IFLink_off	
};

enum{
	IFAdmin_on = 1,
	IFAdmin_off 
};

enum {
	IFDuplex_auto = 0,
	IFDuplex_full,
	IFDuplex_half
};


enum {
	IFFlowControl_on = 1,
	IFFlowControl_off
};

enum {
	IFSpeed_auto = 0,
	IFSpeed_10m,
	IFSpeed_100m,
	IFSpeed_1000m
};


enum {
	IFLineRate_unlimited = 0,
	IFLineRate_128Kbps,
	IFLineRate_256Kbps,
	IFLineRate_512Kbps,
	IFLineRate_1Mbps,
	IFLineRate_2Mbps,
	IFLineRate_4Mbps,
	IFLineRate_8Mbps
};



enum {
	IFPriority0 = 0,
	IFPriority1,		
	IFPriority2,
	IFPriority3,
	IFPriority4,		
	IFPriority5,
	IFPriority6,
	IFPriority7,
	IFPriorityDisabled,
};

enum{
	CNUAnonymousTimeout_unlimited = 0,
	CNUAnonymousTimeout_1minute,
	CNUAnonymousTimeout_1hour,
	CNUAnonymousTimeout_1day
};



enum{
	CNURefresh_done = 1,
	CNURefresh_scan,
	CNURefresh_scanning,	
};

enum {
	CNUACCESS_anonymous = 1, 
	CNUACCESS_white,
	CNUACCESS_black
};

enum{
	CNURowStatus_active = 1,
	CNURowStatus_towhile,
	CNURowStatus_toblack,
	CNURowStatus_reset
};

enum {
	CNUTopologyStatus_active = 1,
	CNUTopologyStatus_nonInService,
	CNUTopologyStatus_refresh
};


enum{
	USERRowStatus_white = 1,
	USERRowStatus_black,
	USERRowStatus_createAndWait,
	USERRowStatus_destroy
};


/*no used yet*/
enum {
	NUOnlineStatus_unkonw = 0,
	NUOnlineStatus_up,
	NUOnlineStatus_down,
	NUOnlineStatus_invalid,
	NUOnlineStatus_deny,
	NUOnlineStatus_others
};


enum{
	VLANMode_transparent = 1,
	VLANMode_dot1q
};

enum {
	RtcRowStatus_active = 1,
	RtcRowStatus_noneInService,
	RtcRowStatus_refresh,
	RtcRowStatus_set
};


#define validConfigAction(x)  (((x) <= ConfigAction_save) && ((x) >= ConfigAction_restore))	

#define validRtcRowStatus(x)  (((x) <= RtcRowStatus_set) && ((x) >= RtcRowStatus_active))	


#define validCNUTopologyStatus(x) ((x) == CNUTopologyStatus_refresh)


#define validVLANMode(x)  (((x) <= VLANMode_dot1q) && ((x) >= VLANMode_transparent))	

#define validCNUAnonymousTimeout(x) (((x) <= CNUAnonymousTimeout_1day) && ((x) >= CNUAnonymousTimeout_unlimited))

#define validUSERRowStatus(x)  (((x) <= USERRowStatus_destroy) && ((x) >= USERRowStatus_white))	

#define validCNUACCESS(x)  (((x) <= CNUACCESS_black) && ((x) >= CNUACCESS_anonymous))	
#define validCNURowStatus(x)  (((x) <= CNURowStatus_reset) && ((x) >= CNURowStatus_active))	

#define validRowStatus(x)  (((x) <= RowStatus_destroy) && ((x) >= RowStatus_active))		
#define validTruthValue(x)  (((x) <= TruthValue_false) && ((x) >= TruthValue_true))	
#define validVLANTAG(x)  (((x) <= VLANTAG_tagged) && ((x) >= VLANTAG_untagged))	
#define validIPMode(x)  (((x) <= IPMode_dynamic) && ((x) >= IPMode_static))	
#define validAdminStatus(x)  (((x) <= AdminStatus_disabled) && ((x) >= AdminStatus_enabled))	
#define validIFType(x)  (((x) <= IFType_ethernet) && ((x) >= IFType_cable))	
#define validIFLinkStatus(x)  (((x) <= IFLink_off) && ((x) >= IFLink_on))	
#define validIFAdminStatus(x)  (((x) <= IFAdmin_off) && ((x) >= IFAdmin_on))	
#define validIFFlowControl(x)  (((x) <= IFFlowControl_off) && ((x) >= IFFlowControl_on))		
#define validIFSpeed(x)  (((x) <= IFSpeed_1000m) && ((x) >= IFSpeed_auto))	
#define validIFDuplex(x)  (((x) <= IFDuplex_half) && ((x) >= IFDuplex_auto))	
#define validIFLineRate(x)  (((x) <= IFLineRate_8Mbps) && ((x) >= IFLineRate_unlimited))	
#define validIFPriority(x)  (((x) <= IFPriorityDisabled) && ((x) >= IFPriority0))	

#define	IP_OCTET(in, str) { \
	unsigned long _in = htonl(in.s_addr); \
	str[0] = (_in & 0xff000000) >> 24; \
	str[1] = (_in & 0x00ff0000) >> 16; \
	str[2] = (_in & 0x0000ff00) >> 8; \
	str[3] = (_in & 0x000000ff); }


/*
TODO:
 speed to 1000m
 priority to 4-7
 ingressratelimit 
 cltrowstatus
*/

#endif /*#ifndef __HEXICOMTC_H__*/

