/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <hal/wifi.h>

#define NETMGR_LOG(format, ...) \
do {\
    printf("\r\n[netmgrapp]>>>");\
    printf(format, ##__VA_ARGS__);\
    printf("\r\n");\
} while (0)

#define CHANNEL_MAX 13
#define MONITOR_PKT_THRESHOLD 1
#define MNGT_MONITOR_PKT_THRESHOLD 1
#define MAX_SEND_TIMES 15
#define RSP_PKT_THRESHOLD 3
#define MNGT_MONITOR_DURATION (15 * 1000)

static aos_sem_t monitor_sem;
static aos_sem_t mngt_monitor_sem;
static aos_sem_t scan_sem;
static aos_sem_t start_sem;
static aos_sem_t send80211_sem;
static hal_wifi_event_cb_t wifi_events;
static uint8_t ip_ready = 0, ip_lost;
static uint32_t monitor_pkt_cnt = 0;
static uint32_t mngt_monitor_pkt_cnt = 0;
static uint8_t own_mac[6];
static uint8_t rsp_pkt_recv;

static uint8_t test_probe_req_frame[] = {
    0x40, 0x00,  // mgnt type, frame control
    0x00, 0x00,  // duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // DA
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SA, to be replaced with wifi mac
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // BSSID
    0xC0, 0x79,  // seq
    0x00, 0x00,  // broadcast ssid
    0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x8C, 0x92, 0x98, 0xA4,  // supported rates
    0x32, 0x04, 0xB0, 0x48, 0x60, 0x6C,  // extended supported rates
    /*--- alibaba ie field begin ---*/
    0xdd, 0x47, 0xd8, 0x96, 0xe0, 0xaa, 0x01, 0x0c,
    0x78, 0x75, 0x61, 0x6e, 0x73, 0x68, 0x75, 0x5f,
    0x38, 0x32, 0x36, 0x36, 0x00, 0x0b, 0x61, 0x31,
    0x41, 0x7a, 0x6f, 0x53, 0x69, 0x35, 0x54, 0x4d,
    0x63, 0x10, 0xc5, 0x5b, 0x6c, 0xa7, 0x45, 0xa1,
    0x44, 0xec, 0x0a, 0x12, 0x30, 0xba, 0x1d, 0x60,
    0xdc, 0x83, 0x04, 0x00, 0x14, 0xca, 0xd5, 0xf5,
    0x7b, 0x3b, 0x45, 0x0b, 0x44, 0xbf, 0x15, 0xa8,
    0x3a, 0x52, 0x6c, 0xaf, 0xd9, 0x9f, 0xa8, 0x1d,
    0x2c,
    /*--- alibaba ie field end ---*/
    0x3F, 0x84, 0x10, 0x9E  // FCS
};

static void switch_ch_task(void *arg);
static int register_wifi_events();

static void ip_got_event(hal_wifi_module_t *m,
                         hal_wifi_ip_stat_t *pnet,
                         void *arg)
{
    NETMGR_LOG("%s - ip: %s, gw: %s, mask: %s", __func__, pnet->ip, pnet->gate, pnet->mask);
    if (aos_sem_is_valid(&start_sem)) {
        aos_sem_signal(&start_sem);
    }
    ip_ready = 1;
    NETMGR_LOG("WiFi start test succeed.");
}

static void start_wifi(char *ssid, char *pw)
{
    int ret = -1;
    hal_wifi_init_type_t type;

    if (ip_ready) {
        NETMGR_LOG("WiFi already be started, let's suspend it first.");
        hal_wifi_suspend_station(NULL);
        ip_ready = 0;
    }

    if (!ssid || !pw) {
        NETMGR_LOG("%s: invalid argument.", __func__);
        NETMGR_LOG("Starting wifi failed.");
        return;
    }

    if (register_wifi_events() != 0) {
        NETMGR_LOG("%s failed to register wifi events.", __func__);
        return;
    }

    wifi_events.ip_got = ip_got_event;

    ret = aos_sem_new(&start_sem, 0);
    if (0 != ret) {
        NETMGR_LOG("%s failed to allocate sem.", __func__);
        return;
    }

    memset(&type, 0, sizeof(type));
    type.wifi_mode = STATION;
    type.dhcp_mode = DHCP_CLIENT;
    strncpy(type.wifi_ssid, ssid, sizeof(type.wifi_ssid) - 1);
    strncpy(type.wifi_key, pw, sizeof(type.wifi_key) - 1);
    ret = hal_wifi_start(NULL, &type);
    NETMGR_LOG("WiFi start test started.");
    if (0 == ret) {
        NETMGR_LOG("Started wifi (ssid: %s, password: %s').", ssid, pw);
    }

    NETMGR_LOG("WiFi start test completed.");

    aos_sem_wait(&start_sem, 60000);
    wifi_events.ip_got = NULL;
    aos_sem_free(&start_sem);
}

static void scan_completed_event(hal_wifi_module_t *m,
                                 hal_wifi_scan_result_t *result,
                                 void *arg)
{
    int i;

    for (i = 0; i < (result->ap_num); i++) {
        NETMGR_LOG("%s ssid found: %s", __func__, result->ap_list[i].ssid);
    }

    if (aos_sem_is_valid(&scan_sem)) {
        aos_sem_signal(&scan_sem);
    }
    NETMGR_LOG("WiFi scan test succeed.");
}

static void scan_adv_completed_event(hal_wifi_module_t *m,
                                     hal_wifi_scan_result_adv_t *result,
                                     void *arg)
{
    int i;

    for (i = 0; i < (result->ap_num); i++) {
        NETMGR_LOG("%s ssid found: %s %02x:%02x:%02x:%02x:%02x:%02x", \
                   __func__, result->ap_list[i].ssid, \
                   result->ap_list[i].bssid[0], result->ap_list[i].bssid[1], result->ap_list[i].bssid[2], \
                   result->ap_list[i].bssid[3], result->ap_list[i].bssid[4], result->ap_list[i].bssid[5]);
    }

    if (aos_sem_is_valid(&scan_sem)) {
        aos_sem_signal(&scan_sem);
    }
    NETMGR_LOG("WiFi adv scan test succeed.");
}

static int register_wifi_events()
{
    hal_wifi_module_t *m;

    m = hal_wifi_get_default_module();
    if (!m) {
        NETMGR_LOG("%s failed: no default wifi module.", __func__);
        return -1;
    }

    /* m->ev_cb is declared as const, can only be assigned once. */
    if (m->ev_cb == NULL) {
        m->ev_cb = &wifi_events;
    }

    return 0;
}

static void start_scan_helper(int adv)
{
    int ret = -1;

    if (register_wifi_events() != 0) {
        NETMGR_LOG("%s failed.", __func__);
        return;
    }

    if (adv == 0) {
        wifi_events.scan_compeleted = scan_completed_event;
    } else if (adv == 1) {
        wifi_events.scan_adv_compeleted = scan_adv_completed_event;
    }

    ret = aos_sem_new(&scan_sem, 0);
    if (0 != ret) {
        NETMGR_LOG("%s failed to allocate sem.", __func__);
        return;
    }

    if (adv == 0) {
        NETMGR_LOG("WiFi scan test started.");
        hal_wifi_start_scan(NULL);
        NETMGR_LOG("WiFi scan test completed.");
    } else if (adv == 1) {
        NETMGR_LOG("WiFi adv scan test started.");
        hal_wifi_start_scan_adv(NULL);
        NETMGR_LOG("WiFi adv scan test completed.");
    }

    aos_sem_wait(&scan_sem, 30000);
    aos_sem_free(&scan_sem);

    if (adv == 0) {
        wifi_events.scan_compeleted = NULL;
    } else if (adv == 1) {
        wifi_events.scan_adv_compeleted = NULL;
    }
}

static void start_scan()
{
    start_scan_helper(0);
}

static void start_scan_adv()
{
    start_scan_helper(1);
}

static int switch_channel_helper(int ch)
{
    int ret = 0;

    ret = hal_wifi_set_channel(NULL, ch);

    if (ret) {
        NETMGR_LOG("Switching wifi channel failed (%d).", ch);
    }

    return ret;
}

static void switch_channel()
{
    int i, fail_cnt = 0;

    NETMGR_LOG("WiFi channel switch test started.");

    for (i = 1; i <= CHANNEL_MAX; i++) {
        fail_cnt += switch_channel_helper(i);
        aos_msleep(100);
    }

    NETMGR_LOG("WiFi channel switch test completed.");

    if (0 != fail_cnt) {
        NETMGR_LOG("WiFi channel switch test failed.");
    } else {
        NETMGR_LOG("WiFi channel switch test succeed.");
    }
}

static inline int is_big_endian(void)
{
    uint32_t data = 0xFF000000;

    if (0xFF == *(uint8_t *)&data) {
        return 1;
    }

    return 0;
}

static inline uint16_t reverse_16bit(uint16_t data)
{
    return (data >> 8) | (data << 8);
}

uint16_t host_to_le16(uint16_t data)
{
    if (is_big_endian()) {
        return reverse_16bit(data);
    }

    return data;
}

typedef struct {
    uint16_t frame_control;
    uint16_t duration_id;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint16_t seq_ctrl;
    uint8_t addr4[6];
} ieee80211_hdr_t;

typedef struct {
    uint8_t *src;
    uint8_t *dst;
    uint8_t *bssid;
    uint8_t channel;
} frame_info_t;

#define IEEE80211_FCTL_TODS             (0x0100)
#define IEEE80211_FCTL_FROMDS           (0x0200)

static inline int ieee80211_has_tods(uint16_t fc)
{
    return (fc & host_to_le16(IEEE80211_FCTL_TODS)) != 0;
}

static inline int ieee80211_has_fromds(uint16_t fc)
{
    return (fc & host_to_le16(IEEE80211_FCTL_FROMDS)) != 0;
}

static inline int ieee80211_has_a4(uint16_t fc)
{
    uint16_t tmp = host_to_le16(IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS);
    return (fc & tmp) == tmp;
}

static inline uint8_t *ieee80211_get_SA(ieee80211_hdr_t *hdr)
{
    if (ieee80211_has_a4(hdr->frame_control)) {
        return hdr->addr4;
    }
    if (ieee80211_has_fromds(hdr->frame_control)) {
        return hdr->addr3;
    }
    return hdr->addr2;
}

static inline uint8_t *ieee80211_get_DA(ieee80211_hdr_t *hdr)
{
    if (ieee80211_has_tods(hdr->frame_control)) {
        return hdr->addr3;
    } else {
        return hdr->addr1;
    }
}

static inline uint8_t *ieee80211_get_BSSID(ieee80211_hdr_t *hdr)
{
    if (ieee80211_has_tods(hdr->frame_control)) {
        if (!ieee80211_has_fromds(hdr->frame_control)) {
            return hdr->addr1;
        } else {
            return NULL;
        }
    } else {
        if (ieee80211_has_fromds(hdr->frame_control)) {
            return hdr->addr2;
        } else {
            return hdr->addr3;
        }
    }
}

static void extract_frame_info(uint8_t *data, int len, frame_info_t *info)
{
    ieee80211_hdr_t *hdr = (ieee80211_hdr_t *)data;

    info->src = ieee80211_get_SA(hdr);
    info->dst = ieee80211_get_DA(hdr);
    info->bssid = ieee80211_get_BSSID(hdr);
}

static void monitor_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    frame_info_t res;

    if (!data || len < 1) {
        NETMGR_LOG("%s failed: invalid argument.", __func__);
        return;
    }

    memset(&res, 0, sizeof(res));
    extract_frame_info(data, len, &res);

    NETMGR_LOG("Frame monitored: "\
               "src (%02x:%02x:%02x:%02x:%02x:%02x), "\
               "dst (%02x:%02x:%02x:%02x:%02x:%02x), "\
               "bssid (%02x:%02x:%02x:%02x:%02x:%02x), "\
               "rssi (%d).", \
               res.src[0], res.src[1], res.src[2], res.src[3], res.src[4], res.src[5], \
               res.dst[0], res.dst[1], res.dst[2], res.dst[3], res.dst[4], res.dst[5], \
               res.bssid[0], res.bssid[1], res.bssid[2], res.bssid[3], res.bssid[4], res.bssid[5], \
               info == NULL ? 0 : info->rssi);

    monitor_pkt_cnt++;
}

#define MGMT_BEACON (0x80)
#define MGMT_PROBE_REQ  (0x40)
#define MGMT_PROBE_RESP (0x50)
static void mngt_monitor_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    int type = data[0];

    switch (type) {
        case MGMT_BEACON:
            NETMGR_LOG(" %s beacon frame.", __func__);
            mngt_monitor_pkt_cnt++;
            break;
        case MGMT_PROBE_REQ:
            NETMGR_LOG(" %s probe req frame.", __func__);
            mngt_monitor_pkt_cnt++;
            break;
        case MGMT_PROBE_RESP:
            NETMGR_LOG(" %s probe resp frame.", __func__);
            mngt_monitor_pkt_cnt++;
            break;
        default:
            NETMGR_LOG("%s non mngt frame (%d): %02x \n", __func__, len, type);
            break;
    }
}

static void data_80211_respsonse_handler(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    int type = data[0];

    switch (type) {
        case MGMT_PROBE_RESP:
            if (memcmp(&data[4], own_mac, sizeof(own_mac)) == 0) {
                NETMGR_LOG(" %s target probe resp frame recv.", __func__);
                if (rsp_pkt_recv == RSP_PKT_THRESHOLD && aos_sem_is_valid(&send80211_sem)) {
                    NETMGR_LOG("WiFi 80211 send test succeed.");
                    aos_sem_signal(&send80211_sem);
                }
                rsp_pkt_recv++;
            }
            break;
        default:
            break;
    }
}

static void switch_ch_task(void *arg)
{
    int ch;

    for (ch = 1; ch <= CHANNEL_MAX; ch++) {
        hal_wifi_set_channel(NULL, ch);
        aos_msleep(1000);
    }

    if (aos_sem_is_valid(&monitor_sem)) {
        aos_sem_signal(&monitor_sem);
    }

    aos_task_exit(0);
}

static void *wifi_stat_chg(hal_wifi_module_t *m, hal_wifi_event_t stat, void *arg)
{
    switch (stat) {
        case NOTIFY_STATION_DOWN:
            ip_ready = 0;
            ip_lost = 1;
            if (aos_sem_is_valid(&mngt_monitor_sem)) {
                aos_sem_signal(&mngt_monitor_sem);
            }
            NETMGR_LOG("%s IP lost.", __func__);
            break;
        default:
            break;
    }
}

static void start_mngt_monitor(char *ssid, char *pw)
{
    int ret;

    if (!ssid || !pw) {
        return;
    }

    /* Management monitor needs be able to work when station connected. */
    if (0 == ip_ready) {
        NETMGR_LOG("%s: let's start wifi first.", __func__);
        start_wifi(ssid, pw);
        if (0 == ip_ready) {
            return;
        }
    }

    ret = aos_sem_new(&mngt_monitor_sem, 0);
    if (0 != ret) {
        NETMGR_LOG("%s failed to allocate sem.", __func__);
        return;
    }

    if (register_wifi_events() != 0) {
        NETMGR_LOG("%s failed to register wifi events.", __func__);
        return;
    }

    wifi_events.stat_chg = wifi_stat_chg;

    mngt_monitor_pkt_cnt = 0;
    ip_lost = 0;
    hal_wlan_register_mgnt_monitor_cb(NULL, mngt_monitor_cb);
    NETMGR_LOG("WiFi management frame monitor test started.");

    aos_sem_wait(&mngt_monitor_sem, 30000);
    aos_sem_free(&mngt_monitor_sem);

    hal_wlan_register_mgnt_monitor_cb(NULL, NULL);
    NETMGR_LOG("WiFi management frame monitor test completed.");

    wifi_events.stat_chg = NULL;

    if (mngt_monitor_pkt_cnt >= MNGT_MONITOR_PKT_THRESHOLD && ip_lost == 0) {
        NETMGR_LOG("WiFi management frame monitor test succeed.");
    } else {
        NETMGR_LOG("WiFi management frame monitor test failed.");
    }
}

static void start_monitor()
{
    int ret;

    ret = aos_sem_new(&monitor_sem, 0);
    if (0 != ret) {
        NETMGR_LOG("%s failed to allocate sem.", __func__);
        return;
    }

    /* Enter monitor state */
    monitor_pkt_cnt = 0;
    hal_wifi_stop_wifi_monitor(NULL);
    hal_wifi_register_monitor_cb(NULL, monitor_cb);
    hal_wifi_start_wifi_monitor(NULL);
    NETMGR_LOG("WiFi monitor test started.");

    /* Monitor data on different channels */
    aos_task_new("switch_ch", switch_ch_task, NULL, 2048);

    /* Wait until all channels monitored */
    aos_sem_wait(&monitor_sem, 30000);
    aos_sem_free(&monitor_sem);

    /* Exit monitor state */
    hal_wifi_stop_wifi_monitor(NULL);
    hal_wifi_register_monitor_cb(NULL, NULL);
    NETMGR_LOG("WiFi monitor test completed.");
    if (monitor_pkt_cnt >= MONITOR_PKT_THRESHOLD) {
        NETMGR_LOG("WiFi monitor test succeed.");
    } else {
        NETMGR_LOG("WiFi monitor test failed.");
    }
}

static void send_helper(void *arg)
{
    static cnt = 0;

    while (cnt < MAX_SEND_TIMES) {
        hal_wlan_send_80211_raw_frame(NULL, test_probe_req_frame,
                                      sizeof(test_probe_req_frame));
        aos_msleep(1000);
    }

    aos_task_exit(0);
}

static void send_80211()
{
    int ret;

    ret = aos_sem_new(&send80211_sem, 0);
    if (0 != ret) {
        NETMGR_LOG("%s failed to allocate sem.", __func__);
        return;
    }

    NETMGR_LOG("WiFi 80211 send test started.");

    hal_wifi_get_mac_addr(NULL, own_mac);
    memcpy(&test_probe_req_frame[10], own_mac, sizeof(own_mac));

    hal_wifi_register_monitor_cb(NULL, data_80211_respsonse_handler);
    hal_wifi_start_wifi_monitor(NULL);

    rsp_pkt_recv = 0;
    aos_task_new("80211_send", send_helper, NULL, 2048);

    aos_task_new("switch_ch", switch_ch_task, NULL, 2048);

    aos_sem_wait(&send80211_sem, 30000);
    aos_sem_free(&send80211_sem);

    hal_wifi_stop_wifi_monitor(NULL);
    hal_wifi_register_monitor_cb(NULL, NULL);

    NETMGR_LOG("WiFi 80211 send test completed.");
}

static void get_mac()
{
    uint8_t mac[6] = {0};

    NETMGR_LOG("WiFi get mac test started.");
    hal_wifi_get_mac_addr(NULL, mac);
    NETMGR_LOG("MAC address: %02x-%02x-%02x-%02x-%02x-%02x",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    NETMGR_LOG("WiFi get mac test completed.");

    if (mac[0] == 0 && mac[1] == 0 && mac[2] == 0 && \
        mac[3] == 0 && mac[4] == 0 && mac[5] == 0) {
        NETMGR_LOG("WiFi get mac test failed.");
    } else {
        NETMGR_LOG("WiFi get mac test succeed.");
    }
}

static void get_ip_stat()
{
    int ret = -1;
    hal_wifi_ip_stat_t pnet;

    if (ip_ready == 0) {
        NETMGR_LOG("IP is not ready, please check if wifi is connected.");
        goto err;
    }

    NETMGR_LOG("WiFi get ip test started.");
    ret = hal_wifi_get_ip_stat(NULL, &pnet, STATION);
    NETMGR_LOG("WiFi get ip test completed.");
    if (ret == 0) {
        NETMGR_LOG("IP: %s, gw: %s, mask: %s", pnet.ip, pnet.gate, pnet.mask);
        NETMGR_LOG("WiFi get ip test succeed.");
        return;
    } else {
        NETMGR_LOG("%s failed.", __func__);
    }

err:
    NETMGR_LOG("WiFi get ip test failed.");
}

static void bulk_test(char *ssid, char *password)
{
    if (!ssid || !password) {
        return;
    }

    NETMGR_LOG("WiFi bulk test started.");
    get_mac();
    start_scan();
    aos_msleep(1000);
    start_scan_adv();
    aos_msleep(1000);
    start_monitor();
    aos_msleep(1000);
    send_80211();
    aos_msleep(1000);
    switch_channel();
    aos_msleep(1000);
    start_wifi(ssid, password);
    aos_msleep(1000);
    start_mngt_monitor(ssid, password);
    aos_msleep(1000);
    get_ip_stat();
    NETMGR_LOG("WiFi bulk test completed.");
}

static void handle_test_wifi_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *ctype = argc > 1 ? argv[1] : "";
    char *ssid, *passwd;

    if (strcmp(ctype, "start") == 0) {
        if (argc != 4) {
            NETMGR_LOG("%s %d: invalid argument.", __func__, __LINE__);
            return;
        }

        ssid = argv[2];
        passwd = argv[3];
        start_wifi(ssid, passwd);
    } else if (strcmp(ctype, "scan") == 0) {
        start_scan();
    } else if (strcmp(ctype, "scan_adv") == 0) {
        start_scan_adv();
    } else if (strcmp(ctype, "get_mac") == 0) {
        get_mac();
    } else if (strcmp(ctype, "ip_info") == 0) {
        get_ip_stat();
    } else if (strcmp(ctype, "monitor") == 0) {
        if (argc == 5 && strcmp(argv[2], "mngt") == 0) {
            ssid = argv[3];
            passwd = argv[4];
            start_mngt_monitor(ssid, passwd);
        } else {
            start_monitor();
        }
    } else if (strcmp(ctype, "80211send") == 0) {
        send_80211();
    } else if (strcmp(ctype, "channel") == 0) {
        switch_channel();
    } else if (strcmp(ctype, "all") == 0) {
        if (argc != 4) {
            NETMGR_LOG("%s %d: invalid argument.", __func__, __LINE__);
            return;
        }

        ssid = argv[2];
        passwd = argv[3];
        bulk_test(ssid, passwd);
    } else {
        NETMGR_LOG("Not supported command - type: %s.", ctype);
    }
}

static void handle_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_cli_printf("Hello world\r\n");
}

static struct cli_command ncmds[] = {
    {
        .name = "test",
        .help = "test",
        .function = handle_test_cmd
    },
    {
        .name = "test_wifi_hal",
        .help = "test_wifi_hal <start <ssid pw>| scan | scan_adv | "
        "monitor [mngt ssid pw] | 80211send | get_mac | ip_info | "
        "channel | all <ssid pw>>",
        .function = handle_test_wifi_cmd
    }
};

int application_start(int argc, char *argv[])
{
    aos_cli_register_commands(&ncmds[0], sizeof(ncmds) / sizeof(ncmds[0]));
    aos_loop_run();
    return 0;
}
