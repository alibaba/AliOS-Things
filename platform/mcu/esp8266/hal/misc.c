/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "esp_system.h"
#include "k_api.h"

extern void vPortETSIntrLock(void);
extern void aos_msleep(int ms);
extern kstat_t krhino_sched_disable(void);

void hal_reboot(void)
{
    printf("reboot!\n");

    vPortETSIntrLock();
    krhino_sched_disable();

    while(1) {
        system_restart();
        aos_msleep(100);
    }
}
