/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */


#include <aos/kernel.h>
#include "board.h"
#include "aos/hal/uart.h"
#include <aos/init.h>

static uart_dev_t uart_0;
void board_stduart_init()
{
    uart_0.port                = MICO_UART_1;
    uart_0.config.baud_rate    = 115200;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
}

void board_dma_init()
{

}

void board_gpio_init()
{

}

void board_tick_init()
{

}

void board_kinit_init(kinit_t* init_args)
{
    /* Use default args in demos.*/
}

void board_flash_init()
{

}

void board_network_init()
{

}