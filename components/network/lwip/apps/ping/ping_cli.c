#include "lwip/debug.h"
#include "lwip/err.h"
#include "aos/cli.h"
#include "aos/kernel.h"
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
    LWIP_DEBUGF( PING_DEBUG, ("Usage: ping  [-c count] [-i interval] [-s packetsize] [-w timeout] destination" ));
    LWIP_DEBUGF( PING_DEBUG, ("       ping  [-h]" ));
    LWIP_DEBUGF( PING_DEBUG, ("       ping  [-e]" ));
    LWIP_DEBUGF( PING_DEBUG, ("  -c,        Stop after sending count ECHO_REQUEST packets" ));
    LWIP_DEBUGF( PING_DEBUG, ("  -i,        Wait milliseconds between sending each packet" ));
    LWIP_DEBUGF( PING_DEBUG, ("  -s,        Specifies the number of data bytes to be sent" ));
    LWIP_DEBUGF( PING_DEBUG, ("  -w,        Time to wait for a response, in millisecond" ));
    LWIP_DEBUGF( PING_DEBUG, ("  -h,        Show help" ));
    LWIP_DEBUGF( PING_DEBUG, ("  -e,        Exit ping" ));
    LWIP_DEBUGF( PING_DEBUG, ("Example:" ));
    LWIP_DEBUGF( PING_DEBUG, ("ping www.aliyun.com" ));
    LWIP_DEBUGF( PING_DEBUG, ("ping -c 3 -i 100 -s 1300 -w 1000 www.aliyun.com" ));
}

void ping_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    int i = 0;
    if ( argc < 2 ) {
        LWIP_DEBUGF( PING_DEBUG, ("Invalid command") );
    }

    if ( strcmp( argv[1], "-h" ) == 0 ) {
        _cli_ping_help_command( argc - 2, &argv[2] );
    }
    else {
        _cli_ping_command( argc - 1, &argv[1] );
    }
}

int ping_cli_register( void )
{
    if( 0 == cli_register_commands( ping_message_cmd, 1 ) )
        return ERR_OK;
    else
        return ERR_VAL;
}
