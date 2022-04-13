/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <network.h>
#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define PORT       6666
#define BUFSIZE    128

static void udp_server_task(void *data)
{
    char    buf[BUFSIZE+1];
    int     sockfd;
    struct  sockaddr_in svraddr = {0};

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    if (sockfd < 0) {
        return;
    }

    svraddr.sin_family=AF_INET;
    svraddr.sin_port=htons(PORT);
    inet_aton("127.0.0.1",&svraddr.sin_addr);

    int ret=bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr));
    if (ret<0) {
        printf("cannot bind!\r\n");
        close(sockfd);
        return;
    }

    while(1)
    {
        struct sockaddr_in cli;
        int len = sizeof(cli);
        int recv_len = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr*)&cli, &len);

        if (recv_len > 0) {
            buf[recv_len]='\0';
            printf("recv: %s\r\n",buf);
        }
    }

    close(sockfd);
}

static void udp_client_task(void *data)
{
    char *buf = "hello UDP";
    int sockfd=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in svraddr;
    int len = 0;

    if (sockfd < 0) {
        return;
    }

    svraddr.sin_family=AF_INET;
    svraddr.sin_port=htons(PORT);
    inet_aton("127.0.0.1",&svraddr.sin_addr);

    while(1) {
        aos_msleep(1000);

        len = sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*)&svraddr,sizeof(svraddr));
        if (len < strlen(buf)) {
            break;
        }
    }

    close(sockfd);
}

static void udp_example(int argc, char **argv)
{
    aos_task_new("udp_server", udp_server_task, NULL, 6 << 10);

    aos_task_new("udp_client", udp_client_task, NULL, 6 << 10);
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(udp_example, test_udp, udp_example)
#endif
