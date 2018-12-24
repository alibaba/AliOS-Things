/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, demo for GD32F3x0
*/

#include <stdio.h>

#include "k_api.h"

#include "aos/hal/uart.h"

#include "gd32f4xx.h"
#include "gd32f450z_eval.h"

uart_dev_t  uart_0;
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

    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    SysTick_Config(SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);

    uart_0.port = 0;
    uart_0.config.baud_rate = 115200;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;
    hal_uart_init(&uart_0);
}
