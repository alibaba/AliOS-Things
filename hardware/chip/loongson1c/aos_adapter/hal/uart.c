#include <aos/kernel.h>
#include <aos/hal/uart.h>
#include <ls1c_uart.h>
#include <ls1c_regs.h>
#include <start.h>

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if (!uart || uart->port != 2 || data == NULL || size == 0)
        return -EINVAL;

    for (size_t i = 0; i < size; i++) {
        char str[2];

        str[0] = ((const char *)data)[i];
        str[1] = '\0';
        uart2_print(str);
    }

    return 0;
}

static size_t rx_data(void *base, void *buf, size_t count)
{
    size_t i;

    for (i = 0; i < count && i < 64; i++) {
        if (!(reg_read_8(base + LS1C_UART_LSR_OFFSET) & LSR_RXRDY))
            break;
        ((uint8_t *)buf)[i] = reg_read_8(base + LS1C_UART_DAT_OFFSET);
    }

    return i;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    void *base = uart_get_base(LS1C_UART2);
    uint32_t expired = 0;

    if (!uart || uart->port != 2 || data == NULL || expect_size == 0)
        return -EINVAL;

    if (recv_size)
        *recv_size = 0;

    do {
        size_t rx_count = rx_data(base, data, expect_size);

        if (rx_count > 0) {
            if (recv_size)
                *recv_size = rx_count;
            break;
        }

        if (timeout != AOS_WAIT_FOREVER) {
            expired += 20;
            if (expired > timeout)
                break;
        }

        aos_msleep(20);
    } while (1);

    return 0;
}
