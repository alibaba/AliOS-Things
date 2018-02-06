/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>

void soc_err_proc(kstat_t err)
{
   (void)err;
    while(1);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

