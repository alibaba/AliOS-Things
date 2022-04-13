/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_defines.h"
#include "ql_uart.h"
#include "amp_hal_uart.h"


#define MOD_STR		"UART"

int32_t amp_hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;
    ql_uart_config_t dcb;

    if (uart == NULL) {
        return ret;
    }

    switch(uart->config.baud_rate) {
        case 300:
            dcb.baudrate = QL_UART_BAUD_300;
            break;
        case 9600:
            dcb.baudrate = QL_UART_BAUD_9600;
            break;
        case 19200:
            dcb.baudrate = QL_UART_BAUD_19200;
            break;
        case 115200:
            dcb.baudrate = QL_UART_BAUD_115200;
            break;
        case 921600:
            dcb.baudrate = QL_UART_BAUD_921600;
            break;
        default: break;
    }

    switch(uart->config.data_width) {
        case DATA_WIDTH_5BIT:
            dcb.data_bit = QL_UART_DATABIT_5;
            break;
        case DATA_WIDTH_6BIT:
            dcb.data_bit = QL_UART_DATABIT_6;
            break;
        case DATA_WIDTH_7BIT:
            dcb.data_bit = QL_UART_DATABIT_7;
            break;
        case DATA_WIDTH_8BIT:
            dcb.data_bit = QL_UART_DATABIT_8;
            break;
        default: break;
    }

    switch(uart->config.parity) {
        case NO_PARITY:
            dcb.parity_bit = QL_UART_PARITY_NONE;
            break;
        case ODD_PARITY:
            dcb.parity_bit = QL_UART_PARITY_ODD;
            break;
        case EVEN_PARITY:
            dcb.parity_bit = QL_UART_PARITY_EVEN;
            break;
        default: break;
    }

    switch(uart->config.stop_bits) {
        case STOP_BITS_1:
            dcb.stop_bit = QL_UART_STOP_1;
            break;
        case STOP_BITS_2:
            dcb.stop_bit = QL_UART_STOP_2;
            break;
        default: break;
    }

     switch(uart->config.flow_control) {
        case FLOW_CONTROL_DISABLED:
            dcb.flow_ctrl = QL_FC_NONE;
            break;
        case FLOW_CONTROL_CTS_RTS:
            dcb.flow_ctrl = QL_FC_HW;
            break;
        default: break;
    }   

    ret = ql_uart_set_dcbconfig(uart->port, &dcb);
    if (ret != 0) {
        return ret;
    }

    ret = ql_uart_open(uart->port, dcb.baudrate, dcb.flow_ctrl);
    if (ret != 0) {
        return ret;
    }


    return ret;
}

int32_t amp_hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = -1;

    ret = ql_uart_write(uart->port, data, size);
    if (ret != 0) {
        return ret;
    }

    return ret;
}

int32_t amp_hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    int32_t ret = -1;

    ret = ql_uart_read(uart->port, data, expect_size);
    if (ret <= 0) {
        return -1;
    }

    return 0;
}

int32_t amp_hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    int32_t ret = -1;
    uint32_t sleep_ms = 1;

    if (uart->port == AMP_REPL_STDIO) {
        sleep_ms = 10;
    }

    do {
        ret = ql_uart_read(uart->port, data, expect_size);
        if (ret < 0) {
            *recv_size = 0;
            return -1;
        } else if (ret == 0) {
            if (timeout >= sleep_ms) {
                HAL_SleepMs(sleep_ms);
                timeout -= sleep_ms;
            } else if (timeout > 0) {
                HAL_SleepMs(timeout);
                timeout = 0;
            } else {
                *recv_size = 0;
                return -1;
            }
        } else {
            break;
        }
    } while (1);

    *recv_size = ret;
    return 0;
}

typedef struct {
    void (*callback)(int, void *, uint16_t, void *);
    void *arg;
    uart_dev_t *uart;
} uart_recv_notify_t;

static uart_recv_notify_t *notifiers[3];

static void __uart_recv_callback(int port, void *para)
{
    uart_recv_notify_t *notify = notifiers[port];

    notify->callback(port, NULL, 0, notify->arg);
}

int32_t amp_hal_uart_callback(uart_dev_t *uart, void (*cb)(int, void *, uint16_t, void *), void *args)
{
    uart_recv_notify_t *notify = notifiers[uart->port];

    if (!notify) {
        notify = HAL_Malloc(sizeof(uart_recv_notify_t));
        if (!notify){
            return -1;
        }
    }
    notify->callback = cb;
    notify->arg = args;
    notify->uart = uart;
    notifiers[uart->port] = notify;

    //ql_uart_flush_fifo(uart->port);
    ql_uart_register_cb(uart->port, __uart_recv_callback);

    return 0;
}

int32_t amp_hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (notifiers[uart->port]) {
        HAL_Free(notifiers[uart->port]);
    }

    ret = ql_uart_close(uart->port);
    if (ret != 0) {
        return ret;
    }
    
    return ret;
}

