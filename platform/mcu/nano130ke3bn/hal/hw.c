/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/base.h>

#include "NuMicro.h"



void hal_reboot(void)
{
    NVIC_SystemReset();
}

#if 0
void hw_start_hal(void)
{
    printf("[%s]\n", __func__);
}
#else
extern k_mm_region_t g_mm_region[1];
void hw_start_hal(void)
{    
#if defined (__CC_ARM) && defined(__MICROLIB)
    printf("[%s] ARMCC\n", __func__ );
#elif defined(__ICCARM__)
    printf("[%s] IAR\n", __func__ );
#else
    printf("[%s] GCC\n", __func__ );
#endif
    printf("Heap start address: 0x%08x\n", g_mm_region[0].start );
    printf("Heap size: %d KB\n", g_mm_region[0].len / 1024 );
    //test_numicro_ota_set_boot();
}
#endif