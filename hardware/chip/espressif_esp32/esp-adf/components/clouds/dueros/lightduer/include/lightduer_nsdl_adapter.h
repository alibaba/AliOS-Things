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
// Description: Adapter between nsdl and Baidu CA CoAP.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_NSDL_ADAPTER_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_NSDL_ADAPTER_H

#include "ns_types.h"
#include "sn_nsdl.h"
#include "sn_coap_header.h"
#include "lightduer_lib.h"
#include "lightduer_types.h"
#include "lightduer_coap.h"

#ifndef MBED_CLIENT_C_VERSION
#define MBED_CLIENT_C_VERSION (30001) // 3.0.1
#endif

/*
 * Set the nsdl address from ca
 *
 * @Param target, in, the target will be evaluated
 * @Param source, the CA address
 */
DUER_INT void duer_nsdl_address_set(sn_nsdl_addr_s* target,
                                  const duer_addr_t* source);

/*
 * Set the nsdl CoAP header from ca
 *
 * @Param target, in, the target will be evaluated
 * @Param opt, in, the CoAP options
 * @Param source, the CA message header
 */
DUER_INT void duer_nsdl_header_set(sn_coap_hdr_s* target,
                                 sn_coap_options_list_s* opt,
                                 const duer_msg_t* source);

/*
 * Set the CoAP address from nsdl
 *
 * @Param target, in, the target will be evaluated
 * @Param source, the nsdl address
 */
DUER_INT void duer_coap_address_set(duer_addr_t* target,
                                  const sn_nsdl_addr_s* source);

/*
 * Set the CoAP header from nsdl
 *
 * @Param target, in, the target will be evaluated
 * @Param source, the nsdl message header
 */
DUER_INT void duer_coap_header_set(duer_msg_t* target,
                                 const sn_coap_hdr_s* source);

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_NSDL_ADAPTER_H
