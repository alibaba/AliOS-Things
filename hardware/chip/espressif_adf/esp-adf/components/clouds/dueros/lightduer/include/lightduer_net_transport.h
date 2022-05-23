/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// Author: Su Hao (suhao@baidu.com)
//
// Description: The wrapper for user's transport.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANS_WRAPPER_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANS_WRAPPER_H

#include "lightduer_network_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DUER_READ_FOREVER        ((duer_u32_t)-1)

typedef void*          duer_socket_t;

/*
 * The handler operate transfer.
 */
typedef void* duer_trans_handler;

#define DUER_SIZEOF_TCPHEADER    8        // more info _baidu_ca_coap_tcp_header_s lightduer_coap.c
#ifdef NET_TRANS_ENCRYPTED_BY_AES_CBC
#define DUER_MIN_BODY_SIZE       (16 + 1) // see http://agroup.baidu.com/duersh/md/article/475516
#else
#define DUER_MIN_BODY_SIZE       (0)
#endif
#define DUER_MAX_BODY_SIZE       2048     // body longest value

typedef struct _duer_trans_s {
    duer_socket_t       ctx;
    duer_context        secure;
    duer_transevt_func  transevt_callback;
    void*               cert;
    duer_size_t         cert_len;
    duer_u32_t          read_timeout;
    duer_addr_t         addr;
    const void          *key_info;
    duer_bool           is_encrypted;
#ifdef NET_TRANS_ENCRYPTED_BY_AES_CBC
    unsigned char       received_header[DUER_SIZEOF_TCPHEADER]; // header info
    duer_size_t         received_header_bytes; // received header bytes[0-DUER_SIZEOF_TCPHEADER]
    unsigned char       received_body[DUER_MAX_BODY_SIZE]; // body [0-DUER_MAX_BODY_SIZE]
    duer_size_t         received_body_bytes; // received body bytes
#endif
} duer_trans_t, *duer_trans_ptr;

/*
 * The network transfer data callbacks.
 * See the detail in @{link baidu_ca_transport_init}
 */
typedef duer_socket_t (*duer_soc_create_f)(duer_transevt_func func);
typedef duer_status_t (*duer_soc_connect_f)(duer_socket_t sock,
                                            const duer_addr_t* addr);
typedef duer_status_t (*duer_soc_send_f)(duer_socket_t sock,
                                         const void* data,
                                         duer_size_t size,
                                         const duer_addr_t* addr);
typedef duer_status_t (*duer_soc_recv_f)(duer_socket_t sock,
                                         void* data,
                                         duer_size_t size,
                                         duer_addr_t* addr);
typedef duer_status_t (*duer_soc_recv_timeout_f)(duer_socket_t sock,
                                                 void* data,
                                                 duer_size_t size,
                                                 duer_u32_t timeout,
                                                 duer_addr_t* addr);
typedef duer_status_t (*duer_soc_close_f)(duer_socket_t sock);
typedef duer_status_t (*duer_soc_destroy_f)(duer_socket_t sock);

/*
 * Set network input/ouput function
 *
 * @Param f_create, in, the function create socket context
 * @Param f_conn, in, the function connect socket to host
 * @Param f_send, in, the function send message
 * @Param f_recv, in, the function receive message
 * @Param f_recv_timeout, in, the function receive message with timeout
 * @Param f_close, in, the function close the socket
 * @Param f_destroy, in, the function destroy the socket context
 */
DUER_EXT void baidu_ca_transport_init(duer_soc_create_f f_create,
                                      duer_soc_connect_f f_conn,
                                      duer_soc_send_f f_send,
                                      duer_soc_recv_f f_recv,
                                      duer_soc_recv_timeout_f f_recv_timeout,
                                      duer_soc_close_f f_close,
                                      duer_soc_destroy_f f_destroy);

/*
 * Create the socket context.
 *
 * @Param hdlr, in, the context for the transport
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_wrapper_create(duer_trans_ptr trans);

/*
 * Connect to the host.
 *
 * @Param hdlr, in, the context for the transport
 * @Param addr, in, the target address infomations
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_wrapper_connect(duer_trans_ptr trans,
                                                  const duer_addr_t* addr);

/*
 * Send data.
 *
 * @Param hdlr, in, the context for the transport
 * @Param data, in, the data will be sent
 * @Param size, in, the data size
 * @Param addr, in, the target address infomations
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_wrapper_send(duer_trans_ptr trans,
                                               const void* data,
                                               duer_size_t size,
                                               const duer_addr_t* addr);

/*
 * Receive data.
 *
 * @Param hdlr, in, the context for the transport
 * @Param data, out, the data will be read
 * @Param size, in, the data size
 * @Param addr, out, the target address infomations
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_wrapper_recv(duer_trans_ptr trans,
                                               void* data,
                                               duer_size_t size,
                                               duer_addr_t* addr);

/*
 * Receive data.
 *
 * @Param hdlr, in, the context for the transport
 * @Param data, out, the data will be read
 * @Param size, in, the data size
 * @Param timeout, in, the receive timeout
 * @Param addr, out, the target address infomations
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_wrapper_recv_timeout(duer_trans_ptr trans,
                                                       void* data,
                                                       duer_size_t size,
                                                       duer_u32_t timeout,
                                                       duer_addr_t* addr);

/*
 * Disconnect from the remote host.
 *
 * @Param hdlr, in, the context for the transport
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_wrapper_close(duer_trans_ptr trans);

/*
 * Destroy the socket context.
 *
 * @Param hdlr, in, the context for the transport
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_wrapper_destroy(duer_trans_ptr trans);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANS_WRAPPER_H
