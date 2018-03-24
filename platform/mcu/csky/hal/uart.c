/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "hal/soc/soc.h"
#include "drv_usart.h"
#include "pin.h"
#include "ringbuffer.h"
#include "soc.h"
#include "mico_rtos.h"

#define MAX_UART_NUM 3
#define STAT_XMIT_IDLE      0x00
#define STAT_XMIT_READ      0x01
#define STAT_XMIT_WRITE     0x02
#define UART_FIFO_SIZE 256

typedef struct usart_dev_s {
    int     crefs;                      /* The number of USARTs the device has been opened */
    mico_semaphore_t ksem_excl;          /* Mutual exclusion semaphore */
    usart_handle_t usart_handle;

    ringbuffer_t *read_buffer;
    /* I/O buffers */
    mico_semaphore_t       ksem_write;
    mico_semaphore_t       ksem_read;
    usart_event_e          usart_txevent;
    usart_event_e          usart_rxevent;
    uint8_t                stat_txmit;
    uint8_t                stat_rxmit;
    uint32_t               read_num;
    uint8_t                flowctrl;
    mico_mutex_t           tx_mutex;
} usart_dev_t;

static usart_dev_t usart_devs[MAX_UART_NUM];

static void usart_event_cb_fun(int32_t idx, usart_event_e event)
{
    usart_dev_t *usart_dev = &usart_devs[idx];
    usart_handle_t handle = usart_dev->usart_handle;

    switch (event) {
        case USART_EVENT_SEND_COMPLETE:
            if (usart_dev->stat_txmit == STAT_XMIT_WRITE) {
                usart_dev->usart_txevent = event;
                mico_rtos_set_semaphore(usart_dev->ksem_write);
            }

            break;

        case USART_EVENT_RECEIVE_COMPLETE:
            if (usart_dev->stat_rxmit == STAT_XMIT_READ) {

                usart_dev->usart_rxevent = event;
                mico_rtos_set_semaphore(usart_dev->ksem_read);
            }

            break;

        case USART_EVENT_RECEIVED: {
            uint8_t data[16];
            int32_t ret;
            memset(data, 0, 16);

            if (usart_dev->read_buffer == NULL) {
                break;
            }

#if 0
            /* flow ctrl */
            if (usart_dev->flowctrl) {
                ret = ringbuffer_available_write_space(usart_dev->read_buffer);

                if (ret <= 64) {
                    csi_usart_interrupt_on_off(handle, USART_INTR_READ, 0);
                    break;
                }
            }
#endif
            ret = csi_usart_receive_query(handle, data, 16);

            if (usart_dev->stat_rxmit != STAT_XMIT_READ) {
                return;
            }

            ringbuffer_write(usart_dev->read_buffer, (uint8_t *)data, ret);

            if (ringbuffer_available_read_space(usart_dev->read_buffer) >= usart_dev->read_num) {
                if (usart_dev->stat_rxmit == STAT_XMIT_READ) {
                    usart_dev->usart_rxevent = USART_EVENT_RECEIVE_COMPLETE;
                    mico_rtos_set_semaphore(usart_dev->ksem_read);
                }
            }
        }
        break;

        case USART_EVENT_RX_OVERFLOW:
            break;

        default:
            if (usart_dev->stat_rxmit == STAT_XMIT_READ) {
                usart_dev->usart_rxevent = event;
                mico_rtos_set_semaphore(usart_dev->ksem_read);

                usart_dev->stat_rxmit = STAT_XMIT_IDLE;
            }

    }
}
static int32_t usart_config(uart_dev_t *uart)
{
    usart_dev_t *usart_dev = &usart_devs[uart->port];

    /* config uart parity attribute */
    usart_parity_e parity;

    switch (uart->config.parity) {
        case NO_PARITY:
            parity = USART_PARITY_NONE;
            break;

        case ODD_PARITY:
            parity = USART_PARITY_ODD;
            break;

        case EVEN_PARITY:
            parity = USART_PARITY_EVEN;
            break;

        default:
            return -1;
    }

    /* config stop bit attribute */
    usart_stop_bits_e stopbits;

    switch (uart->config.stop_bits) {
        case STOP_BITS_1:
            parity = USART_STOP_BITS_1;
            break;

        case STOP_BITS_2:
            parity = USART_STOP_BITS_2;
            break;

        default:
            return -1;
    }

    int32_t ret = csi_usart_config(usart_dev->usart_handle, uart->config.baud_rate, USART_MODE_ASYNCHRONOUS, parity, stopbits, uart->config.data_width);

    if (ret < 0) {
        return -1;
    }

    ret = csi_usart_config_flowctrl(usart_dev->usart_handle, uart->config.flow_control);

    if (ret == 0) {
        usart_dev->flowctrl = 1;
    }

    return 0;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    usart_dev_t *usart_dev = &usart_devs[uart->port];

    /* get usart handle */
    if (uart->port >= MAX_UART_NUM) {
        return -EIO;
    }

    usart_dev->usart_handle = csi_usart_initialize(uart->port, usart_event_cb_fun);

    if (usart_dev->usart_handle == NULL) {
        return -EIO;
    }

    /* config uart attribute */

    int32_t ret = usart_config(uart);

    if (ret < 0) {
        csi_usart_uninitialize(usart_dev->usart_handle);
        return -EIO;
    }

    printf("enter hal_uart_init Ok\n");
    mico_rtos_init_semaphore(&usart_dev->ksem_write, 0);
    mico_rtos_init_semaphore(&usart_dev->ksem_read, 0);
    mico_rtos_deinit_mutex(&usart_dev->tx_mutex);

    usart_dev->read_buffer = ringbuffer_create(UART_FIFO_SIZE);

    if (usart_dev->read_buffer == NULL) {
        return -EIO;
    }

    usart_dev->crefs++;
    usart_dev->stat_rxmit = STAT_XMIT_IDLE;
    usart_dev->stat_txmit = STAT_XMIT_IDLE;

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    usart_dev_t *usart_dev = &usart_devs[uart->port];

    usart_status_t stat = csi_usart_get_status(usart_dev->usart_handle);

    if (stat.tx_busy != 0 && stat.rx_busy != 0) {
        return  -EIO;
    }

    if (usart_dev->crefs > 0) {
        usart_dev->crefs--;
    }

    ringbuffer_t *pread_buffer = usart_dev->read_buffer;
    usart_dev->read_buffer = NULL;
    ringbuffer_destroy(pread_buffer);

    mico_rtos_deinit_semaphore(&usart_dev->ksem_write);
    mico_rtos_deinit_semaphore(&usart_dev->ksem_read);
    csi_usart_uninitialize(usart_dev->usart_handle);

    usart_dev->stat_rxmit = STAT_XMIT_IDLE;
    usart_dev->stat_txmit = STAT_XMIT_IDLE;

    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    usart_dev_t *usart_dev = &usart_devs[uart->port];
    usart_handle_t handle = usart_dev->usart_handle;

    mico_rtos_lock_mutex(&usart_dev->tx_mutex);

    usart_dev->stat_txmit = STAT_XMIT_WRITE;

    csi_usart_send(handle, data , size);
    //wait transimit done
    mico_rtos_get_semaphore(usart_dev->ksem_write, 5);
    usart_dev->stat_txmit = STAT_XMIT_IDLE;

    if (usart_dev->usart_txevent != USART_EVENT_SEND_COMPLETE) {
        // return  -EIO;
    }

    mico_rtos_unlock_mutex(&usart_dev->tx_mutex);

    return 0;

}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    usart_dev_t *usart_dev = &usart_devs[uart->port];
    int32_t ret;

    usart_dev->stat_rxmit = STAT_XMIT_READ;
    usart_dev ->read_num = expect_size;

    //wait receive done
    if (ringbuffer_available_read_space(usart_dev->read_buffer) < expect_size) {
        mico_rtos_get_semaphore(&usart_dev->ksem_read, 50);
    }

    if (ringbuffer_available_read_space(usart_dev->read_buffer) >= expect_size) {
        usart_dev->usart_rxevent = USART_EVENT_RECEIVE_COMPLETE;
    }

    if (usart_dev->usart_rxevent != USART_EVENT_RECEIVE_COMPLETE) {
        usart_dev->stat_rxmit = STAT_XMIT_IDLE;
        return -EIO;
    }

    ret = ringbuffer_read(usart_dev->read_buffer, data, expect_size);

    recv_size = (uint32_t *)&ret;
    /* flow ctrl */
#if 0

    if (usart_dev->flowctrl) {
        if (ringbuffer_available_read_space(usart_dev->read_buffer) > 64) {
            csi_usart_set_interrupt(usart_dev->usart_handle, USART_INTR_READ, 1);
        }
    }

#endif

    if (ret <= 0) {
        usart_dev->stat_rxmit = STAT_XMIT_IDLE;
        return -EIO;
    }

    usart_dev->stat_rxmit = STAT_XMIT_IDLE;
    return 0;

}

