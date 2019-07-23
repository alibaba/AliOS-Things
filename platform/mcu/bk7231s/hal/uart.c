/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/hal/uart.h"
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
    uint8_t              status;
    beken_semaphore_t    rx_semphr;
    beken_semaphore_t    tx_semphr;
    beken_mutex_t        tx_mutex;
} _uart_drv_t;

static _uart_drv_t _uart_drv[MAX_UART_NUM];

void uart_rx_cb(uint8_t port, void *param);
void uart_tx_cb(uint8_t port, void *param);

int32_t hal_uart_init(uart_dev_t *uart)
{
	struct uart_callback_des uart_rx_callback;
	struct uart_callback_des uart_tx_callback;
    _uart_drv_t *pdrv;
    UINT32 status;
    DD_HANDLE uart_hdl;
	
    if(uart->port == UART1_PORT)
    {
        uart_hdl = ddev_open(UART2_DEV_NAME, &status, 0);
		pdrv  = &_uart_drv[1];
    }
    else
    {
        uart_hdl = ddev_open(UART1_DEV_NAME, &status, 0);
		pdrv  = &_uart_drv[0];
    }

    if(pdrv->status == _UART_STATUS_CLOSED)
    {
        rtos_init_semaphore( &pdrv->tx_semphr, 0 );
        rtos_init_semaphore( &pdrv->rx_semphr, 0 );
        rtos_init_mutex( &pdrv->tx_mutex );
		
        pdrv->status = _UART_STATUS_OPENED;
    }

    ddev_control(uart_hdl, CMD_UART_INIT, (void *)&uart->config);
	
	uart_rx_callback.callback = uart_rx_cb;
	uart_rx_callback.param = NULL;
	ddev_control(uart_hdl, CMD_UART_SET_RX_CALLBACK, (void *)&uart_rx_callback);

	uart_tx_callback.callback = uart_tx_cb;
	uart_tx_callback.param = NULL;
	ddev_control(uart_hdl, CMD_UART_SET_TX_CALLBACK, (void *)&uart_tx_callback);

	ddev_close(uart_hdl);

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    _uart_drv_t *pdrv;

	if(uart->port == UART1_PORT)
	{
		pdrv = &_uart_drv[1];
	}
	else
	{
		pdrv = &_uart_drv[0];
	}

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

    rtos_lock_mutex( &pdrv->tx_mutex );
	
    if(uart->port == UART1_PORT)
    {
        uart_hdl = ddev_open(UART2_DEV_NAME, &status, 0);
		pdrv = &_uart_drv[1];
		port = UART2_PORT;
    }
    else
    {
        uart_hdl = ddev_open(UART1_DEV_NAME, &status, 0);
		pdrv = &_uart_drv[0];
		port = UART1_PORT;
    }

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
    uint32_t read_size, rx_size;
    uint32_t start_time, expired_time;
    UINT32 status;
    DD_HANDLE uart_hdl;
    _uart_drv_t *pdrv;
	int ret = 0;

	if(uart == NULL || data == NULL || !expect_size)
	{
		return -1;
	}
	
    start_time = rtos_get_time();
    expired_time = 0;
	if(recv_size) *recv_size = 0;

    if(uart->port == UART1_PORT)
    {
        uart_hdl = ddev_open(UART2_DEV_NAME, &status, 0);
		pdrv = &_uart_drv[1];
    }
    else
    {
        uart_hdl = ddev_open(UART1_DEV_NAME, &status, 0);
		pdrv = &_uart_drv[0];
    }

	while(1)
	{
		rx_size = ddev_control(uart_hdl, CMD_RX_COUNT, 0);		
        read_size = expect_size > rx_size ? rx_size : expect_size;
		if(read_size)
		{
			ddev_read(uart_hdl, data, read_size, 0);
	        expect_size -= read_size;
	        data += read_size;
			if(recv_size) *recv_size += read_size;
		}
		
        if(expect_size)
        {
			if(expired_time < timeout)
			{
				if ( rtos_get_semaphore( &pdrv->rx_semphr, timeout - expired_time) != kNoErr )
				{
					ret = -1;
					break;
				}
				expired_time = rtos_get_time() - start_time;
			}
			else
			{
				ret = -1;
				break;
			}
        }
		else
		{
			ret = 0;
            break;
        }

    }

	ddev_close(uart_hdl);
	return ret;
}

void uart_rx_cb(uint8_t port, void *param)
{
	_uart_drv_t *pdrv;
	
	if(port == UART1_PORT)
	{
    	pdrv = &_uart_drv[1];
	}
	else
	{
		pdrv = &_uart_drv[0];
	}

	rtos_set_semaphore( &pdrv->rx_semphr );
}

void uart_tx_cb(uint8_t port, void *param)
{
	_uart_drv_t *pdrv;

	if(port == UART1_PORT)
	{
    	pdrv = &_uart_drv[1];
	}
	else
	{
		pdrv = &_uart_drv[0];
	}

    rtos_set_semaphore( &pdrv->tx_semphr );
}

