/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_defines.h"
#include "amp_system.h"
#include "be_inl.h"


#define MOD_STR "Battery"

static duk_ret_t native_battery_conn_state_get(duk_context *ctx)
{
    int ret = -1;
	int state;

    if (HAL_Battery_Connect_State_Get(&state)) {
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

    if (HAL_Battery_Voltage_Get(&voltage)) {
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

    if (HAL_Battery_Level_Get(&level)) {
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

    if (HAL_Battery_Temperature_Get(&temperature)) {
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

    duk_push_c_function(ctx, native_battery_conn_state_get, 0);
    duk_put_prop_string(ctx, -2, "get_connect_state");

    duk_push_c_function(ctx, native_battery_voltage_get, 0);
    duk_put_prop_string(ctx, -2, "get_voltage");

    duk_push_c_function(ctx, native_battery_level_get, 0);
    duk_put_prop_string(ctx, -2, "get_level");

    duk_push_c_function(ctx, native_battery_temperature_get, 0);
    duk_put_prop_string(ctx, -2, "get_temperature");

    duk_put_prop_string(ctx, -2, "Battery");
}

