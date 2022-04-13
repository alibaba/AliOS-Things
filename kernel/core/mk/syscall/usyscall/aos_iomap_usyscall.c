/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <utask.h>
#include "k_config.h"
#include "syscall.h"
#include "syscall_no.h"
#include "aos_iomap_syscall_arg.h"

int32_t aos_io_mmap(uintptr_t vaddr, uintptr_t paddr, size_t len)
{
    aos_io_map_syscall_arg_t _arg;
    _arg.vaddr = vaddr;
    _arg.paddr = paddr;
    _arg.len = len;
    return (int32_t)SYSCALL(SYS_AOS_IOMAP_MAP, (void *)&_arg);
}

int32_t aos_io_unmmap(uintptr_t vaddr)
{
    aos_io_unmap_syscall_arg_t _arg;
    _arg.vaddr = vaddr;
    return (int32_t)SYSCALL(SYS_AOS_IOMAP_UNMAP, (void *)&_arg);
}

uintptr_t aos_io_mva2pa(uintptr_t vaddr)
{
    /*
    aos_io_mva2pa_syscall_arg_t _arg;
    _arg.vaddr = vaddr;
    return (int32_t)SYSCALL(SYS_AOS_IOMAP_VA2PA, (void*)&_arg);*/
    return vaddr;
}

uintptr_t aos_io_mpa2va(uintptr_t paddr)
{
    /*
    aos_io_mpa2va_syscall_arg_t _arg;
    _arg.paddr = paddr;
    return (int32_t)SYSCALL(SYS_AOS_IOMAP_PA2VA, (void*)&_arg);
    */
    return paddr;
}

int32_t aos_get_sys_mem_info(int type, uintptr_t addr, int len)
{
    aos_get_sys_mem_info_arg_t _arg;
    _arg.type = type;
    _arg.addr = addr;
    _arg.len = len;
    return (int32_t)SYSCALL(SYS_AOS_MEM_INFO_GET, (void*)&_arg);
}

int32_t aos_mem_mmap(uintptr_t vaddr, uintptr_t paddr, size_t len, int attr)
{
    aos_mm_map_syscall_arg_t _arg;
    _arg.vaddr = vaddr;
    _arg.paddr = paddr;
    _arg.len = len;
    _arg.attr = attr;
    return (int32_t)SYSCALL(SYS_AOS_MEMMAP_MAP, (void *)&_arg);
}

