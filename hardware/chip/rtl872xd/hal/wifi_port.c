#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/hal/wifi.h"

#include <platform/platform_stdlib.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include <wifi/wifi_ind.h>
#include <lwip_netconf.h>
#include <osdep_service.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <devicevfs/devicevfs.h>
#include <lwip/inet.h>

#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM]; 
#endif

monitor_data_cb_t g_promisc_data_callback = NULL;  
monitor_data_cb_t g_mgnt_filter_callback = NULL;
static aos_sem_t scan_sem;

int haas200_wifi_init(netdev_t *dev)
{
#if CONFIG_LWIP_LAYER
    LwIP_Init();
#endif
    wifi_on(RTW_MODE_STA);
    wifi_disable_powersave();

    if (0 != aos_sem_new(&scan_sem, 0))
    {
        return -1;
    }
    return 0;
}

int haas200_wifi_deinit(netdev_t *dev)
{
    return wifi_off();
}

int haas200_wifi_set_mode(netdev_t *dev, wifi_mode_t mode)
{
    return wifi_set_mode(mode);
}

int haas200_wifi_get_mode(netdev_t *dev, wifi_mode_t *devode)
{
    return wext_get_mode(WLAN0_NAME, devode);
}

int haas200_wifi_install_event_cb(netdev_t *dev, wifi_event_func *evt_cb)
{
    return 0;
}

int haas200_wifi_cancel_connect(netdev_t *dev)
{
    int ret = 0;
    if((ret = wifi_disconnect()) < 0) {
        printf("\n\rERROR: Operation failed!");
        return ret;
    }
    return ret;
}

int haas200_wifi_set_mac_addr(netdev_t *dev, const uint8_t *devac)
{
    return wifi_set_mac_address(devac);
}

int haas200_wifi_get_mac_addr(netdev_t *dev, uint8_t *devac)
{
    char buf[32];
    uint32_t mac[ETH_ALEN];
    int i = 0;

    wifi_get_mac_address(buf);
    sscanf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

    for(i = 0; i < ETH_ALEN; i++)
        devac[i] = mac[i] & 0xFF;

    return 0;
}

int haas200_wifi_set_lpm(netdev_t *dev, wifi_lpm_mode_t mode)
{
    wext_set_powersave_mode(mode);
    return 0;
}

int haas200_wifi_get_lpm(netdev_t *dev, wifi_lpm_mode_t *mode)
{
    wext_get_powersave_mode(mode);
    return 0;
}

int haas200_wifi_notify_ip_state2drv(netdev_t *dev, wifi_ip_stat_t *out_net_para, wifi_mode_t wifi_type)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    if (!out_net_para) {
        printf("%s: invalid argument!\n", __func__);
        return -1;
    }

    if (wifi_type == RTW_MODE_STA) {
        printf("set ip: %s\n", out_net_para->ip);
        printf("set mask: %s\n", out_net_para->mask);
        printf("set gw: %s\n", out_net_para->gate);
        inet_aton(out_net_para->ip, &ipaddr);
        inet_aton(out_net_para->mask, &netmask);
        inet_aton(out_net_para->gate, &gw);
        netif_set_addr(&xnetif[0], &ipaddr, &netmask, &gw);
        return 0;
    }

    return -1;
}

static void print_scan_result( rtw_scan_result_t* record )
{
    printf("%s\t ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra");
    printf(MAC_FMT, MAC_ARG(record->BSSID.octet));
    printf(" %d\t ", record->signal_strength);
    printf(" %d\t  ", record->channel);
    printf(" %d\t  ", record->wps_type);
    printf("%s\t\t ", ( record->security == RTW_SECURITY_OPEN ) ? "Open" :
                                 ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
                                 ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
                                 ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
                                 ( record->security == RTW_SECURITY_WPA_MIXED_PSK ) ? "WPA Mixed" :
                                 ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
                                 ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
                                 ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
#ifdef CONFIG_SAE_SUPPORT
                                 ( record->security == RTW_SECURITY_WPA3_AES_PSK) ? "WP3-SAE AES" :
#endif
                                 "Unknown" );

    printf(" %s ", record->SSID.val);
    printf("\r\n");
}

static int scan_result_handler(rtw_scan_handler_result_t *malloced_scan_result)
{
    static int ApNum = 0;
    if (malloced_scan_result->scan_complete != RTW_TRUE){
        rtw_scan_result_t* record = &malloced_scan_result->ap_details;
        record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */
        printf("%d\t ", ++ApNum);
        print_scan_result(record);
        if(malloced_scan_result->user_data)
            rtw_memcpy((void *)((char *)malloced_scan_result->user_data + (ApNum-1)*sizeof(rtw_scan_result_t)),(char *)record,sizeof(rtw_scan_result_t));
    } else {
        int i = 0;
        static wifi_scan_result_t result = {0};

        if(ApNum == 0)
            goto end;

        result.ap_list = (ap_list_t *)malloc(ApNum * sizeof(ap_list_t));
        if(result.ap_list == NULL) {
            printf("ERROR: malloc failed!\n");
            goto end;
        }
        for(i = 0; i < ApNum; i++) {
            rtw_scan_result_t *res =  malloced_scan_result->user_data + i*sizeof(rtw_scan_result_t);
            rtw_memcpy((result.ap_list+i)->ssid,res->SSID.val,res->SSID.len);
            rtw_memcpy((result.ap_list+i)->bssid,res->BSSID.octet,6);
            (result.ap_list+i)->ap_power = res->signal_strength;
            (result.ap_list+i)->channel = res->channel;
            (result.ap_list+i)->sec_type = res->security;
        }
        result.ap_num = ApNum;
end:
        ApNum = 0;
        event_publish(EVENT_WIFI_SCAN_DONE,&result);
        if(malloced_scan_result->user_data)
            rtw_free(malloced_scan_result->user_data);
    }
    return 0;
}

int haas200_wifi_start_scan(netdev_t *dev, wifi_scan_config_t *config, bool block)
{
    printf("%s\n",__func__);
    char *scan_buf;

    scan_buf = rtw_malloc(65 * sizeof(rtw_scan_result_t));
    if(!scan_buf) {
        printf("ERROR: malloc failed!\n");
        return -1;
    }
    rtw_memset(scan_buf, 0, 65*sizeof(rtw_scan_result_t));
    if(wifi_scan_networks(scan_result_handler, scan_buf) != 0) {
        printf("ERROR: wifi scan failed!\n");
        return -1;
    }

    return 0;
}

/*find ap with "ssid" from scan list*/
static int _find_ap_from_scan_buf(char*buf, int buflen, char *target_ssid, void *user_data)
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
            //DBG_8195A("find a ap \r\n");
            strncpy((char*)pwifi->ssid, target_ssid, 33);
            // channel offset = 13
            pwifi->channel = *(buf + plen + 13);
            // security_mode offset = 11
            security_mode = (u8)*(buf + plen + 11);
            switch (security_mode)
            {
            case IW_ENCODE_ALG_NONE:
                pwifi->security_type = RTW_SECURITY_OPEN;
                break;
            case IW_ENCODE_ALG_WEP:
                pwifi->security_type = RTW_SECURITY_WEP_PSK;
                break;
            case IW_ENCODE_ALG_CCMP:
                pwifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
                break;
            }

            if(IW_ENCODE_ALG_WEP != security_mode)
                break;
        }
        plen += len;
    }
    return 0;
}

/*get ap security mode from scan list*/
static int _get_ap_security_mode(IN char * ssid, OUT rtw_security_t *security_mode, OUT u8 * channel)
{
    rtw_wifi_setting_t wifi;
    u32 scan_buflen = 1000;

    memset(&wifi, 0, sizeof(wifi));

    if(wifi_scan_networks_with_ssid(_find_ap_from_scan_buf, (void*)&wifi, scan_buflen, ssid, strlen(ssid)) != RTW_SUCCESS){
        printf("ERROR: wifi scan failed!\n");
        return 0;
    }

    if(strcmp(wifi.ssid, ssid) == 0){
        //printf("Wifi scanned !\n");
        *security_mode = wifi.security_type;
        *channel = wifi.channel;
        return 1;
    }
    
    return 0;
}

int haas200_wifi_connect(netdev_t *dev, wifi_config_t *config)
{
    int ret = -1;
    int key_id = -1;
    uint8_t ssid[MAX_SSID_SIZE + 1] = {0};
    uint8_t password[MAX_PASSWD_SIZE + 1] = {0};
    uint32_t ssid_len, passwd_len;
    uint8_t channel;
    uint8_t pscan_config = (PSCAN_ENABLE | PSCAN_FAST_SURVEY | PSCAN_SIMPLE_CONFIG);
    rtw_security_t security_type = RTW_SECURITY_UNKNOWN;
    
    if(!config)
        return -1;
    
    if(config->mode != RTW_MODE_STA)
        return -1;

#if CONFIG_AUTO_RECONNECT
    wifi_set_autoreconnect(0);
#endif

    ssid_len = strlen(config->ssid);
    passwd_len = strlen(config->password);
    channel = config->sta_config.channel;
    rtw_memcpy(ssid, config->ssid, ssid_len);
    
    if(passwd_len == 0)
        security_type = RTW_SECURITY_OPEN;
    else {
        rtw_memcpy(password, config->password, passwd_len);
        //security_type = RTW_SECURITY_WPA2_AES_PSK;    
    }
    
    if(security_type == RTW_SECURITY_UNKNOWN) {
        int security_retry_count = 0;
        while (1) {
            if (_get_ap_security_mode((char*)ssid, &security_type, &channel))
                break;
            security_retry_count++;
            if(security_retry_count >= 3){
                printf("Can't get AP security mode and channel.\n");
                printf("Warning : unknow security type, default set to WPA2_AES\r\n");
                security_type = RTW_SECURITY_WPA2_AES_PSK;                
                break;
            }
        }
        wifi_set_pscan_chan(&channel, &pscan_config, 1);
    }

    if (security_type == RTW_SECURITY_WEP_PSK || security_type ==RTW_SECURITY_WEP_SHARED) 
        key_id = 0;
    
    printf("\nconnect to ssid:%s,password:%s\n",ssid,password);
    ret = wifi_connect(ssid, security_type, password, ssid_len, passwd_len, key_id, NULL);
    if(ret != 0) {
        if(ret == RTW_INVALID_KEY)
            printf("\n\rERROR:Invalid Key\n");        
        printf("\n\rERROR: Can't connect to AP,ret:%d\n",ret);
        return ret;
    }
    printf("\r\nWifi_connect successed\n\r");
    event_publish(EVENT_WIFI_CONNECTED, &xnetif[0]);

    return 0;
}

int haas200_wifi_disconnect(netdev_t *dev)
{
    char essid[MAX_SSID_SIZE + 1];
    int ret = 0, timeout = 20;
    
    if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
        printf("\n\rnot connected yet");
        return ret;
    }
    printf("\n\rDeassociating AP...\n\r");
    if((ret = wifi_disconnect()) < 0) {
        printf("\n\rERROR: Operation failed!\n");
        return ret;
    }
    
    while(1) {
        if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
            printf("\n\rWIFI disconnect succeed\n");
            break;
        }
        
        if(timeout == 0) {
            printf("\n\rERROR: Deassoc timeout!\n");
            ret = -1;
            break;
        }
        aos_msleep(1);
        timeout--;
    }
#if CONFIG_LWIP_LAYER
    LwIP_ReleaseIP(WLAN0_IDX);
#endif
    event_publish(EVENT_WIFI_DISCONNECTED, NULL);
    return ret;
}

int haas200_wifi_sta_get_link_status(netdev_t *dev, wifi_ap_record_t *ap_info)
{
    rtw_wifi_setting_t setting;
    int rssi;
    wifi_get_setting(WLAN0_NAME,&setting);
    wifi_get_rssi(&rssi);
    
    ap_info->link_status = setting.mode;
    rtw_memcpy(ap_info->ssid, setting.ssid, strlen(setting.ssid));
    ap_info->channel = setting.channel;
    ap_info->rssi = (int8_t)rssi;
    ap_info->encryptmode = setting.security_type;
    
    switch(setting.security_type){
    case RTW_SECURITY_OPEN:
        ap_info->authmode = WIFI_AUTH_OPEN;
        break;
    case RTW_SECURITY_WEP_PSK:
    case RTW_SECURITY_WEP_SHARED:
        ap_info->authmode = WIFI_AUTH_WEP;
        break;
    case RTW_SECURITY_WPA_TKIP_PSK:
    case RTW_SECURITY_WPA_AES_PSK:
        ap_info->authmode = WIFI_AUTH_WPA_PSK;
        break;
    case RTW_SECURITY_WPA2_AES_PSK:
    case RTW_SECURITY_WPA2_TKIP_PSK:
    case RTW_SECURITY_WPA2_MIXED_PSK:
        ap_info->authmode = WIFI_AUTH_WPA2_PSK;
        break;
    case RTW_SECURITY_WPA_WPA2_MIXED_PSK:
        ap_info->authmode = WIFI_AUTH_MAX;
        break;
    default:
        printf("unknow security mode!\n");
        break;
    }
    
    return 0;
}

int haas200_wifi_ap_get_sta_list(netdev_t *dev, wifi_sta_list_t *sta)
{
    return wifi_get_associated_client_list(sta,sizeof(sta));
}

// callback for promisc packets, like rtk_start_parse_packet in SC, wf, 1021
static void wifi_promisc_rx_cb(unsigned char* buf, unsigned int len, void* user_data)
{
    wifi_link_info_t link_info;
    
    link_info.rssi = ((ieee80211_frame_info_t *)user_data)->rssi;

    if(g_promisc_data_callback) {
        g_promisc_data_callback(buf, len, &link_info);
    }
}

int haas200_wifi_start_monitor(netdev_t *dev, wifi_promiscuous_cb_t cb)
{
    printf("%s\n",__func__);
#if CONFIG_AUTO_RECONNECT
    wifi_set_autoreconnect(RTW_AUTORECONNECT_DISABLE);
#endif
    wifi_init_packet_filter();
    wifi_enter_promisc_mode();
    wifi_set_promisc(RTW_PROMISC_ENABLE_2, wifi_promisc_rx_cb, 0);
    return 0;
}

int haas200_wifi_stop_monitor(netdev_t *dev)
{
    printf("%s\n",__func__);
    g_promisc_data_callback = NULL;
    wifi_set_promisc(RTW_PROMISC_DISABLE, NULL, 0);
    return 0;
}

int haas200_wifi_send_80211_raw_frame(netdev_t *dev, void *buffer, uint16_t len)
{
    int ret = -2;
    len = len - 4; /* remove fcs */
    ret = wext_send_mgnt(WLAN0_NAME, (char*)buffer, len, 1);    
    return ret;
}

int haas200_wifi_get_channel(netdev_t *dev, int *channel)
{
    return wifi_get_channel(channel);
}

int haas200_wifi_set_channel(netdev_t *dev, int channel)
{
    return wifi_set_channel(channel);
}

static int specified_scan_result_handler(char*buf, int buflen, char *target_ssid, void *user_data)
{
    ap_list_t *ap_list = (ap_list_t *)user_data;
    int plen = 0;
    
    while(plen < buflen) {
        int len, rssi, ssid_len, security_mode, i;
        char *ssid, *mac;
        printf("\n\r");
        /* len offset = 0 */
        len = (int)*(buf + plen);
        /* check end */
        if(len == 0) break;
        /* mac */
        mac = buf + plen + 1;
        strncpy(ap_list->bssid,mac,6);
        printf("mac = ");
        for(i=0; i<6; i++)
            printf("%02x ", (u8)*(mac+i));
        printf(",\t");
        /* rssi */
        rssi = *(int *)(buf + plen + 7);
        ap_list->ap_power = rssi;
        printf("rssi = %d,\t",rssi);
        /* ssid offset = 14 */
        ssid_len = len - 14;
        ssid = buf + plen + 14 ;
    
        if((ssid_len == strlen(target_ssid))
            && (!memcmp(ssid, target_ssid, ssid_len)))
        {
            strncpy((char*)ap_list->ssid, target_ssid, 33);
            printf("ssid = ");
            for(i = 0; i < ssid_len; i++)
                printf("%c", *(ssid+i));
            printf(",\t");
            /* channel offset = 13 */
            ap_list->channel = *(buf + plen + 13);
            printf("channel = %d,\t",ap_list->channel);
            /* security_mode offset = 11 */
            security_mode = (u8)*(buf + plen + 11);
            switch(security_mode){
                case IW_ENCODE_ALG_NONE:
                    ap_list->sec_type = RTW_SECURITY_OPEN;
                    printf("sec = open");
                    break;
                case IW_ENCODE_ALG_WEP:
                    ap_list->sec_type = RTW_SECURITY_WEP_PSK;
                    printf("sec = wep");
                    break;
                case IW_ENCODE_ALG_CCMP:
                    ap_list->sec_type = RTW_SECURITY_WPA2_AES_PSK;
                    printf("sec = wpa/wpa2");
                    break;
            }
        }
        plen += len;
    }
    aos_sem_signal((aos_sem_t *)&scan_sem);
    return 0;
}

int haas200_wifi_start_specified_scan(netdev_t *dev, ap_list_t *ap_list, int ap_num)
{
    static wifi_scan_result_t result = {0};
    int i, scan_buf_len = 500;

    if(!ap_num || !ap_list) {
        printf("ERROR: error params\n");
        return -1;
    }

    if(result.ap_list == NULL) {
        result.ap_list = (ap_list_t *)malloc(ap_num * sizeof(ap_list_t));
    }

    if(!result.ap_list){
        printf("ERROR: malloc failed!\n");
        return -1;
    }

    result.ap_num = ap_num;
    for(i = 0; i< ap_num; i++) {
        ap_list_t ap = *(ap_list + i);
        if(wifi_scan_networks_with_ssid(specified_scan_result_handler, &ap, scan_buf_len, ap.ssid, strlen(ap.ssid)) != 0){
            printf("ERROR: wifi scan failed\n\r");
            return -1;
        }
        aos_sem_wait((aos_sem_t *)&scan_sem, AOS_WAIT_FOREVER);    
        *(ap_list + i) = ap;
        *(result.ap_list + i) = ap;
    }

end:
    event_publish(EVENT_WIFI_SCAN_DONE,&result);
    return 0;
}

int haas200_wifi_register_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
    if(!fn)
        return -1;
    g_promisc_data_callback = fn;
    return 0;
}

int haas200_wifi_start_mgnt_monitor(netdev_t *dev)
{
     wifi_set_indicate_mgnt(2);
    return 0;
}

int haas200_wifi_stop_mgnt_monitor(netdev_t *dev)
{
    g_mgnt_filter_callback = NULL;
    wifi_set_indicate_mgnt(0);
    return 0;
}

static void wifi_rx_mgnt_hdl(u8 *buf, int len, int flags, void *user_data)
{
    wifi_link_info_t link_info;
    /* only deal with Probe Request*/
    if(g_mgnt_filter_callback && buf[0] == 0x40)
        g_mgnt_filter_callback((u8*)buf, len, &link_info);
}

int haas200_wifi_register_mgnt_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
    if(!fn)
        return -1;
    
    g_mgnt_filter_callback = fn;
    wifi_reg_event_handler(WIFI_EVENT_RX_MGNT, wifi_rx_mgnt_hdl, NULL);
    return 0;
}

static wifi_driver_t haas200_wifi_driver = {

    /** driver info */
    .base.os                    = "rtos",
    .base.type                  = "solo",
    .base.partner               = "AliOS Things Team",
    .base.app_net               = "rtsp+rtp+rtcp",
    .base.project               = "HAAS200",
    .base.cloud                 = "aliyun",

    /** common APIs */
    .init                       = haas200_wifi_init,
    .deinit                     = haas200_wifi_deinit,
    .set_mode                   = haas200_wifi_set_mode,
    .get_mode                   = haas200_wifi_get_mode,
    
    .install_event_cb            = haas200_wifi_install_event_cb,

    /** conf APIs */
    .set_mac_addr               = haas200_wifi_set_mac_addr,
    .get_mac_addr               = haas200_wifi_get_mac_addr,
    .set_lpm                    = haas200_wifi_set_lpm,
    .get_lpm                    = haas200_wifi_get_lpm,
    .notify_ip_state2drv        = haas200_wifi_notify_ip_state2drv,

    /** connection APIs */
    .start_scan                 = haas200_wifi_start_scan,
    .start_specified_scan       = haas200_wifi_start_specified_scan,
    .connect                    = haas200_wifi_connect,
    .cancel_connect             = haas200_wifi_cancel_connect,
    .disconnect                 = haas200_wifi_disconnect,
    .sta_get_link_status        = haas200_wifi_sta_get_link_status,
    .ap_get_sta_list            = haas200_wifi_ap_get_sta_list,


    /** promiscuous APIs */
    .start_monitor              = haas200_wifi_start_monitor,
    .stop_monitor               = haas200_wifi_stop_monitor,
    .send_80211_raw_frame       = haas200_wifi_send_80211_raw_frame,
    .set_channel                = haas200_wifi_set_channel,
    .get_channel                = haas200_wifi_get_channel,
    .register_monitor_cb        = haas200_wifi_register_monitor_cb,
    .start_mgnt_monitor         = haas200_wifi_start_mgnt_monitor,
    .stop_mgnt_monitor          = haas200_wifi_stop_mgnt_monitor,
    .register_mgnt_monitor_cb   = haas200_wifi_register_mgnt_monitor_cb,

    .set_smartcfg = NULL,
};

int haas200_wifi_register(void)
{
    vfs_wifi_dev_register(&haas200_wifi_driver,0);
    return 0;
}

VFS_DRIVER_ENTRY(haas200_wifi_register);