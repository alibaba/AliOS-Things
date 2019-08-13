/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>
#include "be_jse_task.h"
#include "hal/log.h"
#include "be_port_osal.h"
#include "board-mgr/board_mgr.h"
#include "bone_engine_inl.h"
#include "aos/hal/uart.h"

#define UART_BUFF_SIZE 256
#define MAX_UART_PORT 6

typedef struct uart_module {
    uart_dev_t *uart;
    uint32_t loop_time;
    uint32_t item_handle;
    uint8_t *start_flag;
    uint8_t *end_flag;
    uint32_t recv_index;
    uint8_t recv_buff[UART_BUFF_SIZE];
    int js_cb_ref;
    void *timer_id;
} uart_module_t;

/*
 * open -- item_handle
 * on -- uart_module_t -- item_handle
 */
static uart_module_t *uart_modules[MAX_UART_PORT];

static int uart_module_is_on(uint32_t item_handle)
{
    int i;
    for (i = 0; i < MAX_UART_PORT; i++) {
        uart_module_t *m = uart_modules[i];
        if (m && m->item_handle == item_handle) return 1;
    }
    return 0;
}

static void uart_module_handle(void *data)
{
    uart_module_t *module = (uart_module_t *)data;
    uint8_t *raw_buff     = module->recv_buff;
    uint32_t max_len;
    uint32_t recvsize = 0;
    uint8_t *start_pos;
    uart_dev_t *dev = (uart_dev_t *)module->uart;

    max_len = UART_BUFF_SIZE - module->recv_index - 1;
    if (max_len <= 0) {
        module->recv_index = 0;
        max_len            = UART_BUFF_SIZE - 1;
    }

    hal_uart_recv_II(dev, &raw_buff[module->recv_index], max_len, &recvsize,
                     module->loop_time);
    if (recvsize <= 0) {
        return;
    }
    module->recv_index += recvsize;
    raw_buff[module->recv_index] = 0;

    start_pos = raw_buff;
    if (module->start_flag) {
        start_pos = (uint8_t *)strstr((const char *)raw_buff,
                                      (const char *)module->start_flag);
        if (!start_pos) {
            jse_debug("start_pos is null\n");
            return;
        }
    }

    if (module->end_flag) {
        uint8_t *end_pos;

        if (module->start_flag)
            end_pos = strstr((const char *)start_pos +
                                 strlen((const char *)module->start_flag),
                             (const char *)module->end_flag);
        else
            end_pos =
                strstr((const char *)start_pos, (const char *)module->end_flag);
        if (!end_pos) {
            jse_debug("recvsize is null\n");
            return;
        }
        end_pos[strlen((const char *)module->end_flag)] = 0;
    }

    jse_debug("start_pos: %s\n", start_pos);
    int js_cb_ref    = module->js_cb_ref;
    duk_context *ctx = bone_engine_get_context();
    bone_engine_push_ref(ctx, js_cb_ref);
    duk_push_string(ctx, start_pos);
    duk_pcall(ctx, 1);
    duk_pop(ctx);
}

static int uart_add_recv(uart_dev_t *uart, uint32_t item_handle, int js_cb_ref,
                         uint8_t *start_flag, uint8_t *end_flag)
{
    /* 找出一个空闲的uart_module */
    int i;
    for (i = 0; i < MAX_UART_PORT; i++)
        if (!uart_modules[i]) break;
    if (i >= MAX_UART_PORT) {
        jse_warn("No free uart module\n");
        return -1;
    }

    uart_module_t *module = jse_calloc(1, sizeof(uart_module_t));
    if (!module) {
        jse_error("uart_start_recv fail!\n");
        return -1;
    }
    module->js_cb_ref   = js_cb_ref;
    module->item_handle = item_handle;
    module->uart        = uart;
    module->loop_time   = 2;
    module->start_flag  = start_flag;
    module->end_flag    = end_flag;
    module->recv_index  = 0;
    module->timer_id = be_jse_task_timer_action(100, uart_module_handle, module,
                                                JSE_TIMER_REPEAT);
    if (!module->timer_id) {
        duk_context *ctx = bone_engine_get_context();
        bone_engine_unref(ctx, module->js_cb_ref);
        jse_free(module->start_flag);
        jse_free(module->end_flag);
        jse_free(module);
        return -1;
    }

    uart_modules[i] = module;
    return 0;
}

static void uart_del_recv(uint32_t item_handle)
{
    int i;
    for (i = 0; i < MAX_UART_PORT; i++) {
        uart_module_t *m = uart_modules[i];
        if (m && m->item_handle == item_handle) {
            duk_context *ctx = bone_engine_get_context();
            bone_engine_unref(ctx, m->js_cb_ref);
            be_jse_task_cancel_timer(m->timer_id);
            jse_free(m->start_flag);
            jse_free(m->end_flag);
            jse_free(m);
            uart_modules[i] = NULL;
            break;
        }
    }
}

static duk_ret_t native_uart_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = 0xFFFFFFFF;
    uart_dev_t *uart_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("parameter must be string\n");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_UART, id, &uart_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }
    jse_debug("UART", "uart handle:%u\n", uart_handle.handle);
    uart_device = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_uart_init(uart_device);
    if (0 != ret) {
        jse_error("hal_uart_init fail!\n");
    }
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
        jse_warn("parameter must be handle\n");
        goto out;
    }
    uart_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_uart_finalize(uart_device);
    if (0 != ret) {
        jse_error("hal_uart_init fail!\n");
        goto out;
    }
    board_disattach_item(MODULE_UART, &uart_handle);
    uart_del_recv(uart_handle.handle);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_uart_write(duk_context *ctx)
{
    int ret          = -1;
    const char *data = NULL;
    uint32_t len     = 0;
    item_handle_t uart_handle;
    uart_dev_t *uart_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1)) {
        jse_warn("parameter must be handle and string\n");
        goto out;
    }
    uart_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    data = duk_get_string(ctx, 1);
    len  = strlen(data);
    ret  = hal_uart_send(uart_device, data, len, 0);
    if (-1 == ret) {
        jse_error("hal_uart_send fail!\n");
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_uart_read(duk_context *ctx)
{
    jse_error("todo......\n");
    duk_push_null(ctx);
    return 1;
}

/* JS接口：
 *   UART.on(handle, start, end, function(data) {})
 * 参数说明：
 *   start － null 或者 string
 *   end － null 或者 string
 */
static duk_ret_t native_uart_on(duk_context *ctx)
{
    int ret = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = 0xFFFFFFFF;
    uart_dev_t *uart_device = NULL;
    uint8_t *start          = NULL;
    uint8_t *end            = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 3)) {
        jse_warn("parameter must be handle, null|string, null|string, function\n");
        goto out;
    }
    uart_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    if (uart_module_is_on(uart_handle.handle)) {
        jse_error("The uart module has set the listener\n");
        goto out;
    }
    if (duk_is_string(ctx, 1)) {
        start = strdup(duk_get_string(ctx, 1));
        if (!start) {
            jse_warn("allocate memory failed\n");
            goto out;
        }
    }
    if (duk_is_string(ctx, 2)) {
        end = strdup(duk_get_string(ctx, 2));
        if (!end) {
            jse_warn("allocate memory failed\n");
            jse_free(start);
            goto out;
        }
    }
    duk_dup(ctx, 1);
    int js_cb_ref = bone_engine_ref(ctx);
    ret = uart_add_recv(uart_device, uart_handle.handle, js_cb_ref, start, end);
    if (ret < 0) {
        jse_error("uart_add_recv fail!\n");
    }
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_uart_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_uart_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_uart_read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, native_uart_write, 2);
    duk_put_prop_string(ctx, -2, "write");

    duk_push_c_function(ctx, native_uart_on, 4);
    duk_put_prop_string(ctx, -2, "on");

    duk_push_c_function(ctx, native_uart_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "UART");
}
