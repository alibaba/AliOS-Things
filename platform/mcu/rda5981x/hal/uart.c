#include "hal/soc/soc.h"
#include "serial_api.h"
#include "rda5981x_pinconfig.h"

#define UART_NUM 2

static serial_t serial_obj[UART_NUM];
static uint8_t is_inited[UART_NUM] = {0};

uart_dev_t uart_0 = {
    0,
    {921600, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},
    NULL,
};

int32_t hal_uart_init(uart_dev_t *uart)
{
    if (uart->port >= UART_NUM)
        return -1;

    if ((0 == uart->port) && (FLOW_CONTROL_DISABLED != uart->config.flow_control))
        return -1;

    uart->priv = &serial_obj[uart->port];

    if (0 == uart->port) {
        serial_init(uart->priv, RDA_UART0_TXD, RDA_UART0_RXD);
    } else {
        serial_init(uart->priv, RDA_UART1_TXD, RDA_UART1_RXD);
    }

    serial_baud(uart->priv, uart->config.baud_rate);
    serial_format(uart->priv, (uart->config.data_width + 5), uart->config.parity, (uart->config.stop_bits + 1));

    if (FLOW_CONTROL_CTS_RTS == uart->config.flow_control) {
        serial_set_flow_control(uart->priv, FLOW_CONTROL_CTS_RTS, RDA_UART1_RTS, RDA_UART1_CTS);
    }

    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if (0 == is_inited[uart->port]) {
        hal_uart_init(uart);
        is_inited[uart->port] = 1;
    }

    uint8_t *send_data = (char *)data;

    while (size > 0) {
        serial_putc(uart->priv, (int)*send_data);
        send_data++;
        size--;
    }

    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{

    if (0 == is_inited[uart->port]) {
        hal_uart_init(uart);
        is_inited[uart->port] = 1;
    }

    uint8_t *recv_data = (char *)data;

    while (expect_size > 0) {
        *recv_data = (uint8_t)serial_getc(uart->priv);
        recv_data++;
        expect_size--;
    }

    if (NULL != recv_size)
        recv_size = expect_size;

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}

