/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "hal/wifi.h"
#include "inet.h"
#include "aos/vfs.h"
#include "fcntl.h"
#include "plat_types.h"
#include "bwifi_interface.h"
#include "bes_sniffer.h"

typedef enum {
    SCAN_NORMAL,
    SCAN_ADV,
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
        hal_wifi_ip_stat_t ip_stat;
    } network;

} wifi_status;

static monitor_data_cb_t promisc_cb;
static monitor_data_cb_t mgnt_frame_cb;

hal_wifi_module_t aos_wifi_haas1000;

static void on_wifi_connect(WIFI_USER_EVT_ID event_id, void *arg)
{
    hal_wifi_module_t *m = &aos_wifi_haas1000;

    printf("%s event_id:%d\n", __func__, event_id);

    if (event_id == WIFI_USER_EVT_CONN_INTER_STATE) {
        BWIFI_CONNECT_INTER_STATE state = *(BWIFI_CONNECT_INTER_STATE *)arg;
        hal_wifi_event_t event;

        switch (state) {
        case INTER_STATE_AUTHENTICATING:
            event = NOTIFY_WIFI_AUTHENTICATING;
            break;
        case INTER_STATE_AUTH_REJECT:
            event = NOTIFY_WIFI_AUTH_REJECT;
            break;
        case INTER_STATE_AUTH_TIMEOUT:
            event = NOTIFY_WIFI_AUTH_TIMEOUT;
            break;
        case INTER_STATE_ASSOCIATING:
            event = NOTIFY_WIFI_ASSOCIATING;
            break;
        case INTER_STATE_ASSOC_REJECT:
            event = NOTIFY_WIFI_ASSOC_REJECT;
            break;
        case INTER_STATE_ASSOC_TIMEOUT:
            event = NOTIFY_WIFI_ASSOC_TIMEOUT;
            break;
        case INTER_STATE_ASSOCIATED:
            event = NOTIFY_WIFI_ASSOCIATED;
            break;
        case INTER_STATE_4WAY_HANDSHAKE:
            event = NOTIFY_WIFI_4WAY_HANDSHAKE;
            break;
        case INTER_STATE_HANDSHAKE_FAIL:
            event = NOTIFY_WIFI_HANDSHAKE_FAILED;
            break;
        case INTER_STATE_GROUP_HANDSHAKE:
            event = NOTIFY_WIFI_GROUP_HANDSHAKE;
            break;
        default:
            /* Unhandled intermediate states */
            return;
        }
        if (m->ev_cb && m->ev_cb->stat_chg) {
            m->ev_cb->stat_chg(m, event, NULL);
        }
    } else if (event_id == WIFI_USER_EVT_CONNECTED) {
        uint8_t *bssid = (uint8_t *)arg;

        wifi_status.sta_connected = 1;
        memcpy(wifi_status.network.bssid, bssid, 6);
        if (m->ev_cb && m->ev_cb->stat_chg) {
            m->ev_cb->stat_chg(m, NOTIFY_WIFI_CONNECTED, NULL);
        }
#if LWIP_SUPPORT
    } else if (event_id == WIFI_USER_EVT_GOT_IP) {
        struct ip_info *ip = (struct ip_info *)arg;
        hal_wifi_ap_info_adv_t info = {0};

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

        if (m->ev_cb && m->ev_cb->ip_got) {
            m->ev_cb->ip_got(m, &wifi_status.network.ip_stat, NULL);
        }
        if (m->ev_cb && m->ev_cb->para_chg) {
            memcpy(info.bssid, wifi_status.network.bssid, 6);
            m->ev_cb->para_chg(m, &info, NULL, 0, NULL);
        }
#endif
    }
}

static void on_wifi_disconnect(WIFI_USER_EVT_ID event_id, void *arg)
{
    hal_wifi_module_t *m = &aos_wifi_haas1000;

    printf("%s event_id:%d\n", __func__, event_id);
    if (event_id == WIFI_USER_EVT_DISCONNECTED) {
        uint8_t reason = *(uint8_t *)arg;

        wifi_status.sta_connected = 0;
        wifi_status.sta_got_ip = 0;
        printf("wifi disconnected (reason=%d, locally_generated=%d)\n",
               reason & 0x7F, (reason & BIT(7)) >> 7);
        if (m->ev_cb && m->ev_cb->stat_chg) {
            m->ev_cb->stat_chg(m, NOTIFY_WIFI_DISCONNECTED, &reason);
        }
    }
}

static void on_fatal_error(WIFI_USER_EVT_ID event_id, void *arg)
{
    hal_wifi_module_t *m = &aos_wifi_haas1000;

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
            if (m->ev_cb && m->ev_cb->fatal_err) {
                m->ev_cb->fatal_err(m, NULL);
            }

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
            if (m->ev_cb && m->ev_cb->fatal_err) {
                m->ev_cb->fatal_err(m, NULL);
            }
        }
    }
}

static int wifi_init(hal_wifi_module_t *m)
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
    printf("wifi init success!!\n");
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    if (!mac) {printf("%s: invalid argument!\n", __func__); return;}
    bwifi_get_own_mac(mac);
};

static void wifi_set_mac_addr(hal_wifi_module_t *m, const uint8_t *mac)
{
    if (!mac) {printf("%s: invalid argument!\n", __func__); return;}
    factory_section_set_wifi_address(mac);
};

static void wifi_connect_task(void *arg)
{
    int ret = 0;
    hal_wifi_init_type_t *init_para = (hal_wifi_init_type_t*)arg;
    hal_wifi_module_t *m = &aos_wifi_haas1000;
    if (!init_para)
        return;
    ret = bwifi_connect_to_ssid(init_para->wifi_ssid, init_para->wifi_key, 0, 0, NULL);
    if (ret) {
        printf("wifi connect to %s fail:%d\n", init_para->wifi_ssid, ret);
        if (m->ev_cb && m->ev_cb->connect_fail) {
            m->ev_cb->connect_fail(m, ret, NULL);
        }
    }
    aos_free(init_para);
    //aos_task_exit(0);
    osThreadExitPub();
}

__SRAMDATA osThreadDef(wifi_connect_task, osPriorityNormal, 1, (4096), "wifi_connect");
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    if (!init_para) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (WARN_ON(!wifi_status.wifi_started || wifi_status.resetting))
        return -1;

    if (init_para->wifi_mode == STATION) {
        hal_wifi_init_type_t *init_para_ptr = aos_malloc(sizeof(hal_wifi_init_type_t));
        if (!init_para_ptr) {
            printf("Failed to alloc init para for wifi_connect_task\n");
            return -1;
        }
        strcpy(init_para_ptr->wifi_ssid, init_para->wifi_ssid);
        strcpy(init_para_ptr->wifi_key, init_para->wifi_key);
        //if (aos_task_new("wifi_connect", wifi_connect_task, init_para_ptr, 4096)) {
        if (osThreadCreate(osThread(wifi_connect_task), init_para_ptr) == NULL) {
            printf("Failed to create wifi_connect_task\n");
            aos_free(init_para_ptr);
            return -1;
        }
    } else if (init_para->wifi_mode == SOFT_AP) {
        /* TODO */
    }

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    printf("WiFi HAL %s not implemeted yet!\n", __func__);
    return 0;
}

static int set_ip_stat(hal_wifi_module_t *m,
                       hal_wifi_ip_stat_t *in_net_para,
                       hal_wifi_type_t wifi_type)
{
#if LWIP_ETHERNETIF && !LWIP_SUPPORT
    struct ip_info ip;

    if (!in_net_para) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (wifi_type == STATION && wifi_status.sta_connected) {
        memcpy(&wifi_status.network.ip_stat, in_net_para, sizeof(hal_wifi_ip_stat_t));
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

static int get_ip_stat(hal_wifi_module_t *m,
                       hal_wifi_ip_stat_t *out_net_para,
                       hal_wifi_type_t wifi_type)
{
#if LWIP_SUPPORT
    if (!out_net_para) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (wifi_type == STATION && wifi_status.sta_got_ip) {
        memcpy(out_net_para, &wifi_status.network.ip_stat, sizeof(hal_wifi_ip_stat_t));
        return 0;
    }
#endif
    return -1;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    if (!out_stat) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (wifi_status.sta_connected) {
        out_stat->is_connected = 1;
        out_stat->rssi = bwifi_get_current_rssi();
        bwifi_get_current_ssid(out_stat->ssid);
        bwifi_get_current_bssid(out_stat->bssid);
        out_stat->channel = bwifi_get_current_channel();
    } else
        out_stat->is_connected = 0;

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

static int wifi_scan(hal_wifi_module_t *m, scan_type_t t)
{
    int i, ret;
    struct bwifi_ssid *scan_ssid, *prev_scan_ssid;
    struct bwifi_scan_config scan_config;
    struct bwifi_bss_info *scan_result;
    hal_wifi_scan_result_t result = {0};
    hal_wifi_scan_result_adv_t result_adv = {0};
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

    if (t == SCAN_ADV) {
        result_adv.ap_num = ret;
        result_adv.ap_list = aos_malloc(ret * sizeof(ap_list_adv_t));
        if (!result_adv.ap_list) {
            printf("Failed to malloc the returned ap list\n");
            result_adv.ap_num = 0; goto end;
        }
    } else {
        result.ap_num = ret;
        result.ap_list = aos_malloc(ret * sizeof(ap_list_t));
        if (!result.ap_list) {
            printf("Failed to malloc the returned ap list\n");
            result.ap_num = 0; goto end;
        }
    }

    for (i = 0; i < ret; i++) {
        struct bwifi_bss_info *r = scan_result + i;

        if (t == SCAN_ADV) {
            ap_list_adv_t *res_adv = result_adv.ap_list + i;
            memcpy(res_adv->ssid, r->ssid, sizeof(res_adv->ssid));
            res_adv->ap_power = r->rssi;
            memcpy(res_adv->bssid, r->bssid, sizeof(res_adv->bssid));
            res_adv->channel = r->channel;
            //res_adv->security = r->security_type;
        } else {
            ap_list_t *res = result.ap_list + i;
            memcpy(res->ssid, r->ssid, sizeof(res->ssid));
            res->ap_power = r->rssi;
            memcpy(res->bssid, r->bssid, sizeof(res->bssid));
        }
    }

end:
    if (t == SCAN_ADV) {
        if (m->ev_cb && m->ev_cb->scan_adv_compeleted) {
            m->ev_cb->scan_adv_compeleted(m, &result_adv, NULL);
        }
    } else {
        if (m->ev_cb && m->ev_cb->scan_compeleted) {
            m->ev_cb->scan_compeleted(m, &result, NULL);
        }
    }

    if (result.ap_list)
        aos_free(result.ap_list);

    if (result_adv.ap_list)
        aos_free(result_adv.ap_list);

    if (scan_result)
        aos_free(scan_result);

    return 0;
}

static int start_scan(hal_wifi_module_t *m)
{
    return wifi_scan(m, SCAN_NORMAL);
}

static int start_scan_adv(hal_wifi_module_t *m)
{
    return wifi_scan(m, SCAN_ADV);
}

static int start_specified_scan(hal_wifi_module_t *m, ap_list_t *ap_list, int ap_num)
{
#if defined(SUPPORT_24G_SPECIFIED_SCAN) || defined(SUPPORT_5G_SPECIFIED_SCAN)
    int i = 0;

    if (ap_num > 3)
        ap_num = 3;
    wifi_status.scan_req.ap_num = ap_num;
    while (i < ap_num) {
        wifi_status.scan_req.ap_list[i] = *(ap_list + i);
        i++;
    }

    return wifi_scan(m, SCAN_SPECIFIED);
#else
    return wifi_scan(m, SCAN_NORMAL);
#endif
}

static int power_off(hal_wifi_module_t *m)
{
    printf("WiFi HAL %s not implemeted yet!\n", __func__);
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    printf("WiFi HAL %s not implemeted yet!\n", __func__);
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    if (wifi_status.wifi_started && wifi_status.sta_connected && !wifi_status.resetting)
        return bwifi_disconnect();

    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    int ret = 0;

    ret = suspend_station(m);
    if (wifi_status.sap_started)
        ret += suspend_soft_ap(m);
    return ret;
}

static int cancel_connect(hal_wifi_module_t *m)
{
    if (wifi_status.wifi_started && !wifi_status.sta_connected && !wifi_status.resetting)
        return bwifi_disconnect();

    return -1;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    if (WARN_ON(!wifi_status.wifi_started ||
                wifi_status.sta_connected))
        return -1;

    return bes_sniffer_set_channel((u8)ch);
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

static int start_monitor(hal_wifi_module_t *m)
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

static int stop_monitor(hal_wifi_module_t *m)
{
    wifi_status.smart_config = 0;
    return bes_sniffer_stop();
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    promisc_cb = fn;
}

/* Wi-Fi Zero Config */

static int start_mgnt_monitor(hal_wifi_module_t *m)
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

static int stop_mgnt_monitor(hal_wifi_module_t *m)
{
    wifi_status.zero_config = 0;
    return bes_sniffer_stop();
}

static void register_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    mgnt_frame_cb = fn;
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    if (WARN_ON(!wifi_status.sta_connected ||
                wifi_status.resetting))
        return -1;

    return bes_sniffer_send_mgmt_frame(bwifi_get_current_channel(), buf, len);
}

struct netif *wifi_get_netif(hal_wifi_module_t *m, hal_wifi_type_t mode)
{
#if LWIP_ETHERNETIF && !LWIP_SUPPORT
    if (mode == STATION)
        return bwifi_get_netif(WIFI_IF_STATION);
    else if (mode == SOFT_AP)
        return bwifi_get_netif(WIFI_IF_SOFTAP);
#endif
    return NULL;
}

static int start_ap(hal_wifi_module_t *m,
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

static int stop_ap(hal_wifi_module_t *m)
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

hal_wifi_module_t aos_wifi_haas1000 = {
    .base.name            = "aos_wifi_haas1000",
    .init                 =  wifi_init,
    .get_mac_addr         =  wifi_get_mac_addr,
    .set_mac_addr         =  wifi_set_mac_addr,
    .start                =  wifi_start,
    .start_adv            =  wifi_start_adv,
    .set_ip_stat          =  set_ip_stat,
    .get_ip_stat          =  get_ip_stat,
    .get_link_stat        =  get_link_stat,
    .start_scan           =  start_scan,
    .start_scan_adv       =  start_scan_adv,
    .start_specified_scan =  start_specified_scan,
    .power_off            =  power_off,
    .power_on             =  power_on,
    .suspend              =  suspend,
    .suspend_station      =  suspend_station,
    .suspend_soft_ap      =  suspend_soft_ap,
    .cancel               =  cancel_connect,
    .set_channel          =  set_channel,
    .start_monitor        =  start_monitor,
    .stop_monitor         =  stop_monitor,
    .register_monitor_cb  =  register_monitor_cb,
    .start_mgnt_monitor   =  start_mgnt_monitor,
    .stop_mgnt_monitor    =  stop_mgnt_monitor,
    .register_wlan_mgnt_monitor_cb =  register_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,

    /* get STA/AP mode interface */
    .get_netif            =  wifi_get_netif,

    /* for device ap */
    .start_ap             =  start_ap,
    .stop_ap              =  stop_ap,

    /* mesh related */
};
