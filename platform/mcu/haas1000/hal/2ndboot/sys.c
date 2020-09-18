/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "sys.h"

static uint8_t ota_2ndboot_heap[256*1024];

void sys_delayms(volatile int ms)
{
    osDelay(ms);
}

void sys_reboot(void)
{
    hal_cmu_sys_reboot();
}

void *sys_set_heap(unsigned int *size)
{
    memset(ota_2ndboot_heap, 0, sizeof(ota_2ndboot_heap));
    *size = sizeof(ota_2ndboot_heap);
    return ota_2ndboot_heap;
}