/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_spi.h"
#include "aos_system.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "SPI"
#define SPI_TIMEOUT (0xFFFFFF)

static JSClassID js_spi_class_id;

static JSValue native_spi_open(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t spi_handle;
    spi_handle.handle = NULL;
    spi_dev_t *spi_device = NULL;

    const char *id = JS_ToCString(ctx, argv[0]);
    if (id == NULL) {
        amp_error(MOD_STR, "get spi id fail!");
        goto out;
    }

    ret = board_attach_item(MODULE_SPI, id, &spi_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!, id %s", id);
        goto out;
    }
    amp_debug(MOD_STR, "spi handle:%p\n", spi_handle.handle);
    spi_device = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_spi_init(spi_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_spi_init fail!");
        goto out;
    }
out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }
    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_SPI, &spi_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_spi_class_id);
        JS_SetOpaque(obj, (void *)spi_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_spi_close(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;

    spi_handle.handle = JS_GetOpaque2(ctx, this_val, js_spi_class_id);
    if (!spi_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    spi_device = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_spi_finalize(spi_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_spi_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_SPI, &spi_handle);
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_spi_write(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    char *data = NULL;
    uint32_t data_len = 0;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;

    spi_handle.handle = JS_GetOpaque2(ctx, this_val, js_spi_class_id);
    if (!spi_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    spi_device = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    data = JS_GetArrayBuffer(ctx, &data_len, argv[0]);
    if (!data) {
        amp_warn(MOD_STR, "parameter buffer is invalid, size: %d", data_len);
        goto out;
    }

    if (NULL == data) {
        amp_warn(MOD_STR, "JS_GetArrayBuffer failed");
        goto out;
    }

    ret = aos_hal_spi_send(spi_device, data, data_len, SPI_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "native_spi_write fail!");
        goto out;
    }

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_spi_read(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    const char *data = NULL;
    uint32_t len = 0;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;
    JSValue response = JS_UNDEFINED;

    spi_handle.handle = JS_GetOpaque2(ctx, this_val, js_spi_class_id);
    if (!spi_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    spi_device = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    JS_ToInt32(ctx, &len, argv[0]);

    data = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }

    ret = aos_hal_spi_recv(spi_device, data, len, SPI_TIMEOUT);

    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_spi_master_recv fail!");
        aos_free(data);
        goto out;
    }

    response = JS_NewArrayBufferCopy(ctx, data, len);
    aos_free(data);

out:
    return response;
}

static JSValue native_spi_send_receive(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    uint8_t *send_data = NULL;
    uint8_t *receive_data = NULL;
    uint32_t send_len = 0;
    uint32_t receive_len = 0;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;
    JSValue response = JS_UNDEFINED;

    spi_handle.handle = JS_GetOpaque2(ctx, this_val, js_spi_class_id);
    if (!spi_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    spi_device = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    send_data = JS_GetArrayBuffer(ctx, &send_len, argv[0]);
    JS_ToInt32(ctx, &receive_len, argv[1]);
    receive_data = (uint8_t *)aos_malloc(sizeof(uint8_t) * receive_len);
    if (NULL == receive_data) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }

    ret = aos_hal_spi_send_recv(spi_device, send_data, receive_data, receive_len, SPI_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "amp_hal_spi_master_recv fail!");
        aos_free(receive_data);
        goto out;
    }

    response = JS_NewArrayBufferCopy(ctx, receive_data, receive_len);
    aos_free(receive_data);

out:
    return response;
}

static JSClassDef js_spi_class = {
    "SPI",
};

static const JSCFunctionListEntry js_spi_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_spi_open),
    JS_CFUNC_DEF("read", 2, native_spi_read),
    JS_CFUNC_DEF("write", 1, native_spi_write),
    JS_CFUNC_DEF("sendRecv", 2, native_spi_send_receive),
    JS_CFUNC_DEF("close", 0, native_spi_close)};

static int js_spi_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_spi_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_spi_class_id, &js_spi_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_spi_funcs,
                               countof(js_spi_funcs));
    JS_SetClassProto(ctx, js_spi_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_spi_funcs,
                                  countof(js_spi_funcs));
}

JSModuleDef *js_init_module_spi(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_spi_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_spi_funcs, countof(js_spi_funcs));
    return m;
}

void module_spi_register(void)
{
    amp_debug(MOD_STR, "module_spi_register");
    JSContext *ctx = js_get_context();
    aos_printf("module spi register\n");
    js_init_module_spi(ctx, "SPI");
}
