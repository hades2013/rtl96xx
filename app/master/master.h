/*
 * master.h
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/stddef.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>

#include <shutils.h>

//#include <nvram.h>
//#include <cfg/nvram_utils.h>
//#include <cable.h>
//#include <switch.h>
//#include <cltmon_log.h>


#include <basetype.h>
#include <debug.h>

#include <ipc_protocol.h>
//#include <config.h>
/*begin modified by liaohongjun 2012/7/6*/
#include <pdt_config.h>
/*end modified by liaohongjun 2012/7/6*/
#include <hex_utils.h>

/*Begin modified by huangmingjian 2012/09/21 for EPN104QID0046*/  
//#define RAW_PACKET_DEBUG 	1
//#define	IPC_PACKET_DEBUG	2
/*End modified by huangmingjian 2012/09/21 for EPN104QID0046 */

#ifndef __packed
#define	__packed	__attribute__((__packed__))
#endif

typedef enum {
	SELECT_TMO = 0,
	EVENT_PACKET = 1,
}
fsm_event_t;

extern ethernet_addr_t	master_ifmac;
//extern nvram_handle_t *nvram_handle;

#endif /* MASTER_H_ */
