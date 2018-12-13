/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_PLATFORM_OS_H__
#define __COAP_PLATFORM_OS_H__
#include <stdio.h>
#include "iot_import.h"
#include "iotx_utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define coap_malloc(size)       LITE_malloc(size, MEM_MAGIC, "coap.cloud")
#define coap_free(ptr)          LITE_free(ptr)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
