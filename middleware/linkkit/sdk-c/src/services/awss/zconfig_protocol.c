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

#ifdef AWSS_SUPPORT_HT40
#include "awss_ht40.h"
#endif

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
            i ++;
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

    if (!ap_info)
        return 0;

    if (auth)
        *auth = ap_info->auth;
    if (encry)
        *encry = ap_info->encry[1];    /* tods side */
    if (!valid_bssid && bssid)
        memcpy(bssid, ap_info->mac, ETH_ALEN);
    if (channel)
        *channel = ap_info->channel;

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
            if (!is_channel_locked())
                zconfig_callback_channel_locked(channel);
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
    if (pkt_length < MIN_PKG || pkt_length > MAX_PKG)
        return 1;
    return 0;
}

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
