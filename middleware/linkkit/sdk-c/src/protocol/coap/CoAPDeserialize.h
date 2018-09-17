/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdio.h>
#include "CoAPExport.h"

#ifndef __COAP_DESERIALIZE_H__
#define __COAP_DESERIALIZE_H__

int CoAPDeserialize_Message(CoAPMessage *msg, unsigned char *buf, int buflen);

#endif
