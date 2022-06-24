#ifndef _MOD_NETWORK_H_
#define _MOD_NETWORK_H_

#include "lwip/ip_addr.h"

#define NETWORK_SSID_MAX_LEN   (32)
#define NETWORK_PASSWD_MAX_LEN (64)

typedef struct network_wifi_info {
    char ssid[NETWORK_SSID_MAX_LEN + 1];
    char bssid[6];
    int rssi;
} network_wifi_info_t;

typedef struct wifi_ap_info {
    char ssid[NETWORK_SSID_MAX_LEN + 1];
    char passwd[NETWORK_PASSWD_MAX_LEN + 1];
} wifi_ap_info_t;

enum network_t {
    NETWORK_TYPE_WIFI = 0,
    NETWORK_TYPE_CELLULAR,
    NETWORK_TYPE_ETHERNET,
};

enum network_event_t {
    NETWORK_EVT_GOT_IP = 0,
    NETWORK_EVT_CONN_FAIL,
    NETWORK_EVT_GOT_SSID_PASSWD,
};

int network_qrcode_scan(void *gray, int w, int h, char *ouput, int max_len);
int network_qrscan_result_process(const char *result, char *ssid, char *passwd);

#endif /* _MOD_NETWORK_H_ */
