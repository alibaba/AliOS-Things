/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulog_api.h"
#include "ulog/ulog.h"


#if ULOG_POP_UDP_ENABLE

#include <string.h>
#include <stdlib.h>
#include "sys/socket.h"
#include "aos/kernel.h"

static struct sockaddr_in* syslog_watcher_addr = NULL;
static int32_t syslog_watcher_fd = -1;
static uint16_t syslog_watcher_port = SYSLOG_WATCHER_DEFAULT_PORT;
static uint32_t syslog_watcher_ip_nl = 0x0100007F;
static bool     tcpip_service_on = false;

/* We have to use sync debug to debug async debug .... */
/* #define SESSION_UDP_INFO printf */
#define SESSION_UDP_INFO(...)

/**
* @brief update address for log pop out via udp
*       prototype is align to the ulog service mapping(refer ulog_man_handler_service)
* @param ip_d ip address of syslog listener, if input 0 then former ip used, MUST BE network byte order
* @param port port of syslog listener, is 0 input then before port used. Default is syslog standard 514 used
*
* @return n.a.
*
*/
void on_update_syslog_watcher_addr(const uint32_t ip_nl, const uint32_t port)
{
    if (syslog_watcher_addr == NULL) {
        syslog_watcher_addr = (struct sockaddr_in*)aos_malloc(sizeof(struct sockaddr_in));
    }
    if (syslog_watcher_addr != NULL) {
        memset(syslog_watcher_addr, 0, sizeof(struct sockaddr_in));
        syslog_watcher_addr->sin_family = AF_INET;

        if (0xFFFF != (port & 0xFFFF)) {
            syslog_watcher_port = port;
            SESSION_UDP_INFO("ulog port %d\n", syslog_watcher_port);
        }
        syslog_watcher_addr->sin_port = syslog_watcher_port;

        if (-1 != ip_nl && 0 != ip_nl) {
            syslog_watcher_ip_nl = ip_nl;
            SESSION_UDP_INFO("ulog ip %s\n", inet_ntoa(*(struct in_addr *)(&syslog_watcher_ip_nl)));
        }
        syslog_watcher_addr->sin_addr.s_addr = syslog_watcher_ip_nl;
        SESSION_UDP_INFO("ulog udp ip %s ort %d \r\n", inet_ntoa(*(struct in_addr *)(&syslog_watcher_addr->sin_addr.s_addr)),
                                syslog_watcher_addr->sin_port);
    }
}

/**
* @brief hook function which used notice this module that the tcpip is ready for use
*        not thread-safe, but only be used in one task(ulog), so not necessary considering mutex.
*        prototype is align to the ulog service mapping(refer ulog_man_handler_service)
* @param on
* @param off
*
* @return n.a.
*
*/
void on_tcpip_service_on(const uint32_t on, const uint32_t off)
{
    if (on == -1 && off == 1) {
        /*means turn off syslog pop out*/
        tcpip_service_on = false;
        if (syslog_watcher_fd != -1) {
            close(syslog_watcher_fd);
            syslog_watcher_fd = -1;
        }
    } else if (on == 1 && off == -1) {
        /*means turn on syslog pop out*/
        if (NULL == syslog_watcher_addr) {
            SESSION_UDP_INFO("fail to turn on syslog pop out for syslog server ip info haven't set yet\n");
            return;
        }

        if (syslog_watcher_fd != -1) {
            SESSION_UDP_INFO("syslog pop out have already started socket\n");
        } else {
            syslog_watcher_fd = socket(AF_INET, SOCK_DGRAM, 0);
            SESSION_UDP_INFO("ulog sys log socket %d\n", syslog_watcher_fd);
        }

        tcpip_service_on = true;
    } else {
        SESSION_UDP_INFO("ulog sys log start invalid param on %d, off %d\n", on, off);
    }
}

/**
* @brief not thread-safe, but only be used in one task(ulog), so not necessary considering mutex
* @param data
* @param len
*
* @return -1 indicates not send out sucessfully
*
*/
int32_t pop_out_on_udp(const char* data, const uint16_t len)
{
    int32_t ret = -1;
    if (tcpip_service_on && syslog_watcher_fd >= 0) {
        ret = sendto(syslog_watcher_fd, data, len, 0, (struct sockaddr *)syslog_watcher_addr, sizeof(struct sockaddr));
    }
    return ret;
}

void update_net_cli(const char cmd, const char* param)
{
    struct in_addr  listen_addr;
    uint16_t listen_port = 0x0;
    char buf[24] = {0};
    if (param != NULL) {
        switch (cmd) {
        case 'a': {
            inet_aton(param, &listen_addr);
            snprintf(buf, 24, "listen ip=%u", listen_addr.s_addr);
            SESSION_UDP_INFO("%s %d ulog set syslog server ip addr %s \r\n", __FILE__, __LINE__, buf);
            ulog_man(buf);
        }
        break;

        case 'p': {
            listen_port = strtoul(param, NULL, 10);
            snprintf(buf, 24, "listen port=%d", listen_port);
            SESSION_UDP_INFO("%s %d ulog set syslog server ip port %s \r\n", __FILE__, __LINE__, buf);
            ulog_man(buf);
        }
        break;

        case 'n': {
            snprintf(buf, 24, "tcpip %s", param);
            SESSION_UDP_INFO("%s %d ulog set network swtich %s \r\n", __FILE__, __LINE__, buf);
            ulog_man(buf);
        }
        break;
        default:
            break;
        }
    }
}

#endif

