#include <atmel_start.h>
#include <hal/soc/soc.h>
#include <string.h>
#include <aos/errno.h>

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;
    enum usart_character_size size;
    enum usart_parity parity;
    enum usart_stop_bits stop;
    union usart_flow_control_state flow;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case 0:
            uart->priv = &USART_0;
            break;
        case 2:
            uart->priv = &USART_2;
            break;
        /* if ohter uart exist add init code here */
        default:
        break;
    }

	return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }
    if (uart->port == STDIO_UART) {
        uart->priv = &USART_2;
    }

    if (io_write(&((struct usart_os_descriptor *)uart->priv)->io, data, size, timeout) != size) {
      return EIO;
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }
    if (uart->port == STDIO_UART) {
        uart->priv = &USART_2;
    }

    *recv_size = io_read(&((struct usart_os_descriptor *)uart->priv)->io, (uint8_t *)data, expect_size, timeout);

    return (*recv_size == 0) ? -1 : 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (uart == NULL) {
        return -1;
    }

    return usart_os_deinit((struct usart_os_descriptor *)(uart->priv));
}
