#ifndef _WRAPPERS_UDP_H_
#define _WRAPPERS_UDP_H_

#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_compat.h"

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

uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname);
#endif

