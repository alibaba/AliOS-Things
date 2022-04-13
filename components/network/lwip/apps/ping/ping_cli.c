
#include <string.h>
#include "lwip/debug.h"
#include "lwip/err.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
void ping_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );

struct cli_command ping_message_cmd[] = {
    { "ping", "ping app", ping_command },
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void _cli_ping_command( int argc, char **argv );
void _cli_ping_help_command( int argc, char **argv );
extern void ping_run( int argc, char **argv );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
static void _cli_ping_command( int argc, char **argv )
{
    return ping_run(argc, argv);
}
void _cli_ping_help_command( int argc, char **argv )
{
    aos_cli_printf("Usage: ping  [-c count] [-i interval] [-s packetsize] [-w timeout] destination\n");
    aos_cli_printf("       ping  [-h]\n");
    aos_cli_printf("       ping  [-e]\n");
    aos_cli_printf("  -c,        Stop after sending count ECHO_REQUEST packets\n");
    aos_cli_printf("  -i,        Wait milliseconds between sending each packet\n");
    aos_cli_printf("  -s,        Specifies the number of data bytes to be sent\n");
    aos_cli_printf("  -w,        Time to wait for a response, in millisecond\n");
    aos_cli_printf("  -h,        Show help\n");
    aos_cli_printf("  -e,        Exit ping\n");
    aos_cli_printf("Example:\n");
    aos_cli_printf("ping www.aliyun.com\n");
    aos_cli_printf("ping -c 3 -i 100 -s 1300 -w 1000 www.aliyun.com\n");
}

void ping_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    if ( argc < 2 ) {
         aos_cli_printf("Invalid command\n");
         _cli_ping_help_command( 0, NULL );
        return;
    }

    if ( strcmp( argv[1], "-h" ) == 0 ) {
        _cli_ping_help_command( argc - 2, &argv[2] );
    }
    else {
        _cli_ping_command( argc - 1, &argv[1] );
    }
}

#ifndef FEATURE_NETWORK_USPACE
static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }
}
#endif


int ping_cli_register( void )
{
#ifndef FEATURE_NETWORK_USPACE
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
#endif
    if( 0 == aos_cli_register_commands( ping_message_cmd, 1 ) )
        return ERR_OK;
    else
        return ERR_VAL;
}
