/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"

#ifdef AOS_COMP_DEBUG
#include "debug_api.h"
#endif

void k_err_proc_debug(kstat_t err, char *file, int line)
{
#ifdef AOS_COMP_DEBUG
    debug_fatal_error(err, file, line);
#endif

    if (g_err_proc != NULL) {
        g_err_proc(err);
    }
}
