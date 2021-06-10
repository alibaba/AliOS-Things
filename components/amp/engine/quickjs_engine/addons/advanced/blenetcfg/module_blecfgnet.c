/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR  "BLECFGNET"

static JSClassID js_blecfgnet_class_id;

static JSValue native_blecfgnet_start(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    ret = BleCfg_run();
    if (ret != 0) {
        amp_warn(MOD_STR, "ble config net start failed");
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_blecfgnet_recovery_wifi(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    ret = BleCfg_recovery_wifi();
    if (ret != 0) {
        amp_warn(MOD_STR, "ble config net recovery wifi failed");
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_blecfgnet_recovery_devinfo(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    ret = BleCfg_recovery_devinfo();
    if (ret != 0) {
        amp_warn(MOD_STR, "ble config net recovery device info failed");
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_blecfgnet_class = {
    "blecfgnet",
};

static const JSCFunctionListEntry js_blecfgnet_funcs[] = {
    JS_CFUNC_DEF("start",        0,   native_blecfgnet_start),
    JS_CFUNC_DEF("recoveryWifi", 0,   native_blecfgnet_recovery_wifi),
    JS_CFUNC_DEF("recoveryDevInfo", 0, native_blecfgnet_recovery_devinfo)
};

static int js_blecfgnet_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_blecfgnet_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_blecfgnet_class_id, &js_blecfgnet_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_blecfgnet_funcs, countof(js_blecfgnet_funcs));
    JS_SetClassProto(ctx, js_blecfgnet_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_blecfgnet_funcs, countof(js_blecfgnet_funcs));
}

JSModuleDef *js_init_module_blecfgnet(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_blecfgnet_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_blecfgnet_funcs, countof(js_blecfgnet_funcs));
    return m;
}

void module_blecfgnet_register(void)
{
    amp_debug(MOD_STR, "module_blecfgnet_register");
    JSContext *ctx = js_get_context();
    js_init_module_blecfgnet(ctx, "BLECFGNET");
}




