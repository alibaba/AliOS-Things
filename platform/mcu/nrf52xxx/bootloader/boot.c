/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <stdint.h>

//#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "sdk_errors.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"

#include "nrf_drv_systick.h"

extern void user_app_run(void);


kstat_t krhino_intrpt_enter(void)
{
    return RHINO_SUCCESS;
}

void krhino_intrpt_exit(void)
{

}

extern void ota_app(void);
int main(void)
{
    //hal_uart_init(NULL);
    //printf("Hello Bootloader!\r\n");
    ota_app();
    //printf("Bye Bootloader!\r\n");
    //while (1) {printf("test\r\n");};
    user_app_run();
    return 0;
}

