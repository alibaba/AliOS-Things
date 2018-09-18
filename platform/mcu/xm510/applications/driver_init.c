/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <hal/wifi.h>

extern hal_wifi_module_t aos_wifi_xm510;
void xm_driver_init(void)
{
	sdio_init();
    hal_wifi_register_module(&aos_wifi_xm510);
    hal_wifi_init();
}
