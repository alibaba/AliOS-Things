/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include "syscall.h"
#include "syscall_no.h"
#include <aos/signal.h>
#include <signal_syscall_arg.h>

int aos_kill(uint32_t pid, int sig)
{
    aos_kill_syscall_arg_t arg;

    if ((sig < 0) || (sig >= _KNSIG)) {
        return -EINVAL;
    }

    arg.pid = pid;
    arg.sig = sig;

    return (int)SYSCALL(SYS_SIGNAL_KILL, &arg);
}

int aos_tkill(aos_hdl_t *task, int sig)
{
    aos_tkill_syscall_arg_t arg;

    if ((task == NULL) || (sig < 0) || (sig >= _KNSIG)) {
        return -EINVAL;
    }

    arg.task = task;
    arg.sig  = sig;

    return (int)SYSCALL(SYS_SIGNAL_TKILL, &arg);
}

int aos_sigpending(ksigset_t *set)
{
    if (set == NULL) {
        return -EINVAL;
    }

    return (int)SYSCALL(SYS_SIGNAL_PENDING, set);
}

int aos_sigprocmask(int how, const ksigset_t *set, ksigset_t *old)
{
    aos_sigprocmask_syscall_arg_t arg;
    arg.how = how;
    arg.set = set;
    arg.old = old;

    return (int)SYSCALL(SYS_SIGNAL_PROCMASK, &arg);
}

int aos_sigaction(int sig, const ksighandler_t sighand, ksighandler_t *oldsighand)
{
    aos_sigaction_syscall_arg_t arg;

    if ((sig < 1) || (sig >= _KNSIG) || (sig == KSIGKILL) || (sig == KSIGSTOP)) {
        return -EINVAL;
    }

    arg.sig = sig;
    arg.sighand = sighand;
    arg.oldsighand = oldsighand;

    return (int)SYSCALL(SYS_SIGNAL_ACTION, &arg);
}
