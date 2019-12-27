
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "aos/kernel.h"

#include "config_esp8266.h"

extern int netm_uart_config(uint32_t baud,
                            uint8_t databits,
                            uint8_t stopbits,
                            uint8_t parity,
                            uint8_t flow_control,
                            uint8_t writetoflash
                           );


static void uartconfig_baud(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 1) {
        aos_cli_printf("default baud is :%d\n", CONFIG_NETM_BAUD);
    } else if (argc == 2) {
        aos_cli_printf("set baud is:%d\n", atoi(argv[1]));
        netm_uart_config(atoi(argv[1]), 8, 1, 0, 0, 1);
    }
}

static struct cli_command uartconfigcmd = {
    .name = "uartcfg",
    .help = "set espuart baud",
    .function = uartconfig_baud
};

void esp8266_uart_config(void)
{
    aos_cli_register_command(&uartconfigcmd);
}
