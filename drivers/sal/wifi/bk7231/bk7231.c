/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "network/hal/wifi.h"
#include <atparser.h>
#include <sal_import.h>

#define TAG "sal_wifi"

#define CMD_SUCCESS_RSP "OK"
#define CMD_FAIL_RSP "ERROR"

#define MAX_DATA_LEN   4096
#define MAX_DOMAIN_LEN 256
#define DATA_LEN_MAX 10
#define LINK_ID_MAX 5
#define SEM_WAIT_DURATION 5000

#define STOP_CMD "AT+CIPSTOP"
#define STOP_CMD_LEN (sizeof(STOP_CMD)+1+1+5+1)

#define STOP_AUTOCONN_CMD "AT+CIPAUTOCONN"
#define STOP_AUTOCONN_CMD_LEN (sizeof(STOP_AUTOCONN_CMD)+1+1+5+1)

#define FWVER_CMD "AT+FWVER?\r"
#define FWVER_CMD_LEN (sizeof(FWVER_CMD) - 1)


#define AT_RESET_CMD "AT"

typedef int (*at_data_check_cb_t)(char data);

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

static link_t g_link[LINK_ID_MAX];
static aos_mutex_t g_link_mutex;
static netconn_data_input_cb_t g_netconn_data_input_cb;
static char localipaddr[16];

static void handle_tcp_udp_client_conn_state(uint8_t link_id)
{
    char s[32] = {0};

    at_read(s, 6);
    if (strstr(s, "CLOSED") != NULL) {
        LOGI(TAG, "Server closed event.");
        if (aos_sem_is_valid(&g_link[link_id].sem_close)) {
            LOGD(TAG, "sem is going to be waked up: 0x%x", &g_link[link_id].sem_close);
            aos_sem_signal(&g_link[link_id].sem_close); // wakeup send task
        }
        LOGI(TAG, "Server conn (%d) closed.", link_id);
    } else if (strstr(s, "CONNEC") != NULL) {
        LOGI(TAG, "Server conn (%d) successful.", link_id);
        at_read(s, 3);
        if (aos_sem_is_valid(&g_link[link_id].sem_start)) {
            LOGD(TAG, "sem is going to be waked up: 0x%x", &g_link[link_id].sem_start);
            aos_sem_signal(&g_link[link_id].sem_start); // wakeup send task
        }
    } else if (strstr(s, "DISCON") != NULL) {
        LOGI(TAG, "Server conn (%d) disconnected.", link_id);
        at_read(s, 6);
    } else {
        LOGW(TAG, "No one handle this unkown event!!!");
    }
}

static void handle_client_conn_state()
{

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

static int socket_data_info_get(char *buf, uint32_t buflen, at_data_check_cb_t valuecheck)
{
    uint32_t i = 0;

    if (NULL == buf || 0 == buflen) {
        return -1;
    }

    do {
        at_read(&buf[i], 1);
        if (buf[i] == ',') {
            buf[i] = 0;
            break;
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

static void handle_socket_data()
{
    int link_id = 0;
    int ret = 0;
    int len = 0;
    char reader[16] = {0};
    char *recvdata = NULL;
    char single;

    /* Eat the "OCKET," */
    at_read(reader, 6);
    if (memcmp(reader, "OCKET,", strlen("OCKET,")) != 0) {
        LOGE(TAG, "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x invalid event format!!!\r\n",
             reader[0], reader[1], reader[2], reader[3], reader[4], reader[5]);
        return;
    }

    memset(reader, 0, sizeof(reader));
    ret = socket_data_info_get(reader, 1, &socket_data_len_check);
    if (ret) {
        LOGE(TAG, "Invalid link id 0x%02x !!!\r\n", reader[0]);
        return;
    }
    link_id = reader[0] - '0';

    memset(reader, 0, sizeof(reader));
    /* len */
    ret = socket_data_info_get(reader, sizeof(reader), &socket_data_len_check);
    if (ret) {
        LOGE(TAG, "Invalid datalen %s !!!\r\n", reader);
        return;
    }

    len = atoi(reader);
    if (len > MAX_DATA_LEN) {
        LOGE(TAG, "invalid input socket data len %d \r\n", len);
        return;
    }
    /* Prepare socket data */
    recvdata = (char *)aos_malloc(len);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory, len is %d. \r\n", __func__, __LINE__, len);
        return;
    }

    ret = at_read(recvdata, len);
    if (ret != len) {
        LOGE(TAG, "at read error recv %d want %d!\n", ret, len);
        goto err;
    }

    memset(reader, 0, sizeof(reader));
    at_read(reader, 2);
    if (strncmp(reader, AT_RECV_PREFIX, 2) != 0) {
        LOGE(TAG, "at fail to read delimiter %s after data %s!\n", AT_RECV_PREFIX, reader);
        goto err;
    }

    if (g_netconn_data_input_cb && (g_link[link_id].fd >= 0)) {
        /* TODO get recv data src ip and port*/
        if (g_netconn_data_input_cb(g_link[link_id].fd, recvdata, len, NULL, 0)) {
            LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                 __func__, g_link[link_id].fd, len);
        }
    }

    LOGD(TAG, "%s socket data on link %d with length %d posted to sal\n",
         __func__, link_id, len);

err:
    aos_free(recvdata);
}

static void handle_udp_broadcast_data()
{
    int len = 0;
    uint32_t remoteport = 0;
    int32_t  linkid = 0;
    int32_t  ret = 0;
    char reader[16] = {0};
    char ipaddr[16] = {0};
    char *recvdata = NULL;

    /* Eat the "DP_BROADCAST," */
    at_read(reader, 13);
    if (memcmp(reader, "DP_BROADCAST,", strlen("DP_BROADCAST,")) != 0) {
        LOGE(TAG, "%s invalid event format!!!\r\n",
             reader[0], reader[1], reader[2], reader[3], reader[4], reader[5]);
        return;
    }

    /* get ip addr */
    ret = socket_data_info_get(ipaddr, sizeof(ipaddr), &socket_ip_info_check);
    if (ret) {
        LOGE(TAG, "Invalid ip addr %s !!!\r\n", ipaddr);
        return;
    }
    LOGD(TAG, "get broadcast form ip addr %s \r\n", ipaddr);

    /* get ip port */
    memset(reader, 0, sizeof(reader));
    ret = socket_data_info_get(reader, sizeof(reader), &socket_data_len_check);
    if (ret) {
        LOGE(TAG, "Invalid ip addr %s !!!\r\n", reader);
        return;
    }
    LOGD(TAG, "get broadcast form ip port %s \r\n", reader);
    remoteport = atoi(reader);

    memset(reader, 0, sizeof(reader));
    ret = socket_data_info_get(reader, 1, &socket_data_len_check);
    if (ret) {
        LOGE(TAG, "Invalid link id 0x%02x !!!\r\n", reader[0]);
        return;
    }
    linkid = reader[0] - '0';
    LOGD(TAG, "get udp broadcast linkid %d \r\n", linkid);

    /* len */
    memset(reader, 0, sizeof(reader));
    ret = socket_data_info_get(reader, sizeof(reader), &socket_data_len_check);
    if (ret) {
        LOGE(TAG, "Invalid datalen %s !!!\r\n", reader);
        return;
    }

    len = atoi(reader);
    if (len > MAX_DATA_LEN) {
        LOGE(TAG, "invalid input socket data len %d \r\n", len);
        return;
    }

    /* Prepare socket data */
    recvdata = (char *)aos_malloc(len);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory, len is %d. \r\n", __func__, __LINE__, len);
        return;
    }

    at_read(recvdata, len);

    if (strcmp(ipaddr, localipaddr) != 0) {
        if (g_netconn_data_input_cb && (g_link[linkid].fd >= 0)) {
            if (g_netconn_data_input_cb(g_link[linkid].fd, recvdata, len, ipaddr, remoteport)) {
                LOGE(TAG, " %s socket %d get data len %d fail to post to sal, drop it\n",
                     __func__, g_link[linkid].fd, len);
            }
        }
    } else {
        LOGD(TAG, "drop broadcast packet len %d \r\n", len);
    }
    aos_free(recvdata);

}

static void bk7231_get_local_ip_addr()
{
    int ret = 0;
    hal_wifi_ip_stat_t ip_stat = {0};

    at_wevent_handler(NULL, NULL, 0);

    ret = hal_wifi_get_ip_stat(NULL, &ip_stat, STATION);
    if (ret) {
        LOGE(TAG, "fail to get local ip addr \r\n");
        return ;
    }
    LOGD(TAG, "local ip is %s \r\n", ip_stat.ip);
    strncpy(localipaddr, ip_stat.ip, sizeof(localipaddr));
}
/*
 * Wifi station event handler. include:
 * +WEVENT:AP_UP
 * +WEVENT:AP_DOWN
 * +WEVENT:STATION_UP
 * +WEVENT:STATION_DOWN
 */

static void bk7231wifi_event_handler(void *arg, char *buf, int buflen)
{
    char eventhead[4] = {0};
    char eventotal[16] = {0};

    at_read(eventhead, 3);
    if (strcmp(eventhead, "AP_") == 0) {
        at_read(eventotal, 2);
        if (strcmp(eventotal, "UP") == 0) {

        } else if (strcmp(eventotal, "DO") == 0) {
            /*eat WN*/
            at_read(eventotal, 2);

        } else {
            LOGE(TAG, "!!!Error: wrong WEVENT AP string received. %s\r\n", eventotal);
            return;
        }
    } else if (strcmp(eventhead, "STA") == 0) {
        at_read(eventotal, 7);
        if (strcmp(eventotal, "TION_UP") == 0) {
            aos_loop_schedule_call(NULL, bk7231_get_local_ip_addr, NULL);
        } else if (strcmp(eventotal, "TION_DO") == 0) {
            /*eat WN*/
            at_read(eventotal, 2);
            memset(localipaddr, 0, sizeof(localipaddr));
        } else {
            LOGE(TAG, "!!!Error: wrong WEVENT STATION string received. %s\r\n", eventotal);
            return;
        }
    } else {
        LOGE(TAG, "!!!Error: wrong WEVENT string received. %s\r\n", eventhead);
        return;
    }

    return;
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
 *   8. +CIPEVENT:UDP_BROADCAST,ip,port,id,len,data
 */
static void net_event_handler(void *arg, char *buf, int buflen)
{
    char c;
    char s[32] = {0};
    LOGD(TAG, "%s entry.", __func__);

    at_read(&c, 1);
    if (c >= '0' && c < ('0' + LINK_ID_MAX)) {
        int link_id = c - '0';
        at_read(&c, 1);
        if (c != ',') {
            LOGE(TAG, "!!!Error: wrong CIPEVENT string. 0x%02x\r\n", c);
            return;
        }
        at_read(&c, 1);
        if (c == 'S') {
            LOGD(TAG, "%s server conn state event, linkid: %d.", __func__, link_id);
            /* Eat the "ERVER," */
            at_read(s, 6);
            if (memcmp(s, "ERVER,", strlen("ERVER,")) != 0) {
                LOGE(TAG, "invalid event format 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                     s[0], s[1], s[2], s[3], s[4], s[5]);
                return;
            }
            handle_tcp_udp_client_conn_state(link_id);
        } else if (c == 'U') {
            LOGD(TAG, "%s UDP conn state event.", __func__);
            /* Eat the "DP," */
            at_read(s, 3);
            if (memcmp(s, "DP,", strlen("DP,")) != 0) {
                LOGE(TAG, "%s invalid event format 0x%02x 0x%02x 0x%02x \r\n", __FUNCTION__, s[0], s[1], s[2]);
                return;
            }
            handle_tcp_udp_client_conn_state(link_id);
        } else {
            LOGE(TAG, "!!!Error: wrong CIPEVENT string 0x%02x at line %d\r\n", c, __LINE__);
            return ;
        }
    } else if (c == 'S') {
        LOGD(TAG, "%s socket data event.", __func__);
        handle_socket_data();
    } else if (c == 'C') {
        LOGD(TAG, "%s client conn state event.", __func__);
        handle_client_conn_state();
    } else if (c == 'U') {
        LOGD(TAG, "%s udp broadcast data event.", __func__);
        handle_udp_broadcast_data();
    } else {
        LOGE(TAG, "!!!Error: wrong CIPEVENT string received. 0x%02x\r\n", c);
        return;
    }

    LOGD(TAG, "%s exit.", __func__);
}

// turn off AT echo
static void bk7231_uart_echo_off()
{
    char out[64] = {0};

    at_send_wait_reply(AT_CMD_EHCO_OFF, strlen(AT_CMD_EHCO_OFF), true,
                       NULL, 0, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
    }

    return;
}
static uint8_t inited = 0;

#define NET_OOB_PREFIX "+CIPEVENT:"
#define WIFIEVENT_OOB_PREFIX "+WEVENT:"
int HAL_SAL_Init(void)
{
    int link;
    char cmd[STOP_AUTOCONN_CMD_LEN] = {0};
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
    }

    aos_msleep(2000);
    /*wifi module fw version print */
    snprintf(cmd, FWVER_CMD_LEN, "%s", FWVER_CMD);
    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out, sizeof(out), NULL);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed, out:%s", __func__, __LINE__, out);
    } else {
        if (strstr(out, "FWVER") != NULL) {
            LOGI(TAG, "wifi bk7231 FW version is: %s", strstr(out, "FWVER"));
        } else {
            LOGI(TAG, "%s %d no found any wifi fw version!", __func__, __LINE__);
        }
    }

    bk7231_uart_echo_off();

    at_register_callback(NET_OOB_PREFIX, NULL, 0, net_event_handler, NULL);
    at_register_callback(WIFIEVENT_OOB_PREFIX, NULL, 0, bk7231wifi_event_handler, NULL);
    inited = 1;

    return 0;
}

int HAL_SAL_Deinit(void)
{
    if (!inited) {
        return 0;
    }

    aos_mutex_free(&g_link_mutex);

    return 0;
}

#define START_CMD "AT+CIPSTART"
#define START_CMD_LEN (sizeof(START_CMD)+1+1+13+1+MAX_DOMAIN_LEN+1+5+1+5+1)
static char *start_cmd_type_str[] = {"tcp_server", "tcp_client", \
                                     "ssl_client", "udp_broadcast", "udp_unicast"
                                    };

int HAL_SAL_Start(sal_conn_t *c)
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
        if (g_link[link_id].fd >= 0) {
            continue;
        } else {
            g_link[link_id].fd = c->fd;
            if (aos_sem_new(&g_link[link_id].sem_start, 0) != 0) {
                LOGE(TAG, "failed to allocate semaphore %s", __func__);
                g_link[link_id].fd = -1;
                return -1;
            }

            if (aos_sem_new(&g_link[link_id].sem_close, 0) != 0) {
                LOGE(TAG, "failed to allocate semaphore %s", __func__);
                aos_sem_free(&g_link[link_id].sem_start);
                g_link[link_id].fd = -1;
                return -1;
            }
            break;
        }
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
                     START_CMD, link_id, start_cmd_type_str[c->type], (int)c->l_port);
            break;
        case TCP_CLIENT:
        case SSL_CLIENT:
            snprintf(cmd, START_CMD_LEN - 5 - 1, "%s=%d,%s,%s,%d",
                     START_CMD, link_id, start_cmd_type_str[c->type],
                     c->addr, (int)c->r_port);
            if (c->l_port >= 0) {
                snprintf(cmd + strlen(cmd), 7, ",%d", (int)c->l_port);
            }
            break;
        case UDP_BROADCAST:
        case UDP_UNICAST:
            snprintf(cmd, START_CMD_LEN - 1, "%s=%d,%s,%s,%d,%d",
                     START_CMD, link_id, start_cmd_type_str[c->type],
                     c->addr, (int)c->r_port, (int)c->l_port);
            break;
        default:
            LOGE(TAG, "Invalid connection type.");
            goto err;
    }

    LOGD(TAG, "\r\n%s %d - AT cmd to run: %s \r\n", __func__, __LINE__, cmd);

    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        goto err;
    }

    if (aos_sem_wait(&g_link[link_id].sem_start, SEM_WAIT_DURATION) != 0) {
        LOGE(TAG, "%s sem_wait failed", __func__);
        goto err;
    }

    LOGD(TAG, "%s sem_wait succeed.", __func__);

    return 0;
err:
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    if (aos_sem_is_valid(&g_link[link_id].sem_start)) {
        aos_sem_free(&g_link[link_id].sem_start);
    }

    if (aos_sem_is_valid(&g_link[link_id].sem_close)) {
        aos_sem_free(&g_link[link_id].sem_close);
    }
    g_link[link_id].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

static int fd_to_linkid(int fd)
{
    int link_id;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }
    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

#define SEND_CMD "AT+CIPSEND"
#define SEND_CMD_LEN (sizeof(SEND_CMD)+1+1+5+1+DATA_LEN_MAX+1)
int HAL_SAL_Send(int fd,
                 uint8_t *data,
                 uint32_t len,
                 char remote_ip[16],
                 int32_t remote_port,
                 int32_t timeout)
{
    int link_id;
    char cmd[SEND_CMD_LEN] = {0}, out[128] = {0};

    if (!data) {
        return -1;
    }

    LOGD(TAG, "%s on fd %d", __func__, fd);

    link_id = fd_to_linkid(fd);
    if (link_id < 0 || link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }

    /* AT+CIPSEND=id, */
    snprintf(cmd, SEND_CMD_LEN - 1, "%s=%d,", SEND_CMD, link_id);
    /* [remote_port,] */
    if (remote_port >= 0) {
        snprintf(cmd + strlen(cmd), 7, "%d,", (int)remote_port);
    }

    /* data_length */
#if AT_CHECK_SUM
    snprintf(cmd + strlen(cmd), DATA_LEN_MAX + 1, "%d", (int)len + 1);
#else
    snprintf(cmd + strlen(cmd), DATA_LEN_MAX + 1, "%d", (int)len);
#endif

    LOGD(TAG, "\r\n%s %d - AT cmd to run: %s\r\n", __func__, __LINE__, cmd);
#if AT_CHECK_SUM
    uint8_t checksum = 0;
    uint8_t* outdata = NULL;

    if ((outdata = (uint8_t *)aos_malloc(len + 1)) == NULL) {
        LOGE(TAG, "%s malloc failed!", __func__);
        return -1;
    }

    for (int i = 0; i < len; i++) {
       outdata[i] = data[i];
       checksum += data[i];
    }
    outdata[len] = checksum;

    at_send_wait_reply((const char *)cmd, strlen(cmd), true, (const char *)outdata, len + 1, out, sizeof(out)
                               NULL);
    aos_free(outdata);
#else
    at_send_wait_reply((const char *)cmd, strlen(cmd), true, (const char *)data, len, out, sizeof(out),
                               NULL);
#endif
    LOGD(TAG, "\r\nThe AT response is: %s\r\n", out);

    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGD(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }

    return 0;
}

#define DOMAIN_RSP "+CIPDOMAIN:"
#define DOMAIN_CMD "AT+CIPDOMAIN"
#define DOMAIN_CMD_LEN (sizeof(DOMAIN_CMD)+MAX_DOMAIN_LEN+1)
/* Return the first IP if multiple found. */
int HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    char cmd[DOMAIN_CMD_LEN] = {0}, out[256] = {0}, *head, *end;

    snprintf(cmd, DOMAIN_CMD_LEN - 1, "%s=%s", DOMAIN_CMD, domain);
    LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, AT_RECV_SUCCESS_POSTFIX) == NULL) {
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
        LOGE(TAG, "No IP info found in result string %s \r\n.", out);
        return -1;
    }

    /* Check the format */
    head += strlen(DOMAIN_RSP);
    if (head[0] < '0' && head[0] >= ('0' + LINK_ID_MAX)) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        goto err;
    }


    head++;
    if (memcmp(head, AT_RECV_PREFIX, strlen(AT_RECV_PREFIX)) != 0) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        goto err;
    }

    /* We find the IP head */
    head += strlen(AT_RECV_PREFIX);

    end = head;
    while (((end - head) < 15) && (*end != '\r')) {
        end++;
    }
    if (((end - head) < 6) || ((end - head) > 15)) {
        goto err;
    }

    /* We find a good IP, save it. */
    memcpy(ip, head, end - head);
    ip[end - head] = '\0';
    LOGD(TAG, "get domain %s ip %s \r\n", domain, ip);
    return 0;

err:
    LOGE(TAG, "Failed to get IP due to unexpected result string %s \r\n.", out);
    return -1;
}

int HAL_SAL_Close(int fd,
                  int32_t remote_port)
{
    int link_id;
    char cmd[STOP_CMD_LEN] = {0}, out[64];

    link_id = fd_to_linkid(fd);
    if (link_id < 0 || link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }

    snprintf(cmd, STOP_CMD_LEN - 1, "%s=%d", STOP_CMD, link_id);
    LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        goto err;
    }

    if (aos_sem_wait(&g_link[link_id].sem_close, SEM_WAIT_DURATION) != 0) {
        LOGE(TAG, "%s sem_wait failed", __func__);
        goto err;
    }

    LOGD(TAG, "%s sem_wait succeed.", __func__);
err:
    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    if (aos_sem_is_valid(&g_link[link_id].sem_start)) {
        aos_sem_free(&g_link[link_id].sem_start);
    }

    if (aos_sem_is_valid(&g_link[link_id].sem_close)) {
        aos_sem_free(&g_link[link_id].sem_close);
    }
    g_link[link_id].fd = -1;
    aos_mutex_unlock(&g_link_mutex);
    return -1;

}

int HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }
    return 0;
}

int sal_device_init(void)
{
    at_init();
    return 0;
}

