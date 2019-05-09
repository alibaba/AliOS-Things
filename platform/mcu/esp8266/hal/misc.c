/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "esp_system.h"

#include "k_api.h"

extern void vPortETSIntrLock(void);
extern void aos_msleep(int ms);
extern kstat_t krhino_sched_disable(void);

unsigned int delay_i = 0;
static void delay()
{
    for(delay_i = 0; delay_i < 100000; delay_i++) {

    }
}

void hal_reboot(void)
{
    printf("reboot!\n");

    vPortETSIntrLock();
    krhino_sched_disable();
    delay();
    delay();
    rom_i2c_writeReg(0x67, 4, 1, 8);
    rom_i2c_writeReg(0x67, 4, 2, 0x81);
    while(1) {
        system_restart();
        aos_msleep(100);
    }
}
