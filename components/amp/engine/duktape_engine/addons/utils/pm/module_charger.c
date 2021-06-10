/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "aos_pm.h"
#include "be_inl.h"


#define MOD_STR "Charger"


static duk_ret_t native_charger_conn_state_get(duk_context *ctx)
{
    int ret = -1;
	int state;

    if (aos_charger_connect_state_get(&state)) {
        amp_error(MOD_STR, "get charger connection state fail");
        goto out;
    }

    ret = state;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_charger_state_get(duk_context *ctx)
{
    int ret = -1;
    aos_charger_state_t state = AOS_CHARGER_STAT_SHUTDOWN;

    if (aos_charger_state_get(&state)) {
        amp_error(MOD_STR, "get charger connection state fail");
        goto out;
    }

    ret = state;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_charger_current_get(duk_context *ctx)
{
    int ret = -1;
	int current;

    if (aos_charger_current_get(&current)) {
        amp_error(MOD_STR, "get charger current fail");
        goto out;
    }

    ret = current;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_charger_switch_set(duk_context *ctx)
{
    int ret = -1;
    int val;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    val = duk_get_int(ctx, 0);
    if (val < 0) {
        amp_error(MOD_STR, "parameter %d invalid", val);
        goto out;
    }

    if (aos_charger_switch_set(val)) {
        amp_error(MOD_STR, "set charger switch fail");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_charger_connect_on(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_function(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be (function)");
        goto out;
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}


void module_charger_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    AMP_ADD_FUNCTION("getConnectState", native_charger_conn_state_get, 0);
    AMP_ADD_FUNCTION("getState",        native_charger_state_get, 0);
    AMP_ADD_FUNCTION("getCurrent",      native_charger_current_get, 0);
    AMP_ADD_FUNCTION("switch",          native_charger_switch_set, 1);
    AMP_ADD_FUNCTION("onConnect",       native_charger_connect_on, 1);

    duk_put_prop_string(ctx, -2, "Charger");
}

