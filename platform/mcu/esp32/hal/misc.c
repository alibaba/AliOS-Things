/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

extern void esp_restart();

void hal_reboot(void)
{
    printf("reboot!\n");
    esp_restart();
}

