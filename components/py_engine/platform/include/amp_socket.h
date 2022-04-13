/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AMP_SOCKET_H_
#define _AMP_SOCKET_H_

#include "infra_types.h"
#include "infra_compat.h"
#include "amp_platform.h"

intptr_t HAL_UDP_SocketCreate();

intptr_t HAL_UDP_SocketBind(intptr_t p_socket, unsigned short port);

intptr_t HAL_UDP_create(char *host, unsigned short port);

int HAL_UDP_write(intptr_t p_socket,
                  const unsigned char *p_data,
                  unsigned int datalen);

int HAL_UDP_readTimeout(intptr_t p_socket,
                        unsigned char *p_data,
                        unsigned int datalen,
                        unsigned int timeout);

intptr_t HAL_UDP_create_without_connect(const char *host, unsigned short port);


int HAL_UDP_close_without_connect(intptr_t sockfd);

int HAL_UDP_joinmulticast(intptr_t sockfd,
                          char *p_group);

int HAL_UDP_recvfrom(intptr_t sockfd,
                     NetworkAddr *p_remote,
                     unsigned char *p_data,
                     unsigned int datalen,
                     unsigned int timeout_ms);

int HAL_UDP_sendto(intptr_t sockfd,
                   const NetworkAddr *p_remote,
                   const unsigned char *p_data,
                   unsigned int datalen,
                   unsigned int timeout_ms);

uintptr_t HAL_TCP_Establish(const char *host, uint16_t port);

int HAL_TCP_Destroy(uintptr_t fd);

int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms);

int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms);

struct hostent *HAL_Httpc_Socket_GetHostByName(const char *name);

int32_t HAL_Httpc_Socket_Connect(uintptr_t fd, const struct sockaddr *name, socklen_t namelen);

#endif /* _AMP_SOCKET_H_ */

