/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <aos/log.h>
#include <aos/network.h>
//#include "coap_transport.h"
#include "iot_import_coap.h"

#define TRANSPORT_ADDR_LEN 16

#ifndef IP_PKTINFO
#define IP_PKTINFO   IP_MULTICAST_IF
#endif

#ifndef IPV6_PKTINFO
#define IPV6_PKTINFO IPV6_V6ONL
#endif

#define NETWORK_ADDR_LEN      (16)

/**@brief Create udp socket.
*
* @details Create udp socket.
*
* @retval COAP_SUCCESS   Indicates if udp socket was created successfully, else an an  error code
*                       indicating reason for failure.
*/
int HAL_UDP_create(void *p_socket)
{
    int flag = 1;
    int ret = -1;
    int sockfd = -1;

    if(NULL == p_socket){
        return -1;
    }

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        return -1;
    }

    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(ret < 0){
        close(sockfd);
        return -1;
    }

    flag = 1;
#ifdef IP_RECVPKTINFO
    if((ret = setsockopt(sockfd, IPPROTO_IP, IP_RECVPKTINFO, &flag, sizeof(flag))) < 0)
#else
    if((ret = setsockopt(sockfd, IPPROTO_IP, IP_PKTINFO, &flag, sizeof(flag))) < 0)
#endif
        if (ret < 0){
            return -1;
        }

    *(int *)p_socket  = sockfd;
    return 0;
}

 void HAL_UDP_close(void *p_socket)
{
    int socket_id = -1;

    if(NULL != p_socket){
        socket_id = *(int *)p_socket;
        close(socket_id);
    }
}

int HAL_UDP_write(void               *p_socket,
                       const coap_address_t    *p_remote,
                       const unsigned char    *p_data,
                       unsigned int            datalen)
{
    int rc = -1;
    int socket_id = -1;
    struct sockaddr_in remote_addr;

    if(NULL == p_socket) {
        return -1;
    }

    socket_id = *((int *)p_socket);
    remote_addr.sin_family = AF_INET;
    if(1 != (rc = inet_pton(remote_addr.sin_family, (const char *)p_remote->addr, &remote_addr.sin_addr.s_addr)))
    {
        return -1;
    }
    remote_addr.sin_port = htons(p_remote->port);
    rc = sendto(socket_id, p_data, (size_t)datalen, 0,
                (const struct sockaddr *)&remote_addr, sizeof(remote_addr));
    if(-1 == rc)
    {
        return -1;
    }
    return rc;

}


int HAL_UDP_read(void                *p_socket,
                              coap_address_t   *p_remote,
                              unsigned char   *p_data,
                              unsigned int     datalen)
{
    int socket_id = -1;
    struct sockaddr from;
    int count = -1;
    int addrlen = 0;

    if(NULL == p_remote || NULL == p_data || NULL == p_socket)
    {
        return -1;
    }

    socket_id = *(int *)p_socket;
    addrlen = sizeof(struct sockaddr);
    count = recvfrom(socket_id, p_data, (size_t)datalen, 0, &from, (unsigned int*)&addrlen);
    if(-1 == count)
    {
        return -1;
    }
    if (from.sa_family == AF_INET)
    {
        struct sockaddr_in *sin = (struct sockaddr_in *)&from;
        inet_ntop(AF_INET, &sin->sin_addr, (char *)p_remote->addr, TRANSPORT_ADDR_LEN);
        p_remote->port = ntohs(sin->sin_port);
    }
    return count;
}

int HAL_UDP_readTimeout( void *p_socket,
                              coap_address_t *p_remote, unsigned char  *p_data,
                              unsigned int datalen,     unsigned int timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int socket_id = -1;

    if(NULL == p_socket || NULL == p_data){
        return -1;
    }
    socket_id = *(int *)p_socket;

    if( socket_id < 0 )
      return -1;

    FD_ZERO( &read_fds );
    FD_SET( socket_id, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = select( socket_id + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );

    /* Zero fds ready means we timed out */
    if( ret == 0 )
      return -2; // receive timeout

    if( ret < 0 )
    {
        if( errno == EINTR )
          return -3; //want read

        return -4; //receive failed
    }

    /* This call will not block */
    return HAL_UDP_read(p_socket, p_remote, p_data, datalen);
}


int HAL_UDP_resolveAddress(const char *p_host, char addr[NETWORK_ADDR_LEN])
{
    struct addrinfo *res, *ainfo;
    struct addrinfo hints = {.ai_socktype = SOCK_DGRAM, .ai_family = AF_UNSPEC};
    int error, len = -1;
    struct sockaddr_in *sa = NULL;

//    memset ((char *)&hints, 0x00, sizeof(hints));
//    hints.ai_socktype = SOCK_DGRAM;
//    hints.ai_family = AF_UNSPEC;

    error = getaddrinfo(p_host, NULL, &hints, &res);

    if (error != 0)
    {
//        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        fprintf(stderr, "error getaddrinfo: %d\n", error);
        return error;
    }

    for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next)
    {
        switch (ainfo->ai_family)
        {
            case AF_INET:
                len = ainfo->ai_addrlen;
                sa = (struct sockaddr_in *)ainfo->ai_addr;
                inet_ntop(AF_INET, &sa->sin_addr, (char *)addr, NETWORK_ADDR_LEN);
                break;
            default:
                ;
        }
    }

    freeaddrinfo(res);
    return 0;
}

