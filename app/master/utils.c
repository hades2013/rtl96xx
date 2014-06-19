/*
 * utils.c
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#include "master.h"
#include <str_utils.h>



int set_if_mac(char *ifname, ethernet_addr_t *mac)
{
/*
	char cmd_str[128];
	sprintf(cmd_str, "ifconfig %s down", ifname);	
	system(cmd_str);
	sprintf(cmd_str, "ifconfig %s hw ether %s", ifname, mac2str(mac));
	system(cmd_str);
	sprintf(cmd_str, "ifconfig %s up", ifname);	
	system(cmd_str);
*/
	eval("ifconfig", ifname, "down");	
	eval("ifconfig", ifname, "hw", "ether", mac2str(mac));
	eval("ifconfig", ifname, "up");
	
	return 0;
}

int get_if_mac(char *ifname, ethernet_addr_t *mac)
{
	int sock, ret;
	struct ifreq ifr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	DBG_ASSERT(sock >= 0, "create socket: %s", strerror(errno));
	if(sock < 0) return -1;

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
	ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
	DBG_ASSERT(ret >= 0, "ioctl: %s", strerror(errno));
	if(ret < 0) return -1;

	memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(ethernet_addr_t));
	close(sock);
	return 0;
}

int create_raw_socket(char *ifname, int *ifindex)
{
	int sock, ret;
	struct ifreq ifr;
	struct packet_mreq mr;
	struct timeval tv;

	sock = socket(PF_PACKET, SOCK_RAW, htons(0x88e1));
	DBG_ASSERT(sock >= 0, "create socket: %s", strerror(errno));
	if(sock < 0) return sock;

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
	ret = ioctl(sock, SIOCGIFINDEX, &ifr);
	DBG_ASSERT(ret >= 0, "ioctl: %s", strerror(errno));
	if(ret < 0) {
		close(sock);
		return ret;
	}
	memset(&mr, 0, sizeof(struct packet_mreq));
	mr.mr_ifindex = ifr.ifr_ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	*ifindex = ifr.ifr_ifindex;

	tv.tv_sec = 0;
	tv.tv_usec = 10 * 1000;	// Timeout 10ms
	ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	DBG_ASSERT(ret >= 0, "setsockopt: %s", strerror(errno));
	if(ret < 0) {
		close(sock);
		return ret;
	}

	return sock;
}

uint32_t checksum_32 (register const void * memory, register size_t extent, register uint32_t checksum)

{
	while (extent >= sizeof (checksum))
	{
		checksum ^= *(typeof (checksum) *)(memory);
		memory += sizeof (checksum);
		extent -= sizeof (checksum);
	}
	return (~checksum);
}
