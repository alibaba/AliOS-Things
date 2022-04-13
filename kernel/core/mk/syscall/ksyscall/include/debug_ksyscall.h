/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef DEBUG_KSYSCALL_H
#define DEBUG_KSYSCALL_H

#include <stdint.h>

int sys_backtrace_now_stub(void *arg);

int sys_backtrace_now_get_stub(void *arg);

void sys_debug_fatal_error_stub(void *arg);

void sys_debug_clear_watchpoint(int i);

int sys_debug_install_watchpoint(int i, unsigned int addr);

void sys_enable_monitor_mode(void *arg);

void sys_debug_cpu_stop(void *arg);

void sys_debug_error_to_cli(void *arg);

void sys_debug_mm_set_head(void *arg);

#endif /* DEBUG_KSYSCALL_H */
