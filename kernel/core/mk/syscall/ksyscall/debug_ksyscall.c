/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <limits.h>
#include "debug_api.h"
#include "debug_syscall_arg.h"

int sys_backtrace_now_stub(void *arg)
{
    backtrace_now_syscall_arg_t *_arg = arg;

    return backtrace_now(_arg->pc, _arg->sp, _arg->bt_space, _arg->print_func);
}

void sys_debug_fatal_error_stub(void *arg)
{
    debug_fatal_error_syscall_arg_t *_arg = arg;

    debug_fatal_error(_arg->err, _arg->file, _arg->line, _arg->inuser);
}

void sys_debug_cpu_stop(void *arg)
{
    debug_cpu_stop();
}

void sys_debug_mm_set_head(void *arg)
{
    debug_umm_syscall_arg_t *umm_arg = arg;

    debug_set_pid_umm_head(umm_arg->mm_head_addr);
}

