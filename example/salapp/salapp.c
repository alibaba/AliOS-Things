/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <sal_arch.h>
#include <sal_def.h>
#include <sal_ipaddr.h>
#include <sal_sockets.h>
#include <sal_err.h>
#include <sal.h>

#include <hal/atcmd.h>
#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#include <netmgr.h>

#define TAG "salapp"
#define SALAPP_BUFFER_MAX_SIZE  1512

static int g_fd;
static char read_temp[64]={0};

static void yloop_action(void *arg);

 void yloop_socket_event_cb(int fd, void *arg)
 {
     LOG("====yloop_socket_event_cb===");
     int rec=read(g_fd, read_temp, sizeof(read_temp)-1);
     LOG("===get socket event,fd=%d= read rec=%d,read=%s ",g_fd,rec,read_temp);
 }

static void yloop_action(void *arg)
{
    LOG("Hello, %s");
    aos_post_delayed_action(5000, yloop_action, NULL);
    aos_poll_read_fd(g_fd, yloop_socket_event_cb, NULL);
}


static void handle_sal(char *pwbuf, int blen, int argc, char **argv)
{
    char *ptype = argc > 1 ? argv[1] : "default";
    char buf[SALAPP_BUFFER_MAX_SIZE] = {0};
    int  readlen = 0;
    /* TCP client case */
    if (strcmp(ptype, "tcp_c") == 0) {
        char *pip, *pport, *pdata;
        ssize_t siz;
        int time = 0;
        struct sockaddr_in addr;
        
        g_fd = socket(AF_INET,SOCK_STREAM,0);

        pip = argv[2];
        pport = argv[3];
        pdata = argv[4];

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons((short)atoi(pport));
        addr.sin_addr.s_addr = inet_addr(pip);

        if (connect(g_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            LOGE(TAG, "Connect failed, errno = %d", errno);
            close(g_fd);
            return;
        }
        
        while(1){
            // send-recv
            if ((siz = send(g_fd, pdata, strlen(pdata), 0)) <= 0) {
                LOGE(TAG, "send failed, errno = %d.", errno);
                close(g_fd);
                return;
            }
            
            readlen = read(g_fd, buf, SALAPP_BUFFER_MAX_SIZE - 1);
            if (readlen < 0){
                LOGE(TAG, "recv failed, errno = %d.", errno);
                close(g_fd);
                return;
            }

            if (readlen == 0){
                LOGE(TAG, "recv buf len is %d \n", readlen);
                break;
            }
            
            LOGD(TAG, "recv server reply len %d info %s \n", readlen, buf);
            if (strstr(buf, pdata)){
                LOGI(TAG, "Goodbye! See you! (%d)\n", g_fd);
                time++;
            }
            
            if (time >= 100){
                break;
            }
        }
        
        close(g_fd);
        LOGI(TAG, "sal tcp_c test successful.");
    }else if (strcmp(ptype, "udp_c") == 0) {
        char *pip, *pport, *pdata;
        ssize_t siz;

        struct sockaddr_in addr;
        g_fd = socket(AF_INET,SOCK_DGRAM,0);

        pip = argv[2];
        pport = argv[3];
        pdata = argv[4];

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons((short)atoi(pport));
        addr.sin_addr.s_addr = inet_addr(pip);
#if 0
        if (connect(g_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            LOGE(TAG, "Connect failed, errno = %d", errno);
            close(g_fd);
            return;
        }

        // send-recv
        if ((siz = send(g_fd, pdata, strlen(pdata), 0)) <= 0) {
            LOGE(TAG, "send failed, errno = %d.", errno);
            close(g_fd);
            return;
        }
#else
        siz = sendto(g_fd, pdata, strlen(pdata), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
        if (siz <= 0){
            LOGE(TAG, "sendto failed, errno = %d.", errno);
            close(g_fd);
            return;
        }
#endif
        close(g_fd);
        LOGI(TAG, "sal udp_c test successful.");
    } else if (strcmp(ptype, "otaapi") == 0) {
        char domain[] = "www.baidu.com";
        int port = 8080;
        struct hostent *host;
        struct timeval timeout;

        struct sockaddr_in server_addr;

        if ((host = gethostbyname(domain)) == NULL) {
            LOGE(TAG, "gethostbyname failed, errno: %d", errno);
            return;
        }

        if ((g_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            LOGE(TAG, "Socket failed, errno: %d", errno);
            return;
        }

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        if (setsockopt (g_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                        sizeof(timeout)) < 0) {
            LOGE(TAG, "setsockopt failed, errno: %d", errno);
            close(g_fd);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);

        if (connect(g_fd, (struct sockaddr *) (&server_addr),
            sizeof(struct sockaddr)) == -1) {
            LOGE(TAG, "Connect failed, errno: %d", errno);
            close(g_fd);
            return;
        }
        close(g_fd);
        LOGI(TAG, "sal otaapi test successful.");
    } 
    else if (strcmp(ptype, "yloop") == 0) {
        char domain[] = "192.168.16.114";
        int port = 8099;
        struct hostent *host;
        struct timeval timeout;

        struct sockaddr_in server_addr;

        if ((host = gethostbyname(domain)) == NULL) {
            LOGE(TAG, "gethostbyname failed, errno: %d", errno);
            return;
        }

        if ((g_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            LOGE(TAG, "Socket failed, errno: %d", errno);
            return;
        }

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        if (setsockopt (g_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                        sizeof(timeout)) < 0) {
            LOGE(TAG, "setsockopt failed, errno: %d", errno);
            close(g_fd);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);

        if (connect(g_fd, (struct sockaddr *) (&server_addr),
            sizeof(struct sockaddr)) == -1) {
            LOGE(TAG, "Connect failed, errno: %d", errno);
            close(g_fd);
            return;
        }
        aos_schedule_call(yloop_action,NULL);

        LOGI(TAG, "sal otaapi test successful.");
    } 
    else if (strcmp(ptype, "mbedtlsapi") == 0) {
        struct addrinfo hints, *addr_list;
        int proto = 0; // 0 - tcp, 1 - udp
        char nodename[] = "www.baidu.com";
        char *port = "8080";
        int ret;

        memset( &hints, 0, sizeof( hints ) );
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = proto ? SOCK_DGRAM : SOCK_STREAM;
        hints.ai_protocol = proto ? IPPROTO_UDP : IPPROTO_TCP;

        // getaddrinfo only returns 1 addr
        if (getaddrinfo(nodename, port, &hints, &addr_list) != 0) {
            LOGE(TAG, "getaddrinfo faied, errno: %d", errno);
            return;
        }

        g_fd = socket(addr_list->ai_family, addr_list->ai_socktype,
                    addr_list->ai_protocol);
        if (g_fd < 0) {
            LOGE(TAG, "socket failed, errno: %d", errno);
            return;
        }

        do {
            ret = connect(g_fd, addr_list->ai_addr, addr_list->ai_addrlen);
            if (ret == -1) {
                close(g_fd);
                freeaddrinfo(addr_list);
            } else {
                if (errno == EINTR) {
                    continue;
                }
                break;
           }
        } while(1);

        close(g_fd);
        freeaddrinfo(addr_list);
    }else if (strcmp(ptype, "close") == 0) {
        close(g_fd);
    }
}

static struct cli_command salcmds[] = {
    {
        .name = "sal",
        .help = "sal tcp_c|udp_c|yloop|otaapi|mbedtlsapi [remote_ip remote_port data]",
        .function = handle_sal
    }
};

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) return;
    if (event->code == CODE_WIFI_ON_PRE_GOT_IP)
        LOG("Hello, WiFi PRE_GOT_IP event!");
    if (event->code == CODE_WIFI_ON_GOT_IP)
        LOG("Hello, WiFi GOT_IP event!");
}

int application_start(int argc, char *argv[])
{
    printf("Hello app started\r\n");

    aos_set_log_level(AOS_LL_DEBUG);

#ifdef AOS_ATCMD
    uart_dev_t uart_1;
    // AT UART init
    uart_1.port                = AT_UART_PORT;
    uart_1.config.baud_rate    = AT_UART_BAUDRATE;
    uart_1.config.data_width   = AT_UART_DATA_WIDTH;
    uart_1.config.parity       = AT_UART_PARITY;
    uart_1.config.stop_bits    = AT_UART_STOP_BITS;
    uart_1.config.flow_control = AT_UART_FLOW_CONTROL;

    if (at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, 
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000) != 0)
        return -1;

    at.set_mode(ASYN);
#endif

    sal_init();
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    aos_cli_register_commands((const struct cli_command *)&salcmds[0],
      sizeof(salcmds) / sizeof(salcmds[0]));

    netmgr_init();
    netmgr_start(false);

    aos_loop_run();

    return 0;
}
