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
#ifndef __COAP_OBSERVE_H__
#define __COAP_OBSERVE_H__
#include "CoAPExport.h"
#include "CoAPMessage.h"
#include "CoAPResource.h"
#include "lite-list.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct
{
    NetworkAddr              remote;
    unsigned char            token[COAP_MSG_MAX_TOKEN_LEN];
    unsigned char            tokenlen;
    unsigned char            ctype;
    CoAPResource             *p_resource_of_interest;
    unsigned int             observer_sequence_num;
    CoAPMessageCode          msg_type;
    struct list_head         obslist;
} CoapObserver;

typedef struct
{
    NetworkAddr            remote;
    unsigned char          token[COAP_MSG_MAX_TOKEN_LEN];
    unsigned char          tokenlen;
    CoAPSendMsgHandler     callback;
    unsigned int           max_age;
    struct list_head       obslist;
    void                   *userdata;
} CoAPObservable;

int CoAPObsServer_init(CoAPContext *context, unsigned char  obs_maxcount);
int CoAPObsServer_deinit(CoAPContext *context);

int CoAPObsServer_add(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *request);
int CoapObsServer_delete(CoAPContext *context,          NetworkAddr  *remote,
                                      CoAPResource *resource);
int CoapObsServerAll_delete(CoAPContext *context,          NetworkAddr  *remote);

int CoAPObsServer_notify(CoAPContext *context,
                            const char *path, unsigned char *payload,
                            unsigned short payloadlen, CoAPDataEncrypt handler);

int CoAPObsClient_init(CoAPContext *context, unsigned char  obs_maxcount);
int CoAPObsClient_deinit(CoAPContext *context);
int CoAPObsClient_add(CoAPContext *context, CoAPMessage *message, NetworkAddr *remote, CoAPSendNode *sendnode);
int CoAPObsClient_delete(CoAPContext *context, CoAPMessage *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
