/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#ifdef AWSS_SUPPORT_AHA

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AHA_SA_OFFSET           (10)
#define AHA_PROBE_PKT_LEN       (49)

const char *zc_default_ssid = "aha";
const char *zc_default_passwd = "12345678";

static const uint8_t aha_probe_req_frame[AHA_PROBE_PKT_LEN] = {
    0x40, 0x00,  /* mgnt type, frame control */
    0x00, 0x00,  /* duration */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* DA */
    0x28, 0xC2, 0xDD, 0x61, 0x68, 0x83,  /* SA, to be replaced with wifi mac */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* BSSID */
    0xC0, 0x79,  /* seq */
    0x00, 0x03, 0x61, 0x68, 0x61,        /* ssid, aha */
    0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x8C, 0x92, 0x98, 0xA4,  /* supported rates */
    0x32, 0x04, 0xB0, 0x48, 0x60, 0x6C,  /* extended supported rates */
    0x3F, 0x84, 0x10, 0x9E  /* FCS */
};

int awss_recv_callback_aha_ssid(struct parser_res *res)
{
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    awss_debug("found default ssid: %s\r\n", zc_default_ssid);
    AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_TIME_START);

    strncpy((char *)zc_ssid, zc_default_ssid, ZC_MAX_SSID_LEN - 1);
    strncpy((char *)zc_passwd, zc_default_passwd, ZC_MAX_PASSWD_LEN - 1);

    zconfig_set_state(STATE_RCV_DONE, tods, channel);
    return PKG_END;
}

int aws_send_aha_probe_req(void)
{
    uint8_t probe[AHA_PROBE_PKT_LEN];
    memcpy(probe, aha_probe_req_frame, sizeof(probe));
    os_wifi_get_mac(&probe[AHA_SA_OFFSET]);
    HAL_Wifi_Send_80211_Raw_Frame(FRAME_PROBE_REQ, probe, sizeof(probe));
    return 0;
}

int awss_ieee80211_aha_process(uint8_t *mgmt_header, int len, int link_type, struct parser_res *res, signed char rssi)
{
    uint8_t ssid[ZC_MAX_SSID_LEN] = {0}, bssid[ETH_ALEN] = {0};
    uint8_t auth, pairwise_cipher, group_cipher;
    struct ieee80211_hdr *hdr;
    int fc, ret, channel;

    /*
     * when device try to connect current router (include aha)
     * skip the new aha and process the new aha in the next scope.
     */
    if (mgmt_header == NULL || zconfig_finished)
        return ALINK_INVALID;

    /*
     * we don't process aha until user press configure button
     */
    if (awss_get_config_press() == 0)
        return ALINK_INVALID;

    hdr = (struct ieee80211_hdr *)mgmt_header;
    fc = hdr->frame_control;

    if (!ieee80211_is_beacon(fc) && !ieee80211_is_probe_resp(fc))
        return ALINK_INVALID;
    ret = ieee80211_get_bssid(mgmt_header, bssid);
    if (ret < 0)
        return ALINK_INVALID;

    ret = ieee80211_get_ssid(mgmt_header, len, ssid);
    if (ret < 0)
        return ALINK_INVALID;

    /*
     * skip ap which is not aha
     */
    if (strcmp((const char *)ssid, zc_default_ssid))
        return ALINK_INVALID;
    channel = cfg80211_get_bss_channel(mgmt_header, len);
    rssi = rssi > 0 ? rssi - 256 : rssi;

    cfg80211_get_cipher_info(mgmt_header, len, &auth,
                             &pairwise_cipher, &group_cipher);
    awss_save_apinfo(ssid, bssid, channel, auth,
                     pairwise_cipher, group_cipher, rssi);
   return ALINK_DEFAULT_SSID;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
