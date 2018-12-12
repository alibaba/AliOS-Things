/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/uart.h"
#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>

/* Board Header files */
#include "Board.h"

/* Driver Header files */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432.h>




extern char *g_pc_buf_queue_uart[];
extern kbuf_queue_t g_buf_queue_uart[];
extern aos_mutex_t uart_tx_mutex[];
extern aos_mutex_t uart_rx_mutex[];
extern UART_Handle uart_handle[];

/* function used to transform hal para to stm32l4 para */
static int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal, uint32_t *data_width_stm32l4);
static int32_t uart_parity_transform(hal_uart_parity_t parity_hal, uint32_t *parity_stm32l4);
static int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal, uint32_t *stop_bits_stm32l4);

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = 0;
    UART_Handle uartHandle;
    UART_Params uartParams;

    if (uart == NULL) {
        return -1;
    }

    UART_Params_init(&uartParams);

    uartParams.baudRate = uart->config.baud_rate;
    ret = uart_dataWidth_transform(uart->config.data_width, &uartParams.dataLength);
    ret |= uart_parity_transform(uart->config.parity, &uartParams.parityType);
    ret |= uart_stop_bits_transform(uart->config.stop_bits, &uartParams.stopBits);
    if (ret) {
        printf("invalid uart data \r\n");
        goto err;
    }

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    
    if(NULL == g_pc_buf_queue_uart[uart->port]){
        g_pc_buf_queue_uart[uart->port] = aos_malloc(MAX_BUF_UART_BYTES);
    }
    
    if (NULL == g_pc_buf_queue_uart[uart->port]) {
        printf("fail to malloc memory size %d at %s %d \r\d", MAX_BUF_UART_BYTES, __FILE__, __LINE__);
        goto err;
    }

    memset(g_pc_buf_queue_uart[uart->port], 0, MAX_BUF_UART_BYTES);

    ret = krhino_buf_queue_create(&g_buf_queue_uart[uart->port], "buf_queue_uart",
          g_pc_buf_queue_uart[uart->port], MAX_BUF_UART_BYTES, 1);
    if (ret) {
         goto err;
    }

    switch (uart->port) {
        case PORT_UART_STD:
            uart->priv = UART_open(Board_UART0, &uartParams);
        break;
        case PORT_UART_AT:
            uart->priv = UART_open(Board_UART1, &uartParams);
        break;
        default :
            printf("uart %d invalid\r\n", uart->port);
            goto err;
    }

    if (NULL == uart->priv) {
        goto err;
    }
    uart_handle[uart->port] = uart->priv;
    aos_mutex_new(&uart_tx_mutex[uart->port]);
    aos_mutex_new(&uart_rx_mutex[uart->port]);
    return ret;

err:
    if (NULL != g_pc_buf_queue_uart[uart->port]){
        aos_free(g_pc_buf_queue_uart);
        g_pc_buf_queue_uart[uart->port] = NULL;
    }

    krhino_buf_queue_del(&g_buf_queue_uart[uart->port]);

    return ret;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    UART_Handle uartHandle;

    if(uart == NULL|| data == NULL) {
      return -EINVAL;
    }
    
    uartHandle = uart_handle[uart->port];
    aos_mutex_lock(&uart_tx_mutex[uart->port], AOS_WAIT_FOREVER);

    UART_write(uartHandle, data, size);

    aos_mutex_unlock(&uart_tx_mutex[uart->port]);

    return 0;
}


int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    UART_Handle uartHandle;
    UARTMSP432_Object *object;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    uartHandle = uart_handle[uart->port];
    if (uartHandle == NULL) {
        return -1;
    }

    aos_mutex_lock(&uart_rx_mutex[uart->port], AOS_WAIT_FOREVER);

    object = (UARTMSP432_Object *)uartHandle->object;
    if (object->bufqueue) {
        if (timeout == 0 || timeout == AOS_WAIT_FOREVER) {
            ret = krhino_buf_queue_recv(object->bufqueue, RHINO_WAIT_FOREVER, pdata, &rx_count);
        } else {
            ret = krhino_buf_queue_recv(object->bufqueue, krhino_ms_to_ticks(timeout), pdata, &rx_count);
        }
    }

    if (recv_size != NULL)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    aos_mutex_unlock(&uart_rx_mutex[uart->port]);

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = -1;
    UART_Handle uartHandle;

    if (uart == NULL) {
        return -1;
    }

    uartHandle = uart_handle[uart->port];
    if (uartHandle == NULL) {
        return -1;
    }

    ret = UARTMSP432_close(uartHandle);
    if(NULL != g_pc_buf_queue_uart[uart->port]){
        free(g_pc_buf_queue_uart[uart->port]);
        g_pc_buf_queue_uart[uart->port] = NULL;
    }

    krhino_buf_queue_del(&g_buf_queue_uart[uart->port]);

    if (aos_mutex_is_valid(&uart_tx_mutex[uart->port])) {
        aos_mutex_free(&uart_tx_mutex[uart->port]);
    }

    if (aos_mutex_is_valid(&uart_tx_mutex[uart->port])) {
        aos_mutex_free(&uart_tx_mutex[uart->port]);
    }

    return ret;
}

int32_t uart_dataWidth_transform(hal_uart_data_width_t data_width_hal,
        uint32_t *data_width_msp432p4)
{
    uint32_t data_width = 0;
    int32_t ret = 0;

    if(data_width_hal == DATA_WIDTH_7BIT)
    {
        data_width = UART_LEN_7;
    }
    else if(data_width_hal == DATA_WIDTH_8BIT)
    {
        data_width = UART_LEN_8;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *data_width_msp432p4 = data_width;
    }

    return ret;
}

int32_t uart_parity_transform(hal_uart_parity_t parity_hal,
        uint32_t *parity_msp432p4)
{
    uint32_t parity = 0;
    int32_t ret = 0;

    if(parity_hal == NO_PARITY)
    {
        parity = UART_PAR_NONE;
    }
    else if(parity_hal == ODD_PARITY)
    {
        parity = UART_PAR_ODD;
    }
    else if(parity_hal == EVEN_PARITY)
    {
        parity = UART_PAR_EVEN;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *parity_msp432p4 = parity;
    }

    return ret;
}

int32_t uart_stop_bits_transform(hal_uart_stop_bits_t stop_bits_hal,
        uint32_t *stop_bits_msp432p4)
{
    uint32_t stop_bits = 0;
    int32_t ret = 0;

    if(stop_bits_hal == STOP_BITS_1)
    {
        stop_bits = UART_STOP_ONE;
    }
    else if(stop_bits_hal == STOP_BITS_2)
    {
        stop_bits = UART_STOP_TWO;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *stop_bits_msp432p4 = stop_bits;
    }

    return ret;
}

