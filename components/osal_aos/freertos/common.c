/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdint.h>
#include "aos/kernel.h"

void aos_srand(uint32_t seed)
{
    srand(seed);
}

int32_t aos_rand(void)
{
    return rand();
}

__attribute__((weak)) void aos_hal_reboot(void)
{
    return;
}
void aos_reboot(void)
{
    aos_hal_reboot();
}
