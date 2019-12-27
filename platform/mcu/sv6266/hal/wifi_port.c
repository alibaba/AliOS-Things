/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <network/network.h>
#include "network/hal/wifi.h"

#include "cli.h"
//#include "common.h"
#include "wifi_api.h"


#if defined(PORTING_DEBUG)
#define LOG_AOS_HAL(...) \
    printf(__VA_ARGS__)
#else
#define LOG_AOS_HAL(...)
#endif

#define ERROR_SUCCESS 0
#define ERROR_INVALID_PARAMETER -1
#define ERROR_NOT_IMPLEMENT -2


extern int mac_80211_tx_rawpkt(u8 *data, int len);
extern void mgmt_register_cbfn(void *fn);
static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type);

hal_wifi_module_t sim_aos_wifi_icomm;
monitor_data_cb_t gallpktfn = NULL;
monitor_data_cb_t gmgmtpktfn = NULL;

static bool factory_mode = false;
void inet_ntoa_icomm(ip4_addr_t *ipaddr, char *ipstr)
{
    char *tmpstr;

    tmpstr = ip4addr_ntoa(ipaddr);
    memcpy(ipstr, tmpstr, strlen(tmpstr));
}

char hextochar(u8 hex)
{
    if(0x0 <= hex && hex <= 0x9)
        return '0' + hex;
    else if(0xa <= hex && hex <= 0xf)
        return 'A' + hex - 0xa;
    else
        return 0;
}

void machextostr(u8 *mac, char *str)
{
    int i;
    for(i = 0; i < 6; i++){
       str[2 * i] = hextochar(mac[i] >> 4);
       str[2 * i + 1] = hextochar(mac[i] & 0xF);
    }
}

//struct AP_LIST_ADV{
//    char ssid[32];    /* The SSID of an access point. */
//    char ap_power;    /* Signal strength, min:0, max:100 */
//    char bssid[6];    /* The BSSID of an access point. */
//    char channel;     /* The RF frequency, 1-13 */
//    uint8_t security; /* Security type, @ref wlan_sec_type_t */
//} AP_LIST_ADV;

void wifi_cbfu(WIFI_RSP *msg)
{
    u8 dhcpen;
    u8 mac[6];
    u8 ipv4[4];
    int wifi_strength, channel;
    u8 ssidlen = 32;

    uip_ip4addr_t ipaddr, submask, gateway, dnsserver;
    s8 ret;
    char ipstr[16];
    hal_wifi_ip_stat_t ipstat = {0};
    hal_wifi_ap_info_adv_t info = {0};

    if(msg->wifistatus == 1) {
        LOG_AOS_HAL("wifi connected:%d\n", msg->id);
        get_ip_stat(&sim_aos_wifi_icomm, &ipstat, STATION);
        sim_aos_wifi_icomm.ev_cb->stat_chg(&sim_aos_wifi_icomm, NOTIFY_STATION_UP, NULL);
        sim_aos_wifi_icomm.ev_cb->ip_got(&sim_aos_wifi_icomm, &ipstat, NULL);
        get_connectap_info(0, info.ssid, &ssidlen, info.bssid, 6, (u8 *)&wifi_strength, (u8 *)&(info.channel));
        sim_aos_wifi_icomm.ev_cb->para_chg(&sim_aos_wifi_icomm, &info, NULL, 0, NULL);
    }
    else {
        LOG_AOS_HAL("wifi disconnected:%d\n", msg->id);
        sim_aos_wifi_icomm.ev_cb->stat_chg(&sim_aos_wifi_icomm, NOTIFY_STATION_DOWN, NULL);
    }
}

void scan_cp()
{
    int i;
    TAG_AP_INFO *tmpap;
    hal_wifi_scan_result_t aplist;

	if (sim_aos_wifi_icomm.ev_cb == NULL)
        return;
    if (sim_aos_wifi_icomm.ev_cb->scan_compeleted == NULL)
        return;

    aplist.ap_num = get_ap_lsit_total_num();
    aplist.ap_list = OS_MemAlloc(aplist.ap_num * sizeof(ap_list_t));
    memset(aplist.ap_list, 0, aplist.ap_num * sizeof(ap_list_t));
    tmpap = OS_MemAlloc(aplist.ap_num * sizeof(TAG_AP_INFO));
    get_ap_list(tmpap, (u8 *)&aplist.ap_num);

    for(i = 0; i < aplist.ap_num; i++){
        aplist.ap_list[i].ap_power = -tmpap[i].rssi;
        memcpy(aplist.ap_list[i].ssid, tmpap[i].name, tmpap[i].name_len);
    }
    OS_MemFree(tmpap);

    sim_aos_wifi_icomm.ev_cb->scan_compeleted(&sim_aos_wifi_icomm, (hal_wifi_scan_result_t*)&aplist, NULL);

    OS_MemFree(aplist.ap_list);
}

u8 enctypeicommtoali(u8 type, u8 subtype)
{
    u8 enctype;

    if(type == NONE)
        enctype = SECURITY_TYPE_NONE;
    else if(type == WEP)
        enctype = SECURITY_TYPE_WEP;
    else if(type == WPA){
        if(subtype == TKIP_CCMP || subtype == CCMP)
            enctype = SECURITY_TYPE_WPA_AES;
        else
            enctype = SECURITY_TYPE_WPA_TKIP;
    }
    else{
        if(subtype == TKIP_CCMP)
            enctype = SECURITY_TYPE_WPA2_MIXED;
        else if(subtype == CCMP)
            enctype = SECURITY_TYPE_WPA2_AES;
        else
            enctype = SECURITY_TYPE_WPA2_TKIP;
    }
    return enctype;
}

u8 enctypealitoicomm(u8 type)
{
    u8 enctype = SECURITY_TYPE_NONE;

    if(type == SECURITY_TYPE_NONE)
        enctype = NET80211_CRYPT_NONE;
    else if(type == SECURITY_TYPE_WEP)
        enctype = NET80211_CRYPT_WEP;
    else if(type == SECURITY_TYPE_WPA_TKIP || type == SECURITY_TYPE_WPA2_TKIP){
        enctype = NET80211_CRYPT_TKIP;
    }
    else if(type == SECURITY_TYPE_WPA_AES || type == SECURITY_TYPE_WPA2_AES){
        enctype = NET80211_CRYPT_CCMP;
    }
    else if(type == SECURITY_TYPE_WPA2_MIXED){
        enctype = NET80211_CRYPT_CCMP_TKIP;
    }
    return enctype;
}

void scan_cpadv()
{
    int i;
    TAG_AP_INFO *tmpap;
    hal_wifi_scan_result_adv_t aplist;

	if (sim_aos_wifi_icomm.ev_cb == NULL)
        return;
    if (sim_aos_wifi_icomm.ev_cb->scan_compeleted == NULL)
        return;

    aplist.ap_num = get_ap_lsit_total_num();
    aplist.ap_list = OS_MemAlloc(aplist.ap_num * sizeof(ap_list_adv_t));
    memset(aplist.ap_list, 0, aplist.ap_num * sizeof(ap_list_adv_t));
    tmpap = OS_MemAlloc(aplist.ap_num * sizeof(TAG_AP_INFO));
    get_ap_list(tmpap, (u8 *)&aplist.ap_num);

    for(i = 0; i < aplist.ap_num; i++){
        aplist.ap_list[i].ap_power = -tmpap[i].rssi;
        memcpy(aplist.ap_list[i].ssid, tmpap[i].name, tmpap[i].name_len);
        memcpy(aplist.ap_list[i].bssid, tmpap[i].mac, 6);
        aplist.ap_list[i].channel = tmpap[i].channel;
        aplist.ap_list[i].security = enctypeicommtoali(tmpap[i].security_type, tmpap[i].security_subType);
    }
    OS_MemFree(tmpap);

	sim_aos_wifi_icomm.ev_cb->scan_adv_compeleted(&sim_aos_wifi_icomm, (hal_wifi_scan_result_adv_t*)&aplist, NULL);

    OS_MemFree(aplist.ap_list);
}

//Need to modify this api for new parameter
void alisniffercb(packetinfo *pinfo)
{
    monitor_data_cb_t fn = NULL;
    hal_wifi_link_info_t info;
    OS_DeclareCritical();
    OS_EnterCritical();
    fn = gallpktfn;
    OS_ExitCritical();

    if(fn != NULL)
    {
        info.rssi = -(pinfo->rssi);
        fn(pinfo->data, pinfo->len, &info);
    }
}
void alimgmtcb(packetinfo *pinfo)
{
    hal_wifi_link_info_t info;
    if(gmgmtpktfn != NULL)
    {
        info.rssi = -(pinfo->rssi);
        gmgmtpktfn(pinfo->data, pinfo->len, &info);
     }
}

static int wifi_init(hal_wifi_module_t *m)
{
    LOG_AOS_HAL("wifi_init!!\n");
    set_country_code(CN);
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    //LOG_AOS_HAL("wifi_get_mac_addr!!\n");
    get_local_mac(0, mac, 6);
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    u32 ipaddr, submask, gateway, dnsserver;
    int ret;

    if(init_para == NULL)
        return -1;

    if(init_para->wifi_mode == SOFT_AP){
        DUT_wifi_start(DUT_AP);
    }
    else if(init_para->wifi_mode == STATION){
        DUT_wifi_start(DUT_STA);
        if(init_para->dhcp_mode == DHCP_SERVER){
            return -1;
        }else if(init_para->dhcp_mode == DHCP_DISABLE){
            ipaddr = ipaddr_addr(init_para->local_ip_addr);
            submask = ipaddr_addr(init_para->net_mask);
            gateway = ipaddr_addr(init_para->gateway_ip_addr);
            dnsserver = ipaddr_addr(init_para->dns_server_ip_addr);
            set_if_config_2(0, 0, ipaddr, submask, gateway, dnsserver);
        }

        ret = set_wifi_config_3(0, (u8 *)init_para->wifi_ssid, strlen(init_para->wifi_ssid), (u8 *)init_para->wifi_key, strlen(init_para->wifi_key), NULL, 6, NET80211_CRYPT_UNKNOWN);
        if(ret == 0) {
            wifi_connect_2(0, wifi_cbfu);
        }else {
            wifi_connect_active_2((u8 *)init_para->wifi_ssid, strlen(init_para->wifi_ssid), (u8 *)init_para->wifi_key, strlen(init_para->wifi_key), wifi_cbfu, NET80211_CRYPT_UNKNOWN);
        }
    }
    return 0;
}

int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    u32 ipaddr, submask, gateway, dnsserver;
    int ret, i;
    u8 apnum;
    TAG_AP_INFO *tmpap;

    LOG_AOS_HAL("wifi_start_adv!!\n");
    if(init_para_adv == NULL)
        return -1;

    DUT_wifi_start(DUT_STA);

    if(init_para_adv->dhcp_mode == DHCP_SERVER){
        return -1;
    }else if(init_para_adv->dhcp_mode == DHCP_DISABLE){
        ipaddr = ipaddr_addr(init_para_adv->local_ip_addr);
        submask = ipaddr_addr(init_para_adv->net_mask);
        gateway = ipaddr_addr(init_para_adv->gateway_ip_addr);
        dnsserver = ipaddr_addr(init_para_adv->dns_server_ip_addr);
        set_if_config_2(0, 0, ipaddr, submask, gateway, dnsserver);
    }

    apnum = get_ap_lsit_total_num();
    tmpap = OS_MemAlloc(apnum * sizeof(TAG_AP_INFO));
    get_ap_list(tmpap, &apnum);

    for(i = 0; i < apnum; i++)
    {
        if(memcmp(init_para_adv->ap_info.ssid, tmpap[i].name, tmpap[i].name_len) != 0){
            continue;
        }
        if(memcmp(init_para_adv->ap_info.bssid, tmpap[i].mac, 6) != 0){
            continue;
        }
        if(init_para_adv->ap_info.channel != 0 && init_para_adv->ap_info.channel != tmpap[i].channel){
            continue;
        }
        if(init_para_adv->ap_info.security != enctypeicommtoali(tmpap[i].security_type, tmpap[i].security_subType)){
            continue;
        }
        break;
    }

    OS_MemFree(tmpap);

    if(i != apnum) {
        set_wifi_config_3(0, (u8 *)init_para_adv->ap_info.ssid, strlen(init_para_adv->ap_info.ssid), (u8 *)init_para_adv->key, init_para_adv->key_len,
            (u8 *)init_para_adv->ap_info.bssid, 6, enctypealitoicomm(init_para_adv->ap_info.security));
        wifi_connect_2(0, wifi_cbfu);
    }else {
/*        wifi_connect_active_3 (init_para_adv->ap_info.ssid, strlen(init_para_adv->ap_info.ssid), init_para_adv->key, init_para_adv->key_len,
            enctypealitoicomm(init_para_adv->ap_info.security), init_para_adv->ap_info.channel, init_para_adv->ap_info.bssid, wifirspcbfunc);*/
    }
    return 0;
}

typedef union icomm_ip4addr {
  u8  u8[4];
  u16 u16[2];
  u32 u32;
} icomm_ip4addr;

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    u8 dhcpen;
    unsigned char mac[6];
    ip4_addr_t ipaddr, submask, gateway, dnsserver;
    char *ipstr;

    if(out_net_para == NULL)
        return -1;

    if(wifi_type == STATION)
    {
        get_if_config_2(IF0_NAME, &out_net_para->dhcp, &ipaddr.addr, &submask.addr, &gateway.addr, &dnsserver.addr, mac, 6);
    }
    else if(wifi_type == SOFT_AP)
    {
        get_if_config_2(IF1_NAME, &out_net_para->dhcp, &ipaddr.addr, &submask.addr, &gateway.addr, &dnsserver.addr, mac, 6);
    }
    else
        return -1;

    machextostr(mac, out_net_para->mac);
    inet_ntoa_icomm(&ipaddr, out_net_para->ip);
    inet_ntoa_icomm(&submask, out_net_para->mask);
    inet_ntoa_icomm(&gateway, out_net_para->gate);
    inet_ntoa_icomm(&dnsserver, out_net_para->dns);

    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    u8 ssidlen = 32, keylen = 64;
    u8 key[64];

    LOG_AOS_HAL("get_link_stat!!\n");
    if(out_stat == NULL)
        return -1;

    out_stat->is_connected = get_wifi_status_2(0);
    if(out_stat->is_connected == 1)
    {
        //Need to use new channel information.
        get_connectap_info(0, out_stat->ssid, &ssidlen, out_stat->bssid, 6, (u8 *)&(out_stat->wifi_strength), (u8 *)&(out_stat->channel));
        out_stat->wifi_strength = -out_stat->wifi_strength;
    }
	return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
    if(factory_mode)
	return;
    LOG_AOS_HAL("start_scan!!\n");
    scan_AP_custom(NULL, scan_cp, 0x1fff, 0x0, 250);
}

void start_scan_adv(hal_wifi_module_t *m)
{
    if(factory_mode)
	return;
    LOG_AOS_HAL("start_scan_adv!!\n");
    scan_AP_custom(NULL, scan_cpadv, 0x1fff, 0x0, 250);
}

static int power_off(hal_wifi_module_t *m)
{
    if(factory_mode);
	return 0;
    LOG_AOS_HAL("power_off!!\n");
    DUT_wifi_start(DUT_NONE);
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    //Enable DUT_STA mode to make sure scan function can work
    if(factory_mode)
	return 0;
    LOG_AOS_HAL("power_on!!\n");
    DUT_wifi_start(DUT_STA);
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    if(factory_mode)
        return 0;
    LOG_AOS_HAL("suspend!!\n");
    DUT_wifi_start(DUT_NONE);
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    if(factory_mode)
        return 0;
    LOG_AOS_HAL("suspend_station!!\n");
    if(get_operation_mode() == DUT_STA)
        DUT_wifi_start(DUT_NONE);
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    LOG_AOS_HAL("suspend_soft_ap!!\n");
    if(get_operation_mode() == DUT_AP)
        DUT_wifi_start(DUT_NONE);
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    int ret;

    if(factory_mode)
        return 0;
    //LOG_AOS_HAL("set_channel!!\n");
    ret = wifi_set_channel(ch, 1);
    return ret;
}

static void start_monitor(hal_wifi_module_t *m)
{
    if(factory_mode)
        return 0;
    LOG_AOS_HAL("start_monitor!!\n");
    DUT_wifi_start(DUT_SNIFFER);
}

static void stop_monitor(hal_wifi_module_t *m)
{
    if(factory_mode)
        return 0;
    LOG_AOS_HAL("stop_monitor!!\n");
    if(get_operation_mode() == DUT_SNIFFER)
        DUT_wifi_start(DUT_NONE);
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    OS_DeclareCritical();
    LOG_AOS_HAL("register_monitor_cb!! fn = %xh\n", fn);
    OS_EnterCritical();
    gallpktfn = fn;
    OS_ExitCritical();
    set_sniffer_config(RECV_MGMT | RECV_DATA, alisniffercb);
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    LOG_AOS_HAL("register_wlan_mgnt_monitor_cb!!\n");
    gmgmtpktfn = fn;
    mgmt_register_cbfn(alimgmtcb);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    int ret;
    if(factory_mode)
        return 0;
    //LOG_AOS_HAL("wlan_send_80211_raw_frame!!\n");
    if(buf == NULL || len <= 0)
        return -1;

    ret = mac_80211_tx_rawpkt(buf, len);
    return ret; // len-4=exclude FCS
}


extern void ssv_dbg_cmd_parse(char *pwbuf, int blen, int argc, char **argv);
static struct cli_command andycmd = {
    .name = "SSV",
    .help = "SSV dbgcmd",
    .function = ssv_dbg_cmd_parse
};


void start_debug_mode(hal_wifi_module_t *m)
{
	factory_mode = true;
	DUT_wifi_start(DUT_NONE);
	aos_cli_register_command(&andycmd);
    //rwnx_go_debug_mode();
}

void stop_debug_mode(hal_wifi_module_t *m)
{
}

static int get_wireless_info(hal_wifi_module_t *m, void *wireless_info)
{
    int chan;
    char bssid[6];

    int rssi = -1;
    int ssidlen = 32;
    char ssid[32 + 1];
    hal_wireless_info_t *info = (hal_wireless_info_t *)wireless_info;

    printf("get wireless info\r\n");

    if (info == NULL)
        return -1;

    if (get_wifi_status() == 1) {
        get_connectap_info(0, ssid, &ssidlen, bssid, 6, (u8 *)&rssi, (u8 *)&chan);
        if (rssi > 0) {
            if (rssi >= 128) rssi = 127;
            rssi -= 128;
        }
        info->rssi = rssi;
        return 0;
    }

    return -1;
}

hal_wifi_module_t sim_aos_wifi_icomm = {
    .base.name           = "sim_aos_wifi_icomm",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode,
    .get_wireless_info   = get_wireless_info,
};

