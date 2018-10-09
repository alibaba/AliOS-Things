/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_PLATFORM_OS_H__
#define __COAP_PLATFORM_OS_H__
#include <stdio.h>
#include "iot_import.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define coap_malloc(size)       HAL_Malloc(size)
#define coap_free(ptr)          HAL_Free(ptr)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
