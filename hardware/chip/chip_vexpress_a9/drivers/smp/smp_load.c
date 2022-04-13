/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include "k_api.h"
#include "aos/kernel.h"

extern void sencond_cpu_bootup(void);

void os_load_slavecpu(void)
{
    k_dcache_clean_all();

    sencond_cpu_bootup();

    k_wait_allcores();
}

