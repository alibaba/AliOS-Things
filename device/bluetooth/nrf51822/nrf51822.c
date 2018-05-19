#include <stdint.h>
#include <hal/soc/soc.h>

extern void hci_h4_set_uart_config(uart_config_t *c);

void nrf51822_h4_set_uart_config()
{
    uart_config_t c = {
        .baud_rate = 115200,
        .data_width = DATA_WIDTH_8BIT,
        .parity = NO_PARITY,
        .stop_bits = STOP_BITS_1,
        .flow_control = FLOW_CONTROL_CTS_RTS
    };

    hci_h4_set_uart_config(&c);
}
