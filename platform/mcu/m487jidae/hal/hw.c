/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include "ulog/ulog.h"

#include "NuMicro.h"



void hal_reboot(void)
{
    SYS_UnlockReg();                   /* Unlock register lock protect */	
    FMC_Open();										/* Enable FMC ISP function */
    FMC_SetVectorPageAddr(0x100000);
    FMC_SET_LDROM_BOOT();
//    SYS_ResetCPU();

    NVIC_SystemReset();
}

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
