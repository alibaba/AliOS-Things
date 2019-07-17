/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_jse_module.h"
#include "be_port_hal.h"

static wdg_dev_t g_wdg;
static be_jse_symbol_t *wdg_start(void) {
    be_jse_int_t ret      = -1;
    be_jse_symbol_t *arg0 = NULL;
    int32_t value_timeout = 0;
    wdg_dev_t *handle     = (wdg_dev_t *)&g_wdg;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    if (handle->config.timeout > 0) {
        goto out;
    }
    value_timeout = get_symbol_value_int(arg0);
    if (value_timeout < 0) {
        be_error("wdg", "get_symbol_value_int error!\n");
        goto out;
    }
    handle->config.timeout = value_timeout;
    ret                    = hal_wdg_init(handle);
    handle->config.timeout = (ret == 0) ? value_timeout : 0;

out:
    symbol_unlock(arg0);

    return new_int_symbol(ret);
}

static be_jse_symbol_t *wdg_feed(void) {
    wdg_dev_t *handle = (wdg_dev_t *)&g_wdg;

    be_jse_handle_function(0, 0, 0, 0, 0);
    hal_wdg_reload(handle);

    return new_symbol(BE_SYM_NULL);
}

static be_jse_symbol_t *wdg_stop(void) {
    be_jse_int_t ret  = 0;
    wdg_dev_t *handle = (wdg_dev_t *)&g_wdg;

    be_jse_handle_function(0, 0, 0, 0, 0);
    hal_wdg_finalize(handle);
    handle->config.timeout = 0;

    return new_symbol(BE_SYM_NULL);
}

static be_jse_symbol_t *wdg_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name) {
    if (0 == strcmp(name, "start")) {
        return wdg_start();
    }
    if (0 == strcmp(name, "feed")) {
        return wdg_feed();
    }
    if (0 == strcmp(name, "stop")) {
        return wdg_stop();
    }
    return (BE_JSE_FUNC_UNHANDLED);
}

void module_wdg_register() {
    memset(&g_wdg, 0x00, sizeof(g_wdg));
    be_jse_module_load("WDG", wdg_module_handle_cb);
}
