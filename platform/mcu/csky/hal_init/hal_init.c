#include "hal/soc/soc.h"

#include <csi_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <hal/wifi.h>

uart_dev_t uart_0;

extern void hal_flash_init(void);

void hal_init(void)
{
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = 115200;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);

    hal_flash_init();

#ifdef CONFIG_NETIF_ETH
    extern hal_wifi_module_t csky_eth_enc28j60;
    extern int lwip_tcpip_init(void);

    hal_wifi_register_module(&csky_eth_enc28j60);

    lwip_tcpip_init();
#endif

#ifdef CONFIG_NETIF_WIFI
    extern hal_wifi_module_t csky_wifi_esp8266;
    hal_wifi_register_module(&csky_wifi_esp8266);
#endif
}

void hal_boot(hal_partition_t partition)
{


}

void hal_reboot(void)

{

}

