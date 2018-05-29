/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f7xx_hal.h"
#include "soc_init.h"
#include "hal/hal.h"
#include "stm32f7xx_hal.h"
#include "hal_uart_stm32f7.h"


UART_HandleTypeDef huart1;


/* USART1 init function */
static int32_t uart1_init(uart_dev_t *uart)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
    return -1;
  }
  return 0;

}


int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case PORT_UART1:
            uart->priv = &huart1;
            ret = uart1_init(uart);
            break;
        case PORT_UART2:
        break;
        /* if ohter uart exist add init code here */
        default:
        break;
    }

    return ret;
}



int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }
    return HAL_UART_Transmit((UART_HandleTypeDef *)uart->priv, (uint8_t *)data, size, 30000);
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    /*
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = HAL_UART_Receive_IT_Buf_Queue_1byte((UART_HandleTypeDef *)uart->priv, &pdata[i], timeout); 
        if (ret == 0) {
            rx_count++;
        } else {
            break;
        }
    }

    if (recv_size)
        *recv_size = rx_count;

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;*/
}
