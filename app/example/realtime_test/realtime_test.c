/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "real_time_test.h"

#define FLASH_ACR 0x40022000 

int application_start(int argc, char *argv[])
{
    LOG("benchmark started.");

#if 0
    /*disable cache for developerkit board*/
    uint32_t flash_acr_data;
    flash_acr_data = *(uint32_t *)(FLASH_ACR);
    *(uint32_t *)(FLASH_ACR) = flash_acr_data & (~0x700);
#endif

    rttest_cli_init();
    return 0;
}


