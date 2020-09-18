/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_btsnoop.c
 * @brief:   vendor btsnoop implement
 * @author:  zhigao.dzg@alibaba-inc.com
 * @date:    2020/03/12
 * @version: 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <posix/sys/socket.h>
#include <fcntl.h>
#include <aos/kernel.h>
#include <aos/cli.h>

#include "tg_btsnoop.h"
#include "tg_bluetooth.h"
#include "tg_bt_mesh.h"
#include "bt_err.h"
#include "bt_hal_os.h"
#include "hci/btsnoop.h"
#include "common/bt_target.h"
#include "common/bt_trace.h"

#if CONFIG_BT_SNOOP_ENABLED
#define TG_BTSNOOP_TASK_STACK_SIZE          (2048*4)
#define TG_BTSNOOP_TASK_PRIO                (BT_TASK_DEFAULT_PRIORITIES - 1)
#define TG_BTSNOOP_TASK_NAME                "BTSNOOP"
#define TG_BTSNOOP_QUEUE_LEN                32

typedef struct _bt_snooop_node {
    void *data;
    int16_t length;
}btsnoop_node;

static int8_t tg_btsnoop_task_enabled = 0;
static int8_t tg_btsnoop_enabled = 0;
static hal_task_t tg_btsnoop_thread;
static hal_queue_t tg_btsnoop_queue;
static int8_t tg_btsnoop_queue_buf[TG_BTSNOOP_QUEUE_LEN * sizeof(btsnoop_node)];

typedef struct header {
    int magic_number;
    int length;
}header_t;

typedef struct _server_t {
    int fd;
}server_t;

typedef struct _client_t {
    int fd;
    uint8_t *pkt_data;
    uint16_t pkt_expect;
    uint16_t pkt_len;
    uint16_t pkt_offset;
}client_t;

static server_t g_tg_btsnoop_server;
static client_t g_tg_btsnoop_client;

extern void btsnoop_net_set_callback(btsnoop_net_callback callback);

static int accept_client(int fd)
{
    struct sockaddr addr;
    socklen_t len;
    int nfd;

    memset(&addr, 0, sizeof(addr));
    len = sizeof(addr);
    if (getsockname(fd, (struct sockaddr *) &addr, &len) < 0) {
        TG_TRACE_ERROR("Failed to get socket name");
        return -1;
    }

    nfd = accept(fd, (struct sockaddr *) &addr, &len);
    if (nfd < 0) {
        TG_TRACE_ERROR("Failed to accept client socket");
        return -1;
    }
    //printf("client accepted:%d\n", nfd);

    return nfd;
}

static int server_accept(server_t *server)
{
    client_t *client = &g_tg_btsnoop_client;

    if (!client)
        return -1;
    memset(client, 0, sizeof(*client));

    client->fd = accept_client(server->fd);
    if (client->fd < 0) {
        TG_TRACE_ERROR("client fd is incorrect\n");
        return -1;
    }

    return 0;
}

static int open_tcp(void)
{
    struct sockaddr_in addr;
    int fd = 0;

    TG_TRACE_WARNING("%s start", __func__);
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        TG_TRACE_ERROR("Failed to open server socket");
        return -1;
    }

    //setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(40001);
    TG_TRACE_WARNING("%s start bind", __func__);
    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        TG_TRACE_ERROR("Failed to bind server socket");
        close(fd);
        return -1;
    }

    TG_TRACE_WARNING("%s start listen", __func__);
    if (listen(fd, 100) < 0) {
        TG_TRACE_ERROR("Failed to listen server socket");
        close(fd);
        return -1;
    }
    TG_TRACE_WARNING("%s end", __func__);

    return fd;
}

static int server_open_tcp(void)
{
    server_t *server;

    TG_TRACE_WARNING("%s", __func__);
    server = &g_tg_btsnoop_server;
    memset(server, 0, sizeof(*server));

    server->fd = open_tcp();
    if (server->fd < 0) {
        return -1;
    }

    return 0;
}

static int server_close_tcp(void)
{
    server_t *server;

    server = &g_tg_btsnoop_server;
    close(server->fd);

    return 0;
}

static int create_tcp_server(void)
{
    TG_TRACE_WARNING("%s", __func__);
    if (server_open_tcp() < 0) {
        TG_TRACE_ERROR("failed to create server socket");
        return -1;
    }

    return 0;
}

static int destroy_tcp_server(void)
{
    return server_close_tcp();
}

void tg_btsnoop_callback(const void* data, size_t length)
{
    btsnoop_node snoop_node;
    int32_t ret;

    if (tg_btsnoop_enabled == 0)
        return;

    if (tg_btsnoop_task_enabled == 0)
        return;

    snoop_node.data = malloc(length);
    if (snoop_node.data == NULL) {
        TG_TRACE_ERROR("%s malloc failed, length = %d", __func__, length);
        return;
    }
    memcpy(snoop_node.data, data, length);
    snoop_node.length = length;

//    TG_TRACE_WARNING("%s send start", __func__);
    ret = hal_os_queue_send(&tg_btsnoop_queue, &snoop_node, sizeof(btsnoop_node));
    if (ret != 0) {
        TG_TRACE_ERROR("%s send failed, length = %d, ret = %d", __func__, length, ret);
        free(snoop_node.data);
    }

    return;
}

void tg_btsnoop_thread_handle(void *pArgs)
{
    btsnoop_node snoop_node;
    int32_t len;
    int ret;
    server_t *srv = &g_tg_btsnoop_server;
    client_t *cli = &g_tg_btsnoop_client;

    TG_TRACE_WARNING("%s", __func__);
    do {
        ret = server_accept(srv);
        if (ret < 0) {
            TG_TRACE_ERROR("fail to accept client connection");
            close(srv->fd);
            return;
        }
    }while(0);

    TG_TRACE_WARNING("%s server accepted", __func__);
    tg_btsnoop_task_enabled = 1;
    while (1) {
        ret = hal_os_queue_recv(&tg_btsnoop_queue, 100, &snoop_node, (unsigned int *)&len);
        if (ret) {
//            TG_TRACE_ERROR("%s recv err: %d", __func__, ret);
            continue;
        }
        
//        TG_TRACE_WARNING("%s recvd: %d", __func__, snoop_node.length);
        send(cli->fd, snoop_node.data, snoop_node.length, 0);

        free(snoop_node.data);
    }
    TG_TRACE_ERROR("%s quit", __func__);
}

static void tg_btsnoop_cmd_handle(bool flag)
{
    int32_t ret;

    TG_TRACE_WARNING("%s: %d", __func__, flag);
    if (flag) {
        if (tg_btsnoop_task_enabled == 0) {
            ret = create_tcp_server();
            if (ret != 0) {
                return;
            }
            ret = hal_os_queue_new(&tg_btsnoop_queue, tg_btsnoop_queue_buf, TG_BTSNOOP_QUEUE_LEN * sizeof(btsnoop_node), sizeof(btsnoop_node));
            if (ret != 0) {
                return;
            }
            hal_os_thread_new(&tg_btsnoop_thread, TG_BTSNOOP_TASK_NAME, tg_btsnoop_thread_handle, NULL, TG_BTSNOOP_TASK_STACK_SIZE, TG_BTSNOOP_TASK_PRIO);
        }

        tg_btsnoop_enabled = 1;
    } else {
        ret = destroy_tcp_server();
        if (ret != 0) {
            return;
        }
        tg_btsnoop_enabled = 0;
    }
}

static void tg_btsnoop_start_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int32_t flag = 0;

    TG_TRACE_WARNING("%s", __func__);
    if (argc < 2)
        return;

    flag = atoi((const char *)argv[1]);
    tg_btsnoop_cmd_handle((bool)flag);
}

static struct cli_command ncmd = {
    .name = "bt_snoop_start",
    .help = "bt_snoop_start",
    .function = tg_btsnoop_start_cmd,
};

static void tg_btsnoop_register_cli(void)
{
    aos_cli_register_command(&ncmd);
}

void tg_btsnoop_init(void)
{
    btsnoop_net_set_callback(tg_btsnoop_callback);
    tg_btsnoop_register_cli();
}

#endif

