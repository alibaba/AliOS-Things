/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "amp_task.h"
#include "aos/list.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "module_network"

static JSClassID js_network_class_id;

static JSValue native_network_get_type(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;

    ret = aos_get_network_type();

    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_network_class = {
    "NETWORK",
};

static const JSCFunctionListEntry js_network_funcs[] = {
    JS_CFUNC_DEF("getType", 0, native_network_get_type)
};

static int js_network_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_network_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_network_class_id, &js_network_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_network_funcs, countof(js_network_funcs));
    JS_SetClassProto(ctx, js_network_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_network_funcs, countof(js_network_funcs));
}

JSModuleDef *js_init_module_network(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_network_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_network_funcs, countof(js_network_funcs));
    return m;
}

void module_network_register(void)
{
    amp_debug(MOD_STR, "module_network_register");
    JSContext *ctx = js_get_context();

    js_init_module_network(ctx, "NETWORK");
}
