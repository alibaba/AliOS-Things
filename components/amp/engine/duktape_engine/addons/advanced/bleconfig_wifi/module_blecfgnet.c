/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "be_inl.h"

#define MOD_STR  "BLECFGNET"

static duk_ret_t native_blecfgnet_start(duk_context *ctx)
{
    int ret = -1;
    ret = BleCfg_run();
    if (ret != 0) {
        amp_warn(MOD_STR, "ble config net start failed");
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_blecfgnet_recovery_wifi(duk_context *ctx)
{
    int ret = -1;
    ret = BleCfg_recovery_wifi();
    if (ret != 0) {
        amp_warn(MOD_STR, "ble config net recovery wifi failed");
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_blecfgnet_recovery_devinfo(duk_context *ctx)
{
    int ret = -1;
    ret = BleCfg_recovery_devinfo();
    if (ret != 0) {
        amp_warn(MOD_STR, "ble config net recovery device info failed");
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_blecfgnet_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("start",           native_blecfgnet_start, 0);
    AMP_ADD_FUNCTION("recoveryWifi",    native_blecfgnet_recovery_wifi, 0);
    AMP_ADD_FUNCTION("recoveryDevInfo", native_blecfgnet_recovery_devinfo, 0);

    duk_put_prop_string(ctx, -2, "BLECFGNET");
}
