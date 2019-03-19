/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <network/network.h>

#include "aos/errno.h"
#include "aos/kernel.h"
#include "aos/yloop.h"

#include <netmgr.h>

/* nbio testcase*/
char *server_name = "8.8.8.8";
int server_port = 80;

/* sack option testcase*/
#if 0
char *server_name = "106.187.96.148";
int server_port = 80;
#endif

char http_request[]={0x47,0x45,0x54,0x20,0x2f,0x31,0x30,0x30,0x4d,0x42,0x2d,0x74,0x6f,0x6b,0x79,0x6f
,0x2e,0x62,0x69,0x6e,0x20,0x48,0x54,0x54,0x50,0x2f,0x31,0x2e,0x31,0x0d,0x0a,0x48
,0x6f,0x73,0x74,0x3a,0x20,0x73,0x70,0x65,0x65,0x64,0x74,0x65,0x73,0x74,0x2e,0x74
,0x6f,0x6b,0x79,0x6f,0x2e,0x6c,0x69,0x6e,0x6f,0x64,0x65,0x2e,0x63,0x6f,0x6d,0x0d
,0x0a,0x43,0x6f,0x6e,0x6e,0x65,0x63,0x74,0x69,0x6f,0x6e,0x3a,0x20,0x6b,0x65,0x65
,0x70,0x2d,0x61,0x6c,0x69,0x76,0x65,0x0d,0x0a,0x55,0x70,0x67,0x72,0x61,0x64,0x65
,0x2d,0x49,0x6e,0x73,0x65,0x63,0x75,0x72,0x65,0x2d,0x52,0x65,0x71,0x75,0x65,0x73
,0x74,0x73,0x3a,0x20,0x31,0x0d,0x0a,0x55,0x73,0x65,0x72,0x2d,0x41,0x67,0x65,0x6e
,0x74,0x3a,0x20,0x4d,0x6f,0x7a,0x69,0x6c,0x6c,0x61,0x2f,0x35,0x2e,0x30,0x20,0x28
,0x57,0x69,0x6e,0x64,0x6f,0x77,0x73,0x20,0x4e,0x54,0x20,0x36,0x2e,0x31,0x3b,0x20
,0x57,0x4f,0x57,0x36,0x34,0x29,0x20,0x41,0x70,0x70,0x6c,0x65,0x57,0x65,0x62,0x4b
,0x69,0x74,0x2f,0x35,0x33,0x37,0x2e,0x33,0x36,0x20,0x28,0x4b,0x48,0x54,0x4d,0x4c
,0x2c,0x20,0x6c,0x69,0x6b,0x65,0x20,0x47,0x65,0x63,0x6b,0x6f,0x29,0x20,0x43,0x68
,0x72,0x6f,0x6d,0x65,0x2f,0x37,0x32,0x2e,0x30,0x2e,0x33,0x36,0x32,0x36,0x2e,0x39
,0x36,0x20,0x53,0x61,0x66,0x61,0x72,0x69,0x2f,0x35,0x33,0x37,0x2e,0x33,0x36,0x0d
,0x0a,0x41,0x63,0x63,0x65,0x70,0x74,0x3a,0x20,0x74,0x65,0x78,0x74,0x2f,0x68,0x74
,0x6d,0x6c,0x2c,0x61,0x70,0x70,0x6c,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x2f,0x78
,0x68,0x74,0x6d,0x6c,0x2b,0x78,0x6d,0x6c,0x2c,0x61,0x70,0x70,0x6c,0x69,0x63,0x61
,0x74,0x69,0x6f,0x6e,0x2f,0x78,0x6d,0x6c,0x3b,0x71,0x3d,0x30,0x2e,0x39,0x2c,0x69
,0x6d,0x61,0x67,0x65,0x2f,0x77,0x65,0x62,0x70,0x2c,0x69,0x6d,0x61,0x67,0x65,0x2f
,0x61,0x70,0x6e,0x67,0x2c,0x2a,0x2f,0x2a,0x3b,0x71,0x3d,0x30,0x2e,0x38,0x0d,0x0a
,0x41,0x63,0x63,0x65,0x70,0x74,0x2d,0x45,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3a
,0x20,0x67,0x7a,0x69,0x70,0x2c,0x20,0x64,0x65,0x66,0x6c,0x61,0x74,0x65,0x0d,0x0a
,0x41,0x63,0x63,0x65,0x70,0x74,0x2d,0x4c,0x61,0x6e,0x67,0x75,0x61,0x67,0x65,0x3a
,0x20,0x7a,0x68,0x2d,0x43,0x4e,0x2c,0x7a,0x68,0x3b,0x71,0x3d,0x30,0x2e,0x39,0x0d
,0x0a,0x0d,0x0a};

void *network_socket_create(const char *net_addr, int port)
{
    int ret;
    int tcp_fd;
    struct hostent *host;
    struct sockaddr_in saddr;
    int opt_val = 1;

    set_errno(0);
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd < 0) {
        printf("creat socket fail - errno: %d\n", errno);
        return NULL;
    }

    host = gethostbyname(net_addr);
    if (host == NULL) {
        printf("get host by name fail - errno: %d\n", errno);
        goto _err;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = *(unsigned long *)(host->h_addr);

    //setsockopt(tcp_fd, SOL_SOCKET, SO_NBIO, NULL, 0);
    //setsockopt(tcp_fd, SOL_SOCKET, SO_BIO, NULL, 0);
    //setsockopt(tcp_fd, SOL_SOCKET, SO_NONBLOCK, &opt_val, sizeof(opt_val));
    opt_val = 0;
    setsockopt(tcp_fd, SOL_SOCKET, SO_NONBLOCK, &opt_val, sizeof(opt_val));

    //SACK test case
#if 0
    setsockopt(tcp_fd, SOL_SOCKET, SO_TCPSACK, &opt_val, sizeof(opt_val));
#endif

    printf("socket enter NBIO mode\n");
    do {
        set_errno(0);
        printf("socket try to connect\n");
        ret = connect(tcp_fd, (struct sockaddr *)&saddr, sizeof(saddr));
        if (ret < 0 && ( errno != EAGAIN && errno != EINPROGRESS && errno != EALREADY)) {
            printf("socket connect fail - errno: %d\n", errno);
            goto _err;
        }
    } while (ret < 0);

    return (void *)tcp_fd;

_err:
    close(tcp_fd);

    return (void *) - 1;
}

void network_socket_destroy(void *tcp_fd)
{
    if (tcp_fd == NULL) {
        return;
    }

    close((int)tcp_fd);

    return;
}

static void tcp_client_sample(void)
{
    int ret = 0;
    char buf[128];
    void *tcp_fd = NULL;
    //int total_len = 0;
    
    tcp_fd = network_socket_create(server_name, server_port);
    
    if ((int)tcp_fd < 0) {
        printf("tcp client connect fail\n");
        return;
    } else {
        printf("network socket create ok\n");
    }
	/* sack option testcase */
#if 0
    /* send http data*/
    if(send(tcp_fd, http_request, sizeof(http_request), 0) == -1) {
      printf("send http request failed\n");
      return;
    }

    recv_data_flag = 1;
    while(1)
    {
      char buf[6000];
      int len;

      printf("try to recv http data ...\n");
      if((len = recv(tcp_fd, buf, sizeof(buf), 0)) == -1) {
        printf("recv http data failed\n");
        return;
      }
      total_len += len;
      printf("recved (%d) http data ...\n", total_len);
      aos_msleep(50);
    }
#endif
    if (tcp_fd != NULL) {
        network_socket_destroy(tcp_fd);
    }

    return;
}

static void app_delayed_action(void *arg)
{
    printf("===========> TLS Client Sample start.\n");
    tcp_client_sample();
    printf("<=========== TLS Client Sample End.\n\n");

}

static void handle_event(input_event_t *event, void *arg)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    aos_post_delayed_action(1000, app_delayed_action, arg);
}

int application_start(int argc, char **argv)
{
#ifdef WITH_SAL
    sal_init();
#endif

    aos_register_event_filter(EV_WIFI, handle_event, NULL);

    netmgr_init();
#if !defined(STM32_USE_SPI_WIFI)
    netmgr_start(true);
#else
    netmgr_start(false);
#endif

    aos_loop_run();
    /* never return */

    return 0;
}

