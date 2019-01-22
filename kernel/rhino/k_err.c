/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>

void k_err_proc_debug(kstat_t err, char *file, int line)
{
#if (DEBUG_CONFIG_ERRDUMP > 0)
    debug_fatal_error(kstat_t err, char *file, int line);
#endif

    if (g_err_proc != NULL) {
        g_err_proc(err);
    }
}
