/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxSocketUtil.h
 * Description : SocketUtil
 * History :
 *
 */

#ifndef CDX_SOCKET_UTIL_H
#define CDX_SOCKET_UTIL_H
#include <CdxTypes.h>

#include <sys/types.h>          /* See NOTES */
#include "lwip/sockets.h"
//#include <arpa/inet.h>
#include "lwip/inet.h"

#define CDX_SELECT_TIMEOUT 100000L /*100 ms*/

#ifdef __cplusplus
extern "C"
{
#endif
cdx_int32 CdxSockSetBlocking(cdx_int32 sockfd, cdx_int32 blocking);

cdx_err CdxMakeSocketBlocking(cdx_int32 s, cdx_bool blocking);

void CdxSocketMakePortPair(cdx_int32 *rtpSocket, cdx_int32 *rtcpSocket,
                                cdx_uint32 *rtpPort);

cdx_int32 CdxSockAddrConstruct(struct sockaddr_in *dest, char *ip, cdx_int32 port);

cdx_ssize CdxSockNoblockRecv(cdx_int32 sockfd, void *buf, cdx_size len);

cdx_ssize CdxSockAsynRecv(cdx_int32 sockfd, void *buf, cdx_size len,
                        cdx_long timeoutUs, cdx_int32 *pForceStop);

cdx_ssize CdxSockAsynSend(cdx_int32 sockfd, const void *buf, cdx_size len,
                          cdx_long timeoutUs, cdx_int32 *pForceStop);

cdx_int32 CdxAsynSocket(int domain, cdx_int32 *nRecvBufLen);

cdx_int32  CdxSockAsynConnect(cdx_int32 sockfd, const struct sockaddr *addr,
            socklen_t addrlen, cdx_long timeoutUs, cdx_int32 *pForceStop);

cdx_int32 CdxSockIsBlocking(cdx_int32 sockfd);

void CdxSockDisableTcpKeepalive(cdx_int32 sockfd);

#ifdef __cplusplus
}
#endif

#endif
