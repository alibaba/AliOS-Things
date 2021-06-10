/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

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
#include "be_inl.h"

#define MOD_STR "MODEL"

static JSClassID js_vm_class_id;

JSValue native_model_setdata(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv);

/*void module_vm_register(void)
{
    JSContext *ctx = js_get_context();
    duk_push_object(ctx);

    duk_push_c_function(ctx, native_model_setdata, 1);
    duk_put_prop_string(ctx, -2, "setData");

    duk_put_prop_string(ctx, -2, "VM");
}*/


static JSClassDef js_vm_class = {
    "VM",
};

static const JSCFunctionListEntry js_vm_funcs[] = {
    JS_CFUNC_DEF("setData", 1, native_model_setdata ),
};

static int js_vm_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_vm_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_vm_class_id, &js_vm_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_vm_funcs,
                               countof(js_vm_funcs));
    JS_SetClassProto(ctx, js_vm_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_vm_funcs,
                                  countof(js_vm_funcs));
}

JSModuleDef *js_init_module_vm(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_vm_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_vm_funcs, countof(js_vm_funcs));
    return m;
}

void module_vm_register(void)
{
    amp_debug(MOD_STR, "module_vm_register");
    JSContext *ctx = js_get_context();
    aos_printf("module vm register\n");
    js_init_module_vm(ctx, "VM");
}

