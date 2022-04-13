/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BACKTRACE_H
#define BACKTRACE_H

/* printf call stack
   return levels of call stack */
int backtrace_now(char *pPC, int *pSP, int bt_space, int (*print_func)(const char *fmt, ...));

/* printf call stack for task */
void backtrace_task(void *task, int (*print_func)(const char *fmt, ...));

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...));

/* backtrace start with PC SP and LR
   return levels of call stack */
int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...));

/**
 * backtrace in panic
 */
void backtrace_handle(char *PC, int *SP, char *LR, int space_type,
                      int (*print_func)(const char *fmt, ...));

#if (RHINO_CONFIG_USER_SPACE > 0)
/* check backtrace space
 * return 0: kspace
          1: uspace
*/
int check_backtrace_space(void);
#endif

#endif /* BACKTRACE_H */
