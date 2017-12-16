/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include "lwip/sockets.h"
#include "mico.h"

//#define wlan_log(M, ...) custom_log("WLAN", M, ##__VA_ARGS__)
#define wlan_log(M, ...)

static char tcp_remote_ip[16] = "192.168.3.190";
static int  tcp_remote_port = 6000;

static void tcp_client_thread( uint32_t arg )
{
    UNUSED_PARAMETER(arg);

    OSStatus err;
    struct sockaddr_in addr;
    struct timeval  t;
    fd_set readfds;
    int    tcp_fd = -1, len;
    char  *buf = NULL;

    buf = (char *)malloc(1024);
    require_action(buf, exit, err = kNoMemoryErr);

re_connect:
    /* Connect to TCP Server. */
    tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    require_action(IsValidSocket(tcp_fd), exit, err = kNoResourcesErr);

    addr.sin_family  = AF_INET;
    addr.sin_addr.s_addr = inet_addr(tcp_remote_ip);
    addr.sin_port = htons(tcp_remote_port);

    wlan_log("Connecting to server: ip = %s port = %d",
             tcp_remote_ip, tcp_remote_port);
    err = connect(tcp_fd, (struct sockaddr *)&addr, sizeof(addr));
    require_noerr_action(err, exit, err = kConnectionErr);
    wlan_log("Connect success");

    t.tv_sec = 2;
    t.tv_usec = 0;

    /* Poll data */
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(tcp_fd, &readfds);

        require_action( select(tcp_fd + 1, &readfds, NULL, NULL, &t) >= 0, exit, err = kConnectionErr );

        if (FD_ISSET(tcp_fd, &readfds)) {
            len = recv(tcp_fd, buf, 1024, 0);
            require_action(len >= 0, exit, err = kConnectionErr);

            if (len == 0) {
                wlan_log("TCP_Client is disconnected, fd: %d", tcp_fd);
                err = kConnectionErr;
                goto exit;
            }
            len = send(tcp_fd, buf, len, 0);
        }
    }

exit:
    if ( err == kConnectionErr ) {
        SocketClose(&tcp_fd);
        mico_rtos_delay_milliseconds( 2 * 1000 );
        goto re_connect;
    }
    if (err != kNoErr) {
        wlan_log("TCP Client thread exit with err: %d", err);
    }
    if (buf != NULL) {
        free(buf);
    }
    SocketClose(&tcp_fd);
    mico_rtos_delete_thread(NULL);
}

OSStatus wlan_coexist_test_start( void )
{
    return mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "TCP Client", tcp_client_thread, 0x800, 0 );
}






