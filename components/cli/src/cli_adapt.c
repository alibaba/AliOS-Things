/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdint.h>
#include "aos/kernel.h"
#include "aos/cli.h"
#include "cli_adapt.h"
#include "cli_console.h"


static aos_task_t cli_task;

int32_t cli_task_create(const char *name, void (*fn)(void *), void *arg,
                        uint32_t stack, uint32_t priority)
{
    return aos_task_new_ext(&cli_task, name, fn, arg, stack, priority);
}

void cli_task_exit(void)
{
    aos_task_exit(0);
}

int32_t cli_getchar(char *inbuf)
{
    return cli_console_read(get_clitask_console(), inbuf, 1);
}

int32_t cli_putchar(char ch)
{
    return cli_console_write(get_clitask_console(), &ch, 1);
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

int aos_cli_suspend(void)
{
    return (int)aos_task_suspend(&cli_task);
}

int aos_cli_resume(void)
{
    return (int)aos_task_resume(&cli_task);
}
