/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "aos_syscall_arg.h"
#include "aos/kernel.h"

bool hal_addr_overlap_kernel(void *addr, size_t len);

char *sys_aos_version_str_get_stub(void *arg)
{
    aos_version_str_get_syscall_arg_t *_arg = arg;


    if (_arg->buf == NULL) {
        return NULL;
    }

    return aos_version_str_get(_arg->buf, _arg->len);
}

void sys_aos_get_calendar_time_stub(void *arg)
{
    uint64_t *calender_time  = (uint64_t *)arg;

    *calender_time = aos_calendar_time_get();
}

void sys_aos_set_calendar_time_stub(void *arg)
{
    uint64_t calender_time  = *(uint64_t *)arg;

    aos_calendar_time_set(calender_time);
}

void sys_aos_get_utc_offset_stub(void *arg)
{
    uint64_t *offset = (uint64_t *)arg;

    *offset = aos_utc_offset_get();
}

void sys_aos_set_utc_offset_stub(void *arg)
{
    uint64_t offset = *(uint64_t *)arg;

    aos_utc_offset_set(offset);
}

void sys_aos_reboot_stub(void *arg)
{
    aos_reboot();
}

void sys_aos_now_ms_stub(void *arg)
{
    uint64_t *sys_time = (uint64_t *)arg;
    *sys_time = aos_now_ms();
}

void sys_aos_get_hz_stub(void *arg)
{
    int *hz = (int *)arg;
    *hz = aos_get_hz();
}

void *sys_aos_ipc_copy_stub(void *arg)
{
    aos_ipc_copy_syscall_arg_t *_arg = arg;

    if (hal_addr_overlap_kernel(_arg->dst, _arg->len)) {
        return NULL;
    }

    if (hal_addr_overlap_kernel(_arg->src, _arg->len)) {
        return NULL;
    }

    return memcpy(_arg->dst, _arg->src, _arg->len);
}

int sys_aos_print_stub(void *arg)
{
    aos_print_syscall_arg_t *_arg = arg;

    if (_arg->buf == NULL) {
        return 0;
    }

    return aos_print(_arg->console, _arg->buf, _arg->size);
}
