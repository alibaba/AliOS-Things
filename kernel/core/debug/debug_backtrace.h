/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_BACKTRACE_H
#define DEBUG_BACKTRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#define BACKTRACE_KSPACE 0
#define BACKTRACE_USPACE 1

/* show backtrace now */
void debug_backtrace_now(void);

/* show backtrace in panic handler */
void debug_panic_backtrace(char *PC, int *SP, char *LR,
                           int (*print_func)(const char *fmt, ...));

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_BACKTRACE_H */
