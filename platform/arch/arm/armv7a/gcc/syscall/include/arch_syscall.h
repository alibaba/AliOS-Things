/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#include <svc.h>

#define SYSCALL(syscall_no, arg1) \
    ({ \
        unsigned int _syscall_no = syscall_no; \
        unsigned int _arg1 = (unsigned int)arg1; \
        union { \
            unsigned long long u64; \
            unsigned long u32[2]; \
        } _ret; \
        __asm__ __volatile__ ( \
            "push   {r2,r3,r8,lr}\t\n" \
            "mov    r0, %[_syscall_no]\t\n" \
            "mov    r1, %[_arg1]\t\n" \
            "svc    %[_svc_no]\t\n" \
            "pop    {r2,r3,r8,lr}\t\n" \
            "mov    %[_ret_0], r0\t\n" \
            "mov    %[_ret_1], r1\t\n" \
            : [_ret_0]"=r"(_ret.u32[0]), \
              [_ret_1]"=r"(_ret.u32[1]) \
            : [_syscall_no]"r"(_syscall_no), \
              [_arg1]"r"(_arg1), \
              [_svc_no]"i"(SVC_syscall) \
            : "r0", "r1" \
            ); \
            _ret.u64; \
    })


#endif // ARCH_SYSCALL_H

