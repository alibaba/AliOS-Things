/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"
#include "hal.h"
#include "stm32l4xx_hal.h"
#include "hal_uart_stm32l4.h"

/* Init and deInit function for uart1 */
static int32_t lpuart1_init(uart_dev_t *uart);
static int32_t lpuart1_DeInit(void);
static void lpuart1_MspInit(void);
static void lpuart1_DeMspInit(void);
static int32_t uart2_init(uart_dev_t *uart);
static int32_t uart2_DeInit(void);
static void uart2_MspInit(void);
static void uart2_DeMspInit(void);
static int32_t uart3_init(uart_dev_t *uart);
static int32_t uart3_DeInit(void);
static void uart3_MspInit(void);
static void uart3_DeMspInit(void);

/* function used to transform hal para to stm32l4 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, uint32_t *data_width_stm32l4);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal, uint32_t *parity_stm32l4);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, uint32_t *stop_bits_stm32l4);
static int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal, uint32_t *flow_control_stm32l4);
static int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4);

/* handle for uart */
UART_HandleTypeDef lpuart1_handle;
UART_HandleTypeDef uart2_handle;
UART_HandleTypeDef uart3_handle;

/* bufferQueue for uart */
kbuf_queue_t g_buf_queue_lpuart1;
char g_buf_lpuart1[MAX_BUF_UART_BYTES];
kbuf_queue_t g_buf_queue_uart2;
char g_buf_uart2[MAX_BUF_UART_BYTES];
kbuf_queue_t g_buf_queue_uart3;
char g_buf_uart3[MAX_BUF_UART_BYTES];

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case PORT_LPUART1:
            uart->priv = &lpuart1_handle;
            ret = lpuart1_init(uart);
            break;
        case PORT_UART2:
            uart->priv = &uart2_handle;
            ret = uart2_init(uart);
            break;
        case PORT_UART3:
            uart->priv = &uart3_handle;
            ret = uart3_init(uart);
            break;
        /* if ohter uart exist add init code here */

        default:
            break;
    }

        return ret;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = -1;

    if((uart != NULL) && (data != NULL)) {
        ret = UART_WaitOnFlagUntilTimeout((UART_HandleTypeDef *)uart->priv, UART_FLAG_TC, RESET, HAL_GetTick(), krhino_ms_to_ticks(timeout));
        if (ret == 0)
            ret = HAL_UART_Transmit_IT((UART_HandleTypeDef *)uart->priv, (uint8_t *)data, size);
    }

    return ret;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    uint32_t start_tick = 0;
    uint32_t timeout_tick = 0;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    timeout_tick = krhino_ms_to_ticks(timeout);
    start_tick = HAL_GetTick();
    for (i = 0; i < expect_size; i++)
    {
        do {
            ret = HAL_UART_Receive_IT_Buf_Queue_1byte((UART_HandleTypeDef *)uart->priv, &pdata[i]);
            if (ret == 0) {
                break;
            }
            //krhino_task_sleep(1);
        } while (HAL_GetTick() - start_tick < timeout_tick);
        if (ret == 0) {
            rx_count++;
        } else {
            break;
        }
    }

    *recv_size = rx_count;

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case PORT_LPUART1:
              ret = lpuart1_DeInit();
            break;
        case PORT_UART2:
              ret = uart2_DeInit();
            break;
        case PORT_UART3:
            ret = uart3_DeInit();
            break;
        /* if other uart exist add Deinit code here */

        default:
            break;
    }

    return ret;
}

int32_t lpuart1_init(uart_dev_t *uart)
{
    int32_t ret = 0;

    lpuart1_handle.Instance                    = LPUART1;
    lpuart1_handle.Init.BaudRate               = uart->config.baud_rate;

    ret = uart_dataWidth_transform(uart->config.data_width, &lpuart1_handle.Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, &lpuart1_handle.Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &lpuart1_handle.Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, &lpuart1_handle.Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, &lpuart1_handle.Init.Mode);

    if (ret != 0) {
        return -1;
    }

    lpuart1_handle.Init.HwFlowCtl              = LPUART1_HW_FLOW_CTL;
    lpuart1_handle.Init.OneBitSampling         = LPUART1_ONE_BIT_SAMPLING;
    lpuart1_handle.AdvancedInit.AdvFeatureInit = LPUART1_ADV_FEATURE_INIT;
    lpuart1_handle.buffer_queue = &g_buf_queue_lpuart1;

    /* init uart */
    lpuart1_MspInit();
    HAL_UART_Init(&lpuart1_handle);

    ret = krhino_buf_queue_create(&g_buf_queue_lpuart1, "buf_queue_uart",
          g_buf_lpuart1, MAX_BUF_UART_BYTES, 1);

    return ret;
}

int32_t lpuart1_DeInit(void)
{
    int32_t ret = -1;

    /* uart deinitialization */
    ret = HAL_UART_DeInit(&lpuart1_handle);
    lpuart1_DeMspInit();

    return ret;
}

void lpuart1_MspInit(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable GPIO clock */
    LPUART1_TX_GPIO_CLK_ENABLE();
    LPUART1_RX_GPIO_CLK_ENABLE();

    /* Enable USART clock */
    LPUART1_CLK_ENABLE();

    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin       = LPUART1_TX_PIN;
    gpio_init_structure.Mode      = LPUART1_TX_MODE;
    gpio_init_structure.Speed     = LPUART1_TX_SPEED;
    gpio_init_structure.Pull      = LPUART1_TX_PULL;
    gpio_init_structure.Alternate = LPUART1_TX_ALTERNATE;
    HAL_GPIO_Init(LPUART1_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin       = LPUART1_RX_PIN;
    gpio_init_structure.Mode      = LPUART1_RX_MODE;
    gpio_init_structure.Alternate = LPUART1_RX_ALTERNATE;
    HAL_GPIO_Init(LPUART1_RX_GPIO_PORT, &gpio_init_structure);

    HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(LPUART1_IRQn);
}

void lpuart1_DeMspInit(void)
{
    /* Disable USART clock */
    LPUART1_CLK_DISABLE();

    /* USART TX/RX pins deinitializations */
    LPUART1_TX_GPIO_CLK_DISABLE();
    LPUART1_RX_GPIO_CLK_DISABLE();
}

int32_t uart2_init(uart_dev_t *uart)
{
    int32_t ret = 0;

    uart2_handle.Instance                    = UART2;
    uart2_handle.Init.BaudRate               = uart->config.baud_rate;

    ret = uart_dataWidth_transform(uart->config.data_width, &uart2_handle.Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, &uart2_handle.Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &uart2_handle.Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, &uart2_handle.Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, &uart2_handle.Init.Mode);

    if (ret != 0) {
        return -1;
    }

    uart2_handle.Init.HwFlowCtl              = UART2_HW_FLOW_CTL;
    uart2_handle.Init.OverSampling           = UART2_OVER_SAMPLING;
    uart2_handle.Init.OneBitSampling         = UART2_ONE_BIT_SAMPLING;
    uart2_handle.AdvancedInit.AdvFeatureInit = UART2_ADV_FEATURE_INIT;
    uart2_handle.buffer_queue = &g_buf_queue_uart2;

    /* init uart */
    uart2_MspInit();
    HAL_UART_Init(&uart2_handle);

    ret = krhino_buf_queue_create(&g_buf_queue_uart2, "buf_queue_uart",
        g_buf_uart2, MAX_BUF_UART_BYTES, 1);

    return ret;
}

int32_t uart2_DeInit(void)
{
    int32_t ret = -1;

    /* uart deinitialization */
    ret = HAL_UART_DeInit(&uart2_handle);
    uart2_DeMspInit();

    return ret;
}

void uart2_MspInit(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable GPIO clock */
    UART2_TX_GPIO_CLK_ENABLE();
    UART2_RX_GPIO_CLK_ENABLE();

    /* Enable USART clock */
    UART2_CLK_ENABLE();

    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin       = UART2_TX_PIN;
    gpio_init_structure.Mode      = UART2_TX_MODE;
    gpio_init_structure.Speed     = UART2_TX_SPEED;
    gpio_init_structure.Pull      = UART2_TX_PULL;
    gpio_init_structure.Alternate = UART2_TX_ALTERNATE;
    HAL_GPIO_Init(UART2_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin       = UART2_RX_PIN;
    gpio_init_structure.Mode      = UART2_RX_MODE;
    gpio_init_structure.Alternate = UART2_RX_ALTERNATE;
    HAL_GPIO_Init(UART2_RX_GPIO_PORT, &gpio_init_structure);

    HAL_NVIC_SetPriority(UART2_IRQn, 0,1);
    HAL_NVIC_EnableIRQ(UART2_IRQn);
}

void uart2_DeMspInit(void)
{
    /* Disable USART clock */
    UART2_CLK_DISABLE();

    /* USART TX/RX pins deinitializations */
    UART2_TX_GPIO_CLK_DISABLE();
    UART2_RX_GPIO_CLK_DISABLE();
}

int32_t uart3_init(uart_dev_t *uart)
{
    int32_t ret = 0;

    uart3_handle.Instance                    = UART3;
    uart3_handle.Init.BaudRate               = uart->config.baud_rate;

    ret = uart_dataWidth_transform(uart->config.data_width, &uart3_handle.Init.WordLength);
    ret |= uart_parity_transform(uart->config.parity, &uart3_handle.Init.Parity);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &uart3_handle.Init.StopBits);
    ret |= uart_flow_control_transform(uart->config.flow_control, &uart3_handle.Init.HwFlowCtl);
    ret |= uart_mode_transform(uart->config.mode, &uart3_handle.Init.Mode);

    if (ret != 0) {
        return -1;
    }

    uart3_handle.Init.HwFlowCtl              = UART3_HW_FLOW_CTL;
    uart3_handle.Init.OverSampling           = UART3_OVER_SAMPLING;
    uart3_handle.Init.OneBitSampling         = UART3_ONE_BIT_SAMPLING;
    uart3_handle.AdvancedInit.AdvFeatureInit = UART3_ADV_FEATURE_INIT;
    uart3_handle.buffer_queue = &g_buf_queue_uart2;

    /* init uart */
    uart3_MspInit();
    HAL_UART_Init(&uart3_handle);

    ret = krhino_buf_queue_create(&g_buf_queue_uart3, "buf_queue_uart",
        g_buf_uart3, MAX_BUF_UART_BYTES, 1);

    return ret;
}

int32_t uart3_DeInit(void)
{
    int32_t ret = -1;

    /* uart deinitialization */
    ret = HAL_UART_DeInit(&uart3_handle);
    uart3_DeMspInit();

    return ret;
}

void uart3_MspInit(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable GPIO clock */
    UART3_TX_GPIO_CLK_ENABLE();
    UART3_RX_GPIO_CLK_ENABLE();

    /* Enable USART clock */
    UART3_CLK_ENABLE();

    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin       = UART3_TX_PIN;
    gpio_init_structure.Mode      = UART3_TX_MODE;
    gpio_init_structure.Speed     = UART3_TX_SPEED;
    gpio_init_structure.Pull      = UART3_TX_PULL;
    gpio_init_structure.Alternate = UART3_TX_ALTERNATE;
    HAL_GPIO_Init(UART3_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin       = UART3_RX_PIN;
    gpio_init_structure.Mode      = UART3_RX_MODE;
    gpio_init_structure.Alternate = UART3_RX_ALTERNATE;
    HAL_GPIO_Init(UART3_RX_GPIO_PORT, &gpio_init_structure);

    HAL_NVIC_SetPriority(UART3_IRQn, 0,1);
    HAL_NVIC_EnableIRQ(UART3_IRQn);
}

void uart3_DeMspInit(void)
{
    /* Disable USART clock */
    UART3_CLK_DISABLE();

    /* USART TX/RX pins deinitializations */
    UART3_TX_GPIO_CLK_DISABLE();
    UART3_RX_GPIO_CLK_DISABLE();
}

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
        uint32_t *data_width_stm32l4)
{
    uint32_t data_width = 0;
    int32_t    ret = 0;

    if(data_width_hal == DATA_WIDTH_7BIT)
    {
        data_width = UART_WORDLENGTH_7B;
    }
    else if(data_width_hal == DATA_WIDTH_8BIT)
    {
        data_width = UART_WORDLENGTH_8B;
    }
    else if(data_width_hal == DATA_WIDTH_9BIT)
    {
        data_width = UART_WORDLENGTH_9B;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *data_width_stm32l4 = data_width;
    }

    return ret;
}

int32_t uart_parity_transform(hal_uart_parity_t parity_hal,
        uint32_t *parity_stm32l4)
{
    uint32_t parity = 0;
    int32_t    ret = 0;

    if(parity_hal == NO_PARITY)
    {
        parity = UART_PARITY_NONE;
    }
    else if(parity_hal == ODD_PARITY)
    {
        parity = UART_PARITY_EVEN;
    }
    else if(parity_hal == EVEN_PARITY)
    {
        parity = UART_PARITY_ODD;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *parity_stm32l4 = parity;
    }

    return ret;
}

int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal,
        uint32_t *stop_bits_stm32l4)
{
    uint32_t stop_bits = 0;
    int32_t    ret = 0;

    if(stop_bits_hal == STOP_BITS_1)
    {
        stop_bits = UART_STOPBITS_1;
    }
    else if(stop_bits_hal == STOP_BITS_2)
    {
        stop_bits = UART_STOPBITS_2;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *stop_bits_stm32l4 = stop_bits;
    }

    return ret;
}

int32_t uart_flow_control_transform(hal_uart_flow_control_t flow_control_hal,
        uint32_t *flow_control_stm32l4)
{
    uint32_t flow_control = 0;
    int32_t    ret = 0;

    if(flow_control_hal == FLOW_CONTROL_DISABLED)
    {
        flow_control = UART_HWCONTROL_NONE;
    }
    else if(flow_control_hal == FLOW_CONTROL_CTS)
    {
        flow_control = UART_HWCONTROL_CTS;
    }
    else if(flow_control_hal == FLOW_CONTROL_RTS)
    {
        flow_control = UART_HWCONTROL_RTS;
    }
    else if(flow_control_hal == FLOW_CONTROL_RTS)
    {
        flow_control = UART_HWCONTROL_RTS_CTS;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *flow_control_stm32l4 = flow_control;
    }

    return ret;
}

int32_t uart_mode_transform(hal_uart_mode_t mode_hal, uint32_t *mode_stm32l4)
{
    uint32_t mode = 0;
    int32_t    ret = 0;

    if(mode_hal == MODE_TX)
    {
        mode = UART_MODE_TX;
    }
    else if(mode_hal == MODE_RX)
    {
        mode = UART_MODE_RX;
    }
    else if(mode_hal == MODE_TX_RX)
    {
        mode = UART_MODE_TX_RX;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *mode_stm32l4 = mode;
    }

    return ret;
}
