/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include "syscall.h"
#include "syscall_no.h"
#include "aos_syscall_arg.h"

char *aos_version_str_get(char *buf, size_t len)
{
    aos_version_str_get_syscall_arg_t arg;

    arg.buf = buf;
    arg.len = len;

    return (char *)SYSCALL(SYS_AOS_VERSION_STR_GET, &arg);
}

uint64_t aos_calendar_time_get(void)
{
    uint64_t calender_time = 0;

    SYSCALL(SYS_AOS_GET_CALENDAR, (void *)&calender_time);

    return calender_time;
}

void aos_calendar_time_set(uint64_t now_ms)
{
    SYSCALL(SYS_AOS_SET_CALENDAR, (void *)&now_ms);
}

uint64_t aos_utc_offset_get(void)
{
    uint64_t offset = 0;

    SYSCALL(SYS_AOS_GET_UTC_OFFSET, (void *)&offset);

    return offset;
}

void aos_utc_offset_set(uint64_t offset_ms)
{
    SYSCALL(SYS_AOS_SET_UTC_OFFSET, (void *)&offset_ms);
}

void aos_reboot(void)
{
    SYSCALL(SYS_AOS_REBOOT, NULL);
}

/* ------------------ time ------------------ */

uint64_t aos_now_ms(void)
{
    uint64_t sys_time;

    SYSCALL(SYS_AOS_NOW_MS, (void *)&sys_time);

    return sys_time;
}

uint64_t aos_now(void)
{
    uint64_t sys_time;

    SYSCALL(SYS_AOS_NOW_MS, (void *)&sys_time);

    return sys_time * 1000;
}

int aos_get_hz(void)
{
    int hz;

    SYSCALL(SYS_AOS_GET_HZ, (void *)&hz);

    return hz;
}

void *aos_ipc_copy(void *dst, void *src, size_t len)
{
    aos_ipc_copy_syscall_arg_t arg;
    arg.dst = dst;
    arg.src = src;
    arg.len = len;
    return (void*)SYSCALL(SYS_AOS_IPC_COPY, &arg);
}

/* ------------------ print ------------------ */

int aos_print(void *console,const void *buf, int size)
{
    aos_print_syscall_arg_t arg;

    arg.console = console;
    arg.buf     = (void *)buf;
    arg.size    = size;

    return (int)SYSCALL(SYS_AOS_PRINT, &arg);
}

