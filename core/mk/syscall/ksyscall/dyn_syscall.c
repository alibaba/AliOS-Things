/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include "k_config.h"
#include "syscall_no.h"

extern void *syscall_tbl[];

int register_syscall(int syscall_no, void *func)
{
    /* invalid syscall number */
    if (syscall_no < SYS_USER_DEFINE_BASE
        || syscall_no > SYS_USER_DEFINE_END) {
        return -1;
    }

    /* syscall is already registered */
    if (NULL != syscall_tbl[syscall_no]) {
        return -2;
    }

    syscall_tbl[syscall_no] = func;

    return 0;
}

int unregister_syscall(int syscall_no)
{
    /* invalid syscall number */
    if (syscall_no < SYS_USER_DEFINE_BASE
        || syscall_no > SYS_USER_DEFINE_END) {
        return -1;
    }

    syscall_tbl[syscall_no] = NULL;

    return 0;
}

