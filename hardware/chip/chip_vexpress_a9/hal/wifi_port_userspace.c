/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <network/network.h>
#include <hal2vfs/io_wifi.h>
#include <drivers/u_ld.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <aos/hal/wifi.h>
#include "aos/vfs.h"
#include "fcntl.h"

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
    return NULL;
}

static void on_wifi_connect(int event_id, void *arg)
{

}

static void on_wifi_disconnect(int event_id, void *arg)
{
}

static void on_fatal_error(int event_id, void *arg)
{
}

static int haas1000_wifi_init(netdev_t *dev)
{
    printf("wifi init success!!\n");
    return 0;
};

static int haas1000_wifi_deinit(netdev_t *dev)
{
    return 0;
}

static int haas1000_wifi_get_mac_addr(netdev_t *dev, uint8_t *mac)
{
    return 0;
};

static int haas1000_wifi_set_mac_addr(netdev_t *dev, const uint8_t *mac)
{
    printf("WiFi HAL %s not implemeted yet!\n", __func__);
    return -1;
};

static void wifi_connect_task(void *arg)
{
}

//__SRAMDATA osThreadDef(wifi_connect_task, osPriorityNormal, 1, (4096), "wifi_connect");
static int haas1000_wifi_connect(netdev_t *dev, wifi_config_t* config)
{
    return 0;
}

static int haas1000_wifi_notify_ip_state2drv(netdev_t *dev,
                       wifi_ip_stat_t *in_net_para,
                       wifi_mode_t mode)
{
    return -1;
}

static int get_ip_stat(netdev_t *dev,
                       wifi_ip_stat_t *out_net_para,
                       wifi_mode_t mode)
{
    return -1;
}

static int haas1000_wifi_sta_get_link_status(netdev_t *dev, wifi_ap_record_t *out_stat)
{
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
    return 0;
}

static int haas1000_wifi_start_scan(netdev_t *dev, wifi_scan_config_t* config, bool block)
{
    return 1;
}

static int haas1000_wifi_start_specified_scan(netdev_t *dev, ap_list_t *ap_list, int ap_num)
{
    return 1;
}

static int disconnect_station(netdev_t *dev)
{
    return 0;
}

static int disconnect_soft_ap(netdev_t *dev)
{
    return 0;
}

static int haas1000_wifi_disconnect(netdev_t *dev)
{
    return 1;
}

static int haas1000_wifi_cancel_connect(netdev_t *dev)
{
    return -1;
}

static int haas1000_wifi_set_channel(netdev_t *dev, int ch)
{
    return 1;
}

static int haas1000_wifi_get_channel(netdev_t *dev, int* ch)
{
    return -1;
}

/* Wi-Fi Smart Config */

static int wifi_sniffer_handler(unsigned short data_len, void *data)
{
    return 0;
}

static int haas1000_wifi_start_monitor(netdev_t *dev)
{
    return 0;
}

static int haas1000_wifi_stop_monitor(netdev_t *dev)
{
    return 0;
}

static void haas1000_wifi_register_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
    
}

/* Wi-Fi Zero Config */

static int haas1000_wifi_start_mgnt_monitor(netdev_t *dev)
{
    return 0;
}

static int haas1000_wifi_stop_mgnt_monitor(netdev_t *dev)
{
    return 1;
}

static void haas1000_wifi_register_mgnt_monitor_cb(netdev_t *dev, monitor_data_cb_t fn)
{
}

static int haas1000_wifi_send_80211_raw_frame(netdev_t *dev, uint8_t *buf, int len)
{
    return 0;
}

static int start_ap(netdev_t *dev,
                    const char *ssid,
                    const char *passwd, int interval, int hide)
{
    return 0;
}

static int stop_ap(netdev_t *dev)
{
    return 0;
}

static wifi_driver_t haas1000_wifi_driver = {

    /** driver info */
    .base.os                    = "rtos",
    .base.type                  = "solo",
    .base.partner               = "AliOS Things Team",
    .base.app_net               = "rtsp+rtp+rtcp",
    .base.project               = "HAAS100-vexpress",
    .base.cloud                 = "aliyun",

    /** common APIs */
    .init                       = haas1000_wifi_init,
    .deinit                     = haas1000_wifi_deinit,

    /** conf APIs */
    .set_mac_addr               = haas1000_wifi_set_mac_addr,
    .get_mac_addr               = haas1000_wifi_get_mac_addr,
    .notify_ip_state2drv        = haas1000_wifi_notify_ip_state2drv,

    /** connection APIs */
    .start_scan                 = haas1000_wifi_start_scan,
    .start_specified_scan       = haas1000_wifi_start_specified_scan,
    .connect                    = haas1000_wifi_connect,
    .cancel_connect             = haas1000_wifi_cancel_connect,
    .disconnect                 = haas1000_wifi_disconnect,
    .sta_get_link_status        = haas1000_wifi_sta_get_link_status,

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

int aos_wifi_register(void)
{
    printf("%s-%d called\n", __FUNCTION__, __LINE__);

    vfs_wifi_dev_register(&haas1000_wifi_driver, 0);
    return 0;
}

VFS_DRIVER_ENTRY(aos_wifi_register)
