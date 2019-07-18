/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ota_socket.h"
#include <errno.h>
#include <network/network.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int ota_socket_connect(int port, char *host_addr)
{
    if (host_addr == NULL || strlen(host_addr) == 0 || port <= 0) {
        printf("ota_socket_connect parms   error\n ");
        return -1;
    }
    struct sockaddr_in server_addr;
    struct hostent *host;
    int sockfd;
    if ((host = gethostbyname(host_addr)) == NULL) {
        printf("Gethostname   error,   %s\n ", strerror(errno));
        return -1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Socket   Error:%s\a\n ", strerror(errno));
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec  = 10;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                   sizeof(timeout)) < 0) {
        printf("setsockopt failed\n");
        goto err_out;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    server_addr.sin_addr   = *((struct in_addr *)host->h_addr);

    if (connect(sockfd, (struct sockaddr *)(&server_addr),
                sizeof(struct sockaddr)) == -1) {
        printf("socket connecting %s failed!\n", strerror(errno));
        if (errno != EINTR) {
            goto err_out;
        }
        if (ota_socket_check_conn(sockfd) < 0) {
            goto err_out;
        }
    }
    return sockfd;

err_out:
    close(sockfd);
    return -1;
}

int ota_socket_send(int socket, const char *buf, size_t len)
{
    if (socket < 0) {
        printf("ota_socket_send: invalid socket fd\n");
        return -1;
    }
    if (buf == NULL) {
        printf("ota_socket_send: buf is NULL\n");
        return -1;
    }

    return write(socket, buf, len);
}

int ota_socket_recv(int socket, char *buf, size_t len)
{
    if (socket < 0) {
        printf("ota_socket_recv: invalid socket fd\n");
        return -1;
    }
    if (buf == NULL) {
        printf("ota_socket_recv: buf is NULL\n");
        return -1;
    }

    return read(socket, buf, len);
}

void ota_socket_close(int socket)
{
    close(socket);
}

int ota_socket_check_conn(int sock)
{
    if (sock < 0) {
        printf("ota_socket_check_conn: invalid socket fd\n");
        return -1;
    }
    /*
        struct pollfd fd = { .fd = sock, .events = POLLOUT };
        int ret = 0;
        socklen_t len = 0;

        while (poll(&fd, 1, -1) == -1) {
            if (errno != EINTR ) {
                return -1;
            }
        }

        len = sizeof(ret);
        if (getsockopt (sock, SOL_SOCKET, SO_ERROR, &ret, &len) == -1 ||
            ret != 0) {
            return -1;
        }
    */
    return 0;
}
