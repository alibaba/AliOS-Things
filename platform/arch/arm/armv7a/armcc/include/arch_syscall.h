/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#include <svc.h>

#if defined(__CC_ARM)
static inline __asm unsigned long long SYSCALL(int syscall_no, void *arg)
{
    push {r2, r3, r8, lr}
    svc  #SVC_syscall
    pop  {r2, r3, r8, pc}
}
#elif defined(__GNUC__)
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
#endif

#endif /* ARCH_SYSCALL_H */

