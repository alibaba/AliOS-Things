/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
void k_err_proc_debug(kstat_t err, char *file, int line)
{
    if (g_err_proc != NULL) {

#if (DEBUG_CONFIG_NORMAL_PRT > 1)
        extern int printf(const char *fmt, ...);
        printf("\r\nError ! no:%d,file:%s,line:%d\r\n", err, file, line);
#endif

#if (DEBUG_CONFIG_BACKTRACE > 0)
        backtrace_now(NULL);
#endif
        g_err_proc(err);
    }
}
