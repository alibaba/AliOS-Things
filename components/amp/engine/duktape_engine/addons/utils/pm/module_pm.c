/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
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

static void __pm_pwrkey_state_notify(void *args)
{
    duk_context *ctx;
    int state = (int)args;
    ctx = be_get_context();
    be_push_ref(ctx, pm_pwrkey_cb_ref);
    duk_push_int(ctx, state);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
}

static void __pm_pwrkey_state_cb(int state)
{
    duk_context *ctx;
    amp_task_schedule_call(__pm_pwrkey_state_notify, state);
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

    if (aos_system_autosleep(mode) < 0) {
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

    if (aos_system_sleep()) {
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

    if (aos_wakelock_lock(w) < 0) {
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

    if (aos_wakelock_timedlock(w, msec) < 0) {
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

    if (aos_wakelock_unlock(w) < 0) {
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

    if (aos_pwrkey_notify_register(__pm_pwrkey_state_cb)) {
        amp_error(MOD_STR, "register event fail");
        goto out;
    }
    
    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pm_power_down(duk_context *ctx)
{
    duk_push_int(ctx, 0);
    aos_power_down();
    return 1;
}

static duk_ret_t native_pm_power_reset(duk_context *ctx)
{
    aos_power_reset();
    duk_push_int(ctx, 0);
    return 1;
}

void module_pm_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    pm_wakelock = aos_wakelock_create("PM.wakelock");
    if (!pm_wakelock) {
        amp_debug(MOD_STR, "wakelock unsupport");
    }

    pm_autosleep_mode = PM_AUTOSLEEP_MODE_NEVER;

    AMP_ADD_FUNCTION("setAutosleepMode", native_pm_autosleep_mode_set, 1);
    AMP_ADD_FUNCTION("getAutosleepMode", native_pm_autosleep_mode_get, 0);
    AMP_ADD_FUNCTION("sleep",            native_pm_sleep, 0);
	AMP_ADD_FUNCTION("powerDown",        native_pm_power_down, 0);
	AMP_ADD_FUNCTION("powerReset",       native_pm_power_reset, 0);
    AMP_ADD_FUNCTION("wakelockLock",      native_pm_wakelock_lock, 0);
    AMP_ADD_FUNCTION("wakelockUnlock",    native_pm_wakelock_unlock, 0);
    AMP_ADD_FUNCTION("wakelockTimedlock", native_pm_wakelock_timedlock, 1);
    AMP_ADD_FUNCTION("onPwrkey",          native_pm_pwrkey_on, 1);

    duk_put_prop_string(ctx, -2, "PM");
}

