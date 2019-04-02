/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_BACKTRACE_H
#define K_BACKTRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#if (RHINO_CONFIG_BACKTRACE > 0)

    /* show backtrace now */
    void krhino_backtrace_now(void);

    /* show backtrace for the task */
    void krhino_backtrace_task(char *taskname);

#endif

#ifdef __cplusplus
}
#endif

#endif /* K_BACKTRACE_H */
