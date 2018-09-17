/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __IEEE80211_H
#define __IEEE80211_H

#include "zconfig_utils.h"
#include "zconfig_protocol.h"

#ifndef ETH_ALEN
#define ETH_ALEN    6
#endif

#define WIFI_RX_SENSITIVITY   (-85)
#define __le16    unsigned short

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
            u8 encry_type;/* none/wep/tkip/aes */
            u16 data_len;/* framelen - 80211 hdr - fcs(4) */
            u16 sn;
        } br;
        /* for alink ie frame */
        struct ie_info {
            u8 *alink_ie;
            u16 alink_ie_len;
        } ie;
        /* for p2p action frame */
        struct action_info {
            u8 *data;
            u16 data_len;
        } action;
        /* for p2p wps frame */
        struct wps_info {
            u8 *data;
            u16 data_len;
        } wps;
    } u;

    u8 *src; /* src mac of sender */
    u8 *dst; /* ff:ff:ff:ff:ff:ff */
    u8 *bssid; /* mac of AP */

    u8 tods; /* fromDs or toDs */
    u8 channel; /* 1 - 13 */
};

/* ap list */
extern struct ap_info *zconfig_aplist;
extern struct adha_info *adha_aplist;
extern void *clr_aplist_timer;
extern u8 zconfig_aplist_num;

int ieee80211_data_extract(u8 *in, int len, int link_type,
                           struct parser_res *res, signed char rssi);

struct ap_info *zconfig_get_apinfo(u8 *mac);
struct ap_info *zconfig_get_apinfo_by_ssid(u8 *ssid);
struct ap_info *zconfig_get_apinfo_by_ssid_prefix(u8 *ssid_prefix);
struct ap_info *zconfig_get_apinfo_by_ssid_suffix(u8 *ssid_suffix);

/* add channel to scanning channel list */
int zconfig_add_active_channel(int channel);
uint8_t zconfig_get_press_status();

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* __IEEE80211_H */
