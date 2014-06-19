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
* FILENAME:  vos_socket.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_socket.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos_socket.h>
#include <vos_libc.h>
#include <vos_alloc.h>


/*******************************************************************************
*
* vosSocketTcpCreate:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketTcpCreate()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

/*******************************************************************************
*
* vosSocketUdpCreate:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketUdpCreate()
{
    return socket(AF_INET, SOCK_DGRAM, 0);
}

/*******************************************************************************
*
* vosSocketBind:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketBind(int sock, uint32 addr, uint16 port)
{
    struct sockaddr_in  LocalAddr;

    vosMemSet(&LocalAddr, 0x00, sizeof(LocalAddr));

    /* get system to allocate a port number by binding a host address */
    LocalAddr.sin_family        = AF_INET;
    LocalAddr.sin_addr.s_addr   = htonl(addr);
    LocalAddr.sin_port          = htons(port);

    /* bind socket to local address */
    return bind(sock, (struct sockaddr *) &LocalAddr, sizeof(LocalAddr));
}

/*******************************************************************************
*
* vosSocketUdpBind:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketUdpBind(int *pSock, uint16 port)
{
    /* Create a udp socket */
    *pSock = vosSocketUdpCreate();
    if (-1 == *pSock)
        return -1;

    return vosSocketBind(*pSock, INADDR_ANY, port);
}

/*******************************************************************************
*
* vosSocketUdpFixedBind:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketUdpFixedBind(int *pSock, uint16 port)
{
    return vosSocketUdpBind(pSock, port);
}

/*******************************************************************************
*
* vosSocketUdpAnyBind:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketUdpAnyBind(int *pSock)
{
    return vosSocketUdpBind(pSock, ANY_PORT);
}


/*******************************************************************************
*
* vosSocketUdpSendTo:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketUdpSendTo(int sock, UDP_PARAMS_t *pParams)
{
    struct sockaddr_in *pAddrTo;
    if ( NULL == (pAddrTo = (struct sockaddr_in *) vosAlloc(sizeof(struct sockaddr_in))) )
        return 0;
    pAddrTo->sin_family = AF_INET;
    pAddrTo->sin_addr.s_addr = pParams->clientAddr;
    pAddrTo->sin_port = htons(pParams->clientPort);
    if (-1 == sendto(sock, (char *) pParams->pSendPacket,
                        pParams->sendPacketLength, 0,
                        (struct sockaddr *)pAddrTo,
                        sizeof(struct sockaddr)) )
    {
        vosFree(pAddrTo);
        return -1;
    }
    vosFree(pAddrTo);
    return 0;
}

/*******************************************************************************
*
* vosSocketUdpRecv:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketUdpRecv(int sock, UDP_PARAMS_t *pParams)
{
    struct sockaddr_in *pAddr;
    int iAddrLen;
    int RecvResult;
    iAddrLen = sizeof(struct sockaddr_in);
    if ( NULL == (pAddr = (struct sockaddr_in *) vosAlloc(iAddrLen)) )
        return 0;
    RecvResult = recvfrom(sock, (char *) pParams->pRecPacket, MAX_UDP_PACKET_SIZE,
                    0, (struct sockaddr *)pAddr, &iAddrLen );
    if (-1 == RecvResult)
    {
        vosFree(pAddr);
        return -1;
    }
    pParams->clientAddr = pAddr->sin_addr.s_addr;
    pParams->clientPort = ntohs(pAddr->sin_port);
    pParams->recvPacketLength = (unsigned int)RecvResult;
    vosFree(pAddr);

    return 0;

}

/*******************************************************************************
*
* vosSocketTcpSvrOpen:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketTcpSvrOpen(int *pSock, uint16 port)
{
    int  rc, on = 1;

    *pSock = vosSocketTcpCreate();
    if (-1 == *pSock)
        return *pSock;
    
     rc = setsockopt(*pSock, SOL_SOCKET,  SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0)
    {
       close(*pSock);
       return rc;
    }

    rc = vosSocketBind(*pSock, INADDR_ANY, port);
    if (rc < 0)
    {
        close(*pSock);
        return rc;
    }

    rc = listen(*pSock, TCP_LISTEN_BACKLOG);
    if (rc < 0)
    {
        close(*pSock);
        return rc;
    }

    return 0;
}

/*******************************************************************************
*
* vosSocketAccept:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketAccept(int sock, struct sockaddr_in *clientAddr)
{
    int iAddrLen;

    iAddrLen = sizeof(struct sockaddr_in);
    return accept(sock, (struct sockaddr *)clientAddr, &iAddrLen);
}

/*******************************************************************************
*
* vosSocketPeerNameGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketPeerNameGet(int sock, struct sockaddr_in *clientAddr)
{
    int iAddrLen;
    
    iAddrLen = sizeof(struct sockaddr_in);
    return getpeername(sock, (struct sockaddr *)clientAddr, &iAddrLen);
}


/*******************************************************************************
*
* vosSocketPeerAddrGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
uint32 vosSocketPeerAddrGet(int ClientSock)
{
    struct sockaddr_in  ClientAddr;
    int                 iAddrLen;
    unsigned int              clientIpAddr = 0;
    iAddrLen = sizeof(ClientAddr);
    vosMemSet(&ClientAddr, 0x00, iAddrLen);

    if (0 == getpeername(ClientSock, (struct sockaddr *)&ClientAddr, &iAddrLen))
    {
       clientIpAddr = ntohl(ClientAddr.sin_addr.s_addr);
    }
    return clientIpAddr;
}

/*******************************************************************************
*
* vosSocketClose:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketClose(int sock)
{
    return close(sock);
}

/*******************************************************************************
*
* vosSocketConnect:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketConnect(int sock, char *pName, uint16 port )
{
    struct sockaddr_in  RemAddr;
    unsigned int  ulHostAddress;
    int                 err;
    char               *pMsg;
    vosMemSet(&RemAddr, 0x00, sizeof(RemAddr));
    ulHostAddress = inet_addr(pName);
    if ( 0 == ulHostAddress )
    {
        return 0;
    }
    RemAddr.sin_family      = AF_INET;
    RemAddr.sin_addr.s_addr = ulHostAddress;
    RemAddr.sin_port        = htons((unsigned short)port);
    /* Create a Client connection */

    err = connect( sock, (struct sockaddr *)&RemAddr, sizeof(RemAddr));
    if ( -1 == err )
    {return 0;
    }
    return 0;
}

/*******************************************************************************
*
* vosSocketConnectTo:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketConnectTo(int *pSock, char *pName, uint16 port )
{
    int status;
    /* Create a stream socket */
    *pSock = vosSocketTcpCreate();
    if (-1 == *pSock)
        return 0;
    status = vosSocketBind(*pSock, INADDR_ANY, ANY_PORT);
    if (0 != status)
        return 0;
    /* Create a Client connection */
    status = vosSocketConnect( *pSock, pName, port );
    return status;
}

/*******************************************************************************
*
* vosSocketCanRead:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*   sock    - socket to read
*	timeout - timeout in millisecond
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*   select
*/
int vosSocketCanRead(int sock, int timeout)
{
    fd_set          fdSocks; 
    struct timeval  tTimeout; 
    
    FD_ZERO(&fdSocks);
    FD_SET(sock, &fdSocks);
    
    tTimeout.tv_sec  = timeout / 1000; 
    tTimeout.tv_usec = (timeout % 1000) * 1000;
    
    return select(sock+1, &fdSocks, 0, 0, &tTimeout);  
}

/*******************************************************************************
*
* vosSocketRead:	 
*
* DESCRIPTION:
* 	sock    - socket to receive
*   pBuf    - pointer to buffer to receive
*   bufSize - size of the buffer
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	The number of bytes received
*
* SEE ALSO: 
*/
int vosSocketRead(int sock, char *pBuf, int bufSize)
{
    /* There's data in the socket, so nab all you can... */
    return recv(sock, pBuf, bufSize, 0);
}


/*******************************************************************************
*
* vosSocketCanWrite:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*   sock    - socket to write
*	timeout - timeout in millisecond
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*   select
*/
int vosSocketCanWrite(int sock, int timeout)
{
    fd_set          fdSocks; 
    struct timeval  tTimeout; 
    
    FD_ZERO(&fdSocks);
    FD_SET(sock, &fdSocks);
    
    tTimeout.tv_sec  = timeout / 1000; 
    tTimeout.tv_usec = (timeout % 1000) * 1000;
    
    return select(sock+1, 0, &fdSocks, 0, &tTimeout);  
}


/*******************************************************************************
*
* vosSocketWrite:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	sock    - socket to send to
*   pBuf    - pointer to buffer to transmit 
*   bufLen  - length of buffer 
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosSocketWrite(int sock, char *pBuf, int BufLen)
{
    return send(sock, pBuf, BufLen, 0);
}
    

