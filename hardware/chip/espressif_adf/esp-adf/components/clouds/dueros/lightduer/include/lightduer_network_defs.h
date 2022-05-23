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
// Description: Network common definitions

#ifndef LIBDUER_DEVICE_FRAMEWORK_INCLUDE_LIGHTDUER_NETWORK_DEFS_H
#define LIBDUER_DEVICE_FRAMEWORK_INCLUDE_LIGHTDUER_NETWORK_DEFS_H

#include "lightduer_types.h"

/*
 * protocol type
 */
enum _duer_transport_protocol_enum {
    DUER_PROTO_TCP,
    DUER_PROTO_UDP
};

/*
 * Transport Event
 */
typedef enum _duer_transport_event_enum {
    DUER_TEVT_CONNECT_RDY,
    DUER_TEVT_SEND_RDY,
    DUER_TEVT_RECV_RDY,
    DUER_TEVT_CLOSE_RDY,
    DUER_TEVT_SEND_TIMEOUT,
    DUER_TEVT_NONE = -1
} duer_transevt_e;

/*
 * The network address structure
 */
typedef struct _duer_address_s {
    duer_u8_t    type;       // the socket type
    duer_u16_t   port;       // the host port
    void*       host;       // the host address
    duer_size_t  host_size;  // the host address length
} duer_addr_t;

typedef void (*duer_transevt_func)(duer_transevt_e event);

#endif // LIBDUER_DEVICE_FRAMEWORK_INCLUDE_LIGHTDUER_NETWORK_DEFS_H
