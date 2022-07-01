/**
 * \file            lwesp_netconn.h
 * \brief           API functions for sequential calls
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#ifndef LWESP_HDR_NETCONN_H
#define LWESP_HDR_NETCONN_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP_API
 * \defgroup        LWESP_NETCONN Network connection
 * \brief           Network connection
 * \{
 */

struct lwesp_netconn;

/**
 * \brief           Netconn object structure
 */
typedef struct lwesp_netconn* lwesp_netconn_p;

/**
 * \brief           Receive data with no timeout
 * \note            Used with \ref lwesp_netconn_set_receive_timeout function
 */
#define LWESP_NETCONN_RECEIVE_NO_WAIT             0xFFFFFFFF
#define LWESP_NETCONN_ACCEPT_NO_WAIT              (LWESP_NETCONN_RECEIVE_NO_WAIT)
#define LWESP_NETCONN_CONNECT_NO_WAIT             (LWESP_NETCONN_RECEIVE_NO_WAIT)
/**
 * \brief           Netconn connection type
 */
typedef enum {
    LWESP_NETCONN_TYPE_TCP = LWESP_CONN_TYPE_TCP,   /*!< TCP connection */
    LWESP_NETCONN_TYPE_SSL = LWESP_CONN_TYPE_SSL,   /*!< SSL connection */
    LWESP_NETCONN_TYPE_UDP = LWESP_CONN_TYPE_UDP,   /*!< UDP connection */
#if LWESP_CFG_IPV6 || __DOXYGEN__
    LWESP_NETCONN_TYPE_TCPV6 = LWESP_CONN_TYPE_TCPV6,   /*!< TCP connection over IPv6 */
    LWESP_NETCONN_TYPE_SSLV6 = LWESP_CONN_TYPE_SSLV6,   /*!< SSL connection over IPv6 */
#endif /* LWESP_CFG_IPV6 || __DOXYGEN__ */
    LWESP_NETCONN_TYPE_MAX = LWESP_NETCONN_TYPE_UDP + 1, 
} lwesp_netconn_type_t;

lwesp_netconn_p      lwesp_netconn_new(lwesp_netconn_type_t type);
lwespr_t             lwesp_netconn_delete(lwesp_netconn_p nc);
lwespr_t             lwesp_netconn_bind(lwesp_netconn_p nc, lwesp_port_t port);
lwespr_t             lwesp_netconn_connect(lwesp_netconn_p nc, const char* host, lwesp_port_t port);
lwespr_t             lwesp_netconn_receive(lwesp_netconn_p nc, lwesp_pbuf_p* pbuf);
lwespr_t             lwesp_netconn_close(lwesp_netconn_p nc);
int8_t               lwesp_netconn_get_connnum(lwesp_netconn_p nc);
lwesp_conn_p         lwesp_netconn_get_conn(lwesp_netconn_p nc);
lwesp_netconn_type_t lwesp_netconn_get_type(lwesp_netconn_p nc);
void                 lwesp_netconn_set_receive_timeout(lwesp_netconn_p nc, uint32_t timeout);
void                 lwesp_netconn_set_accept_timeout(lwesp_netconn_p nc, uint32_t timeout);
void                 lwesp_netconn_set_connect_timeout(lwesp_netconn_p nc, uint32_t timeout);
uint32_t             lwesp_netconn_get_receive_timeout(lwesp_netconn_p nc);
lwespr_t lwesp_netconn_receive_ready(lwesp_netconn_p nc);
lwespr_t lwesp_netconn_accept_ready(lwesp_netconn_p nc);
 
lwespr_t             lwesp_netconn_connect_ex(lwesp_netconn_p nc, const char* host, lwesp_port_t port,
                                              uint16_t keep_alive, const char* local_ip, lwesp_port_t local_port, uint8_t mode);

/* TCP only */
lwespr_t             lwesp_netconn_listen(lwesp_netconn_p nc);
lwespr_t             lwesp_netconn_listen_with_max_conn(lwesp_netconn_p nc, uint16_t max_connections);
lwespr_t             lwesp_netconn_set_listen_conn_timeout(lwesp_netconn_p nc, uint16_t timeout);
lwespr_t             lwesp_netconn_accept(lwesp_netconn_p nc, lwesp_netconn_p* client);
lwespr_t             lwesp_netconn_write(lwesp_netconn_p nc, const void* data, size_t btw);
lwespr_t             lwesp_netconn_flush(lwesp_netconn_p nc);

/* UDP only */
lwespr_t             lwesp_netconn_send(lwesp_netconn_p nc, const void* data, size_t btw);
lwespr_t             lwesp_netconn_sendto(lwesp_netconn_p nc, const lwesp_ip_t* ip, lwesp_port_t port, const void* data, size_t btw);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_NETCONN_H */
