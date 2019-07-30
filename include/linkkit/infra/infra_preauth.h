/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __INFRA_PREAUTH__
#define __INFRA_PREAUTH__

#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_types.h"

#ifdef INFRA_LOG
    #include "linkkit/infra/infra_log.h"
    #define preauth_err(...)   log_err("preauth", __VA_ARGS__)
    #define preauth_info(...)  log_info("preauth", __VA_ARGS__)
    #define preauth_debug(...) log_debug("preauth", __VA_ARGS__)
#else
    #define preauth_err(...)
    #define preauth_info(...)
    #define preauth_debug(...)
#endif

#define PRE_AUTH_HOSTNAME_MAXLEN  (64)
#define PRE_AUTH_CLIENT_ID_MAXLEN (200)
#define PRE_AUTH_USERNAME_MAXLEN  (64)
#define PRE_AUTH_PASSWORD_MAXLEN  (65)

typedef struct {
    char hostname[PRE_AUTH_HOSTNAME_MAXLEN];
    uint16_t port;
    char clientid[PRE_AUTH_CLIENT_ID_MAXLEN];
    char username[PRE_AUTH_USERNAME_MAXLEN];
    char password[PRE_AUTH_PASSWORD_MAXLEN];
} iotx_pre_auth_output_t;

int preauth_get_connection_info(iotx_mqtt_region_types_t region, iotx_dev_meta_info_t *dev_meta,
                                const char *sign, const char *device_id, iotx_pre_auth_output_t *preauth_output);

#endif /* #ifndef __INFRA_PREAUTH__ */