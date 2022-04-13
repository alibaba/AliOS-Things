/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef SIGNAL_SYSCALL_H
#define SIGNAL_SYSCALL_H

#include <aos/kernel.h>
#include <aos/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t pid;
    int      sig;
} aos_kill_syscall_arg_t;

typedef struct {
    aos_hdl_t *task;
    int      sig;
} aos_tkill_syscall_arg_t;

typedef struct {
    int        how;
    const ksigset_t *set;
    ksigset_t *old;
} aos_sigprocmask_syscall_arg_t;

typedef struct {
    int            sig;
    ksighandler_t  sighand;
    ksighandler_t *oldsighand;
} aos_sigaction_syscall_arg_t;

#ifdef __cplusplus
}
#endif

#endif /* SIGNAL_SYSCALL_H */
