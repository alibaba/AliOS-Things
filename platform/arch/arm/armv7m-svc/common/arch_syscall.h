/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#include <svc.h>

#if defined(__CC_ARM)
#error "SVC is not implemented on __CC_ARM"
#elif defined(__ICCARM)
#error "SVC is not implemented on __ICCARM"
#elif defined (__GNUC__)
static inline unsigned long long SYSCALL(int syscall_no, void *arg)
{
    register unsigned long long ret asm("r0");

    register int reg0 asm("r0") = (int)syscall_no;
    register int reg1 asm("r1") = (int)arg;

    /* r0-r3:   caller-save registers
     * r4-r8:   callee-save registers
     * r9:      might be callee-save register or not,
     *          it depends on the AAPCS.
     * r10-r11: callee-save registers
     * r12-r15: special registers
     */

    /* use r8 to save the return address during
     * executing system call on Cortex-M platform
     */

    __asm__ __volatile__ (
        "push   {r2,r3,r8,r9,r12,r14}\t\n"
        "svc    %[_svc_no]\t\n"
        "pop    {r2,r3,r8,r9,r12,r14}\t\n"
        : "=r"(ret)
        : [_svc_no]"i"(SVC_syscall), "r"(reg0), "r"(reg1)
        : "memory"
        );

    return ret;
}
#else
#error "Unknown Compiler"
#endif

#endif /* ARCH_SYSCALL_H */

