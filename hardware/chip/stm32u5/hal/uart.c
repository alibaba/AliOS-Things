/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include "k_api.h"
#include "aos/hal/uart.h"
#include "k_config.h"
#include "board.h"
#include "stm32u5xx_hal.h"

extern UART_HandleTypeDef huart1;
#define MAX_BUF_UART_BYTES   128
kbuf_queue_t g_buf_queue_uart;
char g_buf_uart[MAX_BUF_UART_BYTES];

int32_t hal_uart_init(uart_dev_t *uart)
{
    kstat_t err_code;
    uart->priv = &huart1;
    err_code = krhino_buf_queue_create(&g_buf_queue_uart, "buf_queue_uart",
        g_buf_uart, MAX_BUF_UART_BYTES, 1);
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i;
    uint8_t byte;
    uint32_t ret = 0;
    for(i=0;i<size;i++)
    {
        byte = *((uint8_t *)data + i);
        HAL_UART_Transmit(&huart1, (uint8_t *)&byte, 1, 0xFFFF);
    }
    return ret;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    // int32_t ret = -1;

    // if((uart != NULL) && (data != NULL)) {
    //     ret = HAL_UART_Receive_IT((UART_HandleTypeDef *)uart->priv,
    //           (uint8_t *)data, *recv_size);
    // }
    // return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{

}