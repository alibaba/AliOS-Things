/**
  ******************************************************************************
  * @file    board.c
  * @author  MCU China FAE team
  * @version 1.0
  * @date    05/01/2019
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  ******************************************************************************
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
    int32_t ret = 0;
    /* init the console*/
    clock_timer_init();
    clock_timer_start();

    #if 0
    console_handle = csi_usart_initialize(CONSOLE_IDX, NULL);
    /* config the UART */
    ret = csi_usart_config(console_handle, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
    #endif
    uart_dev_t uart0;
    uart0.port = CONSOLE_IDX;
    ret = hal_uart_init(&uart0);

    if (ret < 0) {
        return;
    }
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
    //flash_partition_init();
}

/**
* @brief This function handles System tick timer.
*/
void systick_handler(void)
{
  //krhino_intrpt_enter();
  krhino_tick_proc();
  //krhino_intrpt_exit();
}

