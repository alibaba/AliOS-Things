/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, demo for GD32F3x0
*/

#include <stdio.h>

#include "k_api.h"

#include "gd32f3x0.h"
#include "gd32f350r_eval.h"

extern k_mm_region_t   g_mm_region[];

void hal_reboot(void)
{
    NVIC_SystemReset();
}

void board_init(void)
{
    memset((uint8_t*)g_mm_region[0].start, 0, g_mm_region[0].len);

    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
}
