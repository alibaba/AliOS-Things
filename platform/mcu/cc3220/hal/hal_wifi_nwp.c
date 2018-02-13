#include <string.h>
#include <stdio.h>
#include <k_api.h>
#include "hal/wifi.h"

#define WIFI_CONNECT_TIMEOUT_MS 20000 //20s
#define WIFI_DHCP_RETRY_TIME 20
static int8_t _traceQcomApi = 1;

static volatile int32_t devId = 0;
static volatile int8_t _isConnected = 0;
static ksem_t wifi_event_sem;

   
#define UINT32_IPADDR_TO_CSV_BYTES(a) (((a) >> 24) & 0xFF), (((a) >> 16) & 0xFF), (((a) >> 8) & 0xFF), ((a)&0xFF)
static void parseIPv4(char *in, uint8_t *addr)
{
    int8_t i, j;
    for (i = 3; i >= 0; i--) {
        for (j = 0; j < 3; j++) {
            if(('0' <= in[j]) && (in[j] <= '9')) {
                addr[i] *= 10;
                addr[i] += (in[j] - 0x30);
            } else {
                break;
            }
        }
        in += (j + 1);
    }
}

static void printError(uint32_t value, const char *funcName)
{
    csp_printf("ERROR: %s() returned %d\r\n", funcName, value);
}

static void hal_wifi_onConnect(uint8_t event, uint8_t devId, char *bssid, uint8_t bssConn)
{
    switch (event)
    {
        case 1:
            if (devId == 0)
            {
                csp_printf("%s connected\r\n", bssConn ? "AP" : "CLIENT");
            }
            else
            {
                csp_printf("Connected\r\n");
            }
            _isConnected = 1;
            // NOTE that station is not fully connected until PEER_FIRST_NODE_JOIN_EVENT
            krhino_sem_give(&wifi_event_sem);
            break;
        case 0:
            _isConnected = 0;
            if (devId == 0)
            {
                csp_printf("%s disconnect\r\n", bssConn ? "AP" : "CLIENT");
            }
            krhino_sem_give(&wifi_event_sem);
            break;		
#if 0	
        case INVALID_PROFILE:
            // this event is used to indicate RSNA failure
            _isConnected = 0;
            csp_printf("4 way handshake failure for device=%d n", devId);
            break;
        case PEER_FIRST_NODE_JOIN_EVENT:
            // this event is used to RSNA success
            csp_printf("4 way handshake success for device=%d\r\n", devId);
            break;
#endif
        default:
            csp_printf("code %d\r\n");
    }
}

static void hal_dhcps_cb(void)
{
    csp_printf("DHCP Setup Event\r\n");
    krhino_sem_give(&wifi_event_sem);
}


static int wifi_init(hal_wifi_module_t *m)
{
    uint8_t status = 0;
    csp_printf("WIFI_Init\r\n");

    status = 0; //WIFISHIELD_Init();
    csp_printf("WIFISHIELD_Init return %d\r\n", status);
    if (status != 0) {
        return status;
    }

    status = 0; //wlan_driver_start();
    csp_printf("wlan_driver_start return %d\r\n", status);
    if (status != 0) {
        return status;
    }

    krhino_sem_create(&wifi_event_sem, "wifi_event_sem", 0);

    return status;
}

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    mac[0] = 0x84; mac[1] = 0x5D; mac[2] = 0xD7; mac[3] = 0x68; mac[4] = 0x3B; mac[5] = 0x91;    
    csp_printf("wifi_get_mac_addr\r\n");
}


static uint8_t wifi_start_station(hal_wifi_init_type_t *init_para)
{
    uint8_t status = 0;

    if (status != 0) {
        return status;
    }

    return status;
}

static uint8_t wifi_start_softAP(hal_wifi_init_type_t *init_para)
{
    uint8_t status = 0;

    if (status != 0) {
        return status;
    }

    return 0;
}

static int wifi_get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type);
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    uint8_t status = 0;
    csp_printf("wifi_start NOT Implemented");
    return status;
}

static int wifi_start_adv(hal_wifi_module_t *m,hal_wifi_init_type_adv_t *init_para_adv)
{
    csp_printf("wifi_start_adv\r\n");
    return 0;
}

static int wifi_get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    csp_printf("wifi_get_ip_stat NOT Implemented");
    return 0;
}

static int wifi_get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    csp_printf("wifi_get_link_stat\r\n");
    return 0;
}

static void wifi_start_scan(hal_wifi_module_t *m)
{
    csp_printf("wifi_start_scan NOT Implemented");
}

static void wifi_start_scan_adv(hal_wifi_module_t *m)
{
    csp_printf("wifi_start_scan_adv NOT Implemented");
}

static int wifi_power_off(hal_wifi_module_t *m)
{
    csp_printf("wifi_power_off NOT Implemented");
    return 0;
}

static int wifi_power_on(hal_wifi_module_t *m)
{
    csp_printf("wifi_power_on NOT Implemented");
    return 0;
}

static int wifi_suspend(hal_wifi_module_t *m)
{
    csp_printf("wifi_suspend NOT Implemented");
    return 0;
}

static int wifi_suspend_station(hal_wifi_module_t *m)
{
    csp_printf("wifi_suspend_station NOT Implemented");
    return 0;
}

static int wifi_suspend_soft_ap(hal_wifi_module_t *m)
{
    csp_printf("wifi_suspend_soft_ap NOT Implemented");
    return 0;
}

static int wifi_set_channel(hal_wifi_module_t *m, int ch)
{
    csp_printf("wifi_set_channel NOT Implemented");
    return 0;
}

static void wifi_start_monitor(hal_wifi_module_t *m)
{
    csp_printf("wifi_start_monitor NOT Implemented");
}

static void wifi_stop_monitor(hal_wifi_module_t *m)
{
    csp_printf("wifi_stop_monitor NOT Implemented");
}

static void wifi_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    csp_printf("wifi_register_monitor_cb NOT Implemented");
}

static void wifi_register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    csp_printf("wifi_register_wlan_mgnt_monitor_cb NOT Implemented");
}

static int wifi_wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    csp_printf("wifi_wlan_send_80211_raw_frame NOT Implemented");
    return 0;
}

hal_wifi_module_t cc3220s_wifi_nwp = {
    .base.name = "cc3220s_wifi_nwp",
    .ev_cb = NULL,
    .init = wifi_init,
    .get_mac_addr = wifi_get_mac_addr,
    .start = wifi_start,
    .start_adv = wifi_start_adv,
    .get_ip_stat = wifi_get_ip_stat,
    .get_link_stat = wifi_get_link_stat,
    .start_scan = wifi_start_scan,
    .start_scan_adv = wifi_start_scan_adv,
    .power_off = wifi_power_off,
    .power_on = wifi_power_on,
    .suspend = wifi_suspend,
    .suspend_station = wifi_suspend_station,
    .suspend_soft_ap = wifi_suspend_soft_ap,
    .set_channel = wifi_set_channel,
    .start_monitor = wifi_start_monitor,
    .stop_monitor = wifi_stop_monitor,
    .register_monitor_cb = wifi_register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = wifi_register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wifi_wlan_send_80211_raw_frame
};


