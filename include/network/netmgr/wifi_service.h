/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "hal/wifi.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    WIFI_SERVICE_MSG_ID_MIN = 0,
    WIFI_SERVICE_MSG_ID_WIFI_STATUS = WIFI_SERVICE_MSG_ID_MIN,
    WIFI_SERVICE_MSG_ID_WIFI_STATUS_FROM_IMPL,
    WIFI_SERVICE_MSG_ID_WIFI_TRACE_FROM_IMPL,
    WIFI_SERVICE_MSG_ID_NETWORK_STATUS,
    WIFI_SERVICE_MSG_ID_MAX
} wifi_service_msgid_t;

/**
  * @brief  The enumeration is event type indicated from wlan driver.
  */
enum _WIFI_SERVICE_EVENT_INDICATE {
    WIFI_SERVICE_EVENT_CONNECT = 0,
    WIFI_SERVICE_EVENT_DISCONNECT = 1,
    WIFI_SERVICE_EVENT_FOURWAY_HANDSHAKE_DONE = 2,
    WIFI_SERVICE_EVENT_SCAN_RESULT_REPORT = 3,
    WIFI_SERVICE_EVENT_SCAN_DONE = 4,
    WIFI_SERVICE_EVENT_RECONNECTION_FAIL = 5,
    WIFI_SERVICE_EVENT_SEND_ACTION_DONE = 6,
    WIFI_SERVICE_EVENT_RX_MGNT = 7,
    WIFI_SERVICE_EVENT_STA_ASSOC = 8,
    WIFI_SERVICE_EVENT_STA_DISASSOC = 9,
    WIFI_SERVICE_EVENT_STA_WPS_START = 10,
    WIFI_SERVICE_EVENT_WPS_FINISH = 11,
    WIFI_SERVICE_EVENT_EAPOL_START = 12,
    WIFI_SERVICE_EVENT_EAPOL_RECVD = 13,
    WIFI_SERVICE_EVENT_NO_NETWORK = 14,
    WIFI_SERVICE_EVENT_BEACON_AFTER_DHCP = 15,
    WIFI_SERVICE_EVENT_IP_CHANGED = 16,
    WIFI_SERVICE_EVENT_ICV_ERROR = 17,
    WIFI_SERVICE_EVENT_CHALLENGE_FAIL = 18,
    WIFI_SERVICE_EVENT_SCAN_START = 19,
    WIFI_SERVICE_EVENT_SCAN_FAILED = 20,
    WIFI_SERVICE_EVENT_AUTHENTICATION = 21,
    WIFI_SERVICE_EVENT_AUTH_REJECT =22 ,
    WIFI_SERVICE_EVENT_DEAUTH   =23 ,
    WIFI_SERVICE_EVENT_AUTH_TIMEOUT =24 ,
    WIFI_SERVICE_EVENT_ASSOCIATING = 25,
    WIFI_SERVICE_EVENT_ASSOCIATED =26 ,
    WIFI_SERVICE_EVENT_ASSOC_REJECT =27 ,
    WIFI_SERVICE_EVENT_ASSOC_TIMEOUT =28 ,
    WIFI_SERVICE_EVENT_HANDSHAKE_FAILED =29 ,
    WIFI_SERVICE_EVENT_4WAY_HANDSHAKE = 30,
    WIFI_SERVICE_EVENT_GROUP_HANDSHAKE = 31,
    WIFI_SERVICE_EVENT_GROUP_HANDSHAKE_DONE = 32,
    WIFI_SERVICE_EVENT_CONN_TIMEOUT =33,
    WIFI_SERVICE_EVENT_MAX,
};

#define WIFI_SERVICE_METHOD_MAX_LENGTH       (32)
#define WIFI_SERVICE_STATUS_MAX_LENGTH       (32)
#define WIFI_SERVICE_SSID_MAX_LENGTH         (32)
#define WIFI_SERVICE_PASSWORD_MAX_LENGTH     (64)

#define MAX_SSID_SIZE  WIFI_SERVICE_SSID_MAX_LENGTH
#define MAX_PWD_SIZE   WIFI_SERVICE_PASSWORD_MAX_LENGTH

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN (MAX_SSID_SIZE + 1)
#endif
#ifndef MAX_PWD_LEN
#define MAX_PWD_LEN (MAX_PWD_SIZE + 1)
#endif
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif

typedef struct
{
   char method[WIFI_SERVICE_METHOD_MAX_LENGTH+1];
   int  quantity;
   char status[WIFI_SERVICE_STATUS_MAX_LENGTH+1];
   char ssid[WIFI_SERVICE_SSID_MAX_LENGTH+1];
   char password[WIFI_SERVICE_PASSWORD_MAX_LENGTH+1];
   uint8_t reason_code;
} wifi_service_network_status_change_t;

typedef struct {
    int id;
    union {
        int   wlan_status;
        void *network_status_change;
        void *wifi_trace;
    } data;
} wifi_service_data_t;

typedef struct {
    char     ssid[WIFI_SERVICE_SSID_MAX_LENGTH+1];
    uint8_t  pwd[WIFI_SERVICE_PASSWORD_MAX_LENGTH+1];
    uint8_t  bssid[ETH_ALEN];
    int8_t   ap_power;
    uint8_t  channel;
    uint8_t  sec_type; /* details see hal_wifi_sec_type */
    uint8_t  is_ssid_gbk;
    char     utf8_ssid[WIFI_SERVICE_SSID_MAX_LENGTH+1];
} wifi_service_ap_info_t;

#define MAX_AP_CONFIG_NUM   (3)
/* 1 bigger than actual size for holding \0 */
typedef struct {
    int ap_num;
    int used_ap;
    wifi_service_ap_info_t config[MAX_AP_CONFIG_NUM];
} wifi_service_ap_config_t;

typedef void (*wifi_service_msg_cb_t)(wifi_service_data_t* data);

int wifi_service_init(void);

int wifi_service_deinit(void);

int wifi_service_on(void);

int wifi_service_off(void);

int wifi_service_wifi_connect(const char *ssid,const char *password,const char *bssid, int time_ms);

int wifi_service_disconnect(void);

void wifi_service_auto_reconnect(int enable);

void wifi_service_set_wifi_msg_cb(wifi_service_msg_cb_t cb);

int wifi_service_get_rssi(int *rssi);

int wifi_service_get_mac(char *mac);

int wifi_service_get_ip_addr(char* ip_address);

int wifi_service_save_config(void);

void wifi_service_handle_cmd(int argc, char **argv);

#ifdef __cplusplus
};  // extern "C"
#endif
#endif
