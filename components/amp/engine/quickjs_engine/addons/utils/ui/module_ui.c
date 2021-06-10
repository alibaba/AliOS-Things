/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifdef AMP_ADVANCED_ADDON_UI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "aos_network.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "amp_task.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "UI"

static JSClassID js_ui_class_id;

char g_app_path[128];
extern void amp_page_changed_set(void);

JSValue native_ui_redirect(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    char *path;
    if (!JS_IsString(argv[0])) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto done;
    }

    path = JS_ToCString(ctx, argv[0]);
    if (path != NULL) {
        memset(&g_app_path[0], 0, sizeof(g_app_path));
        snprintf(&g_app_path[0],128,"%s",path);
        amp_page_changed_set();
    }

    amp_debug(MOD_STR, "native_ui_redirect success");

done:
    return 1;
}
/*
void module_ui_register(void)
{
    JSContext *ctx = js_get_context();
    duk_push_object(ctx);

    duk_push_c_function(ctx, native_ui_redirect, 1);
    duk_put_prop_string(ctx, -2, "redirectTo");

    duk_put_prop_string(ctx, -2, "UI");
}
*/

static JSClassDef js_vm_class = {
    "UI",
};

static const JSCFunctionListEntry js_ui_funcs[] = {
    JS_CFUNC_DEF("redirectTo", 1, native_ui_redirect ),
};

static int js_ui_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_ui_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_ui_class_id, &js_vm_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_ui_funcs,
                               countof(js_ui_funcs));
    JS_SetClassProto(ctx, js_ui_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_ui_funcs,
                                  countof(js_ui_funcs));
}

JSModuleDef *js_init_module_ui(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_ui_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_ui_funcs, countof(js_ui_funcs));
    return m;
}

void module_ui_register(void)
{
    amp_debug(MOD_STR, "module_ui_register");
    JSContext *ctx = js_get_context();
    aos_printf("module ui register\n");
    js_init_module_ui(ctx, "UI");
}

#endif