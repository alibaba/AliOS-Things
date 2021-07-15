/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
#include "iotx_dm_internal.h"

static dm_opt_ctx g_dm_opt = {
    0, 0, 1
};

int dm_opt_set(dm_opt_t opt, void *data)
{
    int res = SUCCESS_RETURN;

    if (data == NULL) {
        return FAIL_RETURN;
    }

    switch (opt) {
        case DM_OPT_DOWNSTREAM_PROPERTY_POST_REPLY: {
            int opt = *(int *)(data);
            g_dm_opt.prop_post_reply_opt = opt;
        }
        break;
        case DM_OPT_DOWNSTREAM_EVENT_POST_REPLY: {
            int opt = *(int *)(data);
            g_dm_opt.event_post_reply_opt = opt;
        }
        break;
        case DM_OPT_UPSTREAM_PROPERTY_SET_REPLY: {
            int opt = *(int *)(data);
            g_dm_opt.prop_set_reply_opt = opt;
        }
        break;
        case DM_OPT_UPSTREAM_EVENT_NOTIFY_REPLY: {
            int opt = *(int *)(data);
            g_dm_opt.event_notify_reply_opt = opt;
        }
        break;
        default: {
            res = FAIL_RETURN;
        }
        break;
    }

    return res;
}

int dm_opt_get(dm_opt_t opt, void *data)
{
    int res = SUCCESS_RETURN;

    if (data == NULL) {
        return FAIL_RETURN;
    }

    switch (opt) {
        case DM_OPT_DOWNSTREAM_PROPERTY_POST_REPLY: {
            *(int *)(data) = g_dm_opt.prop_post_reply_opt;
        }
        break;
        case DM_OPT_DOWNSTREAM_EVENT_POST_REPLY: {
            *(int *)(data) = g_dm_opt.event_post_reply_opt;
        }
        break;
        case DM_OPT_UPSTREAM_PROPERTY_SET_REPLY: {
            *(int *)(data) = g_dm_opt.prop_set_reply_opt;
        }
        break;
        case DM_OPT_UPSTREAM_EVENT_NOTIFY_REPLY: {
            *(int *)(data) = g_dm_opt.event_notify_reply_opt;
        }
        break;
        default: {
            res = FAIL_RETURN;
        }
        break;
    }

    return res;
}
#endif