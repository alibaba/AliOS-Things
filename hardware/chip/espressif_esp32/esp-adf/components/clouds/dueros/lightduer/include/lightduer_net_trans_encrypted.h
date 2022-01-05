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
// Description: Define the encrypted networks I/O.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANS_ENCRYPTED_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANS_ENCRYPTED_H

#include "lightduer_net_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Connect to the host.
 *
 * @Param hdlr, in, the context for the transport
 * @Param addr, in, the target address infomations
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_encrypted_connect(duer_trans_ptr trans,
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
DUER_INT duer_status_t duer_trans_encrypted_send(duer_trans_ptr trans,
                                                 const void* data,
                                                 duer_size_t size,
                                                 const duer_addr_t* addr);

/*
 * Set the timeout for receiving data.
 *
 * @Param hdlr, in, the context for the transport
 * @Param timeout, in, the recv data timeout interval
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_encrypted_set_read_timeout(duer_trans_ptr trans,
                                                             duer_u32_t timeout);

/*
 * Receive data.
 *
 * @Param hdlr, in, the context for the transport
 * @Param data, out, the data will be read
 * @Param size, in, the data size
 * @Param addr, out, the target address infomations
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_trans_encrypted_recv(duer_trans_ptr trans,
                                                 void* data,
                                                 duer_size_t size,
                                                 duer_addr_t* addr);


DUER_INT duer_status_t duer_trans_encrypted_recv_timeout(duer_trans_ptr trans,
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
DUER_INT duer_status_t duer_trans_encrypted_close(duer_trans_ptr trans);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TRANS_ENCRYPTED_H
