/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "jse_port.h"
#include "jse_task.h"
#include "bone_engine_inl.h"

static void next_tick_cb(void *arg)
{
    int ref          = (int)arg;
    duk_context *ctx = bone_engine_get_context();
    bone_engine_push_ref(ctx, ref);
    duk_pcall(ctx, 0);
    duk_pop(ctx);
    bone_engine_unref(ctx, ref);
}

static duk_ret_t native_process_nextTick(duk_context *ctx)
{
    if (!duk_is_function(ctx, -1)) {
        jse_warn("is not function\n");
        duk_push_string(ctx, "nextTick parameter is not function");
        return duk_throw(ctx);
    }
    duk_dup(ctx, -1);
    int ref = bone_engine_ref(ctx);
    if (be_jse_task_schedule_call(next_tick_cb, (void *)ref) < 0) {
        jse_warn("be_jse_task_schedule_call failed\n");
        bone_engine_unref(ctx, ref);
    }
    return 0;
}

static duk_ret_t native_process_getTime(duk_context *ctx)
{
    duk_push_number(ctx, jse_osal_get_clocktime());
    return 1;
}

static duk_ret_t native_process_uptime(duk_context *ctx)
{
    duk_push_number(ctx, jse_osal_get_clocktime());
    return 1;
}

void module_process_register(void)
{
    duk_context *ctx = bone_engine_get_context();
    duk_push_object(ctx);

    /* version */
    duk_push_string(ctx, "0.0.2");
    duk_put_prop_string(ctx, -2, "version");

    /* platform */
    duk_push_string(ctx, jse_system_get_platform_type());
    duk_put_prop_string(ctx, -2, "platform");

    /* nextTick(function () {}) */
    duk_push_c_function(ctx, native_process_nextTick, 1);
    duk_put_prop_string(ctx, -2, "nextTick");

    /* getTime() */
    duk_push_c_function(ctx, native_process_getTime, 0);
    duk_put_prop_string(ctx, -2, "getTime");

    /* uptime() */
    duk_push_c_function(ctx, native_process_uptime, 0);
    duk_put_prop_string(ctx, -2, "uptime");

    duk_put_global_string(ctx, "process");
}
