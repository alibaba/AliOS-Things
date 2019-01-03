/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include "ulog/ulog.h"
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/wifi.h>

void hal_reboot(void)
{
}

#if defined(DEV_SAL_MK3060)
extern hal_wifi_module_t aos_wifi_module_mk3060;
#endif

void hw_start_hal(void)
{
#if defined(DEV_SAL_MK3060)
    hal_wifi_register_module(&aos_wifi_module_mk3060);
#endif

    hal_wifi_init();
}
