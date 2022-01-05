#include <errno.h>
#include <driver/uart.h>
#include <aos_hal_uart_internal.h>

int32_t aos_hal_uart_init(aos_hal_uart_dev_t *uart)
{
    uart_config_t config = {
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        /* .source_clk = UART_SCLK_APB, */
    };
    int tx_io_num;
    int rx_io_num;

    if (!uart)
        return -EINVAL;

    switch (uart->port) {
    case 0:
        tx_io_num = 1;
        rx_io_num = 3;
        break;
    case 1:
        tx_io_num = 10;
        rx_io_num = 9;
        break;
#if SOC_UART_NUM > 2
    case 2:
#ifdef BOARD_M5STACKCORE2
        tx_io_num = 14;
        rx_io_num = 13;
#else
        tx_io_num = 17;
        rx_io_num = 16;
#endif
        break;
#endif
    default:
        return -EINVAL;
    }

    config.baud_rate = uart->config.baud_rate;

    switch (uart->config.data_width) {
    case DATA_WIDTH_5BIT:
        config.data_bits = UART_DATA_5_BITS;
        break;
    case DATA_WIDTH_6BIT:
        config.data_bits = UART_DATA_6_BITS;
        break;
    case DATA_WIDTH_7BIT:
        config.data_bits = UART_DATA_7_BITS;
        break;
    case DATA_WIDTH_8BIT:
        config.data_bits = UART_DATA_8_BITS;
        break;
    default:
        return -EINVAL;
    }

    switch (uart->config.stop_bits) {
    case STOP_BITS_1:
        config.stop_bits = UART_STOP_BITS_1;
        break;
    case STOP_BITS_2:
        config.stop_bits = UART_STOP_BITS_2;
        break;
    default:
        return -EINVAL;
    }

    switch (uart->config.parity) {
    case NO_PARITY:
        config.parity = UART_PARITY_DISABLE;
        break;
    case ODD_PARITY:
        config.parity = UART_PARITY_ODD;
        break;
    case EVEN_PARITY:
        config.parity = UART_PARITY_EVEN;
        break;
    default:
        return -EINVAL;
    }

    (void)uart_driver_delete(uart->port);

    if (uart_param_config(uart->port, &config) != ESP_OK)
        return -EINVAL;

    if (uart_driver_install(uart->port, 256, 256, 0, NULL, 0) != ESP_OK)
        return -EINVAL;

    if (uart_set_pin(uart->port, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        (void)uart_driver_delete(uart->port);
        return -EINVAL;
    }

    return 0;
}

int32_t aos_hal_uart_finalize(aos_hal_uart_dev_t *uart)
{
    if (!uart)
        return -EINVAL;

    if (uart_wait_tx_done(uart->port, portMAX_DELAY) != ESP_OK)
        return -EIO;

    if (uart_driver_delete(uart->port) != ESP_OK)
        return -EINVAL;

    return 0;
}

int32_t aos_hal_uart_recv_II(aos_hal_uart_dev_t *uart, void *data, uint32_t expect_size,
                             uint32_t *recv_size, uint32_t timeout)
{
    TickType_t ticks;
    int r;

    if (!uart || !data || expect_size == 0)
        return -EINVAL;

    if (timeout == 0)
        ticks = 0;
    else if (timeout == HAL_WAIT_FOREVER)
        ticks = portMAX_DELAY;
    else
        ticks = pdMS_TO_TICKS(timeout);

    r = uart_read_bytes(uart->port, data, expect_size, ticks);
    if (r < 0)
        return -EIO;

    if (recv_size)
        *recv_size = r;

    return 0;
}

int32_t aos_hal_uart_send(aos_hal_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int r;

    if (!uart || !data || size == 0)
        return -EINVAL;

    r = uart_write_bytes(uart->port, data, size);
    if (r < 0)
        return -EIO;

    return 0;
}
