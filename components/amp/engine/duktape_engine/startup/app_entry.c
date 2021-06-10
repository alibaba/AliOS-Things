/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos_fs.h"
#include "aos_system.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "cJSON.h"
#include "startup/app_entry.h"

#define MOD_STR "APPENTRY"

app_options_t app_options;

/* App(Object options) entry */
static duk_ret_t native_app_entry(duk_context *ctx)
{
    int i;

    /* check paramters */
    if (!duk_is_object(ctx, 0))
    {
        // amp_warn("parameter must be object\n");
        duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "parameter must be object");
        return duk_throw(ctx);
    }

    memset(&app_options, 0, sizeof(app_options_t));

    /* get options object */
    duk_dup(ctx, -1);
    app_options.object = be_ref(ctx);

    /* find globalData */
    if (duk_get_prop_string(ctx, 0, "globalData"))
    {
        if (!duk_is_object(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "onLaunch must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find globalData\n");
        duk_dup(ctx, -1);
        app_options.global_data = be_ref(ctx);
        duk_pop(ctx);
    }

    /* find onLaunch() */
    if (duk_get_prop_string(ctx, 0, "onLaunch"))
    {
        if (!duk_is_function(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "onLaunch must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find onLaunch()\n");
        duk_dup(ctx, -1);
        app_options.on_launch = be_ref(ctx);
        duk_pop(ctx);
    }

    /* find onError() */
    if (duk_get_prop_string(ctx, 0, "onError"))
    {
        if (!duk_is_function(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "onError must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find onError()\n");
        duk_dup(ctx, -1);
        app_options.on_error = be_ref(ctx);
        duk_pop(ctx);
    }

    /* find onError() */
    if (duk_get_prop_string(ctx, 0, "onExit"))
    {
        if (!duk_is_function(ctx, -1))
        {
            duk_pop(ctx);
            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "onExit must be function");
            return duk_throw(ctx);
        }
        amp_debug(MOD_STR, "find onExit()\n");
        duk_dup(ctx, -1);
        app_options.on_exit = be_ref(ctx);
        duk_pop(ctx);
    }

    amp_task_schedule_call(app_entry, NULL);
    return 1; /* one return value */
}

void app_entry(void *data)
{
    int i = 0;

    duk_context *ctx = be_get_context();
    /* onLaunch hook */
    be_push_ref(ctx, app_options.on_launch);

    duk_push_object(ctx);

    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
    /* onExit hook */
    // be_push_ref(ctx, app_options.on_exit);

    // duk_push_object(ctx);

    // duk_pcall(ctx, 1);
    // duk_pop(ctx);
}

void app_entry_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    /* App({}) */
    duk_push_c_function(ctx, native_app_entry, 1);
    duk_put_global_string(ctx, "App");
}
