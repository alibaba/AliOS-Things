/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_IOMAP_SYSCALL_H
#define AOS_IOMAP_SYSCALL_H
#include "stdint.h"

typedef struct {
    uintptr_t vaddr;
    uintptr_t paddr;
    size_t len;
} aos_io_map_syscall_arg_t;

typedef struct {
    uintptr_t vaddr;
} aos_io_unmap_syscall_arg_t;

typedef struct {
    uintptr_t vaddr;
} aos_io_mva2pa_syscall_arg_t;

typedef struct {
    uintptr_t paddr;
} aos_io_mpa2va_syscall_arg_t;

typedef struct {
    int type;
    uintptr_t addr;
    int len;
} aos_get_sys_mem_info_arg_t;

typedef struct {
    uintptr_t vaddr;
    uintptr_t paddr;
    size_t len;
    int attr;
} aos_mm_map_syscall_arg_t;


#endif /* AOS_IOMAP_SYSCALL_H */

