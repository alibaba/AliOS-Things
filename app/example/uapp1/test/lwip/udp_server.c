#define EMBED

#ifdef EMBED
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
#else
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#endif

#define PORT 1234

#define BUF_SIZE 1024

static char buf[BUF_SIZE];

#ifdef  EMBED
void udp_server(void)
#else
int main(int argc, char **argv)
#endif
{
    int sockfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
    int recv_bytes;
    int send_bytes;
    int total_recv_bytes;
    int total_send_bytes;
    int file_size;
    int round;

    printf("start udp server\r\n");

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("Creating socket failed.\n");
        return -1;
    }



    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Bind() error.\n");
        return -1;
    }

    len = sizeof(client);
    recv_bytes = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &len);
    if(recv_bytes < 0) {
        goto out;
    } else {
        buf[recv_bytes] = 0;
        printf("receive file: %s from client %s:%d\r\n",
        buf, inet_ntoa(client.sin_addr),htons(client.sin_port));

        recv_bytes = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &len);
        if (recv_bytes < 0) {
            goto out;
        } else {
            buf[recv_bytes] = 0;
            file_size = atoi(buf);
            printf("file size: %d\r\n", file_size);
        }
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        printf("setsockopt failed:");
        goto out;
    }

    total_send_bytes = 0;
    total_recv_bytes = 0;
    round = 0;
    while(1)
    {
        recv_bytes = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &len);
        if(recv_bytes < 0)
        {
            printf("recvfrom() error, ret %d\r\n", recv_bytes);
            goto out;
        }
        total_recv_bytes += recv_bytes;

        send_bytes = sendto(sockfd, buf, recv_bytes, 0, (struct sockaddr *)&client, len);
        if (send_bytes < 0) {
            printf("sednto() error, ret %d\r\n", send_bytes);
            goto out;
        }
        printf("%04d: received len %d, send len %d, total received lend %d\r\n",
                round, recv_bytes, send_bytes, total_recv_bytes);
        total_send_bytes += send_bytes;

        if (file_size == total_recv_bytes){
            printf("file received end, received bytes %d, send bytes %d\r\n",
                   total_recv_bytes, total_send_bytes);
            break;
        }

        round++;
    }

out:
    close(sockfd);

    return 0;
}
