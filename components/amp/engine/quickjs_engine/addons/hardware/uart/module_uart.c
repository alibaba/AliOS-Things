/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0
* generate:
*   ./qjsc -c -m src/uart.js -N jslib_uart -M UART -M events -o bytecode/jslib_uart.c
* linux sim:
*   ./test/qjsc -c ./test/testapp.js -M uart -M events -o ./test/app.c
*   python ./test/gen_jsb.py ./test/app.c ./test/app.jsb
*/

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "aos_hal_uart.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "MODULE_UART"
#define UART_BUFF_SIZE 1024
#define MAX_UART_PORT 6

static JSClassID js_uart_class_id;

typedef struct uart_module {
    uart_dev_t *uart;
    uint32_t item_handle;
    uint32_t recv_index;
    uint8_t recv_buff[UART_BUFF_SIZE];
    int js_cb_ref;
} uart_module_t;

typedef struct {
    uint8_t recv_buff[UART_BUFF_SIZE];
    int recv_len;
    JSValue js_cb_ref;
} uart_notify_t;

extern JSContext *js_get_context(void);

/*
 * open -- item_handle
 * on -- uart_module_t -- item_handle
 */
static uart_module_t *g_uart_modules[MAX_UART_PORT];

static uart_dev_t *g_uart_devices[MAX_UART_PORT];

static int uart_module_is_on(uint32_t item_handle)
{
    int i;
    for (i = 0; i < MAX_UART_PORT; i++) {
        uart_module_t *m = g_uart_modules[i];
        if (m && m->item_handle == item_handle) return 1;
    }
    return 0;
}

static void uart_recv_notify(void *pdata)
{
    int i = 0;
    uart_notify_t *notify = (uart_notify_t *)pdata;

    JSContext *ctx = js_get_context();

    JSValue val = JS_Call(ctx, notify->js_cb_ref, JS_UNDEFINED, notify->recv_len, (JSValueConst *)(notify->recv_buff));
    JS_FreeValue(ctx, val);

    aos_free(notify);
}

void uart_recv_callback(int port, void *data, uint16_t len, void *arg)
{
    uart_module_t *module = g_uart_modules[port];
    uint32_t recvsize = 0;
    uart_dev_t *dev = (uart_dev_t *)module->uart;
    uart_notify_t *notify;

    if (!module)
        return;

    notify = aos_malloc(sizeof(uart_notify_t));
    if (!notify) {
        amp_error(MOD_STR, "alloc uart notify fail");
        return;
    }

    if (data != NULL) {
        recvsize = len <= UART_BUFF_SIZE ? len : UART_BUFF_SIZE;
        memcpy(notify->recv_buff, data, recvsize);
    } else {
        aos_hal_uart_recv_II(dev, notify->recv_buff, UART_BUFF_SIZE, &recvsize, 0);
        if (recvsize <= 0) {
            aos_free(notify);
            return;
        }
    }
    notify->recv_len = recvsize;
    notify->js_cb_ref = module->js_cb_ref;
    amp_task_schedule_call(uart_recv_notify, notify);
}

static int uart_add_recv(uart_dev_t *uart, uint32_t item_handle, int js_cb_ref)
{
    uart_module_t *module = aos_calloc(1, sizeof(uart_module_t));
    if (!module) {
        amp_error(MOD_STR, "uart_start_recv fail!");
        return -1;
    }

    module->js_cb_ref   = js_cb_ref;
    module->item_handle = item_handle;
    module->recv_index  = 0;
    module->uart        = uart;

    if (aos_hal_uart_callback(uart, uart_recv_callback, module) != 0) {
        JSContext *ctx = js_get_context();
        JS_FreeValue(ctx, module->js_cb_ref);
        aos_free(module);
        return -1;
    }

    g_uart_modules[uart->port] = module;
    return 0;
}

static void uart_del_recv(uint32_t item_handle)
{
    int i;
    for (i = 0; i < MAX_UART_PORT; i++) {
        uart_module_t *m = g_uart_modules[i];
        if (m && m->item_handle == item_handle) {
            JSContext *ctx = js_get_context();
            JS_FreeValue(ctx, m->js_cb_ref);
            aos_free(m);
            g_uart_modules[i] = NULL;
            break;
        }
    }
}

static uint16_t uart_init_flag = 0;
static JSValue native_uart_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    amp_warn(MOD_STR, "native uart open");
    int8_t ret = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = NULL;
    uart_dev_t *uart_device = NULL;
    const char *id;

    if((argc < 1) || (!JS_IsString(argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }

    id = JS_ToCString(ctx, argv[0]);

    ret = board_attach_item(MODULE_UART, id, &uart_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!");
        goto out;
    }
    amp_debug(MOD_STR, "uart handle:%u\n", uart_handle.handle);
    uart_device = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    if (uart_init_flag & (1 << uart_device->port)) {
        amp_debug(MOD_STR, "uart port [%d] is already inited\n", uart_device->port);
        goto out;
    }

    // amp_info(MOD_STR, "uart uart_device->config.baud_rate:%d\n", uart_device->config.baud_rate);
    // amp_info(MOD_STR, "uart uart_device->config.parity:%d\n", uart_device->config.parity);
    // amp_info(MOD_STR, "uart uart_device->config.data_width:%d\n", uart_device->config.data_width);
    // amp_info(MOD_STR, "uart uart_device->config.stop_bits:%d\n", uart_device->config.stop_bits);
    // amp_info(MOD_STR, "uart uart_device->port:%d\n", uart_device->port);

    ret = aos_hal_uart_init(uart_device);
    if (0 != ret) {
        amp_error(MOD_STR, "native_uart_open fail!");
        goto out;
    }
    uart_init_flag |= (1 << uart_device->port);
    g_uart_devices[uart_device->port] = uart_device;
out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }
    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_UART, &uart_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_uart_class_id);
        JS_SetOpaque(obj, (void *)uart_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, 0);
}

static JSValue native_uart_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t uart_handle;
    uart_dev_t *uart_device = NULL;

    uart_handle.handle = JS_GetOpaque2(ctx, this_val, js_uart_class_id);
    if (!uart_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    uart_device = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_uart_finalize(uart_device);
    if (0 != ret) {
        amp_error(MOD_STR, "native_uart_close fail!");
        goto out;
    }
    uart_init_flag -= (1 << uart_device->port);
    g_uart_devices[uart_device->port] = NULL;
    board_disattach_item(MODULE_UART, &uart_handle);
    uart_del_recv((uint32_t)(uart_handle.handle));
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_uart_write(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;
    int i = 0;
    char *msg = NULL;
    size_t msg_len = 0;
    item_handle_t uart_handle;
    uart_dev_t *uart_device = NULL;
    JSValue val;
    const char *buf;

    if(argc < 1) {
        amp_warn(MOD_STR, "parameter must be array");
        goto out;
    }

    if(JS_IsString(argv[0])) {
        buf = JS_ToCString(ctx, argv[0]);
        msg_len = strlen(buf);
        i = 1;
    } else {
        buf = JS_GetArrayBuffer(ctx, &msg_len, argv[0]);
        if(!buf)
        {
            amp_warn(MOD_STR, "parameter buffer is invalid, size: %d", msg_len);
            goto out;
        }
    }

    uart_handle.handle = JS_GetOpaque2(ctx, this_val, js_uart_class_id);
    if (!uart_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    uart_device = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    msg = (char *)aos_malloc(msg_len + 1);
    if (!msg) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    memcpy(msg, buf, msg_len);

    msg[msg_len] = 0;

    //amp_info(MOD_STR, "uart write:%s\n", msg);
    ret  = aos_hal_uart_send(uart_device, msg, msg_len, osWaitForever);
    if (-1 == ret) {
        amp_error(MOD_STR, "native_uart_write fail!");
    }
    aos_free(msg);
out:
    if (i == 1) {
        JS_FreeCString(ctx, buf);
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_uart_read(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret          = -1;
    char *data = NULL;
    uint32_t max_len = 16;
    uint32_t recvsize = 0;
    item_handle_t uart_handle;
    uart_dev_t *uart_device = NULL;
    JSValue obj;

    uart_handle.handle = JS_GetOpaque2(ctx, this_val, js_uart_class_id);
    if (!uart_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    uart_device  = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    data = (uint8_t *)aos_malloc(sizeof(uint8_t) * max_len);
    if (NULL == data) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }

    ret = aos_hal_uart_recv_II(uart_device, data, 1, &recvsize, 0);
    if (recvsize <= 0) {
        //amp_error(MOD_STR, "native_uart_read error! ret = %d, recvsize= %d", ret, recvsize);
        goto out;
    }
    if (ret >= 0) {
        // amp_info(MOD_STR, "uart recvsize:%d\n", recvsize);
        obj = JS_NewStringLen(ctx, data, recvsize);
        if(NULL != data)
        {
            aos_free((void *)data);
        }
        return obj;
    }
out:
    if(NULL != data)
    {
        aos_free(data);
    }

    return JS_NewInt32(ctx, 0);
}

static JSValue native_uart_on(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int ret = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = NULL;
    uart_dev_t *uart_device = NULL;
    uint8_t *start          = NULL;
    uint8_t *end            = NULL;

    if((argc < 1) || (JS_IsNull(argv[0]) || JS_IsUndefined(argv[0])))
    {
        amp_warn(MOD_STR, "parameter is invalid, argc = %d\n", argc);
        goto out;
    }

    uart_handle.handle = JS_GetOpaque2(ctx, this_val, js_uart_class_id);
    if (!uart_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    uart_device = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    if (uart_module_is_on((uint32_t)(uart_handle.handle))) {
        amp_error(MOD_STR, "The uart module has set the listener");
        goto out;
    }

    ret = uart_add_recv(uart_device, (uint32_t)(uart_handle.handle), JS_DupValue(ctx, argv[0]));
    if (ret < 0) {
        amp_error(MOD_STR, "uart_add_recv fail!");
    }
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_uart_clean(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    uart_module_t *mod;
    uart_dev_t *dev;
    int i, ret;

    for (i = 0; i < MAX_UART_PORT; i++) {
        mod = g_uart_modules[i];
        if (mod) {
            JS_FreeValue(ctx, mod->js_cb_ref);
            aos_free(mod);
            g_uart_modules[i] = NULL;
        }

        dev = g_uart_devices[i];
        if (dev) {
            ret = aos_hal_uart_finalize(dev);
            if (ret) {
                amp_error(MOD_STR, "finalize uart %d fail %d", i, ret);
                continue;
            }
            uart_init_flag -= (1 << dev->port);
            g_uart_devices[i] = NULL;
        }
    }
}

static JSClassDef js_uart_class = {
    "UART",
};

static const JSCFunctionListEntry js_uart_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_uart_open),
    JS_CFUNC_DEF("read", 0, native_uart_read ),
    JS_CFUNC_DEF("write", 1, native_uart_write ),
    JS_CFUNC_DEF("on", 1, native_uart_on ),
    JS_CFUNC_DEF("close", 0, native_uart_close ),
};

static int js_uart_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_uart_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_uart_class_id, &js_uart_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_uart_funcs,
                               countof(js_uart_funcs));
    JS_SetClassProto(ctx, js_uart_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_uart_funcs,
                                  countof(js_uart_funcs));
}

JSModuleDef *js_init_module_uart(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_uart_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_uart_funcs, countof(js_uart_funcs));
    return m;
}

void module_uart_register(void)
{
    amp_debug(MOD_STR, "module_uart_register");
    JSContext *ctx = js_get_context();

    amp_module_free_register(native_uart_clean);

    js_init_module_uart(ctx, "UART");
}
