
#ifndef _LOOPD_H_
#define _LOOPD_H_

#ifdef  __cplusplus
extern "C"{
#endif


#include "lw_type.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"


#define RLPP_DBG_EN    (0)
#if RLPP_DBG_EN
  #define RLPP_DBG(fmt, ...) do { fprintf(stderr, "[%s,%d]: " fmt, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#else /* else of DBG_RLPP_ENABLE */
  #define RLPP_DBG(fmt, ...)
#endif /* end of else of RLPP_DBG_EN */

#ifndef FCS_LEN
#define FCS_LEN          		(4)
#endif

#ifndef RTK_PROTO_ERR_OK
#define RTK_PROTO_ERR_OK      (0)
#endif

#ifndef RTK_PROTO_ERR_FAILED
#define RTK_PROTO_ERR_FAILED  (-1)
#endif

#ifndef ETH_TYPE_LOOPD
#define ETH_TYPE_LOOPD    0x9001
#endif

#define CPU_TAG_LEN             8
#define OTAG_LEN                4
#define ITAG_LEN                4
#define MEMCPY_RESERVE          8  // because Rx & Tx share the same buffer, to avoid copy same memory, 1 byte is reserved.
#define HEADER_EXTENSION_SIZE   (ITAG_LEN + OTAG_LEN + CPU_TAG_LEN + MEMCPY_RESERVE)
#define MAX_PKT_PAYLOAD_LEN     1514

#define MAX_PKT_LEN             (MAX_PKT_PAYLOAD_LEN + FCS_LEN + OTAG_LEN + ITAG_LEN + CPU_TAG_LEN)
#define MIN_PKT_LEN             (60 + CPU_TAG_LEN)

//#define PKT_BUFFER_SIZE         (HEADER_EXTENSION_SIZE + HEADER_EXTENSION_SIZE + MAX_PKT_PAYLOAD_LEN + FCS_LEN)
#define PKT_BUFFER_SIZE 1518+4
#define RTK_PROTO_ID_RLPP 0x25

#define RLPP_SWPRI_LEN   (3)
#define ETH_HWADDR_LEN   (6)
#define RLPP_PKT_LEN     (64)

/* Period to send hello packet */
#define RLPP_HELLO_TIME  (1)
/* Timeout time for port vector */
#define RLPP_PV_TIME     (5)
/* Timeout time for self loop vector */
#define RLPP_SL_TIME     (10)

#define RLPP_HOPCOUNT_MAX    (32)

typedef enum tx_pkt_mode_e {
    TX_DPORT_DEFAULT,        /* do not assign tx portmask */
	TX_DPORT_ASSIGN_PMSK     /* follow tx portmask */
} tx_pkt_mode_t;

typedef enum layer2_fmt_e {
    L2FMT_NONE,
    L2FMT_ETH,
    L2FMT_ETHARP,
    L2FMT_LEN
} layer2_fmt_t;

typedef enum layer3_fmt_e {
    L3FMT_NONE,
    L3FMT_IP,
    L3FMT_IPV6,
    L3FMT_RSTP  /* LLC+RSTP is in L3 in this system */
} layer3_fmt_t;

typedef enum rlpp_port_state_e
{
    RLPP_DISABLE_STATE = 0,
    RLPP_BLOCKING_STATE,
    RLPP_LISTENING_STATE,
    RLPP_FORWARDING_STATE
} rlpp_port_state_t;

#pragma pack (1)

typedef struct eth_addr_s {
    UINT8 addr[ETH_HWADDR_LEN];
} eth_addr_t;

typedef struct eth_hdr_s {
    struct eth_addr_s dst_mac;
    struct eth_addr_s src_mac;
    UINT16 ether_type;
} eth_hdr_t;


typedef struct rlpp_switch_id_s
{
    UINT8      swpri[RLPP_SWPRI_LEN];
    UINT8      swmac[MAC_ADDR_LEN];
} rlpp_switch_id_t;

typedef struct rlpp_port_vector_s
{
    rlpp_switch_id_t rid;
	UINT16           hopcount;
	rlpp_switch_id_t sid;
	UINT8 swppri;
	UINT8 swport;    	
} rlpp_port_vector_t;

typedef struct pktBuf_s {
    //UINT16 size;
    //UINT8 *frame_info;
    //UINT8 *pkt_cputag;
	//UINT8 *pkt_vlantag;
    UINT8 *pkt_head;
    UINT8 *l2;
    UINT8 *l3;
    //UINT8 *l4;
    UINT8 l2fmt;          /* Layer 2 Format */
    UINT8 l3fmt;          /* Layer 3 Format */
    //UINT8 l4fmt;          /* Layer 4 Format */

    UINT8  spa;             /* Rx port */

    //UINT8  tx_chksum_mode;  /* Check sum config for NIC tx function */
    UINT8  tx_dport_mode;   /* Tx packet mode. please reference to tx_pkt_mode_t */
    logic_pmask_t tx_dst_pmsk;
    //UINT8  inner_tag_mode;  /* InnerTagMode of TX pkt */
    //UINT8  outer_tag_mode;  /* InnerTagMode of TX pkt */    
    //UINT32 inner_tag;       /* Inner tag of rx pkt / Inner tag of tx pkt */
    //UINT32 outer_tag;       /* Outer tag of rx pkt / Outer tag of tx pkt */    

    //UINT8  tx_trunk_support; /* Send pacekt to represent port in trunk or all member port */
} pktBuf_t;

typedef struct rlpp_port_var_s{
    rlpp_port_vector_t port_vector;
    UINT8 slen;
    UINT8 selfloop;
    UINT8 ppri;
    UINT8 hc;
    UINT8 pv_ag_enable;
    UINT8 pv_ag;
    UINT8 pv_sl_enable;
    UINT8 sl_ag;
    UINT8 port_state;
} rlpp_port_var_t;

typedef struct rlpp_payload_s
{
    UINT8              proto_id;
    rlpp_port_vector_t pkt_port_vector;
} rlpp_payload_t;

#pragma pack ()



INT32 rlpp_init(VOID);
INT32 rlpp_setState(UINT8 enable);
INT32 rlpp_getPortState(port_num_t logicPort, rlpp_port_state_t *state);
INT32 rlpp_up(pktBuf_t *pBuf);
INT32 rlpp_pkt_process(pktBuf_t *pktBuf, UINT16 len);
void RlppHelloTimerHandle(int signal);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _LOOPD_H_ */

