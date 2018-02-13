#include "hal/soc/soc.h"

#include <csi_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <hal/wifi.h>

uart_dev_t uart_0;

#ifdef CONFIG_NETIF_ETH
extern hal_wifi_module_t hobbit_eth_enc28j60;
#endif

void hal_init(void)
{
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = 115200;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);

#ifdef CONFIG_NETIF_ETH
    hal_wifi_register_module(&hobbit_eth_enc28j60);
#endif
}

void hal_boot(hal_partition_t partition)
{


}

void hal_reboot(void)

{

}

