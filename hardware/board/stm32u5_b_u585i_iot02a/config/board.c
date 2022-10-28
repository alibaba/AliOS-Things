/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "board.h"

extern void aos_heap_set();
extern void krhino_tick_proc(void);

extern void ioreuse_initial(void);
extern int clock_timer_init(void);
extern int clock_timer_start(void);

extern UART_HandleTypeDef huart1;

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
    // huart1.Instance = USART1;
    // huart1.Init.BaudRate = 115200;
    // huart1.Init.WordLength = UART_WORDLENGTH_8B;
    // huart1.Init.StopBits = UART_STOPBITS_1;
    // huart1.Init.Parity = UART_PARITY_NONE;
    // huart1.Init.Mode = UART_MODE_TX_RX;
    // huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    // huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    // huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    // huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    // huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    // if (HAL_UART_Init(&huart1) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
    // {
    //     Error_Handler();
    // }
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
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

