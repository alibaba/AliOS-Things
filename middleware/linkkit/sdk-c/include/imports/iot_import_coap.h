/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __COAP_PLATFORM_H__
#define __COAP_PLATFORM_H__
#include "iot_import.h"

#define coap_malloc(size)       HAL_Malloc(size)
#define coap_free(ptr)          HAL_Free(ptr)

#endif
