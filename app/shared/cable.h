/*
 * cable.h
 *
 *  Created on: Jan 13, 2011
 *      Author: root
 */

#ifndef CABLE_H_
#define CABLE_H_

#define	MAX_CLT_CHANNEL				2

#define	MAX_SUPPORTED_CNU_MODELS	8

#define	MAX_CNU_PER_CLT				64
#define	CNU_MAX_ETH_PORTS			4

#define	HFID_NAME_SIZE				64
#define	TMPL_NAME_SIZE				64
#define	TMPL_DESC_SIZE				64
#define	ALIAS_NAME_SIZE				64
#define	USER_NAME_SIZE				64


#define EOC_VERSION_SIZE  64

#define	EOC_MAX_VLAN_NUMS		4096
#define	EOC_MAX_DEVS_NUMS		8
#define	EOC_MAX_TMPL_NUMS		128
#define	EOC_MAX_PORT_NUMS		5
#define	EOC_MAX_USER_NUMS		128
/*begin added by liaohongjun 2012/6/29*/
/*definition of EPN104*/
#define ONU_MAX_VLAN_NUMS    4096
#define ONU_MAX_WEB_USERS    4
#define ONU_MAX_AUX_USERS    3

typedef enum {
	ARRY_TYPE_VLAN = 0,
	ARRY_TYPE_USER_WEB,
	ARRY_TYPE_USER_AUX,
	ARRY_TYPE_MAX       
}CONFIG_ARRY_TYPE_E;
/*end added by liaohongjun 2012/6/29*/

#define	MAX_CLT_ETH_PORTS			5	// eth0, eth1, cab0

#define	CLT_ID_RANGE_CHECK(x) ((x > 0) && (x <= MAX_CLT_CHANNEL))
#define	CNU_ID_RANGE_CHECK(x) ((x > 0) && (x <= MAX_CNU_PER_CLT))


typedef enum {
	CNU_LINK_STATUS_UNKNOW = 0,
	CNU_LINK_STATUS_UP,
	CNU_LINK_STATUS_DOWN
}
cnu_link_status_t;


typedef enum {
	CNU_AUTH_UNKNOW = 0,
	CNU_AUTH_ANONACCEPT,
	CNU_AUTH_ACCEPT,
	CNU_AUTH_DENY
}
cnu_auth_t;


typedef enum{
	CNU_LINK_UNKNOW = 0,
	CNU_LINK_UP,
	CNU_LINK_DOWN,
	CNU_LINK_INVALID,
	CNU_LINK_DENY
}
cnu_link_t;


typedef enum {
	CNU_CONFIG_UNKNOW = 0,
	CNU_CONFIG_PIB,
	CNU_CONFIG_SWITCH,
	CNU_CONFIG_MIB,
	CNU_CONFIG_DONE
}
cnu_config_state_t;

typedef enum{
	CLT_STATE_OFFLINE = 0,
	CLT_STATE_PROBED,
	CLT_STATE_STANDBY,
}
cnu_clt_state_t;


typedef struct{
	uint8_t snid;
	uint16_t self_polling_period;
	uint16_t network_refresh_period; // in second
	uint8_t anonymous_control;
	//uint32_t anonymous_online_time; // deprecated
#define SNID_CHANGED 0x01
#define REFRESH_CHANGED 0x02
#define ANONYMOUS_CTRL_CHANGED 0x04	
	uint8_t update;// master use only, neglect in ipc client
}
cable_parameter_t;

typedef struct {
	float pre_fec;
	float bits_carrier;
	float snr_carrier;
	float tx_pbs_err;
	float rx_pbs_err;
	float avg_att;
	float avg_txpwr;
	uint32_t gil;
	uint64_t txpkt, txerr;
	uint64_t rxpkt, rxerr;
	uint32_t valid;
}
cable_link_stats_t;

#define LINK_STATS_VALID_STATS  0x01
#define LINK_STATS_VALID_TONEMAP  0x02



#endif /* CABLE_H_ */
