/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_TRACE_H
#define DEBUG_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* sysacll trace */
void debug_trace_syscall_hook(uint32_t syscall_no);

void debug_trace_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_TRACE_H */
