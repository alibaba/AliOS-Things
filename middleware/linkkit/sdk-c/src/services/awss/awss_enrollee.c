/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stddef.h>
#include "aws_lib.h"
#include "os.h"
#include "awss.h"
#include "awss_enrollee.h"
#include "awss_main.h"
#include "passwd.h"
#include "awss_cmp.h"
#include "awss_packet.h"
#include "awss_wifimgr.h"
#include "awss_statis.h"
#include "awss_crypt.h"
#include "zconfig_utils.h"
#include "zconfig_ieee80211.h"

#ifndef AWSS_DISABLE_ENROLLEE

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

const uint8_t probe_req_frame[ZC_PROBE_LEN] = {
    0x40, 0x00,  /* mgnt type, frame control */
    0x00, 0x00,  /* duration */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* DA */
    0x28, 0xC2, 0xDD, 0x61, 0x68, 0x83,  /* SA, to be replaced with wifi mac */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* BSSID */
    0xC0, 0x79,  /* seq */
    0x00, 0x00,  /* hide ssid, */
    0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x8C, 0x92, 0x98, 0xA4,  /* supported rates */
    0x32, 0x04, 0xB0, 0x48, 0x60, 0x6C,  /* extended supported rates */
    0x3F, 0x84, 0x10, 0x9E  /* FCS */
};

enum {
    ENROLLEE_ERR_CODE_NO_ERR = 0,
    ENROLLEE_ERR_CODE_DISABLE,
    ENROLLEE_ERR_CODE_SSID_ERR,
    ENROLLEE_ERR_CODE_PASSWD_ERR,
    ENROLLEE_ERR_CODE_GOT_SSID_PASSWD,
    ENROLLEE_ERR_CODE_CONN_ROUTER_ERR,
};

#ifdef ENROLLEE_REPORT_STATUS
static uint8_t *g_err_code;    /* pointer to error code of enrollee */
#endif
static uint8_t *g_dev_sign;    /* pointer to dev_name_len start pos */
static uint8_t *g_product_key; /* pointer to model_len start pos */
static uint8_t *enrollee_frame;
static uint16_t enrollee_frame_len;

static int decrypt_ssid_passwd(uint8_t *ie, uint8_t ie_len,
                               uint8_t out_ssid[OS_MAX_SSID_LEN],
                               uint8_t out_passwd[OS_MAX_PASSWD_LEN],
                               uint8_t out_bssid[ETH_ALEN]);

void awss_enrollee_connect_router_fail(int reson)
{
#ifdef ENROLLEE_REPORT_STATUS
    if (g_err_code) *g_err_code = ENROLLEE_ERR_CODE_CONN_ROUTER_ERR;
#endif
}

void awss_init_enrollee_info(void)
{
    char *pk = NULL, *dev_name = NULL, *text = NULL;
    uint8_t sign[ENROLLEE_SIGN_SIZE + 1] = {0};
    char key[OS_DEVICE_SECRET_LEN + 1] = {0};
    int dev_name_len, pk_len;
    int len, ie_len;

    if (enrollee_frame_len) {
        return;
    }

    dev_name = awss_zalloc(OS_DEVICE_NAME_LEN + 1);
    pk = awss_zalloc(OS_PRODUCT_KEY_LEN + 1);

    os_product_get_key(pk);
    pk_len = strlen(pk);

    os_device_get_name(dev_name);
    dev_name_len = strlen(dev_name);

    len = RANDOM_MAX_LEN + dev_name_len + pk_len;
    text = awss_zalloc(len + 1); /* +1 for string print */

    awss_build_sign_src(text, &len);
    if (os_get_conn_encrypt_type() == 3) { /* aes-key per product */
        os_product_get_secret(key);
    } else { /* aes-key per device */
        os_device_get_secret(key);
    }
    produce_signature(sign, (uint8_t *)text, len, key);

    awss_free(text);

    ie_len = pk_len + dev_name_len + ENROLLEE_IE_FIX_LEN;
    enrollee_frame_len = sizeof(probe_req_frame) + ie_len;

    enrollee_frame = awss_zalloc(enrollee_frame_len);

    /* construct the enrollee frame right now */
    len = sizeof(probe_req_frame) - FCS_SIZE;
    memcpy(enrollee_frame, probe_req_frame, len);

    enrollee_frame[len ++] = 221; /* vendor ie */
    enrollee_frame[len ++] = ie_len - 2; /* exclude 221 & len */
    enrollee_frame[len ++] = 0xD8;
    enrollee_frame[len ++] = 0x96;
    enrollee_frame[len ++] = 0xE0;
    enrollee_frame[len ++] = 0xAA;/* OUI type */
    enrollee_frame[len ++] = DEVICE_TYPE_VERSION;/* version & dev type */

    enrollee_frame[len ++] = dev_name_len;/* dev name len*/
    memcpy(&enrollee_frame[len], dev_name, dev_name_len);
    len += dev_name_len;

    enrollee_frame[len ++] = ENROLLEE_FRAME_TYPE;/* frame type */

    g_product_key = &enrollee_frame[len]; /* pointer to pk len, see decrypt func */
    enrollee_frame[len ++] = pk_len;
    memcpy(&enrollee_frame[len], pk, pk_len);
    len += pk_len;

    enrollee_frame[len ++] = RANDOM_MAX_LEN;
    memcpy(&enrollee_frame[len], aes_random, RANDOM_MAX_LEN);
    len += RANDOM_MAX_LEN;

    enrollee_frame[len ++] = os_get_conn_encrypt_type();  /* encrypt type */
    enrollee_frame[len ++] = 0;  /* signature method, 0: hmacsha1, 1: hmacsha256 */
    enrollee_frame[len ++] = ENROLLEE_SIGN_SIZE;  /* signature length */
    g_dev_sign = &enrollee_frame[len];
    memcpy(&enrollee_frame[len], sign, ENROLLEE_SIGN_SIZE);
    len += ENROLLEE_SIGN_SIZE;
#ifdef ENROLLEE_REPORT_STATUS
    g_err_code = &enrollee_frame[len ++];
    if (awss_get_config_press() == 0)
        *g_err_code = ENROLLEE_ERR_CODE_DISABLE;
#endif

    memcpy(&enrollee_frame[len],
           &probe_req_frame[sizeof(probe_req_frame) - FCS_SIZE], FCS_SIZE);

    /* update probe request frame src mac */
    os_wifi_get_mac(enrollee_frame + SA_POS);

    awss_free(pk);
    awss_free(dev_name);
}

void awss_destroy_enrollee_info(void)
{
    if (enrollee_frame_len) {
        awss_free(enrollee_frame);
        enrollee_frame_len = 0;
        enrollee_frame = NULL;
        g_product_key = NULL;
        g_dev_sign = NULL;
#ifdef ENROLLEE_REPORT_STATUS
        g_err_code = NULL;
#endif
    }
}

void awss_broadcast_enrollee_info(void)
{
    if (enrollee_frame_len == 0 || enrollee_frame == NULL) {
        return;
    }

#ifdef ENROLLEE_REPORT_STATUS
    if (awss_get_config_press() == 0) {
        *g_err_code = ENROLLEE_ERR_CODE_DISABLE;
    } else if (*g_err_code == ENROLLEE_ERR_CODE_DISABLE) {
        *g_err_code = ENROLLEE_ERR_CODE_NO_ERR;
    }
#endif

    os_wifi_send_80211_raw_frame(FRAME_PROBE_REQ, enrollee_frame,
                                 enrollee_frame_len);
}

/* return 0 for success, -1 dev_name not match, otherwise return -2 */
static int decrypt_ssid_passwd(
            uint8_t *ie, uint8_t ie_len,
            uint8_t out_ssid[OS_MAX_SSID_LEN],
            uint8_t out_passwd[OS_MAX_PASSWD_LEN],
            uint8_t out_bssid[ETH_ALEN])
{
    uint8_t tmp_ssid[OS_MAX_SSID_LEN + 1] = {0}, tmp_passwd[OS_MAX_PASSWD_LEN + 1] = {0};
    uint8_t *p_dev_name_sign = NULL, *p_ssid = NULL, *p_passwd = NULL, *p_bssid = NULL;

    /* ignore ie hdr: 221, len, oui[3], type(0xAB) */
#define REGISTRAR_IE_HDR    (6)
    ie += REGISTRAR_IE_HDR;
    if (ie[0] != DEVICE_TYPE_VERSION) {
        awss_debug("devtype/ver=%d not supported!", ie[0]);
        return -1;
    }

    ie ++;  /* skip version */
    p_dev_name_sign = ie;

    if (!g_dev_sign || memcmp(g_dev_sign, p_dev_name_sign + 1, p_dev_name_sign[0])) {
        p_dev_name_sign[p_dev_name_sign[0]] = '\0';
        awss_debug("dev_name not match, expect:");
        if (g_dev_sign) {
            dump_hex(g_dev_sign, p_dev_name_sign[0], 16);
        }
        awss_debug("\nbut recv:");
        dump_hex(p_dev_name_sign + 1, p_dev_name_sign[0], 16);
        return -2;
    }
    ie += ie[0] + 1; /* eating device name sign length & device name sign[n] */

    if (ie[0] != REGISTRAR_FRAME_TYPE) {
        awss_debug("frametype=%d not supported!", ie[0]);
        return -1;
    }

    ie ++;  /* eating frame type */
    p_ssid = ie;
    if (ie[0] >= OS_MAX_SSID_LEN) {
        awss_debug("ssidlen=%d invalid!", ie[0]);
#ifdef ENROLLEE_REPORT_STATUS
        if (g_err_code) *g_err_code = ENROLLEE_ERR_CODE_SSID_ERR;
#endif
        return -1;
    }
    memcpy(tmp_ssid, &p_ssid[1], p_ssid[0]);

    ie += ie[0] + 1; /* eating ssid_len & ssid[n] */

    p_passwd = ie;
    if (p_passwd[0] >= OS_MAX_PASSWD_LEN) {
        awss_debug("passwdlen=%d invalid!", p_passwd[0]);
#ifdef ENROLLEE_REPORT_STATUS
        if (g_err_code) *g_err_code = ENROLLEE_ERR_CODE_PASSWD_ERR;
#endif
        return -1;
    }

    ie += ie[0] + 1; /* eating passwd_len & passwd */

    p_bssid = ie;
    ie += ETH_ALEN; /* eating bssid len */

    AWSS_UPDATE_STATIS(AWSS_STATIS_ZCONFIG_IDX, AWSS_STATIS_TYPE_TIME_START);

    aes_decrypt_string((char *)p_passwd + 1, (char *)tmp_passwd, p_passwd[0],
            1, os_get_conn_encrypt_type(), 0, (const char *)aes_random); /*aes128 cfb*/
    if (is_utf8((const char *)tmp_passwd, p_passwd[0]) != 1) {
        awss_debug("passwd invalid!");
        AWSS_UPDATE_STATIS(AWSS_STATIS_ZCONFIG_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
#ifdef ENROLLEE_REPORT_STATUS
        if (g_err_code) *g_err_code = ENROLLEE_ERR_CODE_PASSWD_ERR;
#endif
        return -1;
    }
    awss_trace("enrollee ssid:%s\r\n", tmp_ssid);

    strncpy((char *)out_passwd, (const char *)tmp_passwd, OS_MAX_PASSWD_LEN - 1);
    strncpy((char *)out_ssid, (const char *)tmp_ssid, OS_MAX_SSID_LEN - 1);
    memcpy((char *)out_bssid, (char *)p_bssid, ETH_ALEN);

    return 0;/* success */
}

int awss_ieee80211_zconfig_process(uint8_t *mgmt_header, int len, int link_type, struct parser_res *res, signed char rssi)
{
    const uint8_t *registrar_ie = NULL;
    struct ieee80211_hdr *hdr;
    uint16_t ieoffset;
    int fc;

    /*
     * when device try to connect current router (include adha and aha)
     * skip the new aha and process the new aha in the next scope.
     */
    if (mgmt_header == NULL || zconfig_finished) {
        return ALINK_INVALID;
    }
    /*
     * we don't process zconfig used by enrollee until user press configure button
     */
    if (awss_get_config_press() == 0) {
        return ALINK_INVALID;
    }

    hdr = (struct ieee80211_hdr *)mgmt_header;
    fc = hdr->frame_control;

    if (!ieee80211_is_probe_req(fc) && !ieee80211_is_probe_resp(fc)) {
        return ALINK_INVALID;
    }

    ieoffset = offsetof(struct ieee80211_mgmt, u.probe_resp.variable);
    if (ieoffset > len) {
        return ALINK_INVALID;
    }

    registrar_ie = (const uint8_t *)cfg80211_find_vendor_ie(WLAN_OUI_ALIBABA,
                   WLAN_OUI_TYPE_REGISTRAR, mgmt_header + ieoffset, len - ieoffset);
    if (registrar_ie == NULL) {
        return ALINK_INVALID;
    }

    res->u.ie.alink_ie_len = len - (registrar_ie - mgmt_header);
    res->u.ie.alink_ie = (uint8_t *)registrar_ie;

    return ALINK_ZERO_CONFIG;
}

int awss_recv_callback_zconfig(struct parser_res *res)
{
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    uint8_t *ie = res->u.ie.alink_ie;
    uint8_t ie_len = ie[1];
    int ret;

    if (res->u.ie.alink_ie_len < ie_len) {
        return PKG_INVALID;
    }

    ret = decrypt_ssid_passwd(ie, ie_len, zc_ssid, zc_passwd, zc_bssid);
    if (ret) {
        return PKG_INVALID;
    }

    zconfig_set_state(STATE_RCV_DONE, tods, channel);

    AWSS_UPDATE_STATIS(AWSS_STATIS_ZCONFIG_IDX, AWSS_STATIS_TYPE_TIME_SUC);

    return PKG_END;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
