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

#ifndef __COAP_MESSAGE_H__
#define __COAP_MESSAGE_H__
#include "CoAPExport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct
{
    CoAPMsgHeader            header;
    unsigned char            retrans_count;
    unsigned char            token[COAP_MSG_MAX_TOKEN_LEN];
    unsigned short           timeout;
    unsigned short           timeout_val;
    unsigned int             msglen;
    CoAPSendMsgHandler       handler;
    NetworkAddr              remote;
    struct list_head         sendlist;
    void                    *user;
    unsigned char           *message;
    int                      acked;
    int                      keep;
} CoAPSendNode;


int CoAPStrOption_add(CoAPMessage *message, unsigned short optnum,
                unsigned char *data, unsigned short datalen);

int CoAPStrOption_get(CoAPMessage *message, unsigned short optnum,
                unsigned char *data, unsigned short *datalen);

int CoAPUintOption_add(CoAPMessage *message, unsigned short  optnum,
                unsigned int data);

int CoAPUintOption_get(CoAPMessage *message,
                              unsigned short  optnum,
                              unsigned int *data);

int CoAPOption_present(CoAPMessage *message, unsigned short option);


unsigned short CoAPMessageId_gen(CoAPContext *context);

int CoAPMessageId_set(CoAPMessage *message, unsigned short msgid);

int CoAPMessageType_set(CoAPMessage *message, unsigned char type);

int CoAPMessageCode_set(CoAPMessage *message, CoAPMessageCode code);

int CoAPMessageToken_set(CoAPMessage *message, unsigned char *token,
        unsigned char tokenlen);

int CoAPMessageUserData_set(CoAPMessage *message, void *userdata);

int CoAPMessagePayload_set(CoAPMessage *message, unsigned char *payload,
        unsigned short payloadlen);

int CoAPMessageHandler_set(CoAPMessage *message, CoAPSendMsgHandler handler);

int CoAPMessage_init(CoAPMessage *message);

int CoAPMessage_destory(CoAPMessage *message);

int CoAPMessage_send(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message);

int CoAPMessage_recv(CoAPContext *context, unsigned int timeout, int readcount);

int CoAPMessage_cycle(CoAPContext *context);

int CoAPMessage_cancel(CoAPContext *context, CoAPMessage *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
