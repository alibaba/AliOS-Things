/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_PLATFORM_OS_H__
#define __COAP_PLATFORM_OS_H__

#include <stdio.h>

#include "iotx_coap_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef INFRA_MEM_STATS
#include "linkkit/infra/infra_mem_stats.h"
#define coap_malloc(size) LITE_malloc(size, MEM_MAGIC, "coap.local")
#define coap_free(ptr)    LITE_free(ptr)
#else
#define coap_malloc(size) HAL_Malloc(size)
#define coap_free(ptr)         \
    {                          \
        HAL_Free((void *)ptr); \
        ptr = NULL;            \
    }
#endif

#ifdef INFRA_LOG
#include "linkkit/infra/infra_log.h"
#define COAP_ERR(...)   log_err("coap_local", __VA_ARGS__)
#define COAP_WRN(...)   log_warning("coap_local", __VA_ARGS__)
#define COAP_INFO(...)  log_info("coap_local", __VA_ARGS__)
#define COAP_TRC(...)   log_debug("coap_local", __VA_ARGS__)
#define COAP_DUMP(...)  log_debug("coap_local", __VA_ARGS__)
#define COAP_DEBUG(...) log_debug("coap_local", __VA_ARGS__)
#define COAP_FLOW(...)  log_flow("coap_local", __VA_ARGS__)
#else
#define COAP_ERR(...)            \
    do {                         \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\r\n");      \
    } while (0)
#define COAP_WRN(...)            \
    do {                         \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\r\n");      \
    } while (0)
#define COAP_INFO(...)           \
    do {                         \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\r\n");      \
    } while (0)
#define COAP_TRC(...)            \
    do {                         \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\r\n");      \
    } while (0)
#define COAP_DUMP(...)           \
    do {                         \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\r\n");      \
    } while (0)
#define COAP_DEBUG(...)          \
    do {                         \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\r\n");      \
    } while (0)
#define COAP_FLOW(...)           \
    do {                         \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\r\n");      \
    } while (0)
#endif

int platform_is_multicast(const char *ip_str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
