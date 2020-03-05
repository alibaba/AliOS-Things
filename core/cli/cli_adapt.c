/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"

#include "aos/hal/uart.h"

#include "cli_api.h"

#include "cli_conf.h"
#include "cli_adapt.h"

#if defined(AOS_COMP_UAGENT)
#include "cli_uagent.h"
#endif

#ifndef STDIO_UART
#define CLI_UART 0
#else
#define CLI_UART STDIO_UART
#endif

static ktask_t *task_handle = NULL;

int32_t cli_task_create(const char *name, void (*fn)(void *), void *arg,
                        uint32_t stack, uint32_t priority)
{
    return krhino_task_dyn_create(&task_handle, name, arg, priority, 0,
                                  stack / sizeof(cpu_stack_t), fn, 1u);
}

void cli_task_exit(void)
{
    krhino_task_dyn_del(NULL);
}

void cli_task_cancel(void)
{
    if (task_handle != NULL) {
        krhino_task_cancel(task_handle);
    }
}

int32_t cli_task_cancel_check(void)
{
    RHINO_BOOL ret;

    ret = krhino_task_cancel_chk();

    return (ret == RHINO_TRUE)? 1: 0;
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
    if (msg[0] != 0) {
#if (CLI_TELNET_SUPPORT > 0)
        extern void TelnetWriteString(char *string);
        TelnetWriteString(msg);
#endif

#if defined(AOS_COMP_UAGENT)
        if (g_cmd_from_cloud) {
            uagent_send(UAGENT_MOD_CLI, CLI_RESPONE, strlen(msg), msg, send_policy_delay);
        } else {
#endif
            memset(&uart_stdio, 0, sizeof(uart_dev_t));
            uart_stdio.port = CLI_UART;
            hal_uart_send(&uart_stdio, (void *)msg, strlen(msg), HAL_WAIT_FOREVER);
#if defined(AOS_COMP_UAGENT)
        }
#endif
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
