/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "esp_system.h"

#include "k_err.h"

extern void vPortETSIntrLock(void);
extern void aos_msleep(int ms);

void hal_reboot(void)
{
    printf("reboot!\n");

    vPortETSIntrLock();
    while(1) {
        system_restart();
        aos_msleep(100);
    }
}
