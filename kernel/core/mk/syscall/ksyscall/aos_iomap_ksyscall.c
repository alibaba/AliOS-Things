/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "k_config.h"
#include "syscall.h"
#include "syscall_no.h"
#include "k_compiler.h"
#include "iomap.h"
#include "aos_iomap_syscall_arg.h"

void sys_aos_mem_mmap_stub(void *arg)
{
    aos_mm_map_syscall_arg_t *_arg = arg;
    aos_mem_mmap(_arg->vaddr, _arg->paddr, _arg->len, _arg->attr);
}

void sys_aos_io_mmap_stub(void *arg)
{
    aos_io_map_syscall_arg_t *_arg = arg;
    aos_io_mmap(_arg->vaddr, _arg->paddr, _arg->len);
}

void sys_aos_io_unmmap_stub(void *arg)
{
    aos_io_unmap_syscall_arg_t *_arg = arg;

    aos_io_unmmap(_arg->vaddr);
}

uintptr_t sys_aos_io_mva2pa_stub(void *arg)
{
    aos_io_mva2pa_syscall_arg_t *_arg = arg;

    return aos_io_mva2pa(_arg->vaddr);
}

uintptr_t sys_aos_io_mpa2va_stub(void *arg)
{
    aos_io_mpa2va_syscall_arg_t *_arg = arg;

    return aos_io_mpa2va(_arg->paddr);
}

RHINO_WEAK int32_t _aos_get_sys_mem_info(int type, uintptr_t addr, int len)
{
    (void)type;
    (void)addr;
    (void)len;
    if (type == 0) {
        *(int *)addr = 0;
    }
    return 0;
}

int sys_aos_get_sys_mem_info_stub(void *arg)
{
    aos_get_sys_mem_info_arg_t *_arg = arg;

    return _aos_get_sys_mem_info(_arg->type, _arg->addr, _arg->len);
}


