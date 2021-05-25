/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_WIFI_H
#define HAL_WIFI_H
//#include <drivers/u_ld.h>
#include <vfsdev/wifi_dev.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* system info */
    /* type, eg: rtos, linux */
    const char *os;
    /* type, eg: solo, sub, gw */
    const char *type;
    /* partner name, eg: tmall_genie */
    const char *partner;
    /* app protocol, eg: http, lwm2m */
    const char *app_net;
    /* project name, eg: tmall_genie*/
    const char *project;
    /* cloud name, eg: aliyun or others */
    const char *cloud;
} wifi_driver_info_t;

//27个HAL接口
typedef struct wifi_driver {
    wifi_driver_info_t base;

    /** common APIs */
    int (*init)(netdev_t *dev);
    int (*deinit)(netdev_t *dev);
    int (*set_mode)(netdev_t *dev, wifi_mode_t mode);
    int (*get_mode)(netdev_t *dev, wifi_mode_t *devode);
    int (*install_event_cb)(netdev_t *dev, wifi_event_func *evt_cb);
    int (*cancel_connect)(netdev_t *dev);

    /** conf APIs */
    int (*set_mac_addr)(netdev_t *dev, const uint8_t *devac);
    int (*get_mac_addr)(netdev_t *dev, uint8_t *devac);
    int (*set_lpm)(netdev_t *dev, wifi_lpm_mode_t mode); //ps on/pff
    int (*get_lpm)(netdev_t *dev, wifi_lpm_mode_t *mode);

    int (*notify_ip_state2drv)(netdev_t *dev, wifi_ip_stat_t *out_net_para, wifi_mode_t wifi_type);

    /** connection APIs */
    int (*start_scan)(netdev_t *dev, wifi_scan_config_t *config, bool block);
    int (*connect)(netdev_t *dev, wifi_config_t * config); //start ap or sta
    int (*disconnect)(netdev_t *dev);//stop ap or sta
    int (*sta_get_link_status)(netdev_t *dev, wifi_ap_record_t *ap_info);
    int (*ap_get_sta_list)(netdev_t *dev, wifi_sta_list_t *sta);


    /** promiscuous APIs */
    int (*start_monitor)(netdev_t *dev, wifi_promiscuous_cb_t cb);
    int (*stop_monitor)(netdev_t *dev);
    int (*send_80211_raw_frame)(netdev_t *dev, void *buffer, uint16_t len);
    int (*set_channel)(netdev_t *dev, int channel);
    int (*get_channel)(netdev_t *dev, int *channel);
    int (*start_specified_scan)(netdev_t *dev, ap_list_t *ap_list, int ap_num);
    int (*register_monitor_cb)(netdev_t *dev, monitor_data_cb_t fn);
    int (*start_mgnt_monitor)(netdev_t *dev);
    int (*stop_mgnt_monitor)(netdev_t *dev);
    int (*register_mgnt_monitor_cb)(netdev_t *dev, monitor_data_cb_t fn);

    /* esp8266 related API */
    int (*set_smartcfg)(netdev_t *dev, int enable);

} wifi_driver_t;

int vfs_wifi_dev_register(wifi_driver_t *ops, int id);

#ifdef __cplusplus
}
#endif

#endif


