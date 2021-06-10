/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "aos_hal_uart.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "MODULE_UART"
#define UART_BUFF_SIZE 1024
#define MAX_UART_PORT 6

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
    int js_cb_ref;
} uart_notify_t;

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

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, notify->js_cb_ref);
    int arr_idx = duk_push_array(ctx);
    for (i = 0; i < notify->recv_len; i++) {
        duk_push_int(ctx, notify->recv_buff[i]);
        duk_put_prop_index(ctx, arr_idx, i);
    }
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
    // be_unref(ctx, p->js_cb_ref);
    // g_uart_modules[p->uart->port] = NULL;
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
        duk_context *ctx = be_get_context();
        be_unref(ctx, module->js_cb_ref);
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
            duk_context *ctx = be_get_context();
            be_unref(ctx, m->js_cb_ref);
            aos_free(m);
            g_uart_modules[i] = NULL;
            break;
        }
    }
}

static uint16_t uart_init_flag = 0;
static duk_ret_t native_uart_open(duk_context *ctx)
{
    amp_warn(MOD_STR, "native uart open");
    int8_t ret = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = NULL;
    uart_dev_t *uart_device = NULL;
    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_UART, id, &uart_handle);
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
    ret = aos_hal_uart_init(uart_device);
    if (0 != ret) {
        amp_error(MOD_STR, "native_uart_open fail!");
    }
    uart_init_flag |= (1 << uart_device->port);
    g_uart_devices[uart_device->port] = uart_device;
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_UART, &uart_handle);
    } else {
        duk_push_pointer(ctx, (void *)uart_handle.handle);
    }
    return 1;
}

static duk_ret_t native_uart_close(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t uart_handle;
    uart_dev_t *uart_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    uart_handle.handle = duk_get_pointer(ctx, 0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
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
    uart_del_recv(uart_handle.handle);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_uart_write(duk_context *ctx)
{
    int ret          = -1;
    int i;
    char *msg = NULL;
    uint32_t msg_len     = 0;
    item_handle_t uart_handle;
    uart_dev_t *uart_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle and array");
        goto out;
    }
    uart_handle.handle = duk_get_pointer(ctx, 0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    msg_len = duk_get_length(ctx, 1);
    msg     = (char *)aos_malloc(msg_len + 1);
    if (!msg) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    for (i = 0; i < msg_len; i++) {
        duk_get_prop_index(ctx, 1, i);
        msg[i] = duk_get_int(ctx, -1);
        duk_pop(ctx);
    }
    msg[msg_len] = 0;

    ret  = aos_hal_uart_send(uart_device, msg, msg_len, osWaitForever);
    if (-1 == ret) {
        amp_error(MOD_STR, "native_uart_write fail!");
    }
    aos_free(msg);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_uart_read(duk_context *ctx)
{
    int ret          = -1;
    const char *data = NULL;
    uint32_t max_len;
    uint32_t recvsize = 0;
    item_handle_t uart_handle;
    uart_dev_t *uart_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    uart_handle.handle = duk_get_pointer(ctx, 0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    ret = aos_hal_uart_recv_II(uart_device, data, max_len, &recvsize, 0);
    if (recvsize <= 0) {
        amp_error(MOD_STR, "native_uart_read error!");
        goto out;
    }

out:
    if (ret != 0) {
        duk_push_string(ctx, data);
    } else {
        duk_push_null(ctx);
    }
    aos_free(data);
    return 1;
}

static duk_ret_t native_uart_on(duk_context *ctx)
{
    int ret = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = NULL;
    uart_dev_t *uart_device = NULL;
    uint8_t *start          = NULL;
    uint8_t *end            = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle, function");
        goto out;
    }
    uart_handle.handle = duk_get_pointer(ctx, 0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    if (uart_module_is_on(uart_handle.handle)) {
        amp_error(MOD_STR, "The uart module has set the listener");
        goto out;
    }

    duk_dup(ctx, 1);
    int js_cb_ref = be_ref(ctx);
    ret = uart_add_recv(uart_device, uart_handle.handle, js_cb_ref);
    if (ret < 0) {
        amp_error(MOD_STR, "uart_add_recv fail!");
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static void native_uart_clean(void)
{
    duk_context *ctx = be_get_context();
    uart_module_t *mod;
    uart_dev_t *dev;
    int i, ret;

    for (i = 0; i < MAX_UART_PORT; i++) {
        mod = g_uart_modules[i];
        if (mod) {
            be_unref(ctx, mod->js_cb_ref);
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

void module_uart_register(void)
{
    amp_debug(MOD_STR, "module_uart_register\r");
    duk_context *ctx = be_get_context();

    amp_module_free_register(native_uart_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("open",  native_uart_open, 1);
    AMP_ADD_FUNCTION("read",  native_uart_read, 1);
    AMP_ADD_FUNCTION("write", native_uart_write, 2);
    AMP_ADD_FUNCTION("on",    native_uart_on, 2);
    AMP_ADD_FUNCTION("close", native_uart_close, 1);

    duk_put_prop_string(ctx, -2, "UART");
}
