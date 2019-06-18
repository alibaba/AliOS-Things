#include "lwip/debug.h"
#include "lwip/err.h"
#include "lwip/apps/iperf_debug.h"
#include "lwip/apps/iperf_task.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define TAG "IPERF_CLI"
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
void iperf_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );

struct cli_command iperf_test_message_cmd[] = {
    { "iperf", "iperf test", iperf_Command },
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void iperf_udp_run_server_thread( void* arg );
static void iperf_tcp_run_server_thread( void* arg );
static void iperf_udp_run_client_thread( void* arg );
static void iperf_tcp_run_client_thread( void* arg );

static void _cli_iperf_server_Command( int argc, char **argv );
static void _cli_iperf_client_Command( int argc, char **argv );
static void _cli_iperf_help_Command( int argc, char **argv );

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

static void _cli_iperf_server_Command( int argc, char **argv )
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    g_iperf_param = malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL )
    {
        LOGD(TAG, "Warning: No enough memory to running iperf.\r\n" );
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );

    for ( i = 0; i < argc; i++ )
    {
        strcpy( (char *) &g_iperf_param[i * offset], argv[i] );
#if defined(IPERF_DEBUG_INTERNAL)
        LOGD(TAG, "_cli_iperf_client, g_iperf_param[%d] is \"%s\"\r\n", i, (char *)&g_iperf_param[i * offset]);
#endif
    }

    for ( i = 0; i < argc; i++ )
    {
        if ( strcmp( argv[i], "-u" ) == 0 )
        {
            LOGD(TAG, "Iperf UDP Server: Start!\r\n" );
            LOGD(TAG, "Iperf UDP Server Receive Timeout = 20 (secs)\r\n" );
            aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_udp_run_server_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
            is_create_task = 1;
            break;
        }
    }
    if ( strcmp( argv[i], "-u" ) != 0 )
    {
        LOGD(TAG, "Iperf TCP Server: Start!\r\n" );
        LOGD(TAG, "Iperf TCP Server Receive Timeout = 20 (secs)\r\n" );
        aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_tcp_run_server_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
        is_create_task = 1;
    }

    if ( is_create_task == 0 )
    {
        free( g_iperf_param );
    }
}

static void _cli_iperf_client_Command( int argc, char **argv )
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    g_iperf_param = malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL )
    {
        LOGD(TAG, "Warning: No enough memory to running iperf.\r\n" );
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    for ( i = 0; i < argc; i++ )
    {
        strcpy( (char *) &g_iperf_param[i * offset], argv[i] );
#if defined(IPERF_DEBUG_INTERNAL)
        LOGD(TAG, "_cli_iperf_client, g_iperf_param[%d] is \"%s\"\r\n", i, (char *)&g_iperf_param[i * offset]);
#endif
    }

    for ( i = 0; i < argc; i++ )
    {
        if ( strcmp( argv[i], "-u" ) == 0 )
        {
            LOGD(TAG, "Iperf UDP Client: Start!\r\n" );
            aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_udp_run_client_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
            is_create_task = 1;
            break;
        }
    }

    if ( i == argc )
    {
        LOGD(TAG, "Iperf TCP Client: Start!\r\n" );
        aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_tcp_run_client_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
        is_create_task = 1;
    }

    if ( is_create_task == 0 )
    {
        free( g_iperf_param );
    }
}

static void _cli_iperf_help_Command( int argc, char **argv )
{
    LOGD(TAG, "Usage: iperf [-s|-c] [options]\r\n" );
    LOGD(TAG, "       iperf [-h]\r\n\n" );
    LOGD(TAG, "Client/Server:\r\n" );
    LOGD(TAG, "  -u,        use UDP rather than TCP\r\n" );
    LOGD(TAG, "  -p,        #server port to listen on/connect to (default 5001)\r\n" );
    LOGD(TAG, "  -n,        #[kmKM]    number of bytes to transmit \r\n" );
    LOGD(TAG, "  -b,        #[kmKM]    for UDP, bandwidth to send at in bits/sec\r\n" );
    LOGD(TAG, "  -i,        10 seconds between periodic bandwidth reports \r\n\n" );
    LOGD(TAG, "Server specific:\r\n" );
    LOGD(TAG, "  -s,        run in server mode\r\n" );
    LOGD(TAG, "  -B,        <ip>    bind to <ip>, and join to a multicast group (only Support UDP)\r\n" );
    LOGD(TAG, "  -r,        for UDP, run iperf in tradeoff testing mode, connecting back to client\r\n\n" );
    LOGD(TAG, "Client specific:\r\n" );
    LOGD(TAG, "  -c,        <ip>run in client mode, connecting to <ip>\r\n" );
    LOGD(TAG, "  -w,        #[kmKM]    TCP window size\r\n" );
    LOGD(TAG, "  -l,        #[kmKM]    UDP datagram size\r\n" );
    LOGD(TAG, "  -t,        #time in seconds to transmit for (default 10 secs)\r\n" );
    LOGD(TAG, "  -S,        #the type-of-service of outgoing packets\r\n\n" );
    LOGD(TAG, "Miscellaneous:\r\n" );
    LOGD(TAG, "  -h,        print this message and quit\r\n\n" );
    LOGD(TAG, "[kmKM] Indicates options that support a k/K or m/M suffix for kilo- or mega-\r\n\n" );
    LOGD(TAG, "TOS options for -S parameter:\r\n" );
    LOGD(TAG, "BE: -S 0\r\n" );
    LOGD(TAG, "BK: -S 32\r\n" );
    LOGD(TAG, "VI: -S 160\r\n" );
    LOGD(TAG, "VO: -S 224\r\n\n" );
    LOGD(TAG, "Tradeoff Testing Mode:\r\n" );
    LOGD(TAG, "Command: iperf -s -u -n <bits/bytes> -r \r\n\n" );
    LOGD(TAG, "Example:\r\n" );
    LOGD(TAG, "Iperf TCP Server: iperf -s\r\n" );
    LOGD(TAG, "Iperf UDP Server: iperf -s -u\r\n" );
    LOGD(TAG, "Iperf TCP Client: iperf -c <ip> -w <window size> -t <duration> -p <port> \r\n" );
    LOGD(TAG, "Iperf UDP Client: iperf -c <ip> -u -l <datagram size> -t <duration> -p <port>\r\n" );
}

#if defined(IPERF_DEBUG_ENABLE)
static uint8_t _cli_iperf_debug(int argc, char **argv)
{
    int debug;
    debug = atoi(argv[0]);
    LOGD(TAG, "Set iperf debug to %d(0x%x)\r\n", debug, debug);
    iperf_set_debug_mode(debug);
    return 0;
}
#endif

void iperf_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    if ( argc < 2 )
    {
        LOGD(TAG, "Invalid command\r\n" );
    }
    if ( strcmp( argv[1], "-s" ) == 0 )
    {
        _cli_iperf_server_Command( argc - 2, &argv[2] );
    }
    else
    if ( strcmp( argv[1], "-c" ) == 0 )
    {
        _cli_iperf_client_Command( argc - 2, &argv[2] );
    }
    else
    if ( strcmp( argv[1], "-h" ) == 0 )
    {
        _cli_iperf_help_Command( argc - 2, &argv[2] );
    }
#if defined(IPERF_DEBUG_ENABLE)
    else
    if ( strcmp( argv[1], "-d" ) == 0 )
    {
        _cli_iperf_debug( argc - 2, &argv[2] );
    }
#endif
}


int iperf_cli_register( void )
{
    if( 0 == aos_cli_register_commands( iperf_test_message_cmd, 1 ) )
        return ERR_OK;
    else
        return ERR_VAL;
}

