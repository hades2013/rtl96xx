/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_socket.h
*
* DESCRIPTION: 
*	
*
* Date Created: Apr 29, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_socket.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __SOCKET_H_
#define __SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <vos_types.h>


#define ANY_PORT  0
#define TCP_LISTEN_BACKLOG 5
#define MAX_UDP_PACKET_SIZE 1024


typedef struct UDP_PARAMS_s
{
    char *pRecPacket;
    char *pSendPacket;
    uint32 clientAddr;
    uint32 sendPacketLength;
    uint32 recvPacketLength;
    uint16 clientPort;
} UDP_PARAMS_t;


int vosSocketTcpCreate(void);
int vosSocketUdpCreate(void);
int vosSocketBind(int sock, uint32 addr, uint16 port);
int vosSocketUdpBind(int *pSock, uint16 port);
int vosSocketUdpFixedBind(int *pSock, uint16 port);
int vosSocketUdpAnyBind(int *pSock);
int vosSocketUdpSendTo(int sock, UDP_PARAMS_t *pParams);
int vosSocketUdpRecv(int sock, UDP_PARAMS_t *pParams);
int vosSocketTcpSvrOpen(int *pSock, uint16 port);
int vosSocketAccept(int sock, struct sockaddr_in *clientAddr);
int vosSocketPeerNameGet(int sock, struct sockaddr_in *clientAddr);
uint32 vosSocketPeerAddrGet(int clientSock);
int vosSocketClose(int sock);
int vosSocketConnect(int sock, char *pName, uint16 port );
int vosSocketConnectTo(int *pSock, char *pName, uint16 port );
int vosSocketCanRead(int sock, int timeout);
int vosSocketRead(int sock, char *pBuf, int bufSize);
int vosSocketCanWrite(int sock, int timeout);
int vosSocketWrite(int sock, char *pBuf, int bufLen);
 
#ifdef __cplusplus
}
#endif

#endif /* #ifndef __SOCKET_H_ */

