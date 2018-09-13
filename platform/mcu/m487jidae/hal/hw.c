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
    SYS_UnlockReg();                   /* Unlock register lock protect */	
	FMC_Open();										/* Enable FMC ISP function */
    FMC_SetVectorPageAddr(0x100000);
    FMC_SET_LDROM_BOOT();
//    SYS_ResetCPU();

    NVIC_SystemReset();
}

void hw_start_hal(void)
{
    printf("[%s]\n", __func__ );
    //test_numicro_ota_set_boot();
}
