#include <errno.h>
#include <driver/uart_select.h>
#include <aos_hal_uart_internal.h>
#include "esp_log.h"
#include <soc/uart_caps.h>

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

    if (uart_is_driver_installed(uart->port)) {
        if (uart_set_baudrate(uart->port, config.baud_rate) != ESP_OK)
            return -EINVAL;

        if (uart_set_parity(uart->port, config.parity) != ESP_OK)
            return -EINVAL;

        if (uart_set_word_length(uart->port, config.data_bits) != ESP_OK)
            return -EINVAL;

        if (uart_set_stop_bits(uart->port, config.stop_bits) != ESP_OK)
            return -EINVAL;
    } else {
        if (uart_driver_install(uart->port, 256, 256, 0, NULL, 0) != ESP_OK)
            return -EINVAL;

        if (uart_set_pin(uart->port, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
            (void)uart_driver_delete(uart->port);
            return -EINVAL;
        }

        if (uart_param_config(uart->port, &config) != ESP_OK) {
            (void)uart_driver_delete(uart->port);
            return -EINVAL;
        }
    }

    (void)uart_flush_input(uart->port);

    return 0;
}

int32_t aos_hal_uart_finalize(aos_hal_uart_dev_t *uart)
{
    if (!uart)
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

int32_t aos_hal_uart_any(aos_hal_uart_dev_t *uart)
{
    int32_t rx_buf_size;
    uart_get_buffered_data_len(uart->port, &rx_buf_size);
    return rx_buf_size;
}

static struct aos_hal_uart_dev_t* uart_dev_table[UART_NUM_MAX] = {NULL};

static void select_notif_callback_isr(uart_port_t uart_num, uart_select_notif_t uart_select_notif, BaseType_t *task_woken)
{
    size_t rx_data_len = 0;
    char data[256] = {0};
    if (!uart_dev_table[uart_num])
        return;

    aos_hal_uart_dev_t *uart_dev = uart_dev_table[uart_num];
    switch (uart_select_notif) {
    case UART_SELECT_READ_NOTIF:
        memset(data, 0, sizeof(data));
        rx_data_len = uart_read_bytes(uart_num, data, sizeof(data), 0);
        uart_dev->cb(uart_num, data, (uint16_t)rx_data_len, uart_dev->userdata);
        break;
    case UART_SELECT_WRITE_NOTIF:
    case UART_SELECT_ERROR_NOTIF:
        break;
    }
}

int32_t aos_hal_uart_callback(aos_hal_uart_dev_t *uart, void (*cb)(int, void *, uint16_t, void *), void *userdata)
{
    if (uart->port < 0 || uart->port > UART_NUM_MAX - 1) {
        return -EINVAL;
    }
    uart->cb = cb;
    uart->userdata = userdata;
    uart_dev_table[uart->port] = uart;
    uart_set_select_notif_callback(uart->port, select_notif_callback_isr);
    return -ENOTSUP;
}