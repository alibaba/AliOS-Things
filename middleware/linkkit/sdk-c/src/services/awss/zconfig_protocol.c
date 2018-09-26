/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdlib.h>
#include <string.h>            // strcpy, memset()
#include <ctype.h>             // isascii()
#include <stdio.h>             // snprintf
#include "os.h"
#include "zconfig_utils.h"
#include "zconfig_ieee80211.h"
#include "zconfig_protocol.h"
#include "passwd.h"
#include "awss_aha.h"
#include "awss_adha.h"
#include "awss_aplist.h"
#include "awss_smartconfig.h"
#include "awss_main.h"
#include "awss_timer.h"
#include "awss_crypt.h"
#include "awss.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/* broadcast mac address */
const uint8_t br_mac[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/* all zero mac address */
const uint8_t zero_mac[ETH_ALEN] = {0, 0, 0, 0, 0, 0};

/* which channel lock at */
uint8_t zconfig_channel_locked = INVALID_CHANNEL;/* locked channel */
/*
 * avoid zconfig_callback_over() was called twice,
 * once from tods, once from fromds
 */
uint8_t zconfig_finished;

/* global data structure, which hold all broadcast data */
struct zconfig_data *zconfig_data;

/*
 * 8bit -> x bit
 *
 * serialize chinese char from 8bit to x bit
 */
void encode_chinese(uint8_t *in, uint8_t in_len,
                    uint8_t *out, uint8_t *out_len, uint8_t bits)
{
    if (bits == 0 || bits > 7)
        return;

    uint8_t bit[ZC_MAX_SSID_LEN * 8] = { 0 };
    uint8_t i, j;
    uint8_t output_len = ((in_len * 8) + bits - 1) / bits;

    //char to bit stream
    for (i = 0; i < in_len; i ++) {
        for (j = 0; j < 8; j ++) {
            bit[i * 8 + j] = (in[i] >> j) & 0x01;
        }
    }

    out[output_len] = '\0'; /* NULL-terminated */
    for (i = 0; i < output_len; i ++) {
        for (j = 0, out[i] = 0; j < bits; j++) {
            out[i] |= bit[i * bits + j] << j;
            //awss_debug("%02x ", out[i]);
        }
    }

    if (out_len)
        *out_len = output_len;
}

/* x bit -> 8bit */
void decode_chinese(uint8_t *in, uint8_t in_len,
                    uint8_t *out, uint8_t *out_len, uint8_t bits)
{
    if (bits == 0 || bits > 7 || in_len == 0)
        return;

    uint8_t i, j;
    uint8_t output_len = (in_len * bits) / 8;
    uint8_t *bit = (uint8_t *)os_zalloc(in_len * bits);
    if (bit == NULL)
        awss_crit("decode malloc failed!\r\n");

    //char to bit stream
    for (i = 0; i < in_len; i ++) {
        for (j = 0; j < bits; j ++) {
            bit[i * bits + j] = (in[i] >> j) & 0x01;
        }
    }

    out[output_len] = '\0'; /* NULL-terminated */
    for (i = 0; i < output_len; i++) {
        for (j = 0, out[i] = 0; j < 8; j++) {
            out[i] |= bit[i * 8 + j] << j;
            //awss_debug("%02x ", out[i]);
        }
    }

    os_free(bit);
    if (out_len)
        *out_len = output_len;
}

int is_ascii_string(uint8_t *str)
{
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] < 128) {
            i++;
        } else {
            return 0;
        }
    }
    return 1;
}

/*
 * 1/locked, 0/not locked
 */
uint8_t is_channel_locked(void)
{
    return zconfig_channel_locked != INVALID_CHANNEL;
}

/*
 * Note: this notification will be kept called, in case of
 *     user clear the channel locked state to re-scanning
 *    channel because of waiting timeout.
 */
uint8_t zconfig_callback_channel_locked(uint8_t channel)
{
    if (channel != zconfig_channel_locked) {
        awss_debug("channel lock @ %d\r\n", channel);
        zconfig_channel_locked = channel;
    }

    /*
     * if recv timeout, vendor may re-scanning channel,
     * so keep calling channel locked notification here.
     */
    zconfig_channel_locked_callback(channel, 0, zc_bssid);

    return 0;
}

/*
 * [IN] ssid or bssid
 * [OUT] auth, encry, channel
 */
uint8_t zconfig_get_auth_info(uint8_t *ssid, uint8_t *bssid, uint8_t *auth,
                              uint8_t *encry, uint8_t *channel)
{
    uint8_t *valid_bssid = NULL;
    struct ap_info *ap_info = NULL;

    /* sanity check */
    if (!bssid || !memcmp(bssid, zero_mac, ETH_ALEN)) {
        valid_bssid = NULL;
    } else {
        valid_bssid = bssid;
    }

    /* use mac or ssid to search apinfo */
    if (valid_bssid) {
        ap_info = zconfig_get_apinfo(valid_bssid);
    } else {
        ap_info = zconfig_get_apinfo_by_ssid(ssid);
    }
    if (!ap_info) {
        return 0;
    }

    if (auth) {
        *auth = ap_info->auth;
    }
    if (encry) {
        *encry = ap_info->encry[1];    /* tods side */
    }
    if (!valid_bssid && bssid) {
        memcpy(bssid, ap_info->mac, ETH_ALEN);
    }
    if (channel) {
        *channel = ap_info->channel;
    }

    return 1;

}

uint8_t zconfig_callback_over(uint8_t *ssid, uint8_t *passwd, uint8_t *bssid)
{
    uint8_t auth = ZC_AUTH_TYPE_INVALID, encry = ZC_ENC_TYPE_INVALID, channel = 0;

    awss_trace("zconfig done. ssid:%s, mac:%02x%02x%02x%02x%02x%02x\r\n",
               ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

    if (zconfig_finished)
        return 0;

    zconfig_get_auth_info(ssid, bssid, &auth, &encry, &channel);

    zconfig_got_ssid_passwd_callback(ssid, passwd, bssid, auth, encry, channel);

    zconfig_finished = 1;

    os_awss_close_monitor();

    return 0;
}

void zconfig_set_state(uint8_t state, uint8_t tods, uint8_t channel)
{
    /* state change callback */
    switch (state) {
        case STATE_CHN_SCANNING:
            break;
        case STATE_CHN_LOCKED_BY_P2P:
            //locked state used by action/wps frame
            zconfig_callback_channel_locked(channel);
            break;
        case STATE_CHN_LOCKED_BY_BR:
            //locked state used by br frame
            zconfig_callback_channel_locked(zc_channel ? zc_channel : channel);
            break;
        case STATE_RCV_DONE:
            // prevent main_thread_func to free zconfig_data until curent task is finished.
            os_mutex_lock(zc_mutex);
            /*
             * in case of p2p/router, direct into RCV_DONE state,
             * skiped the chn lock state, so better to call channel lock here
             */
            if (!is_channel_locked()) {
                zconfig_callback_channel_locked(channel);
            }
            zconfig_callback_over(zc_ssid, zc_passwd, zc_bssid);
            break;
        default:
            break;
    }

    /*
     * state machine loop:
     * scanning -> p2p lock -> rcv_done
     * scanning -> (p2p) rcv_done
     * scanning -> br lock -> (br) rcv_done
     * scanning -> br lock -> (p2p) recv_done
     * scanning -> p2p lock -> br lock -> (br) recv_done
     *
     * watch out zc_state rolling back.
     * zconfig_set_state(CHN_LOCKED) will be called more than once,
     */
    if (zc_state < state)
        zc_state = state;
    if (state == STATE_RCV_DONE)
        os_mutex_unlock(zc_mutex);
}

/*
    pkt_data & pkt_length:
        radio_hdr + 80211 hdr + payload, without fcs(4B)
    return:
        PKG_INVALID -- invalid pkt,
        PKG_START_FRAME -- start frame,
        PKG_DATA_FRAME -- data frame,
        PKG_ALINK_ROUTER -- alink router,
        PKG_GROUP_FRAME -- group frame,
        PKG_BC_FRAME -- broadcast frame
*/
int is_invalid_pkg(void *pkt_data, uint32_t pkt_length)
{
#define MIN_PKG         (33)
#define MAX_PKG         (1480 + 56 + 200)
    if (pkt_length < MIN_PKG || pkt_length > MAX_PKG) {
        return 1;
    } else {
        return 0;
    }
}

/*  Variable
 *  Qos: 2Byte
 *  auth offset: 36 44 52 56
 *  group frame: 4e0 3e1~3e7
 */

#ifdef  AWSS_SUPPORT_HT40
uint8_t ht40_hint_frame_cnt[64];
uint8_t ht40_filter;
signed char ht40_rssi_low, ht40_rssi_high;
#define ht40_rssi_range         (15)    /* suggested by Fuzhibo */
/* for ios: start frame interval is 20/100 ms */
#define HIT_FRAME_PER_CHANNEL   (2)

#define IEEE80211_MIN_HDRLEN    (24)

uint32_t ht40_timestamp;
uint8_t ht40_channel[ZC_MAX_CHANNEL + 1];
uint8_t ht40_channel_filter[ZC_MAX_CHANNEL + 1];
uint8_t ht40_state;

void ht40_init(void)
{
    ht40_state = 0;
    ht40_filter = 0;
    ht40_rssi_low = 0;
    ht40_rssi_high = 0;
    ht40_timestamp = 0;
    memset(ht40_channel, 0, sizeof(ht40_channel));
    memset(ht40_channel_filter, 0, sizeof(ht40_channel_filter));
    memset(ht40_hint_frame_cnt, 0, sizeof(ht40_hint_frame_cnt));
}

int ht40_lock_channel(uint8_t channel, uint8_t filter)
{
    if (channel < 1 || channel > 14) {
        return 0;
    }

    if (!ht40_channel[channel]) {  // replace when 0
        ht40_channel[channel] ++;
        ht40_channel_filter[channel] = filter;
    } else if (filter == ht40_channel_filter[channel]) {
        ht40_channel[channel] ++;  /* increase */
    } else {
        ht40_channel[channel] --;  /* decrease */
    }

    if (ht40_channel[channel] >= HIT_FRAME_PER_CHANNEL) {
        return 1;
    }

    return 0;
}

int ht40_scanning_hint_frame(uint8_t filter, signed char rssi, uint32_t length, uint8_t channel)
{
    uint8_t channel_locked = 0, next_loop = 0;
    int hint_pos = -1;
    int tods = 1;
    int i, j, k;

    if (ht40_state != STATE_CHN_SCANNING) {
        return -1;
    }

    /* range check, max: 0x4e0 + tkip + qos, min: 0x3e0 + open */
    if (length > START_FRAME + zconfig_fixed_offset[2][0] + 2
        || length <= GROUP_FRAME + zconfig_fixed_offset[0][0]) {
        return -1;
    }

    for (i = 1; i >= 0; i--) //Qos or not
        for (j = 3; j >= 0; j--) //auth type, without open
            for (k = 0; k < 8; k++) {//group frame
                if (zconfig_hint_frame[k] + zconfig_fixed_offset[j][0] + i * 2 == length) {
                    hint_pos = i * 32 + j * 8 + k;
#if 1
                    awss_trace("\r\nfilter:%x, rssi:%d, len:%d, Qos:%d, auth:%d, group:%d, %s\r\n",
                              filter, rssi, length, i, j, k,
                              next_loop ? "DUP" : "");
#endif
                    if (!next_loop) {
                        channel_locked = ht40_lock_channel(channel, filter);
                        if (channel_locked) {
                            zconfig_set_state(STATE_CHN_LOCKED_BY_BR, tods, channel);

                        }
                        next_loop = 1;/* don't enter this loop again */
                    }

                    ht40_hint_frame_cnt[hint_pos]++;
                }
            }//end of for

    if (channel_locked) {
        ht40_rssi_high = rssi + ht40_rssi_range;
        if (ht40_rssi_high > -1) {
            ht40_rssi_high = -1;
        }
        ht40_rssi_low = rssi - ht40_rssi_range;
        if (ht40_rssi_low < -128) {
            ht40_rssi_low = -128;
        }

        ht40_filter = filter;

        awss_trace("filter:%x, rssi range:[%d, %d]\r\n",
                  filter, ht40_rssi_low, ht40_rssi_high);
    }

    return hint_pos;
}

int ht40_get_qos_auth_group_info(uint32_t length)
{
    int count = 0, max_count = 0, max_count_pos = 0;
    int continues = 0, max_continues = 0, max_continues_pos = 0, second_continues = 0;
    int tods = 1;
    int i, j, k;

    if (zc_state != STATE_CHN_LOCKED_BY_BR ||
        ht40_state != STATE_CHN_SCANNING) {
        return 0;
    }

    for (i = 1; i >= 0; i--) //Qos or not
        for (j = 3; j >= 0; j--) //auth type
            for (count = 0, continues = 0, k = 0; k < 8; k++) {//group frame
                int pos = i * 32 + j * 8 + k;

                if (ht40_hint_frame_cnt[pos]) {
                    count += ht40_hint_frame_cnt[pos];
                    if (count > max_count) {//NOTE: not >=, see continues
                        max_count = count;
                        max_count_pos = pos;
                    }

                    continues += 1;
                    if (continues >= max_continues) {
                        second_continues = max_continues;
                        max_continues = continues;
                        max_continues_pos = pos;
                    }
                }
            }

    awss_debug("max_cont:%d, sec_cont:%d, max_count:%d, max_cont_pos:%d, max_count_pos:%d\r\n",
               max_continues, second_continues, max_count, max_count_pos, max_continues_pos);

    if (max_continues > second_continues // not >=
        && max_count_pos == max_continues_pos) {
        uint8_t qos = max_count_pos / 32;
        uint8_t auth = (max_count_pos % 32) / 8;

        zc_frame_offset = zconfig_fixed_offset[auth][0] + qos * 2;
        length -= zc_frame_offset;
        if (is_start_frame(length) || is_group_frame(length)) {
            uint8_t group = get_group_index(length);

            zc_group_pos = group;
            zc_cur_pos = group;
            zc_score_uplimit = score_mid;
            ht40_timestamp = os_get_time_ms();

            ht40_state = STATE_RCV_IN_PROGRESS;
            awss_debug("len:%d, qos:%d, auth:%d, group:%d, offset:%d\r\n",
                       length, qos, auth, group, zc_frame_offset);
        }
    }

    return 0;
}

int zconfig_recv_callback_ht40_ctrl(uint8_t filter, signed char rssi, uint32_t length, uint8_t channel)
{
    uint32_t now = os_get_time_ms();
    int pkg_type = PKG_INVALID;
    uint8_t tods = 1, timeout = 0, equal = 0, score = 0;
    uint16_t pos = 0, index = 0, len = 0;

    int hint_pos = -1;

    if (length > IEEE80211_MIN_HDRLEN) {
        length -= IEEE80211_MIN_HDRLEN;
        len = length;
    } else {
        goto drop;
    }

    hint_pos = ht40_scanning_hint_frame(filter, rssi, length, channel);

    if (hint_pos >= 0) {
        ht40_get_qos_auth_group_info(length);
        return PKG_GROUP_FRAME;
    }

    if (ht40_state == STATE_RCV_IN_PROGRESS) {
        if (rssi <= ht40_rssi_low && rssi >= ht40_rssi_high) {
            goto drop;
        }
        if (filter != ht40_filter) {
            goto drop;
        }
        if (len <= zc_frame_offset) { /* length invalid */
            goto drop;
        }

        len -= zc_frame_offset;

        if (is_data_frame(len)) {
            pkg_type = PKG_DATA_FRAME;
            index = get_data_index(len);
            pos = zc_group_pos + index;

            if (now - ht40_timestamp > time_interval) {
                awss_debug("\t\t\t\t\ttimestamp = %d, pos:%d, len:%x\r\n",
                           now - ht40_timestamp, pos, len);
                timeout = 1;
                goto drop;
            }

            /*
             * pos_unsync: 进入条件,任一条
             *     case1: index rollback
             *     case2: index equal but len not equal
             *     case3: data frame & timeout
             * 退出条件：
             *     case1: 进入条件同时也是退出条件
             *     case2: 收到同步帧
             */
            if (pos < zc_cur_pos) {
                awss_debug("drop: index rollback. prev:%d, cur:%d\n", zc_cur_pos, pos);
                goto drop;
            }

            if (pos == zc_cur_pos && len != pkg_len(zc_cur_pos)) {
                awss_debug("drop: index equal, but len not. prev:%x, cur:%x\n",
                           pkg_len(pos), len);
                pkg_score(pos)--;
                goto drop;
            }

            if (pos > zc_cur_pos + 4) {
                awss_debug("drop: over range too much, prev:%d, cur:%d\n",
                           zc_cur_pos, pos);
                goto drop;
            }

            score = zc_score_uplimit - (pos - zc_cur_pos - 1);
            zc_score_uplimit = score;

            awss_trace("ht40 %d+%d [%d] -- T %-3x\r\n", zc_group_pos, index, score, len);
            /*
               score now > last:
               1) data equal:    pkg_score = now
               2) not equal:    pkg_score = now, data replace
               score now == last:
               1) data equal:    pkg_score++    and ???
               2) not equal:    pkg_score cut down & give warning & try_to_replace
               score now < last:
               1) data equal:    score_uplimit up???
               2) not equal:    goto pos_unsync
               */
            equal = !package_cmp((uint8_t *)pkg(pos), NULL, NULL, tods, len);

            if (score > pkg_score(pos)) {
                pkg_score(pos) = score;    //update score first
                if (!equal) {
                    zc_replace = 1;
                    package_save((uint8_t *)pkg(pos), NULL, NULL, tods, len);
                }
            } else if (score == pkg_score(pos)) {/* range check ? */
                if (equal) {
                    pkg_score(pos)++;
                } else {
                    pkg_score(pos)--;
                }
            } else {//pkg_score(pos) > score
                /* do nothing */
            }

            zc_cur_pos = pos;
            zc_max_pos = (zc_max_pos < zc_cur_pos) ? zc_cur_pos : zc_max_pos;
            if (zc_replace && zconfig_recv_completed(tods)) {
                zc_replace = 0;
                if (!zconfig_get_ssid_passwd(tods)) {
                    /* we got it! */
                    zconfig_set_state(STATE_RCV_DONE, tods, channel);
                    return PKG_END;
                }
            }

            ht40_timestamp = now;
            return pkg_type;

        } else {
            if (is_start_frame(len) || is_group_frame(len)) {
                uint8_t group = get_group_index(len);

                if (!group || group == zc_group_pos + 8) {
                    zc_group_pos = group;
                    zc_score_uplimit = score_mid;

                    if (zc_cur_pos + 1 == group) {
                        pkg_score(zc_cur_pos) += 1;
                    }

                    zc_cur_pos = group;

                    awss_trace("%d+%d [%d] -- T %-3x\r\n", group, 0, zc_score_uplimit, len);

                    //ignore PKG_GROUP_FRAME here
                    pkg_type = PKG_START_FRAME;
                    ht40_timestamp = now;
                    return pkg_type;
                }
            }
        }
    }

drop:
    return PKG_INVALID;
}
#endif

/*
 * zconfig_recv_callback()
 *
 * ieee80211 package parser
 *
 * @Return:
 *     zconfig state
 */
int zconfig_recv_callback(void *pkt_data, uint32_t pkt_length, uint8_t channel,
                          int link_type, int with_fcs, signed char rssi)
{
    int pkt_type = PKG_INVALID;
    struct parser_res res = {0};

    /* remove FCS filed */
    if (with_fcs)
        pkt_length -= 4;

    /* useless, will be removed */
    if (is_invalid_pkg(pkt_data, pkt_length))
        return PKG_INVALID;

#ifdef AWSS_SUPPORT_HT40
    if (link_type == AWSS_LINK_TYPE_HT40_CTRL) {
        if (!g_user_press) {
            return PKG_INVALID;
        }
        struct ht40_ctrl *ctrl = pkt_data;
        return zconfig_recv_callback_ht40_ctrl(ctrl->filter, ctrl->rssi, pkt_length, channel);
    }
#endif

    res.channel = channel;

    pkt_type = ieee80211_data_extract(pkt_data, pkt_length, link_type, &res, rssi);

    return pkt_type;
}

/* init mem & timer */
void zconfig_init()
{
    awss_debug("%s\r\n", __func__);

    zconfig_channel_locked = INVALID_CHANNEL;
    zconfig_finished = 0;

    zconfig_data = (struct zconfig_data *)os_zalloc(sizeof(struct zconfig_data));
    if (zconfig_data == NULL)
        goto ZCONFIG_INIT_FAIL;
    zc_mutex = os_mutex_init();
    if (zc_mutex == NULL)
        goto ZCONFIG_INIT_FAIL;

    if (awss_init_ieee80211_aplist())
        goto ZCONFIG_INIT_FAIL;

    if (awss_init_adha_aplist())
        goto ZCONFIG_INIT_FAIL;

#ifdef AWSS_SUPPORT_HT40
    ht40_init();
#endif
    return;

ZCONFIG_INIT_FAIL:
    awss_crit("malloc failed!\r\n");
    if (zconfig_data) {
        if (zc_mutex)
            os_mutex_destroy(zc_mutex);
        os_free(zconfig_data);
        zconfig_data = NULL;
    }

    awss_deinit_ieee80211_aplist();
    awss_deinit_adha_aplist();
    return;
}

void zconfig_destroy(void)
{
    if (zconfig_data) {
        if (zc_mutex)
            os_mutex_destroy(zc_mutex);
        os_free((void *)zconfig_data);
        zconfig_data = NULL;
    }
}

void zconfig_force_destroy(void)
{
    zconfig_destroy();

    awss_deinit_ieee80211_aplist();
    awss_deinit_adha_aplist();

    awss_close_aplist_monitor();
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
