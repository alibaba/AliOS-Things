/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"
#include <network/network.h>
#include <netmgr.h>

#define TAG "sal_demo"
#define BUF_MAX  1512
static char skt_pbuf[BUF_MAX]; /* buffer used for socket data receiving */

/* TCP payload for testing, this is actually a HTTP request packet */
static char *tcp_payload ="GET / HTTP/1.1 \r\n\
Host: coap.me\r\n\
Connection: keep-alive\r\n\
Upgrade-Insecure-Requests: 1\r\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: zh-CN,zh;q=0.9\r\n\
\r\n";

/**
 * This is TCP client demo, which demenstrates:
 *   (1) create a TCP socket;
 *   (2) get IP of server domain;
 *   (3) connect TCP socket;
 *   (4) send data via TCP socket;
 *   (5) recv data from the TCP socket;
 *   (6) close a socket.
 * , by using SAL-provided socket API.
 */
static void sal_tcp_demo()
{
    const char *server_domain = "coap.me";
    const short server_port = 80;
    struct addrinfo  hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur          = NULL;
    char servname[6] = {0};
    struct sockaddr_in addr;
    int fd, rc, totallen, readlen;
    char *pbuf, *rsp = "HTTP/1.1", *rsp_ok = "HTTP/1.1 200 OK";

    LOGD(TAG, "SAL TCP demo start");

    LOGD(TAG, "The TCP server to connect: %s:%d", server_domain, server_port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET; // only IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    snprintf(servname, sizeof(servname), "%u", server_port);

    if ((rc = getaddrinfo(server_domain, servname, &hints, &addrInfoList)) != 0) {
        LOGE(TAG, "getaddrinfo error: %d, errno = %d", rc, errno);
        return;
    }

    /**
     * getaddrinfo() returns a list of address structures.
     * Try each address until we successfully connect.
     * If socket fails, we (close the socket
     * and) try the next address.
     */
    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {
        if (cur->ai_family != AF_INET) {
            LOGE(TAG, "Socket type error");
            continue;
        }

        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            LOGE(TAG, "Failed to create socket, errno = %d", errno);
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            break;
        } else {
            LOGE(TAG, "Failed to connect addr, errno = %d", errno);
        }

        close(fd);
    }

    if (!cur) {
        LOGE(TAG, "Failed to connect to server");
        goto ret;
    } else {
        LOGD(TAG, "TCP socket connect to server OK");
    }

    if (send(fd, tcp_payload, strlen(tcp_payload), 0) <= 0) {
        LOGE(TAG, "Failed to send data, errno = %d", errno);
        goto ret;
    } else {
        LOGD(TAG, "TCP socket send to server OK");
    }

    totallen = 0;
    do {
        memset(skt_pbuf, 0, BUF_MAX);
        readlen = read(fd, skt_pbuf, BUF_MAX - 1);
        if (readlen < 0 && errno != EINTR && errno != EWOULDBLOCK 
               && errno != EALREADY && errno != EINPROGRESS) {
            LOGE(TAG, "Failed to recv, errno = %d", errno);
            goto ret;
        }

        if (readlen == 0) {
            LOGD(TAG, "socket is closed");
            break;
        }

        totallen += readlen;

        if (strncmp(skt_pbuf, rsp, strlen(rsp)) == 0) {
            if (strncmp(skt_pbuf, rsp_ok, strlen(rsp_ok)) == 0) {
                LOGD(TAG, "recv http 200 OK");
                break;
            } else {
                LOGE(TAG, "recv http erorr");
                goto ret;
            }
        }
    } while (1);

ret:
    if (fd >= 0) close(fd);
    if (addrInfoList) freeaddrinfo(addrInfoList);

    LOGD(TAG, "SAL TCP demo end");
}

/* UDP payload for testing, binary data. */
static char *udp_payload = "\x40\x01\x73\x6a\x37\x63\x6f\x61\x70\x2e\x6d\x65";

/**
 * This is UDP client demo, which demenstrates:
 *   (1) create an UDP socket;
 *   (2) send data to UDP server via UDP socket;
 *   (3) close a socket.
 * , by using SAL-provided socket API.
 */
static void sal_udp_demo()
{
    int ret, fd;
    const char *server_ipaddr = "134.102.218.18";
    const short server_port = 5683;
    struct sockaddr_in addr;
    socklen_t recvaddrlen;
    char recvbuf[200] = {0};

    LOGD(TAG, "SAL UDP demo start");

    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = inet_addr(server_ipaddr);
    addr.sin_family = AF_INET;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0){
        LOGE(TAG, "Failed to create socket, errno = %d", errno);
        return;
    } else {
        LOGD(TAG, "UDP socket create OK");
    }

    ret = sendto(fd, udp_payload, strlen(udp_payload), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0){
        LOGE(TAG, "udp sendto failed, errno = %d", errno);
        close(fd);
        return;
    } else {
        LOGD(TAG, "UDP socket sendto OK");
    }

    ret = recvfrom(fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&addr, &recvaddrlen);
    if (ret < 0) {
        LOGE(TAG, "udp sendto failed, errno = %d", errno);
        close(fd);
        return;
    } else {
        LOGD(TAG, "%d bytes data received.", ret);
    }

    aos_msleep(3000);
    close(fd);

    LOGD(TAG, "SAL UDP demo end");
}

static void sal_demo()
{
    LOGD(TAG, "SAL demo start");
    sal_tcp_demo();
    sal_udp_demo();
    LOGD(TAG, "SAL demo end");
}

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) return;

    if (event->code == CODE_WIFI_ON_PRE_GOT_IP)
        LOG("Hello, WiFi PRE_GOT_IP event!");

    if (event->code == CODE_WIFI_ON_GOT_IP){
        LOG("Hello, WiFi GOT_IP event!");
        /* Only start to connect socket after network is ready. */
        sal_demo();
    }
}

/**
 * Application entry.
 */
int application_start(int argc, char *argv[])
{
    /**
     * Register and Initialize SAL device.
     *
     * Attention:
     *     (1) If register SAL device using "sal_add_dev(NULL, NULL)",
     *         it means to use the device and related configuration
     *         prodived in board settings (e.g. module ?= wifi.bk7231 
     *         in board/developer/aos.mk).
     *     (2) If want to register SAL device with specific parameters,
     *         Please use 'sal_add_dev("<device_name>", "<dev_para>")',
     *         in which the <device_name> is the module name of the AT device,
     *         and fill in the <dev_para> with UART config parameters.
     */
    sal_add_dev(NULL, NULL);

    /* Initialize the SAL core. */
    sal_init();

    /**
     * Register WiFi event handler, SAL device will post the GOT_IP
     * event once the network is connected, only after the event socket
     * code can be executed.
     */
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    /**
     * Initialize and start netmgr core, and register netmgr commands,
     * which users may need to trigger network connectin if using
     * WiFi modules.
     */
    netmgr_init();
    netmgr_start(false);

    /* Enter into aos yloop scheduler, will not return. */
    aos_loop_run();

    return 0;
}
