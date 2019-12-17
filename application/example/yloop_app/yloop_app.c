/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"
#include "netmgr.h"
#include "network/network.h"

#define TAG "yloop_app"

static bool _network_ready = false;

#define DEFAULT_ACTION_COUNT       5
#define DEFAULT_DEST_IP            "127.0.0.1"
#define DEFAULT_ACTION_INTERVAL_MS 1000
#define DEFAULT_ACTION_DATA_SIZE   10
#define DEFAULT_ACTION_PORT        9999
#define DEFAULT_BUFFER_SIZE        1024
#define MAX_DEST_IP_STR_LEN 15

static bool  m_in_aciton = false;
static char  m_dest_ip[MAX_DEST_IP_STR_LEN + 1] = { 0 };
static int   m_dest_port = DEFAULT_ACTION_PORT;
static int   m_action_count = DEFAULT_ACTION_COUNT;
static int   m_action_intval_ms = DEFAULT_ACTION_INTERVAL_MS;
static int   m_action_data_size = DEFAULT_ACTION_DATA_SIZE;

static void yloop_para_reset(void)
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

static void print_help_message()
{
    LOG("Usage:\r\n"
        "    # yloop [-h] | domain name\r\n"
        "- Note: please make sure network is connected before executing \"yloop\"\r\n"
        "        You can connect network with \"netmgr connect <ssid> <passwd>\"\r\n"
        "- To get help:\r\n"
        "    # yloop -h \r\n"
        "- To udp test \r\n"
        "    # yloop -d <dest_ip> -i <interval> -c <action_count> -s <data_size>\r\n");
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

    read_fd = (int *) malloc(sizeof(int));
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
        aos_cancel_poll_read_fd(*read_fd, yloop_read_fd_action, NULL);
        close(*read_fd);
    }

    aos_free(read_fd);
    read_fd = NULL;
}

static void app_delayed_action(void *arg)
{
    int *fd = (int *)arg;

    if (false == m_in_aciton)
        m_action_count = 0;

    if (m_action_count > 0) {
        yloop_udp_send(*fd);
        m_action_count--;
    }

    LOGI(TAG, "<%s> in non-main loop remain time %d\r\n", aos_task_name(), m_action_count);

    if (m_action_count > 0) {
        aos_post_delayed_action(m_action_intval_ms, app_delayed_action, arg);
    } else {
        aos_loop_exit();
    }
}

static void yloop_proc_task(void *arg)
{
    int ret;
    int read_fd;
    aos_loop_t *loop;

    if (!arg)
        return;

    read_fd = *((int *)arg);
    if (read_fd < 0)
        goto err;

    loop = aos_loop_init();
    if ( !loop )
        goto err;

    /* schedule a delayed action */
    ret = aos_post_delayed_action(m_action_intval_ms, app_delayed_action, arg);
    if (ret < 0) {
        LOGE(TAG, "aos_post_delayed_action fail!\n");
        goto err;
    }

    ret = aos_poll_read_fd(read_fd, yloop_read_fd_action, arg);
    if (ret < 0) {
        LOGE(TAG, "aos_poll_read_fd fail!\n");
        goto err;
    }

    m_in_aciton = true;

    aos_loop_run();
    aos_loop_destroy();

err:
    local_udp_deinit(arg);
    m_in_aciton = false;
}

static void yloop_cmd_exec(void)
{
    void *fd = NULL;

    fd = local_udp_init();
    if (NULL == fd) {
        LOGE(TAG, "local udp init failed!");
        return;
    }

    aos_task_new("yloop_cmd", yloop_proc_task, fd, 4 * 1024);
}

static void yloop_exit_current_action(void)
{
    m_in_aciton = false;
}

static void yloop_handle_cmd(char *buffer, int32_t buf_len, int32_t argc, char **argv)
{
    int i;
    int temp;

    if (m_in_aciton) {
         /* exit current action */
        if ( argc > 1 && strcmp( (char *) argv[1], "-e" ) == 0 ) {
            yloop_exit_current_action();
        } else {
            LOGI(TAG, "Use \"yloop -e\" to exit current action first!");
        }

        return;
    }

    yloop_para_reset();

    for(i = 1; i < argc; i++) {
        if ( strcmp( (char *) argv[i], "-c" ) == 0 ) {
            i++;
            if ((i < argc) && ((temp = atoi(argv[i])) > 0)) {
                m_action_count = temp;
            }
            else {
                LOGE(TAG, "yloop: bad action count ");
                return;
            }
        }
        else if ( strcmp( (char *) argv[i], "-i" ) == 0 ) {
            i++;
            if ((i < argc) && ((temp = atoi(argv[i])) > 0)) {
                m_action_intval_ms = temp;
            }
            else {
                LOGE(TAG, "yloop: bad action interval\n");
                return;
            }
        }
        else if ( strcmp( (char *) argv[i], "-d" ) == 0 ) {
            i++;
            if ((i < argc) && (strlen((char *) argv[i]) <= MAX_DEST_IP_STR_LEN) ) {
                memset(m_dest_ip, 0, sizeof(m_dest_ip));
                strncpy(m_dest_ip, argv[i], sizeof(m_dest_ip));
            }
            else {
                LOGE(TAG, "yloop: bad dest ip\n");
                return;
            }
        }
        else if ( strcmp( (char *) argv[i], "-s" ) == 0 ) {
            i++;
            if((i < argc) && ((temp = atoi(argv[i])) > 0)) {
                m_action_data_size = temp;
            }
            else {
                LOGE(TAG, ("yloop: bad packet data size\n"));
                return;
            }
        }
        else if (argc != i + 1) {
            print_help_message();
            return;
        }
    }

    LOGI(TAG, "Input para: dest %s count %d interval %d  size %d\n", m_dest_ip, m_action_count, m_action_intval_ms, m_action_data_size);
    yloop_cmd_exec();
}

/**
 * To use the yloop test commands, please follow below steps:
 *   - Setup the network with "netmgr" cli:
 *     # netmgr connect <WiFi_AP_ssid> <WiFi_AP_password>
 *   - Start yloop tests with "yloop" cli:
 *     # start action: yloop -d [destIP] -i [inteval] -c [count] -s [data size]
 *     # yloop -e
 */
static struct cli_command yloop_test_commands[] = {
    {"yloop", "yloop example", yloop_handle_cmd},
};

int application_start(int argc, char *argv[])
{
    /* set log level */
    aos_set_log_level(AOS_LL_INFO);

    /* register  test commands */
    aos_cli_register_commands(yloop_test_commands,
        sizeof(yloop_test_commands) / sizeof(struct cli_command));

    /* setup wifi network event handler */
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    /* use netmgr command to setup network connection */
    netmgr_init();
    netmgr_start(false);

     /* enter main loop */
    aos_loop_run();

    return 0;
}
