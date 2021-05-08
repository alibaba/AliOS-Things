
#if AOS_COMP_CLI
#include <string.h>
#include <stdbool.h>
#include "lwip/opt.h"
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
    LWIP_DEBUGF( PING_DEBUG, ("Usage: ping  [-c count] [-i interval] [-s packetsize] [-w timeout] destination\n"));
    LWIP_DEBUGF( PING_DEBUG, ("       ping  [-h]\n"));
    LWIP_DEBUGF( PING_DEBUG, ("       ping  [-e]\n"));
    LWIP_DEBUGF( PING_DEBUG, ("  -c,        Stop after sending count ECHO_REQUEST packets\n"));
    LWIP_DEBUGF( PING_DEBUG, ("  -i,        Wait milliseconds between sending each packet\n"));
    LWIP_DEBUGF( PING_DEBUG, ("  -s,        Specifies the number of data bytes to be sent\n"));
    LWIP_DEBUGF( PING_DEBUG, ("  -w,        Time to wait for a response, in millisecond\n"));
    LWIP_DEBUGF( PING_DEBUG, ("  -h,        Show help\n"));
    LWIP_DEBUGF( PING_DEBUG, ("  -e,        Exit ping\n"));
    LWIP_DEBUGF( PING_DEBUG, ("Example:\n"));
    LWIP_DEBUGF( PING_DEBUG, ("ping www.aliyun.com\n"));
    LWIP_DEBUGF( PING_DEBUG, ("ping -c 3 -i 100 -s 1300 -w 1000 www.aliyun.com\n"));
}

static bool      m_got_ip = false;

void ping_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    if ( m_got_ip == false ) {
        LWIP_DEBUGF( PING_DEBUG, ("Connect network first!\n"));
        return;
    }

    if ( argc < 2 ) {
         LWIP_DEBUGF( PING_DEBUG, ("Invalid command\n"));
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

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id != EVENT_NETMGR_DHCP_SUCCESS) {
        return;
    }

    m_got_ip = true;
}

int ping_cli_register( void )
{
    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);

    if( 0 == cli_register_commands( ping_message_cmd, 1 ) )
        return ERR_OK;
    else
        return ERR_VAL;
}
#endif /* AOS_COMP_CLI */
