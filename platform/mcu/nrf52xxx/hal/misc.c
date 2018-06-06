/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

uint32_t sd_nvic_SystemReset(void);

void hal_reboot(void)
{
    printf("reboot!\n");
    sd_nvic_SystemReset();
}

