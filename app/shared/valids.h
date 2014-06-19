
#ifndef __VALIDS_H__
#define __VALIDS_H__

#include <basetype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define in_range(v, bottom, top) (((v <= top) && (v >= bottom)) ? 1 : 0)

#define valid_snid(v) in_range(v, 1, 8)
#define valid_clt_poll_period(v) in_range(v, 30, 180) /*in seconds*/
#define valid_network_refresh_period(v) in_range(v, 30, 180) /*in seconds*/
#define valid_vlan_id(v) in_range(v, 1, 4094)

#define valid_admin_len(v) in_range(v, 1, 12)
#define valid_admin(s) 1

#define MIN_ADMIN_TIMEOUT   1
#define MAX_ADMIN_TIMEOUT   60





int valid_time_zone(int tz);
int valid_unicast_mac(ethernet_addr_t *addr);

int valid_ip(struct in_addr ip);
int valid_subnet_mask(struct in_addr ip, struct in_addr mask);
int valid_gateway(struct in_addr ip, struct in_addr mask, struct in_addr gw);
int valid_hostname(char *v);


/*
TODO:
 Admin timeout in WEB
#define valid_admin(s) 


*/

#endif /*#ifndef __VALIDS_H__*/

