/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
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
