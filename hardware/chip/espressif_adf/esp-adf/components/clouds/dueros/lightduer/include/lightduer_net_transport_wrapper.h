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
// Description: Define the adapter for networks.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANSPORT_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANSPORT_H

#include "lightduer_net_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Acquire the handler.
 *
 * @Param func, in, the callback when transport event notify.
 * @Param key_info, something used in down layer but generated on up layer
 * @Return duer_trans_handler, the transport context
 */
DUER_INT duer_trans_handler duer_trans_acquire(duer_transevt_func func, const void *key_info);

/*
 * Set the public key or certificate.
 *
 * @Param hdlr, in, the context for the transport
 * @Param data, in, the content of the public key or certificate
 * @Param size, in, the size of the data
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_set_pk(duer_trans_handler hdlr,
                                         const void* data,
                                         duer_size_t size);

/*
 * clear the public key or certificate, release the buffer
 *
 * @Param hdlr, in, the context for the transport
 */
DUER_INT duer_status_t duer_trans_unset_pk(duer_trans_handler hdlr);

/*
 * Set the timeout for receiving data.
 *
 * @Param hdlr, in, the context for the transport
 * @Param timeout, in, the recv data timeout interval
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_set_read_timeout(duer_trans_handler hdlr,
                                                   duer_u32_t timeout);

/*
 * Connect to the host.
 *
 * @Param hdlr, in, the context for the transport
 * @Param addr, in, the target address infomations
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_connect(duer_trans_handler hdlr,
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
DUER_INT duer_status_t duer_trans_send(duer_trans_handler hdlr,
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
DUER_INT duer_status_t duer_trans_recv(duer_trans_handler hdlr,
                                       void* data,
                                       duer_size_t size,
                                       duer_addr_t* addr);


DUER_INT duer_status_t duer_trans_recv_timeout(duer_trans_handler hdlr,
                                       void* data,
                                       duer_size_t size,
                                       duer_addr_t* addr);
/*
 * Disconnect from the remote host.
 *
 * @Param hdlr, in, the context for the transport
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_close(duer_trans_handler hdlr);

/*
 * Release the handler.
 *
 * @Param hdlr, in, the context for the transport
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_release(duer_trans_handler hdlr);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANSPORT_H
