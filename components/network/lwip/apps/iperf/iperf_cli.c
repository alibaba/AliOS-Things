#include "lwip/debug.h"
#include "lwip/err.h"
#include "lwip/apps/iperf_debug.h"
#include "lwip/apps/iperf_task.h"
#include "aos/cli.h"
#include "aos/kernel.h"
/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
static aos_task_t   aos_iperf_task;

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
void iperf_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );

struct cli_command iperf_test_message_cmd[] = {
    { "iperf", "iperf test", iperf_command },
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void iperf_udp_run_server_thread( void* arg );
static void iperf_tcp_run_server_thread( void* arg );
static void iperf_udp_run_client_thread( void* arg );
static void iperf_tcp_run_client_thread( void* arg );

static void _cli_iperf_server_command( int argc, char **argv );
static void _cli_iperf_client_command( int argc, char **argv );
static void _cli_iperf_help_command( int argc, char **argv );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static void iperf_udp_run_server_thread( void* arg )
{
    iperf_udp_run_server( (char **) arg );
}

static void iperf_tcp_run_server_thread( void* arg )
{
    iperf_tcp_run_server( (char **) arg );
}

static void iperf_udp_run_client_thread( void* arg )
{
    iperf_udp_run_client( (char **) arg );
}

static void iperf_tcp_run_client_thread( void* arg )
{
    iperf_tcp_run_client( (char **) arg );
}

static void _cli_iperf_server_command( int argc, char **argv )
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    g_iperf_param = malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Warning: No enough memory to running iperf" ));
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );

    for ( i = 0; i < argc; i++ ) {
        strcpy( (char *) &g_iperf_param[i * offset], argv[i] );
#if defined(IPERF_DEBUG_INTERNAL)
        LWIP_DEBUGF( IPERF_DEBUG, ("_cli_iperf_client, g_iperf_param[%d] is \"%s\"", i, (char *)&g_iperf_param[i * offset]));
#endif
    }

    for ( i = 0; i < argc; i++ ) {
        if ( strcmp( argv[i], "-u" ) == 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("Iperf UDP Server: Start!\r\n" ));
            LWIP_DEBUGF( IPERF_DEBUG, ("Iperf UDP Server Receive Timeout = 20 (secs)" ));
            aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_udp_run_server_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
            is_create_task = 1;
            break;
        }
    }

    if ( is_create_task != 1 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Iperf TCP Server: Start!" ));
        LWIP_DEBUGF( IPERF_DEBUG, ("Iperf TCP Server Receive Timeout = 20 (secs)" ));
        aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_tcp_run_server_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
    }

}

static void _cli_iperf_client_command( int argc, char **argv )
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    g_iperf_param = malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Warning: No enough memory to running iperf." ));
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    for ( i = 0; i < argc; i++ ) {
        strcpy( (char *) &g_iperf_param[i * offset], argv[i] );
#if defined(IPERF_DEBUG_INTERNAL)
        LWIP_DEBUGF( IPERF_DEBUG, ("_cli_iperf_client, g_iperf_param[%d] is \"%s\"", i, (char *)&g_iperf_param[i * offset]));
#endif
    }

    for ( i = 0; i < argc; i++ ) {
        if ( strcmp( argv[i], "-u" ) == 0 ) {
            LWIP_DEBUGF( IPERF_DEBUG, ("Iperf UDP Client: Start!" ));
            aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_udp_run_client_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
            is_create_task = 1;
            break;
        }
    }

    if ( is_create_task != 1 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Iperf TCP Client: Start!" ));
        aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_tcp_run_client_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
    }

}

static void _cli_iperf_help_command( int argc, char **argv )
{
    LWIP_DEBUGF( IPERF_DEBUG, ("Usage: iperf [-s|-c] [options]" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("       iperf [-h]" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Client/Server:" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -u,        use UDP rather than TCP" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -p,        #server port to listen on/connect to (default 5001)" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -n,        #[kmKM]    number of bytes to transmit" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -b,        #[kmKM]    for UDP, bandwidth to send at in bits/sec" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -i,        10 seconds between periodic bandwidth reports\n" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Server specific:" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -s,        run in server mode" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -B,        <ip>    bind to <ip>, and join to a multicast group (only Support UDP)" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -r,        for UDP, run iperf in tradeoff testing mode, connecting back to client\n" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Client specific:" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -c,        <ip>run in client mode, connecting to <ip>" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -w,        #[kmKM]    TCP window size" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -l,        #[kmKM]    UDP datagram size" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -t,        #time in seconds to transmit for (default 10 secs)" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -S,        #the type-of-service of outgoing packets\n" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Miscellaneous:" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("  -h,        print this message and quit\n" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("[kmKM] Indicates options that support a k/K or m/M suffix for kilo- or mega-\n" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("TOS options for -S parameter:" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("BE: -S 0" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("BK: -S 32" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("VI: -S 160" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("VO: -S 224\n" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Tradeoff Testing Mode:" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Command: iperf -s -u -n <bits/bytes> -r \n" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Example:" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Iperf TCP Server: iperf -s" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Iperf UDP Server: iperf -s -u" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Iperf TCP Client: iperf -c <ip> -w <window size> -t <duration> -p <port>" ));
    LWIP_DEBUGF( IPERF_DEBUG, ("Iperf UDP Client: iperf -c <ip> -u -l <datagram size> -t <duration> -p <port>" ));
}

#if defined(IPERF_DEBUG_ENABLE)
static uint8_t _cli_iperf_debug(int argc, char **argv)
{
    int debug;

    if(argc != 1) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Invalid iperf debug command"));
        return -1;
    }
    debug = atoi(argv[0]);
    LWIP_DEBUGF( IPERF_DEBUG, ("Set iperf debug to %d(0x%x)", debug, debug));
    iperf_set_debug_mode(debug);
    return 0;
}
#endif

void iperf_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    if ( argc < 2 ) {
        LWIP_DEBUGF( IPERF_DEBUG, ("Invalid command" ));
    }

    if ( strcmp( argv[1], "-s" ) == 0 ) {
        _cli_iperf_server_command( argc - 2, &argv[2] );
    }
    else if ( strcmp( argv[1], "-c" ) == 0 ) {
        _cli_iperf_client_command( argc - 2, &argv[2] );
    }
    else if ( strcmp( argv[1], "-h" ) == 0 ) {
        _cli_iperf_help_command( argc - 2, &argv[2] );
    }
#if defined(IPERF_DEBUG_ENABLE)
    else if ( strcmp( argv[1], "-d" ) == 0 ) {
        _cli_iperf_debug( argc - 2, &argv[2] );
    }
#endif
}


int iperf_cli_register( void )
{
    if( 0 == cli_register_commands( iperf_test_message_cmd, 1 ) )
        return ERR_OK;
    else
        return ERR_VAL;
}

