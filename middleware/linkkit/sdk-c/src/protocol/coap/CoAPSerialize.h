/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include "CoAPExport.h"
#ifndef __COAP_SERIALIZE_H__
#define __COAP_SERIALIZE_H__

unsigned short CoAPSerialize_MessageLength(CoAPMessage *msg);

int CoAPSerialize_Message(CoAPMessage *msg, unsigned char *buf, unsigned short buflen);

#endif

