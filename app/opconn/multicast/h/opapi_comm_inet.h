/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* @file opapi_comm_inet.h
*
* DESCRIPTION:
*
* Date Created:
*
* Authors(optional):
*
* Reviewed by (optional):
*
* Edit History:
*
*************************************************************************/

#ifndef OPAPI_COMM_INET_H
#define OPAPI_COMM_INET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "vos_libc.h"

/* ipv4 */
struct ip_addr {
  unsigned int addr;
};

static inline unsigned int _chksum(void *dataptr, int len)
{
  unsigned int acc;

  for (acc = 0; len > 1; len -= 2) {
    acc += (*((unsigned short *) dataptr))++;
  }

  /* add up any odd byte */
  if (len == 1) {
    acc += htons((unsigned short) ((*(unsigned char *) dataptr) & 0xff) << 8);
    printf("inet: chksum: odd byte %d\n", *(unsigned char *) dataptr);
  }

  return acc;
}

/* internet check sum calculate */
static inline unsigned short clInetChksum(void *dataptr, unsigned short len)
{
  unsigned int acc;

  acc = _chksum(dataptr, len);

  while (acc >> 16) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  return ~(acc & 0xffff);
}

/* change the multicast ip address to multicast mac format,
   example: 239.0.0.1 -> 01:00:5e:XX:XX:XX */
static inline int clMcMacGetByMcIp(unsigned char *mcmac, unsigned int mcip)
{
  unsigned char * mac;

  if (mcmac == NULL) {
    return -1;
  }

  mac = mcmac;
  mac[0] = 0x01;
  mac[1] = 0x0;
  mac[2] = 0x5e;
  mac[3] = ((mcip & 0x7fffff) >> 16) & 0xff;
  mac[4] = ((mcip & 0x7fffff) >> 8) & 0xff;
  mac[5] = (mcip & 0x7fffff) & 0xff;
  return 0;
}

/* compare if mac1 is equal to mac2 */
static inline int clMacCmp(unsigned char *mac1, unsigned char *mac2)
{
  int i;

  if ((mac1 == NULL) || (mac2 == NULL)) {
    return -1;
  }

  for (i = 0; i < 6; i++) {
    if (mac1[i] != mac2[i]) {
      return FALSE;
    }
  }
  return TRUE;
}

/* get the string form mac address array */
static inline int clStringGetByMac(unsigned char *mac_string,
                                   unsigned char *mac)
{
  if (mac == NULL || mac_string == NULL) {
    return -1;
  }

  vosSprintf(mac_string,
             "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0],
             mac[1],
             mac[2],
             mac[3],
             mac[4],
             mac[5]);

  return 0;
}

static inline int clMacGetByString(unsigned char *mac,
                                   unsigned char *mac_string)
{
  unsigned int strLen = 0;
  unsigned int i = 0;
  int macAddr[6];
  int nVaildNum;

  if ((NULL == mac_string) || (NULL == mac)) {
    return -1;
  }

  strLen = vosStrLen(mac_string);
  if (strLen != 17) {
    return -1;
  }

  /*convert string to value*/
  nVaildNum = sscanf(mac_string,
                     "%02x:%02x:%02x:%02x:%02x:%02x",
                     &macAddr[0],
                     &macAddr[1],
                     &macAddr[2],
                     &macAddr[3],
                     &macAddr[4],
                     &macAddr[5]);

  if (nVaildNum != 6) {
    return -1;
  }


  for (i = 0; i < 6; i ++) {
    mac[i] = (char) macAddr[i];
  }

  return 0;
}

static inline int clIpAddrCmp(struct ip_addr *addr1, struct ip_addr *addr2)
{
  return(addr1->addr == addr2->addr);
}

static inline int clStringGetByIp(unsigned char *ip_string, unsigned int ip)
{
  struct in_addr in;

  memcpy(&in, &ip, 4);

  strcpy(ip_string, inet_ntoa(in));

  return 0;
}

static inline int ethPacketPrint(void *pkt, int len)
{
  int i;

  if (pkt == NULL || len < 0) {
    return -1;
  }

  printf("%-32s:\n", "------packet print------");
  for (i = 0; i < (len + 3) / 4; i++) {
    printf("%08X ", *((unsigned int *) pkt + i));
    if (i != 0 && (i + 1) % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");

  return 0;
}

static inline int ipAddrPrint(struct ip_addr *ipaddr)
{
  if (ipaddr == NULL) {
    return -1;
  }

  printf("%d.%d.%d.%d\n",
         (unsigned char) (ntohl((ipaddr)->addr) >> 24) & 0xff,
         (unsigned char) (ntohl((ipaddr)->addr) >> 16) & 0xff,
         (unsigned char) (ntohl((ipaddr)->addr) >> 8) & 0xff,
         (unsigned char) ntohl((ipaddr)->addr) & 0xff);

  return 0;
}

static inline int ethMacPrint(unsigned char *mac)
{
  char macstring[20];

  if (mac == NULL) {
    return -1;
  }

  memset(macstring, 0, 20);

  clStringGetByMac(macstring, mac);

  printf("MAC addr=%s\n", macstring);

  return 0;
}

#if 0
#define CRC_BYTE_BIT_COUNT           0x80
#define CRC16_POLYNOMIAL_AMERICAN    0x8005
#define CRC16_POLYNOMIAL_CCITT       0x1021
#define CRC16_RESULT_LENGTH          0x0000FFFF
#define CRC16_MOST_SIGNIFICIANT_BIT  0x00008000
#define CRC32_POLYNOMIAL             0x04C11DB7
#define CRC32_RESULT_LENGTH          0xFFFFFFFF
#define CRC32_MOST_SIGNIFICIANT_BIT  0x80000000


static inline int CRC16BitShift(UINT8 *pucValue,
                                UINT32 ulValueLegnth,
                                UINT16 *pusCrcValue)
{
  UINT8 ucCharBit = 0;
  UINT16 usCrc = 0xFFFF;

  if (NULL == pucValue) {
    return -1;
  }

  while (0 != ulValueLegnth--) {
    for (ucCharBit = CRC_BYTE_BIT_COUNT; 0 != ucCharBit; ucCharBit /= 2) {
      if (0 != (usCrc & CRC16_MOST_SIGNIFICIANT_BIT)) {
        usCrc *= 2;
        usCrc ^= CRC16_POLYNOMIAL_CCITT;
      }
      else {
        usCrc *= 2;
      }

      if (((*pucValue) & ucCharBit) != 0) {
        usCrc ^= CRC16_POLYNOMIAL_CCITT;
      }
    }

    pucValue++;
  }

  *pusCrcValue = usCrc;

  return 0;
}

static inline int TableAddrCal16(UINT8 *pucValue,
                                 UINT32 ulValueLen,
                                 UINT16 usAddrMask,
                                 UINT16 *pusAddress)
{
  UINT16 usCrcValue = 0;
  int iStatus;

  if (NULL == pucValue) {
    return -1;
  }

  iStatus = CRC16BitShift(pucValue, ulValueLen, &usCrcValue);
  if (0 != iStatus) {
    return iStatus ;
  }

  *pusAddress = usAddrMask & usCrcValue;

  return 0;
}
#endif
#endif /* OPAPI_COMM_INET_H */


