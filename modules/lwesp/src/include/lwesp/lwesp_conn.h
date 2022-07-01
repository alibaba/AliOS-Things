/**
 * \file            lwesp_conn.h
 * \brief           Connection API
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
#ifndef LWESP_HDR_CONN_H
#define LWESP_HDR_CONN_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_CONN Connection API
 * \brief           Connection API functions
 * \{
 */

lwespr_t    lwesp_conn_start(lwesp_conn_p* conn, lwesp_conn_type_t type, uint8_t linkid, const char* const remote_host, lwesp_port_t remote_port, void* const arg, lwesp_evt_fn conn_evt_fn, const uint32_t blocking);
lwespr_t    lwesp_conn_startex(lwesp_conn_p* conn, lwesp_conn_start_t* start_struct, void* const arg, lwesp_evt_fn conn_evt_fn, const uint32_t blocking);

lwespr_t    lwesp_conn_close(lwesp_conn_p conn, const uint32_t blocking);
lwespr_t    lwesp_conn_send(lwesp_conn_p conn, const void* data, size_t btw, size_t* const bw, const uint32_t blocking);
lwespr_t    lwesp_conn_sendto(lwesp_conn_p conn, const lwesp_ip_t* const ip, lwesp_port_t port, const void* data, size_t btw, size_t* bw, const uint32_t blocking);
lwespr_t    lwesp_conn_set_arg(lwesp_conn_p conn, void* const arg);
void*       lwesp_conn_get_arg(lwesp_conn_p conn);
uint8_t     lwesp_conn_is_client(lwesp_conn_p conn);
uint8_t     lwesp_conn_is_server(lwesp_conn_p conn);
uint8_t     lwesp_conn_is_active(lwesp_conn_p conn);
uint8_t     lwesp_conn_is_closed(lwesp_conn_p conn);
int8_t      lwesp_conn_getnum(lwesp_conn_p conn);
lwespr_t    lwesp_conn_set_ssl_buffersize(size_t size, const uint32_t blocking);
lwespr_t    lwesp_get_conns_status(const uint32_t blocking);
lwesp_conn_p  lwesp_conn_get_from_evt(lwesp_evt_t* evt);
lwespr_t    lwesp_conn_write(lwesp_conn_p conn, const void* data, size_t btw, uint8_t flush, size_t* const mem_available);
lwespr_t    lwesp_conn_recved(lwesp_conn_p conn, lwesp_pbuf_p pbuf);
size_t      lwesp_conn_get_total_recved_count(lwesp_conn_p conn);

uint8_t     lwesp_conn_get_remote_ip(lwesp_conn_p conn, lwesp_ip_t* ip);
lwesp_port_t  lwesp_conn_get_remote_port(lwesp_conn_p conn);
lwesp_port_t  lwesp_conn_get_local_port(lwesp_conn_p conn);
lwespr_t    lwesp_conn_ssl_set_config(uint8_t link_id, uint8_t auth_mode, uint8_t pki_number, uint8_t ca_number, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);


/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_CONN_H */
