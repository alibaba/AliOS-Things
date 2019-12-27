/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"
#include "netmgr.h"
#include "network/network.h"

#define TAG "yloop_app"

static bool _network_ready = false;

#define DEFAULT_ACTION_COUNT       1000
#define DEFAULT_DEST_IP            "127.0.0.1"
#define DEFAULT_ACTION_INTERVAL_MS 1000
#define DEFAULT_ACTION_DATA_SIZE   10
#define DEFAULT_ACTION_PORT        9999
#define DEFAULT_BUFFER_SIZE        1024
#define MAX_DEST_IP_STR_LEN        15

static char  m_dest_ip[MAX_DEST_IP_STR_LEN + 1] = { 0 };
static int   m_dest_port = DEFAULT_ACTION_PORT;
static int   m_action_count = DEFAULT_ACTION_COUNT;
static int   m_action_intval_ms = DEFAULT_ACTION_INTERVAL_MS;
static int   m_action_data_size = DEFAULT_ACTION_DATA_SIZE;

static void para_reset(void)
{
    memset(m_dest_ip, 0, sizeof(m_dest_ip));
    strncpy(m_dest_ip, DEFAULT_DEST_IP, sizeof(m_dest_ip));
    m_action_count = DEFAULT_ACTION_COUNT;
    m_action_intval_ms = DEFAULT_ACTION_INTERVAL_MS;
    m_action_data_size = DEFAULT_ACTION_DATA_SIZE;
}

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP){
        return;
    }

    _network_ready = true;

    LOGI(TAG, "Got IP! wifi_event_handler cb called!");
}

static void yloop_read_fd_action(int fd, void *arg)
{
    int ret;
    char *recv_buf = NULL;

    recv_buf = aos_malloc(DEFAULT_BUFFER_SIZE);
    if (!recv_buf) {
        LOGE(TAG, "recv buffer malloc failed!");
        return;
    }
    memset(recv_buf, 0, DEFAULT_BUFFER_SIZE);

    ret = recv(fd, recv_buf, DEFAULT_BUFFER_SIZE, 0);
    LOGI(TAG, "fd %d read cb called! recv len %d", fd, ret);

    aos_free(recv_buf);
    recv_buf = NULL;
}

static int yloop_gen_str(char *buff, unsigned int size)
{
    int i;

    if (!buff || size <= 0)
        return -1;

    for (i = 0; i < size; i++) {
        buff[i] = 'A' + (i % 26);
    }

    return 0;
}

static void yloop_udp_send(int fd)
{
    int ret, len;
    struct sockaddr_in dest_addr;
    char *buf = NULL;

    if (fd < 0)
        return;

    buf = (char *) aos_malloc(m_action_data_size);
    if (!buf) {
        LOGE(TAG, "buffer malloc failed!");
        return;
    }

    len = sizeof(dest_addr);
    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_aton(m_dest_ip, &dest_addr.sin_addr);
    dest_addr.sin_port = htons(m_dest_port);

    yloop_gen_str(buf, m_action_data_size);

    ret = sendto(fd, buf, m_action_data_size, 0, 
                 (struct sockaddr*)&dest_addr, len);
    if (ret != m_action_data_size) {
        LOGE(TAG, "fd %d send failed!", fd);
    } else {
        LOGI(TAG, "fd %d send len %d", fd, ret);
    }

    aos_free(buf);
    buf = NULL;
}

static void *local_udp_init(void)
{
    int ret;
    struct sockaddr_in serv_addr;
    int *read_fd = NULL;

    read_fd = (int *) aos_malloc(sizeof(int));
    if (!read_fd) {
        LOGE(TAG, "malloc fd failed\n");
        return NULL;
    }

    *read_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*read_fd < 0) {
        LOGE(TAG, "create socket failed\n");
        goto err;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(DEFAULT_ACTION_PORT);

    ret = bind(*read_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        LOGE(TAG, "socket bind fail!\n");
        goto err;
    }

    return (void *)read_fd;
err:
    if (read_fd && *read_fd >= 0)
        close(*read_fd);

    aos_free(read_fd);
    read_fd = NULL;

    return NULL;
}

static void local_udp_deinit(void *para)
{
    int *read_fd = (int *) para;

    if (read_fd && *read_fd >= 0) {
        /* yloop interface: unregister a poll event on fd */
        aos_cancel_poll_read_fd(*read_fd, yloop_read_fd_action, NULL);
        close(*read_fd);
    }

    aos_free(read_fd);
    read_fd = NULL;
}

static void app_delayed_action(void *arg)
{
    int *fd = (int *)arg;

    if (m_action_count > 0) {
        yloop_udp_send(*fd);
        m_action_count--;
    }

    LOGI(TAG, "<%s> in non-main loop remain time %d\r\n", aos_task_name(), m_action_count);

    if (m_action_count > 0) {
        /* yloop interface: schedule a delayed action, send data when timer fires */
        aos_post_delayed_action(m_action_intval_ms, app_delayed_action, arg);
    } else {
        local_udp_deinit(arg);
    }
}

static void yloop_proc(void *arg)
{
    int ret;
    int read_fd;

    if (!arg)
        return;

    read_fd = *((int *)arg);
    if (read_fd < 0)
        goto err;

    /* yloop interface: schedule a delayed action, send data when timer fires */
    ret = aos_post_delayed_action(m_action_intval_ms, app_delayed_action, arg);
    if (ret < 0) {
        LOGE(TAG, "aos_post_delayed_action fail!\n");
        goto err;
    }

    /* yloop interface: register a poll event on fd, read data on data arrival. */
    ret = aos_poll_read_fd(read_fd, yloop_read_fd_action, arg);
    if (ret < 0) {
        LOGE(TAG, "aos_poll_read_fd fail!\n");
        goto err;
    }

    return;
err:
    local_udp_deinit(arg);
}

static void udp_loopback_run(void)
{
    void *fd = NULL;

    /* set up parameter */
    para_reset();

    /* initialize udp fd */
    fd = local_udp_init();
    if (NULL == fd) {
        LOGE(TAG, "local udp init failed!");
        return;
    }

    /* call yloop API to set up actions */
    yloop_proc(fd);
}

/*
 *  This example demostrates how to use yloop API to perform periodic action without 'while'.
 *  Specifcially, this example sends/reads UDP packet to/from loopback address in one loop.
 */
int application_start(int argc, char *argv[])
{
    /* set log level */
    aos_set_log_level(AOS_LL_INFO);

    /* setup wifi network event handler */
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    /* use netmgr command to setup network connection */
    netmgr_init();
    netmgr_start(false);

    /* main body */
    udp_loopback_run();

     /* enter main loop */
    aos_loop_run();

    return 0;
}
