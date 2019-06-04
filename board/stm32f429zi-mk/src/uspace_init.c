/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_proc.h>

int aos_run_app(void)
{
    int ret;

    ret = k_proc_init();
    if (!ret) {
        k_proc_exec_all();
    }

    return 0;
}


