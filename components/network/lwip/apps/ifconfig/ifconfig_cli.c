/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include "stdbool.h"
#include "network/network.h"
#include "lwip/apps/ifconfig.h"
#include "aos/cli.h"
#include "aos/yloop.h"
#include "lwip/err.h"

void ifconfig_help(void)
{
    aos_cli_printf("Usage: ifconfig\n");
    aos_cli_printf("Example:\n");
    aos_cli_printf("ifconfig           Display net interface config information\n");
    aos_cli_printf("ifconfig status    Display net interface config information\n");
}

void ifconfig_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
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

struct cli_command ifconfig_message_cmd[] = {
    { "ifconfig", "ifconfig command", ifconfig_command },
};

int ifconfig_cli_register( void )
{
    if( 0 == aos_cli_register_commands( ifconfig_message_cmd, 1 ) ) {
        return ERR_OK;
    }
    else {
        return ERR_VAL;
    }
}
