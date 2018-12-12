/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#include <svc.h>

static inline __asm unsigned long long SYSCALL(int syscall_no, void *arg)
{
    push {lr}
    svc  #SVC_syscall
    pop  {pc}
}

#endif /* ARCH_SYSCALL_H */

