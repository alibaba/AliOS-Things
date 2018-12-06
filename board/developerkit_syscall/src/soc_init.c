/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/hal.h"
#include "k_config.h"

uart_dev_t uart_0;

static void stduart_init(void)
{
    uart_0.port = 0;
    uart_0.config.baud_rate = 115200;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.mode = MODE_TX_RX;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;

    hal_uart_init(&uart_0);
}

void stm32_soc_init(void)
{
    int freq;

    HAL_Init();

    //SystemClock_Config();

    freq = HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND;

    HAL_SYSTICK_Config(freq);

    stduart_init();
}

void SysTick_Handler(void)
{
    krhino_intrpt_enter();

    HAL_IncTick();
    krhino_tick_proc();

    krhino_intrpt_exit();
}
