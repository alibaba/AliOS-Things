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


#ifndef __COAP_SERVER_H__
#define __COAP_SERVER_H__

#include "CoAPExport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CoAPContext *CoAPServer_init();

#ifndef COAP_WITH_YLOOP
void *CoAPServer_yield(void *param);
#endif
void CoAPServer_add_timer (void (*on_timer)(void*));
void CoAPServer_loop(CoAPContext *context);


void CoAPServer_deinit(CoAPContext *context);
//void CoAPServer_deinit0(CoAPContext *context);

int CoAPServer_register(CoAPContext *context, const char *uri, CoAPRecvMsgHandler callback);

int CoAPServerMultiCast_send(CoAPContext *context, NetworkAddr *remote, const char *uri,
        unsigned char *buff, unsigned short len, CoAPSendMsgHandler callback, unsigned short *msgid);

int CoAPServerResp_send(CoAPContext *context, NetworkAddr *remote, unsigned char *buff, unsigned short len, void *req, const char *paths);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
