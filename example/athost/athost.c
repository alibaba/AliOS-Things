/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <aos/aos.h>
#include <atparser.h>
#include <aos/network.h>
#include <hal/wifi.h>

#include "athost.h"

#define TAG "athost"

#define MAX_ATCMD_PREFIX 32
#define LINK_ID_MAX 5

static hal_wifi_event_cb_t wifi_events;
static aos_sem_t start_sem;
static uint8_t ip_ready = 0;

#define MAX_RECV_BUF_SIZE 1500
#define HDLC_BUF_MAX_SIZE  256
#define HDLC_ECHO_PRFIX "HDLC_ECHO:"
#define HDLC_SERVER_ECHO_INFO "MSG_TOO_LONG."

static char buf[HDLC_BUF_MAX_SIZE];
static char out[HDLC_BUF_MAX_SIZE];
static char prefix[] = HDLC_ECHO_PRFIX;
static char info[] = HDLC_SERVER_ECHO_INFO;

static link_t g_link[LINK_ID_MAX];
static aos_mutex_t g_link_mutex;

static bool inited = false;
static bool uart_echo_on = false;

static const char *prefix_athost = "AT+";
static const char *prefix_cipevent = "AT+CIPEVENT:";
static const char *prefix_cipdomain = "AT+CIPDOMAIN:";
static const char *prefix_wevent = "AT+WEVENT:";
static const char *conntype_str[] = { "tcp_server",
                                      "tcp_client",
                                      "ssl_client",
                                      "udp_broadcast",
                                      "udp_unicast"
                                    };

static int notify_cip_connect_status_events(int sockid, int status, int recvstatus);
static int post_send_at_uart_task(const char *cmd);
static int post_send_socket_data_task(int sockid, const char *data, int datalen);
static int notify_atcmd_recv_status(int status);

static int send_at_uart(char *arg)
{
    LOG("at going to send %s!\n", arg);
    if (arg == NULL) {
        return -1;
    }

    at.send_raw(arg, NULL, 0);

    return 0;
}

static int socket_data_len_check(char data)
{
    if (data > '9' || data < '0') {
        return -1;
    }
    return 0;
}

static int socket_ip_info_check(char data)
{
    if ((data > '9' || data < '0') && data != '.') {
        return -1;
    }

    return 0;
}

static int socket_conntype_check(char data)
{
    if ((data > 'z' || data < 'a') && data != '_') {
        return -1;
    }

    return 0;
}

// ret: -1 error, 0 more field, 1 no more field
static int socket_data_info_get(char *buf, uint32_t buflen, at_data_check_cb_t valuecheck)
{
    uint32_t i = 0;

    if (NULL == buf || 0 == buflen) {
        return -1;
    }

    do {
        at.parse(&buf[i], 1);
        if (buf[i] == ',') {
            buf[i] = 0;
            break;
        } else if (buf[i] == '\r') { //TODO: replace with normal end
            LOGD(TAG, "********delimiter find here********\n");
            buf[i] = 0;
            return 1;
        }

        if (i >= buflen) {
            LOGE(TAG, "Too long length of data.reader is %s \r\n", buf);
            return -1;
        }
        if (NULL != valuecheck) {
            if (valuecheck(buf[i])) {
                LOGE(TAG, "Invalid string!!!, reader is %s \r\n", buf);
                return -1;
            }
        }
        i++;
    } while (1);

    return 0;
}

static int get_conntype_index(char *str)
{
    int i;

    if (NULL == str) {
        return -1;
    }

    for (i = 0; i < CONN_TYPE_NUM; i++) {
        if (memcmp(str, conntype_str[i], strlen(str)) == 0) {
            return i;
        }
    }

    return -1;
}

static int find_linkid_by_sockfd(int fd)
{
    int i;
    int linkid = -1;

    if (fd < 0) {
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (i = 0 ; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd == fd) {
            linkid = g_link[i].linkid;
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return linkid;
}

static int find_conntype_by_sockfd(int fd)
{
    int i;
    int type;

    if (fd < 0) {
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (i = 0 ; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd == fd) {
            type = g_link[i].type;
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return type;
}

static int find_sockfd_by_linkid(int linkid)
{
    int i;
    int fd = -1;

    if (linkid < 0) {
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (i = 0 ; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd >= 0 &&
            g_link[i].linkid == linkid) {
            fd = g_link[i].fd;
            break;
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return fd;
}

static int add_link_info(int fd, int linkid, CONN_TYPE type)
{
    int i;
    int ret = -1;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return ret;
    }

    for (i = 0; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd >= 0) {
            continue;
        } else {
            g_link[i].fd = fd;
            g_link[i].type = type;
            g_link[i].linkid = linkid;

            if (aos_sem_new(&g_link[i].sem_start, 0) != 0) {
                LOGE(TAG, "failed to allocate semaphore %s", __func__);
                g_link[i].fd = -1;
                g_link[i].linkid = -1;
                break;
            }

            if (aos_sem_new(&g_link[i].sem_close, 0) != 0) {
                LOGE(TAG, "failed to allocate semaphore %s", __func__);
                aos_sem_free(&g_link[i].sem_start);
                g_link[i].fd = -1;
                g_link[i].linkid = -1;
                break;
            }

            ret = 0;
            break;
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int delete_link_info_by_sockfd(int sockfd)
{
    int i;
    int ret = -1;

    if (sockfd < 0) {
        return ret;
    }

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return ret;
    }

    for (i = 0; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd == sockfd) {
            g_link[i].fd = -1;
            g_link[i].linkid = -1;

            if (aos_sem_is_valid(&g_link[i].sem_start)) {
                aos_sem_free(&g_link[i].sem_start);
            }

            if (aos_sem_is_valid(&g_link[i].sem_close)) {
                aos_sem_free(&g_link[i].sem_close);
            }

            ret = 0;
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

#define MAX_ATCMD_DATA_RECV_PREFIX_LEN 60
/*
 *  Network data recv event handler. Events includes:
 *   1. +CIPEVENT:SOCKET,id,len,data
 *   2. +CIPEVENT:UDP_BROADCAST,ip,port,id,len,data
 *
 *   data len should be within a reasonable range
 */
static int notify_cip_data_recv_event_unblock(int sockid, char *databuf, int datalen)
{
    char *type_str;
    char addr_str[16] = {0};  // ipv4 only
    int port;
    char port_str[6] = {0};
    char linkid_str[5] = {0};
    char datalen_str[6] = {0};
    char *sendbuf = NULL;
    int sendbuflen, offset = 0;
    int type, linkid;

    if (sockid < 0) {
        LOGE("Invalid sock id %d!\n", sockid);
        goto err;
    }

    // add one more for debug
    sendbuflen = MAX_ATCMD_DATA_RECV_PREFIX_LEN + datalen + 1 + 1;
    sendbuf = (char *) aos_malloc(sendbuflen);
    if (!sendbuf) {
        LOGE(TAG, "Error: %s %d out of memory, len is %d. \r\n",
             __func__, __LINE__, sendbuflen);
        goto err;
    }

    type = find_conntype_by_sockfd(sockid);
    if (type == UDP_BROADCAST) {
        type_str = "UDP_BROADCAST";
    } else {
        type_str = "SOCKET";
    }

    if (type == UDP_BROADCAST) {
        struct sockaddr_in peer;
        uint32_t peerlen = sizeof(struct sockaddr_in);
        char *remoteip;

        if (getpeername(sockid, (struct sockaddr *)&peer, &peerlen) != 0) {
            LOGE("Fail to sock %d get remote address!\n", sockid);
            goto err;
        }

        remoteip = inet_ntoa(peer.sin_addr);
        memcpy(addr_str, remoteip, strlen(remoteip));
        port = peer.sin_port;
    }

    if ((linkid = find_linkid_by_sockfd(sockid)) < 0) {
        LOGE(TAG, "Invalid link id %d!\n", linkid);
        goto err;
    }

    // prefix
    if (offset + strlen(prefix_cipevent) < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen, "%s", prefix_cipevent);
    } else {
        LOGE(TAG, "at string too long %s\n", sendbuf);
        goto err;
    }

    // type
    if (offset + strlen(type_str) + 1 < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen - offset, "%s,", type_str);
    } else {
        LOGE(TAG, "at string too long %s\n", sendbuf);
        goto err;
    }

    if (type == UDP_BROADCAST) {
        // ip
        if (offset + strlen(addr_str) + 1 < sendbuflen) {
            offset += snprintf(sendbuf + offset, sendbuflen - offset,
                               "%s,", addr_str);
        } else {
            LOGE(TAG, "at string too long %s\n", sendbuf);
            goto err;
        }

        // port
        itoa(port, port_str, 10);
        if (offset + strlen(port_str) + 1 < sendbuflen) {
            offset += snprintf(sendbuf + offset, sendbuflen - offset,
                               "%s,", port_str);
        } else {
            LOGE(TAG, "at string too long %s\n", sendbuf);
            goto err;
        }
    }

    itoa(linkid, linkid_str, 10);
    // append id
    if (offset + strlen(linkid_str) + 1 < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen - offset,
                           "%s,", linkid_str);
    } else {
        LOGE(TAG, "at string too long %s\n", sendbuf);
        goto err;
    }

    itoa(datalen, datalen_str, 10);
    // append datalen
    if (offset + strlen(datalen_str) + 1 < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen - offset,
                           "%s,", datalen_str);
    } else {
        LOGE(TAG, "at string too long %s\n", sendbuf);
        goto err;
    }

    // append data
    if (offset + datalen < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen - offset,
                           "%s", databuf);
    } else {
        LOGE(TAG, "at string too long %s\n", sendbuf);
        goto err;
    }

    if (post_send_at_uart_task(sendbuf) != 0) {
        LOGE(TAG, "fail to send at cmd %s\n", sendbuf);
        goto err;
    }

    aos_free(sendbuf);
    return 0;

err:
    aos_free(sendbuf);
    return -1;
}

static int notify_cip_data_recv_event(int sockid, char *databuf, int datalen)
{
    char *type_str;
    char addr_str[16] = {0};  // ipv4 only
    int port;
    char port_str[6] = {0};
    char linkid_str[5] = {0};
    char datalen_str[6] = {0};
    char sendbuf[MAX_ATCMD_DATA_RECV_PREFIX_LEN] = {0};
    int sendbuflen = MAX_ATCMD_DATA_RECV_PREFIX_LEN, offset = 0;
    int type, linkid;

    if (sockid < 0) {
        LOGE("Invalid sock id %d!\n", sockid);
        goto err;
    }

    type = find_conntype_by_sockfd(sockid);
    if (type == UDP_BROADCAST) {
        type_str = "UDP_BROADCAST";
    } else {
        type_str = "SOCKET";
    }

    if (type == UDP_BROADCAST) {
        struct sockaddr_in peer;
        uint32_t peerlen = sizeof(struct sockaddr_in);
        char *remoteip;

        if (getpeername(sockid, (struct sockaddr *)&peer, &peerlen) != 0) {
            LOGE("Fail to sock %d get remote address!\n", sockid);
            goto err;
        }

        remoteip = inet_ntoa(peer.sin_addr);
        memcpy(addr_str, remoteip, strlen(remoteip));
        port = peer.sin_port;
    }

    if ((linkid = find_linkid_by_sockfd(sockid)) < 0) {
        LOGE(TAG, "Invalid link id %d!\n", linkid);
        goto err;
    }

    // prefix
    if (offset + strlen(prefix_cipevent) < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen, "%s", prefix_cipevent);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__, sendbuf);
        goto err;
    }

    // type
    if (offset + strlen(type_str) + 1 < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen - offset, "%s,", type_str);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,  sendbuf);
        goto err;
    }

    if (type == UDP_BROADCAST) {
        // ip
        if (offset + strlen(addr_str) + 1 < sendbuflen) {
            offset += snprintf(sendbuf + offset, sendbuflen - offset,
                               "%s,", addr_str);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__, sendbuf);
            goto err;
        }

        // port
        itoa(port, port_str, 10);
        if (offset + strlen(port_str) + 1 < sendbuflen) {
            offset += snprintf(sendbuf + offset, sendbuflen - offset,
                               "%s,", port_str);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__, sendbuf);
            goto err;
        }
    }

    itoa(linkid, linkid_str, 10);
    // append id
    if (offset + strlen(linkid_str) + 1 < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen - offset,
                           "%s,", linkid_str);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__, sendbuf);
        goto err;
    }

    itoa(datalen, datalen_str, 10);
    // append datalen
    if (offset + strlen(datalen_str) + 1 < sendbuflen) {
        offset += snprintf(sendbuf + offset, sendbuflen - offset,
                           "%s,", datalen_str);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n",  __func__, __LINE__, sendbuf);
        goto err;
    }

    at.send_data_2stage((const char *)sendbuf, (const char *)databuf, datalen, NULL, 0);

    return 0;

err:
    return -1;
}


// TODO: add udp client
void tcp_client_recv_task(void *arg)
{
    char *buf = NULL;
    int len = 0;
    int fd =  *((int *)arg);
    fd_set readfds;

    aos_free(arg);

    buf = (char *) aos_malloc(MAX_RECV_BUF_SIZE);
    if (NULL == buf) {
        LOGE("fail to malloc memory %d at %s %d \r\n", MAX_RECV_BUF_SIZE, __FUNCTION__, __LINE__);
        goto exit;
    }

    LOG("New client task starts on socket %d\n", fd);

    while ( 1 ) {
        if (find_linkid_by_sockfd(fd) < 0) {
            LOGD("Client exit on socket %d\n", fd);
            goto exit;
        }

        FD_ZERO( &readfds );
        FD_SET( fd, &readfds );

        select(fd + 1, &readfds, NULL, NULL, NULL );

        if (FD_ISSET(fd, &readfds)) {
            if ((len = recv(fd, buf, MAX_RECV_BUF_SIZE, 0)) <= 0 ) {
                LOG("Client task exit!\n");
                goto exit;
            }

            buf[len] = '\0';
            LOG("tcp client %d recv len %d\n", fd, len);
            notify_cip_data_recv_event(fd, buf, len);
        }
    }

exit:
    LOG("check here0\n");
    aos_free(buf);
    LOG("check here1.1\n");

    // need to close by task
    if (find_linkid_by_sockfd(fd) >= 0) {
        LOG("check here1\n");
        notify_cip_connect_status_events(fd, CIP_STATUS_CLOSED, 0);
        LOG("check here2\n");
        delete_link_info_by_sockfd(fd);
    }

    close(fd);
    aos_task_exit(1);
}

#define MAX_ATCMD_RESPONSE_LEN 20
static int notify_atcmd_recv_status(int status)
{
    int offset = 0;
    char *status_str;
    char response[MAX_ATCMD_RESPONSE_LEN] = {0};

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_RESPONSE_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_RESPONSE_LEN - offset, "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (status == ATCMD_FAIL) {
        status_str = AT_RECV_FAIL_POSTFIX;
    } else if (status == ATCMD_SUCCESS) {
        status_str = AT_RECV_SUCCESS_POSTFIX;
    } else {
        LOGE(TAG, "unknown status\n", response);
        goto err;
    }

    // status
    if (offset + strlen(status_str) < MAX_ATCMD_RESPONSE_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_RESPONSE_LEN - offset, "%s", status_str);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    at.send_raw(response, NULL, 0);

    return 0;
err:
    return -1;
}

/**
 * Network connection state event handler. Events includes:
 *   1. +CIPEVENT:id,SERVER,CONNECTED
 *   2. +CIPEVENT:id,SERVER,CLOSED
 *   3. +CIPEVENT:CLIENT,CONNECTED,ip,port
 *   4. +CIPEVENT:CLIENT,CLOSED,ip,port
 *   5. +CIPEVENT:id,UDP,CONNECTED
 *   6. +CIPEVENT:id,UDP,CLOSED
 */
#define MAX_ATCMD_CON_STATUS_LEN 80
static int notify_cip_connect_status_events(int sockid, int status, int recvstatus)
{
    char *status_str;
    char *type_str;
    char addr_str[16] = {0};  // ipv4 only
    int port;
    char port_str[6] = {0};
    char cmd[MAX_ATCMD_CON_STATUS_LEN] = {0};
    int offset = 0;
    int type, linkid;

    LOG("notify starts for fd %d\n", sockid);

    if (sockid < 0) {
        LOGE("Invalid sock id %d!\n", sockid);
        goto err;
    }

    if (status == CIP_STATUS_CONNECTED) {
        status_str = "CONNECTED";
    } else if (status == CIP_STATUS_CLOSED) {
        status_str = "CLOSED";
    } else {
        LOGE("Invalid connect status %d!\n", status);
        goto err;
    }

    type = find_conntype_by_sockfd(sockid);
    if (type == TCP_CLIENT || type == SSL_CLIENT) {
        type_str = "SERVER";
    } else if (type == TCP_SERVER) {
        struct sockaddr_in peer;
        uint32_t peerlen = sizeof(struct sockaddr_in);
        char *remoteip;
        type_str = "CLIENT";

        if (getpeername(sockid, (struct sockaddr *)&peer, &peerlen) != 0) {
            LOGE("Fail to sock %d get remote address!\n", sockid);
            goto err;
        }

        remoteip = inet_ntoa(peer.sin_addr);
        //TODO: check len
        memcpy(addr_str, remoteip, strlen(remoteip));
        port = peer.sin_port;
    } else if (type == UDP_BROADCAST || type == UDP_UNICAST) {
        type_str = "UDP";

    } else {
        LOGE(TAG, "Invalid connect type %d!\n", type);
        goto err;
    }

    if ((linkid = find_linkid_by_sockfd(sockid)) < 0) {
        LOGE(TAG, "Invalid link id %d!\n", linkid);
        goto err;
    }

    if (recvstatus > 0) {
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_CON_STATUS_LEN) {
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset, "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }
    }

    // prefix
    if (offset + strlen(prefix_cipevent) < MAX_ATCMD_CON_STATUS_LEN) {
        offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset, "%s", prefix_cipevent);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    if (type == TCP_SERVER) {
        // do nothing
    } else {
        char linkid_str[5] = {0};

        itoa(linkid, linkid_str, 10);
        // append id
        if (offset + strlen(linkid_str) + 1 < MAX_ATCMD_CON_STATUS_LEN) {
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                               "%s,", linkid_str);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }
    }

    // type
    if (offset + strlen(type_str) + 1 < MAX_ATCMD_CON_STATUS_LEN) {
        offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                           "%s,", type_str);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    // status
    if (offset + strlen(status_str) < MAX_ATCMD_CON_STATUS_LEN) {
        offset +=  snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                            "%s", status_str);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    if (type == TCP_SERVER) {
        // ip
        if (offset + strlen(addr_str) + 1 < MAX_ATCMD_CON_STATUS_LEN) {
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                               ",%s", addr_str);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }

        // port
        itoa(port, port_str, 10);
        if (offset + strlen(port_str) + 1 < MAX_ATCMD_CON_STATUS_LEN) {
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                               ",%s", port_str);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }
    }

    if (recvstatus > 0) {
        if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_CON_STATUS_LEN) {
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset, "%s", AT_RECV_SUCCESS_POSTFIX);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }
    }

    //Todo: delete after debug
    //offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
    //                  ";");

    //if (send_at_uart(cmd) != 0) {
    if (post_send_at_uart_task(cmd) != 0) {
        LOGE(TAG, "fail to send at cmd %s\n", cmd);
        goto err;
    }

    return 0;

err:
    return -1;
}

void send_at_uart_task(void *arg)
{
    if (!arg) {
        goto exit;
    }

    LOG("at going to send %s!\n", (char *)arg);

    at.send_raw((char *) arg, NULL, 0);
exit:
    aos_free(arg);
    aos_task_exit(0);
}

static int post_send_at_uart_task(const char *cmd)
{
    int size = strlen(cmd) + 1;
    char *tskarg = NULL;

    tskarg = (char *) aos_malloc(size);
    if (tskarg == NULL) {
        LOGE(TAG, "Fail to allcate memory %d byte for uart send task arg\r\n", size);
        goto exit;
    }
    memcpy(tskarg, cmd, size);

    if (aos_task_new("uart_send_task", send_at_uart_task,
                     (void *) tskarg, 2048) != 0) {
        LOGE(TAG, "Fail to create uart send task\r\n");
        goto exit;
    }

    return 0;

exit:
    aos_free(tskarg);
    return -1;
}

struct socket_data_arg {
    int sockid;
    char *data;
    int datalen;
};

void send_socket_data_task(void *arg)
{
    struct socket_data_arg *sendarg;

    if (!arg) {
        goto exit;
    }

    sendarg = (struct socket_data_arg *) arg;

    if (sendarg->sockid < 0   ||
        sendarg->data == NULL ||
        sendarg->datalen <= 0) {
        LOGE(TAG, "invalid socket %d data len %d\n", sendarg->sockid, sendarg->datalen);
        goto exit;
    }

    LOG("socket %d going to send data len %d!\n", sendarg->sockid, sendarg->datalen);

    if (send(sendarg->sockid, sendarg->data, sendarg->datalen, 0) <= 0) {
        LOGE(TAG, "send data failed, errno = %d. \r\n", errno);
    }

exit:
    aos_free(sendarg->data);
    aos_free(arg);
    aos_task_exit(0);
}

static int post_send_socket_data_task(int sockid, const char *data, int datalen)
{
    int size = sizeof(struct socket_data_arg);
    struct socket_data_arg *arg = NULL;
    char *buf = NULL;

    if (sockid < 0 || data == NULL || datalen <= 0) {
        LOGE(TAG, "invalid socket %d data len %d\n", sockid, datalen);
        goto exit;
    }

    arg = (struct socket_data_arg *) aos_malloc(size);
    if (arg == NULL) {
        LOGE(TAG, "Fail to allcate memory %d byte for socket send task arg\r\n", size);
        goto exit;
    }

    size = datalen;
    buf = (char *) aos_malloc(size);
    if (buf == NULL) {
        LOGE(TAG, "Fail to allcate memory %d byte for socket send task buf\r\n", size);
        goto exit;
    }
    memcpy(buf, data, datalen);

    arg->sockid = sockid;
    arg->data = buf;
    arg->datalen = datalen;

    if (aos_task_new("socket_send_task", send_socket_data_task,
                     (void *) arg, 2048) != 0) {
        LOGE(TAG, "Fail to create socket send task\r\n");
        goto exit;
    }

    return 0;

exit:
    aos_free(buf);
    aos_free(arg);
    return -1;
}

// AT+CIPSTART=linkid,conntype,address,remoteport
int atcmd_cip_start()
{
    char single;
    char body[16];
    int ret;
    int linkid;
    int type;
    char remoteip[16] = {0};
    uint16_t remoteport;
    struct sockaddr_in addr;
    int fd = -1;
    int socktype;
    recv_task_t recvtsk;
    char tskname[16] = {0};
    int *tskarg = NULL;
    int stacksize;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    at.parse(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = socket_data_info_get(body, sizeof(body), &socket_data_len_check);
    if (ret < 0 || (linkid = atoi(body)) < 0) {
        LOGE(TAG, "Invalid link id %s !!!\r\n", body);
        goto err;
    }

    // check linkid exist
    if (find_sockfd_by_linkid(linkid) >= 0) {
        LOGE(TAG, "link id %d exist !!!\r\n", linkid);
        goto err;
    }

    // connect type
    memset(body, 0, sizeof(body));
    ret = socket_data_info_get(body, sizeof(body), &socket_conntype_check);
    if (ret < 0 || (type = get_conntype_index(body)) < 0) {
        LOGE(TAG, "Invalid connect type %s !!!\r\n", body);
        goto err;
    }

    // remote ip
    ret = socket_data_info_get(remoteip, sizeof(remoteip), &socket_ip_info_check);
    if (ret < 0) {
        LOGE(TAG, "Invalid ip addr %s !!!\r\n", remoteip);
        goto err;
    }

    // port
    memset(body, 0, sizeof(body));
    ret = socket_data_info_get(body, sizeof(body), &socket_data_len_check);
    if (ret < 0) {
        LOGE(TAG, "Invalid portno %s !!!\r\n", body);
        goto err;
    }
    LOG("port %s\n", body);
    remoteport = atoi(body);

    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons(remoteport);
    if (0 == addr.sin_port) {
        LOGE(TAG, "invalid input port info %u \r\n", remoteport);
        goto err;
    }

    addr.sin_addr.s_addr = inet_addr(remoteip);
    if (IPADDR_NONE == addr.sin_addr.s_addr) {
        LOGE(TAG, "invalid input addr info %s \r\n", remoteip);
        goto err;
    }

    addr.sin_family = AF_INET;

    if (type == TCP_CLIENT ||
        type == SSL_CLIENT ||
        type == TCP_SERVER) {
        socktype = SOCK_STREAM;
    } else if (type == UDP_BROADCAST ||
               type == UDP_UNICAST) {
        socktype = SOCK_DGRAM;
    } else {
        LOGE(TAG, "invalid conntype %d \r\n", type);
        goto err;
    }

    fd = socket(AF_INET, socktype, 0);

    if (fd < 0) {
        LOGE(TAG, "fail to creat socket errno = %d \r\n", errno);
        goto err;
    }

    if (type == TCP_CLIENT) {
        char *prefix = "tcp_client";

        tskarg = (int *) aos_malloc(sizeof(int));
        if (tskarg == NULL) {
            LOGE(TAG, "Fail to allcate memory  %d byte for task arg\r\n", (sizeof(int)));
            goto err;
        }
        *tskarg = fd;

        LOG("addr %u port %u \n", addr.sin_addr.s_addr, remoteport);
        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            LOGE(TAG, "Connect failed, errno = %d, ip %s port %s \r\n", errno, remoteip, remoteport);
            goto err;
        }
        LOG("TCP client connect success!\n");

        recvtsk = tcp_client_recv_task;
        sprintf(tskname, "%s_%d", prefix, linkid);
        stacksize = 2048; // TODO need set by configuration
    } else if (type == TCP_SERVER) {
        LOG("TCP server not implement yet!\n");
        goto err;
    }

    // save global info for new socket
    if (add_link_info(fd, linkid, type) != 0) {
        LOGE(TAG, "Fail to add link info for sock %d linkid %d type %d\r\n", fd, linkid, type);
        goto err;
    }

    if (aos_task_new(tskname, recvtsk, (void *) tskarg, stacksize) != 0) {
        LOGE(TAG, "Fail to create task %s\r\n", tskname);
        delete_link_info_by_sockfd(fd);
        goto err;
    }

    // notify over uart
    if (notify_cip_connect_status_events(fd, CIP_STATUS_CONNECTED, ATCMD_SUCCESS) != 0) {
        LOGE(TAG, "Fail to create task\r\n");
        delete_link_info_by_sockfd(fd);
        goto err;
    }

    return 0;

err:
    // notify fail response
    notify_atcmd_recv_status(ATCMD_FAIL);

    if (fd >= 0) {
        close(fd);
    }

    return -1;
}

// AT+CIPSEND=linkid,<remote_port>,datalen
int at_cip_send()
{
    char single;
    char body[16];
    char *recvdata = NULL, *tmp;
    int linkid, sockid;
    int remoteport, datalen;
    int ret, offset;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    at.parse(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip send prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = socket_data_info_get(body, sizeof(body), &socket_data_len_check);
    if (ret < 0 || (linkid = atoi(body)) < 0) {
        LOGE(TAG, "Invalid link id %s !!!\r\n", body);
        goto err;
    }

    // check linkid exist
    if ((sockid = find_sockfd_by_linkid(linkid)) < 0) {
        LOGE(TAG, "link id %d does not exist !!!\r\n", linkid);
        goto err;
    }

    LOG("try to get remote port\n");
    // try get remote port
    memset(body, 0, sizeof(body));
    ret = socket_data_info_get(body, sizeof(body), &socket_data_len_check);
    if (ret < 0 || (remoteport = atoi(body)) < 0) {
        LOGE(TAG, "Invalid port %s !!!\r\n", body);
        goto err;
    }

    LOG("after try get remote port %d ret %d\n", remoteport, ret);
    if (ret == 0) {
        memset(body, 0, sizeof(body));
        ret = socket_data_info_get(body, sizeof(body), &socket_data_len_check);
        if (ret < 0 || (datalen = atoi(body)) < 0) {
            LOGE(TAG, "Invalid link id %s !!!\r\n", body);
            goto err;
        }
    } else if (ret == 1) {
        datalen = remoteport;
    }

    LOGD(TAG, "%s socket data on link %d with length %d to remote\n",
         __func__, linkid, datalen);

    // Prepare socket data
    recvdata = (char *)aos_malloc(datalen + 1);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory, len is %d. \r\n",
             __func__, __LINE__, datalen + 1);
        goto err;
    }

    // eat delimiter '\r'
    /*at.parse(&single, 1);

    offset = 0;
    if (single != '\r') {
        LOG("------->there may be problem<-------");
        recvdata[0] = single;
        offset++;
    }*/
    offset = 0;

    at.parse(recvdata + offset, datalen - offset);

    recvdata[datalen] = '\0';
    LOG("single %c -->%s<--\n", single, recvdata);

    // TODO: what to do with remote port recvdata
    if (post_send_socket_data_task(sockid, recvdata, datalen) != 0) {
        LOG(TAG, "Error post send socket fail \r\n");
        goto err;
    }

    notify_atcmd_recv_status(ATCMD_SUCCESS);
    aos_free(recvdata);
    return 0;

err:
    notify_atcmd_recv_status(ATCMD_FAIL);
    aos_free(recvdata);
    return -1;
}

// AT+CIPSTOP=linkid
int atcmd_cip_stop()
{
    char single;
    char body[5];
    int ret;
    int linkid, sockfd;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    at.parse(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = socket_data_info_get(body, sizeof(body), &socket_data_len_check);
    if (ret < 0 || (linkid = atoi(body)) < 0) {
        LOGE(TAG, "Invalid link id %s !!!\r\n", body);
        goto err;
    }

    // check linkid exist
    if ((sockfd = find_sockfd_by_linkid(linkid)) < 0) {
        LOGE(TAG, "link id %d does not exist !!!\r\n", linkid);
        goto err;
    }

    notify_cip_connect_status_events(sockfd, CIP_STATUS_CLOSED, ATCMD_SUCCESS);
    delete_link_info_by_sockfd(sockfd);
    return 0;

err:
    return -1;
}

// TODO: need flash read / write
// AT+CIPAUTOCONN=linkid,0/1
int atcmd_cip_auto_connect()
{
    char single;
    char body[5];
    int ret;
    int linkid, sockfd;
    int auto_set;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    at.parse(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = socket_data_info_get(body, sizeof(body), &socket_data_len_check);
    if (ret < 0 || (linkid = atoi(body)) < 0) {
        LOGE(TAG, "Invalid link id %s !!!\r\n", body);
        goto err;
    }

    // set bit
    at.parse(&single, 1);
    if (single != '0' && single != '1') {
        LOGE(TAG, "Invalid auto connect set %c !!!\r\n", single);
        goto err;
    }
    auto_set = single - '0';

    // TODO: check linkid info exist from flash
    if ((sockfd = find_sockfd_by_linkid(linkid)) < 0) {
        LOGE(TAG, "link id %d does not exist !!!\r\n", linkid);
        goto err;
    }

    // close connection
    if (0 == auto_set) {
        notify_cip_connect_status_events(sockfd, CIP_STATUS_CLOSED, ATCMD_SUCCESS);
        delete_link_info_by_sockfd(sockfd);
    }

    // TODO: change the setting on the flash.

    return 0;

err:
    return -1;
}

#define MAX_ATCMD_DOMAIN_LEN 80
// AT+CIPDOMAIN=domain
// Respone: AT+CIPDOMAIN:180.97.33.108
int atcmd_cip_domain_dns()
{
    char single;
    char domain[50];
    char addr_str[16];  // ipv4 only
    char response[MAX_ATCMD_DOMAIN_LEN] = {0};
    int ret;
    struct hostent *host;
    struct in_addr **addrlist;
    int i, offset = 0;
    char *index;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    at.parse(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // domain
    memset(domain, 0, sizeof(domain));
    ret = socket_data_info_get(domain, sizeof(domain), NULL);
    if (ret < 0) {
        LOGE(TAG, "Invalid domain %s !!!\r\n", domain);
        goto err;
    }

    if ((host = gethostbyname(domain)) == NULL) {
        LOGE(TAG, "fail to find domain %s !!!\r\n", domain);
        goto err;
    }

    addrlist = (struct in_addr **) host->h_addr_list;
    for (i = 0; addrlist[i] != NULL; i++) {
        // return the first one
        strcpy(addr_str, inet_ntoa(*addrlist[i]));
        break;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_DOMAIN_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (offset + strlen(prefix_cipdomain) < MAX_ATCMD_DOMAIN_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_LEN - offset,
                           "%s", prefix_cipdomain);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // default 0
    index = "0";
    if (offset + strlen(index) < MAX_ATCMD_DOMAIN_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_LEN - offset,
                           "%s", index);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_DOMAIN_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (offset + strlen(addr_str) < MAX_ATCMD_DOMAIN_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_LEN - offset,
                           "%s", addr_str);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_DOMAIN_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_DOMAIN_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_LEN - offset,
                           "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (post_send_at_uart_task(response) != 0) {
        LOGE(TAG, "%s %d post send at uart task fail!\n", __func__, __LINE__);
        goto err;
    }

    return 0;
err:
    // todo post fail response
    return -1;
}

/*
 * Wifi station event handler. include:
 *  +WEVENT:AP_UP
 *  +WEVENT:AP_DOWN
 *  +WEVENT:STATION_UP
 *  +WEVENT:STATION_DOWN
 */
#define MAX_ATCMD_AP_STA_STATUS_LEN 30
int notify_AP_STA_status_events(int type, int status)
{
    char *status_str;
    char *type_str;
    char cmd[MAX_ATCMD_AP_STA_STATUS_LEN] = {0};
    int offset = 0;

    if (type == AP) {
        type_str = "AP_";
    } else if (type == STA) {
        type_str = "STATION_";
    } else {
        LOGE("Invalid type %d!\n", type);
        goto err;
    }

    if (status == WEVENT_STATUS_UP) {
        status_str = "UP";
    } else if (status == WEVENT_STATUS_DOWN) {
        status_str = "DOWN";
    } else {
        LOGE("Invalid connect status %d!\n", status);
        goto err;
    }

    if (offset + strlen(prefix_wevent) < MAX_ATCMD_AP_STA_STATUS_LEN) {
        offset += snprintf(cmd + offset, MAX_ATCMD_AP_STA_STATUS_LEN - offset,
                           "%s", prefix_wevent);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    if (offset + strlen(type_str) + strlen(status_str) <
        MAX_ATCMD_AP_STA_STATUS_LEN) {
        offset += snprintf(cmd + offset, MAX_ATCMD_AP_STA_STATUS_LEN - offset,
                           "%s%s", type_str, status_str);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    at.send_raw(cmd, NULL, 0);

    return 0;

err:
    return -1;
}

// AT+UARTE=OFF
int atcmd_uart_echo()
{
    int ret;
    char single;
    char body[10] = {0};
    bool echo;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    at.parse(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // echo setting
    ret = socket_data_info_get(body, sizeof(body), NULL);
    if (ret < 0) {
        LOGE(TAG, "Invalid command %s !!!\r\n", body);
        goto err;
    }

    if (memcmp(body, "ON", strlen(body)) == 0) {
        uart_echo_on = true;
    } else if (memcmp(body, "OFF", strlen(body)) == 0) {
        uart_echo_on = false;
    } else {
        LOGE(TAG, "Invalid command %s !!!\r\n", body);
        goto err;
    }

    return 0;

err:
    return -1;
}

enum {
    ATCMD_UART_ECHO = 0,
    ATCMD_CIP_DOMAIN,
    ATCMD_CIP_AUTOCONN,
    ATCMD_CIP_START,
    ATCMD_CIP_STOP,
    ATCMD_CIP_SEND,
};

static const struct at_cli_command at_cmds_table[] = {
    // uart setting
    {.name = "AT+UARTE", .help = "AT+UARTE=<ON/OFF>", .function = atcmd_uart_echo},

    // TCP/UDP:
    {.name = "AT+CIPDOMAIN", .help = "AT+CIPDOMAIN", .function = atcmd_cip_domain_dns},
    {.name = "AT+CIPAUTOCONN", .help = "AT+CIPAUTOCONN=<id>[,option]", .function = atcmd_cip_auto_connect},
    {.name = "AT+CIPSTART", .help = "AT+CIPSTART", .function = atcmd_cip_start},
    {.name = "AT+CIPSTOP", .help = "AT+CIPSTOP", .function = atcmd_cip_stop},
    {.name = "AT+CIPSEND", .help = "AT+CIPSEND=<id>,[<remote_port>,]<data_length>", .function = at_cip_send},
};


static int athost_init()
{
    int i;

    if (inited) {
        LOGW(TAG, "at host already initialized");
        return 0;
    }

    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    memset(g_link, 0, sizeof(g_link));
    for (i = 0; i < LINK_ID_MAX; i++) {
        g_link[i].fd = -1;
        g_link[i].linkid = -1;
    }

    inited = true;
    return 0;
}

static void ip_got_event(hal_wifi_module_t *m,
                         hal_wifi_ip_stat_t *pnet,
                         void *arg)
{
    LOG("%s - ip: %s, gw: %s, mask: %s", __func__, pnet->ip, pnet->gate, pnet->mask);
    if (aos_sem_is_valid(&start_sem)) {
        aos_sem_signal(&start_sem);
    }
    ip_ready = 1;
    LOG("WiFi start test succeed.");
}

static int register_wifi_events()
{
    hal_wifi_module_t *m;

    m = hal_wifi_get_default_module();
    if (!m) {
        LOGE(TAG, "failed: no default wifi module.");
        return -1;
    }

    /* m->ev_cb is declared as const, can only be assigned once. */
    if (m->ev_cb == NULL) {
        m->ev_cb = &wifi_events;
    }

    return 0;
}

static void start_wifi(char *ssid, char *pw)
{
    int ret = -1;
    hal_wifi_init_type_t type;

    LOG("WiFi start test started.");

    if (!ssid || !pw) {
        LOGE(TAG, "%s: invalid argument.", __func__);
        LOGE(TAG, "Starting wifi failed.");
        return;
    }

    if (register_wifi_events() != 0) {
        LOGE(TAG, "%s failed to register wifi events.", __func__);
        return;
    }

    wifi_events.ip_got = ip_got_event;

    ret = aos_sem_new(&start_sem, 0);
    if (0 != ret) {
        LOGE(TAG, "%s failed to allocate sem.", __func__);
        return;
    }

    memset(&type, 0, sizeof(type));
    type.wifi_mode = STATION;
    type.dhcp_mode = DHCP_CLIENT;
    strncpy(type.wifi_ssid, ssid, sizeof(type.wifi_ssid) - 1);
    strncpy(type.wifi_key, pw, sizeof(type.wifi_key) - 1);
    ret = hal_wifi_start(NULL, &type);
    if (0 == ret) {
        LOGE(TAG, "Started wifi (ssid: %s, password: %s').", ssid, pw);
    }

    LOG("WiFi start test completed.");

    aos_sem_wait(&start_sem, 60000);
    aos_sem_free(&start_sem);
}

static int uart_echo()
{
    int i = 0;

    do {
        if (!uart_echo_on) {
            return 0;
        }

        if (i >= sizeof(buf)) {
            LOGE(TAG, "Too long length\r\n");
            break;
        }

        if (at.parse(&buf[i], 1) <= 0) {
            LOGE(TAG, "read fail\r\n");
            break;
        }

        // end of message then echo
        if (buf[i] == ';') {
            buf[i] = '\0';

            if (memcmp(buf, "UARTE=OFF", strlen(buf)) == 0) {
                uart_echo_on = false;
                break;
            }

            LOG("Echo server recv msg len %d -->%s<--\n", i, buf);

            memcpy(out, buf, i);
            memcpy(buf, prefix, strlen(prefix));
            if (i + strlen(prefix) + 1 < sizeof(buf)) {
                memcpy(buf + strlen(prefix), out, i);
                buf[strlen(prefix) + i] = ';'; // TODO
                buf[strlen(prefix) + i + 1] = '\0';
            } else {
                memcpy(buf + strlen(prefix), info, strlen(info));
                buf[strlen(prefix) + strlen(info)] = '\0';
            }

            at.send_raw(buf, NULL, 0);
            break;
        }
        i++;
    } while (1);

    return 1;
}

static struct at_cli_command *get_atcmd_cip_handler()
{
    const char *cmd_prefix = "IP";
    char prefix[MAX_ATCMD_PREFIX] = {0};
    char single;
    int index = -1;

    // Eat "IP"
    at.parse(prefix, strlen(cmd_prefix));
    if (memcmp(prefix, cmd_prefix, strlen(cmd_prefix)) != 0) {
        LOGE(TAG, "invalid cip prefix %s\n", prefix);
        return NULL;
    }

    at.parse(&single, 1);

    switch (single) {
        case 'S':
            at.parse(prefix, 3);

            if (memcmp(prefix, "TAR", 3) == 0) {
                // Eat 'T'
                at.parse(&single, 1);
                index = ATCMD_CIP_START;
            } else if (memcmp(prefix, "TOP", 3) == 0) {
                index = ATCMD_CIP_STOP;
            } else if (memcmp(prefix, "END", 3) == 0) {
                index = ATCMD_CIP_SEND;
            } else {
                LOGE(TAG, "invalid cip prefix %s\n", prefix);
            }
            break;

        case 'D':
            // Eat "OMAIN"
            at.parse(prefix, 5);
            index = ATCMD_CIP_DOMAIN;
            break;

        case 'A':
            // Eat "UTOCONN"
            at.parse(prefix, 7);
            index = ATCMD_CIP_AUTOCONN;
            break;

        default:
            LOGE(TAG, "invalid cip prefix %c\n", single);
            break;
    }

    if (index >= 0) {
        return &at_cmds_table[index];
    }

    return NULL;
}

static struct at_cli_command *get_atcmd_uart_handler()
{
    const char *cmd_prefix = "ART";
    char prefix[MAX_ATCMD_PREFIX] = {0};
    char single;
    int index = -1;

    // Eat "ART"
    at.parse(prefix, strlen(cmd_prefix));
    if (memcmp(prefix, cmd_prefix, strlen(cmd_prefix)) != 0) {
        LOGE(TAG, "invalid uart prefix %s\n", prefix);
        return;
    }

    at.parse(&single, 1);

    switch (single) {
        case 'E':
            index = ATCMD_UART_ECHO;
            break;

        default:
            LOGE(TAG, "invalid uart prefix %c\n", single);
            break;
    }

    if (index >= 0) {
        return &at_cmds_table[index];
    }

    return NULL;
}

static void atcmd_handler()
{
    char single;
    struct at_cli_command *handler = NULL;
    LOGD(TAG, "%s entry.", __func__);

    if (uart_echo() != 0) {
        return;
    }

    at.parse(&single, 1);

    switch (single) {
        case 'C':
            handler = get_atcmd_cip_handler();
            break;

        case 'U':
            handler = get_atcmd_uart_handler();
            break;

        //Add other cmd handles here

        default:
            LOGE(TAG, "Unknown at command AT+%c\n", single);
            return;
    }

    if (handler != NULL) {
        handler->function();
    }

    LOGD(TAG, "%s exit.", __func__);
}

static void app_delayed_action(void *arg)
{
    LOG("AT host server: alive %s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(50000, app_delayed_action, NULL);
}

char *test_ssid =  "ChinaNet-qbTr";//"Yuemewifi-3766"; //"ChinaNet-qbTr"
char *test_pwd =  "cplahjrp";//"aos12345"; //"cplahjrp"
char *test_string = "AT+CIPSTART=0,tcp_client,192.168.1.5,20001";

int application_start(int argc, char *argv[])
{
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 10000);

    athost_init();

    at.oob(prefix_athost,  NULL, 0, atcmd_handler, NULL);

    LOG("AT host server start!\n");
    aos_post_delayed_action(1000, app_delayed_action, NULL);

    start_wifi(test_ssid, test_pwd);

    aos_loop_run();

    return 0;
}
