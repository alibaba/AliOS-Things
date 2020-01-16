/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <network/network.h>
#include <lwip/apps/sendfile.h>

#define DATALEN 1024
#define MAXSIZE 32
#define PATHMAX 64
static int sendfile_server_task_started = 0;

ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count)
{
    char data[DATALEN];
    int writelen;
    int readlen;
    int curlen = 0;

    while(1) {
        memset(data, 0, sizeof(data));
        readlen = read(in_fd, data, sizeof(data));

        if(0 == readlen) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d read finished", __func__, __LINE__));
            break;
        }

        if(((readlen < 0) && ((EAGAIN != errno) && (EINTR != errno) && (EINPROGRESS != errno)))) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d readlen %d errno: %d out_fd %d in_fd %d", __func__, __LINE__, readlen, errno, out_fd, in_fd));
            return -1;
        }

        if((NULL != offset) && (curlen < *offset) && (curlen + readlen > *offset)) {
            curlen += readlen;

            writelen = write(out_fd, data + *offset - curlen, curlen - *offset);
            if (((writelen < 0)&&(EAGAIN != errno) && (EINTR != errno) && (EINPROGRESS != errno))
                   || (writelen != readlen)) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("sendfile failed out_fd %d in_fd %d", out_fd, in_fd));
                return -1;
            }
        }
        else if((NULL == offset) || (curlen + readlen > *offset)) {
            curlen += readlen;
            writelen = write(out_fd, data, readlen);
            if (((writelen < 0)&&(EAGAIN != errno) && (EINTR != errno) && (EINPROGRESS != errno))
                   || (writelen != readlen)) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("sendfile write failed out_fd %d errno", out_fd, errno));
                return -1;
            }
        }
        memset(data, 0, sizeof(data));
        aos_msleep(100);
    }
    return curlen;
}

int sendfile_client(int argc,char *argv[])
{
    int fd, sockfd;
    char buf[MAXSIZE];
    char mode_buf[MAXSIZE];
    char size_buf[MAXSIZE];
    int mode, size;
    struct stat stat_buf;
    struct sockaddr_in srvaddr;
    char * dst_addr = NULL;
    char * dst_port = NULL;
    char * src_file = NULL;
    char * file_name = NULL;
    int i;
    int ret = -1;

    for(i = 1; i< argc; i++) {
        if(0 == strcmp(argv[i], "-d")) {
            i++;
            if(i >= argc) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d Invalid command", __func__, __LINE__));
                goto exit;
            }
            dst_addr = argv[i];
        }
        else if(0 == strcmp(argv[i], "-p")) {
            i++;
            if(i >= argc) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d Invalid command", __func__, __LINE__));
                goto exit;
            }
            dst_port = argv[i];
        }
        else if(0 == strcmp(argv[i], "-f")) {
            i++;
            if(i >= argc) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d Invalid command", __func__, __LINE__));
                goto exit;
            }
            src_file = argv[i];
        }
        else if(0 == strcmp(argv[i], "-n")) {
            i++;
            if(i >= argc) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d Invalid command", __func__, __LINE__));
                goto exit;
            }
            file_name = argv[i];
        }
    }

    if((NULL == dst_addr) || (NULL == dst_port) || (NULL == src_file) || (NULL == file_name)) {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d Invalid command", __func__, __LINE__));
        goto exit;
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&srvaddr,sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    inet_pton(AF_INET, dst_addr, (struct sockaddr *)&(srvaddr.sin_addr));
    srvaddr.sin_port = htons(atoi(dst_port));
    connect(sockfd, (struct sockaddr *)&srvaddr,sizeof(srvaddr));

    /* write file name to server */
    write(sockfd, src_file, strlen(src_file));

    /* get the file mode/size from server */
    memset(buf, 0, sizeof(buf));
    ret = recv(sockfd, buf, sizeof(buf), 0);
    if (ret == -1) {
        close(sockfd);
        LWIP_DEBUGF( SENDFILE_DEBUG, ("recv failed: %s", strerror(errno)));
        goto exit;
    }

    sscanf(buf, "mode=%s size=%s\n", mode_buf, size_buf);

    mode = atoi(mode_buf);
    size = atoi(size_buf);
    
    fd = open(file_name, O_WRONLY|O_CREAT, mode);
    if(fd <= 0) {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("open file %s failed: %s", file_name, strerror(errno)));
        close(sockfd);
        close(fd);
        goto exit;
    }

    while(1) {
        ret = sendfile(fd, sockfd, 0, size);
        if((ret < 0) && ((EAGAIN != errno) && (EINTR != errno) && (EINPROGRESS != errno))) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("sendfile ret %d, %s", ret, strerror(errno)));
            break;
        }
        if(ret == 0) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("sendfile finished"));
            break;
        }
    }

    close(fd);
    close(sockfd);
exit:
    return ret;
}

int sendfile_server(int argc, char **argv)
{
    int i;
    char* port = NULL;          /* port number to use */

    for(i = 1; i< argc; i++) {
        if(0 == strcmp(argv[i], "-p")) {
            i++;
            if(i >= argc) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("%s:%d Invalid command", __func__, __LINE__));
                return -1;
            }
            port = argv[i];
            if (atoi(port) <= 0) {
                LWIP_DEBUGF( SENDFILE_DEBUG, ("invalid port: %s", port));
                return -1;
            }
	} 
    }

    if (port == NULL) {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("No port input"));
        return -1;
    }
    sendfile_server_task_create(port);
    return 0;
}

void sendfile_server_thread(void *args)
{
    int sockfd;                	/* socket desciptor */
    int connfd;                	/* file descriptor for socket */
    int addrlen;				/* argument to accept */
    int ret;                    /* holds return code of system calls */
    int fd;                    	/* file descriptor for file to send */
    int port = 0;               /* port number to use */

    char filename[PATHMAX]={0}; /* filename to send */
    char buf[MAXSIZE];
    off_t offset = 0;          	/* file offset */
    struct stat stat_buf;      	/* argument to fstat */
	
    struct sockaddr_in addrserver;   	/* socket parameters for bind ipv4*/
    struct sockaddr_in addrclient;  	/* socket parameters for accept ipv4*/

    port = atoi((char *)args);

    /* create Internet domain socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("unable to create socket: %s", strerror(errno)));
        goto exit;
    }

    /* fill in socket structure */
    bzero(&addrserver, sizeof(addrserver));
    addrserver.sin_family = AF_INET;
    addrserver.sin_addr.s_addr = INADDR_ANY;
    addrserver.sin_port = htons(port);

    /* bind socket to the port */
    ret =  bind(sockfd, (struct sockaddr *)&addrserver, sizeof(addrserver));
    if (ret == -1) {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("unable to bind to socket: %s", strerror(errno)));
        goto exit;
    }

    /* listen for clients on the socket */
    ret = listen(sockfd, 1);
    if (ret == -1) {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("listen failed: %s", strerror(errno)));
        goto exit;
    }

    for ( ;; ) {
        addrlen = sizeof(addrclient);
        /* wait for a client to connect */
        connfd = accept(sockfd, (struct sockaddr *)&addrclient, &addrlen);
        if (connfd == -1) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("accept failed: %s", strerror(errno)));
            goto exit;
        }

        /* get the file name from the client */
        ret = recv(connfd, filename, sizeof(filename), 0);
        if (ret == -1) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("recv failed: %s", strerror(errno)));
	    close(connfd);
            goto exit;
        }

        /* null terminate and strip any \r and \n from filename */
        filename[ret] = '\0';
        if ( (filename[strlen(filename)-1] == '\n') || \
            (filename[strlen(filename)-1] == '\r'))
            filename[strlen(filename)-1] = '\0';

        /* exit server if filename is "quit" */
        if (strcmp(filename, "quit") == 0) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("quit command received, shutting down server"));
            break;
        }

        LWIP_DEBUGF( SENDFILE_DEBUG, ("received request to send file %s", filename));

        /* open the file to be sent */
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
	    close(connfd);
            close(fd);
            LWIP_DEBUGF( SENDFILE_DEBUG, ("unable to open '%s': %s", filename, strerror(errno)));
            goto exit;
        }

        /* get the size of the file to be sent */
        fstat(fd, &stat_buf);
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "mode=%d size=%d\n", stat_buf.st_mode, stat_buf.st_size);
        write(connfd, buf, strlen(buf));

        /* copy file using sendfile */
        offset = 0;
        ret = sendfile(connfd, fd, &offset, stat_buf.st_size);
        if (ret < 0) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("error from sendfile: %s", strerror(errno)));
	    close(connfd);
            close(fd);
            goto exit;
        }

        if(ret == 0) {
            close(connfd);
            close(fd);
            goto exit;
        }

        if (ret != stat_buf.st_size) {
            LWIP_DEBUGF( SENDFILE_DEBUG, ("incomplete transfer from sendfile: %d of %d bytes",
		  ret, (int)stat_buf.st_size));
	    close(connfd);
            close(fd);
            goto exit;
        }
    }//for(;;)

exit:
    /* close socket */
    close(sockfd);
    sendfile_server_task_started = 0;
    aos_task_exit(0);
}

void sendfile_server_task_create(char *port)
{
    if(1 != sendfile_server_task_started) {
        aos_task_new("sendfile_server_task", sendfile_server_thread, port, 20 * 1024);
        sendfile_server_task_started = 1;
    }
    else {
        LWIP_DEBUGF( SENDFILE_DEBUG, ("http upload task is already running, create a new task should do previous exit first"));
    }
}

