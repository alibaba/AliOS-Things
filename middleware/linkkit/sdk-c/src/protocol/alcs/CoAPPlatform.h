/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __COAP_PLATFORM_OS_H__
#define __COAP_PLATFORM_OS_H__
#include <stdio.h>
#ifdef COAP_USE_PLATFORM_MEMORY
#include "iotx_utils.h"
#endif
#include "iotx_log.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if 0

#ifdef COAP_USE_PLATFORM_MEMORY
#define coap_malloc(size) LITE_malloc(size, MEM_MAGIC, "CoAP")
#else
#define coap_malloc malloc
#endif

#ifdef COAP_USE_PLATFORM_MEMORY
#define coap_free LITE_free
#else
#define coap_free free
#endif

#ifdef COAP_USE_PLATFORM_LOG
#define COAP_TRC   log_debug
#define COAP_DUMP  log_debug
#define COAP_DEBUG log_debug
#define COAP_INFO  log_info
#define COAP_WARN  log_warning
#define COAP_ERR   log_err
#else
#define coap_log(level, ...) \
    {\
    fprintf(stderr, "\r\n[%s LINE #%d]   ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);\
    }

#define coap_dump(...)\
{\
    fprintf(stderr, __VA_ARGS__);\
}

#define COAP_DUMP(fmt, args, ...)  coap_log("<DUMP>",  fmt, ##__VA_ARGS__)
#define COAP_TRC(fmt,  args, ...)  coap_log("<TRACE>", fmt, ##__VA_ARGS__)
#define COAP_DEBUG(fmt,args, ...)  coap_log("<DEBUG>", fmt, ##__VA_ARGS__)
#define COAP_INFO(fmt, args, ...)  coap_log("<INFO> ", fmt, ##__VA_ARGS__)
#define COAP_ERR(fmt,  args, ...)  coap_log("<ERROR>", fmt, ##__VA_ARGS__)
#endif
#endif

#define COAP_TRC(...)     log_debug("alcs", __VA_ARGS__)
#define COAP_DUMP(...)    log_debug("alcs", __VA_ARGS__)
#define COAP_DEBUG(...)   log_debug("alcs", __VA_ARGS__)
#define COAP_INFO(...)    log_info("alcs", __VA_ARGS__)
#define COAP_WRN(...)     log_warning("alcs", __VA_ARGS__)
#define COAP_ERR(...)     log_err("alcs", __VA_ARGS__)

int platform_is_multicast(const char *ip_str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
