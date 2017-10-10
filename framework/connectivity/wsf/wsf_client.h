/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_CLIENT_H
#define WSF_CLIENT_H
#include "wsf_list.h"
#include "wsf_defines.h"
#include "os.h"

typedef wsf_response_t *(*wsf_push_callback)(void *arg, int length);

typedef void (*wsf_error_callback)(wsf_code error_code);

wsf_code wsf_config(config_opt opt, void *value);

wsf_code wsf_init();

wsf_code wsf_shutdown();

/**
 * return NULL if wsf client not initialized or already shutdown
 */
wsf_response_t *wsf_invoke(const char *service_name, wsf_list_t *parameters,
                           int timeout);

wsf_code wsf_register_push_callback(wsf_push_callback callback);

void wsf_wait();

wsf_code wsf_response_destroy(wsf_response_t *response, int free_data);

wsf_code wsf_set_secret_key(const char *secret_key);

wsf_code wsf_register_error_callback(wsf_error_callback callback);

#endif
