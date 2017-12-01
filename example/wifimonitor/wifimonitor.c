/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <aos/aos.h>
#include <aos/network.h>

#include <aos/errno.h>
#include <hal/wifi.h>

#define SERVER_PORT 12345 // subject to change
#define SERVER_IP "255.255.255.255" // subject to change
#define GATEWAY_PORT 12346
#define GATEWAY_IP "10.0.0.2"
#define Method_PostNewProbeReqData "postDeviceData"

#define WIFI_JSON_SIGN "\"type\":\"wifimonitor\","
#define PostMacDataFormat "{"WIFI_JSON_SIGN"\"ap\":\"%02x:%02x:%02x:%02x:%02x:%02x\",\"sta\":\"%02x:%02x:%02x:%02x:%02x:%02x\",\"rssi\":\"%d\", \"timestamp\":\"%lld\"}"

#define post_data_buffer_size (512)
#define MODULE_NAME "wifimonitor"
#ifndef MAC_KV_NAME
#define MAC_KV_NAME "mac_sum"
#endif
#define MAC_HASH_IDX_BYTES 1
#define MAC_HASH_TABLE_SIZE (1<<(8*MAC_HASH_IDX_BYTES))
#ifndef MGMT_PROBE_REQ
#define MGMT_PROBE_REQ  (0x40)
#endif
#define WIFI_RSSI_THRESHHOLD (-60) // subject to change

typedef enum {
    SERVER=0,
    GATEWAY,
    INVALID
}CLOUD_T;

/*
 * low 2 bytes as hash index, store high 4 bytes
 * |--------|
 * |  head1 | -> mac_hash_t node -> node -> null
 * |  head2 | -> node -> null
 * |  head3 | -> null
 * |  ...   |
 * | head256| -> node -> null
 *
 * table size: 4B * 2 ^ 8
 */
struct mac_hash_t {
    struct mac_hash_t * next;
    uint8_t high_mac[6-MAC_HASH_IDX_BYTES];
};

static struct mac_hash_t *g_mac_table[MAC_HASH_TABLE_SIZE] = {NULL};

struct remote_conf {
    char remote_ip[16];
    in_port_t remote_port;
};

struct socket_state {
    uint8_t ready_to_create:1;
    uint8_t ready:1;
    uint8_t gateway_mode:1;
};

struct socket_info {
    struct socket_state s_state;
    int sockfd;
    struct sockaddr_in saddr;
};

static struct wifimonitor_t {
    int mac_sum;
    struct mac_hash_t *mac_table[MAC_HASH_TABLE_SIZE];
    struct remote_conf svr_conf;
    struct remote_conf gtw_conf;
    struct socket_info svr_skt_info;
    struct socket_info gtw_skt_info;
    uint8_t own_mac[6];
    uint8_t started;
} g_wifimonitor = {
    0,
    {NULL},
    {SERVER_IP, SERVER_PORT},
    {GATEWAY_IP, GATEWAY_PORT},
    {},
    {{.ready_to_create = 1, .ready = 0, .gateway_mode = 0}, -1, {0}},
    {0},
    0
};

struct remote_conf * svr_conf_p = &(g_wifimonitor.svr_conf);
struct socket_info * svr_skt_info_p = &(g_wifimonitor.svr_skt_info);
struct remote_conf * gtw_conf_p = &(g_wifimonitor.gtw_conf);
struct socket_info * gtw_skt_info_p = &(g_wifimonitor.gtw_skt_info);

static char post_data_buffer[post_data_buffer_size];

static void do_report(struct socket_info *s, const char *data, int len);
static int create_socket(struct remote_conf *c, struct socket_info *s);
static void wifimonitor_wifi_mgnt_frame_callback(uint8_t *buffer,
  int length, char rssi);
static void clear_socket(struct socket_info *s);
static int get_gateway_mode();

static int get_gateway_mode()
{
    return gtw_skt_info_p->s_state.gateway_mode == 1;
}

static void free_mac_table()
{
    int tbl_idx;
    struct mac_hash_t *h, *tmp;

    for (tbl_idx = 0; tbl_idx < MAC_HASH_TABLE_SIZE; tbl_idx++) {
        h = g_mac_table[tbl_idx];
        while (h != NULL) {
            tmp = h;
            h = h->next;
            aos_free(tmp);
        }
    }

    return;
}

static void start_helper()
{
    LOG("Will start the mac count process now.");
    hal_wlan_register_mgnt_monitor_cb(NULL,
      wifimonitor_wifi_mgnt_frame_callback);
}

static void gateway_deamon()
{
    int n;
    char buff[256];

    LOG("gateway_deamon task created.");

    while (1) {
        socklen_t len;
        n = recvfrom(gtw_skt_info_p->sockfd, buff, sizeof(buff)-1, 0,
          (struct sockaddr *)&(gtw_skt_info_p->saddr), &len);
        if (n > 0) {
            buff[n] = 0;
            LOGD(MODULE_NAME, "%s %u received: %s",
              inet_ntoa(gtw_skt_info_p->saddr.sin_addr),
              ntohs(gtw_skt_info_p->saddr.sin_port),
              buff);
            n = sendto(svr_skt_info_p->sockfd, buff, n, 0,
              (struct sockaddr *)&(gtw_skt_info_p->saddr),
              sizeof(gtw_skt_info_p->saddr));
            if (n < 0) {
                LOG("sendto error %s", __func__);
                break;
            }
        } else {
            LOG("recv error %s", __func__);
            break;
        }
    }

    // Do not want to reach here
    return;
}

#define LISTEN_ADDR "0.0.0.0" // INADDR_ANY
static void gateway_work_init()
{
    int ret;

    if (get_gateway_mode()) { // gateway mode, listen socket
        LOG("Gateway mode, will create listen socket.");
        strncpy(gtw_conf_p->remote_ip, LISTEN_ADDR,
          sizeof(gtw_conf_p->remote_ip));
    } else {
        LOG("None gateway mode, will create send socket.");
    }

    create_socket(gtw_conf_p, gtw_skt_info_p);

    if (get_gateway_mode()) {
        ret = bind(gtw_skt_info_p->sockfd,
          (struct sockaddr *)&(gtw_skt_info_p->saddr),
          sizeof(gtw_skt_info_p->saddr));
        if (ret < 0) {
            close(gtw_skt_info_p->sockfd);
            return;
        }
        aos_task_new("gateway_wifimonitor", gateway_deamon, NULL, 4096);
    }
}

static void start_count_mac()
{
    char ip[16], port[10];
    int ip_len = sizeof(ip), port_len = sizeof(port);
    int ret;

    if ((ret = aos_kv_get("server_ip", (void *)ip, &ip_len)) == 0) {
        memcpy(svr_conf_p->remote_ip, ip, sizeof(svr_conf_p->remote_ip));
        LOGD("wifimonitor", "remote_ip kv will be used: %s", ip);
    }
    if ((ret = aos_kv_get("server_port", (void *)port, &port_len)) == 0) {
        svr_conf_p->remote_port = (in_port_t)atol(port);
        LOGD("wifimonitor", "remote_port kv will be used: %s", port);
    }

    gateway_work_init();

    g_wifimonitor.mac_sum = 0;
    memset(g_mac_table, 0, sizeof(g_mac_table));
    hal_wifi_get_mac_addr(NULL, g_wifimonitor.own_mac);
    aos_post_delayed_action(10000, start_helper, NULL);
    g_wifimonitor.started = 1;
#ifdef ENABLE_LIGHT
    light_init();
#endif
}

static void handle_count_mac_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";

    if (strcmp(rtype, "stop") == 0) {
        aos_cli_printf("Will stop the mac count process\r\n");
        /*
         * We simply register a NULL mgnt frame callback here to stop mac
         * count process. Later we can create a new hal, e.g. named as
         * hal_wlan_get_mgnt_monitor_cb, to save the old mgnt frame callback
         * (if any) so that we can save it. After the mac count process is
         * stopped, we can resume the old mgnt frame callback. A typical
         * example of the old mngt frame callback is the registrar's.
         * <TODO>
         */
        hal_wlan_register_mgnt_monitor_cb(NULL, NULL);
        g_wifimonitor.started = 0;
        clear_socket(gtw_skt_info_p);
        clear_socket(svr_skt_info_p);
        free_mac_table();
        return;
    } else if (strcmp(rtype, "suspend") == 0) {
        aos_cli_printf("Will suspend the mac count process\r\n");
        hal_wlan_register_mgnt_monitor_cb(NULL, NULL);
    } else if (strcmp(rtype, "resume") == 0) {
        aos_cli_printf("Will resume the mac count process\r\n");
        hal_wlan_register_mgnt_monitor_cb(NULL,
          wifimonitor_wifi_mgnt_frame_callback);
    } else {
        start_count_mac();
    }
}

static void handle_set_server_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc != 3) return;
    memset(svr_conf_p->remote_ip, 0, sizeof(svr_conf_p->remote_ip));
    strncpy(svr_conf_p->remote_ip, argv[1], sizeof(svr_conf_p->remote_ip));
    svr_conf_p->remote_port = (in_port_t)atol(argv[2]);
    aos_kv_set("server_ip", (const void *)svr_conf_p->remote_ip,
      sizeof(svr_conf_p->remote_ip), 1);
    aos_kv_set("server_port", (const void *)argv[2], strlen(argv[2]) + 1, 1);
    aos_cli_printf("Server ip/port set: ip - %s, por - %d\r\n",
                   svr_conf_p->remote_ip, svr_conf_p->remote_port);
}

/*
 * This command will register a mgnt frame callback, which will overwrite
 * the registrar callback, to handle wifi probe req frame. And the wifi monitor
 * needs to be in open state.
 */
static struct cli_command count_mac_cmds[] = {
    {
        .name = "count_mac",
        .help = "count_mac [start|stop|suspend|resume]",
        .function = handle_count_mac_cmd
    },
    {
        .name = "set_server",
        .help = "set_server [ip port]",
        .function = handle_set_server_cmd
    }
};

static int compare_high_mac(uint8_t * mac1, uint8_t * mac2, int len)
{
    while (len) {
        if (mac1[len-1] != mac2[len-1]) return 1;
        len--;
    }

    return 0;
}

static inline int check_same_mac_and_add_new(uint8_t * mac)
{
    struct mac_hash_t * node, * tail;
    uint8_t entry = *mac & 0xff;
    struct mac_hash_t * new_n;

    node = g_mac_table[entry];
    tail = g_mac_table[entry];

    while (node != NULL) {
        if (0 == compare_high_mac(node->high_mac,
          mac+MAC_HASH_IDX_BYTES, 6-MAC_HASH_IDX_BYTES)) {
            return 1;
        }
        tail = node;
        node = node->next;
    }

    new_n = (struct mac_hash_t *)aos_malloc(sizeof(struct mac_hash_t));
    if (new_n == NULL) {
        LOGI(MODULE_NAME, "Error: aos_malloc failed");
        return -1;
    }
    new_n->next = NULL;
    memcpy(new_n->high_mac, mac+MAC_HASH_IDX_BYTES, 6-MAC_HASH_IDX_BYTES);
    if (tail == NULL)  g_mac_table[entry] = new_n;
    else tail->next = new_n;

    g_wifimonitor.mac_sum++;
    LOGD(MODULE_NAME, "mac_sum is %d", g_wifimonitor.mac_sum);

    return 0;
}

static void report_probe(const uint8_t *mac, char rssi, CLOUD_T type)
{
    uint8_t *own_mac = g_wifimonitor.own_mac;
    long long timestamp = aos_now_ms();

    if (!mac) return;

    snprintf(post_data_buffer, post_data_buffer_size, PostMacDataFormat,
      own_mac[0], own_mac[1], own_mac[2], own_mac[3], own_mac[4], own_mac[5],
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], rssi, timestamp);
    LOGD(MODULE_NAME, "Start report new found mac. %s", post_data_buffer);
    if (type == SERVER) {
        LOGD(MODULE_NAME, "Report to server.");
        do_report(svr_skt_info_p, post_data_buffer,
          strlen(post_data_buffer));
    } else {
        LOGD(MODULE_NAME, "Report to gateway.");
        do_report(gtw_skt_info_p, post_data_buffer,
          strlen(post_data_buffer));
    }
}

static void wifimonitor_wifi_mgnt_frame_callback
(
    uint8_t *buffer,
    int length,
    char rssi
)
{
    int type = buffer[0];
    uint8_t * mac = &buffer[10]; // SA in the probe req frame

    if (!g_wifimonitor.started) return;

    switch (type) {
    case MGMT_PROBE_REQ:
#ifdef ENABLE_LIGHT
        rssi_light_control(mac, rssi);
#endif
        if (get_gateway_mode())
            report_probe(mac, rssi, SERVER);
        else
            report_probe(mac, rssi, GATEWAY);
        break;
    default:
        break;
    }
}

static int create_socket(struct remote_conf *c, struct socket_info *s)
{
    if (0 == s->s_state.ready_to_create) {
        LOG("Info: network is not ready to create socket.");
        return 1;
    }

    if (1 == s->s_state.ready) {
        LOG("Info: socket is already created.");
        return 0;
    }

    set_errno(0);
    s->sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (s->sockfd < 0) {
        LOG("Error: Creating socket failed (errno: %d).", errno);
        return -1;
    }

    memset(&(s->saddr), 0, sizeof(s->saddr));
    s->saddr.sin_family = AF_INET;
    s->saddr.sin_port = htons(c->remote_port);
    s->saddr.sin_addr.s_addr = inet_addr(c->remote_ip);

    s->s_state.ready = 1;

    return 0;
}

static void do_report(struct socket_info *s, const char *data, int len)
{
    int ret;

    if (0 == s->s_state.ready) {
        LOG("Error: socket is not ready");
        return;
    }

    if (!data) return;

    ret = sendto(s->sockfd, data, len, 0,
      (struct sockaddr *)&(s->saddr), sizeof(s->saddr));
    if (ret < 0) LOG("Error: sendto failed");
    LOGD("wifimonitor", "%s: sendto OK, fd: %d, ip: %08x, port: %d", __func__,
      s->sockfd, s->saddr.sin_addr.s_addr, s->saddr.sin_port);
}

static void clear_socket(struct socket_info *s)
{
    if (s->s_state.ready) close(s->sockfd);
    s->s_state.ready = 0;
}

static void wifimonitor_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI || event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    svr_skt_info_p->s_state.ready_to_create = 1;
    gtw_skt_info_p->s_state.gateway_mode = 1;
    if (gtw_skt_info_p->s_state.ready == 1) {
        gtw_skt_info_p->s_state.ready = 0;
        close(gtw_skt_info_p->sockfd);
        gateway_work_init();
    }
    create_socket(svr_conf_p, svr_skt_info_p);
}

void wifimonitor_start(int automatic)
{
    if (automatic)
        start_count_mac();
    aos_register_event_filter(EV_WIFI, wifimonitor_event_handler, NULL);
    aos_cli_register_commands(&count_mac_cmds[0],
      sizeof(count_mac_cmds) / sizeof(count_mac_cmds[0]));
}
