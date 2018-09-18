/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include<stdio.h> 
#include<sys/socket.h> 
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include <string.h>

#define MAX_LISTEN 1024
#define MAX_LINE 1024

int Socket(int domain, int type, int protocol){
    int sockfd = socket(domain, type, protocol);
    if ( sockfd < 0 ){
        perror("init socket:  ");
        exit(0);
    }
    return sockfd;
}

void Bind(int sockfd, struct sockaddr *myaddr, int addrlen){
    if ( bind(sockfd, myaddr, addrlen) < 0 ){
        perror("bind");
        exit(0);
    } }

void Listen(int sockfd, int backlog){
    if ( listen(sockfd, backlog) < 0){
        perror("listen");
        exit(0);
    }
}

int Accept(int listenfd, struct sockaddr *addr, int *addrlen){
    int clientfd = accept(listenfd, addr, addrlen);
    if ( clientfd < 0){
        perror("accept");
        exit(0);
    }
    return clientfd;
}

void Close(int clientfd){
    if ( close(clientfd) < 0){
        perror("close");
        exit(0);
    }
}
struct hostent* Gethostbyaddr(const char *addr, int len, int domain){
    struct hostent* host = gethostbyaddr(addr, len, domain);
    if ( NULL == host ){
        perror("host_by_addr");
        exit(0);
    }
    return host;
}

ssize_t Read(int fd, void* buf, size_t n){
    ssize_t num= read(fd, buf, n);
    if ( n < 0){
        perror("read");
        exit(0);
    }
    return num;
}

ssize_t Write(int fd, const void* buf, size_t n){
    ssize_t num = read(fd, (void *)buf, n);
    if ( n < 0){
        perror("write");
        exit(0);
    }
    return num;
}

void echo(int listenfd){
    ssize_t n;
    char write_buff[MAX_LINE];
    char read_buff[MAX_LINE];
    fd_set readfds;
    

    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( listenfd, &readfds );

        select( listenfd+1, &readfds, NULL, NULL, NULL);

        if ( FD_ISSET( listenfd, &readfds ) ) /*one client has data*/
        {
            memset(write_buff, 0, MAX_LINE);
            memset(read_buff, 0, MAX_LINE);
            n = 0;
            n = read(listenfd, read_buff, MAX_LINE);
            if ( n == 0 ){
                fprintf(stdout, "TCP Client is disconnected, fd: %d\n", listenfd);
                break;
            }
            read_buff[n] = '\0';
            fprintf(stdout, "Receive string from client: %s\r\n", read_buff);

            strcpy(write_buff, "from server echo: ");
            n = strlen("from server echo: ");
            strcpy(write_buff+n, read_buff);

            n += strlen(read_buff);
            write_buff[n] = '\0';

            fprintf(stdout, "Begin to send echo string to client: %s\r\n", write_buff);
            n = write(listenfd, write_buff, strlen(write_buff));
        }
    }
    Close(listenfd);
    return;
}

int main(int argc, char **argv){
    int servfd, clientfd, port, clientlen;
    fd_set readfds;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    struct hostent *host;
    char* hostaddr;
    if ( argc != 2){
        fprintf(stderr,"usage:%s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);  // get port

    servfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    // init servaddr
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons((unsigned short)port);
    clientlen = sizeof(cliaddr);
    
    Bind(servfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    Listen(servfd, MAX_LISTEN);

    while ( 1 ){
        FD_ZERO( &readfds );
        FD_SET( servfd, &readfds );

        select( servfd + 1, &readfds, NULL, NULL, NULL);

        if ( FD_ISSET( servfd, &readfds ) ){
            memset(&cliaddr, 0, sizeof(cliaddr));
            clientfd = Accept( servfd, (struct sockaddr *) &cliaddr, &clientlen );
            if ( clientfd >= 0 ){
                echo(clientfd);
            }
        }
    }
}
