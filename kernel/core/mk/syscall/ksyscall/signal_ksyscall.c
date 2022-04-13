/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <aos/signal.h>
#include <signal_syscall_arg.h>

int sys_aos_kill_stub(void *arg)
{
    aos_kill_syscall_arg_t *_arg = arg;
    return aos_kill(_arg->pid, _arg->sig);
}

int sys_aos_tkill_stub(void *arg)
{
    aos_tkill_syscall_arg_t *_arg = arg;
    return aos_tkill(_arg->task, _arg->sig);
}

int sys_aos_sigpending_stub(void *arg)
{
    ksigset_t *_arg = arg;
    return aos_sigpending(_arg);
}

int sys_aos_sigprocmask_stub(void *arg)
{
    aos_sigprocmask_syscall_arg_t *_arg = arg;
    return aos_sigprocmask(_arg->how, _arg->set, _arg->old);
}

int sys_aos_sigaction_stub(void *arg)
{
    aos_sigaction_syscall_arg_t *_arg = arg;
    return aos_sigaction(_arg->sig, _arg->sighand, _arg->oldsighand);
}
