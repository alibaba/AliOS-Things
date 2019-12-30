/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_BACKTRACE_H
#define DEBUG_BACKTRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* show backtrace now */
void debug_backtrace_now(void);

/* show backtrace for the task */
void debug_backtrace_task(char *taskname);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_BACKTRACE_H */
