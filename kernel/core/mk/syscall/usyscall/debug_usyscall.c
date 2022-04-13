/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "syscall_no.h"
#include "syscall.h"
#include "debug_syscall_arg.h"
#include "debug_api.h"

int backtrace_now(char *pPC, int *pSP, int bt_space, int (*print_func)(const char *fmt, ...))
{
    backtrace_now_syscall_arg_t _arg;

    _arg.pc         = pPC;
    _arg.sp         = pSP;
    _arg.bt_space   = bt_space;
    _arg.print_func = print_func;

    return SYSCALL(SYS_BACKTRACE_NOW, (void *)&_arg);
}

void debug_fatal_error(kstat_t err, char *file, int line, int inuser)
{
    debug_fatal_error_syscall_arg_t _arg;

    _arg.err      = err;
    _arg.file     = file;
    _arg.line     = line;
    _arg.inuser   = inuser;

    SYSCALL(SYS_DEBUG_FATAL_ERROR, (void *)&_arg);
}

void debug_cpu_stop(void)
{
    SYSCALL(SYS_DEBUG_CPU_STOP, NULL);
}

void debug_set_pid_umm_head(void *addr)
{
    debug_umm_syscall_arg_t _arg;

    _arg.mm_head_addr       = addr;

    SYSCALL(SYS_DEBUG_MM_SET_HEAD, (void *)&_arg);
}

