/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/kernel.h"
#include "aos/hal/uart.h"
#include "cli_conf.h"
#include "cli_adapt.h"
#include "cli_console.h"

#ifndef STDIO_UART
#define CLI_UART 0
#else
#define CLI_UART STDIO_UART
#endif

static aos_task_t cli_task;

int32_t cli_task_create(const char *name, void (*fn)(void *), void *arg,
                        uint32_t stack, uint32_t priority)
{
    return aos_task_new_ext(&cli_task, name, fn, arg, stack, priority, 1);
}

void cli_task_exit(void)
{
    aos_task_exit(0);
}

int32_t cli_getchar(char *inbuf)
{
    return cli_console_read(get_clitask_console(), inbuf, 1);
}

int32_t cli_putstr(char *msg)
{
    return cli_console_write(get_clitask_console(), (void *)msg, strlen(msg));
}

void *cli_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void cli_free(void *ptr)
{
    aos_free(ptr);
}
