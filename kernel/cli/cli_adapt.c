/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"

#include "aos/hal/uart.h"

#include "cli_conf.h"
#include "cli_adapt.h"
#include "cli_err.h"

#ifndef STDIO_UART
#define CLI_UART 0
#else
#define CLI_UART STDIO_UART
#endif

int32_t cli_task_create(const char *name, void (*fn)(void *), void *arg,
                        uint32_t stack, uint32_t priority)
{
    ktask_t *task_handle = NULL;

    return krhino_task_dyn_create(&task_handle, name, arg, priority, 0,
                                  stack / sizeof(cpu_stack_t), fn, 1u);
}

void cli_task_exit(void)
{
    krhino_task_dyn_del(NULL);
}

int32_t cli_getchar(char *inbuf)
{
    uart_dev_t uart_stdio;

    int32_t  ret       = CLI_OK;
    uint32_t recv_size = 0;

    memset(&uart_stdio, 0, sizeof(uart_dev_t));
    uart_stdio.port = CLI_UART;

    ret = hal_uart_recv_II(&uart_stdio, inbuf, 1, &recv_size, HAL_WAIT_FOREVER);

    if ((ret == 0) && (recv_size == 1)) {
        return recv_size;
    } else {
        return 0;
    }
}

int32_t cli_putstr(char *msg)
{
    uart_dev_t uart_stdio;

    memset(&uart_stdio, 0, sizeof(uart_dev_t));
    uart_stdio.port = CLI_UART;

    if (msg[0] != 0) {

#if (CLI_TELNET_SUPPORT > 0)
        extern void TelnetWriteString(char *string);
        TelnetWriteString(msg);
#endif

        hal_uart_send(&uart_stdio, (void *)msg, strlen(msg), HAL_WAIT_FOREVER);
    }

    return CLI_OK;
}

void *cli_malloc(uint32_t size)
{
    return krhino_mm_alloc(size);
}

void cli_free(void *ptr)
{
    krhino_mm_free(ptr);
}
