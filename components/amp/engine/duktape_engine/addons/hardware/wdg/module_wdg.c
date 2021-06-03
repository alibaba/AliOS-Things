/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_wdg.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "WDG"
static wdg_dev_t wdg_dev;

static duk_ret_t native_wdg_start(duk_context *ctx)
{
    int ret           = -1;
    int32_t timeout   = 0;
    wdg_dev_t *handle = (wdg_dev_t *)&wdg_dev;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number\n");
        goto out;
    }

    timeout = duk_get_int(ctx, 0);
    if (timeout < 0) {
        amp_error(MOD_STR, "invalid timeout: %d\n", timeout);
        goto out;
    }
    handle->config.timeout = timeout;
    ret                    = aos_hal_wdg_init(handle);
    handle->config.timeout = (ret == 0) ? timeout : 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_wdg_feed(duk_context *ctx)
{
    wdg_dev_t *handle = (wdg_dev_t *)&wdg_dev;
    aos_hal_wdg_reload(handle);
    return 0;
}

static duk_ret_t native_wdg_stop(duk_context *ctx)
{
    wdg_dev_t *handle = (wdg_dev_t *)&wdg_dev;
    aos_hal_wdg_finalize(handle);
    handle->config.timeout = 0;
    return 0;
}

void module_wdg_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("start", native_wdg_start, 1);
    AMP_ADD_FUNCTION("stop",  native_wdg_stop, 0);
    AMP_ADD_FUNCTION("feed",  native_wdg_feed, 0);

    duk_put_prop_string(ctx, -2, "WDG");
}
