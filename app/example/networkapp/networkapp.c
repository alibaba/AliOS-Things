/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include <aos/aos.h>
#include <aos/network.h>
#include <netmgr.h>
#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#define BUFFER_MAX_SIZE  1512
#define TCP_DEMO_TARGET_TCP_PORT 443

#ifndef IPADDR_NONE
#define IPADDR_NONE ((uint32_t)0xffffffffUL)
#endif

static int networktestcmd_tcp_client(int argc, char **argv)
{
    int  ret = 0;
    int  readlen = 0;
    int  fd = 0;
    int  time = 0;
    int  testtimes = 1;
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
    
    timeout.tv_sec = 30;
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
        if ((ret = send(fd, pcdata, strlen(pcdata), 0)) <= 0) {
            printf("send data failed, errno = %d. for the %d time \r\n", errno, time);
            goto err;
        }
        
        memset(pbuf, 0, BUFFER_MAX_SIZE);
        
        readlen = read(fd, pbuf, BUFFER_MAX_SIZE - 1);
        if (readlen < 0){
            printf("recv failed, errno = %d.\r\n", errno);
            goto err;
        }

        if (readlen == 0){
            printf("recv buf len is %d \r\n", readlen);
            break;
        }
        
        printf("recv server %d time reply len %d \r\n", time, readlen);
        
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

static int networktestcmd_udp_client(int argc, char **argv)
{
    int ret = 0;
    int fd  = 0;
    int testtimes = 1;
    char *pcipaddr = NULL;
    char *pcdestport = NULL;
    char *pcdata = NULL;
    char *pctesttime = NULL;
    struct sockaddr_in addr;

    if (argc < 5){
        printf("invalid input udp clinet test command \r\n");
        return -1;
    }
    
    pcipaddr = argv[2];
    pcdestport = argv[3];
    pcdata = argv[4];
    
    if (argc == 6){
        pctesttime = argv[5];
        testtimes = atoi(pctesttime);
        if (testtimes <= 0){
            printf("invalid input udp client test time %s \r\n", pctesttime);
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
    
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if (fd < 0){
        printf("fail to creat socket errno = %d \r\n", errno);
        return -1;
    }
    
    while(testtimes > 0) {
        ret = sendto(fd, pcdata, strlen(pcdata), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
        if (ret < 0){
            printf("udp sendto failed, errno = %d.", errno);
            close(fd);
            return -1;
        }
        testtimes--;
    }

    close(fd);
    return 0;

}

static int networktestcmd_domain(int argc, char **argv)
{
    char *pcdomain = NULL;
    char *pcdestport = NULL;
    int port = 0;
    int fd = 0;
    struct hostent *host = NULL;
    struct sockaddr_in server_addr;

    if (argc < 3){
        printf("network domain test invalid input \r\n");
        return -1;
    }

    pcdomain = argv[2];
    if (argc > 3){
        pcdestport = argv[3];
        port = atoi(pcdestport);
        if (port == 0){
            printf("invalid input domain dest port %s \r\n", pcdestport);
            return -1;
        }
    }
    
    if ((host = gethostbyname(pcdomain)) == NULL) {
        printf("gethostbyname failed, errno: %d domain %s \r\n", errno, pcdomain);
        return -1;
    }
    printf("get target IP is %d.%d.%d.%d\n", (unsigned char)((*(unsigned long *)(host->h_addr) & 0x000000ff) >> 0),
                                        (unsigned char)((*(unsigned long *)(host->h_addr) & 0x0000ff00) >> 8),
                                        (unsigned char)((*(unsigned long *)(host->h_addr) & 0x00ff0000) >> 16),
                                        (unsigned char)((*(unsigned long *)(host->h_addr) & 0xff000000) >> 24));
    if (port){
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            printf("Socket failed, errno: %d \r\n", errno);
            return -1;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        printf("connect to ip 0x%x port 0x%x \r\n", server_addr.sin_addr.s_addr, server_addr.sin_port);
        
        if (connect(fd, (struct sockaddr *) (&server_addr),
            sizeof(struct sockaddr)) == -1) {
            printf("Connect failed, errno: %d\r\n", errno);
            close(fd);
            return -1;
        }
        
        close(fd);
    }
    return 0;
}

static void handle_networktestcmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *ptype = NULL;
    int  ret = 0;

    if (argc < 2 || NULL == argv){
        printf("invalid input netword test command argc %d argv %p \r\n", argc, argv);
        return;
    }

    ptype = argv[1];

    /* TCP client case */
    if (strcmp(ptype, "tcp_c") == 0) {
        ret = networktestcmd_tcp_client(argc, argv);
        if (ret){
            printf("fail to execute tcp client test command \r\n");
            return;
        }
    } else if (strcmp(ptype, "udp_c") == 0) {
        ret = networktestcmd_udp_client(argc, argv);
        if (ret){
            printf("fail to execute udp client test command \r\n");
            return;
        } 
    } else if (strcmp(ptype, "domain") == 0){
        ret = networktestcmd_domain(argc, argv);
        if (ret){
            printf("fail to execute domain test commnad \r\n");
            return;
        }
    }else {
        printf("invalid netword test command input \r\n");
    }
    printf("network command test successed \r\n");
}


static struct cli_command networktestcmds[] = {
    {
        .name = "network",
        .help = "netowork { tcp_c|udp_c remote_ip remote_port data [times] } | { domain domain_info [ remote_port ]}",
        .function = handle_networktestcmd
    }
};

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) return;
    
    if (event->code == CODE_WIFI_ON_PRE_GOT_IP)
        LOG("Hello, WiFi PRE_GOT_IP event!");
    
    if (event->code == CODE_WIFI_ON_GOT_IP){
        aos_cli_register_commands((const struct cli_command *)&networktestcmds[0],
            sizeof(networktestcmds) / sizeof(networktestcmds[0]));
        LOG("Hello, WiFi GOT_IP event!");
    }
}

static void app_delayed_action(void *arg)
{
    LOG("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    //aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, 
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);
#endif

#ifdef WITH_SAL
    sal_init();
#endif
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);
    
    netmgr_init();
    netmgr_start(false);

    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}

