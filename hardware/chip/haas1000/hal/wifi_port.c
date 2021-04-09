/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drivers/u_ld.h>
#include <vfsdev/wifi_dev.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <aos/hal/wifi.h>
#if LWIP_ETHERNETIF && !LWIP_SUPPORT
#include <lwip/def.h>
#include <lwip/netdb.h>
#endif
#include "aos/vfs.h"
#include "fcntl.h"
#include "plat_types.h"
#include "bwifi_interface.h"
#include "bes_sniffer.h"

typedef enum {
    SCAN_NORMAL,
    SCAN_SPECIFIED
} scan_type_t;

static struct {
    uint8_t wifi_started:1;
    uint8_t resetting:1;
    uint8_t sta_connected:1;
    uint8_t sta_got_ip:1;
    uint8_t sap_started:1;
    uint8_t sap_connected:1;
    uint8_t smart_config:1;
    uint8_t zero_config:1;

    struct {
        int ap_num;
        ap_list_t ap_list[3];
    } scan_req;

    struct {
        uint8_t bssid[6];
        wifi_ip_stat_t ip_stat;
    } network;

    netdev_t* dev;

} wifi_status;

static monitor_data_cb_t promisc_cb;
static monitor_data_cb_t mgnt_frame_cb;

struct netif *wifi_get_netif(netdev_t *dev, wifi_mode_t mode)
{
#if LWIP_ETHERNETIF && !LWIP_SUPPORT
    if (mode == WIFI_MODE_STA)
        return bwifi_get_netif(WIFI_IF_STATION);
    else if (mode == WIFI_MODE_AP)
        return bwifi_get_netif(WIFI_IF_SOFTAP);
#endif
    return NULL;
}

static void on_wifi_connect(WIFI_USER_EVT_ID event_id, void *arg)
{
    printf("%s event_id:%d\n", __func__, event_id);

    if (event_id == WIFI_USER_EVT_CONN_INTER_STATE) {
        BWIFI_CONNECT_INTER_STATE state = *(BWIFI_CONNECT_INTER_STATE *)arg;
        int event;

        switch (state) {
        case INTER_STATE_AUTHENTICATING:
            event = EVENT_WIFI_AUTHENTICATING;
            break;
        case INTER_STATE_AUTH_REJECT:
            event = EVENT_WIFI_AUTH_REJECT;
            break;
        case INTER_STATE_AUTH_TIMEOUT:
            event = EVENT_WIFI_AUTH_TIMEOUT;
            break;
        case INTER_STATE_ASSOCIATING:
            event = EVENT_WIFI_ASSOCIATING;
            break;
        case INTER_STATE_ASSOC_REJECT:
            event = EVENT_WIFI_ASSOC_REJECT;
            break;
        case INTER_STATE_ASSOC_TIMEOUT:
            event = EVENT_WIFI_ASSOC_TIMEOUT;
            break;
        case INTER_STATE_ASSOCIATED:
            event = EVENT_WIFI_ASSOCIATED;
            break;
        case INTER_STATE_4WAY_HANDSHAKE:
            event = EVENT_WIFI_4WAY_HANDSHAKE;
            break;
        case INTER_STATE_HANDSHAKE_FAIL:
            event = EVENT_WIFI_HANDSHAKE_FAILED;
            break;
        case INTER_STATE_GROUP_HANDSHAKE:
            event = EVENT_WIFI_GROUP_HANDSHAKE;
            break;
        default:
            /* Unhandled intermediate states */
            return;
        }
#if 0
        if (m->ev_cb && m->ev_cb->stat_chg) {
            m->ev_cb->stat_chg(m, event, NULL);
        }
#endif
        event_publish(event, NULL);
    } else if (event_id == WIFI_USER_EVT_CONNECTED) {
        uint8_t *bssid = (uint8_t *)arg;

        wifi_status.sta_connected = 1;
        memcpy(wifi_status.network.bssid, bssid, 6);
#if 0
        if (m->ev_cb && m->ev_cb->stat_chg) {
            m->ev_cb->stat_chg(m, NOTIFY_WIFI_CONNECTED, NULL);
        }
#endif
        event_publish(EVENT_WIFI_CONNECTED, wifi_get_netif(wifi_status.dev, WIFI_MODE_STA));
#if LWIP_SUPPORT
    } else if (event_id == WIFI_USER_EVT_GOT_IP) {
        struct ip_info *ip = (struct ip_info *)arg;

        wifi_status.sta_got_ip = 1;
        snprintf(wifi_status.network.ip_stat.ip,
                 sizeof(wifi_status.network.ip_stat.ip),
                 "%s", inet_ntoa(ip->ip));
        snprintf(wifi_status.network.ip_stat.mask,
                 sizeof(wifi_status.network.ip_stat.mask),
                 "%s", inet_ntoa(ip->netmask));
        snprintf(wifi_status.network.ip_stat.gate,
                 sizeof(wifi_status.network.ip_stat.gate),
                 "%s", inet_ntoa(ip->gw));
#if 0
        if (m->ev_cb && m->ev_cb->ip_got) {
            m->ev_cb->ip_got(m, &wifi_status.network.ip_stat, NULL);
        }
        if (m->ev_cb && m->ev_cb->para_chg) {
            memcpy(info.bssid, wifi_status.network.bssid, 6);
            m->ev_cb->para_chg(m, &info, NULL, 0, NULL);
        }
#endif
#endif
    }
}

static void on_wifi_disconnect(WIFI_USER_EVT_ID event_id, void *arg)
{
    printf("%s event_id:%d\n", __func__, event_id);
    if (event_id == WIFI_USER_EVT_DISCONNECTED) {
        uint8_t reason = *(uint8_t *)arg;

        wifi_status.sta_connected = 0;
        wifi_status.sta_got_ip = 0;
        printf("wifi disconnected (reason=%d, locally_generated=%d)\n",
               reason & 0x7F, (reason & BIT(7)) >> 7);
#if 0
        if (m->ev_cb && m->ev_cb->stat_chg) {
            m->ev_cb->stat_chg(m, NOTIFY_WIFI_DISCONNECTED, &reason);
        }
#endif
        event_publish(EVENT_WIFI_DISCONNECTED, &reason);
    }
}

static void on_fatal_error(WIFI_USER_EVT_ID event_id, void *arg)
{
    printf("%s event_id:%d\n", __func__, event_id);

    if (event_id == WIFI_USER_EVT_FATAL_ERROR) {
        uint8 rst_flag = ((BWIFI_FATAL_ERROR_RESET_T *)arg)->rst_flag;
        uint16 error = ((BWIFI_FATAL_ERROR_RESET_T *)arg)->error_cause;
        BWIFI_LMAC_STATUS_DUMP_T dump_info =
            ((BWIFI_FATAL_ERROR_RESET_T *)arg)->dump_info;
        char str_buf[300];
        size_t nbytes;
        ssize_t written_bytes;
        const char *pfile = "/data/wifi_reset_reports";
        int fd;

        if (rst_flag == 1) {
            wifi_status.resetting = 1;
            printf("Bottom layer crashed, wifi card will be reset...\n");
#if 0
            if (m->ev_cb && m->ev_cb->fatal_err) {
                m->ev_cb->fatal_err(m, NULL);
            }
#endif
            nbytes = snprintf(str_buf, sizeof(str_buf),
                     "errorCode=0x%04x\ndumpInfo=0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x",
                     error,
                     dump_info.pac_rxc_rx_buf_in_ptr,
                     dump_info.pac_rxc_rx_buf_out_ptr,
                     dump_info.scheduler_events,
                     dump_info.lmac_pc0,
                     dump_info.lmac_pc1,
                     dump_info.lmac_lr,
                     dump_info.lmac_sp,
                     dump_info.pac_ntd_status_peek,
                     dump_info.pac_txc_status,
                     dump_info.QUEUE_0_CONTROL,
                     dump_info.QUEUE_1_CONTROL,
                     dump_info.QUEUE_2_CONTROL,
                     dump_info.QUEUE_3_CONTROL,
                     dump_info.wlan_sw_override_1,
                     dump_info.tsq_in_prog,
                     dump_info.tsq_in_cmpl);

            if (nbytes <= 0) {
                printf("snprintf fail, return %d", nbytes);
                return;
            }

            aos_vfs_init();
            fd = aos_open(pfile, O_CREAT | O_RDWR);
            if (fd < 0) {
                printf("Failed to open file %s (%d)\n", pfile, fd);
                return;
            }
            written_bytes = aos_write(fd, str_buf, nbytes);
            if (written_bytes < 0) {
                printf("Failed to write file %s\n", pfile);
            } else if (written_bytes != nbytes) {
                printf("Incompletely write file %s, nbytes:%d, written_bytes:%d\n",
                       pfile, nbytes, written_bytes);
            }
            aos_close(fd);
        } else if (rst_flag == 2) {
            wifi_status.resetting = 0;
            printf("wifi card is completely reset !!!\n");
#if 0
            if (m->ev_cb && m->ev_cb->fatal_err) {
                m->ev_cb->fatal_err(m, NULL);
            }
#endif
        }
    }
}

static int haas1000_wifi_init(netdev_t *dev)
{
    static int inited;
    int ret;

    if (inited)
        return 0;

    ret = bwifi_init();
    if (ret) {
        printf("bwifi init fail\n");
        return -1;
    }

    /* Register event callbacks */
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_CONN_INTER_STATE, on_wifi_connect);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_CONNECTED, on_wifi_connect);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_GOT_IP, on_wifi_connect);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_DISCONNECTED, on_wifi_disconnect);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_FATAL_ERROR, on_fatal_error);

    inited = 1;
    wifi_status.wifi_started = 1;
    wifi_status.dev = dev;
    printf("wifi init success!!\n");
    return 0;
};

static int haas1000_wifi_deinit(netdev_t *dev)
{
    return 0;
}

static int haas1000_wifi_get_mac_addr(netdev_t *dev, uint8_t *mac)
{
    if (!mac) {printf("%s: invalid argument!\n", __func__); return -1;}
    bwifi_get_own_mac(mac);
    return 0;
};

static int haas1000_wifi_set_mac_addr(netdev_t *dev, const uint8_t *mac)
{
    printf("WiFi HAL %s not implemeted yet!\n", __func__);
    return -1;
};

static void wifi_connect_task(void *arg)
{
    int ret = 0;
    wifi_config_t *init_para = (wifi_config_t*)arg;
    if (!init_para)
        return;
    ret = bwifi_connect_to_ssid(init_para->ssid, init_para->password, 0, 0, NULL);
    if (ret) {
        printf("wifi connect to %s fail:%d\n", init_para->ssid, ret);
#if 0
        if (m->ev_cb && m->ev_cb->connect_fail) {
            m->ev_cb->connect_fail(m, ret, NULL);
        }
#endif
    }
    aos_free(init_para);
    //aos_task_exit(0);
    osThreadExitPub();
}

__SRAMDATA osThreadDef(wifi_connect_task, osPriorityNormal, 1, (4096), "wifi_connect");
static int haas1000_wifi_connect(netdev_t *dev, wifi_config_t* config)
{
    if (!config) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (WARN_ON(!wifi_status.wifi_started || wifi_status.resetting))
        return -1;

    if (config->mode == WIFI_MODE_STA) {
        wifi_config_t *init_config_ptr = aos_malloc(sizeof(wifi_config_t));
        if (!init_config_ptr) {
            printf("Failed to alloc init para for wifi_connect_task\n");
            return -1;
        }
        strcpy(init_config_ptr->ssid, config->ssid);
        strcpy(init_config_ptr->password, config->password);
        //if (aos_task_new("wifi_connect", wifi_connect_task, init_para_ptr, 4096)) {
        if (osThreadCreate(osThread(wifi_connect_task), init_config_ptr) == NULL) {
            printf("Failed to create wifi_connect_task\n");
            aos_free(init_config_ptr);
            return -1;
        }
    } else if (config->mode == WIFI_MODE_AP) {
        /* TODO */
    }

    return 0;
}

static int haas1000_wifi_notify_ip_state2drv(netdev_t *dev,
                       wifi_ip_stat_t *in_net_para,
                       wifi_mode_t mode)
{
#if LWIP_ETHERNETIF && !LWIP_SUPPORT
    struct ip_info ip;

    if (!in_net_para) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (mode == WIFI_MODE_STA&& wifi_status.sta_connected) {
        memcpy(&wifi_status.network.ip_stat, in_net_para, sizeof(wifi_ip_stat_t));
        printf("set ip: %s\n", in_net_para->ip);
        printf("set mask: %s\n", in_net_para->mask);
        printf("set gw: %s\n", in_net_para->gate);
        inet_aton(in_net_para->ip, &ip.ip);
        inet_aton(in_net_para->mask, &ip.netmask);
        inet_aton(in_net_para->gate, &ip.gw);
        return bwifi_set_ip_addr(WIFI_IF_STATION, &ip);
    }
#endif
    return -1;
}

static int get_ip_stat(netdev_t *dev,
                       wifi_ip_stat_t *out_net_para,
                       wifi_mode_t mode)
{
#if LWIP_SUPPORT
    if (!out_net_para) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (mode == WIFI_MODE_STA&& wifi_status.sta_got_ip) {
        memcpy(out_net_para, &wifi_status.network.ip_stat, sizeof(wifi_ip_stat_t));
        return 0;
    }
#endif
    return -1;
}

static int haas1000_wifi_sta_get_link_status(netdev_t *dev, wifi_ap_record_t *out_stat)
{
    if (!out_stat) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (wifi_status.sta_connected) {
        out_stat->link_status = WIFI_STATUS_LINK_UP;
        out_stat->rssi = bwifi_get_current_rssi();
        bwifi_get_current_ssid(out_stat->ssid);
        bwifi_get_current_bssid(out_stat->bssid);
        out_stat->channel = bwifi_get_current_channel();
    } else {
        out_stat->link_status = WIFI_STATUS_LINK_DOWN;
    }

    return 0;
}

static int find_index_by_value(int value, int *array, int size)
{
    int i;

    for( i=0; i<size; i++ ) {
        if( array[i] == value )
            break;
    }
    return i;
}

static int wifi_scan(netdev_t *dev, wifi_scan_config_t* config, bool block, scan_type_t t)
{
    int i, ret;
    struct bwifi_ssid *scan_ssid, *prev_scan_ssid;
    struct bwifi_scan_config scan_config;
    struct bwifi_bss_info *scan_result;
    static wifi_scan_result_t result = {0};
    int index = 0, a_size = 0;
    int scan_chs[] =  {1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13, 0};

    if (WARN_ON(!wifi_status.wifi_started || wifi_status.resetting))
        return -1;

    if (t == SCAN_SPECIFIED) {
        prev_scan_ssid = NULL;
        a_size = sizeof(scan_chs)/sizeof(scan_chs[0]) - 1;
        for (i = 0; i < wifi_status.scan_req.ap_num; i++) {
            index = find_index_by_value(wifi_status.scan_req.ap_list[i].channel, scan_chs, a_size);
            if( index < a_size && index > i) {
                scan_chs[index] = scan_chs[i];
                scan_chs[i] = wifi_status.scan_req.ap_list[i].channel;
            }
            printf("Scan for specific SSID %s\n", wifi_status.scan_req.ap_list[i].ssid);
            scan_ssid = (struct bwifi_ssid *)aos_malloc(sizeof(struct bwifi_ssid));
            if (!scan_ssid) {
                printf("Failed to malloc scan ssid struct\n");
                break;
            }
            memcpy(scan_ssid->ssid,
                   wifi_status.scan_req.ap_list[i].ssid,
                   sizeof(scan_ssid->ssid));
            scan_ssid->next = NULL;
            if (prev_scan_ssid)
                prev_scan_ssid->next = scan_ssid;
            else
                scan_config.ssids = scan_ssid;
            prev_scan_ssid = scan_ssid;
        }
        scan_config.channels = scan_chs;
        ret = bwifi_config_scan(&scan_config);

        scan_ssid = scan_config.ssids;
        while (scan_ssid) {
            prev_scan_ssid = scan_ssid;
            scan_ssid = scan_ssid->next;
            aos_free(prev_scan_ssid);
        }
    } else {
        ret = bwifi_scan();
    }
    if (ret <= 0) {
        printf("wifi scan fail\n");
        return -1;
    }

    scan_result = (struct bwifi_bss_info *)aos_malloc(ret * sizeof(struct bwifi_bss_info));
    if (!scan_result) {
        printf("Failed to malloc scan result buffer\n");
        goto end;
    }
    ret = bwifi_get_scan_result(scan_result, ret);

    result.ap_num = ret;
    if (result.ap_list == NULL) {
        result.ap_list = aos_malloc(ret * sizeof(ap_list_t));
    } else {
        result.ap_list = aos_realloc(result.ap_list, ret * sizeof(ap_list_t));
    }
    if (!result.ap_list) {
        printf("Failed to malloc the returned ap list\n");
        result.ap_num = 0; goto end;
    }

    for (i = 0; i < ret; i++) {
        struct bwifi_bss_info *r = scan_result + i;

        ap_list_t *res = result.ap_list + i;
        memcpy(res->ssid, r->ssid, sizeof(res->ssid));
        res->ap_power = r->rssi;
        memcpy(res->bssid, r->bssid, sizeof(res->bssid));
    }

end:
    event_publish(EVENT_WIFI_SCAN_DONE, &result);

    if (scan_result)
        aos_free(scan_result);

    return 0;
}

static int haas1000_wifi_start_scan(netdev_t *dev, wifi_scan_config_t* config, bool block)
{
    return wifi_scan(dev, config, block, SCAN_NORMAL);
}

static int haas1000_wifi_start_specified_scan(netdev_t *dev, ap_list_t *ap_list, int ap_num)
{
    int i = 0;

    if (ap_num > 3)
        ap_num = 3;
    wifi_status.scan_req.ap_num = ap_num;
    while (i < ap_num) {
        wifi_status.scan_req.ap_list[i] = *(ap_list + i);
        i++;
    }

    int ret = wifi_scan(dev, NULL, false, SCAN_SPECIFIED);

    printf("haas start scan:%d", ret);
    return ret;
}

static int disconnect_station(netdev_t *dev)
{
    if (wifi_status.wifi_started && wifi_status.sta_connected && !wifi_status.resetting)
        return bwifi_disconnect();

    return 0;
}

static int disconnect_soft_ap(netdev_t *dev)
{
    return 0;
}

static int haas1000_wifi_disconnect(netdev_t *dev)
{
    int ret = 0;

    ret = disconnect_station(dev);
    if (wifi_status.sap_started)
        ret += disconnect_soft_ap(dev);
    return ret;
}

static int haas1000_wifi_cancel_connect(netdev_t *dev)
{
    if (wifi_status.wifi_started && !wifi_status.sta_connected && !wifi_status.resetting)
        return bwifi_disconnect();

    return -1;
}

static int haas1000_wifi_set_channel(netdev_t *dev, int ch)
{
    if (WARN_ON(!wifi_status.wifi_started ||
                wifi_status.sta_connected))
        return -1;

    return bes_sniffer_set_channel((u8)ch);
}

static int haas1000_wifi_get_channel(netdev_t *dev, int* ch)
{
    if(ch != NULL) {
       *ch  = bwifi_get_current_channel();
       return 0;
    }
    return -1;
}

/* Wi-Fi Smart Config */

static int wifi_sniffer_handler(unsigned short data_len, void *data)
{
    uint8_t *frame = (uint8_t *)data;

    if (data == NULL) {
        printf("%s ldpc:%d\n", __func__, data_len);
        return 0;
    }

    printf("%s data:%p, len:%d\n", __func__, data, data_len);
#if 0
    printf("	%02x %02x %02x %02x\n",
           frame[0], frame[1], frame[2], frame[3]);
    printf("	%02x %02x %02x %02x %02x %02x\n",
           frame[4], frame[5], frame[6], frame[7], frame[8], frame[9]);
    printf("	%02x %02x %02x %02x %02x %02x\n",
           frame[10], frame[11], frame[12], frame[13], frame[14], frame[15]);
    printf("	%02x %02x %02x %02x %02x %02x\n",
           frame[16], frame[17], frame[18], frame[19], frame[20], frame[21]);
    printf("	%02x %02x\n", frame[22], frame[23]);
#endif

    if (wifi_status.smart_config && promisc_cb)
        promisc_cb(frame, data_len, NULL);
    else if (wifi_status.zero_config && mgnt_frame_cb) {
        uint8_t type = frame[0] & 0xFC;
        if (type == PROBE_REQ) {
            printf("%s: probe request received!\n", __func__);
            mgnt_frame_cb(frame, data_len, NULL);
        }
    }
    return 0;
}

static int haas1000_wifi_start_monitor(netdev_t *dev)
{
    if (WARN_ON(!wifi_status.wifi_started ||
                wifi_status.sta_connected))
        return -1;

    if (bes_sniffer_start(wifi_sniffer_handler) ||
        bes_sniffer_set_filter(0,1,1,0,0,0,0))
        return -2;

    wifi_status.smart_config = 1;
    return 0;
}

static int haas1000_wifi_stop_monitor(netdev_t *dev)
{
    wifi_status.smart_config = 0;
    return bes_sniffer_stop();
}

static void haas1000_wifi_register_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
    promisc_cb = fn;
}

/* Wi-Fi Zero Config */

static int haas1000_wifi_start_mgnt_monitor(netdev_t *dev)
{
    if (WARN_ON(!wifi_status.sta_connected ||
                wifi_status.resetting))
        return -1;

    if (bes_sniffer_start(wifi_sniffer_handler) ||
        bes_sniffer_set_filter(1,0,0,0,0,0,0))
        return -2;

    wifi_status.zero_config = 1;
    return 0;
}

static int haas1000_wifi_stop_mgnt_monitor(netdev_t *dev)
{
    wifi_status.zero_config = 0;
    return bes_sniffer_stop();
}

static void haas1000_wifi_register_mgnt_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
    mgnt_frame_cb = fn;
}

static int haas1000_wifi_send_80211_raw_frame(netdev_t *dev, uint8_t *buf, int len)
{
    if (WARN_ON(!wifi_status.sta_connected ||
                wifi_status.resetting))
        return -1;

    return bes_sniffer_send_mgmt_frame(bwifi_get_current_channel(), buf, len);
}

static int start_ap(netdev_t *dev,
                    const char *ssid,
                    const char *passwd, int interval, int hide)
{
#ifdef __AP_MODE__

    BWIFI_SEC_TYPE_T sec;

    printf("start_ap ssid:%s, pwd:%s, beacon_int:%d, hidden:%d\n", ssid, passwd, interval, hide);

    if (!passwd || !strlen(passwd))
        sec = SECURITY_NONE;
    else
        sec = SECURITY_WPA2;

    if (bwifi_set_softap_config(ssid, 0, hide, sec, passwd)) {
        printf("Softap %s config failed\n", ssid);
        return -1;
    }

    if (bwifi_softap_start()) {
        printf("Softap %s start failed\n", ssid);
        return -2;
    }

    printf("Softap %s start success!!\n", ssid);
    wifi_status.sap_started = 1;
    if (m->ev_cb && m->ev_cb->stat_chg) {
        m->ev_cb->stat_chg(m, NOTIFY_AP_UP, NULL);
    }
#endif
    return 0;
}

static int stop_ap(netdev_t *dev)
{
#ifdef __AP_MODE__

    printf("stop_ap\n");

    bwifi_softap_stop();
    wifi_status.sap_started = 0;
    if (m->ev_cb && m->ev_cb->stat_chg) {
        m->ev_cb->stat_chg(m, NOTIFY_AP_DOWN, NULL);
    }
#endif
    return 0;
}

static wifi_driver_t haas1000_wifi_driver = {

    /** driver info */
    .base.os                    = "rtos",
    .base.type                  = "solo",
    .base.partner               = "AliOS Things Team",
    .base.app_net               = "rtsp+rtp+rtcp",
    .base.project               = "HAAS100",
    .base.cloud                 = "aliyun",

    /** common APIs */
    .init                       = haas1000_wifi_init,
    .deinit                     = haas1000_wifi_deinit,
//    .set_mode                   = haas1000_wifi_set_mode,
//    .get_mode                   = haas1000_wifi_get_mode,

    /** conf APIs */
    .set_mac_addr               = haas1000_wifi_set_mac_addr,
    .get_mac_addr               = haas1000_wifi_get_mac_addr,
//    .set_lpm                    = haas1000_wifi_set_lpm,
//    .get_lpm                    = haas1000_wifi_get_lpm,
    .notify_ip_state2drv        = haas1000_wifi_notify_ip_state2drv,

    /** connection APIs */
    .start_scan                 = haas1000_wifi_start_scan,
    .start_specified_scan       = haas1000_wifi_start_specified_scan,
    .connect                    = haas1000_wifi_connect,
    .cancel_connect             = haas1000_wifi_cancel_connect,
    .disconnect                 = haas1000_wifi_disconnect,
    .sta_get_link_status        = haas1000_wifi_sta_get_link_status,
//    .ap_get_sta_list            = haas1000_wifi_ap_get_sta_list,


    /** promiscuous APIs */
    .start_monitor              = haas1000_wifi_start_monitor,
    .stop_monitor               = haas1000_wifi_stop_monitor,
    .send_80211_raw_frame       = haas1000_wifi_send_80211_raw_frame,
    .set_channel                = haas1000_wifi_set_channel,
    .get_channel                = haas1000_wifi_get_channel,
    .register_monitor_cb        = haas1000_wifi_register_monitor_cb,
    .start_mgnt_monitor         = haas1000_wifi_start_mgnt_monitor,
    .stop_mgnt_monitor          = haas1000_wifi_stop_mgnt_monitor,
    .register_mgnt_monitor_cb   = haas1000_wifi_register_mgnt_monitor_cb,

    .set_smartcfg = NULL,
};

int haas1000_wifi_register(void)
{
    vfs_wifi_dev_register(&haas1000_wifi_driver, 0);
}

VFS_DRIVER_ENTRY(haas1000_wifi_register)

