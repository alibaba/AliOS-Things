/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#define uart_dev_t aos_uart_dev_t
#define uart_config_t aos_uart_config_t
#include "aos/hal/uart.h"
#undef uart_dev_t
#undef uart_config_t

#include <rom/ets_sys.h>
#include <driver/uart.h>


int32_t hal_uart_send(aos_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uart_write_bytes(uart->port, data, size);
    return 0;
}

int32_t hal_uart_recv_II(aos_uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    int ttl_len = 0;
    char *buf = data;
    while (1) {
        char c;
        int ret = uart_read_bytes(uart->port, &c, 1, timeout);
        if (ret <= 0)
            break;

        *buf++ = c;
        ttl_len ++;
        if (ttl_len >= expect_size)
            break;
    }

    if (recv_size)
        *recv_size = ttl_len;

    return ttl_len > 0 ? 0 : -1;
}

#if defined(CONFIG_BLE_HCI_H4_UART_PORT)
static void prepare_h4_uart_config(aos_uart_config_t *ac, uart_config_t *c)
{
    /* buat rate */
    c->baud_rate = ac->baud_rate;
    /* data width */
    switch (ac->data_width) {
        case DATA_WIDTH_5BIT: c->data_bits = UART_DATA_5_BITS; break;
        case DATA_WIDTH_6BIT: c->data_bits = UART_DATA_6_BITS; break;
        case DATA_WIDTH_7BIT: c->data_bits = UART_DATA_7_BITS; break;
        case DATA_WIDTH_8BIT: default: c->data_bits = UART_DATA_8_BITS; break;
    }
    /* parity */
    switch (ac->parity) {
        case ODD_PARITY: c->parity = UART_PARITY_ODD; break;
        case EVEN_PARITY: c->parity = UART_PARITY_EVEN; break;
        case NO_PARITY: default: c->parity = UART_PARITY_DISABLE; break;
    }
    /* stop bits */
    switch (ac->stop_bits) {
        case STOP_BITS_2: c->stop_bits = UART_STOP_BITS_2; break;
        case STOP_BITS_1: default : c->stop_bits = UART_STOP_BITS_1; break;
    }
    /* flow control */
    switch (ac->flow_control) {
        case FLOW_CONTROL_CTS: c->flow_ctrl = UART_HW_FLOWCTRL_CTS; break;
        case FLOW_CONTROL_RTS: c->flow_ctrl = UART_HW_FLOWCTRL_RTS; break;
        case FLOW_CONTROL_CTS_RTS: c->flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS; break;
        case FLOW_CONTROL_DISABLED: default: c->flow_ctrl = UART_HW_FLOWCTRL_DISABLE; break;
    }
    c->rx_flow_ctrl_thresh = 122;
}
#endif

int32_t hal_uart_init(aos_uart_dev_t *uart)
{
#if defined(CONFIG_BLE_HCI_H4_UART_PORT)
    if (uart->port == CONFIG_BLE_HCI_H4_UART_PORT) {
        uart_config_t uart_config;
        prepare_h4_uart_config(&(uart->config), &uart_config);
        uart_param_config(uart->port, &uart_config);
        uart_set_pin(uart->port, 4, 5, 18, 19);
        uart_driver_install(uart->port, 1024 * 2, 1024 * 2, 0, NULL, 0);
    } else {
        uart_driver_install(uart->port, 256, 0, 0, NULL, 0);
    }
#else
    uart_driver_install(uart->port, 256, 0, 0, NULL, 0);
#endif

    return 0;
}

int32_t hal_uart_finalize(aos_uart_dev_t *uart)
{
    return 0;
}
