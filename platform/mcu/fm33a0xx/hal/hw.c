/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "FM33A0XX.h"

void hal_reboot(void)
{
    aos_msleep(10);
    NVIC_SystemReset();
}

