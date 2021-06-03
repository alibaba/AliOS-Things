
#if AOS_COMP_CLI
#include <string.h>
#include <stdbool.h>
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/debug.h"
#include "lwip/err.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#include "uservice/uservice.h"
#include "uservice/eventid.h"
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

/******************************************************
 *               Function Declarations
 ******************************************************/
static void _cli_ping_cmd( int argc, char **argv );
void _cli_ping_help_command( int argc, char **argv );
extern void ping_run( int argc, char **argv );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
static void _cli_ping_cmd( int argc, char **argv )
{
    return ping_run(argc, argv);
}
void _cli_ping_help_cmd( int argc, char **argv )
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

static int ping_cmd(int argc, char **argv )
{
	struct netif *net_if = NULL;

    NETIF_FOREACH(net_if) {
        if (netif_is_up(net_if) && netif_is_link_up(net_if)
            && !ip_addr_isany(&net_if->ip_addr)
            && !ip_addr_isloopback(&net_if->ip_addr)) {
            break;
        }
    }

    if(net_if == NULL) {
        aos_cli_printf("Connect network first!\n");
        return -1;
    }

    if ( argc < 2 ) {
         aos_cli_printf("Invalid command\n");
         _cli_ping_help_cmd( 0, NULL );
        return -1;
    }

    if ( strcmp( argv[1], "-h" ) == 0 ) {
        _cli_ping_help_cmd( argc - 2, &argv[2] );
    }
    else {
        _cli_ping_cmd( argc - 1, &argv[1] );
    }
    return 0;
}

/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ping_cmd, ping, Ping command)
#endif /* AOS_COMP_CLI */
