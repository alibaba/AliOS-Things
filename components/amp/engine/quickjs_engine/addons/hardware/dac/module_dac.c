/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_dac.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "DAC"

static JSClassID js_dac_class_id;

static JSValue native_dac_open(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t dac_handle;
    dac_handle.handle     = NULL;
    dac_dev_t *dac_device = NULL;

    const char *id = JS_ToCString(ctx, argv[0]);
    if (id == NULL) {
        amp_error(MOD_STR, "get dac id fail!");
        goto out;
    }

    ret = board_attach_item(MODULE_DAC, id, &dac_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!, id %s", id);
        goto out;
    }

    amp_debug(MOD_STR, "dac handle:%u\n", dac_handle.handle);
    dac_device = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = aos_hal_dac_init(dac_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_dac_init fail!\n");
        goto out;
    }
    ret = aos_hal_dac_start(dac_device, dac_device->port);
    if (0 != ret) {
        amp_error(MOD_STR, "hal_dac_start fail!\n");
    }

out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }
    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_DAC, &dac_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_dac_class_id);
        JS_SetOpaque(obj, (void *)dac_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_dac_setVol(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int8_t ret       = -1;
    int8_t result    = -1;
    uint32_t voltage = 0;
    item_handle_t dac_handle;
    dac_dev_t *dac_device = NULL;

    dac_handle.handle = JS_GetOpaque2(ctx, this_val, js_dac_class_id);
    if (!dac_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    dac_device = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    JS_ToInt32(ctx, &voltage, argv[0]);

    ret = aos_hal_dac_set_value(dac_device, dac_device->port, voltage);
    if (-1 == ret) {
        amp_error(MOD_STR, "dac set val fail!");
        goto out;
    }
    result = 0;
out:
    return JS_NewInt32(ctx, result);
}

static JSValue native_dac_getVol(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    uint32_t voltage = 0;
    item_handle_t dac_handle;
    dac_dev_t *dac_device = NULL;

    dac_handle.handle = JS_GetOpaque2(ctx, this_val, js_dac_class_id);
    if (!dac_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    dac_device = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    voltage = (uint32_t)aos_hal_dac_get_value(dac_device, dac_device->port);
out:
    JS_NewInt32(ctx, voltage);
}

static JSValue native_dac_close(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t dac_handle;
    dac_dev_t *dac_device = NULL;

    dac_handle.handle = JS_GetOpaque2(ctx, this_val, js_dac_class_id);
    if (!dac_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    dac_device = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    aos_hal_dac_stop(dac_device, dac_device->port);
    ret = aos_hal_dac_finalize(dac_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_dac_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_DAC, &dac_handle);

out:
    return JS_NewInt32(ctx, ret);
}

static JSClassDef js_dac_class = {
    "DAC",
};

static const JSCFunctionListEntry js_dac_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_dac_open),
    JS_CFUNC_DEF("getVol", 0, native_dac_getVol),
    JS_CFUNC_DEF("setVol", 1, native_dac_setVol),
    JS_CFUNC_DEF("close", 0, native_dac_close),
};

static int js_dac_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_dac_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_dac_class_id, &js_dac_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_dac_funcs,
                               countof(js_dac_funcs));
    JS_SetClassProto(ctx, js_dac_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_dac_funcs,
                                  countof(js_dac_funcs));
}

JSModuleDef *js_init_module_dac(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_dac_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_dac_funcs, countof(js_dac_funcs));
    return m;
}

void module_dac_register(void)
{
    amp_debug(MOD_STR, "module_dac_register");
    JSContext *ctx = js_get_context();
    aos_printf("module dac register\n");
    js_init_module_dac(ctx, "DAC");
}
