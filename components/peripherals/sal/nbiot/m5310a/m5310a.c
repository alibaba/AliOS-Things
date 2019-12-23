/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <atcmd_config_module.h>

#include <aos/kernel.h>
#include <sal_import.h>
#include <ulog/ulog.h>
#include <stdbool.h>
#include <atparser.h>
#include <aos/yloop.h>
#include <string.h>

/* 0: disable log, 1: emerg, 2: fatal, 3: error, 4: info, 5: debug */
#define M5310A_AT_DEBUG 5

#define TAG "m5310a"

#if M5310A_AT_DEBUG >= 5
#define M5310A_LOGD LOGD
#else
#define M5310A_LOGD
#endif
#define M5310A_LOGI LOGI
#define M5310A_LOGW LOGW
#define M5310A_LOGE LOGE

/* AT commands for m5310a, TODO: put all commands here later */

#define M5310A_AT_CMD_SUCCESS_RSP "OK"
#define M5310A_AT_CMD_FAIL_RSP "ERROR"
#define M5310A_DEFAULT_CMD_LEN 64
#define M5310A_DEFAULT_RSP_LEN 64

#define LINK_ID_MAX 7 /* m5310a support up to 7 sockets at the same time */
#define SEM_WAIT_DURATION_MS 5000
#define SKT_RECV_BUFFER_SIZ 256 /* date length in byte */
/* socket data format: fd(max 1),ip_str(max 15),len(max 10),<data>,remain_len(max 10)\r\n */
#define _RECV_OVERHEAD_SIZ (1+1+15+1+10+1+1+10+2)

typedef enum {
    M5310A_SOCK_TCPCLIENT,
    M5310A_SOCK_UDPCLIENT,
    M5310A_SOCK_UNKNOWN
} m5310a_sock_t;

typedef struct {
    int fd; /*  SAL fd*/
    int at_fd; /* AT fd */
    m5310a_sock_t socktype;
    char remote_ip[16]; /* for UDP use */
    int remote_port; /* for UDP use */
} link_t;

static link_t g_link[LINK_ID_MAX]; /* used for AT-fd and SAL-fd mapping */
static aos_mutex_t g_link_mutex; /* used for g_link access protect */
static aos_mutex_t g_dns_mutex; /* used for DNS AT cmd protect */
static aos_mutex_t g_connect_mutex; /* used for socket connect AT cmd protect */
static aos_sem_t g_sem_dns; /* used for DNS operations sync */
static aos_sem_t g_sem_connect; /* used for socket start operations sync */
static char _dns_result[16]; /* used to store the IP string of the DNS result */
static netconn_data_input_cb_t g_netconn_data_input_cb;
int at_dev_fd = -1;
static uart_dev_t uart_dev;
static uint8_t inited = 0;
static char skt_recv_buffer[SKT_RECV_BUFFER_SIZ * 2];

#define M5310A_SEND_AT(cmd,out) \
    do { \
        M5310A_LOGD(TAG, "Sending cmd %s", cmd); \
        at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, \
                           NULL, 0, out, M5310A_DEFAULT_RSP_LEN, NULL); \
        M5310A_LOGD(TAG, "cmd response: %s", out); \
    } while (0)

#define M5310A_SEND_AT_NO_REPLY(cmd,delimeter) \
    do { \
        M5310A_LOGD(TAG, "Sending cmd (no reply) %s", cmd); \
        at_send_no_reply(at_dev_fd, cmd, strlen(cmd), true); \
    } while (0)

static int fd2atfd(int fd)
{
    int link_id, ret = -1;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd == fd) {
            ret = g_link[link_id].at_fd;
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int fd2linkid(int fd)
{
    int link_id, ret = -1;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd == fd) {
            ret = link_id;
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int atfd2linkid(int atfd)
{
    int link_id, ret = -1;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].at_fd == atfd) {
            ret = link_id;
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

static int atfd2fd(int atfd)
{
    int link_id, ret = -1;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].at_fd == atfd) {
            ret = g_link[link_id].fd;
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return ret;
}

#define M5310A_AT_CMD_TEST "AT"
static int m5310a_at_ready_test()
{
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    M5310A_SEND_AT(M5310A_AT_CMD_TEST, rsp);
    if (strstr(rsp, M5310A_AT_CMD_FAIL_RSP) != NULL) {
        M5310A_LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }

    return 0;
}

#define M5310A_AT_CMD_UART_CONFIG "AT+NATSPEED"
static int m5310a_uart_init()
{
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    /*set baudrate 9600, timeout 0, store 0, sync_mode 0, stopbits 1, parity 0, xonoff 0*/
    snprintf(cmd, sizeof(cmd) - 1, "%s=%d,%d,%d,%d,%d,%d,%d",
             M5310A_AT_CMD_UART_CONFIG,
             9600, 0, 0, 0, 1, 0, 0);
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

#define M5310A_AT_CMD_RESET "AT+NRB"
static int m5310a_reset()
{
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};

    snprintf(cmd, sizeof(cmd) - 1, "%s", M5310A_AT_CMD_RESET);

#if 0 /* NRB command really need too long time, so it's really
         not possible to use at_send_wait_reply here. */
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, "REBOOTING") == NULL ||
        strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
#else
    M5310A_SEND_AT_NO_REPLY(cmd, true);
    /* It really takes long long long time for m5310a module to be ready again after reset! */
    aos_msleep(60000);
#endif

    return 0;
}

#define M5310A_AT_CMD_CFUN_SET "AT+CFUN"
static int m5310a_set_func(int value)
{
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    if (value != 0 && value != 1) {
        M5310A_LOGE(TAG, "Invalid functionarity value (%u)", value);
        return -1;
    }

    snprintf(cmd, sizeof(cmd) - 1, "%s=%u", M5310A_AT_CMD_CFUN_SET, value);
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

#define M5310A_AT_CMD_CFUN_QUERY "AT+CFUN?"
static int m5310a_check_func()
{
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    snprintf(cmd, sizeof(cmd) - 1, "%s", M5310A_AT_CMD_CFUN_QUERY);
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL ||
        strstr(rsp, "+CFUN:1") == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

#define M5310A_AT_CMD_ATTACH "AT+CGATT"
static int m5310a_net_attach(int value)
{
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    if (value != 0 && value != 1) {
        M5310A_LOGE(TAG, "Invalid attach value (%u)", value);
        return -1;
    }

    snprintf(cmd, sizeof(cmd) - 1, "%s=%u", M5310A_AT_CMD_ATTACH, value);
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    return 0;
}

#define M5310A_AT_CMD_GET_SIGNAL_STRENGTH "AT+CSQ"
static int m5310a_check_signal_quality()
{
    char *str;
    int rssi, ber;
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    snprintf(cmd, sizeof(cmd) - 1, "%s", M5310A_AT_CMD_GET_SIGNAL_STRENGTH);
    M5310A_SEND_AT(cmd, rsp);
    if ((strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) ||
         ((str = strstr(rsp, "+CSQ:")) == NULL)) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }

    sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
    if (rssi >= 99) {
        M5310A_LOGE(TAG, "Invalid rssi %d", rssi);
        return -1;
    }

    return 0;
}

static uint8_t hexchartohexbyte(char hexc)
{
    if (hexc >= '0' && hexc <= '9') return hexc - '0';
    else if (hexc >= 'a' && hexc <= 'f') return hexc - 'a' + 10;
    else if (hexc >= 'A' && hexc <= 'F') return hexc - 'A' + 10;
    else return 0;
}

static void hexstr2hex_in_field(char *hexstr, int len)
{
    char high, low;
    int cnt = 0;
    uint8_t *hex = (uint8_t *)hexstr;

    if (!hexstr || len <= 0) return;

    while (cnt < (len - 1)) {
        high = *(hexstr + cnt);
        low = *(hexstr + cnt + 1);
        *hex++ = (hexchartohexbyte(high) << 4) | hexchartohexbyte(low);
        cnt += 2;
    }
}

struct at_evt_s {
    int fd;
    int length;
};

#define M5310A_AT_CMD_RECV_SOCKET_DATA "AT+NSORF"

static void do_recv_at(void *arg)
{
    int fd, length;
    struct at_evt_s *evt = (struct at_evt_s *)arg;
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    M5310A_LOGD(TAG, "%S, arg: %p", __func__, arg);

    if (!evt) return;

    fd = evt->fd;
    length = evt->length;

    snprintf(cmd, sizeof(cmd) - 1, "%s=%d,%d",
             M5310A_AT_CMD_RECV_SOCKET_DATA, fd, length);
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n",
                    __func__, __LINE__, rsp);
    }

    aos_free(evt);
}

static void do_recv_at_via_event(input_event_t *event, void *priv_data)
{
    do_recv_at(priv_data);
}

/**
 * Socket data arrives in format:
 *   <fd>,<ip_str>,<port>,<len>,<data>,<remaining_len>
 * in which, fd & port & len & remaining_len is in decimal format,
 * and "data" is in hexstring format.
 *
 * @note Use static memory here so we don't need frequent
 *       mm alloc/free operations.
 *
 * @note The event handler is executed in a single task (usually
 *       it's at_worker) by nature. So we don't need protect
 *       critical resource (e.g. skt_recv_buffer) here.
 */
static void m5310a_socket_data_event_handler(void *arg, char *buf, int buflen)
{
    char c;
    int fd = 0, length = 0;

    M5310A_LOGD(TAG, "%s\n", __func__);

    c = 0;
    while (c != ',') {
        at_read(at_dev_fd, &c, 1);
        if(c >= '0' && c <= '9') {
            fd = fd * 10 + (c - '0');
        }
    }

    c = 0;
    while (c != '\n') {
        at_read(at_dev_fd, &c, 1);
        if(c >= '0' && c <= '9') {
            length = length * 10 + (c - '0');
        }
    }

    M5310A_LOGD(TAG, "Socket data arrived, id: %d, length: %d.", fd, length);

#if 1
    char cmd[M5310A_DEFAULT_CMD_LEN] = {0};
    snprintf(cmd, sizeof(cmd) - 1, "%s=%d,%d",
             M5310A_AT_CMD_RECV_SOCKET_DATA, fd, length);
    M5310A_SEND_AT_NO_REPLY(cmd, true);
#else /* aos_schedule_call seems not working ... */
    struct at_evt_s *evt;
    evt = (struct at_evt_s *)aos_malloc(sizeof(struct at_evt_s));
    if (!evt) {
        M5310A_LOGE(TAG, "%s failed to allocate mm, socket data not recevied");
        return;
    }

    evt->fd = fd;
    evt->length = length;
    aos_schedule_call(do_recv_at, evt);
#endif
}

static void m5310a_socket_data2_event_handler(void *arg, char *buf, int buflen)
{
    char c;
    int fd, atfd, readlen, remainlen, comma_cnt;

    M5310A_LOGD(TAG, "%s\n", __func__);

    /* read atfd */
    at_read(at_dev_fd, &c, 1);
    atfd = c - '0';
    fd = atfd2fd(atfd);
    if (fd < 0) {
        M5310A_LOGE(TAG, "%s %d failed, invalid fd %c\r\n",
                    __func__, __LINE__, c);
        goto err;
    }

    /* Get to length field  */
    comma_cnt = 0;
    while (comma_cnt < 3) {
        at_read(at_dev_fd, &c, 1);
        if (c == ',') {
            comma_cnt++;
        }
    }

    /* Get real read length */
    c = 0;
    readlen = 0;
    at_read(at_dev_fd, &c, 1);
    while (c != ',') {
        if (c > '9' || c < '0') {
            M5310A_LOGE(TAG, "%s %d failed, invalid char in length"
                        "%c\r\n", __func__, __LINE__, c);
            goto err;
        }
        readlen = readlen * 10 + c - '0';
        at_read(at_dev_fd, &c, 1);
    }

    int left = readlen, toread;

    /* We use static mm skt_recv_buffer here to avoid frequent mm alloc */
    while (left > 0) {
        toread = left > SKT_RECV_BUFFER_SIZ ? SKT_RECV_BUFFER_SIZ : left;
        memset(skt_recv_buffer, 0, sizeof(skt_recv_buffer));
        at_read(at_dev_fd, skt_recv_buffer, toread << 1);
        /* Convert hexstring to hex, in field so we don't allocte new mm */
        hexstr2hex_in_field(skt_recv_buffer, toread << 1);
        left -= toread;

        /* Report socket data */
        if (g_netconn_data_input_cb) {
            /* TODO get recv data src ip and port*/
            if (g_netconn_data_input_cb(fd, skt_recv_buffer, toread, NULL, 0)) {
                M5310A_LOGE(TAG, " %s socket %d get data len %d fail to post"
                            " to sal, drop it\n", __func__, fd, toread);
            }
        }

        M5310A_LOGD(TAG, "%s socket data on link %d with length %d posted to sal\n",
                    __func__, fd, toread);
    }

    /* Check the comma after data */
    at_read(at_dev_fd, &c, 1);
    if (c != ',') {
        M5310A_LOGE(TAG, "%s %d failed, invalid char after data %c\r\n",
                    __func__, __LINE__, c);
        goto err;
    }

    /* Get remainlen */
    remainlen = 0;
    at_read(at_dev_fd, &c, 1);
    while (c >= '0' && c <= '9') {
        remainlen = remainlen * 10 + c - '0';
        at_read(at_dev_fd, &c, 1);
    }

    /* Eat '\n' */
    at_read(at_dev_fd, &c, 1);

    /* Eat 'OK\r\n' in the next line */
    char data[4];
    at_read(at_dev_fd, data, 4);

    return;

err:
    M5310A_LOGD(TAG, "%s socket data not posted to sal!!!\n",
                __func__, fd, readlen);

    return;
}

/* Just eating the characters for drop event. TODO: check if is the right handling. */
static void m5310a_socket_drop_event_handler(void *arg, char *buf, int buflen)
{
    char c = 0;

    M5310A_LOGD(TAG, "%s", __func__);

    /* Read until '\r' */
    while ((c != '\r') && (c != '\n')) {
        at_read(at_dev_fd, &c, 1);
        M5310A_LOGD(TAG, "%s char %c read\n", __func__, c);
    }

    /* If last char is '\r', need to read '\n' futher */
    if (c == '\r') {
        at_read(at_dev_fd, &c, 1);
        M5310A_LOGD(TAG, "%s char %c read\n", __func__, c);
    }
}

/* The callback function to handle socket connection state */
static void m5310a_socket_connect_event_handler(void *arg, char *buf, int buflen)
{
    char data[5] = {0};

    at_read(at_dev_fd, data, 2);
    if (data[0] != '\r' || data[1] != '\n') {
        M5310A_LOGE(TAG, "Abnormal socket connect event (postfix: %s), ignore it.", data);
        return;
    }

    if (aos_sem_is_valid(&g_sem_connect)) {
        M5310A_LOGD(TAG, "sem is going to be waked up: 0x%x", &g_sem_connect);
        aos_sem_signal(&g_sem_connect); /* wakeup socket start task */
    }

    M5310A_LOGD(TAG, "Socket connected.");
}

static void m5310a_socket_close_event_handler(void *arg, char *buf, int buflen)
{
    M5310A_LOGW(TAG, "Socket close event handler not implemented!");
}

static void m5310a_dns_event_handler(void *arg, char *buf, int buflen)
{
    char c = 0, *p = _dns_result;

    M5310A_LOGD(TAG, "%s DNS event!", __func__);
    memset(_dns_result, 0, sizeof(_dns_result));
    while (c != '\r' && c != '\n') {
        at_read(at_dev_fd, &c, 1);
        if (c == '\r' || c == '\n') break;
        if ((c > '9' || c < '0') && (c != '.')) {
            M5310A_LOGE(TAG, "%s invalid IP address char %c(%02x) received!",
                        __func__, c, c);
            memset(_dns_result, 0 , sizeof(_dns_result));
            return;
        } else {
            *p++ = c;
            M5310A_LOGD(TAG, "%s valid IP address char %c received!",
                        __func__, c);
        }
    }

    if (aos_sem_is_valid(&g_sem_dns)) {
        M5310A_LOGD(TAG, "dns sem is going to be waked up: 0x%x", &g_sem_dns);
        aos_sem_signal(&g_sem_dns); /* wakeup dns wait task */
    }
}

#define M5310A_SOCKET_DATA_EVENT_PREFIX "+NSONMI:"
#define M5310A_SOCKET_DROP_EVENT_PREFIX "+NSODROP:"
#define M5310A_SOCKET_DATA2_EVENT_PREFIX "+NSORF:"
#define M5310A_SOCKET_CLOSE_EVENT_PREFIX "+NSOCLI: "
#define M5310A_SOCKET_CONNECT_EVENT_PREFIX "CONNECT OK"
#define M5310A_DNS_EVENT_PREFIX "+CMDNS:"

/* To implement SAL Init */
static int HAL_SAL_Init(void)
{
    int ret;
    int link_id;

    if (inited) {
        M5310A_LOGW(TAG, "sal component is already initialized");
        return 0;
    }

    if (0 != aos_mutex_new(&g_link_mutex)) {
        M5310A_LOGE(TAG, "Creating link mutex failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    if (0 != aos_mutex_new(&g_dns_mutex)) {
        M5310A_LOGE(TAG, "Creating DNS mutex failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    if (0 != aos_mutex_new(&g_connect_mutex)) {
        M5310A_LOGE(TAG, "Creating connect mutex failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    if (0 != aos_sem_new(&g_sem_connect, 0)) {
        M5310A_LOGE(TAG, "Creating socket start sem failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    if (0 != aos_sem_new(&g_sem_dns, 0)) {
        M5310A_LOGE(TAG, "Creating DNS sem failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    memset(g_link, 0, sizeof(g_link));
    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        g_link[link_id].fd = -1;
        g_link[link_id].at_fd = -1;
        g_link[link_id].socktype = M5310A_SOCK_UNKNOWN;
        /*close all link, TODO */
        /*close all link auto reconnect, TODO */
    }

    at_register_callback(at_dev_fd, M5310A_SOCKET_DATA_EVENT_PREFIX, NULL,
                         NULL, 0, m5310a_socket_data_event_handler, NULL);
    at_register_callback(at_dev_fd, M5310A_SOCKET_DROP_EVENT_PREFIX, NULL,
                         NULL, 0, m5310a_socket_drop_event_handler, NULL);
    at_register_callback(at_dev_fd, M5310A_SOCKET_DATA2_EVENT_PREFIX, NULL,
                         NULL, 0, m5310a_socket_data2_event_handler, NULL);
    at_register_callback(at_dev_fd, M5310A_SOCKET_CONNECT_EVENT_PREFIX, NULL,
                         NULL, 0, m5310a_socket_connect_event_handler, NULL);
    at_register_callback(at_dev_fd, M5310A_SOCKET_CLOSE_EVENT_PREFIX, NULL,
                         NULL, 0, m5310a_socket_close_event_handler, NULL);
    at_register_callback(at_dev_fd, M5310A_DNS_EVENT_PREFIX, NULL,
                         NULL, 0, m5310a_dns_event_handler, NULL);

    /* AT device init related hereafter */

#if 0 /* TODO: not working? */
    ret = m5310a_uart_init();
    if (ret) {
        M5310A_LOGE(TAG, "%s failed to init uart, ret: %d", __func__, ret);
        goto err;
    }

    ret = m5310a_reset();
    if (ret) {
        M5310A_LOGE(TAG, "%s failed to reset AT, ret: %d", __func__, ret);
        goto err;
    }
#endif

    ret = -1;
    while (ret != 0) {
        ret = m5310a_at_ready_test();
        aos_msleep(500);
    }

    ret = m5310a_set_func(1);
    if (ret) {
        M5310A_LOGE(TAG, "%s failed to set functionarity, ret: %d", __func__, ret);
        goto err;
    }

    ret = m5310a_check_func();
    if (ret) {
        M5310A_LOGE(TAG, "%s functionarity check failed, ret: %d", __func__, ret);
        goto err;
    }

    ret = m5310a_net_attach(1);
    if (ret) {
        M5310A_LOGE(TAG, "%s failed to setup network, ret: %d", __func__, ret);
        goto err;
    }

    ret = m5310a_check_signal_quality();
    if (ret) {
        M5310A_LOGE(TAG, "%s signal quality is not expected, ret: %d", __func__, ret);
        goto err;
    }

    inited = 1;

    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);

    return 0;

err:
    /* add your code here */
    return ret;
}

/* To implement SAL Deinit */
static int HAL_SAL_Deinit(void)
{
    if (!inited) {
        return 0;
    }

    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

    if (aos_mutex_is_valid(&g_dns_mutex)) {
        aos_mutex_free(&g_dns_mutex);
    }

    if (aos_mutex_is_valid(&g_connect_mutex)) {
        aos_mutex_free(&g_connect_mutex);
    }

    if (aos_sem_is_valid(&g_sem_connect)) {
        aos_sem_free(&g_sem_connect);
    }

    if (aos_sem_is_valid(&g_sem_dns)) {
        aos_sem_free(&g_sem_dns);
    }

    /* add your code here */
    return 0;
}

#define M5310A_AT_CMD_SOCKET_CREATE "AT+NSOCR"
#define M5310A_AT_CMD_SOCKET_CONNECT "AT+NSOCO"
/* To implement the SAL Start */
static int HAL_SAL_Start(sal_conn_t *conn)
{
    int link_id;
    char cmd[64] = {0};
    char rsp[64] = {0};

    if (!conn || !conn->addr) {
        M5310A_LOGE(TAG, "%s %d - invalid argument", __func__, __LINE__);
        return -1;
    }

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd >= 0) {
            continue;
        } else {
            g_link[link_id].fd = conn->fd;
            break;
        }
    }

    /* The caller should deal with this failure */
    if (link_id >= LINK_ID_MAX) {
        M5310A_LOGI(TAG, "No link available for now, %s failed.", __func__);
        goto err;
    }

    /* Create socket at AT level */
    int protocol, port = conn->r_port;
    char *type, *ip = conn->addr, *p;
    switch (conn->type) {
        case TCP_CLIENT:
            protocol = 6;
            type = "STREAM";
            g_link[link_id].socktype = M5310A_SOCK_TCPCLIENT;
            break;
        case UDP_UNICAST:
            protocol = 17;
            type = "DGRAM";
            g_link[link_id].socktype = M5310A_SOCK_UDPCLIENT;
            break;
        case SSL_CLIENT:
        case TCP_SERVER:
        case UDP_BROADCAST:
            M5310A_LOGW(TAG, "Connection type %d not supported.", conn->type);
            goto err;
        default:
            M5310A_LOGE(TAG, "%s %d invalid c->type %d\r\n", __func__, __LINE__, conn->type);
            goto err;
    }

    /* Send AT command to create the socket */
    snprintf(cmd, sizeof(cmd) - 1, "%s=%s,%d", M5310A_AT_CMD_SOCKET_CREATE, type, protocol);
    if (protocol == 17) {
        snprintf(cmd + strlen(cmd), 7, ",%d", (int)conn->l_port);
    }
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        goto err;
    }

    /* Get the socket AT fd */
    p = rsp;
    while (*p > '9' || *p < '0') p++;
    g_link[link_id].at_fd = *p - '0';

    /* It's time to return now if UDP */
    if (protocol == 17) {
        g_link[link_id].remote_port = conn->r_port;
        strncpy(g_link[link_id].remote_ip, conn->addr, sizeof(g_link[link_id].remote_ip));
        goto success;
    }

    /**
     * Get socket connect lock so as to avoid 2 or more
     * connect AT cmds entangled with each other.
     *
     * @note Be careful of deadlock.
     */
    if (aos_mutex_lock(&g_connect_mutex, AOS_WAIT_FOREVER) != 0) {
        M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        goto err;
    }

    /* Do connect for TCP */
    memset(cmd, 0, sizeof(cmd));
    memset(rsp, 0, sizeof(rsp));
    snprintf(cmd, sizeof(cmd) - 1, "%s=%d,%s,%d",
             M5310A_AT_CMD_SOCKET_CONNECT,
             g_link[link_id].at_fd, ip, port);
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        goto err;
    }

    /* Wait until socket successfully started */
    if (aos_sem_is_valid(&g_sem_connect)) {
        if (aos_sem_wait(&g_sem_connect, SEM_WAIT_DURATION_MS) != 0) {
            M5310A_LOGE(TAG, "Failed to wait sem %s %d", __func__, __LINE__);
            aos_mutex_unlock(&g_connect_mutex);
            goto err;
        }
    }

    aos_mutex_unlock(&g_connect_mutex);

    M5310A_LOGD(TAG, "%s sem_wait succeed.", __func__);

success:
    aos_mutex_unlock(&g_link_mutex);
    return 0;

err:
    g_link[link_id].fd = -1;
    g_link[link_id].at_fd = -1;
    g_link[link_id].socktype = M5310A_SOCK_UNKNOWN;
    aos_mutex_unlock(&g_link_mutex);
    return -1;
}

#define M5310A_AT_CMD_SOCKET_CLOSE "AT+NSOCL"
/* To implement the SAL Close */
static int HAL_SAL_Close(int fd, int32_t remote_port)
{
    int at_fd, link_id;
    char cmd[64] = {0};
    char rsp[64] = {0};

    link_id = fd2linkid(fd);
    if (link_id >= 0) {
        at_fd = g_link[link_id].at_fd;
        snprintf(cmd, sizeof(cmd) - 1, "%s=%u",
                 M5310A_AT_CMD_SOCKET_CLOSE, at_fd);
        M5310A_SEND_AT(cmd, rsp);
        if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
            M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
            return -1;
        }

        if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
            M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
            return -1;
        }

        g_link[link_id].fd = -1;
        g_link[link_id].fd = -1;
        g_link[link_id].socktype = M5310A_SOCK_UNKNOWN;

        aos_mutex_unlock(&g_link_mutex);
    }

    /* add your code here */
    return link_id < 0 ? -1 : 0;
}

#define M5310A_AT_CMD_DNS "AT+CMDNS"
/* To implement the Domain to ip address translation */
static int HAL_SAL_DomainToIp(char *domain, char ip[16])
{
    char cmd[64] = {0};
    char rsp[64] = {0};

    if (!domain || !ip) return -1;

    memset(ip, 0, 16);

    /**
     * Get DNS lock so as to avoid 2 or more DNS
     * AT cmds entangled with each other.
     *
     * @note Be careful of deadlock!
     */
    if (aos_mutex_lock(&g_dns_mutex, AOS_WAIT_FOREVER) != 0) {
        M5310A_LOGE(TAG, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    snprintf(cmd, sizeof(cmd) - 1, "%s=\"%s\"",
             M5310A_AT_CMD_DNS, domain);
    M5310A_SEND_AT(cmd, rsp);
    if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
        M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        goto err;
    }

    /* Wait for DNS result */
    if (aos_sem_wait(&g_sem_dns, SEM_WAIT_DURATION_MS) != 0) {
        M5310A_LOGE(TAG, "%s sem_wait failed", __func__);
        goto err;
    }

    strncpy(ip, _dns_result, 16);
    aos_mutex_unlock(&g_dns_mutex);
    M5310A_LOGD(TAG, "%s sem_wait succeed.", __func__);
    return 0;

err:
    aos_mutex_unlock(&g_dns_mutex);
    return -1;
}

static void hex2hexstr(char *str, uint8_t *hex, int len)
{
    int cnt = 0;
    int8_t high, low;

    if (!str || !hex || (len <= 0)) return;

    while (cnt++ < len) {
        high = (*hex & 0xf0) >> 4;
        low = *hex++ & 0x0f;
        *str++ = (high >= 10) ? (high - 10 + 'A') : (high + '0');
        *str++ = (low >= 10) ? (low - 10 + 'A') : (low + '0');
    }
}

/* AT+NSOSD/NSOST/NSOSTF=<fd>,<len>,<data_hexstring>\r\n\0 (+5 bytes buffer) */
#define GET_SKT_SEND_BUFFER_SIZ(l) (14+(l<<1)+2+1+5)

#define M5310A_AT_CMD_TCP_SEND "AT+NSOSD"
#define M5310A_AT_CMD_UDP_SEND "AT+NSOST"
#define M5310A_AT_CMD_UDPF_SEND "AT+NSOSTF"
/* To implement the data transmission */
static int HAL_SAL_Send(int fd,
                 uint8_t *data,
                 uint32_t len,
                 char remote_ip[16],
                 int32_t remote_port,
                 int32_t timeout)
{
    int atfd, link_id;
    uint32_t sentlen = 0, cmdlen;
    char *cmd = NULL;
    char rsp[M5310A_DEFAULT_RSP_LEN] = {0};

    link_id = fd2linkid(fd);
    if (link_id < 0) {
        M5310A_LOGE(TAG, "%s can't find the AT link", __func__);
        return -1;
    }

    atfd = g_link[link_id].at_fd;

    cmdlen = GET_SKT_SEND_BUFFER_SIZ(len);
    cmd = (char *)aos_calloc(cmdlen, 1);
    if (!cmd) {
        M5310A_LOGE(TAG, "%s failed to alloc mm", __func__);
        return -1;
    }

    if (g_link[link_id].socktype == M5310A_SOCK_UDPCLIENT) { /* UDP send */
        snprintf(cmd, cmdlen - 1, "%s=%d,%s,%d,%d,", M5310A_AT_CMD_UDP_SEND,
                 atfd, g_link[link_id].remote_ip, g_link[link_id].remote_port, len);
        hex2hexstr(cmd + strlen(cmd), data, len);
#if 0 /* Strange! AT+NSOST cmd wants '\n' delimeter, not '\r\n' */
        M5310A_SEND_AT(cmd, rsp);
        if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
            M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
            aos_free(cmd);
            return -1;
        }
#else
        /* This is a trick, anyway it works. */
        snprintf(cmd + strlen(cmd), 1, "%s", "\n");
        M5310A_SEND_AT_NO_REPLY(cmd, false);
#endif
    } else if (g_link[link_id].socktype == M5310A_SOCK_TCPCLIENT) { /* TCP send */
        snprintf(cmd, cmdlen - 1, "%s=%d,%d,", M5310A_AT_CMD_TCP_SEND, atfd, len);
        hex2hexstr(cmd + strlen(cmd), data, len);
        M5310A_SEND_AT(cmd, rsp);
        if (strstr(rsp, M5310A_AT_CMD_SUCCESS_RSP) == NULL) {
            M5310A_LOGE(TAG, "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
            aos_free(cmd);
            return -1;
        }
        /* TODO: check returned length and re-send if necessay? */
    } else {
        /* TODO */
    }

    if (cmd) aos_free(cmd);

    /* add your code here */
    return 0;
}

/* To implement the data reception callback */
static int HAL_SAL_RegisterNetconnDataInputCb(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    }

    /* add your code here */
    return 0;
}
 
/* To implement the driver configuration */
static int m5310a_sal_add_dev(void* data)
{
    at_config_t at_config = { 0 };

    if(data != NULL)
    {
        sal_device_config_t* config = (sal_device_config_t *)data;
        uart_dev.port  = config->uart_dev.port;
        uart_dev.config.baud_rate    = config->uart_dev.config.baud_rate;
        uart_dev.config.data_width   = config->uart_dev.config.data_width;
        uart_dev.config.parity       = config->uart_dev.config.parity;
        uart_dev.config.stop_bits    = config->uart_dev.config.stop_bits;
        uart_dev.config.flow_control = config->uart_dev.config.flow_control;
        uart_dev.config.mode         = config->uart_dev.config.mode;
    }
    else
    {
        M5310A_LOGE(TAG, "Error: Uart dev is not configured! Please"
                    " provide the config in sal_add_dev() call.\n");
        return -1;
    }

    at_init();

    /* configure and add one uart dev */
    at_config.type                             = AT_DEV_UART;
    at_config.port                             = uart_dev.port;
    at_config.dev_cfg                          = &uart_dev;
    at_config.send_delimiter                   = AT_SEND_DELIMITER;
    at_config.reply_cfg.reply_prefix           = AT_RECV_PREFIX;
    at_config.reply_cfg.reply_success_postfix  = AT_RECV_SUCCESS_POSTFIX;
    at_config.reply_cfg.reply_fail_postfix     = AT_RECV_FAIL_POSTFIX;

    if ((at_dev_fd = at_add_dev(&at_config)) < 0) {
        M5310A_LOGE(TAG, "AT parser device add failed!\n");
        return -1;
    }

    /* add your code here */
    return 0;
}

/* Don't modify */
static sal_op_t sal_op = {
    .next = NULL,
    .version = "1.0.0",
    .name = "m5310a",
    .add_dev = m5310a_sal_add_dev,
    .init = HAL_SAL_Init,
    .start = HAL_SAL_Start,
    .send_data = HAL_SAL_Send,
    .domain_to_ip = HAL_SAL_DomainToIp,
    .finish = HAL_SAL_Close,
    .deinit = HAL_SAL_Deinit,
    .register_netconn_data_input_cb = HAL_SAL_RegisterNetconnDataInputCb,
};

/* Generated by aos tools, do NOT modify! */
#ifndef __SAL_DEV_NAME
#error "__SAL_DEV_NAME C mocro not defined, please define it in the SAL device driver."
#endif

int __SAL_DEV_NAME_sal_device_init(void)
{
    return sal_module_register(&sal_op);
}
