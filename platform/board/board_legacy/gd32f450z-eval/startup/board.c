/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, demo for GD32F3x0
*/

#include <stdio.h>

#include "k_api.h"

#include "gd32f4xx.h"
#include "gd32f450z_eval.h"

extern k_mm_region_t   g_mm_region[];

void hal_reboot(void)
{
    NVIC_SystemReset();
}

void board_init(void)
{
    memset((uint8_t*)g_mm_region[0].start, 0, g_mm_region[0].len);

    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
}
