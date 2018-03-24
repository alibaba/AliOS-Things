/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "ringbuf.h"
#include "mico_rtos.h"

typedef unsigned char  		  UINT8;          /* Unsigned  8 bit quantity        */
typedef signed   char  		  INT8;           /* Signed    8 bit quantity        */
typedef unsigned short 		  UINT16;         /* Unsigned 16 bit quantity        */
typedef signed   short 		  INT16;          /* Signed   16 bit quantity        */
typedef uint32_t   		      UINT32;         /* Unsigned 32 bit quantity        */
typedef int32_t   		      INT32;          /* Signed   32 bit quantity        */

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
    mico_semaphore_t    rx_semphr;
    mico_semaphore_t    tx_semphr;
    mico_mutex_t        tx_mutex;
} _uart_drv_t;

static _uart_drv_t _uart_drv[MAX_UART_NUM];

extern int32_t uart_read_byte( uint8_t port, uint8_t *rx_byte );
extern void uart_write_byte( uint8_t port, uint8_t byte );
extern uint8_t uart_is_tx_fifo_empty( uint8_t port );
extern uint8_t uart_is_tx_fifo_full( uint8_t port );
extern void uart_set_tx_stop_end_int( uint8_t port, uint8_t set );

int32_t hal_uart_init(uart_dev_t *uart)
{
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    if(pdrv->status == _UART_STATUS_CLOSED)
    {
        pdrv->rx_buf = (uint8_t *)malloc(UART_FIFO_SIZE);
        ring_buffer_init(&pdrv->rx_ringbuf, pdrv->rx_buf, UART_FIFO_SIZE);

        mico_rtos_init_semaphore( &pdrv->tx_semphr, 0 );
        mico_rtos_init_semaphore( &pdrv->rx_semphr, 0 );
        mico_rtos_init_mutex( &pdrv->tx_mutex );

        uart_open(uart->port);

        pdrv->status = _UART_STATUS_OPENED;
    }

    while(!uart_is_tx_fifo_empty(uart->port));
    uart_config(uart->port, &uart->config);

    return 0;
}


int32_t hal_uart_finalize(uart_dev_t *uart)
{
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    while(!uart_is_tx_fifo_empty(uart->port));

    uart_close(uart->port);

    ring_buffer_deinit(&pdrv->rx_ringbuf);
    free(pdrv->rx_buf);

    mico_rtos_deinit_semaphore(&pdrv->rx_semphr);
    mico_rtos_deinit_semaphore(&pdrv->tx_semphr);
    mico_rtos_deinit_mutex(&pdrv->tx_mutex);

    pdrv->status = _UART_STATUS_CLOSED;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i = 0;
    (void)timeout;

    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    mico_rtos_lock_mutex( &pdrv->tx_mutex );

    for( i = 0; i < size; i++ )
    {
        if( uart_is_tx_fifo_full(uart->port) )
        {
            uart_set_tx_stop_end_int(uart->port, 1 );
            /* The data in Tx FIFO may have been sent out before enable TX_STOP_END interrupt */
            /* So double check the FIFO status */
            while( !uart_is_tx_fifo_empty(uart->port) )
                mico_rtos_get_semaphore(&pdrv->tx_semphr, 50 );

            uart_set_tx_stop_end_int( uart->port, 0 );
        }

        uart_write_byte(uart->port, ((uint8_t *)data)[i] );
    }

    mico_rtos_unlock_mutex( &pdrv->tx_mutex );

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    uint32_t read_size, actual_size, tmp;
    uint32_t ringbuf_size;
    uint32_t start_time, expired_time;
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    recv_size = recv_size == NULL ? &actual_size : recv_size;

    start_time = mico_rtos_get_time();
    expired_time = 0;

    *recv_size = 0;
    ringbuf_size = ring_buffer_total_size(&pdrv->rx_ringbuf);

    for(;;)
    {
        read_size = expect_size > ringbuf_size ? ringbuf_size : expect_size;

        if(read_size > ring_buffer_used_space( &pdrv->rx_ringbuf ))
        {
            pdrv->rx_size = read_size;

            if ( mico_rtos_get_semaphore( &pdrv->rx_semphr, timeout - expired_time) != kNoErr )
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

        expired_time = mico_rtos_get_time() - start_time;
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
        ring_buffer_write( &pdrv->rx_ringbuf, &rx_byte,1 );
    }

    // Notify thread if sufficient data are available
    if ( pdrv->rx_size > 0 && ring_buffer_used_space( &pdrv->rx_ringbuf ) >= pdrv->rx_size )
    {
        mico_rtos_set_semaphore( &pdrv->rx_semphr );
        pdrv->rx_size = 0;
    }  
}

void uart_tx_cb(uint8_t port)
{
    _uart_drv_t *pdrv = &_uart_drv[port];

    mico_rtos_set_semaphore( &pdrv->tx_semphr );
}
