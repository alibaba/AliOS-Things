/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "aos/kv.h"
#include "amp_defines.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "KV"
#define KV_BUFFER_MAX_LEN   256

static JSClassID js_kv_class_id;

static JSValue native_kv_setStorage(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    int32_t value_len = 0;
    const char *key = NULL;
    const char *value = NULL;

    key = JS_ToCString(ctx, argv[0]);
    if (key == NULL) {
        amp_error(MOD_STR, "get kv key fail!");
        goto out;
    }

    value = JS_ToCString(ctx, argv[1]);
    if (value == NULL) {
        amp_error(MOD_STR, "get kv value fail!");
        goto out;
    }

    value_len = strlen(value);

    ret = aos_kv_set(key, value, value_len, 1);
    if (ret != 0) {
        amp_warn(MOD_STR, "kv set storage failed");
    }

out:
    if (key != NULL) {
        JS_FreeCString(ctx, key);
    }
    if (value != NULL) {
        JS_FreeCString(ctx, value);
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_kv_getStorage(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    int32_t value_len = KV_BUFFER_MAX_LEN;
    const char *key = NULL;
    char *value = NULL;
    JSValue obj = JS_UNDEFINED;

    key = JS_ToCString(ctx, argv[0]);
    if (key == NULL) {
        amp_error(MOD_STR, "get kv key fail!");
        goto out;
    }

    value = (char *)aos_malloc(KV_BUFFER_MAX_LEN);
    if (value == NULL) {
        amp_warn(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    ret = aos_kv_get(key, value, &value_len);
    if (ret != 0) {
        amp_warn(MOD_STR, "kv get storage failed");
        goto out;
    }

    obj = JS_NewStringLen(ctx, value, value_len);

out:
    if (key != NULL) {
        JS_FreeCString(ctx, key);
    }
    if (value != NULL) {
        aos_free(value);
    }
    return obj;
}

static JSValue native_kv_removeStorage(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    const char *key = NULL;

    key = JS_ToCString(ctx, argv[0]);
    if (key == NULL) {
        amp_error(MOD_STR, "get kv key fail!");
        goto out;
    }

    ret = aos_kv_del(key);
    if (ret != 0) {
        amp_warn(MOD_STR, "kv delete item failed");
    }

out:
    if (key != NULL) {
        JS_FreeCString(ctx, key);
    }
    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_kv_class = {
    "KV",
};

static const JSCFunctionListEntry js_kv_funcs[] = {
    JS_CFUNC_DEF("setStorageSync", 2, native_kv_setStorage),
    JS_CFUNC_DEF("getStorageSync", 1, native_kv_getStorage),
    JS_CFUNC_DEF("removeStorageSync", 1, native_kv_removeStorage),
};

static int js_kv_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_kv_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_kv_class_id, &js_kv_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_kv_funcs,
                               countof(js_kv_funcs));
    JS_SetClassProto(ctx, js_kv_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_kv_funcs,
                                  countof(js_kv_funcs));
}

JSModuleDef *js_init_module_kv(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_kv_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_kv_funcs, countof(js_kv_funcs));
    return m;
}

void module_kv_register(void)
{
    amp_debug(MOD_STR, "module_kv_register");
    JSContext *ctx = js_get_context();
    aos_printf("module kv register\n");
    js_init_module_kv(ctx, "kv");
}
