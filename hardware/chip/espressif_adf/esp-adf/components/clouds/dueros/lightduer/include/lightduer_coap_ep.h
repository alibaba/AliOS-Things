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
// Description: The CoAP endpoint definitions.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_COAP_EP_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_COAP_EP_H

#include "lightduer_lib.h"

#define LIFETIME_MAX_LEN    (10)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Endpoint registration parameters
 */
typedef struct _baidu_ca_coap_endpoint_s {
    char*          name_ptr;           // Endpoint name
    duer_size_t     name_len;

    char*          type_ptr;           // Endpoint type
    duer_size_t     type_len;

    char*          lifetime_ptr;       // Endpoint lifetime in seconds. eg. "1200" = 1200 seconds
    duer_size_t     lifetime_len;

    duer_addr_t*    address; // Endpoint address to be accessed by the server, optional, default NULL
} duer_coap_ep_t, *duer_coap_ep_ptr;

/*
 * Create the endpoint.
 *
 * @Param hdlr, in, the global context for baidu ca
 * @Param name, in, the endpoint name
 * @Param type, in, the endpoint type
 * @Param lifetime, in, the lifetime of the endpoint
 * @Param addr, in, the endpoint address to be accessed by the server, optional, default NULL
 * @Return duer_coap_ep_ptr, the endpoint context pointer
 */
DUER_INT duer_coap_ep_ptr duer_coap_ep_create(const char* name,
                                              const char* type,
                                              duer_u32_t lifetime,
                                              const duer_addr_t* addr);


/*
 * Destroy the endpoint that created by ${link duer_coap_ep_create}.
 *
 * @Param hdlr, in, the global context for baidu ca
 * @Param ptr, in, the endpoint context pointer
 * @Return duer_status_t, the operation result
 */
DUER_INT duer_status_t duer_coap_ep_destroy(duer_coap_ep_ptr ptr);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_COAP_EP_H
