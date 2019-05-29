/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <network/network.h>

#include "at_util.h"

#define TAG "attcpudp"

#ifndef IPADDR_NONE
#define IPADDR_NONE ((uint32_t)0xffffffffUL)
#endif

#define MAX_RECV_BUF_SIZE 1500

typedef void (*recv_task_t)(void *arg);

static const char *prefix_cipevent = "+CIPEVENT:";

bool inited = false;

enum
{
    CIP_STATUS_CLOSED    = 0,
    CIP_STATUS_CONNECTED = 1,
};

typedef enum
{
    /* WiFi */
    TCP_SERVER = 0,
    TCP_CLIENT,
    SSL_CLIENT,
    UDP_BROADCAST,
    UDP_UNICAST,

    CONN_TYPE_NUM
    /*WiFi end */
    /* Add others hereafter */
} CONN_TYPE;

typedef struct link_s
{
    int       fd;
    int       linkid;
    CONN_TYPE type;

    // used by broadcast udp
    struct sockaddr_in remoteaddr;
} link_t;

typedef struct
{
    int      sockfd;
    uint8_t *dataptr;
    uint16_t datalen;
} sock_send_info_t;

typedef struct
{
    uint32_t total_byte;
    uint32_t fetch_error;
    uint32_t put_error;
    uint32_t send_error;
} sock_send_stat_t;

#define DEFAULT_SOCK_SEND_BUF_SIZE 50
static aos_queue_t      sock_send_queue;
static sock_send_stat_t sock_send_statistic;

#define LINK_ID_MAX 5
static link_t      g_link[LINK_ID_MAX];
static aos_mutex_t g_link_mutex;

static int find_remoteaddr_by_sockfd(int fd, struct sockaddr_in *remoteaddr);
static int find_conntype_by_sockfd(int fd);
static int notify_cip_connect_status_events(int sockid, int status,
                                            int recvstatus);

#define AT_SOCKET_ASYNC_SEND_MODE 0
#if AT_SOCKET_ASYNC_SEND_MODE
static int sock_send_queue_init()
{
    uint32_t size = sizeof(sock_send_info_t) * DEFAULT_SOCK_SEND_BUF_SIZE;
    sock_send_info_t *sock_send_buf = NULL;

    memset(&sock_send_statistic, 0, sizeof(sock_send_statistic));

    sock_send_buf = (sock_send_info_t *)aos_malloc(size);
    if (!sock_send_buf) {
        LOGE(TAG, "sock send buf allocate %u fail!\r\n", size);
        goto err;
    }

    if (aos_queue_new(&sock_send_queue, sock_send_buf, size,
                      sizeof(sock_send_info_t)) != 0) {
        LOGE(TAG, "sock send queue create fail!\r\n");
        goto err;
    }

    return 0;
err:
    aos_free(sock_send_buf);

    aos_queue_free(&sock_send_queue);

    return -1;
}


static int sock_send_queue_finalize()
{
    sock_send_info_t *sock_send_buf = NULL;

    if (!aos_queue_is_valid(&sock_send_queue)) {
        return -1;
    }

    sock_send_buf = (sock_send_info_t *)aos_queue_buf_ptr(&sock_send_queue);
    aos_free(sock_send_buf);

    aos_queue_free(&sock_send_queue);

    return 0;
}

int insert_sock_send_msg(int sockfd, uint8_t *dataptr, uint16_t datalen)
{
    sock_send_info_t sock_send_buf;

    if (sockfd < 0 || !dataptr || !datalen) {
        return -1;
    }

    if (!aos_queue_is_valid(&sock_send_queue)) {
        return -1;
    }

    memset(&sock_send_buf, 0, sizeof(sock_send_info_t));
    sock_send_buf.dataptr = (uint8_t *)aos_malloc(datalen);
    if (!sock_send_buf.dataptr) {
        LOGE(TAG, "Sock send msg allocate fail\n");
        goto err;
    }

    memcpy(sock_send_buf.dataptr, dataptr, datalen);
    sock_send_buf.datalen = datalen;

    if (aos_queue_send(&sock_send_queue, &sock_send_buf,
                       sizeof(sock_send_buf)) != 0) {
        LOGE(TAG, "Error: sock queue send fail, total fail %lu!\r\n",
             ++sock_send_statistic.put_error);
        goto err;
    }

    sock_send_statistic.total_byte += datalen;
    LOGD(TAG, "insert sock send data datalen %d total %lu\n", (int)datalen,
         sock_send_statistic.total_byte);

    return 0;

err:
    free_sock_send_msg(&sock_send_buf);
    return -1;
}
#endif

void free_sock_send_msg(sock_send_info_t *msgptr)
{
    if (!msgptr) {
        return;
    }

    aos_free(msgptr->dataptr);
}

// return total byte sent
int send_over_sock(sock_send_info_t *msgptr)
{
    int                size = 0, sent_len = 0;
    int                type;
    struct sockaddr_in remote;
    int                remotelen;

    if (!msgptr || msgptr->sockfd < 0 || !msgptr->dataptr || !msgptr->datalen) {
        LOGE(TAG, "invalid sock data parameter!\n");
        return -1;
    }

    if ((type = find_conntype_by_sockfd(msgptr->sockfd)) < 0) {
        LOGE(TAG, "Invalid connect type for sock %d\n", msgptr->sockfd);
        return -1;
    }

    if (type == UDP_BROADCAST &&
        find_remoteaddr_by_sockfd(msgptr->sockfd, &remote) < 0) {
        LOGE(TAG, "Invalid remote addr for broadcast udp sock %d\n",
             msgptr->sockfd);
        return -1;
    }

    LOGD(TAG, "socket %d going to send data len %d!\n", msgptr->sockfd,
         msgptr->datalen);

    if (type == UDP_BROADCAST) {
        remotelen = sizeof(remote);
        if (sendto(msgptr->sockfd, msgptr->dataptr, msgptr->datalen, 0,
                   (struct sockaddr *)&remote, remotelen) <= 0) {
            LOGE(TAG,
                 "udp broadcast sock %d send data failed, errno = %d. \r\n",
                 msgptr->sockfd, errno);
            return -1;
        }
    } else {
        if ((sent_len = send(msgptr->sockfd, msgptr->dataptr, msgptr->datalen, 0))
            <= 0) {
            LOGE(TAG, "sock %d send data failed, errno = %d. \r\n",
                 msgptr->sockfd, errno);
            return -1;
        }

        if (sent_len != msgptr->datalen) {
            LOGE(TAG, "sock %d want %d only sent %d. \r\n",
                 msgptr->sockfd, msgptr->datalen, sent_len);
            return -1;
        }
    }

    size += msgptr->datalen;

    return size;
}

// socket data send go through this task
void socket_send_task()
{
    int              ret;
    uint32_t         size = 0, sent_size = 0;
    sock_send_info_t msg;

    LOG("Socket send task starts!\r\n");

    while (true) {
        if (!inited) {
            LOGE(TAG, "cip not inited!\r\n");
            goto exit;
        }

        if (!aos_queue_is_valid(&sock_send_queue)) {
            LOGE(TAG, "Error sock send queue invalid!");
            goto exit;
        }

        memset(&msg, 0, sizeof(sock_send_info_t));
        ret = aos_queue_recv(&sock_send_queue, AOS_WAIT_FOREVER, &msg, &size);
        if (ret != 0) {
            LOGE(
              TAG,
              "Error sock send queue recv, errno %d, total fetch error %lu\r\n",
              ret, ++sock_send_statistic.fetch_error);
            goto done;
        }

        if (size != sizeof(sock_send_info_t)) {
            LOGE(TAG, "Error sock send recv: msg size %lu is not valid\r\n",
                 size);
            goto done;
        }

        if ((sent_size = send_over_sock(&msg)) <= 0) {
            LOGE(TAG, "Error sock send fail, errno %d, total fail %lu\n",
                 errno, ++sock_send_statistic.send_error);
            goto done;
        }

    done:
        if (sent_size > 0 && sent_size != msg.datalen) {
            LOGE(TAG, "Erro send %lu datalen %d\n", sent_size, (int)msg.datalen);
        }

        if (sock_send_statistic.total_byte >= msg.datalen) {
            sock_send_statistic.total_byte -= msg.datalen;
            LOGD(TAG, "sock send queue remain size %lu \r\n",
                 sock_send_statistic.total_byte);
        } else {
            LOGE(TAG, "Error: sock send queue remain %lu sent %lu \r\n",
                 sock_send_statistic.total_byte, sent_size);

            sock_send_statistic.total_byte = 0;
        }

        free_sock_send_msg(&msg);
    }

exit:
    LOG("Socket send task exits!\r\n");
    aos_task_exit(0);
}

void send_socket_data_task(void *arg)
{
    sock_send_info_t *sendarg = NULL;

    if (!arg) {
        goto exit;
    }

    sendarg = (sock_send_info_t *)arg;

    if (sendarg->sockfd < 0 || sendarg->dataptr == NULL ||
        sendarg->datalen <= 0) {
        LOGE(TAG, "invalid socket %d data len %d\n", sendarg->sockfd,
             sendarg->datalen);
        goto exit;
    }

    LOGD(TAG, "socket %d going to send data len %d!\n", sendarg->sockfd,
         sendarg->datalen);

    if (send(sendarg->sockfd, sendarg->dataptr, sendarg->datalen, 0) <= 0) {
        LOGE(TAG, "send data failed, errno = %d. \r\n", errno);
    }

exit:
    if (sendarg)
        aos_free(sendarg->dataptr);
    aos_free(arg);
    aos_task_exit(0);
}

static int get_conntype_index(char *str)
{
    const char *conntype_str[] = { "tcp_server", "tcp_client", "ssl_client",
                                   "udp_broadcast", "udp_unicast" };
    int         i;

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
    for (i = 0; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd == fd) {
            linkid = g_link[i].linkid;
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return linkid;
}

static int update_remoteaddr_by_sockfd(int fd, struct sockaddr_in *remoteaddr)
{
    int i;

    if (fd < 0 || !remoteaddr) {
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd == fd) {
            memcpy(&g_link[i].remoteaddr, remoteaddr,
                   sizeof(struct sockaddr_in));
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return 0;
}

static int find_remoteaddr_by_sockfd(int fd, struct sockaddr_in *remoteaddr)
{
    int i;

    if (fd < 0 || !remoteaddr) {
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd == fd) {
            memcpy(remoteaddr, &g_link[i].remoteaddr,
                   sizeof(struct sockaddr_in));
            if (IPADDR_NONE == remoteaddr->sin_addr.s_addr) {
                return -1;
            }
        }
    }
    aos_mutex_unlock(&g_link_mutex);

    return 0;
}

static int find_conntype_by_sockfd(int fd)
{
    int i;
    int type = -1;

    if (fd < 0) {
        return -1;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < LINK_ID_MAX; i++) {
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
    for (i = 0; i < LINK_ID_MAX; i++) {
        if (g_link[i].fd >= 0 && g_link[i].linkid == linkid) {
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
            g_link[i].fd     = fd;
            g_link[i].type   = type;
            g_link[i].linkid = linkid;
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
            g_link[i].fd     = -1;
            g_link[i].linkid = -1;
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
 */
static int notify_cip_data_recv_event(int sockid, char *databuf, int datalen,
                                      struct sockaddr_in *remote)
{
    char *type_str;
    char  addr_str[16] = { 0 }; // ipv4 only
    int   port;
    char  port_str[10]                            = { 0 };
    char  linkid_str[10]                          = { 0 };
    char  datalen_str[10]                         = { 0 };
    char  sendbuf[MAX_ATCMD_DATA_RECV_PREFIX_LEN] = { 0 };
    int   sendbuflen = MAX_ATCMD_DATA_RECV_PREFIX_LEN, offset = 0;
    int   type, linkid;

    if (sockid < 0) {
        LOGE(TAG, "Invalid sock id %d!\n", sockid);
        goto err;
    }

    if ((type = find_conntype_by_sockfd(sockid)) < 0) {
        LOGE(TAG, "Invalid connect type for sock %d\n", sockid);
        goto err;
    }

    if (type == UDP_BROADCAST && !remote) {
        LOGE(TAG, "Sock %d remote addr missing for udp broadcast!\n",
             sockid);
        goto err;
    }

    if (type == UDP_BROADCAST) {
        char *remoteip;
        type_str = "UDP_BROADCAST";

        remoteip = inet_ntoa(remote->sin_addr);
        strncpy(addr_str, remoteip, sizeof(addr_str));
        port = ntohs(remote->sin_port);
    } else {
        type_str = "SOCKET";
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
        offset +=
          snprintf(sendbuf + offset, sendbuflen - offset, "%s,", type_str);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__, sendbuf);
        goto err;
    }

    if (type == UDP_BROADCAST) {
        // ip
        if (offset + strlen(addr_str) + 1 < sendbuflen) {
            offset +=
              snprintf(sendbuf + offset, sendbuflen - offset, "%s,", addr_str);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 sendbuf);
            goto err;
        }

        // port
        itoa_decimal(port, port_str);
        if (offset + strlen(port_str) + 1 < sendbuflen) {
            offset +=
              snprintf(sendbuf + offset, sendbuflen - offset, "%s,", port_str);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 sendbuf);
            goto err;
        }
    }

    itoa_decimal(linkid, linkid_str);
    // append id
    if (offset + strlen(linkid_str) + 1 < sendbuflen) {
        offset +=
          snprintf(sendbuf + offset, sendbuflen - offset, "%s,", linkid_str);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__, sendbuf);
        goto err;
    }

    itoa_decimal(datalen, datalen_str);
    // append datalen
    if (offset + strlen(datalen_str) + 1 < sendbuflen) {
        offset +=
          snprintf(sendbuf + offset, sendbuflen - offset, "%s,", datalen_str);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__, sendbuf);
        goto err;
    }

    if (atcmd_write(sendbuf, (uint8_t *) databuf, strlen(sendbuf), datalen) != 0) {
        LOGE(TAG, "Error send msg fail\r\n");
        goto err;
    }

    return 0;

err:
    return -1;
}

void udp_broadcast_recv_task(void *arg)
{
    char              *buf = NULL;
    int                len = 0;
    int                fd  = *((int *)arg);
    fd_set             readfds;
    uint32_t           remoteaddrlen;
    struct sockaddr_in remoteaddr;

    aos_free(arg);

    buf = (char *)aos_malloc(MAX_RECV_BUF_SIZE);
    if (NULL == buf) {
        LOGE(TAG, "fail to malloc memory %d at %s %d \r\n", MAX_RECV_BUF_SIZE,
             __FUNCTION__, __LINE__);
        goto exit;
    }

    LOG("New udp broadcast task starts on socket %d\n", fd);

    while (1) {
        if (find_linkid_by_sockfd(fd) < 0) {
            LOGD(TAG, "Client exit on socket %d\n", fd);
            goto exit;
        }

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        if (select(fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            LOGE(TAG, "Select fail! Client task exit!");
            goto exit;
        }

        if (FD_ISSET(fd, &readfds)) {
            memset(&remoteaddr, 0, sizeof(remoteaddr));
            len = recvfrom(fd, buf, MAX_RECV_BUF_SIZE, 0,
                           (struct sockaddr *)&remoteaddr, &remoteaddrlen);

            if (0 == len) {
                LOG("Client task (fd = %d) exit normally! ret %d \n", fd, len);
                goto exit;
            } else if (len < 0) {
                // TODO for some errror connection should be held
                LOGE(TAG, "Client task (fd = %d) recv error! ret %d errno %d\n",
                     fd, len, errno);
                goto exit;
            }

            LOGD(TAG, "Client task (fd = %d) recv len %d\n", fd, len);
            notify_cip_data_recv_event(fd, buf, len, &remoteaddr);
        }
    }

exit:
    aos_free(buf);

    // need to close by task
    if (find_linkid_by_sockfd(fd) >= 0) {
        notify_cip_connect_status_events(fd, CIP_STATUS_CLOSED, 0);
        delete_link_info_by_sockfd(fd);
    }

    close(fd);
    aos_task_exit(1);
}

void tcp_udp_client_recv_task(void *arg)
{
    char  *buf = NULL;
    int    len = 0;
    int    fd  = *((int *)arg);
    fd_set readfds;

    aos_free(arg);

    buf = (char *)aos_malloc(MAX_RECV_BUF_SIZE);
    if (NULL == buf) {
        LOGE(TAG, "fail to malloc memory %d at %s %d \r\n", MAX_RECV_BUF_SIZE,
             __FUNCTION__, __LINE__);
        goto exit;
    }

    LOG("New tcp/udp client task starts on socket %d\n", fd);

    while (1) {
        if (find_linkid_by_sockfd(fd) < 0) {
            LOGD(TAG, "tcp/udp client exit on socket %d\n", fd);
            goto exit;
        }

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        if (select(fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            LOGE(TAG, "Select fail! Client task exit!");
            goto exit;
        }

        if (FD_ISSET(fd, &readfds)) {
            len = recv(fd, buf, MAX_RECV_BUF_SIZE, 0);
            if (0 == len) {
                LOG("Client task (fd = %d) exit normally! ret %d \n", fd, len);
                goto exit;
            } else if (len < 0) {
                // TODO for some errror connection should be held
                LOGE(TAG, "Client task (fd = %d) recv error! ret %d errno %d\n",
                     fd, len, errno);
                goto exit;
            }

            LOGD(TAG, "Client task (fd = %d) recv len %d\n", fd, len);
            notify_cip_data_recv_event(fd, buf, len, NULL);
        }
    }

exit:
    aos_free(buf);

    // need to close by task
    if (find_linkid_by_sockfd(fd) >= 0) {
        notify_cip_connect_status_events(fd, CIP_STATUS_CLOSED, 0);
        delete_link_info_by_sockfd(fd);
    }

    close(fd);
    aos_task_exit(1);
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
static int notify_cip_connect_status_events(int sockid, int status,
                                            int recvstatus)
{
    char *status_str;
    char *type_str;
    char  addr_str[16] = { 0 }; // ipv4 only
    int   port =  -1;
    char  port_str[6]                   = { 0 };
    char  cmd[MAX_ATCMD_CON_STATUS_LEN] = { 0 };
    int   offset                        = 0;
    int   type, linkid;

    if (sockid < 0) {
        LOGE(TAG, "Invalid sock id %d!\n", sockid);
        goto err;
    }

    if (status == CIP_STATUS_CONNECTED) {
        status_str = "CONNECTED";
    } else if (status == CIP_STATUS_CLOSED) {
        status_str = "CLOSED";
    } else {
        LOGE(TAG, "Invalid connect status %d!\n", status);
        goto err;
    }

    type = find_conntype_by_sockfd(sockid);
    if (type == TCP_CLIENT || type == SSL_CLIENT) {
        type_str = "SERVER";
    } else if (type == TCP_SERVER) {
        struct sockaddr_in peer;
        uint32_t           peerlen = sizeof(struct sockaddr_in);
        char              *remoteip;
        type_str = "CLIENT";

        if (getpeername(sockid, (struct sockaddr *)&peer, &peerlen) != 0) {
            LOGE(TAG, "Fail to sock %d get remote address!\n", sockid);
            goto err;
        }

        remoteip = inet_ntoa(peer.sin_addr);
        // check len
        if (strlen(remoteip) >= sizeof(addr_str)) {
            LOGE(TAG, "invalid remote ip addr len %d\n", strlen(remoteip));
            goto err;
        }

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
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                               "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }
    }

    // prefix
    if (offset + strlen(prefix_cipevent) < MAX_ATCMD_CON_STATUS_LEN) {
        offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                           "%s", prefix_cipevent);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    if (type == TCP_SERVER) {
        // do nothing
    } else {
        char linkid_str[5] = { 0 };

        itoa_decimal(linkid, linkid_str);
        LOGD(TAG, "linkid %d linkid str -->%s<--\n", linkid, linkid_str);
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
        offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
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
        if (port < 0) {
            LOGE(TAG, "invalid port %d\n", port);
            goto err;
        }

        itoa_decimal(port, port_str);
        if (offset + strlen(port_str) + 1 + 1 < MAX_ATCMD_CON_STATUS_LEN) {
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                               ",%s\r", port_str);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }
    }

    if (recvstatus > 0) {
        if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
            MAX_ATCMD_CON_STATUS_LEN) {
            offset += snprintf(cmd + offset, MAX_ATCMD_CON_STATUS_LEN - offset,
                               "%s", AT_RECV_SUCCESS_POSTFIX);
        } else {
            LOGE(TAG, "at string too long %s\n", cmd);
            goto err;
        }
    }

    if (insert_uart_send_msg(cmd, NULL, strlen(cmd), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;

err:
    return -1;
}

static const char *task_name_tcp_client = "tcp_client_task";
static const char *task_name_udp_unicast = "udp_unicast_task";
static const char *task_name_udp_broadcast = "udp_broadcast_task";
// AT+CIPSTART=
// tcp_server: id,tcp_server,local_port
// tcp_client/ssl_client: id,tcp_client,domain,remote_port[,local_port]
// udp_broadcast: id,udp_broadcast,domain,remote_port,local_port
// udp_unicast: id,udp_unicast,domain,remote_port,local_port
int atcmd_cip_start()
{
    char               single;
    char               body[16];
    int                ret;
    int                linkid;
    int                type;
    char               remoteip[16] = { 0 };
    uint16_t           remoteport, localport;
    struct sockaddr_in remoteaddr, localaddr;
    int                fd = -1;
    int                socktype;
    recv_task_t        recvtsk = NULL;
    const char       **tskname = NULL;
    int               *tskarg      = NULL;
    int                stacksize   = 1024;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    atcmd_read(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = atcmd_socket_data_info_get(body, sizeof(body),
                                     &atcmd_socket_data_len_check);
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
    ret = atcmd_socket_data_info_get(body, sizeof(body),
                                     &atcmd_socket_conntype_check);
    if (ret < 0 || (type = get_conntype_index(body)) < 0) {
        LOGE(TAG, "Invalid connect type %s !!!\r\n", body);
        goto err;
    }

    if (type != TCP_SERVER) {
        // remote ip
        ret = atcmd_socket_data_info_get(remoteip, sizeof(remoteip),
                                         &atcmd_socket_ip_info_check);
        if (ret < 0) {
            LOGE(TAG, "Invalid ip addr %s !!!\r\n", remoteip);
            goto err;
        }
    }

    // remote port
    memset(body, 0, sizeof(body));
    ret = atcmd_socket_data_info_get(body, sizeof(body),
                                     &atcmd_socket_data_len_check);
    if (ret < 0) {
        LOGE(TAG, "Invalid remote portno %s !!!\r\n", body);
        goto err;
    }
    LOGD(TAG, "remote port %s\n", body);
    remoteport = atoi(body);

    if (type == TCP_SERVER) {
        localport = remoteport;
    } else {
        memset(&remoteaddr, 0, sizeof(remoteaddr));
        remoteaddr.sin_port = htons(remoteport);
        if (0 == remoteaddr.sin_port) {
            LOGE(TAG, "invalid input port info %u \r\n", remoteport);
            goto err;
        }

        remoteaddr.sin_addr.s_addr = inet_addr(remoteip);
        if (IPADDR_NONE == remoteaddr.sin_addr.s_addr) {
            LOGE(TAG, "invalid input addr info %s \r\n", remoteip);
            goto err;
        }

        remoteaddr.sin_family = AF_INET;
    }

    if (type == TCP_CLIENT || type == SSL_CLIENT || type == TCP_SERVER) {
        socktype = SOCK_STREAM;
    } else if (type == UDP_BROADCAST || type == UDP_UNICAST) {
        socktype = SOCK_DGRAM;

        // local port
        memset(body, 0, sizeof(body));
        ret = atcmd_socket_data_info_get(body, sizeof(body),
                                         &atcmd_socket_data_len_check);
        if (ret < 0) {
            LOGE(TAG, "Invalid local portno %s !!!\r\n", body);
            goto err;
        }
        LOGD(TAG, "local port %s\n", body);
        localport = atoi(body);
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
        LOGD(TAG, "remote addr %lu port %u \n", remoteaddr.sin_addr.s_addr,
             remoteport);
        if (connect(fd, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) !=
            0) {
            LOGE(TAG, "TCP Connect failed, errno = %d, ip %s port %u \r\n",
                 errno, remoteip, remoteport);
            goto err;
        }
        LOGD(TAG, "TCP client connect success!\n");

        recvtsk = tcp_udp_client_recv_task;
        tskname = &task_name_tcp_client;
        stacksize = 2048; // TODO need set by configuration
    } else if (type == UDP_UNICAST) {
        localaddr.sin_family      = AF_INET;
        localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        localaddr.sin_port        = htons(localport);

        if (bind(fd, (struct sockaddr *)&localaddr, sizeof(localaddr)) != 0) {
            LOGE(TAG,
                 "UDP unicast sock bind failed, errno = %d, local port %u \r\n",
                 errno, localport);
            goto err;
        }
        LOGD(TAG, "UDP unicast sock bind success!\n");

        if (connect(fd, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) !=
            0) {
            LOGE(TAG,
                 "UDP unicast connect failed, errno = %d, ip %s port %u \r\n",
                 errno, remoteip, remoteport);
            goto err;
        }
        LOGD(TAG, "UDP unicast sock connect success!\n");

        recvtsk = tcp_udp_client_recv_task;
        tskname = &task_name_udp_unicast;
        stacksize = 1024; // TODO need set by configuration
    } else if (type == UDP_BROADCAST) {
        int   broadcast = 1;

        if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast,
                       sizeof(broadcast)) != 0) {
            LOGE(TAG, "setsockopt SO_BROADCAST fail, errno = %d \r\n", errno);
            goto err;
        }

        localaddr.sin_family      = AF_INET;
        localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        localaddr.sin_port        = htons(localport);

        if (bind(fd, (struct sockaddr *)&localaddr, sizeof(localaddr)) != 0) {
            LOGE(
              TAG,
              "UDP broadcast sock bind failed, errno = %d, local port %u \r\n",
              errno, localport);
            goto err;
        }
        LOGD(TAG, "UDP broadcast sock bind success!\n");

        /*if (connect(fd, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) !=
        0) { LOGE(TAG, "UDP connect failed, errno = %d, ip %s port %u \r\n",
        errno, remoteip, remoteport); goto err;
        }
        LOGD(TAG, "UDP sock connect success!\n");*/

        update_remoteaddr_by_sockfd(fd, &remoteaddr);

        recvtsk = udp_broadcast_recv_task;
        tskname = &task_name_udp_broadcast;
        stacksize = 1024; // TODO need set by configuration
    } else if (type == TCP_SERVER) {
        LOGW(TAG, "TCP server not implement yet!\n");
        goto err;
    }

    // recv task args
    tskarg = (int *)aos_malloc(sizeof(int));
    if (tskarg == NULL) {
        LOGE(TAG, "Fail to allcate memory %d byte for task arg\r\n",
             (sizeof(int)));
        goto err;
    }
    *tskarg = fd;
    // save global info for new socket
    if (add_link_info(fd, linkid, type) != 0) {
        LOGE(TAG, "Fail to add link info for sock %d linkid %d type %d\r\n", fd,
             linkid, type);
        goto err;
    }

    if (aos_task_new(*tskname, recvtsk, (void *)tskarg, stacksize) != 0) {
        LOGE(TAG, "Fail to create task %s\r\n", *tskname);
        delete_link_info_by_sockfd(fd);
        goto err;
    }

    // notify over uart
    if (notify_cip_connect_status_events(fd, CIP_STATUS_CONNECTED,
                                         ATCMD_SUCCESS) != 0) {
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

    aos_free(tskarg);

    return -1;
}

// AT+CIPSEND=linkid,<remote_port>,datalen
int at_cip_send()
{
    char             single;
    char             body[16];
    char            *recvdata = NULL;
    int              linkid, sockid;
    int              remoteport, datalen = 0;
    int              ret;
    int              readsize;
    sock_send_info_t sendpara;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    atcmd_read(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip send prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = atcmd_socket_data_info_get(body, sizeof(body),
                                     &atcmd_socket_data_len_check);
    if (ret < 0 || (linkid = atoi(body)) < 0) {
        LOGE(TAG, "Invalid link id %s !!!\r\n", body);
        goto err;
    }

    // check linkid exist
    if ((sockid = find_sockfd_by_linkid(linkid)) < 0) {
        LOGE(TAG, "link id %d does not exist !!!\r\n", linkid);
        goto err;
    }

    // try get remote port
    memset(body, 0, sizeof(body));
    ret = atcmd_socket_data_info_get(body, sizeof(body),
                                     &atcmd_socket_data_len_check);
    if (ret < 0 || (remoteport = atoi(body)) < 0) {
        LOGE(TAG, "Invalid port %s !!!\r\n", body);
        goto err;
    }

    LOGD(TAG, "get remote port %d ret %d\n", remoteport, ret);
    if (ret == 0) {
        memset(body, 0, sizeof(body));
        ret = atcmd_socket_data_info_get(body, sizeof(body),
                                         &atcmd_socket_data_len_check);
        if (ret < 0 || (datalen = atoi(body)) < 0) {
            LOGE(TAG, "Invalid link id %s !!!\r\n", body);
            goto err;
        }
    } else if (ret == 1) {
        datalen = remoteport;
    }

    LOGD(TAG, "%s socket data on link %d with length %d to remote\n", __func__,
         linkid, datalen);

    // Prepare socket data
    recvdata = (char *)aos_malloc(datalen);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory, len is %d. \r\n", __func__,
             __LINE__, datalen);
        goto err;
    }

    if ((readsize = atcmd_read(recvdata, datalen)) <= 0) {
        LOGE(TAG, "Error at read data \r\n");
        goto err;
    }

    if (readsize != datalen) {
        LOGE(TAG, "Error datalen: %d readsize: %d\n", datalen, readsize);
        goto err;
    }

#if AT_CHECK_SUM
    uint8_t checksum = 0;

    for (int i = 0; i < datalen - 1; i++) {
        checksum += (uint8_t)recvdata[i];
    }

    if (checksum != (uint8_t)recvdata[datalen - 1]) {
        LOGE(TAG, "Error checksum exam fail!!! want %u but is %u",
            checksum, recvdata[datalen - 1]);
        goto err;
    }

    datalen -= 1;
#endif

    LOGD(TAG, "CIPSend datalen: %d readsize: %d\n", datalen, readsize);

    // TODO: what to do with remote port recvdata
#if AT_SOCKET_ASYNC_SEND_MODE
    if (insert_sock_send_msg(sockid, recvdata, datalen) != 0) {
        LOGE(TAG, "Error insert send socket fail \r\n");
        goto err;
    }
#else
    sendpara.sockfd  = sockid;
    sendpara.dataptr = (uint8_t *) recvdata;
    sendpara.datalen = datalen;

    if (send_over_sock(&sendpara) <= 0) {
        LOGE(TAG, "Error send socket data fail \r\n");
        goto err;
    }
#endif

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
    int  ret;
    int  linkid, sockfd;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    atcmd_read(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = atcmd_socket_data_info_get(body, sizeof(body),
                                     &atcmd_socket_data_len_check);
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
    notify_atcmd_recv_status(ATCMD_FAIL);
    return -1;
}

// TODO: need flash read / write
// AT+CIPAUTOCONN=linkid,0/1
int atcmd_cip_auto_connect()
{
    char single;
    char body[5];
    int  ret;
    int  linkid, sockfd;
    int  auto_set;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    atcmd_read(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // link id
    memset(body, 0, sizeof(body));
    ret = atcmd_socket_data_info_get(body, sizeof(body),
                                     &atcmd_socket_data_len_check);
    if (ret < 0 || (linkid = atoi(body)) < 0) {
        LOGE(TAG, "Invalid link id %s !!!\r\n", body);
        goto err;
    }

    // set bit
    atcmd_read(&single, 1);
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
        notify_cip_connect_status_events(sockfd, CIP_STATUS_CLOSED,
                                         ATCMD_SUCCESS);
        delete_link_info_by_sockfd(sockfd);
    }

    // TODO: change the setting on the flash
    return 0;

err:
    notify_atcmd_recv_status(ATCMD_FAIL);
    return -1;
}

#define MAX_ATCMD_DOMAIN_RSP_LEN 80
// AT+CIPDOMAIN=domain
// Respone: AT+CIPDOMAIN:180.97.33.108
int atcmd_cip_domain_dns()
{
    const char      *prefix_cipdomain = "+CIPDOMAIN:";
    char             single;
    char             domain[MAX_ATCMD_DOMAIN_LEN];
    char             addr_str[16] = {0}; // ipv4 only
    char             response[MAX_ATCMD_DOMAIN_RSP_LEN] = { 0 };
    int              ret;
    struct hostent  *host;
    struct in_addr **addrlist;
    int              offset = 0;
    char            *index;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // Eat '='
    atcmd_read(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // domain
    memset(domain, 0, sizeof(domain));
    ret = atcmd_socket_data_info_get(domain, sizeof(domain), NULL);
    if (ret < 0) {
        LOGE(TAG, "Invalid domain %s !!!\r\n", domain);
        goto err;
    }

    if ((host = gethostbyname(domain)) == NULL) {
        LOGE(TAG, "fail to find domain %s !!!\r\n", domain);
        goto err;
    }

    addrlist = (struct in_addr **)host->h_addr_list;
    // return the first one
    strncpy(addr_str, inet_ntoa(*addrlist[0]), sizeof(addr_str));

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_DOMAIN_RSP_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_RSP_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (offset + strlen(prefix_cipdomain) < MAX_ATCMD_DOMAIN_RSP_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_RSP_LEN - offset,
                           "%s", prefix_cipdomain);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // default 0
    index = "0";
    if (offset + strlen(index) < MAX_ATCMD_DOMAIN_RSP_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_RSP_LEN - offset,
                           "%s", index);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIXf
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_DOMAIN_RSP_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_RSP_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (offset + strlen(addr_str) < MAX_ATCMD_DOMAIN_RSP_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_RSP_LEN - offset,
                           "%s", addr_str);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_DOMAIN_RSP_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_RSP_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_DOMAIN_RSP_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_DOMAIN_RSP_LEN - offset,
                           "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;
err:
    notify_atcmd_recv_status(ATCMD_FAIL);
    return -1;
}

enum
{
    ATCMD_CIP_DOMAIN = 0,
    ATCMD_CIP_AUTOCONN,
    ATCMD_CIP_START,
    ATCMD_CIP_STOP,
    ATCMD_CIP_SEND,
};

static const atcmd_hdl_t at_cip_cmds_table[] = {
    { .name     = "AT+CIPDOMAIN",
      .help     = "AT+CIPDOMAIN",
      .function = atcmd_cip_domain_dns },
    { .name     = "AT+CIPAUTOCONN",
      .help     = "AT+CIPAUTOCONN=<id>[,option]",
      .function = atcmd_cip_auto_connect },
    { .name     = "AT+CIPSTART",
      .help     = "AT+CIPSTART",
      .function = atcmd_cip_start },
    { .name = "AT+CIPSTOP", .help = "AT+CIPSTOP", .function = atcmd_cip_stop },
    { .name     = "AT+CIPSEND",
      .help     = "AT+CIPSEND=<id>,[<remote_port>,]<data_length>",
      .function = at_cip_send }
};

static atcmd_hdl_ptr_t get_atcmd_cip_handler()
{
    const char *cmd_prefix                   = "IP";
    char        prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    char        single;
    int         index = -1;

    // Eat "IP"
    atcmd_read(prefix, strlen(cmd_prefix));
    if (memcmp(prefix, cmd_prefix, strlen(cmd_prefix)) != 0) {
        LOGE(TAG, "invalid cip prefix %s\n", prefix);
        return NULL;
    }

    atcmd_read(&single, 1);

    switch (single) {
        case 'S':
            atcmd_read(prefix, 3);

            if (memcmp(prefix, "TAR", 3) == 0) {
                // Eat 'T'
                atcmd_read(&single, 1);
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
            atcmd_read(prefix, 5);
            index = ATCMD_CIP_DOMAIN;
            break;

        case 'A':
            // Eat "UTOCONN"
            atcmd_read(prefix, 7);
            index = ATCMD_CIP_AUTOCONN;
            break;

        default:
            LOGE(TAG, "invalid cip prefix %c\n", single);
            break;
    }

    if (index >= 0 && index < sizeof(at_cip_cmds_table)) {
        return  (atcmd_hdl_ptr_t) &at_cip_cmds_table[index];
    }

    return NULL;
}

static int cip_init()
{
    if (aos_mutex_new(&g_link_mutex) != 0) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        goto err;
    }

    memset(g_link, 0, sizeof(g_link));
    for (int i = 0; i < LINK_ID_MAX; i++) {
        g_link[i].fd     = -1;
        g_link[i].linkid = -1;
    }

    // async transmit not used for now
#if AT_SOCKET_ASYNC_SEND_MODE
    if (sock_send_queue_init() != 0) {
        LOGE(TAG, "Creating sock send que fail (%s %d).", __func__, __LINE__);
        goto err;
    }

    if (aos_task_new("athost_socket_send_task", socket_send_task, NULL, 1024) !=
        0) {
        LOGE(TAG, "Fail to create socket send task\r\n");
        goto err;
    }
#endif

    inited = true;

    return 0;

err:
    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

#if AT_SOCKET_ASYNC_SEND_MODE
    sock_send_queue_finalize();
#endif

    inited = false;

    return -1;
}

static void cip_deinit()
{
    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

#if AT_SOCKET_ASYNC_SEND_MODE
    sock_send_queue_finalize();
#endif

    inited = false;
}

atcmd_op_t cip_op = {
    .name              = "cip",
    .prefix            = "C",
    .init              = cip_init,
    .deinit            = cip_deinit,
    .get_atcmd_handler = get_atcmd_cip_handler,
};

int register_atcmd_cip_opt()
{
    return atcmd_register_handlers(&cip_op);
}
