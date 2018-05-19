/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __CoAPNETWORK_H__
#define __CoAPNETWORK_H__
#include <stdint.h>
#include "iot_import.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum
{
    COAP_NETWORK_NOSEC = 0,
    COAP_NETWORK_DTLS,
}CoAPNetworkType;

typedef struct
{
    CoAPNetworkType       type;
    unsigned short        port;
    intptr_t             fd;
}NetworkConf;

typedef void NetworkContext;


typedef struct
{
    CoAPNetworkType       type;
    char                  *group;
    unsigned short        port;
#ifdef COAP_DTLS_SUPPORT
    // TODO:
#endif
} NetworkInit;

NetworkContext *CoAPNetwork_init (const NetworkInit   *p_param);


int CoAPNetwork_write(NetworkContext          *p_context,
                                         NetworkAddr   *p_remote,
                                  const unsigned char  *p_data,
                                  unsigned int          datalen,
                                  unsigned int          timeout);

int CoAPNetwork_read(NetworkContext *p_context,
                            NetworkAddr    *p_remote,
                            unsigned char  *p_data,
                            unsigned int datalen,
                            unsigned int timeout);

void CoAPNetwork_deinit(NetworkContext *p_context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

