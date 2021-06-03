/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
#if AOS_COMP_CLI
#include "stdbool.h"
#include "lwip/opt.h"
#include "lwip/apps/ifconfig.h"
#include <lwip/def.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include "aos/cli.h"

void ifconfig_help(void)
{
    aos_cli_printf("Usage: ifconfig\n");
    aos_cli_printf("Example:\n");
    aos_cli_printf("ifconfig           Display net interface config information\n");
    aos_cli_printf("ifconfig status    Display net interface config information\n");
}

void ifconfig_cmd(int argc, char **argv )
{
    if ( argc < 1 ) {
        aos_cli_printf("Invalid command\n");
        ifconfig_help();
        return;
    }

    if((argc == 2) && (strcmp(argv[1], "-h") == 0)) {
        ifconfig_help();
    }
    else {
        ifconfig(argc - 1, &argv[1]);
    }
}

/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ifconfig_cmd, ifconfig, Ifconfig command)
#endif /* AOS_COMP_CLI */
