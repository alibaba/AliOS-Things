/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifdef AMP_ADVANCED_ADDON_UI

#include "aos_fs.h"
#include "aos_system.h"
#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "cJSON.h"
#include "app_entry.h"
#include "quickjs_addon_common.h"

#define MOD_STR "APPENTRY"

app_options_t app_options;

static JSClassID js_app_entry_class_id;

/* App(Object options) entry */
static JSValue native_app_entry(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int i;

    JSValue irq_cb = argv[0];

    /* check paramters */
    if (!JS_IsObject(irq_cb))
    {
        return JS_ThrowTypeError(ctx, "not a object");
    }

    memset(&app_options, 0, sizeof(app_options_t));

    /* get options object */
    app_options.object = JS_DupValue(ctx, irq_cb);

    /* find globalData */
    if (JS_GetPropertyStr(ctx, argv[0], "globalData"))
    {
        amp_debug(MOD_STR, "find app#globalData()");
        app_options.global_data = JS_DupValue(ctx, irq_cb);
    }

    /* find onLaunch() */
    if (JS_GetPropertyStr(ctx, argv[0], "onLaunch"))
    {
        amp_debug(MOD_STR, "find app#onLaunch()");
        app_options.on_launch = JS_DupValue(ctx, irq_cb);
    }

    /* find onError() */
    if (JS_GetPropertyStr(ctx, argv[0], "onError"))
    {
        amp_debug(MOD_STR, "find app#onError()");
        app_options.on_error = JS_DupValue(ctx, irq_cb);
    }

    /* find onExit() */
    if (JS_GetPropertyStr(ctx, argv[0], "onExit"))
    {
        amp_debug(MOD_STR, "find app#onExit()");
        app_options.on_exit = JS_DupValue(ctx, irq_cb);
    }

    amp_task_schedule_call(app_entry, NULL);
    return 1; /* one return value */
}

void app_entry(void *data)
{
    JSContext *ctx = js_get_context();
    /* onLaunch hook */
    uint32_t value = 0;
    JSValue val = JS_Call(ctx, app_options.on_launch, JS_UNDEFINED, 1, &value);
    JS_FreeValue(ctx, val);
}


static JSClassDef js_app_entry_class = {
    "APPENTRY",
};

static int js_app_entry_init(JSContext *ctx)
{
    JSValue proto;

    JS_NewClassID(&js_app_entry_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_app_entry_class_id, &js_app_entry_class);
    proto = JS_NewObject(ctx);
    JS_SetClassProto(ctx, js_app_entry_class_id, proto);

    return;
}

void app_entry_register(void)
{
    amp_debug(MOD_STR, "module_app_entry_register");
    JSContext *ctx = js_get_context();
    aos_printf("module app_entry register\n");

    js_app_entry_init(ctx);

    QUICKJS_GLOBAL_FUNC("App", native_app_entry);
}

#endif