/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H


#define SYSCALL(syscall_no, arg1) \
    ({ \
        unsigned int _syscall_no = syscall_no; \
        unsigned int _arg1 = (unsigned int)arg1; \
        int _ret; \
        __asm__ __volatile__ ( \
            "push {r2,r3,r8}\t\n" \
            "mov r0, %[_syscall_no]\t\n" \
            "mov r1, %[_arg1]\t\n" \
            "svc #0x10\t\n" \
            "pop {r2,r3,r8}\t\n" \
            "mov %[_ret], r0\t\n" \
            : [_ret]"=r"(_ret) \
            : [_syscall_no]"r"(_syscall_no), [_arg1]"r"(_arg1) \
            : "r0", "r1" \
            ); \
            _ret; \
    })


#endif // ARCH_SYSCALL_H

