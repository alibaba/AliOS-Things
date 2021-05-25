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
    LWIP_DEBUGF( IFCONFIG_DEBUG, ("Usage: ifconfig\n"));
    LWIP_DEBUGF( IFCONFIG_DEBUG, ("Example:\n"));
    LWIP_DEBUGF( IFCONFIG_DEBUG, ("ifconfig           Display net interface config information\n"));
    LWIP_DEBUGF( IFCONFIG_DEBUG, ("ifconfig status    Display net interface config information\n"));
}

void ifconfig_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    if ( argc < 1 ) {
        LWIP_DEBUGF(IFCONFIG_DEBUG, ("Invalid command\n"));
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
    if( 0 == cli_register_commands( ifconfig_message_cmd, 1 ) ) {
        return ERR_OK;
    }
    else {
        return ERR_VAL;
    }
}
#endif /* AOS_COMP_CLI */
