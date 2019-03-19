#include "time.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <lwip/sockets.h>
#include <lwip/debug.h>
#include <lwip/dns.h>
#include <lwip/netdb.h>
#include <lwip/api.h>

#define PORT 1234

#define MAXDATASIZE 100

#define BUF_SIZE 1024

const char *ipaddr="49.221.198.82";

char buf[BUF_SIZE];
int sockfd, num;
struct hostent *he;
struct sockaddr_in server, peer;
int read_len;
int send_bytes = 0;
int recv_bytes = 0;
int total_send_bytes = 0;
int total_recv_bytes = 0;
int file_size;
int round;


void udp_client(void)
{
    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() error\n");
        return;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    server.sin_addr.s_addr = inet_addr(ipaddr);
    //server.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("connect() error.\n");
        goto out;
    }

    printf("connected to server\r\n");

    memset(buf, 0, BUF_SIZE);
    //sprintf(buf, "%s", "start to transfer");
    buf[0] = 'o';
    buf[1] = 'k';
    buf[2] = 0;
    send(sockfd, buf, strlen(buf), 0);

    printf("send %s to server\r\n", buf);

    // read file size
    memset(buf, 0, BUF_SIZE);
    if((recv_bytes = recv(sockfd, buf, BUF_SIZE, 0)) == -1) {
        printf("recv ret %d\r\n", recv_bytes);
        goto out;
    }
    printf("1: recved %d bytes\r\n", recv_bytes);

    buf[recv_bytes] = 0;
    file_size = atoi(buf);

    // read file name
    memset(buf, 0, BUF_SIZE);
    if((recv_bytes = recv(sockfd, buf, BUF_SIZE, 0)) == -1) {
        printf("recv ret %d\r\n", recv_bytes);
        goto out;
    }
    printf("2: recved %d bytes\r\n", recv_bytes);
    buf[recv_bytes] = 0;

    printf("file name %s, size %d\r\n", buf, file_size);

    round = 0;
    while(1) {
        memset(buf, 0, BUF_SIZE);
        if((recv_bytes = recv(sockfd, buf, BUF_SIZE, 0)) == -1)
        {
            printf("recv() error.\n");
            goto out;
        }
        total_recv_bytes += recv_bytes;

        printf("%04d: recv_bytes %d, total_recv_bytes %d\r\n",
                round, recv_bytes, total_recv_bytes);

        if (total_recv_bytes >= file_size)
            break;

        round++;
    }

    printf("total send %d bytes, recved bytes %d\r\n",
            total_send_bytes, total_recv_bytes);

out:
    close(sockfd);
}

