#ifndef _QOS_H
#define _QOS_H

#define TRUE  1
#define FALSE 0

#include "defs.h"

typedef int    QOS_STATUS;

typedef enum STREAM_TYPE_S
{
	UP_STREAM_   = 0,
	DOWN_STREAM_ = 1,
	BOTH_STREAM_ = 2
}STREAM_TYPE_T;

typedef enum {
    QOS_FALSE  = 0,
    QOS_TRUE   = 1
} QOS_BOOL;

typedef enum {
    CLASS_MOV  = 0,
    CLASS_ADD  = 1
} CLASS_ACT;

typedef enum{
    PASS = 0,
    DROP,
    COPYTOCPU,
    SENDTOCPU,
    INSERTVID,
    CHANGEVID
}TRANF_ACT;
typedef enum
{
	CLASS_ETHERTYPE =0,
	CLASS_VID,
	CLASS_SRCMAC,	
	CLASS_DSTMAC,
	CLASS_SRCIP,	
	CLASS_DSTIP,	
	CLASS_IPDSCP,
	CLASS_IPPRO,
	CLASS_ETHERPRI,
	CLASS_SRCPORT,
	CLASS_DSTPORT
}ONU_CLASS_TYPE;
typedef struct onu_filter_s
{
     int   type;
     int   transfAct;
     char  value[18];
	 int   portId;
	 char  name[20];
}onu_filter_t;
#define QOS_OK 0
#define ODM_QUEUE_LIMIT_NUM			0x3FFF

#define QOS_CLASS_SESSION       "/cfg/qos.conf"
#define QOS_OAM_NAME            "oamcls_%d/%d"
#define QOS_WEB_NAME            "web%d"

#define CLASSKEY_DESTMAC        "DestMac"
#define CLASSKEY_SRCMAC         "SrcMac"
#define CLASSKEY_TYPE           "Type"
#define CLASSKEY_DESTIP         "DestIP"
#define CLASSKEY_SRCIP          "SrcIP"
#define CLASSKEY_FVID           "FVid"
#define CLASSKEY_SVID           "SVid"
#define CLASSKEY_FCOS           "FCos"
#define CLASSKEY_SCOS           "SCos"
#define CLASSKEY_PROTOCAL       "IpType"
#define CLASSKEY_TOS            "IpTos"
#define CLASSKEY_V6PRE			"V6Pre"
#define CLASSKEY_DSCP           "IpDscp"
#define CLASSKEY_DESTL4PORT     "DestPort"
#define CLASSKEY_SRCL4PORT      "SrcPort"
#define CLASSKEY_COMMIX         "Commix"
#define CLASSKEY_USERDEF        "UserDef"
#define CLASSKEY_DEFMASK        "DefMask"
#define CLASSKEY_DEFVALUE       "DefValue"
#define CLASSKEY_DEFOFFSET      "DefOffset"
#define CLASSKEY_QUEUE          "queue"
#define CLASSKEY_PRIORITY       "priority"
/*add by ltang,for the bug3222  ---start*/
#define CLASSKEY_CHANGEICOS     "changeicos"
#define CLASSKEY_CHANGECOS      "changecos"
/*add by ltang,for the bug3222  ---end*/
#define CLASSKEY_TRUSTMODE      "trustmode"
#define CLASSKEY_METER          "Meter"
#define CLASSKEY_MCIR           "MCIR"
#define CLASSKEY_MCBS           "MCBS"

#define CLASSKEY_Precedence     "precedence"
#define CLASSKEY_TRANFACT       "trafact"
#define CLASSKEY_INSTALL        "install"
#define CLASSKEY_PORTS          "portnumber"
#define CLASSKEY_CHANGEVID      "remarkvid"
#define CLASSKEY_INSERTVID      "insertvid"
#define CLASSKEY_DIRECTION      "streamtype"

#define QOS_QosSched_SESSION       "/cfg/qosmap.conf"

#define PORT_SCHEDMODE "SchedMode"
#define PORT_CosRegen "Cos-%d"
#define QOS_Cos2QMap "QueueId"

#define CLASSQUEUIESECTION      "queuelimit"
#define CLASSQUEUELIMIT         "queuelim%d"
#define QOS_QOSBLOCK_SESSION     "/cfg/qosblock.conf"
#endif
