/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI
#include <string.h>
#include <stdbool.h>
#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/err.h"
#include <aos/cli.h>
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
void sendfile_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
extern int sendfile_client(int argc,char *argv[]);
extern int sendfile_server(int argc, char **argv);

struct cli_command sendfile_message_cmd[] = {
    { "sendfile", "sendfile client/server app", sendfile_command },
};

/******************************************************
 *               Function Declarations
 ******************************************************/
void sendfile_help( int argc, char **argv );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

void sendfile_help( int argc, char **argv )
{
    LWIP_DEBUGF( SENDFILE_DEBUG, ("Usage: sendfile -c/-s [Options]\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("       sendfile  [-h]\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("Server specific:\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -s,        sendfile server\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -d,        destination addr\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -p,        destination port\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -f,        to download file from server\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -n,        to save file\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("Client specific:\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -c,        sendfile client\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -p,        source listening port\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("Miscellaneous:\n" ));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("  -h,        Show help\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("Example:\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("sendfile -c -d 192.168.0.1 -p 12345 -f 1.txt -n 2.txt\n"));
    LWIP_DEBUGF( SENDFILE_DEBUG, ("sendfile -s -p 12345\n"));
}

static bool      m_got_ip = false;

void sendfile_command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    if ( m_got_ip == false ) {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("Connect network first!\n"));
        return;
    }

    if ( argc < 2 ) {
         LWIP_DEBUGF( SENDFILE_DEBUG, ("Invalid command\n"));
         sendfile_help( 0, NULL );
        return;
    }

    if ( strcmp( argv[1], "-c" ) == 0 ) {
        sendfile_client( argc - 1, &argv[1] );
    }
    else if ( strcmp( argv[1], "-s" ) == 0 ) {
        sendfile_server( argc - 1, &argv[1] );
    }
    else {
        sendfile_help( argc - 1, &argv[1] );
    }
}

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id != EVENT_NETMGR_DHCP_SUCCESS) {
        return;
    }

    m_got_ip = true;
}

int sendfile_cli_register( void )
{
    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);

    if( 0 == aos_cli_register_commands( sendfile_message_cmd, 1 ) ) {
        return ERR_OK;
    }
    else {
        return ERR_VAL;
    }
}
#endif /* AOS_COMP_CLI */
