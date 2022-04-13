/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"

#ifdef AOS_COMP_DEBUG
    #include "debug_api.h"
#endif

void k_err_proc_debug(kstat_t err, char *file, int line)
{
    kstat_t stat_save;

    stat_save  = g_sys_stat;
    g_sys_stat = RHINO_STOPPED;

#ifdef AOS_COMP_DEBUG
    /* stop the othre core */
    debug_cpu_stop();
    g_crash_steps = 1;

    debug_backtrace_now();
    debug_fatal_error(err, file, line, DEBUG_PANIC_IN_KERNEL);
#endif

    if (g_err_proc != NULL) {
        g_err_proc(err);
    }

    g_sys_stat = stat_save;
}
