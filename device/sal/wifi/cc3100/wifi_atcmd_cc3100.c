/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include "netcfg.h"
#include "BCDS_NetworkConfig.h"
#include "NetworkConfig.h"

#define TAG "wifi_port"

#define AT_RSP_SUCCESS "OK"
#define AT_RSP_FAIL "ERROR"

static int get_mac_helper(char *mac);
static int get_ip_stat_helper(hal_wifi_ip_stat_t *result);

static void fetch_ip_stat(void *arg)
{
    hal_wifi_ip_stat_t ip_stat = {0};
    hal_wifi_module_t *m;

    if (!arg) {
        LOGE(TAG, "%s failed, invalid argument\r\n", __func__);
        return;
    }

    m = (hal_wifi_module_t *)arg;

    get_ip_stat_helper(&ip_stat);

    if (m->ev_cb->ip_got != NULL) {
        m->ev_cb->ip_got(m, &ip_stat, NULL);
    }
}

void at_wevent_handler(void *arg, char *buf, int buflen)
{
    hal_wifi_module_t *m;

    if (NULL == arg) {
        m = hal_wifi_get_default_module();
    } else {
        m = (hal_wifi_module_t *)arg;
    }

    if (NULL == m) {
        return;
    }

    if (m->ev_cb->stat_chg != NULL) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
    }

    fetch_ip_stat(m);
}



#define MAC_STR_LEN 12
// str: char[12], mac: hex[6]
static void mac_str_to_hex(char *str, uint8_t *mac)
{
    int i;
    char c;
    uint8_t j;

    if (!str || !mac) {
        return;
    }

    memset(mac, 0, MAC_STR_LEN >> 1);
    for (i = 0; i < MAC_STR_LEN; i++) {
        c = str[i];
        if (c >= '0' && c <= '9') {
            j = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            j = c - 'A' + 0xa;
        } else if (c >= 'a' && c <= 'f') {
            j = c - 'a' + 0xa;
        } else {
            j = c % 0xf;
        }
        j <<= i & 1 ? 0 : 4;
        mac[i >> 1] |= j;
    }
}



////////////////////////////////////////////
static int get_mac_helper(char *mac)
{

    if (!mac) {
        return -1;
    }

    int16_t sRetCode;
    uint8_t ucMacAddressVal[128] = {0};
    uint16_t usMacAddressLen = 256;
    uint16_t usConfigOpt = 0;

    sRetCode = sl_NetCfgGet( SL_MAC_ADDRESS_GET,
                             &usConfigOpt,
                             &usMacAddressLen,
                             ( uint8_t * ) ucMacAddressVal );

    sscanf(ucMacAddressVal, "%*[^:]:%[^\r]", mac);
    LOGI(TAG, "mac result: %s\r\n", mac);

    return 0;
}

static int get_ip_stat_helper(hal_wifi_ip_stat_t *result)
{
    char atheader[64] = {0};
    char out[128] = {0};
    int ret = 0;

    if (!result) {
        return -1;
    }

    _u16 len = sizeof(SlNetCfgIpV4Args_t);
    _u16 ConfigOpt = 0;   //return value could be one of the following: SL_NETCFG_ADDR_DHCP / SL_NETCFG_ADDR_DHCP_LLA / SL_NETCFG_ADDR_STATIC
    SlNetCfgIpV4Args_t ipV4 = {0};
    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO,&ConfigOpt,&len,(_u8 *)&ipV4);
                                      
    LOGD(TAG,"DHCP is %s IP %d.%d.%d.%d MASK %d.%d.%d.%d GW %d.%d.%d.%d DNS %d.%d.%d.%d\n",
        (ConfigOpt == SL_IPV4_DHCP_CLIENT) ? "ON" : "OFF",                                                           
        SL_IPV4_BYTE(ipV4.ipV4,3),SL_IPV4_BYTE(ipV4.ipV4,2),SL_IPV4_BYTE(ipV4.ipV4,1),SL_IPV4_BYTE(ipV4.ipV4,0), 
        SL_IPV4_BYTE(ipV4.ipV4Mask,3),SL_IPV4_BYTE(ipV4.ipV4Mask,2),SL_IPV4_BYTE(ipV4.ipV4Mask,1),SL_IPV4_BYTE(ipV4.ipV4Mask,0),         
        SL_IPV4_BYTE(ipV4.ipV4Gateway,3),SL_IPV4_BYTE(ipV4.ipV4Gateway,2),SL_IPV4_BYTE(ipV4.ipV4Gateway,1),SL_IPV4_BYTE(ipV4.ipV4Gateway,0),                 
        SL_IPV4_BYTE(ipV4.ipV4DnsServer,3),SL_IPV4_BYTE(ipV4.ipV4DnsServer,2),SL_IPV4_BYTE(ipV4.ipV4DnsServer,1),SL_IPV4_BYTE(ipV4.ipV4DnsServer,0));
    sprintf(result->ip,"%d.%d.%d.%d",SL_IPV4_BYTE(ipV4.ipV4,3),SL_IPV4_BYTE(ipV4.ipV4,2),SL_IPV4_BYTE(ipV4.ipV4,1),SL_IPV4_BYTE(ipV4.ipV4,0));
    sprintf(result->mask,"%d.%d.%d.%d",SL_IPV4_BYTE(ipV4.ipV4Mask,3),SL_IPV4_BYTE(ipV4.ipV4Mask,2),SL_IPV4_BYTE(ipV4.ipV4Mask,1),SL_IPV4_BYTE(ipV4.ipV4Mask,0));
    sprintf(result->gate,"%d.%d.%d.%d",SL_IPV4_BYTE(ipV4.ipV4Gateway,2),SL_IPV4_BYTE(ipV4.ipV4Gateway,1),SL_IPV4_BYTE(ipV4.ipV4Gateway,0));
    sprintf(result->dns,"%d.%d.%d.%d",SL_IPV4_BYTE(ipV4.ipV4DnsServer,3),SL_IPV4_BYTE(ipV4.ipV4DnsServer,2),SL_IPV4_BYTE(ipV4.ipV4DnsServer,1),SL_IPV4_BYTE(ipV4.ipV4DnsServer,0));
    if(ConfigOpt == SL_IPV4_DHCP_CLIENT) 
        result->dhcp = 1;
    else
        result->dhcp = 0;
    LOGD(TAG, "result: %s %s %s %s\r\n",
         result->ip, result->mask, result->gate, result->dns);

    ret = get_mac_helper(result->mac);

    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    (void)wifi_type;
    (void)m;

    get_ip_stat_helper(out_net_para);

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;

    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{

}

static void start_scan_adv(hal_wifi_module_t *m)
{
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return 0;
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

static int wifi_init(hal_wifi_module_t *m)
{
    int retcode;
    retcode = WlanConnect_Init();
    if(retcode == 0)
    {
        LOGI(TAG, "wifi init success!!\n");
        return 0;
    }
    else
    {
        LOGI(TAG, "wifi init fail!!\n");
        return -1;
    }
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    char mac_str[MAC_STR_LEN + 1] = {0};

    (void)m;

    get_mac_helper(mac_str);

    LOGD(TAG, "wifi_get_mac_addr !!\n");

    mac_str_to_hex(mac_str, mac);
    LOGI(TAG, "mac in hex: %02x%02x%02x%02x%02x%02x",
         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
};


void myDhcpIpCallbackFunc(NetworkConfig_IpStatus_T returnStatus)
{
    /* set the DHCP flag*/
    if (NETWORKCONFIG_IPV4_ACQUIRED == returnStatus)
    {
        //dhcpFlag_mdu8 = NETWORKCONFIG_DHCP_FLG_ACQ;
        
        aos_post_event(EV_WIFI, CODE_WIFI_ON_PRE_GOT_IP, 0u);
        printf("[NCA] : Callback Function : IP was acquired using DHCP\n\r");
    }
    else
    {
        /* DHCP failed */
    }
}

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    char in[128] = {0};
    char out[128] = {0};

    (void)init_para;
    SlSecParams_t secParams;
    NetworkConfig_IpCallback_T myIpCallback;
    myIpCallback = myDhcpIpCallbackFunc;

    /* Set network parameters */
    secParams.Key = init_para->wifi_key;
    secParams.KeyLen = strlen((const char*) init_para->wifi_key);
    secParams.Type = SL_SEC_TYPE_WPA;
    printf("sl_WlanConnect ssid:%s,pwd:%s\n",init_para->wifi_ssid,init_para->wifi_key);
    Retcode_T retcode = NetworkConfig_SetIpDhcp(myIpCallback);
    if (RETCODE_OK == retcode)
    {
        printf("[NCA] : IP will be set using DHCP with callback. Waiting for IP ...\n\r");
    }
    else
    {
        printf("[NCA] : NetworkConfig_SetIpDhcp API has failed\n\r");
    }
    int32_t slStatus = NETWORKCONFIG_SUCCESS;

    slStatus = sl_WlanConnect(init_para->wifi_ssid, strlen(((char*) init_para->wifi_ssid)), NULL, &secParams, NULL);
    printf("sl_WlanConnect  slStatus=%d\n",slStatus);
    if (0 == slStatus)
    {
        return 0;
    }
    else
    {
        return -1;
    }

}



hal_wifi_module_t aos_wifi_module_cc3100 = {
    .base.name           = "aos_wifi_module_cc3100",
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

