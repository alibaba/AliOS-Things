/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>

#include <platform/platform_stdlib.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include <wifi/wifi_ind.h>
#include <lwip_netconf.h>
#include <osdep_service.h>

#define  WIFI_CONNECT_MAX_ATTEMPT_COUNT  3

extern hal_wifi_module_t rtl8710bn_wifi_module;

static rtw_wifi_setting_t alink_wifi_config;

#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM]; 
#endif

void NetCallback(hal_wifi_ip_stat_t *pnet);
void WifiStatusHandler(int status);

static int wifi_init(hal_wifi_module_t *m)
{
    wifi_on(RTW_MODE_STA);
    wifi_disable_powersave();   
    DBG_8195A("wifi init success!!\n");
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    int ret = 0;
    memcpy(mac, LwIP_GetMAC(&xnetif[0]), 6);
    DBG_8195A("wifi_get_mac_addr: 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, mac ptr 0x%x\r\n", mac[0], mac[1],mac[2],mac[3],mac[4],mac[5], mac);
    return;
};


static void alink_set_sta_mode() 
{
	int mode;
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);

	if(mode == IW_MODE_MASTER) {
#if CONFIG_LWIP_LAYER
		dhcps_deinit();
#endif
		wifi_off();
		rtw_mdelay_os(20);
		if (wifi_on(RTW_MODE_STA) < 0){
			DBG_8195A("[%s]: Wifi on failed!Do zconfig reset!", __FUNCTION__);
		}
                wifi_disable_powersave();   
	}
	return;
}

static int alink_find_ap_from_scan_buf(char*buf, int buflen, char *target_ssid, void *user_data)
{
	rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *)user_data;
	int plen = 0;
	
	while(plen < buflen){
		u8 len, ssid_len, security_mode;
		char *ssid;

		// len offset = 0
		len = (int)*(buf + plen);
		// check end
		if(len == 0) break;
		// ssid offset = 14
		ssid_len = len - 14;
		ssid = buf + plen + 14 ;
		if((ssid_len == strlen(target_ssid))
			&& (!memcmp(ssid, target_ssid, ssid_len)))
		{
			strcpy((char*)pwifi->ssid, target_ssid);
			// channel offset = 13
			pwifi->channel = *(buf + plen + 13);
			// security_mode offset = 11
			security_mode = (u8)*(buf + plen + 11);
			if(security_mode == IW_ENCODE_ALG_NONE)
				pwifi->security_type = RTW_SECURITY_OPEN;
			else if(security_mode == IW_ENCODE_ALG_WEP)
				pwifi->security_type = RTW_SECURITY_WEP_PSK;
			else if(security_mode == IW_ENCODE_ALG_CCMP)
				pwifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
			break;
		}
		plen += len;
	}
	return 0;
}

int alink_get_ap_security_mode(char * ssid, rtw_security_t *security_mode, u8 * channel)
{
	rtw_wifi_setting_t wifi;
	int scan_buflen = 1000;

	memset(&wifi, 0, sizeof(wifi));

	if(wifi_scan_networks_with_ssid(alink_find_ap_from_scan_buf, (void*)&wifi, scan_buflen, ssid, strlen(ssid)) != RTW_SUCCESS){
		DBG_8195A("[%s]: Wifi scan failed!", __FUNCTION__);
		return -1;
	}

	if(strcmp(wifi.ssid, ssid) == 0){
		*security_mode = wifi.security_type;
		*channel = wifi.channel;
		return 0;
	}
	
	return -1;
}

int alink_connect_to_ap(unsigned char *ssid, unsigned char ssid_len, unsigned char *passwd, unsigned char passwd_len)
{	
	u8 *ifname[2] = {WLAN0_NAME,WLAN1_NAME};
	rtw_wifi_setting_t setting;
	rtw_network_info_t wifi_info;
	memset(&wifi_info, 0 , sizeof(rtw_network_info_t));
	u8 connect_retry = 3;
	int ret;
	u8 channel;

	alink_set_sta_mode();

#if CONFIG_AUTO_RECONNECT
	wifi_set_autoreconnect(1);
#endif

  	wifi_info.password_len = passwd_len;
	wifi_info.ssid.len = ssid_len;
	memcpy(wifi_info.ssid.val, ssid, ssid_len);
	memset(wifi_info.bssid.octet, 0, sizeof(wifi_info.bssid.octet));
	wifi_info.password = (unsigned char *)passwd;

	if(alink_get_ap_security_mode(wifi_info.ssid.val, &wifi_info.security_type, &channel) != 0)
	{
		channel = 0;
		wifi_info.security_type = RTW_SECURITY_WPA2_AES_PSK;
		alink_wifi_config.security_type = wifi_info.security_type;
		DBG_8195A("Warning : unknow security type, default set to WPA2_AES\r\n");
	}
	else
		alink_wifi_config.security_type = wifi_info.security_type;

	if (wifi_info.security_type == RTW_SECURITY_WEP_PSK) {
		if(wifi_info.password_len == 10) {
			u32 p[5];
			u8 pwd[6], i = 0; 
			sscanf((const char*)wifi_info.password, "%02x%02x%02x%02x%02x", &p[0], &p[1], &p[2], &p[3], &p[4]);
			for(i=0; i< 5; i++)
				pwd[i] = (u8)p[i];
			pwd[5] = '\0';
			memset(wifi_info.password, 0, 33);
			strcpy(wifi_info.password, pwd);
		} else if (wifi_info.password_len == 26) {
			u32 p[13];
			u8 pwd[14], i = 0;
			sscanf((const char*)wifi_info.password, "%02x%02x%02x%02x%02x%02x%02x"\
				"%02x%02x%02x%02x%02x%02x", &p[0], &p[1], &p[2], &p[3], &p[4],\
				&p[5], &p[6], &p[7], &p[8], &p[9], &p[10], &p[11], &p[12]);
			for(i=0; i< 13; i++)
				pwd[i] = (u8)p[i];
			pwd[13] = '\0';
			memset(wifi_info.password, 0, 33);
			strcpy(wifi_info.password, pwd);
		}
		memset(alink_wifi_config.password, 0, 65);
		memcpy(alink_wifi_config.password, wifi_info.password, strlen(wifi_info.password));
	}
	while (connect_retry) {
		DBG_8195A("\r\nwifi_connect to ssid: %s, type %d, password %s\r\n", wifi_info.ssid.val, wifi_info.security_type, wifi_info.password);
		ret = wifi_connect(wifi_info.ssid.val, wifi_info.security_type, 
				   wifi_info.password, wifi_info.ssid.len, 
				   wifi_info.password_len,
				   0,NULL);
		if (ret == 0) {
			ret = LwIP_DHCP(0, DHCP_START);
			int i = 0;
			for(i=0;i<NET_IF_NUM;i++){
				if(rltk_wlan_running(i)){
					wifi_get_setting((const char*)ifname[i],&setting);
				}
			}
			alink_wifi_config.channel = setting.channel;

			memcpy(alink_wifi_config.ssid, ssid, ssid_len);		
			memcpy(alink_wifi_config.password, passwd, passwd_len);
			hal_wifi_ip_stat_t stat;
	                u8 *ip = LwIP_GetIP(&xnetif[0]);
	                u8 *gw = LwIP_GetGW(&xnetif[0]);
	                u8 *mask = LwIP_GetMASK(&xnetif[0]);
	                u8 *mac = LwIP_GetMAC(&xnetif[0]);
            
                        stat.dhcp = DHCP_CLIENT;
                        
                        snprintf(stat.ip, 16, "%d.%d.%d.%d",  ip[0],  ip[1],  ip[2],  ip[3]);
                        snprintf(stat.gate, 16, "%d.%d.%d.%d",  gw[0],  gw[1],  gw[2],  gw[3]);
                        snprintf(stat.mask, 16, "%d.%d.%d.%d",  mask[0],  mask[1],  mask[2],  mask[3]);
                        snprintf(stat.dns, 16, "%d.%d.%d.%d",  gw[0],  gw[1],  gw[2],  gw[3]);
                        snprintf(stat.mac, 16, "%x%x%x%x%x%x",  mac[0],  mac[1],  mac[2],  mac[3], mac[4], mac[5]);
                        DBG_8195A("ip %s gw %s mask %s dns %s mac %s \r\n", stat.ip, stat.gate, stat.mask, stat.dns, stat.mac);
                        NetCallback(&stat);
			return 0;
		}else{
		    DBG_8195A("\r\nwifi connect failed, ret %d, retry %d\r\n", ret, connect_retry);
		}
	
		connect_retry --;
        
		if (connect_retry == 0) {
			return -1;
		}
	}
	return 0;
}

void wifi_connect_task(void *arg)
{
    int ret = 0;
    hal_wifi_init_type_t *init_para = (hal_wifi_init_type_t*)arg;
    
    ret = alink_connect_to_ap(init_para->wifi_ssid, strlen(init_para->wifi_ssid), 
        init_para->wifi_key, strlen(init_para->wifi_key));

    rtw_free(init_para);
    aos_task_exit(0);
}


static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    DBG_8195A("wifi_start ssid %s, key %s \r\n", init_para->wifi_ssid, init_para->wifi_key);
    if (NULL == m || NULL == init_para) {
        DBG_8195A("wifi_start: invalid parameter\n");
        return -1;
    }
    
    hal_wifi_init_type_t * init_para_ptr = rtw_malloc(sizeof(hal_wifi_init_type_t));
    strcpy(init_para_ptr->wifi_ssid, init_para->wifi_ssid);
    strcpy(init_para_ptr->wifi_key, init_para->wifi_key);

    aos_task_new("wifi_connect", wifi_connect_task, init_para_ptr, 4096);
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    DBG_8195A("wifi_start adv\r\n");

    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    DBG_8195A("get_ip_stat\r\n");            
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    DBG_8195A("get_link_stat\r\n");

    u8 *ifname[2] = {WLAN0_NAME,WLAN1_NAME};
    rtw_wifi_setting_t setting;
    int rssi;
    wifi_get_setting((const char*)ifname[0],&setting);
    wifi_show_setting((const char*)ifname[0],&setting);
    out_stat->is_connected = wifi_is_ready_to_transceive(RTW_STA_INTERFACE)?0:1;
    wifi_get_rssi(&rssi);
    out_stat->wifi_strength = rssi;
    strcpy(out_stat->ssid, setting.ssid);
    out_stat->channel = setting.channel;
    
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
    DBG_8195A("start_scan\r\n");

    fATWS(NULL);

    DBG_8195A("start_scan done\r\n");
    
    return;
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    DBG_8195A("start_scan_adv, %d\r\n", rtw_getFreeHeapSize());
    fATWS(NULL);        
    return;
}

static int power_off(hal_wifi_module_t *m)
{
    wifi_off();   
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    wifi_on(RTW_MODE_STA); 
    wifi_disable_powersave();      
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    wifi_set_channel(ch); 
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{
    DBG_8195A("start_monitor\r\n");
    
#if CONFIG_AUTO_RECONNECT
    wifi_set_autoreconnect(0);
#endif
    
    wifi_enter_promisc_mode();

    return;
}

static void stop_monitor(hal_wifi_module_t *m)
{
    DBG_8195A("stop_monitor\r\n");

    wifi_set_promisc(RTW_PROMISC_DISABLE, NULL, 0);

#if CONFIG_AUTO_RECONNECT
    wifi_set_autoreconnect(1);
#endif
   
    return;
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    wifi_off();
    rtw_mdelay_os(20);
    wifi_on(RTW_MODE_PROMISC);
    wifi_disable_powersave();   

    DBG_8195A("register_monitor_cb\r\n");
    wifi_set_promisc(RTW_PROMISC_ENABLE_2, fn, 0);
    return;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    DBG_8195A("register_wlan_mgnt_monitor_cb done\r\n");

    return;
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    int ret = 0;
    const char *ifname = WLAN0_NAME;
    
    DBG_8195A("wlan_send_80211_raw_frame done\r\n");
   
    ret = wext_send_mgnt(ifname, (char*)buf, len, 1);
    return 0;
}

void NetCallback(hal_wifi_ip_stat_t *pnet)
{
    if (rtl8710bn_wifi_module.ev_cb == NULL)
        return;
    if (rtl8710bn_wifi_module.ev_cb->ip_got == NULL)
        return;

    rtl8710bn_wifi_module.ev_cb->ip_got(&rtl8710bn_wifi_module, pnet, NULL);
}

void connected_ap_info(hal_wifi_ap_info_adv_t *ap_info, char *key, int key_len)
{
    if (rtl8710bn_wifi_module.ev_cb == NULL)
        return;
    if (rtl8710bn_wifi_module.ev_cb->para_chg == NULL)
        return;

    rtl8710bn_wifi_module.ev_cb->para_chg(&rtl8710bn_wifi_module, ap_info, key, key_len, NULL);
}

void WifiStatusHandler(int status)
{
    if (rtl8710bn_wifi_module.ev_cb == NULL)
        return;
    if (rtl8710bn_wifi_module.ev_cb->stat_chg == NULL)
        return;

    rtl8710bn_wifi_module.ev_cb->stat_chg(&rtl8710bn_wifi_module, (hal_wifi_event_t)status, NULL);
}

void ApListCallback(hal_wifi_scan_result_t *pApList)
{
    int i;
    
    DBG_8195A("AP %d: \r\n", pApList->ap_num);
    for(i=0; i<pApList->ap_num; i++) {
        DBG_8195A("\t %s rssi %d\r\n", pApList->ap_list[i].ssid, pApList->ap_list[i].ap_power);
    }
    if (rtl8710bn_wifi_module.ev_cb == NULL)
        return;
    if (rtl8710bn_wifi_module.ev_cb->scan_compeleted == NULL)
        return;

    rtl8710bn_wifi_module.ev_cb->scan_compeleted(&rtl8710bn_wifi_module, 
        (hal_wifi_scan_result_t*)pApList, NULL);
}

void ApListAdvCallback(hal_wifi_scan_result_adv_t *pApAdvList)
{
    if (rtl8710bn_wifi_module.ev_cb == NULL)
        return;
    if (rtl8710bn_wifi_module.ev_cb->scan_adv_compeleted == NULL)
        return;

    rtl8710bn_wifi_module.ev_cb->scan_adv_compeleted(&rtl8710bn_wifi_module, 
        pApAdvList, NULL);
}

hal_wifi_module_t rtl8710bn_wifi_module = {
    .base.name           = "rtl8710bn_wifi_module",
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
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};
