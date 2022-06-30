/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("nano entry here!\r\n");

    while (1) {
        printf("hello world! count %d \r\n", count++);
        aos_msleep(10000);
    };
}

static void tcp_echo_server(int argc, char **argv)
{
    if (argc < 2) {
        printf("invalid arguments\r\n");
        return;
    }

    unsigned short port = atoi(argv[1]);

    // Create a listening socket (also called server socket) 

    int servsock;
    if ((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket failed\r\n");
        return;
    }

    // Construct local address structure

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // any network interface
    servaddr.sin_port = htons(port);

    // Bind to the local address

    if (bind(servsock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("bind failed\r\n");
        lwip_close(servsock);
        return;
    }

    // Start listening for incoming connections

    if (listen(servsock, 5 /* queue size for connection requests */ ) < 0) {
        printf("listen failed\r\n");
        lwip_close(servsock);
        return;
    }

    int r;
    char buf[10];

    int clntsock;
    socklen_t clntlen;
    struct sockaddr_in clntaddr;

    while (1) {

        // Accept an incoming connection

        printf("waiting for client...\r\n");

        clntlen = sizeof(clntaddr); // initialize the in-out parameter

        if ((clntsock = accept(servsock, (struct sockaddr *) &clntaddr, &clntlen)) < 0) {
            printf("accept failed\r\n");
            lwip_close(servsock);
            return;
        }

        // accept() returned a connected socket (also called client socket)
        // and filled in the client's address into clntaddr

        printf("client ip: %s\r\n", inet_ntoa(clntaddr.sin_addr));

        // Receive msg from client, capitalize the 1st char, send it back

        while ((r = recv(clntsock, buf, sizeof(buf), 0)) > 0) {
            if (send(clntsock, buf, r, 0) != r) {
                printf("ERR: send failed\r\n");
                break;
            }
        }

        if (r < 0) {
            printf("ERR: recv failed\r\n");
        } else if (r == 0) {
            printf("closed by client\r\n");
        }

        // Client closed the connection (r==0) or there was an error
        // Either way, close the client socket and go back to accept()

        lwip_close(clntsock);
    }
}

ALIOS_CLI_CMD_REGISTER(tcp_echo_server, tcp_echo_server, tcp echo server)
