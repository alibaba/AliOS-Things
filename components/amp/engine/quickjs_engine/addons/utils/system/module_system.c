/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "amp_utils.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "SYSTEM"

extern const char *amp_jsapp_version_get(void);
static JSClassID js_system_class_id;

static JSValue native_module_versions(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv)
{
    JSValue t = JS_NewObject(ctx);
    const char *version = aos_get_system_version();

    JS_SetPropertyStr(ctx, t, "module", JS_NewStringLen(ctx, version, strlen(version)));
    return t;
}

static JSValue native_system_version(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    const char *version = aos_get_system_version();
    if (strlen(version) == 0)
        version = "null";

    return JS_NewStringLen(ctx, version, strlen(version));
}

static JSValue native_system_app_version(JSContext *ctx, JSValueConst this_val,
                                           int argc, JSValueConst *argv)
{
//    const char *version = amp_jsapp_version_get();  /* App mgr is currently not enabled */
    const char *version = "0.0.1";
    if (strlen(version) == 0)
        version = "null";
    return JS_NewStringLen(ctx, version, strlen(version));
}

static JSValue native_system_platform(JSContext *ctx, JSValueConst this_val,
                                        int argc, JSValueConst *argv)
{
    const char *p = aos_get_platform_type();
    if (strlen(p) == 0)
        p = "null";
    return JS_NewStringLen(ctx, p, strlen(p));
}

static JSValue native_system_uptime(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv)
{
    return JS_NewInt64(ctx, aos_now_ms());
}

static JSValue native_system_memory(JSContext *ctx, JSValueConst this_val,
                                      int argc, JSValueConst *argv)
{
    int32_t ret = -1;

    amp_heap_info_t heap_info;

    ret = amp_heap_memory_info(&heap_info);
    if (ret != 0) {
        amp_warn(MOD_STR, "get heap memory failed");
        goto out;
    }

    JSValue t = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, t, "heapTotal", JS_NewInt32(ctx, heap_info.heap_total));
    JS_SetPropertyStr(ctx, t, "heapUsed",  JS_NewInt32(ctx, heap_info.heap_used));
    JS_SetPropertyStr(ctx, t, "heapFree",  JS_NewInt32(ctx, heap_info.heap_free));

    amp_memmgt_mem_show_rec();

    return t;
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_system_gc(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    return JS_NewInt32(ctx, 1);
}

static JSClassDef js_system_class = {
    "SYSTEM",
};

static int js_system_init(JSContext *ctx)
{
    JSValue proto;

    JS_NewClassID(&js_system_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_system_class_id, &js_system_class);
    proto = JS_NewObject(ctx);
    JS_SetClassProto(ctx, js_system_class_id, proto);

    return 0;
}

void module_system_register(void)
{
    amp_debug(MOD_STR, "module_system_register");

    JSContext *ctx = js_get_context();

    aos_printf("module system register\n");

    js_system_init(ctx);

    JSValue global_obj, system;

    /* XXX: should these global definitions be enumerable? */
    global_obj = JS_GetGlobalObject(ctx);

    system = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, system, "versions",
                      JS_NewCFunction(ctx, native_module_versions, "verisons", 0));
    JS_SetPropertyStr(ctx, system, "version",
                      JS_NewCFunction(ctx, native_system_version, "verison", 0));
    JS_SetPropertyStr(ctx, system, "appversion",
                      JS_NewCFunction(ctx, native_system_app_version, "appverison", 0));
    JS_SetPropertyStr(ctx, system, "platform",
                      JS_NewCFunction(ctx, native_system_platform, "platform", 0));
    JS_SetPropertyStr(ctx, system, "uptime",
                      JS_NewCFunction(ctx, native_system_uptime, "uptime", 0));
    JS_SetPropertyStr(ctx, system, "memory",
                      JS_NewCFunction(ctx, native_system_memory, "memory", 0));
    JS_SetPropertyStr(ctx, system, "gc",
                      JS_NewCFunction(ctx, native_system_gc, "gc", 0));

    JS_SetPropertyStr(ctx, global_obj, "system", system);

    JS_FreeValue(ctx, global_obj);
}

