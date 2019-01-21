/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include "Cloud_CoAPExport.h"
#ifndef __COAP_SERIALIZE_H__
#define __COAP_SERIALIZE_H__

unsigned short Cloud_CoAPSerialize_MessageLength(Cloud_CoAPMessage *msg);

int Cloud_CoAPSerialize_Message(Cloud_CoAPMessage *msg, unsigned char *buf, unsigned short buflen);

#endif

