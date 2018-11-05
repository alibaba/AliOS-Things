/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <aos/aos.h>
#include "m2m_wifi.h"
#include "os/include/net_init.h"

hal_wifi_module_t aos_wifi_module_wilc1000;

/** Temporary Wi-Fi Settings */
#define MAIN_WLAN_SSID        "mchp_demo" /* < Destination SSID */
#define MAIN_WLAN_AUTH        M2M_WIFI_SEC_WPA_PSK /* < Security manner */
#define MAIN_WLAN_PSK         "mchp5678" /* < Password for Destination SSID */
char g_ssid[32+1];
char g_wifikey[64+1];

static void wifi_cb(uint8 msg_type, void *msg)
{
	switch (msg_type) {
		case M2M_WIFI_RESP_CON_STATE_CHANGED : {
			tstrM2mWifiStateChanged *ctx = (tstrM2mWifiStateChanged*)msg;
			if (ctx->u8IfcId == STATION_INTERFACE) {
				if (ctx->u8CurrState == M2M_WIFI_CONNECTED) {
					printf("WiFi Connected\r\n");
					net_interface_up(0x1);
					m2m_wifi_request_dhcp_client_ex();
				}
				if(ctx->u8CurrState == M2M_WIFI_DISCONNECTED) {
					printf("WiFi Disconnected\r\n");
					printf("WiFi Reconnecting... ssid=%s, len=%d\r\n", g_ssid, strlen(g_ssid));
					//net_interface_down(0x1);      // To Do: Comment this because DHCP cannot run when WiFi reconnect 

                    if (strcmp(g_wifikey, "open") == 0) 
                    {
                        m2m_wifi_connect(g_ssid, strlen(g_ssid), M2M_WIFI_SEC_OPEN, g_wifikey, M2M_WIFI_CH_ALL);
                    }
                    else
                    {
                        m2m_wifi_connect(g_ssid, strlen(g_ssid), M2M_WIFI_SEC_WPA_PSK, g_wifikey, M2M_WIFI_CH_ALL);
                    }
		
				}
			}
		}
		break;
#if 1
		case NET_IF_REQ_DHCP_CONF : {
			tstrM2MIPConfig2 *strIpConfig = msg;
			uint16_t *a = (void *)strIpConfig->u8StaticIPv6;
			LOG("wifi_cb: STA M2M_WIFI_REQ_DHCP_CONF\r\n");
			LOG("wifi_cb: STA IPv4 addr: %d.%d.%d.%d\r\n", strIpConfig->u8StaticIP[0], strIpConfig->u8StaticIP[1],
			strIpConfig->u8StaticIP[2], strIpConfig->u8StaticIP[3]);
			//LOG("wifi_cb: STA IPv6 addr: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\r\n",
			//htons(a[0]), htons(a[1]), htons(a[2]), htons(a[3]),
			//htons(a[4]), htons(a[5]), htons(a[6]), htons(a[7]));

            hal_wifi_ip_stat_t ip_stat = {0};
            sprintf(ip_stat.ip, "%d.%d.%d.%d", strIpConfig->u8StaticIP[0], strIpConfig->u8StaticIP[1],
			strIpConfig->u8StaticIP[2], strIpConfig->u8StaticIP[3]);
            aos_wifi_module_wilc1000.ev_cb->ip_got(&aos_wifi_module_wilc1000, &ip_stat, NULL);

		}
		break;
#endif

		default:
		break;
	}
}



static int wifi_init(hal_wifi_module_t *m)
{
    tstrWifiInitParam param;
	memset(&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	os_m2m_wifi_init(&param);

    return 0;
};


// mac - hex[6]
static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{

};


static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    LOG("wifi_start() In, ssid=%s\r\n", init_para->wifi_ssid);
    if (init_para->wifi_mode == STATION)
    {
        if (strcmp(init_para->wifi_key, "open") == 0) 
        {
            m2m_wifi_connect(init_para->wifi_ssid, strlen(init_para->wifi_ssid), M2M_WIFI_SEC_OPEN, init_para->wifi_key, M2M_WIFI_CH_ALL);
            strcpy(g_ssid, init_para->wifi_ssid);
            strcpy(g_wifikey, init_para->wifi_key);
        }
        else
        {
            m2m_wifi_connect(init_para->wifi_ssid, strlen(init_para->wifi_ssid), M2M_WIFI_SEC_WPA_PSK, init_para->wifi_key, M2M_WIFI_CH_ALL);
            strcpy(g_ssid, init_para->wifi_ssid);
            strcpy(g_wifikey, init_para->wifi_key);
        }
    }

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
  
 
    return 0;
}



static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{

    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{

}

static void start_scan_adv(hal_wifi_module_t *m)
{
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
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
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{

}

static void stop_monitor(hal_wifi_module_t *m)
{

}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

hal_wifi_module_t aos_wifi_module_wilc1000 = {
    .base.name           = "aos_wifi_module_wilc1000",
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

