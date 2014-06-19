/*
 * ===============================================================================
 * Header Name: eopl_host.h
 *
 * General Description:
 *    Definition for Opconn host interface driver.
 *
 * ===============================================================================
 * $Copyright: (c) 2007 OPulan Corp.
 * All Rights Reserved.$
 * ===============================================================================
 *
 * Revision History:
 *
 * Author                Date              Description of Changes
 * ----------------   ------------  ----------------------------------------------
 * rzhou              12/10/2007    initial
 * ----------------   ------------  ----------------------------------------------
 *
 * ===============================================================================
 */

#ifndef OPL_HOST_INCLUDE
#define	OPL_HOST_INCLUDE

#pragma pack (1)

#define DAL_NUM_OF_PORTS    1 
#define MAX_NUM_OF_VLAN 4095

typedef enum OPCONN_VLAN_MODE_s
{
	VLAN_TRANSPARENT,
	VLAN_TAG,
	VLAN_TRANSLATION,
	VLAN_AGGREGATION,
	VLAN_TRUNK,
	VLAN_END
}OPCONN_VLAN_MODE_e;


typedef struct {
    UINT32 dmaTx;
    UINT32 dmaRx;
    UINT32 dmaTxFail;	

    UINT32 oamTx;	
    UINT32 oamRx;	
    UINT32 oamTxFail;			
} hostCntS; 
extern hostCntS hostCnt ;

#define ETH_TYPE_SLOW_PROTOCOL		0x8809	/* Ethernet Type Slow Protocol */


/* Host inbound header definition */
typedef struct host_inbound_hdr_s
{
	#ifdef EOPL_LITTLE_BITFIELD
   UINT32 iport:4,  /* Inbound  port index */
     reserved:28;  /* Reserved */
  #else
UINT32 reserved:28,  /* Reserved */
       	iport:4;  /* Inbound  port index */
  #endif

}host_inbound_hdr_t;

/* Host outbound header definition */
typedef struct host_outbound_hdr_s
{
	#ifdef EOPL_LITTLE_BITFIELD
	UINT8 eport:4,  /* Inbound  port index */
         icos:3,  /* ICOS */
     reserved:1;  /* Reserved */

  #else
	UINT8 reserved:1,  /* Reserved */
         icos:3,  /* ICOS */
       	eport:4;  /* Inbound  port index */  /*changed by wxian for send port*/
  #endif

}host_outbound_hdr_t;

typedef struct GT_ETHER_FROM_CPU_DSA_TAG_RSV_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_ETHER_FROM_CPU_DSA_TAG_RSV_t;

typedef struct GT_ETHER_TO_CPU_DSA_TAG_RSV_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_ETHER_TO_CPU_DSA_TAG_RSV_t;

typedef struct GT_ETHER_FROM_CPU_DSA_TAG_IGMP_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_ETHER_FROM_CPU_DSA_TAG_IGMP_t;

typedef struct GT_ETHER_TO_CPU_DSA_TAG_IGMP_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short tpid;
    unsigned short vlanInfo;

    unsigned short  etherType;
    unsigned short  reserved;

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_ETHER_TO_CPU_DSA_TAG_IGMP_t;

typedef struct GT_FROM_CPU_DSA_TAG_RSV_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_FROM_CPU_DSA_TAG_RSV_t;

typedef struct GT_TO_CPU_DSA_TAG_RSV_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_TO_CPU_DSA_TAG_RSV_t;

typedef struct GT_FROM_CPU_DSA_TAG_IGMP_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned int    dir:2;           /*should be 0b01*/
    unsigned int    trg_tagged:1;
    unsigned int    trg_dev:5;
    unsigned int    trg_port:5;
    unsigned int    pad1:2;          /*shouled be 0b00*/
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    pad2:1;          /*should be 0b0*/
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_FROM_CPU_DSA_TAG_IGMP_t;

typedef struct GT_TO_CPU_DSA_TAG_IGMP_s{
    unsigned char   da[6];
    unsigned char   sa[6];

    unsigned short tpid;
    unsigned short vlanInfo;

    unsigned int    dir:2;             /*should be 0b00*/
    unsigned int    src_tagged:1;
    unsigned int    src_dev:5;
    unsigned int    src_port:5;
    unsigned int    codeH:2;
    unsigned int    cfi:1;
    unsigned int    pri:3;
    unsigned int    codeL:1;
    unsigned int    vid:12;

    unsigned char   payLoad[0];
}GT_TO_CPU_DSA_TAG_IGMP_t;


typedef struct ATHEROS_HEADER_FRAME_s
{
#if defined(ONU_4PORT_AR8327)
    #ifdef EOPL_LITTLE_BITFIELD
   UINT32 iport:2,  /* Inbound  port index */
     reserved:30;  /* Reserved */
  #else
UINT32 reserved:30,  /* Reserved */
       	iport:2;  /* Inbound  port index */
  #endif

#endif

    unsigned char   da[6];
    unsigned char   sa[6];

#if defined(ONU_4PORT_AR8306)
    unsigned short	ver:2;             /* should be 0b10 */
    unsigned short	priority:2;
    unsigned short	type:4;
    unsigned short	broadcast:1;
    unsigned short	fromCpu:1;
    unsigned short	ver2:2;				/* should be 0b10 */
    unsigned short	portNum:4;
#elif defined(ONU_4PORT_AR8327)
    unsigned short   head_type;
    unsigned short	ver:2;             /* should be 0b10 */
    unsigned short	priority:3;
    unsigned short	type:5;
    unsigned short   resv:2;
    unsigned short	with_tag:1;
    unsigned short	portNum:3;
#else
    unsigned short	ver:2;             /* should be 0b10 */
    unsigned short	priority:2;
    unsigned short	type:4;
    unsigned short	fromCpu:1;
    unsigned short	portNum:7;
#endif
    unsigned char   payLoad[0];
}ATHEROS_HEADER_FRAME_t;

typedef struct ATHEROS_HEADER_OUTBOUND_FRAME_s
{
#if defined(ONU_4PORT_AR8327)
    #ifdef EOPL_LITTLE_BITFIELD
	UINT8 eport:4,  /* Inbound  port index */
         icos:3,  /* ICOS */
     reserved:1;  /* Reserved */

  #else
	UINT8 reserved:1,  /* Reserved */
         icos:3,  /* ICOS */
       	eport:4;  /* Inbound  port index */
  #endif

#endif

    unsigned char   da[6];
    unsigned char   sa[6];

#if defined(ONU_4PORT_AR8306)
    unsigned short	ver:2;             /* should be 0b10 */
    unsigned short	priority:2;
    unsigned short	type:4;
    unsigned short	broadcast:1;
    unsigned short	fromCpu:1;
    unsigned short	ver2:2;				/* should be 0b10 */
    unsigned short	portNum:4;
#elif defined(ONU_4PORT_AR8327)
    unsigned short   head_type;
#if 0
   unsigned short	ver:2;             /* should be 0b10 */
    unsigned short	priority:3;
    unsigned short	type:5;
    unsigned short   resv:2;
    unsigned short	with_tag:1;
    unsigned short	portNum:3;
#endif	
	unsigned short     ver:2;             /* should be 0b10 */
	unsigned short     priority:3;  
	unsigned short     type:3;
	unsigned short     fromCpu:1;
	unsigned short     portNum:7;
#else
    unsigned short	ver:2;             /* should be 0b10 */
    unsigned short	priority:2;
    unsigned short	type:4;
    unsigned short	fromCpu:1;
    unsigned short	portNum:7;
#endif
    unsigned char   payLoad[0];
}ATHEROS_HEADER_OUTBOUND_FRAME_t;

#pragma pack ()

/* define DMA1 for Atheros */
#define NR_START 4000 //_MIPS_SIM_ABI32:4000,316; _MIPS_SIM_ABI64:5000,275; _MIPS_SIM_NABI32:6000,279
#define NR_TEST_WAITINTERRUPT (NR_START + 316)
#define NR_TEST_GETDATA (NR_START + 317)
#define NR_TEST_SETDATA (NR_START + 318)


#if 1
extern UINT8 hostDumpDataEn;

#define opl_host_printf(x)	if(hostDumpDataEn) printf x

#define opl_dump_data(p,len,width) \
{ \
    int index;unsigned char *x = p; \
    for(index = 0; index < len; index++) \
    { \
        if(!hostDumpDataEn) break; \
        if((index != 0) && (index%width == 0)) \
        { \
            opl_host_printf(("\n%02x ",x[index])); \
        } \
        else \
        { \
            opl_host_printf(("%02x ",x[index])); \
        } \
    } \
    opl_host_printf(("\n")); \
    opl_host_printf(("%s,%d\n",__FUNCTION__,__LINE__)); \
}

#else
#define opl_host_printf(x)

#define opl_dump_data(p,len,width)
#endif

enum {
	OAM_PKT_TYPE,
	STP_PKT_TYPE,
	IGMP_PKT_TYPE,
	END_TYPE
};

INT16 eopl_send_to_uplink(UINT16 portNum,void *pkt,UINT16 len);


INT16 eopl_send_to_down_link(UINT16 portNum,UINT16 withTag,UINT8 pktType,void *pkt,UINT16 len);


INT32 eopl_rx_pkt_process(UINT32 pktType,UINT8 portNum,UINT16 vlan,UINT8 withTag,UINT8 *pOrgPkt,UINT8 *payLoad,UINT16 len);


INT32 eopl_rx_pkt_dispatch(void *pBuff,UINT16 len);

/*
* eopl_pkt_process
*
* DESCRIPTION:
*   This function processes the received packets.
*
* INPUT: pkt, len
*   pkt    - packet pointer
*   len    - length of packet
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
void eopl_pkt_process(void * pkt, UINT16 len);

/*
* eopl_host_send
*
* DESCRIPTION:
*   This function send packet from host interface.
*
* INPUT: pkt, len
*   pkt         - packet pointer
*   len         - length of packet
*
* OUTPUT: None
*
* RETURNS:
*   -1: failed; >0: succeed.
*
* SEE ALSO:
*/
extern INT16 eopl_host_send(void *pkt, UINT16 len);

/*
* eopl_host_recv
*
* DESCRIPTION:
*   This function receives packets from host interface.
*
* INPUT: None
*
* OUTPUT: None
*
* RETURNS:
*   Always return 0.
*
* SEE ALSO:
*/
extern INT16 eopl_host_recv(void);

extern INT16 eopl_host_socket_recv(int socket_fd);
/*
* eopl_host_dump_enable
*
* DESCRIPTION:
*   This function enable packet dump switch.
*
* INPUT: enable
*    enable    - TRUE: enable; FALSE:disable.
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
extern void eopl_host_dump_enable(INT32 enable);

#ifdef OPCONN_HOST_DEBUG

/*
* eopl_host_send_test
*
* DESCRIPTION:
*   This function test to send packets.
*
* INPUT:
*			port     - port ID;
*			icos     - ICOS;
*			type     - 0: fixed with input data and dataLen;
*							1: fixed with 1 byte data;
*							2:random data.
*							3: increased with 0x0;
*							4:decreased with 0xFF;
*
*			pktlen   - real packet length to send;
*         count    - count to send.
*			data     - data pointer;
*         datalen - data ength;
*
* OUTPUT: None
*
* RETURNS: None
*
* SEE ALSO:
*/
extern void eopl_host_send_test (
	UINT32 port ,
	UINT8 icos,
	UINT8 type,
	UINT16 pktLen,
	UINT32 count,
	UINT8 *data,
	UINT16 dataLen);

#endif

#if defined(ONU_4PORT_AR8327)
UINT8 eopl_host_atheros_headflag_get();
UINT32 eopl_host_atheros_portnum_get();
#endif
#endif /* OPL_HOST_INCLUDE */
