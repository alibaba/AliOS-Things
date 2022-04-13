/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef DEVICE_MODEL_ENABLED
#ifndef _DM_OPT_H
#define _DM_OPT_H

typedef enum {
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    DM_OPT_DOWNSTREAM_PROPERTY_POST_REPLY,
    DM_OPT_DOWNSTREAM_EVENT_POST_REPLY,
    DM_OPT_UPSTREAM_PROPERTY_SET_REPLY,
#endif
    DM_OPT_DOWNSTREAM_EVENT_PROPERTY_DESIRED_DELETE_REPLY,
    DM_OPT_DOWNSTREAM_EVENT_PROPERTY_DESIRED_GET_REPLY,
    DM_OPT_FOTA_RETRY_TIMEOUT_MS,
    DM_OPT_PROXY_PRODUCT_REGISTER
} dm_opt_t;

typedef struct {
    int prop_post_reply_opt;
    int event_post_reply_opt;
    int prop_set_reply_opt;
    int prop_desired_get_reply_opt;
    int prop_desired_delete_reply_opt;
    int fota_retry_timeout_ms;
    int proxy_product_register;
} dm_opt_ctx;

int dm_opt_set(dm_opt_t opt, void *data);
int dm_opt_get(dm_opt_t opt, void *data);

#endif
#endif
