/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/kernel.h"
#include <network/network.h>
#include <netmgr.h>
#include "aos/cli.h"


#define BUFFER_MAX_SIZE  1512
#define TCP_DEMO_TARGET_TCP_PORT 443

#ifndef IPADDR_NONE
#define IPADDR_NONE ((uint32_t)0xffffffffUL)
#endif
int networktestcmd_tcp_client(int argc, char **argv)
{
    int  ret = 0;
    int  readlen = 0;
    int  fd = 0;
    int  time = 0;
    int  testtimes = 10;
    char *pbuf = NULL;
    char *pcipaddr = NULL;
    char *pcdestport = NULL;
    char *pcdata = NULL;
    char *pctesttime = NULL;
    struct sockaddr_in addr;
    struct timeval timeout;
    
    if (argc < 5){
        printf("invalid input tcp clinet test command \r\n");
        return -1;
    }

    pcipaddr = argv[2];
    pcdestport = argv[3];
    pcdata = argv[4];
    
    if (argc == 6){
        pctesttime = argv[5];
        testtimes = atoi(pctesttime);
        if (0 == testtimes){
            printf("invalid input tcp client test time %s \r\n", pctesttime);
            return -1;
        }
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons((short)atoi(pcdestport));

    if (0 == addr.sin_port){
        printf("invalid input port info %s \r\n", pcdestport);
        return -1;
    }
    
    addr.sin_addr.s_addr = inet_addr(pcipaddr);

    if (IPADDR_NONE == addr.sin_addr.s_addr){
        printf("invalid input addr info %s \r\n", pcipaddr);
        return -1;
    }

    addr.sin_family = AF_INET;

    fd = socket(AF_INET,SOCK_STREAM,0);
    if (fd < 0){
        printf("fail to creat socket errno = %d \r\n", errno);
        return -1;
    }
    printf("client fd=%d, ip=%s, port=%d\n", fd, pcipaddr, addr.sin_port);
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;

    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0) {
        printf("setsockopt failed, errno: %d\r\n", errno);
        goto err;
    }
    
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        printf("Connect failed, errno = %d, ip %s port %s \r\n", errno, pcipaddr, pcdestport);
        goto err;
    }
    
    pbuf = aos_malloc(BUFFER_MAX_SIZE);
    if (NULL == pbuf){
        printf("fail to malloc memory %d at %s %d \r\n", BUFFER_MAX_SIZE, __FUNCTION__, __LINE__);
        goto err;
    }
    
    while(1){
        // send-recv
        printf("send data to server: \"%s\"\r\n", pcdata);
        if ((ret = send(fd, pcdata, strlen(pcdata), 0)) <= 0) {
            printf("send data failed, errno = %d. for the %d time \r\n", errno, time);
            goto err;
        }
        
        memset(pbuf, 0, BUFFER_MAX_SIZE);
        
        printf("read data from server...\r\n");

        readlen = read(fd, pbuf, BUFFER_MAX_SIZE - 1);
        if (readlen < 0){
            printf("recv failed, errno = %d.\r\n", errno);
            goto err;
        }

        if (readlen == 0){
            printf("recv buf len is %d \r\n", readlen);
            break;
        }
        
        printf("recv server %d time reply len %d. str: %s\r\n", time, readlen, pbuf);
        
        time++;
        
        if (time >= testtimes){
            break;
        }
    }
    close(fd);
    aos_free(pbuf);
    return 0;
err:
    close(fd);
    if (NULL != pbuf){
        aos_free(pbuf);
    }
    return -1;
}