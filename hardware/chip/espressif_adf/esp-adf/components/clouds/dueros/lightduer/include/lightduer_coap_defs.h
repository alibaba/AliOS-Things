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
// Description: CoAP common definitions

#ifndef LIBDUER_DEVICE_FRAMEWORK_INCLUDE_LIGHTDUER_COAP_DEFS_H
#define LIBDUER_DEVICE_FRAMEWORK_INCLUDE_LIGHTDUER_COAP_DEFS_H

#include "lightduer_types.h"
#include "lightduer_network_defs.h"
#include "lightduer_net_transport.h"

/*
 * The message codes.
 */
typedef enum {
    DUER_MSG_EMPTY_MESSAGE                       = 0,
    // Request message code
    DUER_MSG_REQ_GET                             = 1,
    DUER_MSG_REQ_POST                            = 2,
    DUER_MSG_REQ_PUT                             = 3,
    DUER_MSG_REQ_DELETE                          = 4,

    // Response message code
    DUER_MSG_RSP_CREATED                         = 65,   // 2.01
    DUER_MSG_RSP_DELETED                         = 66,   // 2.02
    DUER_MSG_RSP_VALID                           = 67,   // 2.03
    DUER_MSG_RSP_CHANGED                         = 68,   // 2.04
    DUER_MSG_RSP_CONTENT                         = 69,   // 2.05
    DUER_MSG_RSP_CONTINUE                        = 95,   // 2.31
    DUER_MSG_RSP_BAD_REQUEST                     = 128,  // 4.00
    DUER_MSG_RSP_UNAUTHORIZED                    = 129,  // 4.01
    DUER_MSG_RSP_BAD_OPTION                      = 130,  // 4.02
    DUER_MSG_RSP_FORBIDDEN                       = 131,  // 4.03
    DUER_MSG_RSP_NOT_FOUND                       = 132,  // 4.04
    DUER_MSG_RSP_METHOD_NOT_ALLOWED              = 133,  // 4.05
    DUER_MSG_RSP_NOT_ACCEPTABLE                  = 134,  // 4.06
    DUER_MSG_RSP_REQUEST_ENTITY_INCOMPLETE       = 136,  // 4.08
    DUER_MSG_RSP_PRECONDITION_FAILED             = 140,  // 4.12
    DUER_MSG_RSP_REQUEST_ENTITY_TOO_LARGE        = 141,  // 4.13
    DUER_MSG_RSP_UNSUPPORTED_CONTENT_FORMAT      = 143,  // 4.15
    DUER_MSG_RSP_INTERNAL_SERVER_ERROR           = 160,  // 5.00
    DUER_MSG_RSP_NOT_IMPLEMENTED                 = 161,  // 5.01
    DUER_MSG_RSP_BAD_GATEWAY                     = 162,  // 5.02
    DUER_MSG_RSP_SERVICE_UNAVAILABLE             = 163,  // 5.03
    DUER_MSG_RSP_GATEWAY_TIMEOUT                 = 164,  // 5.04
    DUER_MSG_RSP_PROXYING_NOT_SUPPORTED          = 165,  // 5.05
    DUER_MSG_RSP_INVALID                         = 0xFF, // used in seperate response

} duer_msg_code_e;

/**
 * CoAP Message type, used in CoAP Header
 */
typedef enum {
    DUER_MSG_TYPE_CONFIRMABLE       = 0x00, // Reliable Request messages
    DUER_MSG_TYPE_NON_CONFIRMABLE   = 0x10, // Non-reliable Request and Response messages
    DUER_MSG_TYPE_ACKNOWLEDGEMENT   = 0x20, // Response to a Confirmable Request
    DUER_MSG_TYPE_RESET             = 0x30  // Answer a Bad Request
} duer_msg_type_e;

/*
 * The resource operation permission
 */
typedef enum {
    DUER_RES_OP_GET      = 0x01,     // Get operation allowed
    DUER_RES_OP_PUT      = 0x02,     // Put operation allowed
    DUER_RES_OP_POST     = 0x04,     // Post operation allowed
    DUER_RES_OP_DELETE   = 0x08      // Delete operation allowed
} duer_resource_operation_e;

/*
 * The resource mode
 */
typedef enum {
    DUER_RES_MODE_STATIC,            // Static resources have some value that doesn't change
    DUER_RES_MODE_DYNAMIC,           // Dynamic resources are handled in application
} duer_resource_mode_e;

typedef struct _duer_context_s duer_context_t;

typedef int (*duer_report_callback)(duer_context_t *);

struct _duer_context_s {
    void *                  _param;

    int                     _status;

    duer_report_callback    _on_report_start;
    duer_report_callback    _on_report_finish;
};

/*
 * The message definition
 */
typedef struct _duer_message_s {
    duer_u16_t token_len;

    duer_u8_t  msg_type;
    duer_u8_t  msg_code;
    duer_u16_t msg_id;

    duer_u16_t path_len;
    duer_u16_t query_len;
    duer_u16_t payload_len;

    duer_u8_t* token;
    duer_u8_t* path;
    duer_u8_t* query;
    duer_u8_t* payload;

    duer_u32_t create_timestamp;
    duer_context_t *context;
} duer_msg_t;

/*
 * The status notification to user.
 */
typedef duer_status_t (*duer_notify_f)(duer_context ctx,
                                       duer_msg_t* msg,
                                       duer_addr_t* addr);

/*
 * The transmit coap data callback.
 */
typedef duer_status_t (*duer_transmit_f)(duer_trans_handler hdlr, const void *data, duer_size_t size, duer_addr_t *addr);

/*
 * The resource for user
 */
typedef struct _duer_resource_s {
    duer_u8_t    mode: 2;    // the resource mode, SEE in ${link duer_resource_mode_e}
    duer_u8_t    allowed: 6; // operation permission, SEE in ${link duer_resource_operation_e}
    char*       path;       // the resource path identify

    union {
        duer_notify_f   f_res;   // dynamic resource handle function, NULL if static
        struct {
            void*       data;   // static resource value data, NULL if dynamic
            duer_size_t  size;   // static resource size
        } s_res;
    } res;
} duer_res_t;

#define DUER_MESSAGE_IS_RESPONSE(_code)     ((_code) > DUER_MSG_REQ_DELETE)

#endif // LIBDUER_DEVICE_FRAMEWORK_INCLUDE_LIGHTDUER_COAP_DEFS_H
