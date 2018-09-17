/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/network.h>
#include <netmgr.h>
#include <sal_sockets.h>
#ifdef AOS_ATCMD
#include <atparser.h>
#endif

#define TAG "salserver"
#define SALSERVER_MAX_SIZE  1024
#define SALSERVER_TCP_SERVER_PORT 20001
#define SALSERVER_UDP_SERVER_PORT 20002

#define SALSERVER_UDP_REPLY "ACKed from UDP server!"
#define SALSERVER_TCP_REPLY "ACKed from TCP server!"

static int udp_server_sockfd = -1;
static int tcp_server_sockfd = -1;

static void close_udp_server()
{
    if ( udp_server_sockfd >= 0 ) {
        close(udp_server_sockfd);
    }

    udp_server_sockfd = -1;

    LOG("udp server close!\n");
}

static void close_tcp_server()
{
    if ( tcp_server_sockfd >= 0 ) {
        close(tcp_server_sockfd);
    }

    tcp_server_sockfd = -1;

    LOG("TCP server close!\n");
}

void tcp_client_task(void *arg)
{
    char buf[SALSERVER_MAX_SIZE];
    int len = 0;
    int fd =  *((int *)arg);
    fd_set readfds;

    LOG("New client task starts on socket %d\n", fd);

    while ( 1 ) {
        FD_ZERO( &readfds );
        FD_SET( fd, &readfds );

        select( fd + 1, &readfds, NULL, NULL, NULL );

        if ( FD_ISSET( fd, &readfds ) ) {
            len = recv( fd, buf, SALSERVER_MAX_SIZE, 0 );

            if ( len == 0 ) {
                goto exit;
            }

            buf[len] = '\0';
            LOG("tcp client %d recv len %d %s\n", fd, len, buf);
            //len = send( fd, buf, len, 0 );
        }
    }

exit:
    close(fd);
    aos_task_exit(0);
}

void start_tcp_server()
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen = sizeof(struct sockaddr_in);
    fd_set readfds;
    int clientfd = -1;

    if ( (tcp_server_sockfd = socket( AF_INET, SOCK_STREAM, 0 ))  < 0 ) {
        LOGE(TAG, "tcp socket create failed, errno = %d.", errno);
        goto exit;
    }

    memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons( SALSERVER_TCP_SERVER_PORT );

    if ( (bind( tcp_server_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) )) < 0 ) {
        LOGE(TAG, "tcp socket bind failed, errno = %d.", errno);
        goto exit;
    }

    LOG( "listen on  tcp socket!\n" );
    if  ( listen( tcp_server_sockfd, 0 )  < 0 ) {
        LOGE(TAG, "tcp socket listen failed, errno = %d.", errno);
        goto exit;
    }
    LOG("tcp server after listen!\n");

    while ( 1 ) {
        FD_ZERO( &readfds );
        FD_SET( tcp_server_sockfd, &readfds );

        LOG("tcp server before select\n");
        select( tcp_server_sockfd + 1, &readfds, NULL, NULL, NULL);

        if ( FD_ISSET( tcp_server_sockfd, &readfds ) ) {
            clientfd = accept( tcp_server_sockfd, (struct sockaddr *) &cliaddr, &clilen );

            if ( clientfd >= 0 ) {
                LOG("tcp server: before go in new client task %d\n", clientfd);
                aos_task_new( "tcp_client", tcp_client_task, (void *) &clientfd, 2048 );
            }
        }

    }

exit:
    close_tcp_server();
    aos_task_exit(0);
}

void start_udp_server()
{
    struct sockaddr_in servaddr, cliaddr;
    char buf[SALSERVER_MAX_SIZE];
    char reply[] = SALSERVER_UDP_REPLY;
    socklen_t clilen = sizeof(cliaddr);
    int recvlen = 0, sendlen = 0;

    if ( (udp_server_sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 ) {
        LOGE(TAG, "udp socket create failed, errno = %d.", errno);
        goto exit;
    }

    // Bind to port and any IP address
    memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons( SALSERVER_UDP_SERVER_PORT );

    if ( (bind( udp_server_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) )) < 0 ) {
        LOGE(TAG, "udp socket bind failed, errno = %d.", errno);
        goto exit;
    }

    LOG("UDP server start!\n");

    while ( 1 ) {
        recvlen = recvfrom( udp_server_sockfd, buf, SALSERVER_MAX_SIZE, 0,
                            (struct sockaddr *) &cliaddr, &clilen );
        if ( recvlen < 0 ) {
            LOGE(TAG, "udp recv failed, errno = %d.", errno);
            break;
        }

        buf[recvlen] = '\0';
        LOG("recv msg len %d : %s!\n", recvlen, buf);

        /*sendlen = sendto( udp_server_sockfd, reply, strlen(reply), 0,
                          (struct sockaddr *) &cliaddr, clilen );
        if ( sendlen <= 0 ) {
            LOGE(TAG, "udp send failed, errno = %d.", errno);
            break;
        }*/
    }

exit:
    close_udp_server();
    aos_task_exit(0);
}

static void handle_sal(char *pwbuf, int blen, int argc, char **argv)
{
    char *ptype = argc > 1 ? argv[1] : "default";

    if (strcmp(ptype, "tcp_s") == 0) {
        // TCP server
        aos_task_new("tcp_server", start_tcp_server, NULL, 4096);

    } else if (strcmp(ptype, "udp_s") == 0) {
        // UDP server
        aos_task_new("udp_server", start_udp_server, NULL, 4096);
        //start_udp_server();

    } else if (strcmp(ptype, "close") == 0) {
        close_udp_server();
        close_tcp_server();
    }
}

static struct cli_command salcmds[] = {
    {
        .name = "sal",
        .help = "sal tcp_s|udp_s|close",
        .function = handle_sal
    }
};

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code == CODE_WIFI_ON_PRE_GOT_IP) {
        LOG("Hello, WiFi PRE_GOT_IP event!");
    }

    if (event->code == CODE_WIFI_ON_GOT_IP) {
        aos_cli_register_commands((const struct cli_command *)&salcmds[0],
                                  sizeof(salcmds) / sizeof(salcmds[0]));
        LOG("Hello, WiFi GOT_IP event!");
    }
}

int application_start(int argc, char *argv[])
{
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);
#endif

#ifdef WITH_SAL
    sal_init();
    LOG("start sal");
#endif
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    netmgr_init();
    netmgr_start(false);

    aos_loop_run();

    return 0;
}
