/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "board.h"
#include "aos/hal/uart.h"
#include "uart_pl011.h"
#include <typedef.h>
#if (RHINO_CONFIG_CPU_NUM > 1)
#include "smp_port.h"
kspinlock_t g_smp_printlock;
#endif
int32_t hal_uart_init(uart_dev_t *uart)
{
	if (uart->port > MICO_UART_MAX)
        return -1;

    uart_config port_config = {
        8, 1, false, 115200
    };
    //port_config.baudrate = uart->config.baud_rate;
#if (RHINO_CONFIG_CPU_NUM > 1)
	krhino_spin_lock_init(&g_smp_printlock);
#endif
    uart_init(uart->port);
    uart_configure(uart->port, &port_config);

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (uart->port > MICO_UART_MAX)
        return -1;
    uart_deinit(uart->port);
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i;
    char *uart_data = (char *)data;
    if (uart->port >= MICO_UART_MAX)
        return -1;

    for(i = 0; i < size; i++) {
        uart_putchar(uart->port,*uart_data);
        uart_data++;
    }
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    char *uart_data = (unsigned char *)data;
    uart_error ret;
    u64 start_tick;
    u64 elapse_tick;
    if (uart->port >= MICO_UART_MAX)
        return -1;

    // get current tick, 1us per tick
    start_tick = hw_timer3_count();

    *recv_size = 0;
    while(*recv_size < expect_size) {
        ret = uart_getchar_from_interrupt(uart->port, uart_data);
        if(ret == UART_OK) {
            uart_data++;
            (*recv_size)++;
        } else if (ret == UART_RECEIVE_ERROR) {
            return -1;
        } else {
            uart_rx_wait(uart->port, timeout);
            // check elapse time, down counter
            elapse_tick = start_tick - hw_timer3_count();
            if(elapse_tick >= (u64)timeout * 1000)
                return -1;
        }
    }
    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    char *uart_data = (unsigned char *)data;
    uart_error ret;
    u64 start_tick;
    u64 elapse_tick;
    int size = 0;
    if (uart->port >= MICO_UART_MAX)
        return -1;

    // get current tick, 1us per tick
    start_tick = hw_timer3_count();


    while(size < expect_size) {
        ret = uart_getchar_from_interrupt(uart->port, uart_data);
        if(ret == UART_OK) {
            uart_data++;
            size++;
        } else if (ret == UART_RECEIVE_ERROR) {
            return -1;
        } else {
            uart_rx_wait(uart->port, timeout);
            // check elapse time, down counter
            elapse_tick = start_tick - hw_timer3_count();
            if(elapse_tick >= (u64)timeout * 1000)
                return size;
        }
    }
    return 0;
}
int alios_debug_print(const char *buf, int size)
{
    uart_dev_t debug_uart;
    debug_uart.port = MICO_UART_0;

    return hal_uart_send(&debug_uart, buf, size, 0);
}

int uart_input_read()
{
	int ch;
    char uart_data;
    uart_error ret;
    uart_dev_t debug_uart;
    debug_uart.port = MICO_UART_0;

    while(1){
        ret = uart_getchar(debug_uart.port, &uart_data);
        if(ret == UART_OK) {
            ch = uart_data;
            break;
        }
	}
	return (int)ch;
}

int32_t hal_uart_recv_cb_reg(uart_dev_t *uart, uart_rx_cb cb)
{
    return uart_rx_callback_register(uart, cb);
}
