/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_i2c.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"
#include "aos_system.h"

#define MOD_STR "I2C"

static JSClassID js_i2c_class_id;

#define I2C_TIMEOUT (0xFFFFFF)

static JSValue native_i2c_open(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t i2c_handle;
    i2c_handle.handle     = NULL;
    i2c_dev_t *i2c_device = NULL;

    const char *id = JS_ToCString(ctx, argv[0]);
    if (id == NULL) {
        amp_error(MOD_STR, "get i2c id fail!");
        goto out;
    }

    ret = board_attach_item(MODULE_I2C, id, &i2c_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!, id %s", id);
        goto out;
    }

    amp_debug(MOD_STR, "i2c handle:%u", i2c_handle.handle);
    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_i2c_init(i2c_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_i2c_init fail!");
        goto out;
    }
out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }
    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_I2C, &i2c_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_i2c_class_id);
        JS_SetOpaque(obj, (void *)i2c_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_i2c_close(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;

    i2c_handle.handle = JS_GetOpaque2(ctx, this_val, js_i2c_class_id);
    if (!i2c_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_i2c_finalize(i2c_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_i2c_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_I2C, &i2c_handle);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_i2c_write(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;

    i2c_handle.handle = JS_GetOpaque2(ctx, this_val, js_i2c_class_id);
    if (!i2c_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    data = JS_GetArrayBuffer(ctx, &len, argv[0]);
    if(!data) {
        amp_error(MOD_STR, "parameter buffer is invalid, size: %d", len);
        goto out;
    }

    if (NULL == data) {
        amp_error(MOD_STR, "JS_GetArrayBuffer fail!");
        goto out;
    }
    ret = aos_hal_i2c_master_send(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_i2c_master_send fail!");
        goto out;
    }
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_i2c_write_reg(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;
    uint32_t mem_addr;

    i2c_handle.handle = JS_GetOpaque2(ctx, this_val, js_i2c_class_id);
    if (!i2c_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    JS_ToInt32(ctx, &mem_addr, argv[0]);

    data = JS_GetArrayBuffer(ctx, &len, argv[1]);
    if (NULL == data) {
        amp_error(MOD_STR, "JS_GetArrayBuffer fail!");
        goto out;
    }

    ret = aos_hal_i2c_mem_write(i2c_device, i2c_device->config.dev_addr,
                                (uint16_t)mem_addr, 1, data, len, I2C_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_i2c_master_send fail!");
        goto out;
    }
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_i2c_read(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;
    JSValue response = JS_UNDEFINED;

    i2c_handle.handle = JS_GetOpaque2(ctx, this_val, js_i2c_class_id);
    if (!i2c_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    JS_ToInt32(ctx, &len, argv[0]);

    data = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }
    ret = aos_hal_i2c_master_recv(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_i2c_master_recv fail!");
        aos_free(data);
        goto out;
    }

    response = JS_NewArrayBufferCopy(ctx, data, len);
    aos_free(data);
out:
    return response;
}

static JSValue native_i2c_read_reg(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;
    uint32_t mem_addr;
    JSValue response = JS_UNDEFINED;

    i2c_handle.handle = JS_GetOpaque2(ctx, this_val, js_i2c_class_id);
    if (!i2c_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    JS_ToInt32(ctx, &mem_addr, argv[0]);
    JS_ToInt32(ctx, &len, argv[1]);

    data = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    ret = aos_hal_i2c_mem_read(i2c_device, i2c_device->config.dev_addr,
                                (uint16_t)mem_addr, 1, data, len, I2C_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_i2c_mem_read fail!");
        aos_free(data);
        goto out;
    }

    response = JS_NewArrayBufferCopy(ctx, data, len);
    aos_free(data);
out:
    return response;
}

static JSClassDef js_i2c_class = {
    "I2C",
};

static const JSCFunctionListEntry js_i2c_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_i2c_open),
    JS_CFUNC_DEF("read", 1, native_i2c_read),
    JS_CFUNC_DEF("write", 1, native_i2c_write),
    JS_CFUNC_DEF("readReg", 2, native_i2c_read_reg),
    JS_CFUNC_DEF("writeReg", 2, native_i2c_write_reg),
    JS_CFUNC_DEF("close", 0, native_i2c_close),
};

static int js_i2c_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_i2c_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_i2c_class_id, &js_i2c_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_i2c_funcs,
                               countof(js_i2c_funcs));
    JS_SetClassProto(ctx, js_i2c_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_i2c_funcs,
                                  countof(js_i2c_funcs));
}

JSModuleDef *js_init_module_i2c(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_i2c_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_i2c_funcs, countof(js_i2c_funcs));
    return m;
}

void module_i2c_register(void)
{
    amp_debug(MOD_STR, "module_i2c_register");
    JSContext *ctx = js_get_context();
    aos_printf("module i2c register\n");
    js_init_module_i2c(ctx, "I2C");
}
