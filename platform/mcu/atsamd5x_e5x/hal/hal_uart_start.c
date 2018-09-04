#include <atmel_start.h>
#include <hal/soc/soc.h>
#include <string.h>
#include <aos/errno.h>

/* Global Variable for VFS DEBUG Output */
uart_dev_t uart_cli = {
   .port = STDIO_UART,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

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
#if 0
    /* Baudrate */
    ret = usart_sync_set_baud_rate((struct usart_sync_descriptor *)uart->priv, uart->config.baud_rate);

    /* Data Width */
    switch (uart->config.data_width) {
    case DATA_WIDTH_5BIT:
      size = USART_CHARACTER_SIZE_5BITS;
      break;
    case DATA_WIDTH_6BIT:
      size = USART_CHARACTER_SIZE_6BITS;
      break;
    case DATA_WIDTH_7BIT:
      size = USART_CHARACTER_SIZE_7BITS;
      break;
    case DATA_WIDTH_8BIT:
      size = USART_CHARACTER_SIZE_8BITS;
      break;
    case DATA_WIDTH_9BIT:
      size = USART_CHARACTER_SIZE_9BITS;
      break;
    default:
      return -1;
    }
    usart_sync_set_character_size((struct usart_sync_descriptor *)uart->priv, size);

    /* Parity */
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
    usart_sync_set_parity((struct usart_sync_descriptor *)uart->priv, parity);

    /* Stop Bit */
    switch (uart->config.stop_bits) {
    case STOP_BITS_1:
      stop = USART_STOP_BITS_ONE;
      break;
    case STOP_BITS_2:
      stop = USART_STOP_BITS_TWO;
      break;
    default:
      return -1;
    }
    usart_sync_set_stopbits((struct usart_sync_descriptor *)uart->priv, stop);

    /* FlowControl*/
    memset(&flow, 0, sizeof(union usart_flow_control_state));
    switch (uart->config.flow_control) {
    case FLOW_CONTROL_CTS:
      flow.bit.cts = 1;
      break;
    case FLOW_CONTROL_RTS:
      flow.bit.rts = 1;
      break;
    case FLOW_CONTROL_CTS_RTS:
      flow.bit.cts = 1;
      flow.bit.rts = 1;
      break;
    default:
      break;
    }
    usart_sync_set_flow_control((struct usart_sync_descriptor *)uart->priv, flow);

    return ret;
#endif
	return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if (uart->port == 0 && uart->priv == 0) {
        uart->port = STDIO_UART;
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
    if (uart->port == 0 && uart->priv == 0) {
        uart->port = STDIO_UART;
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
