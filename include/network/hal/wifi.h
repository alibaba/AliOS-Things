/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_WIFI_H
#define HAL_WIFI_H

#include <stdint.h>
#include <hal/base.h>
#ifdef STM32L475xx
#include "stm32_wifi.h"
#endif

typedef struct hal_wifi_module_s hal_wifi_module_t;

enum wlan_sec_type_e {
    SECURITY_TYPE_NONE,       /* Open system. */
    SECURITY_TYPE_WEP,        /* Wired Equivalent Privacy. WEP security. */
    SECURITY_TYPE_WPA_TKIP,   /* WPA /w TKIP */
    SECURITY_TYPE_WPA_AES,    /* WPA /w AES */
    SECURITY_TYPE_WPA2_TKIP,  /* WPA2 /w TKIP */
    SECURITY_TYPE_WPA2_AES,   /* WPA2 /w AES */
    SECURITY_TYPE_WPA2_MIXED, /* WPA2 /w AES or TKIP */
    SECURITY_TYPE_AUTO,       /* It is used when calling @ref micoWlanStartAdv, MICO read security type from scan result. */
};

typedef struct {
    char ssid[32 + 1];
    char ap_power;
} ap_list_t;

/*
 *  Scan result using normal scan.
 */
typedef struct {
    char ap_num;       /* The number of access points found in scanning. */
    ap_list_t *ap_list;
} hal_wifi_scan_result_t;

typedef struct {
    char ssid[32 + 1];  /* The SSID of an access point. */
    char ap_power;    /* Signal strength, min:0, max:100 */
    char bssid[6];    /* The BSSID of an access point. */
    char channel;     /* The RF frequency, 1-13 */
    uint8_t security; /* Security type, @ref wlan_sec_type_t */
} ap_list_adv_t;

typedef struct {
    char ap_num;          /* The number of access points found in scanning. */
    ap_list_adv_t *ap_list;
} hal_wifi_scan_result_adv_t;

typedef enum {
    NOTIFY_STATION_UP = 1,
    NOTIFY_STATION_DOWN,

    NOTIFY_AP_UP,
    NOTIFY_AP_DOWN,
} hal_wifi_event_t;


typedef struct {
    char    ssid[32 + 1]; /* SSID of the wlan that needs to be connected. Example: "SSID String". */
    char    bssid[6]; /* BSSID of the wlan needs to be connected. Example: {0xC8 0x93 0x46 0x11 0x22 0x33}. */
    uint8_t channel;  /* Wlan's RF frequency, channel 0-13. 1-13 means a fixed channelthat can speed up a connection procedure,
                         0 is not a fixed input means all channels are possible*/
    uint8_t security;
} hal_wifi_ap_info_adv_t;

typedef struct {
    char wifi_mode;              /* DHCP mode: @ref wlanInterfaceTypedef. */
    char wifi_ssid[32 + 1];          /* SSID of the wlan needs to be connected. */
    char wifi_key[64 + 1];           /* Security key of the wlan needs to be connected, ignored in an open system. */
    char local_ip_addr[16];      /* Static IP configuration, Local IP address. */
    char net_mask[16];           /* Static IP configuration, Netmask. */
    char gateway_ip_addr[16];    /* Static IP configuration, Router IP address. */
    char dns_server_ip_addr[16]; /* Static IP configuration, DNS server IP address. */
    char dhcp_mode;              /* DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
    char reserved[32];
    int  wifi_retry_interval;    /* Retry interval if an error is occured when connecting an access point,
                                    time unit is millisecond. */
#ifdef STM32L475xx
    WIFI_Ecn_t access_sec;
#endif
} hal_wifi_init_type_t;


typedef struct {
    hal_wifi_ap_info_adv_t ap_info;
    char key[64];                /* Security key or PMK of the wlan. */
    int  key_len;                /* The length of the key. */
    char local_ip_addr[16];      /* Static IP configuration, Local IP address. */
    char net_mask[16];           /* Static IP configuration, Netmask. */
    char gateway_ip_addr[16];    /* Static IP configuration, Router IP address. */
    char dns_server_ip_addr[16]; /* Static IP configuration, DNS server IP address. */
    char dhcp_mode;              /* DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
    char reserved[32];
    int  wifi_retry_interval;    /* Retry interval if an error is occured when connecting an access point, time unit is millisecond. */
} hal_wifi_init_type_adv_t;


typedef struct {
    uint8_t dhcp;     /* DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server. */
    char    ip[16];   /* Local IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    gate[16]; /* Router IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    mask[16]; /* Netmask on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    dns[16];  /* DNS server IP address. */
    char    mac[16];  /* MAC address, example: "C89346112233". */
    char    broadcastip[16];
} hal_wifi_ip_stat_t;

typedef enum {
    SOFT_AP,  /* Act as an access point, and other station can connect, 4 stations Max */
    STATION   /* Act as a station which can connect to an access point */
} hal_wifi_type_t;

enum {
    DHCP_DISABLE = 0,
    DHCP_CLIENT,
    DHCP_SERVER,
};

typedef struct {
    int     is_connected;  /* The link to wlan is established or not, 0: disconnected, 1: connected. */
    int     wifi_strength; /* Signal strength of the current connected AP */
    uint8_t ssid[32 + 1];    /* SSID of the current connected wlan */
    uint8_t bssid[6];      /* BSSID of the current connected wlan */
    int     channel;       /* Channel of the current connected wlan */
} hal_wifi_link_stat_t;

typedef struct hal_wifi_link_info_s {
    int8_t rssi; /* rssi value of received packet */
} hal_wifi_link_info_t;

/*
 * The event call back function called at specific events occurred.
 *
 * @note For HAL implementors, these callbacks must be
 *       called under normal task context, not from interrupt.
 */
typedef struct {
    void (*connect_fail)(hal_wifi_module_t *m, int err, void *arg);
    void (*ip_got)(hal_wifi_module_t *m, hal_wifi_ip_stat_t *pnet, void *arg);
    void (*stat_chg)(hal_wifi_module_t *m, hal_wifi_event_t stat, void *arg);
    void (*scan_compeleted)(hal_wifi_module_t *m, hal_wifi_scan_result_t *result,
                            void *arg);
    void (*scan_adv_compeleted)(hal_wifi_module_t *m,
                                hal_wifi_scan_result_adv_t *result, void *arg);
    void (*para_chg)(hal_wifi_module_t *m, hal_wifi_ap_info_adv_t *ap_info,
                     char *key, int key_len, void *arg);
    void (*fatal_err)(hal_wifi_module_t *m, void *arg);
} hal_wifi_event_cb_t;

typedef void (*monitor_data_cb_t)(uint8_t *data, int len, hal_wifi_link_info_t *info);

struct hal_wifi_module_s {
    hal_module_base_t    base;
    const hal_wifi_event_cb_t *ev_cb;

    int  (*init)(hal_wifi_module_t *m);
    void (*get_mac_addr)(hal_wifi_module_t *m, uint8_t *mac);
    void (*set_mac_addr)(hal_wifi_module_t *m, const uint8_t *mac);
    int  (*start)(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para);
    int  (*start_adv)(hal_wifi_module_t *m,
                      hal_wifi_init_type_adv_t *init_para_adv);
    int  (*get_ip_stat)(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para,
                        hal_wifi_type_t wifi_type);
    int  (*get_link_stat)(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat);
    void (*start_scan)(hal_wifi_module_t *m);
    void (*start_scan_adv)(hal_wifi_module_t *m);
    int  (*power_off)(hal_wifi_module_t *m);
    int  (*power_on)(hal_wifi_module_t *m);
    int  (*suspend)(hal_wifi_module_t *m);
    int  (*suspend_station)(hal_wifi_module_t *m);
    int  (*suspend_soft_ap)(hal_wifi_module_t *m);
    int  (*set_channel)(hal_wifi_module_t *m, int ch);
    int  (*get_channel)(hal_wifi_module_t *m);
    int  (*get_channel_list)(hal_wifi_module_t *m, const uint8_t **chnlist);
    void (*start_monitor)(hal_wifi_module_t *m);
    void (*stop_monitor)(hal_wifi_module_t *m);
    void (*register_monitor_cb)(hal_wifi_module_t *m, monitor_data_cb_t fn);
    void (*register_wlan_mgnt_monitor_cb)(hal_wifi_module_t *m, monitor_data_cb_t fn);
    int  (*wlan_send_80211_raw_frame)(hal_wifi_module_t *m, uint8_t *buf, int len);

    /* debug related */
    void (*start_debug_mode)(hal_wifi_module_t *m);
    void (*stop_debug_mode)(hal_wifi_module_t *m);

    /* mesh related */
    void (*mesh_register_cb)(hal_wifi_module_t *m, monitor_data_cb_t fn);
    void (*mesh_set_bssid)(hal_wifi_module_t *m, const uint8_t *mac);
    int  (*mesh_enable)(hal_wifi_module_t *m);
    int  (*mesh_disable)(hal_wifi_module_t *m);
    int  (*mesh_radio_sleep)(hal_wifi_module_t *m);
    int  (*mesh_radio_wakeup)(hal_wifi_module_t *m);
};

/**
 * Get the default wifi instance.
 * The system may have more than one wifi instance,
 * this API returns the default one.
 *
 * @return  Instance pointer, or NULL if no instance registered.
 */
hal_wifi_module_t *hal_wifi_get_default_module(void);

/**
 * Regster a wifi instance to the HAL framework.
 *
 * @param[in]  m  the wifi instance.
 */
void hal_wifi_register_module(hal_wifi_module_t *m);

/**
 * Initialize wifi instances.
 *
 * @note  This is supposed to be called during system boot,
 *        not supposed to be called by user module directly.
 *
 * @return  0 on success, otherwise failure.
 */
int hal_wifi_init(void);

/**
 * Get the MAC address of the specified wifi instance.
 *
 * @param[in]   m    the wifi instance, NULL if default.
 * @param[out]  mac  the place to hold the result.
 *
 * @return  0 on success, otherwise failure.
 */
int hal_wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac);

/**
 * Set the MAC address of the specified wifi instance.
 *
 * @param[in]   m    the wifi instance, NULL if default.
 * @param[in]   mac  mac value
 *
 * @return  0 on success, otherwise failure.
 */
int hal_wifi_set_mac_addr(hal_wifi_module_t *m, const uint8_t *mac);

/**
 * Start the wifi instance.
 *
 * @param[in]  m          the wifi instance, NULL if default.
 * @param[in]  init_para  the config used to start the wifi.
 *
 * @return      0 on success, otherwise failure.
 */
int hal_wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para);

/**
 * Start the wifi instance in anvanced way (more config specified).
 *
 * @param[in]  m              the wifi instance, NULL if default.
 * @param[in]  init_para_adv  the advanced config used to start the wifi.
 *
 * @return      0 on success, otherwise failure.
 */
int hal_wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv);

/**
 * Get the status of the specified wifi instance, e.g. the IP, mask, dhcp mode, etc.
 *
 * @param[in]   m             the wifi instance, NULL if default.
 * @param[out]  out_net_para  the place to hold the results.
 * @param[in]   wifi_type     SOFT_AP or STATION.
 *
 * @return      0 on success, otherwise failure.
 */
int hal_wifi_get_ip_stat(hal_wifi_module_t *m,
                         hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type);

/**
 * Get the link status of the wifi instance ,e.g. ssid, bssid, channel, rssi, etc.
 *
 * @param[in]   m         the wifi instance, NULL if default.
 * @param[out]  out_stat  the place to hold the results.
 *
 * @return      0 on success, otherwise failure.
 */
int hal_wifi_get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat);

/**
 * Start the scanning of the specified wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 */
void hal_wifi_start_scan(hal_wifi_module_t *m);

/**
 * Start the scanning of the specified wifi instance in advanced way.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 */
void hal_wifi_start_scan_adv(hal_wifi_module_t *m);

/**
 * Power off the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 *
 * @return     0 on success, otherwise failure.
 */
int hal_wifi_power_off(hal_wifi_module_t *m);

/**
 * Power on the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 *
 * @return     0 on success, otherwise failure.
 */
int hal_wifi_power_on(hal_wifi_module_t *m);

/**
 * Suspend the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 *
 * @return     0 on success, otherwise failure.
 */
int hal_wifi_suspend(hal_wifi_module_t *m);

/**
 * Suspend the wifi instance in station mode.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 *
 * @return     0 on success, otherwise failure.
 */
int hal_wifi_suspend_station(hal_wifi_module_t *m);

/**
 * Suspend the wifi instance in soft_ap mode.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 *
 * @return     0 on success, otherwise failure.
 */
int hal_wifi_suspend_soft_ap(hal_wifi_module_t *m);

/**
 * Set the channel of the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 *
 * @return     0 on success, otherwise failure.
 */
int hal_wifi_set_channel(hal_wifi_module_t *m, int ch);

/**
 * Get the channel of the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 *
 * @return     -1 on failure, otherwise current channel number.
 */
int hal_wifi_get_channel(hal_wifi_module_t *m);

/**
 * Get the channel list of the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 * @param[out]  chnlist  channel list in array
 *
 * @return     -1 on failure, otherwise number of available channels.
 */
int hal_wifi_get_channel_list(hal_wifi_module_t *m, const uint8_t **chnlist);

/**
 * Start the monitor mode of the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 */
void hal_wifi_start_wifi_monitor(hal_wifi_module_t *m);

/**
 * Stop the monitor mode of the wifi instance.
 *
 * @param[in]  m  The wifi instance, NULL if default.
 */
void hal_wifi_stop_wifi_monitor(hal_wifi_module_t *m);

/**
 * Register the montior callback on the wifi instance.
 *
 * @param[in]  m   the wifi instance, NULL if default.
 * @param[in]  fn  the callback function.
 */
void hal_wifi_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn);

/**
 * Register management frame montior callback on the wifi instance.
 *
 * @param[in]  m   the wifi instance, NULL if default.
 * @param[in]  fn  the callback function.
 */
void hal_wlan_register_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn);

/**
 * Send 802.11 raw frame
 *
 * @param[in]  m   the wifi instance, NULL if default.
 * @param[in]  buf frame buffer.
 * @param[in]  len length of frame buffer.
 */
int hal_wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len);

/**
 * Start debug mode of the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 */
void hal_wifi_start_debug_mode(hal_wifi_module_t *m);

/**
 * Stop debug mode of the wifi instance.
 *
 * @param[in]  m  the wifi instance, NULL if default.
 */
void hal_wifi_stop_debug_mode(hal_wifi_module_t *m);

/**
 * Set the event callback function array for the wifi.
 * Please don't do time consuming work in these callbacks.
 *
 * @note Please don't do time consuming work in these callbacks.
 *
 * @param[in]  m   the wifi instance, NULL for default.
 * @param[in]  cb  the event callback function info.
 */
void hal_wifi_install_event(hal_wifi_module_t *m, const hal_wifi_event_cb_t *cb);

/**
 * Regster a wifi instance to the uMesh
 *
 * @param[in]  m  the wifi instance.
 */
void hal_umesh_register_wifi(hal_wifi_module_t *m);

#endif /* HAL_WIFI_H */

