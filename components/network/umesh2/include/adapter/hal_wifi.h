

#ifndef UMESH_HAL_H_
#define UMESH_HAL_H_

#include <stdint.h>
#include "define.h"


typedef void (*umesh_wifi_frame_cb_t)(uint8_t *buffer, int len,uint8_t rssi, void *context);

int umesh_wifi_send(const uint8_t *buf, int len);
int umesh_wifi_register_monitor_cb(uint32_t filter_mask, umesh_wifi_frame_cb_t callback, void *context);
int umesh_wifi_get_mac(uint8_t mac_str[IEEE80211_MAC_ADDR_LEN]);
int umesh_wifi_set_channel(int channel);
int umesh_wifi_get_channel();
int umesh_wifi_get_channel_list(const uint8_t **p, int *num);
int umesh_wifi_get_connect_state();
int umesh_wifi_get_ap_info(char ssid[UMESH_MAX_SSID_LEN],
                         char passwd[UMESH_MAX_PASSWD_LEN],
                         uint8_t bssid[IEEE80211_MAC_ADDR_LEN]);


int umesh_wifi_set_ap_info(char ssid[UMESH_MAX_SSID_LEN],
                        char passwd[UMESH_MAX_PASSWD_LEN],
                        uint8_t bssid[IEEE80211_MAC_ADDR_LEN],
                        uint32_t connection_timeout_ms);
#endif /* UMESH_HAL_H_ */
