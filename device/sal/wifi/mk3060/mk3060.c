/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <aos/aos.h>
#include <atparser.h>
#include <sal_arch.h>
#include <sal_ipaddr.h>
#include <sal.h>

#define TAG "sal_wifi"

#define CMD_SUCCESS_RSP "OK"
#define CMD_FAIL_RSP "ERROR"

#define MAX_DOMAIN_LEN 256
#define DATA_LEN_MAX 10
#define LINK_ID_MAX 5

#define STOP_CMD "AT+CIPSTOP"
#define STOP_CMD_LEN (sizeof(STOP_CMD)+1+1+5+1)

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

static link_t g_link[LINK_ID_MAX];
static aos_mutex_t g_link_mutex;
static netconn_data_input_cb_t g_netconn_data_input_cb;

static void handle_tcp_udp_client_conn_state(uint8_t link_id)
{
    char s[32] = {0};

    at.read(s, 6);
    if (strstr(s, "CLOSED") != NULL) {
        LOGI(TAG, "Server closed event.");
        // Clear fd_map here
        if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
            LOGE(TAG, "Failed to lock mutex (%s).", __func__);
            return;
        }
        g_link[link_id].fd = -1;
        aos_mutex_unlock(&g_link_mutex);
        if (aos_sem_is_valid(&g_link[link_id].sem_close)) {
            LOGD(TAG, "sem is going to be waked up: 0x%x", &g_link[link_id].sem_close);
            aos_sem_signal(&g_link[link_id].sem_close); // wakeup send task
        }
        LOGI(TAG, "Server conn (%d) closed.", link_id);
    } else if (strstr(s, "CONNEC") != NULL){
        LOGI(TAG, "Server conn (%d) successful.", link_id);
        at.read(s, 3);
        if (aos_sem_is_valid(&g_link[link_id].sem_start)) {
            LOGD(TAG, "sem is going to be waked up: 0x%x", &g_link[link_id].sem_start);
            aos_sem_signal(&g_link[link_id].sem_start); // wakeup send task
        }
    } else if (strstr(s, "DISCON") != NULL) {
        LOGI(TAG, "Server conn (%d) disconnected.", link_id);
        at.read(s, 6);
    } else {
        LOGW(TAG, "No one handle this unkown event!!!");
    }
}

static void handle_client_conn_state()
{

}

static void handle_socket_data()
{
    int link_id, i, j;
    uint32_t len;
    char reader[16];
    char *recvdata = NULL;

    /* Eat the "OCKET," */
    at.read(reader, 6);
    if (memcmp(reader, "OCKET,", strlen("OCKET,")) != 0) {
        LOGE(TAG, "%s invalid event format!!!");
        assert(0);
    }

    at.read(&reader[0], 1);
    link_id = reader[0] - '0';
    if ((link_id < 0) || (link_id >= LINK_ID_MAX)) {
        LOGE(TAG, "Invalid link id!!!");
        assert(0);
    }

    /* Eat the , char */
    at.read(&reader[0], 1);

    /* len */
    i = 0;
    do {
        at.read(&reader[i], 1);
        if (reader[i] == ',') break;
        if (i >= DATA_LEN_MAX) {
            LOGE(TAG, "Too long length of data.");
            assert(0);
        }
        if (reader[i] > '9' || reader[i] < '0') {
            LOGE(TAG, "Invalid len string!!!");
            assert(0);
        }
        i++;
    } while (1);

    /* len: string to number */
    len = 0;
    for (j = 0; j < i; j++) {
        len = len * 10 + reader[j] - '0';
    }

    /* Prepare socket data */
    recvdata = (char *)aos_malloc(len + 1);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory.", __func__, __LINE__);
        assert(0);
    }

    at.read(recvdata, len);
    recvdata[len] = '\0';
    LOGD(TAG, "The socket data is %s", recvdata);
    
    if (g_netconn_data_input_cb && (g_link[link_id].fd >= 0)){
        /* TODO get recv data src ip and port*/
        if (g_netconn_data_input_cb(g_link[link_id].fd, recvdata, len, NULL, 0)){
            LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                 __func__, g_link[link_id].fd, len);
        }
    }
    
    LOGD(TAG, "%s socket data on link %d with length %d posted to sal\n",
         __func__, link_id, len);

    aos_free(recvdata);

}

/**
 * Network connection state event handler. Events includes:
 *   1. +CIPEVENT:id,SERVER,CONNECTED
 *   2. +CIPEVENT:id,SERVER,CLOSED
 *   3. +CIPEVENT:CLIENT,CONNECTED,ip,port
 *   4. +CIPEVENT:CLIENT,CLOSED,ip,port
 *   5. +CIPEVENT:id,UDP,CONNECTED
 *   6. +CIPEVENT:id,UDP,CLOSED
 *   7. +CIPEVENT:SOCKET,id,len,data
 */
static void net_event_handler(void *arg)
{
    char c;
    char s[32] = {0}; 
    LOGD(TAG, "%s entry.", __func__);

    at.read(&c, 1);
    if (c >= '0' && c < ('0' + LINK_ID_MAX)) {
        int link_id = c - '0';
        at.read(&c, 1);
        if (c != ',') {
            LOGE(TAG, "!!!Error: wrong CIPEVENT string.!!!");
            assert(0);
        }
        at.read(&c, 1);
        if (c == 'S') {
            LOGD(TAG, "%s server conn state event, linkid: %d.", __func__, link_id);
            /* Eat the "ERVER," */
            at.read(s, 6);
            if (memcmp(s, "ERVER,", strlen("ERVER,")) != 0) {
                LOGE(TAG, "%s invalid event format!!!");
                assert(0);
            }
            handle_tcp_udp_client_conn_state(link_id);
        } else if (c == 'U') {
            LOGD(TAG, "%s UDP conn state event.", __func__);
            /* Eat the "DP," */
            at.read(s, 3);
            if (memcmp(s, "DP,", strlen("DP,")) != 0) {
                LOGE(TAG, "%s invalid event format!!!");
                assert(0);
            }
            handle_tcp_udp_client_conn_state(link_id);
        } else {
            LOGE(TAG, "!!!Error: wrong CIPEVENT string.");
            assert(0);
        }
    } else if (c == 'S') {
        LOGD(TAG, "%s socket data event.", __func__);
        handle_socket_data();
    } else if (c == 'C') {
        LOGD(TAG, "%s client conn state event.", __func__);
        handle_client_conn_state();
    } else {
        LOGE(TAG, "!!!Error: wrong CIPEVENT string received.");
        assert(0);;
    }

    LOGD(TAG, "%s exit.", __func__);
}

static uint8_t inited = 0;

#define NET_OOB_PREFIX "+CIPEVENT:"
static int sal_wifi_init(void)
{
    int link;
    char cmd[STOP_CMD_LEN] = {0};
    char out[64] = {0};
    
    if (inited) {
        LOGW(TAG, "sal component is already initialized");
        return 0;
    }

    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    memset(g_link, 0, sizeof(g_link));
    for (link = 0; link < LINK_ID_MAX; link++) {
        g_link[link].fd = -1;
        /*close all link */
        snprintf(cmd, STOP_CMD_LEN - 1, "%s=%d", STOP_CMD, link);
        LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

        at.send_raw(cmd, out, sizeof(out));
        LOGD(TAG, "The AT response is: %s", out);
        if (strstr(out, CMD_FAIL_RSP) != NULL) {
            LOGE(TAG, "%s %d failed", __func__, __LINE__);
            //return -1;
        }
    }
    
    at.oob(NET_OOB_PREFIX, net_event_handler, NULL);
    inited = 1;
    
    return 0;
}

static int sal_wifi_deinit(void)
{
    if (!inited) return 0;

    // at.exitoob(NET_OOB_PREFIX); // <TODO>

    aos_mutex_free(&g_link_mutex);

    return 0;
}

#define START_CMD "AT+CIPSTART"
#define START_CMD_LEN (sizeof(START_CMD)+1+1+13+1+MAX_DOMAIN_LEN+1+5+1+5+1)
static char *start_cmd_type_str[] = {"tcp_server", "tcp_client", \
  "ssl_client", "udp_broadcast", "udp_unicast"};

int sal_wifi_start(sal_conn_t *c)
{
    int link_id;
    char cmd[START_CMD_LEN] = {0};
    char out[256] = {0};

    if (!c || !c->addr) {
        LOGE(TAG, "%s %d - invalid argument", __func__, __LINE__);
        return -1;
    }

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd >= 0) continue;
        else {g_link[link_id].fd = c->fd; break;}
    }

    aos_mutex_unlock(&g_link_mutex);

    // The caller should deal with this failure
    if (link_id >= LINK_ID_MAX) {
        LOGI(TAG, "No link available for now, %s failed.", __func__);
        return -1;
    }

    LOGD(TAG, "Creating %s connection ...", start_cmd_type_str[c->type]);

    switch (c->type) {
    case TCP_SERVER:
        snprintf(cmd, START_CMD_LEN - 1, "%s=%d,%s,%d,",
          START_CMD, link_id, start_cmd_type_str[c->type], c->l_port);
        break;
    case TCP_CLIENT:
    case SSL_CLIENT:
        snprintf(cmd, START_CMD_LEN - 5 - 1, "%s=%d,%s,%s,%d",
          START_CMD, link_id, start_cmd_type_str[c->type],
          c->addr, c->r_port);
        if (c->l_port >= 0)
            snprintf(cmd + strlen(cmd), 7, ",%d", c->l_port);
        break;
    case UDP_BROADCAST:
    case UDP_UNICAST:
        snprintf(cmd, START_CMD_LEN - 1, "%s=%d,%s,%s,%d,%d",
          START_CMD, link_id, start_cmd_type_str[c->type],
          c->addr, c->r_port, c->l_port);
        break;
    default:
        LOGE(TAG, "Invalid connection type.");
        goto err;
    }

    LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at.send_raw(cmd, out, sizeof(out));
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        goto err;
    }
    
    if (aos_sem_new(&g_link[link_id].sem_start, 0) != 0) {
       LOGE(TAG, "failed to allocate semaphore %s", __func__);
       goto err;
    } 
    
    if (aos_sem_is_valid(&g_link[link_id].sem_start)) {
        if (aos_sem_wait(&g_link[link_id].sem_start, AOS_WAIT_FOREVER) != 0) {
            LOGE(TAG, "%s sem_wait failed", __func__);
            goto err;
        }
        aos_sem_free(&g_link[link_id].sem_start);
        g_link[link_id].sem_start.hdl = NULL;
        LOGD(TAG, "%s sem_wait succeed.", __func__);
    }

    return 0;
err:
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    g_link[link_id].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

static int fd_to_linkid(int fd)
{
    int link_id;

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd == fd) break;
    }

    return link_id;
}

#define SEND_CMD "AT+CIPSEND"
#define SEND_CMD_LEN (sizeof(SEND_CMD)+1+1+5+1+DATA_LEN_MAX+1)
static int sal_wifi_send(int fd,
                         uint8_t *data,
                         uint32_t len,
                         char remote_ip[16],
                         int32_t remote_port)
{
    int link_id;
    char cmd[SEND_CMD_LEN] = {0}, out[128] = {0};

    if (!data) return -1;

    LOGD(TAG, "%s on fd %d", __func__, fd);

    link_id = fd_to_linkid(fd);
    if (link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }

    /* AT+CIPSEND=id, */
    snprintf(cmd, SEND_CMD_LEN - 1, "%s=%d,", SEND_CMD, link_id);
    /* [remote_port,] */
    if (remote_port >= 0)
        snprintf(cmd + strlen(cmd), 7, "%d,", remote_port);
    /* data_length */
    snprintf(cmd + strlen(cmd), DATA_LEN_MAX + 1, "%d", len);

    LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at.send_data_2stage((const char *)cmd, (const char *)data, len, out, sizeof(out));
    LOGD(TAG, "The AT response is: %s", out);

    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }

    return 0;
}

#define DOMAIN_RSP "+CIPDOMAIN:"
#define DOMAIN_CMD "AT+CIPDOMAIN"
#define DOMAIN_CMD_LEN (sizeof(DOMAIN_CMD)+MAX_DOMAIN_LEN+1)
/* Return the first IP if multiple found. */
static int sal_wifi_domain_to_ip(char *domain,
                                 char ip[16])
{
    char cmd[DOMAIN_CMD_LEN] = {0}, out[256] = {0}, *head, *end;

    snprintf(cmd, DOMAIN_CMD_LEN - 1, "%s=%s", DOMAIN_CMD, domain);
    LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at.send_raw(cmd, out, sizeof(out));
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }

    /**
     * +CIPDOMAIN:1\r\n
     * 180.97.33.108\r\n
     *
     * OK\r\n
     */
    if ((head = strstr(out, DOMAIN_RSP)) == NULL) {
        LOGE(TAG, "No IP info found in result string.");
        return -1;
    }

    /* Check the format */
    head += strlen(DOMAIN_RSP);
    if (head[0] < '0' && head[0] >= ('0' + LINK_ID_MAX))
        goto err;

    head++;
    if (memcmp(head, at._recv_delimiter, strlen(at._recv_delimiter)) != 0)
        goto err;

   /* We find the IP head */
   head += strlen(at._recv_delimiter);

   end = head;
   while (((end - head) < 15) && (*end != at._recv_delimiter[0])) end++;
   if (((end - head) < 6) || ((end -head) > 15)) goto err;

   /* We find a good IP, save it. */
   memcpy(ip, head, end - head);
   ip[end-head] = '\0';

   return 0;

err:
    LOGD(TAG, "Failed to get IP due to unexpected result string.");
    return -1;
}


static int sal_wifi_close(int fd,
                          int32_t remote_port)
{
    int link_id;
    char cmd[STOP_CMD_LEN] = {0}, out[64];

    link_id = fd_to_linkid(fd);
    if (link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }

    if (aos_sem_new(&g_link[link_id].sem_close, 0) != 0) {
        LOGE(TAG, "failed to allocate semaphore %s", __func__);
        return -1;
    }
 
    snprintf(cmd, STOP_CMD_LEN - 1, "%s=%d", STOP_CMD, link_id);
    LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at.send_raw(cmd, out, sizeof(out));
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }

    if (aos_sem_is_valid(&g_link[link_id].sem_close)) {
        if (aos_sem_wait(&g_link[link_id].sem_close, AOS_WAIT_FOREVER) != 0) {
            LOGE(TAG, "%s sem_wait failed", __func__);
            return -1;
        }
        aos_sem_free(&g_link[link_id].sem_close);
        g_link[link_id].sem_close.hdl = NULL;
        LOGD(TAG, "%s sem_wait succeed.", __func__);
    }

    return 0;
}

static int mk3060_wifi_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb)
        g_netconn_data_input_cb = cb;
    return 0;
}

sal_op_t sal_op = {
    .version = "1.0.0",
    .init = sal_wifi_init,
    .start = sal_wifi_start,
    .send = sal_wifi_send,
    .domain_to_ip = sal_wifi_domain_to_ip,
    .close = sal_wifi_close,
    .deinit = sal_wifi_deinit,
    .register_netconn_data_input_cb = mk3060_wifi_packet_input_cb_register,
};

int mk3060_sal_init(void)
{
    return sal_module_register(&sal_op);
}

