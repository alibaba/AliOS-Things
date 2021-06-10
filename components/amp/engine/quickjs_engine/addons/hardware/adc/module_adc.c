/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_adc.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "ADC"

static JSClassID js_adc_class_id;

static JSValue native_adc_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t adc_handle;
    adc_handle.handle     = NULL;
    adc_dev_t *adc_device = NULL;

    const char *id = JS_ToCString(ctx, argv[0]);
    if (id == NULL) {
        amp_error(MOD_STR, "get adc id fail!");
        goto out;
    }

    ret = board_attach_item(MODULE_ADC, id, &adc_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!, id %s", id);
        goto out;
    }

    amp_debug(MOD_STR, "adc handle:%u\n", adc_handle.handle);
    adc_device = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_adc_init(adc_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_adc_init fail!");
        goto out;
    }

out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }
    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_ADC, &adc_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_adc_class_id);
        JS_SetOpaque(obj, (void *)adc_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_adc_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t adc_handle;
    adc_dev_t *adc_device = NULL;

    adc_handle.handle = JS_GetOpaque2(ctx, this_val, js_adc_class_id);
    if (!adc_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    adc_device = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_adc_finalize(adc_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_adc_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_ADC, &adc_handle);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_adc_read(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    uint32_t adc_value = 0;
    item_handle_t adc_handle;
    adc_dev_t *adc_device = NULL;

    adc_handle.handle = JS_GetOpaque2(ctx, this_val, js_adc_class_id);
    if (!adc_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    adc_device = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    (void)aos_hal_adc_value_get(adc_device, &adc_value, 0);
out:
    return JS_NewInt32(ctx, adc_value);
}

static JSClassDef js_adc_class = {
    "ADC",
};

static const JSCFunctionListEntry js_adc_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_adc_open),
    JS_CFUNC_DEF("read", 0, native_adc_read),
    JS_CFUNC_DEF("close", 0, native_adc_close),
};

static int js_adc_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_adc_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_adc_class_id, &js_adc_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_adc_funcs,
                               countof(js_adc_funcs));
    JS_SetClassProto(ctx, js_adc_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_adc_funcs,
                                  countof(js_adc_funcs));
}

JSModuleDef *js_init_module_adc(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_adc_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_adc_funcs, countof(js_adc_funcs));
    return m;
}

void module_adc_register(void)
{
    amp_debug(MOD_STR, "module_adc_register");
    JSContext *ctx = js_get_context();
    aos_printf("module adc register\n");
    js_init_module_adc(ctx, "ADC");
}
