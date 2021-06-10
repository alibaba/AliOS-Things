/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_wdg.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "WDG"

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

static wdg_dev_t wdg_dev = {0};
static JSClassID js_wdg_class_id;

static JSValue native_wdg_start(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret           = -1;
    int32_t timeout   = 0;
    int8_t result   = -1;
    wdg_dev_t *handle = (wdg_dev_t *)&wdg_dev;

    if((argc < 1) || (0 != JS_ToInt32(ctx, &timeout, argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid\n");
        goto out;
    }

    handle->config.timeout = timeout;
    ret                    = aos_hal_wdg_init(handle);
    handle->config.timeout = (ret == 0) ? timeout : 0;

out:
    return JS_NewInt32(ctx, result);
}

static JSValue native_wdg_feed(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t result   = -1;
    wdg_dev_t *handle = (wdg_dev_t *)&wdg_dev;
    aos_hal_wdg_reload(handle);
    result = 0;

out:
    return JS_NewInt32(ctx, result);
}

static JSValue native_wdg_stop(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t result = -1;
    wdg_dev_t *handle = (wdg_dev_t *)&wdg_dev;
    aos_hal_wdg_finalize(handle);
    handle->config.timeout = 0;
    result = 0;

out:
    return JS_NewInt32(ctx, result);
}

static JSClassDef js_wdg_class = {
    "WDG",
};

static const JSCFunctionListEntry js_wdg_funcs[] = {
    JS_CFUNC_DEF("start", 1, native_wdg_start),
    JS_CFUNC_DEF("stop", 0, native_wdg_stop ),
    JS_CFUNC_DEF("feed", 0, native_wdg_feed ),
};

static int js_wdg_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_wdg_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_wdg_class_id, &js_wdg_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_wdg_funcs,
                               countof(js_wdg_funcs));
    JS_SetClassProto(ctx, js_wdg_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_wdg_funcs,
                                  countof(js_wdg_funcs));
}

JSModuleDef *js_init_module_wdg(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_wdg_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_wdg_funcs, countof(js_wdg_funcs));
    return m;
}

void module_wdg_register(void)
{
    amp_debug(MOD_STR, "module_wdg_register");
    JSContext *ctx = js_get_context();
    aos_printf("module wdg register\n");
    js_init_module_wdg(ctx, "WDG");
}
