/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_jse_module.h"
#include "jse_task.h"
#include "be_port_hal.h"
#include "board-mgr/board_mgr.h"

#define UART_BUFF_SIZE (256)
#define MAX_UART_PORT (6)

typedef struct uart_module {
    uart_dev_t *uart;
    uint32_t loop_time;
    uint32_t item_handle;
    uint8_t *start_flag;
    uint8_t *end_flag;
    uint32_t recv_index;
    uint32_t read_index;
    uint8_t recv_buff[UART_BUFF_SIZE];
    be_jse_symbol_t *fun_symbol;
} uart_module_t;

static uart_module_t *g_uart_module[MAX_UART_PORT] = {NULL};
static inline uart_module_t **get_handle(void)
{
    return (uart_module_t **)&g_uart_module;
}

static int32_t uart_get_free()
{
    uint32_t i             = 0;
    uart_module_t **module = get_handle();
    if (NULL == module) {
        return (-1);
    }
    for (i = 0; i < MAX_UART_PORT; ++i) {
        if (NULL != module[i]) {
            continue;
        }
        return i;
    }
    return (-1);
}

static int8_t uart_get_module(uint32_t item_handle)
{
    uint32_t i             = 0;
    uart_module_t **module = get_handle();
    if (NULL == module) {
        return (-1);
    }
    for (i = 0; i < MAX_UART_PORT; ++i) {
        if (NULL == module[i] || item_handle != module[i]->item_handle) {
            continue;
        }
        return i;
    }

    return (-1);
}

static void uart_handle(void *data)
{
    uart_module_t *module = (uart_module_t *)data;
    if (NULL == module) {
        return;
    }
    uint8_t *raw_buff           = module->recv_buff;
    uint32_t max_len            = UART_BUFF_SIZE - module->recv_index - 1;
    uint32_t recvsize           = 0;
    int8_t ret                  = -1;
    uint8_t flag                = 1;
    uint8_t *start_pos          = NULL;
    uint8_t *end_pos            = NULL;
    uint32_t i                  = 0;
    be_jse_symbol_t *fun_symbol = (be_jse_symbol_t *)module->fun_symbol;

    if (NULL == fun_symbol || !symbol_is_function(fun_symbol)) {
        return;
    }
    if (max_len <= 0) {
        module->recv_index = 0;
        max_len            = UART_BUFF_SIZE - 1;
    }
    uart_dev_t *dev = (uart_dev_t *)module->uart;

    ret = hal_uart_recv_II(dev, &raw_buff[module->recv_index], max_len,
                           &recvsize, module->loop_time);
    if (ret < 0) {
        goto out;
    }
    module->recv_index += recvsize;
    start_pos = raw_buff;
    end_pos   = &raw_buff[module->recv_index];

    raw_buff[module->recv_index] = 0;
    if (NULL != module->start_flag) {
        start_pos = strstr(raw_buff, module->start_flag);
        if (NULL == start_pos) {
            goto out;
        }
    }
    if (NULL != module->end_flag) {
        uint8_t *tmp = NULL;
        if (NULL != module->start_flag) {
            tmp = start_pos + strlen(module->start_flag);
        } else {
            tmp = raw_buff;
        }
        end_pos = strstr(tmp, module->end_flag);
        if (NULL == end_pos) {
            goto out;
        }
        *(uint8_t *)(end_pos + strlen(module->end_flag)) = 0;
    }

    be_jse_symbol_t *arg[1];
    arg[0] = new_str_symbol(start_pos);
    be_jse_execute_func(jsengine_get_executor(), fun_symbol, 1, arg);
    module->recv_index = 0;

out:

    /* be_jse_task_schedule_call(uart_handle,module); */
    /* be_osal_post_delayed_action(module->loop_time,uart_handle,module); */
    return;
}

static int8_t uart_add_recv(uart_dev_t *uart, uint32_t item_handle,
                            be_jse_symbol_t *fun_symbol, uint8_t *start_flag,
                            uint8_t *end_flag)
{
    int8_t ret             = -1;
    uart_module_t **handle = get_handle();
    if (NULL == handle) {
        return (-1);
    }
    int32_t index = uart_get_free();
    if (index < 0) {
        return (-1);
    }
    uart_module_t *module = jse_calloc(1, sizeof(uart_module_t));
    if (NULL == module) {
        jse_error("uart_start_recv fail!\n");
        return (-1);
    }
    module->fun_symbol  = fun_symbol;
    module->item_handle = item_handle;
    module->uart        = uart;
    module->loop_time   = 2;
    module->start_flag  = start_flag;
    module->end_flag    = end_flag;
    module->recv_index  = 0;
    if (NULL ==
        be_jse_task_timer_action(100, uart_handle, module, JSE_TIMER_REPEAT)) {
        jse_free(module);
        module = NULL;
        return (-1);
    }
    handle[index] = module;
    return (0);
}

static int8_t uart_del_recv(uint32_t item_handle)
{
    int8_t ret             = -1;
    uart_module_t **handle = get_handle();
    if (NULL == handle) {
        return (-1);
    }
    int32_t index = uart_get_module(item_handle);
    if (index < 0) {
        return (-1);
    }

    if (NULL == handle[index]) {
        return (-1);
    }
    if (NULL != handle[index]->end_flag) {
        jse_free(handle[index]->end_flag);
        handle[index]->end_flag = NULL;
    }
    if (NULL != handle[index]->start_flag) {
        jse_free(handle[index]->start_flag);
        handle[index]->start_flag = NULL;
    }
    symbol_unlock(handle[index]->fun_symbol);
    jse_free(handle[index]);
    handle[index] = NULL;

    return (0);
}

static be_jse_symbol_t *uart_open(void)
{
    int32_t len   = -1;
    char *data    = NULL;
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = 0xFFFFFFFF;
    be_jse_symbol_t *arg0   = NULL;
    uart_dev_t *uart_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_string(arg0)) {
        goto out;
    }
    len  = symbol_str_len(arg0);
    data = jse_calloc(1, sizeof(char) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    symbol_to_str(arg0, data, len);
    ret = board_attach_item(MODULE_UART, data, &uart_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }
    jse_debug("uart handle:%u\n", uart_handle.handle);
    uart_device = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_uart_init(uart_device);
    if (0 != ret) {
        jse_error("hal_uart_init fail!\n");
        goto out;
    }
    result = 0;
out:
    if (NULL != data) {
        jse_free(data);
        data = NULL;
    }
    symbol_unlock(arg0);
    if (0 != result) {
        board_disattach_item(MODULE_UART, &uart_handle);
        return new_int_symbol(-1);
    }

    return new_int_symbol(uart_handle.handle);
}

static be_jse_symbol_t *uart_close(void)
{
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t uart_handle;
    be_jse_symbol_t *arg0   = NULL;
    uart_dev_t *uart_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    uart_handle.handle = get_symbol_value_int(arg0);
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
    result = 0;
out:
    symbol_unlock(arg0);

    return new_int_symbol(result);
}

static be_jse_symbol_t *uart_write(void)
{
    int8_t ret    = -1;
    int8_t result = -1;
    char *data    = NULL;
    uint32_t len  = 0;
    item_handle_t uart_handle;
    be_jse_symbol_t *arg0   = NULL;
    be_jse_symbol_t *arg1   = NULL;
    uart_dev_t *uart_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    uart_handle.handle = get_symbol_value_int(arg0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    if (!arg1 || !symbol_is_string(arg1)) {
        goto out;
    }
    len  = symbol_str_len(arg1);
    data = jse_calloc(1, sizeof(char) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    symbol_to_str(arg1, data, len);
    ret = hal_uart_send(uart_device, data, len, 0);
    if (-1 == ret) {
        jse_error("hal_uart_send fail!\n");
        goto out;
    }
    result = 0;
out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    if (NULL != data) {
        jse_free(data);
        data = NULL;
    }

    return new_int_symbol(len);
}

static be_jse_symbol_t *uart_read(void)
{
    jse_error("todo......\n");
    return new_int_symbol(-1);
}

static be_jse_symbol_t *uart_watch(void)
{
    int32_t len         = -1;
    char *data          = NULL;
    uint8_t *start_flag = NULL;
    uint8_t *end_flag   = NULL;
    int8_t ret          = -1;
    int8_t result       = -1;
    item_handle_t uart_handle;
    uart_handle.handle      = 0xFFFFFFFF;
    be_jse_symbol_t *arg0   = NULL;
    be_jse_symbol_t *arg1   = NULL;
    be_jse_symbol_t *arg2   = NULL;
    be_jse_symbol_t *arg3   = NULL;
    uart_dev_t *uart_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, &arg3);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    uart_handle.handle = get_symbol_value_int(arg0);
    uart_device        = board_get_node_by_handle(MODULE_UART, &uart_handle);
    if (NULL == uart_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    if (uart_get_module(uart_handle.handle) > 0) {
        jse_error("uart_get_module fail!\n");
        goto out;
    }

    if (!arg3 || !symbol_is_function(arg3)) {
        goto out;
    }
    if (arg1 && symbol_is_string(arg1)) {
        len        = symbol_str_len(arg1);
        start_flag = jse_calloc(1, sizeof(uint8_t) * (len + 1));
        if (NULL == start_flag) goto out;
        symbol_to_str(arg1, start_flag, len);
    }

    if (arg2 && symbol_is_string(arg2)) {
        len      = symbol_str_len(arg2);
        end_flag = jse_calloc(1, sizeof(uint8_t) * (len + 1));
        if (NULL == end_flag) goto out;
        symbol_to_str(arg2, end_flag, len);
    }
    ret = uart_add_recv(uart_device, uart_handle.handle, arg3, start_flag,
                        end_flag);
    if (ret < 0) {
        jse_error("uart_add_recv fail!\n");
        goto out;
    }
    result = 0;

out:

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    if (0 != result) {
        symbol_unlock(arg3);
        if (NULL != start_flag) {
            jse_free(start_flag);
            start_flag = NULL;
        }
        if (NULL != end_flag) {
            jse_free(end_flag);
            end_flag = NULL;
        }
    }

    return new_int_symbol(result);
}

static be_jse_symbol_t *uart_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                              be_jse_symbol_t *var,
                                              const char *name)
{
    if (0 == strcmp(name, "open")) {
        return uart_open();
    }
    if (0 == strcmp(name, "read")) {
        return uart_read();
    }
    if (0 == strcmp(name, "write")) {
        return uart_write();
    }
    if (0 == strcmp(name, "on")) {
        return uart_watch();
    }
    if (0 == strcmp(name, "close")) {
        return uart_close();
    }
    return (BE_JSE_FUNC_UNHANDLED);
}

void module_uart_register(void)
{
    be_jse_module_load("UART", uart_module_handle_cb);
}
