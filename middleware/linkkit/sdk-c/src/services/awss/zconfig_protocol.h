/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef ZCONFIG_PROTOCOL_H
#define ZCONFIG_PROTOCOL_H

#include "zconfig_utils.h"
#include "zconfig_ieee80211.h"
#include "zconfig_lib.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

enum state_machine {
	STATE_CHN_SCANNING,
	STATE_CHN_LOCKED_BY_P2P,//by wps/action
	STATE_CHN_LOCKED_BY_BR,//by broadcast
	STATE_GOT_BEACON,
	STATE_RCV_IN_PROGRESS,
	STATE_RCV_COMPLETE,
	STATE_RCV_DONE
};

#define PASSWD_ENCRYPT_BIT_OFFSET (1)
#define PASSWD_ENCRYPT_MASK       (0x06)
#define SSID_EXIST_BIT            (0)
#define SSID_EXIST_MASK           (1 << SSID_EXIST_BIT)
#define SSID_ENCODE_BIT           (5)
#define SSID_ENCODE_MASK          (1 << SSID_ENCODE_BIT)
enum passwd_encpyt_type {
    PASSWD_ENCRYPT_OPEN = 0,
    PASSWD_ENCRYPT_CIPHER,
    PASSWD_ENCRYPT_AESCFB,
    PASSWD_ENCRYPT_AESOFB,
};

struct ap_info {
	u8 auth;
	u8 channel;
	u8 encry[2];
	u8 mac[ETH_ALEN];
	char ssid[ZC_MAX_SSID_LEN];
	signed char rssi;
};

struct adha_info {
    u8 try_idx;
    u8 cnt;
    u8 aplist[MAX_APLIST_NUM];
};

#define flag_tods(tods)			  ((tods) ? 'T' : 'F')

extern const u8 br_mac[ETH_ALEN];

#define ZC_MAX_CHANNEL            (14)
#define ZC_MIN_CHANNEL            (1)
static inline int zconfig_is_valid_channel(int channel)
{
	return (ZC_MIN_CHANNEL <= channel && channel <= ZC_MAX_CHANNEL);
}

#define P2P_ENCODE_TYPE_OFFSET    (0x05)
#define P2P_SSID_LEN_MASK         ((1 << P2P_ENCODE_TYPE_OFFSET) - 1)
#define P2P_ENCRYPT_BIT_MASK      ((u8)(~P2P_SSID_LEN_MASK))
enum p2p_encode_type {
    P2P_ENCODE_TYPE_DICT = 0x00,
    P2P_ENCODE_TYPE_ENCRYPT,
};

#define zconfig_get_time          os_get_time_ms

/* global data */
/* max: 48(ssid gbk encode) + 64 (passwd) + 6 (1(tlen) + 1(flag) + 1(ssid_len) + 1(passwd_len) + 2(crc)) */
#define MAX_PKG_NUMS              (128)

/* zconfig protocol */
#define START_FRAME               (0x4E0) /* 0x4E0 is group 0 */
#define GROUP_FRAME               (0x3E0) /* exclusive, 0x401 is group 1, 0x400 is not used */
#define GROUP_FRAME_END           (GROUP_FRAME + MAX_PKG_NUMS / GROUP_NUMBER) /* exclusive */
#define GROUP_NUMBER              (8)
#define ZC_GRP_PKT_IDX_START      (2)
#define ZC_GRP_PKT_IDX_END        (ZC_GRP_PKT_IDX_START + GROUP_NUMBER - 1)

struct package {
	u16 len;
    char score;
};

struct zconfig_data {
	struct {
		u8 state_machine;	/* state for tods/fromds */
		u8 frame_offset;	/* frame fixed offset */
		u8 group_pos;		/* latest group pkg pos */
		u8 cur_pos;			/* data abs. position */
		u8 max_pos;			/* data max len */
		u8 last_index;
		u8 replace;			/* whether pkg has been replaced recently */
		u8 score_uplimit;
#define score_max                 (100)
#define score_high                (98)
#define score_mid                 (50)
#define score_low                 (1)
#define score_min                 (0)

		u8 pos_unsync;
		u16 group_sn;		/* latest group pkg sn */
		u16 prev_sn;		/* last sn */
		u16 last_len;		/* len pkg len */
		u32 timestamp;		/* last timestamp */
#define time_interval             (300)	//ms
	} data[2];

	/* package store */
	struct package	pkg[2][MAX_PKG_NUMS];

	struct package	tmp_pkg[2][GROUP_NUMBER + 1];

	u8 src_mac[ETH_ALEN];
	u8 channel;				/* from 1 -- 13 */

	/* result, final result */
	u8 ssid[ZC_MAX_SSID_LEN];
	u8 passwd[ZC_MAX_PASSWD_LEN];
	u8 bssid[ETH_ALEN];
	u8 ssid_is_gbk;
	u8 ssid_auto_complete_disable;

	/* used by v2 android p2p protocol, for gbk ssid correctness */
	u8 android_pre_ssid[ZC_MAX_SSID_LEN];
	u8 android_ssid[ZC_MAX_SSID_LEN];
	u8 android_bssid[ETH_ALEN];
	u8 android_src[ETH_ALEN];
};

extern struct zconfig_data *zconfig_data;

#define zc_state                  zconfig_data->data[tods].state_machine
#define zc_frame_offset           zconfig_data->data[tods].frame_offset
#define zc_group_pos              zconfig_data->data[tods].group_pos
#define zc_group_sn               zconfig_data->data[tods].group_sn
#define zc_prev_sn                zconfig_data->data[tods].prev_sn
#define zc_cur_pos                zconfig_data->data[tods].cur_pos
#define zc_max_pos                zconfig_data->data[tods].max_pos
#define zc_last_index             zconfig_data->data[tods].last_index
#define zc_last_len               zconfig_data->data[tods].last_len
#define zc_replace                zconfig_data->data[tods].replace
#define zc_score_uplimit          zconfig_data->data[tods].score_uplimit
#define zc_timestamp              zconfig_data->data[tods].timestamp
#define zc_pos_unsync             zconfig_data->data[tods].pos_unsync

#define zc_src_mac                &zconfig_data->src_mac[0]

#define zc_channel                zconfig_data->channel

#define zc_ssid                   (&zconfig_data->ssid[0])
#define zc_passwd                 (&zconfig_data->passwd[0])
#define zc_bssid                  (&zconfig_data->bssid[0])
#define zc_ssid_is_gbk            (zconfig_data->ssid_is_gbk)
#define zc_ssid_auto_complete_disable    (zconfig_data->ssid_auto_complete_disable)

#define pkg_score(n)              zconfig_data->pkg[tods][n].score
#define pkg_len(n)                zconfig_data->pkg[tods][n].len
#define pkg(n)                    &zconfig_data->pkg[tods][n]

#define tmp_score(n)              zconfig_data->tmp_pkg[tods][n].score
#define tmp_len(n)                zconfig_data->tmp_pkg[tods][n].len
#define tmp(n)                    &zconfig_data->tmp_pkg[tods][n]

#define zc_pre_ssid               (&zconfig_data->android_pre_ssid[0])
#define zc_android_ssid           (&zconfig_data->android_ssid[0])
#define zc_android_bssid          (&zconfig_data->android_bssid[0])
#define zc_android_src            (&zconfig_data->android_src[0])

/*
 * [IN] ssid or bssid
 * [OUT] auth, encry, channel
 */
u8 zconfig_get_auth_info(u8 *ssid, u8 *bssid, u8 *auth, u8 *encry, u8 *channel);
void zconfig_force_destroy(void);

#define MAC_FORMAT                "%02x%02x%02x%02x%02x%02x"
#define MAC_VALUE(mac)            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

extern const char *zc_default_ssid;
extern const char *zc_default_passwd;

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* __IEEE80211_123_H */
