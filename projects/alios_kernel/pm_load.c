/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "k_compiler.h"
#include <aos/kernel.h>
#include "aos/hal/flash.h"
#include "k_proc.h"

#define LOAD_PM_FROM_ROMFS

#define PM_PATH  "/usr/procmgmt.app"
static uint32_t g_pm_pid = 1;

void aos_load_pm()
{
#ifdef LOAD_PM_FROM_ROMFS
    g_pm_pid = k_proc_load(PM_PATH, NULL);
#endif
}

int32_t aos_get_pm_pid(void)
{
    return (int32_t)g_pm_pid;
}

