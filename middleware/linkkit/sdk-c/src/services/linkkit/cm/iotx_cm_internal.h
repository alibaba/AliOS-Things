/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef _IOTX_CM_INTERNAL_H_
#define _IOTX_CM_INTERNAL_H_
#include "iot_import.h"
#include "iotx_log.h"
#include "iotx_utils.h"

#define cm_malloc(size)     LITE_malloc(size, MEM_MAGIC, "cm")
#define cm_free(p)          LITE_free(p)

#ifdef ESP8266
    #include "esp_common.h"
    #define CM_READ_ONLY ICACHE_RODATA_ATTR STORE_ATTR
#else
    #define CM_READ_ONLY
#endif

#define CM_DEBUG(...)        log_debug("CM", __VA_ARGS__)
#define CM_INFO(...)         log_info("CM", __VA_ARGS__)
#define CM_WARN(...)         log_warning("CM", __VA_ARGS__)
#define CM_ERR(...)          log_err("CM", __VA_ARGS__)

typedef int (*iotx_cm_connect_fp)(uint32_t timeout);
typedef int (*iotx_cm_yield_fp)(unsigned int timeout);
typedef int (*iotx_cm_sub_fp)(iotx_cm_ext_params_t *params, const char *topic,
                              iotx_cm_data_handle_cb topic_handle_func, void *pcontext);
typedef int (*iotx_cm_unsub_fp)(const char *topic);
typedef int (*iotx_cm_pub_fp)(iotx_cm_ext_params_t *params, const char *topic, const char *payload,
                              unsigned int payload_len);
typedef int (*iotx_cm_close_fp)();


typedef struct iotx_connection_st {
    int                              fd;
    void                             *open_params;
    void                             *context;
    void                             *list_lock;
    iotx_cm_protocol_types_t         protocol_type;
    iotx_cm_connect_fp               connect_func;
    iotx_cm_sub_fp                   sub_func;
    iotx_cm_unsub_fp                 unsub_func;
    iotx_cm_pub_fp                   pub_func;
    iotx_cm_yield_fp                 yield_func;
    iotx_cm_close_fp                 close_func;
    iotx_cm_event_handle_cb          event_handler;
    void                             *cb_data;

} iotx_cm_connection_t;

extern const char ERR_INVALID_PARAMS[] CM_READ_ONLY;
#endif /* _LINKKIT_CM_H_ */
