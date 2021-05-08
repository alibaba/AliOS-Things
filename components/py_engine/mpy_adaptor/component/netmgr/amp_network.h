#ifndef _AMP_NETWORK_H_
#define _AMP_NETWORK_H_
#define SCANNED_WIFI_COUNT_MAX 32
#define SCANNED_LOCATOR_COUNT_MAX 32


typedef struct amp_wifi_info {
    char ssid[128];
    char mac[6];
    char ip[16];
    int rssi;
} amp_wifi_info_t;


#endif /* _AMP_NETWORK_H_ */