/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_UDP_H_
#define _AOS_UDP_H_

#define NETWORK_ADDR_LEN (16)

typedef struct {
    unsigned char
    addr[NETWORK_ADDR_LEN];
    unsigned short  port;
} aos_networkAddr;

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_socket_create();

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_socket_bind(int p_socket, unsigned short port);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_create(char *host, unsigned short port);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_write(int p_socket,
                  const unsigned char *p_data,
                  unsigned int datalen);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_read_timeout(int p_socket,
                        unsigned char *p_data,
                        unsigned int datalen,
                        unsigned int timeout);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_create_without_connect(const char *host, unsigned short port);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_close_without_connect(int sockfd);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_joinmulticast(int sockfd,
                          char *p_group);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_recvfrom(int sockfd,
                     aos_networkAddr *p_remote,
                     unsigned char *p_data,
                     unsigned int datalen,
                     unsigned int timeout_ms);

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_udp_sendto(int sockfd,
                   const aos_networkAddr *p_remote,
                   const unsigned char *p_data,
                   unsigned int datalen,
                   unsigned int timeout_ms);

#endif /* _AOS_UDP_H_ */
