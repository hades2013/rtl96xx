#ifndef __NSCRTV_H__
#define  __NSCRTV_H__


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


#include "../hexicom/hexicom.h"
#include "../hexicom/hexicomTC.h"

#undef DBGID 

#define DBGID "nscrtv"



typedef enum{
	NSCRTV_MOCA = 1,
	NSCRTV_HPBPL,
	NSCRTV_HPAV,
	NSCRTV_PNA,
	NSCRTV_WIFI,
	NSCRTV_OTHERS
}
nscrtv_devinfo_t;


typedef enum{
	NSCRTV_DETECTION_DISABLED = 1,
	NSCRTV_DETECTION_ENABLED,
	NSCRTV_DETECTION_ENABLED_RESET
}
nscrtv_detection_t;


typedef enum{
	NSCRTV_RESET_REASON_OTHER = 1,
	NSCRTV_RESET_REASON_POWERUP,
	NSCRTV_RESET_REASON_COMMAND,
	NSCRTV_RESET_REASON_WATCHDOG,
	NSCRTV_RESET_REASON_CRAFT 	
}
nscrtv_reset_reason_t;


typedef enum{
	NSCRTV_SAVE_OK = 1,
	NSCRTV_SAVE_DOING,
	NSCRTV_SAVE_FAILED
}
nscrtv_save_result_t;


typedef enum{
	NSCRTV_COMMUNITY_PUBLIC = 1,
	NSCRTV_COMMUNITY_PRIVATE
}
nscrtv_community_permission_t;


typedef enum{
	NSCRTV_SW_DOWNLOAD = 1,
	NSCRTV_SW_UPGRADE
}
nscrtv_sw_upgrade_proceed_t;


typedef enum{
	NSCRTV_SW_UPGRADE_NONE = 0,
	NSCRTV_SW_UPGRADE_DOWNLOADING,
	NSCRTV_SW_UPGRADE_DOWNLOAD_FAILED,
	NSCRTV_SW_UPGRADE_DOWNLOAD_OK,
	NSCRTV_SW_UPGRADING,
	NSCRTV_SW_UPGRADE_FAILED,
	NSCRTV_SW_UPGRADE_OK
}
nscrtv_sw_upgrade_result_t;

typedef enum{
	NSCRTV_AUTHORIZED = 1,
	NSCRTV_NO_AUTHORIZED  = 0	
}
nscrtv_cnu_authorization_t;




#endif /*#ifndef __NSCRTV_H__*/


