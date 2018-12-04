/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#include <svc.h>

static inline unsigned long long SYSCALL(int syscall_no, void *arg)
{
    register unsigned long long ret asm("r0");

    register int reg0 asm("r0") = (int)syscall_no;
    register int reg1 asm("r1") = (int)arg;


    __asm__ __volatile__ (
        "svc    %[_svc_no]\t\n"
        : "=r"(ret)
        : [_svc_no]"i"(SVC_syscall), "r"(reg0), "r"(reg1)
        : "r14", "memory"
        );

        return ret;
}

#endif /* ARCH_SYSCALL_H */

