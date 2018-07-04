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


#include "CoAPExport.h"

#ifndef __COAP_HANDLE_MSG_H__
#define __COAP_HANDLE_MSG_H__

int CoAPStrOption_add(CoAPMessage *message, unsigned short optnum,
            unsigned char *data, unsigned short datalen);


int CoAPUintOption_add(CoAPMessage *message, unsigned short  optnum,
            unsigned int data);

unsigned short CoAPMessageId_gen(CoAPContext *context);

int CoAPMessageId_set(CoAPMessage *message, unsigned short msgid);

int CoAPMessageType_set(CoAPMessage *message, unsigned char type);

int CoAPMessageCode_set(CoAPMessage *message, CoAPMessageCode code);

int CoAPMessageToken_set(CoAPMessage *message, unsigned char *token,
        unsigned char tokenlen);

int CoAPMessageUserData_set(CoAPMessage *message, void *userdata);

int CoAPMessagePayload_set(CoAPMessage *message, unsigned char *payload,
        unsigned short payloadlen);

int CoAPMessageHandler_set(CoAPMessage *message, CoAPRespMsgHandler handler);

int CoAPMessage_init(CoAPMessage *message);

int CoAPMessage_destory(CoAPMessage *message);

int CoAPMessage_send(CoAPContext *context, CoAPMessage *message);

int CoAPMessage_recv(CoAPContext *context, unsigned int timeout, int readcount);

int CoAPMessage_cycle(CoAPContext *context);



#endif
