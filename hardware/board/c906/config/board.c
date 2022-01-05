/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "drv_usart.h"
#include "soc.h"
#include <csi_config.h>
#include <csi_core.h>
#include "pin.h"

#include "aos/init.h"
#include "k_config.h"
#include "board.h"
#include "aos/hal/uart.h"

extern void aos_heap_set();
extern void krhino_tick_proc(void);

extern usart_handle_t console_handle;
extern void ioreuse_initial(void);
extern int clock_timer_init(void);
extern int clock_timer_start(void);

void board_pre_init(void)
{
}


/**
  * @general board init entry board_basic_init
  * @retval None
  */
void board_basic_init(void)
{
    /*mm heap set*/
    aos_heap_set();
}
/**
  * @general board tick init entry board_tick_init
  * @retval None
  */
void board_tick_init(void)
{

}

/**
* @init the default uart init example.
*/
void board_stduart_init(void)
{
    int32_t ret = 0;

    uart_dev_t uart0;
    uart0.port = CONSOLE_IDX;
    ret = hal_uart_init(&uart0);

    if (ret < 0) {
        return;
    }
}

/**
  * Enable DMA controller clock
  */
void board_dma_init(void)
{

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void board_gpio_init(void)
{

}

/**
  * @brief WIFI Initialization Function
  * @param None
  * @retval None
  */
void board_wifi_init(void)
{

}

void board_network_init(void)
{

}

void board_kinit_init(kinit_t* init_args)
{
    return;
}

void board_flash_init(void)
{
}

/**
* @brief This function handles System tick timer.
*/
void systick_handler(void)
{
  krhino_tick_proc();
}

