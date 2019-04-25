/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "rda59xx_daemon.h"
#include "rda59xx_wifi_include.h"
#include "hal/wifi.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#ifndef DELETE_HFILOP_CODE
#include "ota_hal_plat.h"
#endif

typedef enum {
    SCAN_NORMAL,
    SCAN_ADV
} scan_type_t;
hal_wifi_ip_stat_t ip_stat;

unsigned int filter_backup = 0;

void wifi_event_cb(WIFI_EVENT evt, void* info)
{
    hal_wifi_module_t *m = hal_wifi_get_default_module();

    rda59xx_bss_info bss_info;
    switch (evt) {
        case EVENT_STA_GOT_IP: {
            rda59xx_sta_get_bss_info(&bss_info);
            memcpy(ip_stat.ip, ip4addr_ntoa(&(bss_info.ipaddr)), sizeof(ip_stat.ip));
            memcpy(ip_stat.gate, ip4addr_ntoa(&(bss_info.gateway)), sizeof(ip_stat.gate));
            memcpy(ip_stat.mask, ip4addr_ntoa(&(bss_info.mask)), sizeof(ip_stat.mask));
            memcpy(ip_stat.dns, ip4addr_ntoa(&(bss_info.dns1)), sizeof(ip_stat.dns));
            snprintf(ip_stat.mac, 12, "%02x%02x%02x%02x%02x%02x",
                    bss_info.bssid[0], bss_info.bssid[1], bss_info.bssid[2], bss_info.bssid[3], bss_info.bssid[4], bss_info.bssid[5]);
            if (m->ev_cb && m->ev_cb->ip_got) {
                m->ev_cb->ip_got(m, &ip_stat, NULL);
            }
            break;
        }
        case EVENT_STA_CONNECTTED: {
            if (m->ev_cb && m->ev_cb->stat_chg) {
                m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
            }
            break;
        }
        case EVENT_STA_DISCONNECTTED: {
            if (m->ev_cb && m->ev_cb->stat_chg) {
                m->ev_cb->stat_chg(m, NOTIFY_STATION_DOWN, NULL);
            }
            break;
        }
        case EVENT_STA_CONNECT_FAIL: {
            if (m->ev_cb && m->ev_cb->connect_fail) {
                m->ev_cb->connect_fail(m, 0, NULL);
            }
            break;
        }
        default: {
            break;
        }
    }

    return;
}

/**
    @brief Wifi scan ready
*/
static void scan_done(hal_wifi_module_t *m, scan_type_t t)
{
    uint16_t ap_num = 16;
    rda59xx_scan_result *ap_records;
    int i;
    hal_wifi_scan_result_t *result = NULL;
    hal_wifi_scan_result_adv_t *result_adv = NULL;

    if (t != SCAN_NORMAL && t != SCAN_ADV) return;

    ap_num = rda59xx_get_scan_num();
    if (ap_num > 16)
        ap_num = 16;

    ap_records = malloc(ap_num * sizeof(*ap_records));
    if (!ap_records)
        return;

    if (t == SCAN_NORMAL) {
        result = malloc(sizeof(hal_wifi_scan_result_t));
        if (!result) goto end;
        result->ap_num = ap_num;
        result->ap_list = malloc(ap_num * sizeof(ap_list_t));
        if (!result->ap_list) goto end;
    } else {
        result_adv = malloc(sizeof(hal_wifi_scan_result_adv_t));
        if (!result_adv) goto end;
        result_adv->ap_num = ap_num;
        result_adv->ap_list = malloc(ap_num * sizeof(ap_list_adv_t));
        if (!result_adv->ap_list) goto end;
    }

    rda59xx_get_scan_result(ap_records, ap_num);

    for (i = 0; i < ap_num; i++) {
        rda59xx_scan_result *r = ap_records + i;

        if (t == SCAN_NORMAL) {
            ap_list_t *res = result->ap_list + i;
            memcpy(res->ssid, r->SSID, sizeof(res->ssid));
            res->ap_power = r->RSSI;
        } else {
            ap_list_adv_t *res_adv = result_adv->ap_list + i;
            memcpy(res_adv->ssid, r->SSID, sizeof(res_adv->ssid));
            res_adv->ap_power = r->RSSI;
            memcpy(res_adv->bssid, r->BSSID, sizeof(res_adv->bssid));
            res_adv->channel = r->channel;
            res_adv->security = r->secure_type;
        }
    }

    if (t == SCAN_NORMAL) {
        if (m->ev_cb && m->ev_cb->scan_compeleted) {
            m->ev_cb->scan_compeleted(m, result, NULL);
        }
    } else {
        if (m->ev_cb && m->ev_cb->scan_adv_compeleted) {
            m->ev_cb->scan_adv_compeleted(m, result_adv, NULL);
        }
    }

end:
    if (ap_records)
        free(ap_records);
    if (result) {
        if (result->ap_list)
            free(result->ap_list);
        free(result);
    }
    if (result_adv) {
        if (result_adv->ap_list)
            free(result_adv->ap_list);
        free(result_adv);
    }
}

static monitor_data_cb_t data_cb = NULL;
static monitor_data_cb_t mngt_data_cb = NULL;
//static monitor_data_cb_t mesh_data_cb = NULL;

typedef enum {
    HAL_WIFI_MODE_STATION,
    HAL_WIFI_MODE_SOFTAP
} hal_wifi_mode_t;

static int wifi_init(hal_wifi_module_t *m)
{
    static int inited;
    if (inited)
        return 0;
    inited = 1;

#ifndef DELETE_HFILOP_CODE
    extern int hfilop_config_init(void);
    hfilop_config_init();
#endif
    rda59xx_wifi_init();
    rda59xx_wifi_set_event_cb(wifi_event_cb);

#ifndef DELETE_HFILOP_CODE
  //  extern struct hal_ota_module_s rda59xx_ota_module;
 //   hal_ota_register_module(&rda59xx_ota_module);
	extern ota_hal_module_t ota_hal_module;
	ota_hal_register_module(&ota_hal_module);

    extern void hfilop_ota_auto_upgrade(char *ssid, char *pwd);
    hfilop_ota_auto_upgrade(NULL, NULL);

    extern int hfilop_mac_key_is_valid(void);
    if(!hfilop_mac_key_is_valid())
    {
        extern void hfilop_uart_task_start(void *);
        hfilop_uart_task_start(NULL);
        while(1)
            aos_msleep(1000);
    }
#endif

    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    rda59xx_get_macaddr(mac, 0);
};

static void wifi_set_mac_addr(hal_wifi_module_t *m, const uint8_t *mac)
{
    rda59xx_set_macaddr(mac, 0);
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    rda59xx_sta_info sta_info;

    memset(&sta_info, 0, sizeof(rda59xx_sta_info));
    memcpy(sta_info.ssid, init_para->wifi_ssid, 32+1);
    memcpy(sta_info.pw, init_para->wifi_key, 64+1);
    if(init_para->dhcp_mode){
        sta_info.dhcp = 1;
    }else{
        sta_info.dhcp = 0;
        ip4addr_aton((const char *)(init_para->local_ip_addr), (ip4_addr_t*)&(sta_info.ip));
        ip4addr_aton((const char *)(init_para->net_mask), (ip4_addr_t*)&(sta_info.netmask));
        ip4addr_aton((const char *)(init_para->gateway_ip_addr), (ip4_addr_t*)&(sta_info.gateway));
    }

#ifndef DELETE_HFILOP_CODE
    if(init_para->reserved[0] == 1){
        extern r_s32 rda59xx_sta_connect_ex(rda59xx_sta_info *sta_info);
         rda59xx_sta_connect_ex(&sta_info);
    }
    else
#endif
    rda59xx_sta_connect(&sta_info);
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m,
                          hal_wifi_init_type_adv_t *init_para_adv)
{
    printf("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m,
                       hal_wifi_ip_stat_t *out_net_para,
                       hal_wifi_type_t wifi_type)
{
    rda59xx_bss_info bss_info;
    unsigned int state = 0;
    state = rda59xx_get_module_state();
    if ((wifi_type == STATION) && (state & STATE_STA)){
        rda59xx_sta_get_bss_info(&bss_info);
        strcpy(out_net_para->ip, ip4addr_ntoa(&bss_info.ipaddr));
        strcpy(out_net_para->mask, ip4addr_ntoa(&bss_info.mask));
        strcpy(out_net_para->gate, ip4addr_ntoa(&bss_info.gateway));
        strcpy(out_net_para->dns, ip4addr_ntoa(&bss_info.dns1));
        r_u32 broadcastip;
        broadcastip = (bss_info.mask&bss_info.ipaddr)|(~bss_info.mask);
        strcpy(out_net_para->broadcastip, ip4addr_ntoa(&broadcastip));
        snprintf(out_net_para->mac, 12, "%02x%02x%02x%02x%02x%02x",
                bss_info.bssid[0], bss_info.bssid[1], bss_info.bssid[2], bss_info.bssid[3], bss_info.bssid[4], bss_info.bssid[5]);
    }
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m,
                         hal_wifi_link_stat_t *out_stat)
{
    rda59xx_bss_info bss_info;
    unsigned int state = 0;
    state = rda59xx_get_module_state();

    out_stat->is_connected = 0;
    if(state & STATE_STA){
        rda59xx_sta_get_bss_info(&bss_info);
        out_stat->is_connected = 1;
        out_stat->channel = bss_info.channel;
        out_stat->wifi_strength = bss_info.rssi;
        memcpy(out_stat->bssid, bss_info.bssid, 6);
        memcpy(out_stat->ssid, bss_info.ssid, sizeof(bss_info.ssid));
    }
    return 0;
}

#ifndef DELETE_HFILOP_CODE
typedef int (*wifi_scan_callback_t)(ap_list_adv_t *);
static int transform_rssi(int rssi_dbm)
{
	int ret;
	ret = (rssi_dbm+95)*2;

	if (ret < 70)
		ret = ret -(15 - ret/5);

	if(ret < 0)
		ret = 0;
	else if(ret >100)
		ret = 100;

	return ret;
}

int hfilop_wifi_scan(wifi_scan_callback_t cb)
{
	rda59xx_scan_info scan_info;
	memset(&scan_info, 0, sizeof(rda59xx_scan_info));
	scan_info.scan_mode = 1;
	scan_info.scan_time = 3;
	rda59xx_scan(&scan_info);

	int ap_num = 0, i;
	if(cb != NULL)
	{
		ap_list_adv_t ap_info;
		rda59xx_scan_result *ap_records;
		ap_num = rda59xx_get_scan_num();
		if (ap_num > 50)
			ap_num = 50;

		ap_records = aos_malloc(ap_num * sizeof(*ap_records));
		if (!ap_records)
			return 0;

		rda59xx_get_scan_result(ap_records, ap_num);

		for (i = 0; i < ap_num; i++)
		{
			rda59xx_scan_result *r = ap_records + i;
			memset(&ap_info, 0, sizeof(ap_info));
			memcpy(ap_info.bssid, r->BSSID, sizeof(ap_info.bssid));
			memcpy(ap_info.ssid, r->SSID, r->SSID_len);
			ap_info.ap_power = transform_rssi(r->RSSI);
			ap_info.channel = r->channel;
			ap_info.security = r->secure_type;

			cb(&ap_info);
		}
		if (ap_records)
			aos_free(ap_records);
	}
	return ap_num;
}
#endif

static void wifi_scan(hal_wifi_module_t *m)
{
    rda59xx_scan_info scan_info;
    memset(&scan_info, 0, sizeof(rda59xx_scan_info));
    rda59xx_scan(&scan_info);
    scan_done(m, SCAN_NORMAL);;
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    rda59xx_scan_info scan_info;
    memset(&scan_info, 0, sizeof(rda59xx_scan_info));
    rda59xx_scan(&scan_info);
    scan_done(m, SCAN_ADV);
}

static int power_off(hal_wifi_module_t *m)
{
    printf("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    printf("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    rda59xx_sta_disconnect();
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    rda59xx_sta_disconnect();
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    //unsigned int mode = rda59xx_get_module_state();
    //if(mode & STATE_SNIFFER)
    //    rda59xx_sniffer_set_channel(ch);
    //else
        rda59xx_set_channel(ch);
    return 0;
}

static int get_channel(hal_wifi_module_t *m)
{
    rda59xx_bss_info bss_info;
    unsigned int state = 0;

    if(state & STATE_STA){
        rda59xx_sta_get_bss_info(&bss_info);
        return bss_info.channel;
    }
    return 0;
}

int sniffer_cb(void *data, unsigned short data_len)
{
    if(data_cb != NULL)
        (*data_cb)((uint8_t*)data, (int)data_len, NULL);
    return 0;
}


static void start_monitor(hal_wifi_module_t *m)
{
    //if softap smartconfig failed, it will start monitor dirictly
    //so add disconnect to end last link
    rda59xx_sta_disconnect();
    rda59xx_sniffer_enable(sniffer_cb);
    rda59xx_sniffer_set_filter(1, 1, 0x27e77);
    filter_backup = 0x27e77;
}

static int start_ap(hal_wifi_module_t *m, const char *ssid, const char *passwd, int interval, int hide)
{
    rda59xx_ap_info aws_ap_info;
    memset((void*)&aws_ap_info, 0, sizeof(rda59xx_ap_info));

    strncpy(aws_ap_info.ssid, ssid, sizeof(aws_ap_info.ssid));
    strncpy(aws_ap_info.pw, passwd, sizeof(aws_ap_info.pw));

    aws_ap_info.channel = 6;
    aws_ap_info.hidden = hide;
    aws_ap_info.beacon = interval;

	aws_ap_info.dhcps=inet_addr("10.10.100.1");
	aws_ap_info.dhcpe=inet_addr("10.10.100.255");
	aws_ap_info.ip=inet_addr("10.10.100.1");
	aws_ap_info.gateway=inet_addr("10.10.100.1");
	aws_ap_info.netmask=inet_addr("255.255.255.0");

	return rda59xx_ap_enable(&aws_ap_info);
}

static int stop_ap(hal_wifi_module_t *m)
{
    return rda59xx_ap_disable();
}

static void stop_monitor(hal_wifi_module_t *m)
{
    rda59xx_sniffer_disable();
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    data_cb = fn;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m,
                                          monitor_data_cb_t fn)
{
    data_cb = fn;
    if (data_cb) {
        rda59xx_sniffer_enable(sniffer_cb);
        rda59xx_sniffer_set_filter(1, 1, 0x7fe77);
        filter_backup = 0x7fe77;
    } else {
        rda59xx_sniffer_disable();
    }
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m,
                                     uint8_t *buf, int len)
{
    rda59xx_send_rawdata(buf, len);
    return 0;
}

hal_wifi_module_t aos_wifi_rda59xx = {
    .base.name           = "aos_wifi_rda59xx",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .set_mac_addr        =  wifi_set_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  wifi_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .get_channel         =  get_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .start_ap            =  start_ap,
    .stop_ap             =  stop_ap,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,

    /* mesh related */
    //.mesh_register_cb    =  register_mesh_cb,
    //.mesh_enable         =  mesh_enable,
    //.mesh_disable        =  mesh_disable,
};


