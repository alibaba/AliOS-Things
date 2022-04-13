/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_defines.h"
#include "amp_system.h"
#include "be_inl.h"


#define MOD_STR "PM"

enum {
    PM_AUTOSLEEP_MODE_NEVER = 0,
    PM_AUTOSLEEP_MODE_SLEEP,
    PM_AUTOSLEEP_MODE_DEEP_SLEEP,
};

static void *pm_wakelock;
static int pm_autosleep_mode;
static int pm_sleep_mode;
static int pm_pwrkey_cb_ref;

static void __pm_pwrkey_state_cb(int state)
{
	duk_context *ctx;

    ctx = be_get_context();
    be_push_ref(ctx, pm_pwrkey_cb_ref);
    duk_push_int(ctx, state);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
}

static duk_ret_t native_pm_autosleep_mode_get(duk_context *ctx)
{
    duk_push_int(ctx, pm_autosleep_mode);
    return 1;
}

static duk_ret_t native_pm_autosleep_mode_set(duk_context *ctx)
{
    int ret = -1;
    int mode;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter not number");
        goto out;
    }

    mode = duk_get_int(ctx, 0);

    if (HAL_System_Autosleep(mode) < 0) {
        amp_error(MOD_STR, "set autosleep mode %d fail", mode);
        goto out;
    }
    pm_autosleep_mode = mode;
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pm_sleep(duk_context *ctx)
{
    int ret = -1;

    if (HAL_System_Sleep()) {
        amp_error(MOD_STR, "pm suspend fail");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pm_wakelock_lock(duk_context *ctx)
{
    int ret = -1;
    void *w = pm_wakelock;

    if (!w) {
        goto out;
    }

    if (HAL_Wakelock_Lock(w) < 0) {
        amp_error(MOD_STR, "wakelock lock fail");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pm_wakelock_timedlock(duk_context *ctx)
{
    int ret = -1;
    void *w = pm_wakelock;
    int msec;

    if (!w) {
        goto out;
    }

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    msec = duk_get_uint(ctx, 0);

    if (HAL_Wakelock_Timedlock(w, msec) < 0) {
        amp_error(MOD_STR, "wakelock timedlock fail");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pm_wakelock_unlock(duk_context *ctx)
{
    int ret = -1;
    void *w = pm_wakelock;

    if (!w) {
        goto out;
    }

    if (HAL_Wakelock_Unlock(w) < 0) {
        amp_error(MOD_STR, "wakelock unlock fail");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pm_pwrkey_on(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_function(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be (function)");
        goto out;
    }

    duk_dup(ctx, 0);
    pm_pwrkey_cb_ref = be_ref(ctx);

    if (HAL_Pwrkey_Notify_Register(__pm_pwrkey_state_cb)) {
        amp_error(MOD_STR, "register event fail");
        goto out;
    }
    
    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_pm_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    pm_wakelock = HAL_Wakelock_Create("PM.wakelock");
    if (!pm_wakelock) {
        amp_debug(MOD_STR, "wakelock unsupport");
    }

    pm_autosleep_mode = PM_AUTOSLEEP_MODE_NEVER;

    duk_push_c_function(ctx, native_pm_autosleep_mode_set, 1);
    duk_put_prop_string(ctx, -2, "set_autosleep_mode");

    duk_push_c_function(ctx, native_pm_autosleep_mode_get, 0);
    duk_put_prop_string(ctx, -2, "get_autosleep_mode");

    duk_push_c_function(ctx, native_pm_sleep, 0);
    duk_put_prop_string(ctx, -2, "sleep");

    duk_push_c_function(ctx, native_pm_wakelock_lock, 0);
    duk_put_prop_string(ctx, -2, "wakelock_lock");

    duk_push_c_function(ctx, native_pm_wakelock_unlock, 0);
    duk_put_prop_string(ctx, -2, "wakelock_unlock");

    duk_push_c_function(ctx, native_pm_wakelock_timedlock, 1);
    duk_put_prop_string(ctx, -2, "wakelock_timedlock");

    duk_push_c_function(ctx, native_pm_pwrkey_on, 1);
    duk_put_prop_string(ctx, -2, "on_pwrkey");

    duk_put_prop_string(ctx, -2, "PM");
}

