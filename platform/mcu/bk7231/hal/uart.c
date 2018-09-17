/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "ringbuf.h"
#include "rtos_pub.h"
#include "error.h"

#include "drv_model_pub.h"
#include "uart_pub.h"

#define MAX_UART_NUM 2
#define UART_FIFO_SIZE 256

enum _uart_status_e
{
    _UART_STATUS_CLOSED = 0,
    _UART_STATUS_OPENED,
};

typedef struct
{
    uint8_t             uart;
    uint8_t             status;
    uint8_t             *rx_buf;
    uint32_t            rx_size;
    ring_buffer_t       rx_ringbuf;
    beken_semaphore_t    rx_semphr;
    beken_semaphore_t    tx_semphr;
    beken_mutex_t        tx_mutex;
} _uart_drv_t;

static _uart_drv_t _uart_drv[MAX_UART_NUM];
void uart_rx_cb(uint8_t port);
void uart_tx_cb(uint8_t port);

uint8_t uart_port_convert(uint8_t port) {
    uint8_t ret;

    if (2 == port)
        ret = UART1_PORT;  //atcmd
    else
        ret = UART2_PORT;  //stdio

    return ret;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    _uart_drv_t *pdrv;
    UINT32 status;
    DD_HANDLE uart_hdl;
    uint8_t port;

    port = uart_port_convert(uart->port);
    pdrv = &_uart_drv[port];

    if(pdrv->status == _UART_STATUS_CLOSED)
    {
        pdrv->rx_buf = (uint8_t *)malloc(UART_FIFO_SIZE);
        ring_buffer_init(&pdrv->rx_ringbuf, pdrv->rx_buf, UART_FIFO_SIZE);

        rtos_init_semaphore( &pdrv->tx_semphr, 0 );
        rtos_init_semaphore( &pdrv->rx_semphr, 0 );
        rtos_init_mutex( &pdrv->tx_mutex );

		bk_uart_cb.uart_rx_cb = uart_rx_cb;
		bk_uart_cb.uart_tx_cb = uart_tx_cb;
        pdrv->status = _UART_STATUS_OPENED;
    }
	
    if(port == UART1_PORT)
    {
        uart_hdl = ddev_open(UART1_DEV_NAME, &status, 0);
    }
    else
    {
        uart_hdl = ddev_open(UART2_DEV_NAME, &status, 0);
    }

    ddev_control(uart_hdl, CMD_UART_INIT, (void *)&uart->config);

	ddev_close(uart_hdl);

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    _uart_drv_t *pdrv;
    uint8_t port;

    port = uart_port_convert(uart->port);
    pdrv = &_uart_drv[port];

    ring_buffer_deinit(&pdrv->rx_ringbuf);
    free(pdrv->rx_buf);

    rtos_deinit_semaphore(&pdrv->rx_semphr);
    rtos_deinit_semaphore(&pdrv->tx_semphr);
    rtos_deinit_mutex(&pdrv->tx_mutex);

    pdrv->status = _UART_STATUS_CLOSED;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i = 0;
    (void)timeout;

    _uart_drv_t *pdrv;
    UINT32 status, set;
    DD_HANDLE uart_hdl;
    uint8_t port;

    port = uart_port_convert(uart->port);
    pdrv = &_uart_drv[port];

    rtos_lock_mutex( &pdrv->tx_mutex );

    if(port == UART1_PORT)
        uart_hdl = ddev_open(UART1_DEV_NAME, &status, 0);
    else
        uart_hdl = ddev_open(UART2_DEV_NAME, &status, 0);
    
    for( i = 0; i < size; i++ )
    {
        if( uart_is_tx_fifo_full(port) )
        {
			set = 1;
            ddev_control(uart_hdl, CMD_SET_STOP_END, &set);
            /* The data in Tx FIFO may have been sent out before enable TX_STOP_END interrupt */
            /* So double check the FIFO status */
            while( !uart_is_tx_fifo_empty(port) )
                rtos_get_semaphore( &pdrv->tx_semphr, 50 );

			set = 0;
            ddev_control(uart_hdl, CMD_SET_STOP_END, &set);
        }

        uart_write_byte(port, ((uint8_t *)data)[i] );
    }

	ddev_close(uart_hdl);

    rtos_unlock_mutex( &pdrv->tx_mutex );

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    uint32_t read_size, actual_size, tmp;
    uint32_t ringbuf_size;
    uint32_t start_time, expired_time;
    _uart_drv_t *pdrv;
    uint8_t port;

    port = uart_port_convert(uart->port);
    pdrv = &_uart_drv[port];

    recv_size = recv_size == NULL ? &actual_size : recv_size;

    start_time = rtos_get_time();
    expired_time = 0;

    *recv_size = 0;
    ringbuf_size = ring_buffer_total_size(&pdrv->rx_ringbuf);

    for(;;)
    {
        read_size = expect_size > ringbuf_size ? ringbuf_size : expect_size;

        if(read_size > ring_buffer_used_space( &pdrv->rx_ringbuf ))
        {
            pdrv->rx_size = read_size;

            if ( rtos_get_semaphore( &pdrv->rx_semphr, timeout - expired_time) != kNoErr )
            {
                pdrv->rx_size = 0;
                read_size = ring_buffer_used_space( &pdrv->rx_ringbuf );
                ring_buffer_read(&pdrv->rx_ringbuf, data, read_size, &tmp);
                *recv_size += read_size;
                return -1;
            }
        }

        ring_buffer_read(&pdrv->rx_ringbuf, data, read_size, &tmp);

        data += read_size;
        *recv_size += read_size;
        expect_size -= read_size;

        if(expect_size == 0)
        {
            break;
        }

        expired_time = rtos_get_time() - start_time;
        if(expired_time > timeout)
        {
            return -1;
        }
    }

    return 0;
}

void uart_rx_cb(uint8_t port)
{
    uint8_t rx_byte;
    _uart_drv_t *pdrv = &_uart_drv[port];

    while(uart_read_byte(port, &rx_byte) == 0)
    {
        ring_buffer_write( &pdrv->rx_ringbuf, &rx_byte, 1 );
    }

    // Notify thread if sufficient data are available
    if ( pdrv->rx_size > 0 && ring_buffer_used_space( &pdrv->rx_ringbuf ) >= pdrv->rx_size )
    {
        rtos_set_semaphore( &pdrv->rx_semphr );
        pdrv->rx_size = 0;
    }  
}

void uart_tx_cb(uint8_t port)
{
    _uart_drv_t *pdrv = &_uart_drv[port];

    rtos_set_semaphore( &pdrv->tx_semphr );
}

