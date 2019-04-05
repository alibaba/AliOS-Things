/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_dbg_api.h>

void k_err_proc(kstat_t err)
{
#if (RHINO_CONFIG_PANIC > 0)
    debug_fatal_error(err, __FILE__, __LINE__);
#endif
    if (g_err_proc != NULL) {
        g_err_proc(err);
    }
}

