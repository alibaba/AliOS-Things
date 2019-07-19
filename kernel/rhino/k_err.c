/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"

void k_err_proc_debug(kstat_t err, char *file, int line)
{
#if (RHINO_CONFIG_ERR_DUMP > 0)
    debug_fatal_error(err, file, line);
#endif

    if (g_err_proc != NULL) {
        g_err_proc(err);
    }
}
