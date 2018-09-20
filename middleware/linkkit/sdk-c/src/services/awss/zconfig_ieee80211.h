/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __ZCONFIG_IEEE80211_H__
#define __ZCONFIG_IEEE80211_H__

#include "zconfig_utils.h"
#include "zconfig_protocol.h"

#ifndef ETH_ALEN
#define ETH_ALEN    6
#endif

#define WIFI_RX_SENSITIVITY   (-85)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

enum ALINK_TYPE {
    ALINK_INVALID = 0,
    ALINK_BROADCAST = 1,
    ALINK_ROUTER = 2,
    ALINK_ACTION = 3,
    ALINK_WPS = 4,
    ALINK_DEFAULT_SSID = 5,
    ALINK_ZERO_CONFIG = 6,
    ALINK_ADHA_SSID = 7,
};

/* 80211 frame parser result */
struct parser_res {
    union _alink_type_ {
        /* for broadcast data frame */
        struct broadcast_info {
            uint8_t encry_type;/* none/wep/tkip/aes */
            uint16_t data_len;/* framelen - 80211 hdr - fcs(4) */
            uint16_t sn;
        } br;
        /* for alink ie frame */
        struct ie_info {
            uint8_t *alink_ie;
            uint16_t alink_ie_len;
        } ie;
        /* for p2p action frame */
        struct action_info {
            uint8_t *data;
            uint16_t data_len;
        } action;
        /* for p2p wps frame */
        struct wps_info {
            uint8_t *data;
            uint16_t data_len;
        } wps;
    } u;

    uint8_t *src; /* src mac of sender */
    uint8_t *dst; /* ff:ff:ff:ff:ff:ff */
    uint8_t *bssid; /* mac of AP */

    uint8_t tods; /* fromDs or toDs */
    uint8_t channel; /* 1 - 13 */
};

/* ap list */
extern struct ap_info *zconfig_aplist;
extern struct adha_info *adha_aplist;
extern void *clr_aplist_timer;
extern uint8_t zconfig_aplist_num;

int ieee80211_data_extract(uint8_t *in, int len, int link_type,
                           struct parser_res *res, signed char rssi);

struct ap_info *zconfig_get_apinfo(uint8_t *mac);
struct ap_info *zconfig_get_apinfo_by_ssid(uint8_t *ssid);
struct ap_info *zconfig_get_apinfo_by_ssid_prefix(uint8_t *ssid_prefix);
struct ap_info *zconfig_get_apinfo_by_ssid_suffix(uint8_t *ssid_suffix);

/* add channel to scanning channel list */
int zconfig_add_active_channel(int channel);
uint8_t zconfig_get_press_status();

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* __IEEE80211_H */
