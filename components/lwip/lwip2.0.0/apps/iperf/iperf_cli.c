/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
#if AOS_COMP_CLI
#include <stdlib.h>
#include <string.h>
#include "lwip/opt.h"
#include "iperf_wrapper.h"
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
    g_iperf_param = iperf_wrapper_malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL ) {
        aos_cli_printf("Warning: No enough memory to running iperf\n" );
        return;
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );

    for ( i = 0; i < argc; i++ ) {
        strncpy( (char *) &g_iperf_param[i * offset], argv[i] , IPERF_COMMAND_BUFFER_SIZE);
#if defined(IPERF_DEBUG_INTERNAL)
        aos_cli_printf("_cli_iperf_client, g_iperf_param[%d] is \"%s\"\n", i, (char *)&g_iperf_param[i * offset]);
#endif
    }

    for ( i = 0; i < argc; i++ ) {
        if ( strcmp( argv[i], "-u" ) == 0 ) {
            aos_cli_printf("Iperf UDP Server: Start!\r\n" );
            aos_cli_printf("Iperf UDP Server Receive Timeout = 20 (secs)\n" );
            aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_udp_run_server_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
            is_create_task = 1;
            break;
        }
    }
    if ( is_create_task != 1 ) {
        aos_cli_printf("Iperf TCP Server: Start!\n" );
        aos_cli_printf("Iperf TCP Server Receive Timeout = 20 (secs)\n" );
        aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_tcp_run_server_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
        is_create_task = 1;
    }

}

static void _cli_iperf_client_command( int argc, char **argv )
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);

    g_iperf_param = iperf_wrapper_malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL ) {
        aos_cli_printf("Warning: No enough memory to running iperf.\n" );
        return;
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    for ( i = 0; i < argc; i++ ) {
        strncpy( (char *) &g_iperf_param[i * offset], argv[i], IPERF_COMMAND_BUFFER_SIZE);
#if defined(IPERF_DEBUG_INTERNAL)
        aos_cli_printf("_cli_iperf_client, g_iperf_param[%d] is \"%s\"\n", i, (char *)&g_iperf_param[i * offset]);
#endif
    }

    for ( i = 0; i < argc; i++ ) {
        if ( strcmp( argv[i], "-u" ) == 0 ) {
            aos_cli_printf("Iperf UDP Client: Start!\n" );
            aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_udp_run_client_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
            is_create_task = 1;
            break;
        }
    }


    if ( is_create_task != 1 ) {
        aos_cli_printf("Iperf TCP Client: Start!\n" );
        aos_task_new_ext(&aos_iperf_task, IPERF_NAME, iperf_tcp_run_client_thread, (void*)g_iperf_param, IPERF_STACKSIZE, IPERF_PRIO);
        is_create_task = 1;
    }

}

static void _cli_iperf_help_command( int argc, char **argv )
{
    aos_cli_printf("Usage: iperf [-s|-c] [options]\n");
    aos_cli_printf("       iperf [-h]\n");
    aos_cli_printf("Client/Server:\n");
    aos_cli_printf("  -u,        use UDP rather than TCP\n");
    aos_cli_printf("  -p,        #server port to listen on/connect to (default 5001)\n");
    aos_cli_printf("  -n,        #[kmKM]    number of bytes to transmit\n");
    aos_cli_printf("  -b,        #[kmKM]    for UDP, bandwidth to send at in bits/sec\n");
    aos_cli_printf("  -i,        10 seconds between periodic bandwidth reports\n");
    aos_cli_printf("Server specific:\n");
    aos_cli_printf("  -s,        run in server mode\n");
    aos_cli_printf("  -B,        <ip>    bind to <ip>, and join to a multicast group (only Support UDP)\n");
    aos_cli_printf("  -r,        for UDP, run iperf in tradeoff testing mode, connecting back to client\n");
    aos_cli_printf("Client specific:\n");
    aos_cli_printf("  -c,        <ip>run in client mode, connecting to <ip>\n");
    aos_cli_printf("  -w,        #[kmKM]    TCP window size\n");
    aos_cli_printf("  -l,        #[kmKM]    UDP datagram size\n");
    aos_cli_printf("  -t,        #time in seconds to transmit for (default 10 secs)\n");
    aos_cli_printf("  -S,        #the type-of-service of outgoing packets\n");
    aos_cli_printf("Miscellaneous:\n" );
    aos_cli_printf("  -h,        print this message and quit\n");
    aos_cli_printf("[kmKM] Indicates options that support a k/K or m/M suffix for kilo- or mega-\n");
    aos_cli_printf("TOS options for -S parameter:\n");
    aos_cli_printf("BE: -S 0\n");
    aos_cli_printf("BK: -S 32\n");
    aos_cli_printf("VI: -S 160\n");
    aos_cli_printf("VO: -S 224\n");
    aos_cli_printf("Tradeoff Testing Mode:\n");
    aos_cli_printf("Command: iperf -s -u -n <bits/bytes> -r\n");
    aos_cli_printf("Example:\n");
    aos_cli_printf("Iperf TCP Server: iperf -s\n");
    aos_cli_printf("Iperf UDP Server: iperf -s -u\n");
    aos_cli_printf("Iperf TCP Client: iperf -c <ip> -w <window size> -t <duration> -p <port>\n");
    aos_cli_printf("Iperf UDP Client: iperf -c <ip> -u -l <datagram size> -t <duration> -p <port>\n");
}

#if defined(IPERF_DEBUG_ENABLE)
extern void iperf_set_debug_mode(uint32_t debug);
static uint8_t _cli_iperf_debug(int argc, char **argv)
{
    int debug;

    if(argc != 1) {
        aos_cli_printf("Invalid iperf debug command\n");
        return -1;
    }
    debug = atoi(argv[0]);
    aos_cli_printf("Set iperf debug to %d(0x%x)\n", debug, debug);
    iperf_set_debug_mode(debug);
    return 0;
}
#endif

void iperf_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    if ( argc < 2 ) {
        aos_cli_printf("Invalid command\n");
        _cli_iperf_help_command( 0, NULL );
        return;
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
    if( 0 == aos_cli_register_commands( iperf_test_message_cmd, 1 ) )
        return ERR_OK;
    else
        return ERR_VAL;
}
#endif /* AOS_COMP_CLI */
