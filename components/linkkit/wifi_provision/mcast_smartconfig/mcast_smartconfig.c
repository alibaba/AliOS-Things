#include "wifi_provision_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
#ifndef DISABLE_SMARTCONFIG_MCAST

#define SUCCESS_RETURN (0)
#define FAILURE_RETURN (-1)
static int processed_packet = 0;
static uint8_t mcast_bssid_mac[ETH_ALEN] = {0};
static uint8_t mcast_src_mac[ETH_ALEN] = {0};
static int mcast_locked_channel = -1;
static struct mcast_smartconfig_data_type mcast_smartconfig_data = {0};
static uint8_t receive_record[MCAST_MAX_LEN] = {0};

void reset_mcast_data() {
    processed_packet = 0;
    memset(mcast_bssid_mac, 0, ETH_ALEN);
    memset(mcast_src_mac, 0, ETH_ALEN);
    mcast_locked_channel = -1;
    memset(&(mcast_smartconfig_data), 0, sizeof(struct mcast_smartconfig_data_type));
    memset(receive_record, 0, MCAST_MAX_LEN);
}

int mcast_receive_done()
{
    int iter = 0;
    while (iter < mcast_smartconfig_data.tlen) {
        if (0 == receive_record[iter++]) {
            return FAILURE_RETURN;
        }
    }
    return SUCCESS_RETURN;
}

int decode_passwd()
{
    int passwd_len = 0;
    /* CAN'T use snawss_debug here, because of SPACE char */
    passwd_len = mcast_smartconfig_data.passwd_len;
    memset(zc_passwd, 0, ZC_MAX_PASSWD_LEN);
    aes_decrypt_string((char *)(mcast_smartconfig_data.passwd), (char *)zc_passwd,
                       passwd_len,
                       1, awss_get_encrypt_type(), 0, NULL);
    if (is_utf8((const char *)zc_passwd, passwd_len) == 0) {
        awss_trace("passwd err\r\n");
        awss_event_post(IOTX_AWSS_PASSWD_ERR);
        AWSS_UPDATE_STATIS(AWSS_STATIS_SM_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
        return FAILURE_RETURN;
    }
    return SUCCESS_RETURN;
}

int verify_checksum()
{
    int ret = 0;
    uint16_t cal_crc = zconfig_checksum_v5(&(mcast_smartconfig_data.data[0]), mcast_smartconfig_data.tlen - 1);
    uint8_t crc = mcast_smartconfig_data.checksum;
    cal_crc = (cal_crc & 0xFF);
    ret = cal_crc - crc;
    awss_debug("mcast: verify checksum diff is %d, cal_crc, crc is %d,%d\n", ret, cal_crc, crc);
    return ret;

}

void set_zc_ssid(int valid_bssid)
{
#ifdef AWSS_SUPPORT_APLIST
    do {  /* amend SSID automatically */
        struct ap_info *ap = NULL;
        if (SUCCESS_RETURN != valid_bssid) {
            break;
        }
        ap = zconfig_get_apinfo(zc_bssid);
        if (ap == NULL || ap->ssid[0] == '\0') {
            break;
        }
        strncpy((char *)zc_ssid, (const char *)ap->ssid, ZC_MAX_SSID_LEN - 1);
        return;
    } while (0);
#endif
    /* if we can't find ssid form aplist, we should take the default one */
    strncpy((char *)zc_ssid, (char const *)mcast_smartconfig_data.ssid, mcast_smartconfig_data.ssid_len);
    return;
}

int set_zc_bssid()
{
    /* compare the last 3 bytes of bssid from apist with the one from app */
    if (!memcmp(mcast_smartconfig_data.bssid, (uint8_t *)mcast_bssid_mac + 3, 3)) {
        memcpy(zc_bssid, mcast_bssid_mac, ETH_ALEN);
        awss_debug("mcast_bssid_mac from aplist");
        return SUCCESS_RETURN;
    } else {
#ifdef AWSS_SUPPORT_APLIST
        struct ap_info *ap_info = zconfig_get_apinfo_by_3_byte_mac(mcast_smartconfig_data.bssid);
        if (NULL != ap_info) {
            memcpy(zc_bssid, ap_info->mac, 6);
            awss_debug("bssid1: bssid is %x,%x,%x,%x,%x,%x", zc_bssid[0], zc_bssid[1], zc_bssid[2],
                       zc_bssid[3], zc_bssid[4], zc_bssid[5]);
            return SUCCESS_RETURN;
        }
#endif
    }
    return -1;
}

void gen16ByteToken()
{
    int bssid_len = mcast_smartconfig_data.bssid_type_len & 0b11111;
    awss_complete_token((char *)zc_passwd, mcast_smartconfig_data.bssid, bssid_len,
                        mcast_smartconfig_data.token, mcast_smartconfig_data.token_len, zc_token);
}

int parse_result()
{
    int offset = 0;
    int ret, valid_bssid;
    uint8_t BIT0_passwd = 0;
    uint8_t BIT1_ssid = 0;
    uint8_t BIT2_token = 0;
    uint8_t BIT6_7_version = 0;

    awss_debug("mcast: tlen is %d\n", mcast_smartconfig_data.tlen);
    mcast_smartconfig_data.flag = mcast_smartconfig_data.data[1];
    offset = 2;
    awss_debug("mcast: flag is %d\n", mcast_smartconfig_data.flag);

    BIT0_passwd = 0x1 & mcast_smartconfig_data.flag;
    BIT1_ssid = 0x1 & (mcast_smartconfig_data.flag >> 1);
    BIT2_token = 0x1 & (mcast_smartconfig_data.flag >> 2);
    BIT6_7_version = 0x3 & (mcast_smartconfig_data.flag >> 6);

    if (0x3 != BIT6_7_version) {
        awss_err("error version");
        return -1;
    }

    if (1 == BIT0_passwd) {
        mcast_smartconfig_data.passwd_len = mcast_smartconfig_data.data[2];
        awss_debug("mcast: passwd_len is %d\n", mcast_smartconfig_data.passwd_len);
        mcast_smartconfig_data.passwd = &(mcast_smartconfig_data.data[3]);
        offset = 3 + mcast_smartconfig_data.passwd_len;
    }

    if (1 == BIT2_token) {
        mcast_smartconfig_data.token_len = mcast_smartconfig_data.data[offset];
        awss_debug("mcast: token_len is %d\n", mcast_smartconfig_data.token_len);
        offset++;
        mcast_smartconfig_data.token = &(mcast_smartconfig_data.data[offset]);
        awss_debug("mcast: token is %.*s\n", mcast_smartconfig_data.token_len, mcast_smartconfig_data.token);
        offset += mcast_smartconfig_data.token_len;
    }

    if (1 == BIT1_ssid) {
        mcast_smartconfig_data.ssid_len = mcast_smartconfig_data.data[offset];
        awss_debug("mcast: ssid_len is %d\n", mcast_smartconfig_data.ssid_len);
        offset++;
        mcast_smartconfig_data.ssid = &(mcast_smartconfig_data.data[offset]);
        awss_debug("mcast: ssid is %.*s\n", mcast_smartconfig_data.ssid_len, mcast_smartconfig_data.ssid);
        offset += mcast_smartconfig_data.ssid_len;
    }

    mcast_smartconfig_data.bssid_type_len = mcast_smartconfig_data.data[offset];
    awss_debug("mcast: bssid_type_len is %d\n", mcast_smartconfig_data.bssid_type_len);
    offset++;
    mcast_smartconfig_data.bssid = &(mcast_smartconfig_data.data[offset]);
    /* get bssid len from last 5 bits from bssid_type_len */
    offset += mcast_smartconfig_data.bssid_type_len & 0b11111;
    mcast_smartconfig_data.checksum = mcast_smartconfig_data.data[offset];
    awss_debug("mcast: checksum is %d\n", mcast_smartconfig_data.checksum);
    awss_debug("mcast: total processed %d packagets \n", processed_packet);

    /* set zc_bssid*/
    valid_bssid = set_zc_bssid();
    /* set zc_ssid */
    set_zc_ssid(valid_bssid);
    ret = verify_checksum();
    if (SUCCESS_RETURN != ret) {
        awss_err("mcast: checksum mismatch\n");
        return -1;
    }
    ret = decode_passwd();
    gen16ByteToken();
    reset_mcast_data();
    if (SUCCESS_RETURN != ret) {
        awss_err("mcast: passwd error\n");
        return -1;
    }
    /* TODO: check channel info*/
    zconfig_set_state(STATE_RCV_DONE, 0, mcast_locked_channel);
    return SUCCESS_RETURN;
}

int awss_ieee80211_mcast_smartconfig_process(uint8_t *ieee80211, int len, int link_type, struct parser_res *res,
        signed char rssi)
{
    int hdrlen, fc, seq_ctrl;
    struct ieee80211_hdr *hdr;
    uint8_t *data, *bssid_mac, *dst_mac;
    uint8_t encry = ZC_ENC_TYPE_INVALID;
    uint8_t tods;

    /*
     * when device try to connect current router (include aha)
     * skip the new packet.
     */
    if (ieee80211 == NULL || zconfig_finished) {
        return ALINK_INVALID;
    }

    /*
     * we don't process smartconfig until user press configure button
     */
    if (awss_get_config_press() == 0) {
        return ALINK_INVALID;
    }

    hdr = (struct ieee80211_hdr *)ieee80211;
    fc = hdr->frame_control;
    seq_ctrl = hdr->seq_ctrl;

    /*
     * for smartconfig with bcast of data
     */
    if (!ieee80211_is_data_exact(fc)) {
        return ALINK_INVALID;
    }

    /* tods = 1, fromds = 0 || tods = 0, fromds = 1 */
    if (ieee80211_has_tods(fc) == ieee80211_has_fromds(fc)) {
        return ALINK_INVALID;
    }

    /* drop frag, more, order*/
    if (ieee80211_has_frags(fc)) {
        return ALINK_INVALID;
    }

    dst_mac = (uint8_t *)ieee80211_get_DA(hdr);
    /* only multicast is passed */
    if (0x1 != dst_mac[0] || 0x0 != dst_mac[1] || 0x5e != dst_mac[2]) {
#ifdef VERBOSE_MCAST_DEBUG
        awss_debug("error type, %x, %x, %x\n", dst_mac[0], dst_mac[1], dst_mac[2]);
#endif
        return ALINK_INVALID;    /* only handle br frame */
    }

    bssid_mac = (uint8_t *)ieee80211_get_BSSID(hdr);

    /*
     * payload len = frame.len - (radio_header + wlan_hdr)
     */
    hdrlen = ieee80211_hdrlen(fc);
    if (hdrlen > len) {
        return ALINK_INVALID;
    }

#ifdef _PLATFORM_QCOM_
    /* Note: http://stackoverflow.com/questions/17688710/802-11-qos-data-frames */
    hdrlen = (hdrlen + 3) & 0xFC;/* align header to 32bit boundary */
#endif

    res->u.br.data_len = len - hdrlen;       /* eating the hdr */
    res->u.br.sn = IEEE80211_SEQ_TO_SN(os_le16toh(seq_ctrl));

    data = ieee80211 + hdrlen;               /* eating the hdr */
    tods = ieee80211_has_tods(fc);

    do {
#ifdef AWSS_SUPPORT_APLIST
        struct ap_info *ap_info;
        ap_info = zconfig_get_apinfo(bssid_mac);
        if (ap_info && ZC_ENC_TYPE_INVALID != ap_info->encry[tods]) {
            encry = ap_info->encry[tods];
        } else
#endif
        {
            if (!ieee80211_has_protected(fc)) {
                encry = ZC_ENC_TYPE_NONE;
            } else {
                /* Note: avoid empty null data */
                if (len < 8) {      /* IV + ICV + DATA >= 8 */
                    return ALINK_INVALID;
                }
                if (!(ieee80211[3] & 0x3F)) {
                    encry = ZC_ENC_TYPE_WEP;
                } else if (data[3] & (1 << 5)) {/* Extended IV */
                    if (data[1] == ((data[0] | 0x20) & 0x7F)) { /* tkip, WEPSeed  = (TSC1 | 0x20 ) & 0x7F */
                        encry = ZC_ENC_TYPE_TKIP;
                    }
                    if (data[2] == 0 && (!(data[3] & 0x0F))) {
                        encry = ZC_ENC_TYPE_AES;
                    }

                    /*
                     * Note: above code use if(tkip) and if(ase)
                     * instead of if(tkip) else if(aes)
                     * beacause two condition may bother match.
                     */
                }
            }
        }
    } while (0);

    if (encry == ZC_ENC_TYPE_INVALID) {
        awss_warn("invalid encry type!\r\n");
    }
    res->u.br.encry_type = encry;

    /* convert IEEE 802.11 header + possible LLC headers into Ethernet header
     * IEEE 802.11 address fields:
     * ToDS FromDS Addr1 Addr2 Addr3 Addr4
     *   0     0   DA    SA    BSSID n/a
     *   0     1   DA    BSSID SA    n/a
     *   1     0   BSSID SA    DA    n/a
     *   1     1   RA    TA    DA    SA
     */
    res->src = ieee80211_get_SA(hdr);
    res->dst = ieee80211_get_DA(hdr);
    res->bssid = ieee80211_get_BSSID(hdr);
    res->tods = ieee80211_has_tods(fc);

    return ALINK_BROADCAST;
}
int static get_all = 0;
int find_channel_from_aplist = 0;

int lock_mcast_channel(struct parser_res *res, int encry_type)
{
#ifdef AWSS_SUPPORT_APLIST
    /* fix channel with apinfo if exist, otherwise return anyway. */
    do {
        struct ap_info *ap_info = zconfig_get_apinfo(res->bssid);
        extern void aws_set_dst_chan(int channel);
        int tods = res->tods;

        if (ap_info && ap_info->encry[tods] == encry_type && ap_info->channel) {
            if (res->channel != ap_info->channel) {
                awss_info("fix channel from %d to %d\r\n", res->channel, ap_info->channel);
                zc_channel = ap_info->channel;  /* fix by ap_info channel */
                aws_set_dst_chan(zc_channel);
            }
        } else {
            /* warning: channel may eq 0! */
        };

        find_channel_from_aplist = 1;
        zconfig_set_state(STATE_CHN_LOCKED_BY_MCAST, 0, res->channel);
        mcast_locked_channel = res->channel;
    } while (0);
#endif
    return  SUCCESS_RETURN;
}

int awss_recv_callback_mcast_smartconfig(struct parser_res *res)
{
    uint8_t index = *(res->dst + 3)  & (0x7f) ;
    uint8_t payload1 =  *(res->dst + 4);
    uint8_t payload2 = *(res->dst + 5);
    uint8_t frame_offset;
    uint16_t len;
    uint8_t encry_type;

    /* since put 2 bytes one time, so it has to mulitplied by 2 */
    index = index << 1;
    if (1 == get_all) {
        return FAILURE_RETURN;
    }

    if (index > MCAST_MAX_LEN) {
#ifdef VERBOSE_MCAST_DEBUG
        awss_debug("error index\n");
#endif
        return FAILURE_RETURN;
    }
    len = res->u.br.data_len;
    encry_type = res->u.br.encry_type;

    frame_offset = zconfig_fixed_offset[encry_type][0];

    len -= frame_offset;
    if (len != 1) {
#ifdef VERBOSE_MCAST_DEBUG
        awss_debug("error len, len is %d\n", len);
#endif
        return FAILURE_RETURN;
    }
    /* Filter out interference */
    /*
         * 1) src not equal, bssid equal, interference
         * 2) src not equal, bssid not equal, interference
         * 3) src equal, bssid equal, good, go on
         * 4) src equal, bssid not equal
    */

    if (!memcmp(mcast_src_mac, zero_mac, ETH_ALEN)) {
        memcpy(mcast_bssid_mac, res->bssid, ETH_ALEN);
        memcpy(mcast_src_mac, res->src, ETH_ALEN);
    } else {
        /* case 1, 2 */
        if(memcmp(mcast_src_mac, res->src, ETH_ALEN)) {
            return FAILURE_RETURN;
        }
    }

    /* lock channel */
    if (0 == find_channel_from_aplist) {
        lock_mcast_channel(res, encry_type);
    }
    processed_packet++;

    awss_debug("mcast: index is %d, %d, %d, %d\n", index, payload1, payload2, len);

    mcast_smartconfig_data.data[index] = payload1;
    receive_record[index] = 1;
    index++;
    mcast_smartconfig_data.data[index] = payload2;
    receive_record[index] = 1;

    if (0 != receive_record[0]) {
        int remain = -1;
        mcast_smartconfig_data.tlen = mcast_smartconfig_data.data[0];
        remain = mcast_receive_done();
        if (0 == remain) {
            get_all = 1;
            parse_result();
        }
    }
    return PKG_MCAST_FRAME;
}
#endif
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */

#endif
