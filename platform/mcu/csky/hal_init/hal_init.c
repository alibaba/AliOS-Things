#include "hal/soc/soc.h"

uart_dev_t uart_0;

void hal_init(void)
{
    uart_0.port                = 0;
    uart_0.config.baud_rate    = 115200;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
}

void hal_boot(hal_partition_t partition)
{


}

void hal_reboot(void)

{

}

