/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "be_inl.h"


#define MOD_STR "Battery"

static duk_ret_t native_battery_conn_state_get(duk_context *ctx)
{
    int ret = -1;
	int state;

    if (aos_battery_connect_state_get(&state)) {
        amp_error(MOD_STR, "get battery connect state fail");
        goto out;
    }

    ret = state;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_battery_voltage_get(duk_context *ctx)
{
    int ret = -1;
	int voltage;

    if (aos_battery_voltage_get(&voltage)) {
        amp_error(MOD_STR, "get battery voltage fail");
        goto out;
    }

    ret = voltage;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_battery_level_get(duk_context *ctx)
{
    int ret = -1;
	int level;

    if (aos_battery_level_get(&level)) {
        amp_error(MOD_STR, "get battery level fail");
        goto out;
    }

    ret = level;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_battery_temperature_get(duk_context *ctx)
{
    int ret = -1;
	int temperature;

    if (aos_battery_temperature_get(&temperature)) {
        amp_error(MOD_STR, "get battery temperature fail");
        goto out;
    }

    ret = temperature;
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_battery_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    AMP_ADD_FUNCTION("getConnectState", native_battery_conn_state_get, 0);
    AMP_ADD_FUNCTION("getVoltage",      native_battery_voltage_get, 0);
    AMP_ADD_FUNCTION("getLevel",        native_battery_level_get, 0);
    AMP_ADD_FUNCTION("getTemperature",  native_battery_temperature_get, 0);

    duk_put_prop_string(ctx, -2, "Battery");
}

