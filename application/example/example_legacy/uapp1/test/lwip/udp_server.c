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

static char buf[MAXDATASIZE];

void udp_server(void)
{
    int sockfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
    int num;
    int total_recv_size;
    int file_size;
    int fd;

    printf("start udp server\r\n");

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("Creating socket failed.\n");
        return;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Bind() error.\n");
        return;
    }

    len = sizeof(client);
    num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&client, &len);
    if(num < 0) {
        goto out;
    } else {
        buf[num] = 0;
        printf("receive file: %s from client %s:%d\r\n",
        buf, inet_ntoa(client.sin_addr),htons(client.sin_port));

        num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&client, &len);
        if (num < 0) {
            goto out;
        } else {
            buf[num] = 0;
            file_size = atoi(buf);
            printf("file size %d\r\n", file_size);
        }
    }

    total_recv_size = 0;
    while(1)
    {
        num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&client, &len);
        if(num < 0)
        {
            printf("recvfrom() error.\n");
            goto out;
        }
        buf[num] = '\0';
        total_recv_size += num;

        sendto(sockfd, buf, num, 0, (struct sockaddr *)&client, len);

        if (file_size == total_recv_size){
            printf("file received end, received size %d\r\n", total_recv_size);
            break;
        }
    }

out:
    close(sockfd);
}
