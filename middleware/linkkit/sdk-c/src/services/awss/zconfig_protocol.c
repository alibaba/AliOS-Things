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
#include "awss_main.h"
#include "awss_timer.h"
#include "awss.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/* broadcast mac address */
const uint8_t br_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/* all zero mac address */
const uint8_t zero_mac[] = {0, 0, 0, 0, 0, 0};

/* invalid channel, neither 0, 0xff, nor 1-13 */
#define INVALID_CHANNEL     (20)
#define PAYLOAD_BITS_CNT    (7)
#define PAYLOAD_BITS_MASK   ((1 << PAYLOAD_BITS_CNT) - 1)

/* which channel lock at */
uint8_t zconfig_channel_locked = INVALID_CHANNEL;/* locked channel */
/*
 * avoid zconfig_callback_over() was called twice,
 * once from tods, once from fromds
 */
uint8_t zconfig_finished;

/* global data structure, which hold all broadcast data */
struct zconfig_data *zconfig_data;

//////////////////////////////////////////////////////////////////////////////////
//following is broadcast protocol related code
static inline uint8_t is_start_frame(uint16_t len)
{
    return (len == START_FRAME);
}

static inline uint8_t is_group_frame(uint16_t len)
{
    /* is group frame? */
    return (len > GROUP_FRAME && len <= GROUP_FRAME_END);
}

static inline uint8_t is_data_frame(uint16_t len)
{
    uint8_t group_frame, index;
    /* is start frame */
    if (is_start_frame(len))
        return 0;

    /* is group frame? */
    group_frame = is_group_frame(len);
    if (group_frame)
        return 0;

    index = (len >> PAYLOAD_BITS_CNT) & 0xF;
    return (index >= ZC_GRP_PKT_IDX_START && index <= ZC_GRP_PKT_IDX_END);
}

static inline uint8_t get_group_index(uint16_t len)
{
    if (is_start_frame(len))
        return 0;

    return (len - GROUP_FRAME) * GROUP_NUMBER;
}

static inline uint8_t get_data_index(uint16_t len)
{
    uint8_t index = (len >> PAYLOAD_BITS_CNT) & 0xF;  /* from 2 to 9 */
    return index - (ZC_GRP_PKT_IDX_START - 1);   /* adjust, from 1 to 8 */
}

#define sn_minus(a,b)    (((a) - (b)) & 0xfff)

/* a, b must be serial seq number */
static inline int sn_compare(uint16_t a, uint16_t b)
{
    /*
        case1: sn = 3, sn_prev = 5;            a < b
        case2: sn = 0, sn_prev = 0xfff;        a > b
        case3: sn = 4, sn_prev = 3;            a > b
    */
    uint16_t res = sn_minus(a, b);

    return res < 1000 ? res : -1;
}

/*
 * zconfig_get_data_len()
 *     here we guess the total_len of protocl message,
 *     base on the score of tods and fromds side.
 */
static inline int zconfig_get_data_len(void)
{
    uint8_t len;    /* total len, include len(1B) & crc(2B) */
    uint8_t score;

    /* tods > fromds */
    if (zconfig_data->pkg[1][1].score > zconfig_data->pkg[0][1].score) {
        len = zconfig_data->pkg[1][1].len & PAYLOAD_BITS_MASK;
        score = zconfig_data->pkg[1][1].score;
    } else {
        len = zconfig_data->pkg[0][1].len & PAYLOAD_BITS_MASK;
        score = zconfig_data->pkg[0][1].score;
    }

    if (len && score > score_mid) {
        //awss_debug("zconfig_get_data_len = %d\r\n", len);
        goto out;
    }

    if (zconfig_data->data[1].max_pos > zconfig_data->data[0].max_pos) {
        len = zconfig_data->data[1].max_pos;
    } else {
        len = zconfig_data->data[0].max_pos;
    }
out:
    if (len < GROUP_NUMBER) {
        return GROUP_NUMBER;
    } else {
        return len >= MAX_PKG_NUMS ? (MAX_PKG_NUMS - GROUP_NUMBER - 1) : len;
    }
}

/*
 * 8bit -> x bit
 *
 * serialize chinese char from 8bit to x bit
 */
static inline void encode_chinese(uint8_t *in, uint8_t in_len,
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
static inline void decode_chinese(uint8_t *in, uint8_t in_len,
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

/* check recv completed or not */
static inline int zconfig_recv_completed(uint8_t tods)
{
    int i;
    uint8_t len, flag, ssid_len, passwd_len;
    /*
        byte:    0    1        2    3        4        5    6
        name:        total_len    flag    ssid_len    passwd_len    ssid    ...
        index:        0x100        0x180    0x200        0x280        0x300    0x380
    */
    len = pkg_len(1) & PAYLOAD_BITS_MASK;/* total len, include len(1B) & crc(2B) */
    flag = pkg_len(2) & PAYLOAD_BITS_MASK;
    if (flag & SSID_EXIST_MASK) {/* ssid exist */
        ssid_len = pkg_len(3) & PAYLOAD_BITS_MASK;
        passwd_len = pkg_len(4) & PAYLOAD_BITS_MASK;
    } else {
        ssid_len = 0;
        passwd_len = pkg_len(3) & PAYLOAD_BITS_MASK;
    }

    if (!len || pkg_score(1) <= score_min) {
        //awss_trace("len=%d, pkg_score(1)=%d\r\n", len, pkg_score(1));
        return 0;
    }

#ifndef DISABLE_SSID_AUTO_COMPLETE
#define SSID_AUTO_COMPLETE_SCORE    (score_max + 1)
    /* ssid atuo-completion */
    if (zc_ssid[0] != '\0' && (flag & SSID_EXIST_MASK)
        && pkg_score(2) < SSID_AUTO_COMPLETE_SCORE
        && pkg_score(3) > score_mid
        && !zc_ssid_auto_complete_disable) {

        /* over-written ssid_len here */
        ssid_len = strlen((char const *)zc_ssid);
        if (ssid_len > ZC_MAX_SSID_LEN - 1)
            ssid_len = ZC_MAX_SSID_LEN - 1;

        if (!(flag & SSID_ENCODE_MASK)) {/* ASCLL ssid */
            if ((ssid_len | 0x200) != pkg_len(3)) {
                awss_warn("ssid len not match! ssid:%s != %d\r\n",
                        zc_ssid, (pkg_len(3) & ~0x200));
                zc_ssid_auto_complete_disable = 1;
                goto skip_ssid_auto_complete;
            }

            awss_trace("ssid auto-complete: %s\r\n", zc_ssid);
            pkg_score(2) = SSID_AUTO_COMPLETE_SCORE;

            pkg_len(3) = ssid_len | 0x200;    /* 0x200 is the index 3 */
            pkg_score(3) = SSID_AUTO_COMPLETE_SCORE;

            for (i = 5; i < ssid_len + 5; i ++) {
                pkg_len(i) = (zc_ssid[i - 5] - 32) | (0x100 + 0x80 * ((i - 1) % GROUP_NUMBER));
                pkg_score(i) =  SSID_AUTO_COMPLETE_SCORE;
            }
        } else if ((flag & SSID_ENCODE_MASK)) { /* include chinese ssid */
            uint8_t *buf, buf_len = 0;

            uint8_t ssid_encode_len = (ssid_len * 8 + 5) / 6;

            /*
             * for GBK encoded chinese, ssid auto-completion lead to crc error.
             * because Android APP may send utf8 encoded ssid.
             */
            if ((ssid_encode_len | 0x200) != pkg_len(3)) {
                zc_ssid_is_gbk = 1;
                zc_ssid_auto_complete_disable = 1;
                goto skip_ssid_auto_complete;
            } else {
                zc_ssid_is_gbk = 0;
            }

            buf = os_zalloc(ssid_encode_len + 1);
            if (buf == NULL)
                awss_crit("malloc failed!\r\n");

            awss_trace("chinese ssid auto-complete: %s\r\n", zc_ssid);
            encode_chinese(zc_ssid, ssid_len, buf, &buf_len, 6);

            pkg_score(2) = SSID_AUTO_COMPLETE_SCORE;

            pkg_len(3) = buf_len | 0x200;    /* 0x200 is the index 3 */
            pkg_score(3) =  SSID_AUTO_COMPLETE_SCORE;

            for (i = 5; i < buf_len + 5; i ++) {
                pkg_len(i) = buf[i - 5] | (0x100 + 0x80 * ((i - 1) % GROUP_NUMBER));
                pkg_score(i) =  SSID_AUTO_COMPLETE_SCORE;
            }
            os_free(buf);
        }
    }
#endif

skip_ssid_auto_complete:
    //awss_debug("expect len = %d, max len = %d\r\n", len, zc_max_pos);
    if (zc_max_pos < len)
        return 0;    // receive all the packets

    for (i = 1; i <= len; i ++) {  // check score for all the packets
        if (pkg_score(i) <= score_min) {
            return 0;
        }
    }

    /* 4 for total_len, flag, ssid_len, passwd_len, 2 for crc */
    if (flag & SSID_EXIST_MASK) { /* ssid exist */
        if (len != ssid_len + passwd_len + 4 + 2) {
            return 0;
        }
    } else if (len != passwd_len + 3 + 2) {
        return 0;
    }

    return 1;
}

static inline int zconfig_get_ssid_passwd(uint8_t tods)
{
    int i, ssid_len, package_num, passwd_len, ret;
    uint8_t *buf, *pbuf, *tmp, flag, passwd_encrypt, passwd_cipher_len = 0;
    uint16_t crc, cal_crc;
    uint8_t data, score;

    if (!zconfig_recv_completed(tods))
        return -1;

    buf = os_zalloc(256);
    tmp = os_zalloc(128);
    if (buf == NULL || tmp == NULL)
        awss_crit("malloc failed!\r\n");

    /* package num */
    package_num = pkg_len(1) & PAYLOAD_BITS_MASK;/* total len, include len(1B) & crc(2B) */

    awss_trace("\r\n");
    for (i = 1; i <= package_num; i ++) {
        data = pkg_len(i);
        score = pkg_score(i);
        buf[i - 1] = data & PAYLOAD_BITS_MASK;
        tmp[i - 1] = score;
    }

    dump_hex(&tmp[0], package_num, GROUP_NUMBER);
    awss_trace("\r\n");
    dump_hex(&buf[0], package_num, GROUP_NUMBER);
    awss_trace("\r\n");

    crc = os_get_unaligned_be16(&buf[package_num - 2]);

    pbuf = &buf[0]; /* len @ [0] */

    flag = pbuf[1];    /* flag @ [1] */
    pbuf += 2;        /* 2B for total_len, flag */

    passwd_encrypt = (flag & PASSWD_ENCRYPT_MASK) >> PASSWD_ENCRYPT_BIT_OFFSET;

    if (passwd_encrypt == PASSWD_ENCRYPT_CIPHER || passwd_encrypt == PASSWD_ENCRYPT_OPEN) {
        awss_trace("!aes128-cfb is not support: flag 0x%x\r\n", flag);
        ret = -1;
        goto exit;
    } else {
        cal_crc = zconfig_checksum_v3(buf, package_num - 2);
    }

    if (crc != cal_crc) {
        awss_trace("crc error: recv 0x%x != 0x%x\r\n", crc, cal_crc);
        //memset(zconfig_data, 0, sizeof(*zconfig_data));
        uint8_t tods_tmp = tods;
        for (tods = 0; tods < 2; tods ++) {
            for (i = 1; i <= package_num; i ++) {
                score = pkg_score(i);
                if (score > 0x60) {
                    pkg_score(i) = 0x60;
                } else {
                    pkg_score(i) = score >> 1;
                }
            }
        }
        tods = tods_tmp;
        ret = -1;
        awss_event_post(AWSS_CS_ERR);
        goto exit;
    }

    if (flag & SSID_EXIST_MASK) {/* ssid exist */
        ssid_len = pbuf[0];
        passwd_len = pbuf[1];
        pbuf += 2; /* 2B for ssid_len, passwd_len */

        if (!(flag & SSID_ENCODE_MASK))    {  // ascii
            /* CAN'T use snprintf here, because of SPACE char */
            memcpy((char *)tmp, pbuf, ssid_len);
            tmp[ssid_len] = '\0';
            for (i = 0; i < ssid_len; i++) {
                tmp[i] += 32;
            }
        } else {//chinese format
            decode_chinese(pbuf, ssid_len, tmp, NULL, 6);
            /* make sure 'tmp' is null-terminated */
        }
        pbuf += ssid_len; /* ssid */

        if (zc_ssid[0] == '\0' || zc_ssid_auto_complete_disable) {
            strncpy((char *)zc_ssid, (const char *)tmp, ZC_MAX_SSID_LEN - 1);
            awss_trace("SSID0: [%s]\r\n", zc_ssid);
        } else {
            if (!strncmp((const char *)tmp, (char *)zc_ssid, ZC_MAX_SSID_LEN - 1)) {
                awss_trace("SSID1: [%s]\r\n", zc_ssid);
            } else {
                if (zc_ssid_is_gbk) {
                    awss_trace("gbk SSID: [%s]\r\n", zc_ssid);
                } else {
                    awss_trace("gbk? SSID: [%s]\r\n", zc_ssid);
                }
            }
        }

        do {  // amend SSID automatically
            struct ap_info *ap = NULL;
            ap = zconfig_get_apinfo(zc_bssid);
            if (ap == NULL || ap->ssid[0] == '\0' ||
                strncmp(ap->ssid, zc_adha_ssid, ZC_MAX_SSID_LEN) == 0 ||
                strncmp(ap->ssid, zc_default_ssid, ZC_MAX_SSID_LEN) == 0) {
                memset(zc_bssid, 0, ETH_ALEN);
                break;
            }
            strncpy((char *)zc_ssid, (const char *)ap->ssid, ZC_MAX_SSID_LEN - 1);
        } while (0);
    } else {
        passwd_len = pbuf[0];
        pbuf += 1; /* 1B for passwd_len */
    }

    /* CAN'T use snprintf here, because of SPACE char */
    if (passwd_encrypt > PASSWD_ENCRYPT_CIPHER) {
        // decypt passwd using aes128-cfb
        decode_chinese(pbuf, passwd_len, tmp, &passwd_cipher_len, 6);
        passwd_len = passwd_cipher_len;
        memset(zc_passwd, 0, ZC_MAX_PASSWD_LEN);
        aes_decrypt_string((char *)tmp, (char *)zc_passwd, passwd_len, os_get_encrypt_type(), 0);
        if (is_utf8((const char *)zc_passwd, passwd_len) == 0) {
            awss_trace("passwd err\r\n");
            memset(zconfig_data, 0, sizeof(*zconfig_data));
            awss_event_post(AWSS_PASSWD_ERR);
            ret = -1;
            goto exit;
        }
    } else {
        memcpy((char *)tmp, pbuf, passwd_len);
        tmp[passwd_len] = '\0';
        for (i = 0; i < passwd_len; i++) {
            tmp[i] += 32;
        }
        strncpy((char *)zc_passwd, (const char *)tmp, ZC_MAX_PASSWD_LEN - 1);

        awss_trace("encrypt:%d not support\r\n", passwd_encrypt);
        memset(zconfig_data, 0, sizeof(*zconfig_data));
        ret = -1;
        goto exit;
    }


    // awss_debug("PASSWD: [%s]\r\n", zc_passwd);
    pbuf += passwd_len; /* passwd */
    ret = 0;
exit:
    os_free(buf);
    os_free(tmp);

    return ret;
}

enum _GOT_RESULT_ {
    GOT_NOTHING = 0,
    GOT_CHN_LOCK = 1,
    GOT_SSID_PASSWD = 2,
};

static int is_ascii_string(uint8_t *str)
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
 * passwd_check_utf8()
 *
 * @Note: see andriod smartconfig with p2p
 *     if the byte in passwd is zero, jave will change 0x00 with 0xc080
 *     the function will restore 0xc080 to 0x00
 */
static inline void passwd_check_utf8(uint8_t *passwd, int *passwd_len)
{
    int i, len;

    if (!passwd || !passwd_len) {
        return;
    }

    len = *passwd_len;
    for (i = 0; i < len; i ++) {
        if (passwd[i] < 0x80) { // [0x01 ~ 0x7F]
            continue;
        }
        passwd[i] = 0;  // resetore to 0x00
        if (i + 2 < len) { // move the rest to overwrite useless content.
            memmove(passwd + i + 1, passwd + i + 2, len - i - 2);
        }
        len --;
    }
    *passwd_len = len;
}

/*
 * get_ssid_passwd_from_wps()
 *
 * @Note: see andriod zconfig protocol
 *     android sdk limit sizeof(passwd) <= 23
 *
 * @Return: _GOT_RESULT_
 *
 * use src mac to do ssid completion
 */
static inline int get_ssid_passwd_from_w(uint8_t *in, int total_len, uint8_t *src, uint8_t *bssid)
{
    uint8_t tmp_ssid[ZC_MAX_SSID_LEN + 1] = {0}, tmp_passwd[ZC_MAX_PASSWD_LEN + 1] = {0};
    int ssid_len, passwd_len, ssid_truncated = 0;
    uint16_t crc, cal_crc;
    char encrypt = 0;
    /* used by prepare frame */
    char *magic_p_w_d = "zl&ws";//FIXME: Maybe it will be dangerous when opening source.
    static uint32_t start_time = 0;

#define W_LEN        (32)  // total_len
#define EXTRA_LEN    (3)   // ssid_len(1B) + checksum(2B)
    if (!in || total_len <= 4)
        return GOT_NOTHING;

    /* attr_id(2) + attr_len(2) = 4 */
    in += 4;
    total_len -= 4;

    if (total_len > W_LEN)
        awss_warn("ssid len > 32\r\n");

    /* total_len: ssid_len(1B), ssid, passwd, crc(2B) */
    ssid_len = in[0];
    if (ssid_len & P2P_ENCRYPT_BIT_MASK) {
        encrypt = (ssid_len & P2P_ENCRYPT_BIT_MASK) >> P2P_ENCODE_TYPE_OFFSET;
        ssid_len &= P2P_SSID_LEN_MASK;
    }
    passwd_len = total_len - ssid_len - EXTRA_LEN; /* ssid_len(1B), crc(2B) */
    if (ssid_len > W_LEN - EXTRA_LEN || passwd_len < 0)
        return GOT_NOTHING;

    crc = os_get_unaligned_be16(in + 1 + ssid_len + passwd_len);
    // restore 0xc080 to 0x00
    passwd_check_utf8(in + 1 + ssid_len, &passwd_len);
    cal_crc = zconfig_checksum_v3(in, 1 + ssid_len + passwd_len);
    if (crc != cal_crc) {
        awss_debug("wps crc check error: recv 0x%x != 0x%x\r\n", crc, cal_crc);
        awss_event_post(AWSS_CS_ERR);
        /*
         * use zconfig_checksum_v3() because
         * java modified UTF-8, U+C080 equal U+00,
         * ssid & passwd & len can not be 0, but checksum may result 0,
         * so add checksum_v2() which avoid use 0.
         *
         * also ssid_dict_decode_table[] avoid use 0
         */
        return GOT_NOTHING;
    }

    if (start_time == 0)
        start_time = zconfig_get_time();

#define MAC_LOCAL_ADMINISTERED_BIT    (0x02)
    memcpy(zc_android_src, src, ETH_ALEN);
    if (zc_android_src[0] & MAC_LOCAL_ADMINISTERED_BIT) {
        zc_android_src[0] &= ~MAC_LOCAL_ADMINISTERED_BIT;
        //awss_debug("android src: %02x%02x%02x\r\n", zc_android_src[0], src[1], src[2]);
    } else {
        awss_warn("local administered bit not set: %02x%02x%02x\r\n",
                src[0], src[1], src[2]);
    }

    in += 1;/* eating ssid_len(1B) */

    memset(tmp_ssid, 0, ZC_MAX_SSID_LEN);
    memset(tmp_passwd, 0, ZC_MAX_PASSWD_LEN);

    memcpy(tmp_ssid, in, ssid_len);
    in += ssid_len;
    if (passwd_len)
        memcpy(tmp_passwd, in, passwd_len);

    awss_dict_crypt(SSID_DECODE_TABLE, tmp_ssid, ssid_len);

    switch (encrypt) {
        case P2P_ENCODE_TYPE_ENCRYPT: {
            // decypt passwd using aes128-cfb
            uint8_t passwd_cipher_len = 0;
            uint8_t *passwd_cipher = os_zalloc(128);
            if (passwd_cipher == NULL)
                return GOT_NOTHING;

            decode_chinese(tmp_passwd, passwd_len, passwd_cipher, &passwd_cipher_len, 7);
            passwd_len = passwd_cipher_len;
            memset(tmp_passwd, 0, ZC_MAX_PASSWD_LEN);
            aes_decrypt_string((char *)passwd_cipher, (char *)tmp_passwd, passwd_len, os_get_encrypt_type(), 0);
            os_free(passwd_cipher);
            if (is_utf8((const char *)tmp_passwd, passwd_len) == 0) {
                memset(zconfig_data, 0, sizeof(*zconfig_data));
                awss_warn("p2p decrypt passwd content err\r\n");
                awss_event_post(AWSS_PASSWD_ERR);
                return GOT_NOTHING;
            }
            break;
        }
        default:
            awss_warn("p2p encypt:%d not support\r\n", encrypt);
            memset(zconfig_data, 0, sizeof(*zconfig_data));
            return GOT_NOTHING;
    }

    awss_debug("ssid:%s, tlen:%d\r\n", tmp_ssid, total_len);
    if (passwd_len && !memcmp(tmp_passwd, magic_p_w_d, passwd_len)) {
        //Note: when v2 rollback to v1, zc_preapre_ssid will useless
        strncpy((char *)zc_pre_ssid, (char const *)tmp_ssid, ZC_MAX_SSID_LEN - 1);
        return GOT_CHN_LOCK;
    }

    // for ascii ssid, max length is 29(32 - 1 - 2).
    // for utf-8 ssid, max length is 29 - 2 or 29 - 3
    // gbk ssid also encoded as utf-8
    // SAMSUNG S4 max name length = 22
    if (!is_ascii_string((uint8_t *)tmp_ssid)) { // chinese ssid
        ssid_truncated = 1;    // in case of gbk chinese
    } else if (total_len >= W_LEN - EXTRA_LEN) {
        ssid_truncated = 1;
    }

    if (ssid_truncated) {
        struct ap_info *ap_info;
        uint8_t *best_ssid;
        int cur_ssid_len = strlen((const char *)tmp_ssid);  // current_ssid
        int pre_ssid_len = strlen((const char *)zc_pre_ssid);  // prepare_ssid
        if (pre_ssid_len && pre_ssid_len < cur_ssid_len) {
            /* should not happen */
            awss_warn("pre:%s < cur:%s\r\n", zc_pre_ssid, tmp_ssid);
            best_ssid = tmp_ssid;  /* current ssid */
        } else if (pre_ssid_len) {
            best_ssid = zc_pre_ssid;    /* prepare ssid */
        } else {
            best_ssid = tmp_ssid;    /* default use current ssid */
        }

        //awss_debug("ssid truncated, best ssid: %s\r\n", best_ssid);

        ap_info = zconfig_get_apinfo_by_ssid_suffix(best_ssid);
        if (ap_info) {
            awss_debug("ssid truncated, got ssid from aplist:%s\r\n", best_ssid);
            strncpy((char *)zc_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
            memcpy(zc_bssid, ap_info->mac, ETH_ALEN);
        } else {
            if (memcmp(bssid, zero_mac, ETH_ALEN) && memcmp(bssid, br_mac, ETH_ALEN)) {
                memcpy(zc_android_bssid, bssid, ETH_ALEN);
            }
            ap_info = zconfig_get_apinfo(zc_android_bssid);
            if (ap_info) {
                if (ap_info->ssid[0] == '\0') {  // hide ssid, MUST not truncate
                    strncpy((char *)zc_android_ssid, (const char *)best_ssid, ZC_MAX_SSID_LEN - 1);
                } else {  // not hide ssid, amend ssid according to ap list
                    strncpy((char *)zc_android_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
                }
            } else if (time_elapsed_ms_since(start_time) > os_awss_get_channelscan_interval_ms() * (13 + 3) * 2) {
                start_time = 0;
                strncpy((char *)zc_android_ssid, (const char *)best_ssid, ZC_MAX_SSID_LEN - 1);
            }

            if (zc_android_ssid[0] == '\0') {
                return GOT_NOTHING;
            }

            strncpy((char *)zc_ssid, (const char *)zc_android_ssid, ZC_MAX_SSID_LEN - 1);
            memcpy(zc_bssid, zc_android_bssid, ETH_ALEN);
        }
    } else {
        strncpy((char *)zc_ssid, (char const *)tmp_ssid, ZC_MAX_SSID_LEN - 1);
        if (memcmp(bssid, zero_mac, ETH_ALEN) && memcmp(bssid, br_mac, ETH_ALEN)) {
            memcpy(zc_bssid, bssid, ETH_ALEN);
        }
    }

    strncpy((char *)zc_passwd, (char const *)tmp_passwd, ZC_MAX_PASSWD_LEN - 1);
    start_time = 0;

    return GOT_SSID_PASSWD;
}

static inline int package_cmp(uint8_t *package, uint8_t *src,
                              uint8_t *dst, uint8_t tods, uint16_t len)
{
    struct package *pkg = (struct package *)package;

    if (pkg->len != len)
        return 1;
    return 0;
}

static inline void package_save(uint8_t *package, uint8_t *src,
                                uint8_t *dst, uint8_t tods, uint16_t len)
{
    struct package *pkg = (struct package *)package;

    pkg->len = len;
}

/* len -= (rth->it_len + hdrlen); see ieee80211.c */
const uint8_t zconfig_fixed_offset[ZC_ENC_TYPE_MAX + 1][2] = {
    {
        //open, none, ip(20) + udp(8) + 8(LLC)
        36, 36
    },
    {
        //wep       , + iv(4) + data + ICV(4)
        44, 44  // feixun, wep64(10byte), wep128(26byte)
    },
    {
        //tkip      ,  + iv/keyID(4)    + Ext IV(4) + data + MIC(8) + ICV(4)
        56, 56  // tkip(10byte, 20byte), wpa2+tkip(20byte)
    },
    {
        //aes       , + ccmp header(8) + data + MIC(8) + ICV(4)
        52, 52 //
    },
    {
        //tkip-aes
        56, 52,    //fromDs==tkip,toDs==aes
    }
};

const uint16_t zconfig_hint_frame[] = {/* GROUP_FRAME is not used, gourp 0 - 7 */
    START_FRAME, GROUP_FRAME + 1, GROUP_FRAME + 2, GROUP_FRAME + 3, GROUP_FRAME + 4,
    GROUP_FRAME + 5, GROUP_FRAME + 6, GROUP_FRAME + 7,
    0    /* NULL terminated */
};

/*
 * is_hint_frame()
 *
 * start frame or group frame can be used as a hint frame
 *
 * @Return:
 *     1/is start or group frame, otherwise return 0.
 */
static inline int is_hint_frame(uint8_t encry, int len, uint8_t *bssid, uint8_t *src,
                                uint8_t channel, uint8_t tods, uint16_t sn)
{
    int i;
    struct ap_info *ap_info;

    if (encry > ZC_ENC_TYPE_MAX)
        return 0;

    len -= zconfig_fixed_offset[encry][0];    /* dont't care about tkip-aes */

    for (i = 0; zconfig_hint_frame[i]; i++) {
        if (zconfig_hint_frame[i] == len) {
            goto found_match;
        }
    }

    return 0;

found_match:
    /* tods/fromds already locked? */
    if (!memcmp(zc_bssid, zero_mac, ETH_ALEN)) {
        //zero mac means not locked
        memcpy(zc_bssid, bssid, ETH_ALEN);
        memcpy(zc_src_mac, src, ETH_ALEN);
    } else {
        /*
         * 1) src not equal, bssid equal, interference
         * 2) src not equal, bssid not equal, interference
         * 3) src equal, bssid equal, good, go on
         * 4) src equal, bssid not equal
         *     if tods is true, replace old ssid in case of WDS
         *     if fromds is true, APP change the AP?? or WDS??
         *         in this situation, zc_bssid is set by tods,
         *         in WDS case, zc_bssid should be unchanged
         */

        if (memcmp(zc_src_mac, src, ETH_ALEN)) {//case 1,2
            //someone must be working in aws at the same time
            awss_warn("%c interference src:"MAC_FORMAT", bssid:"MAC_FORMAT"\r\n",
                    flag_tods(tods), MAC_VALUE(src), MAC_VALUE(bssid));
            return 0;
        } else {
            if (memcmp(zc_bssid, bssid, ETH_ALEN)) {//case 4
                if (tods) {
                    memcpy(zc_bssid, bssid, ETH_ALEN);
                    memcpy(zc_src_mac, src, ETH_ALEN);
                    //TODO: clear previous buffer, channel lock state?
                    if (zconfig_data->data[0].state_machine == STATE_CHN_LOCKED_BY_BR) { //zc_state
                        zconfig_data->data[0].state_machine = STATE_CHN_SCANNING;
                    }
                    awss_warn("%c WDS! bssid:"MAC_FORMAT" -> bssid:"MAC_FORMAT"\r\n",
                         flag_tods(tods), MAC_VALUE(zc_bssid),
                         MAC_VALUE(bssid));
                } else {
                    awss_trace("%c WDS? src:"MAC_FORMAT" -> bssid:"MAC_FORMAT"\r\n",
                        flag_tods(tods), MAC_VALUE(src),
                        MAC_VALUE(bssid));
                    return 0;
                }
            } //else case 3
        }
    }

    zc_frame_offset = zconfig_fixed_offset[encry][0];/* delta, len(80211) - len(8023) */
    zc_group_pos = i * GROUP_NUMBER;
    zc_cur_pos = zc_group_pos;
    zc_group_sn = sn;
    zc_prev_sn = sn;
    zc_score_uplimit = score_max;

    //fix channel with apinfo if exist, otherwise return anyway.
    ap_info = zconfig_get_apinfo(bssid);
    if (ap_info && ap_info->encry[tods] > ZC_ENC_TYPE_MAX)
    awss_warn("invalid apinfo ssid:%s\r\n", ap_info->ssid);

    if (ap_info && ap_info->encry[tods] == encry && ap_info->channel) {
        if (channel != ap_info->channel) {
            awss_debug("fix channel from %d to %d\r\n", channel, ap_info->channel);
            zc_channel = ap_info->channel;  // fix by ap_info channel
            extern void aws_set_dst_chan(int channel);
            aws_set_dst_chan(zc_channel);
        }
    } else {
        /* warning: channel may eq 0! */
    };

    if (ap_info) { /* save ssid */
        strncpy((char *)zc_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
    } else { /* in case of zc_ssid is dirty by v2 */
        memset(zc_ssid, 0, ZC_MAX_SSID_LEN);
    }

    return 1;
}

/*
 * get_data_score()
 *
 * calc package score
 *
 * @Return:
 *     score between [0, 100]
 */
static inline uint8_t get_data_score(uint16_t group_sn, uint16_t sn_now, uint16_t sn_last,
                                     uint8_t index_now, uint8_t index_last, uint8_t tods)
{
    /*
    example: 1
     8+3 250 0  d0e cc:fa:00:c8:cf:d2 > ff:ff:ff:ff:ff:ff
     8+4 2bf 0  d15 cc:fa:00:c8:cf:d2 > ff:ff:ff:ff:ff:ff    //两个包index_delta=1, sn_delta=7

    example: 2
     8+0, 3e1,  9a5
     8+1, 13f,  9a7
             group_sn=9a7, sn=9ab-9a7, pos=e-9, len=3ce        //here, index_delta=5, sn_delta=4
             group_sn=9a7, sn=9ac-9ab, pos=f-9, len=454
             group_sn=9a7, sn=9ad-9ac, pos=10-9, len=4d2
    example: 3
     8+3, 225,  a32
     8+6, 3c7,  a39        //此处应该是16+6, index_delta=3, sn_delta=7
    example: 4
     0+0, 4e0,  da5
     0+7, 441,  dab        //此处应该是8+7， index_delta=7, sn_delta=6
     0+0, 4e0,  d89
     0+8, 4c2,  d8f        //此处应该是8+8， index_delta=8, sn_delta=6

    //example: 4
     0+0 [100] 294 0 4e0
     0+1 [60] 2a2 0 11a
     0+2 [40] 2aa 0 181
             group_sn:2aa, sn:2b8-2aa=14, pos:3-2, len:20a
             group_sn:2aa, sn:2bc-2b8=18, pos:4-2, len:28a
             group_sn:2aa, sn:2c0-2bc=22, pos:5-2, len:310
             group_sn:2aa, sn:2c4-2c0=26, pos:6-2, len:391
             group_sn:2aa, sn:2c8-2c4=30, pos:7-2, len:412
             group_sn:2aa, sn:2cc-2c8=34, pos:8-2, len:493
    */
    static const uint16_t score_level[][2] = {
        {0,      0},
        {1,      2}, //include, example 1, 3
        {4,      8},
        {8,      16},//example 1
        {15,     30},
        {40,     40},
        {0xFFFF, score_max}    //the end missing seq, example 2
    };

    uint16_t sn_delta = sn_minus(sn_now, group_sn) - 1;
    uint16_t index_delta = (index_now - index_last) - 1;
    uint16_t delta = sn_delta + index_delta;

    uint8_t i = 0;
    uint8_t res;

    /* suppose: sn > zc_prev_sn, pos > zc_cur_pos */
    if (sn_compare(sn_now, group_sn) <= 0 || sn_compare(sn_now, zc_prev_sn) <= 0) {
        return score_min;
    } else if (index_now <= index_last) {
        return score_min;
    }

    while (delta > score_level[i][0]) { /* include */
        i++;
    }

    res = score_level[i][1];

    if (zc_score_uplimit > res) {
        return zc_score_uplimit - res;
    } else {
        return score_low;
    }
}

/*
    遍历所有分组，找到最多匹配的分组: 分组号，匹配的起止位置，匹配的最小score
    遍历分两步：遍历相等的数量 和 遍历空位置的数量
    guess_pos: 根据前后位置确定的pos
    match_pos: 根据匹配计算的pos

    1) 如果guess_pos && match_pos存在，且相等，则score += 5, pos = match_pos
    2)                                    不相等，则score -= 5, pos = match_pos
    3) 只有guess_pos存在，则score = 2
    4) 只有match_pos存在，则score不变
*/
static inline int try_to_sync_pos(uint8_t tods, uint16_t last_sn, uint8_t sn,
                                  int last_group_pos, int group_pos)
{
    int ret = -1, empty_match = 0, reason = 0;
    int guess_pos = -1, final_pos = -1;

    int max_match = 0, match_group = -1, match_end = GROUP_NUMBER, match_score = 0;
    int match, i, j, score;    //loop variable

retry:
    for (i = 0; i <= zconfig_get_data_len(); i += GROUP_NUMBER) {
        for (match = 0, score = score_max, j = 1; j <= GROUP_NUMBER; j ++) {
            if (!tmp_score(j)) {
                continue;
            }

            if (empty_match) {
                if (pkg_score(i + j) <= 1) {
                    match++;
                    score = 1;
                }
            } else {
                if (!pkg_len(i + j)) {
                    continue;
                }
                if (pkg_len(i + j) == tmp_len(j)) {
                    match++;
                    score = (score > pkg_score(i + j)) ? pkg_score(i + j) : score;
                } else {/* encounter first unmatch */
                    awss_trace("[%d]=%x, [%d]=%x\r\n", i + j, pkg_len(i + j), j, tmp_len(j));
                    break;
                }
            }
        }
        if (match > max_match) {
            max_match = match;
            match_group = i;
            match_end = j - 1;
            match_score = score;
            awss_trace("match=%d, match_group=%d, match_end=%d\r\n",
                match, match_group, match_end);
        }
    }

    if (!max_match && !empty_match) {//retry empty place match
        empty_match = 1;
        goto retry;
    }

    if (group_pos != -1) {//根据后位置确定
        guess_pos = group_pos - GROUP_NUMBER;//前一组
        if (guess_pos < 0) {
            guess_pos = (zconfig_get_data_len() / GROUP_NUMBER) * GROUP_NUMBER;
        }

        if (!max_match || empty_match) {//case 3
            match_score = 2;
            final_pos = guess_pos;
            reason = 3;
            goto replace;
            //can not del goto, cause guess_pos has higher priority than empty match
        }
    }
    //前位置 有效性难以判断，忽略

    if (max_match > 0) {
        if (max_match == 1) {
            match_score = match_score > 10 ? 10 : match_score;
        } else if (max_match == 2) {
            match_score = match_score > 20 ? 20 : match_score;
        } else if (max_match <= GROUP_NUMBER) {
            match_score = match_score > 30 ? 30 : match_score;
        } else {
            goto clear;
        }

        if (guess_pos != -1) {
            if (guess_pos == match_group) {//case 1
                match_end = GROUP_NUMBER;
                match_score += 2;//bonus
                final_pos = match_group;
                reason = 1;
            } else {//case 2
                match_score -= 0;//bonus
                if (max_match >= 2 && !empty_match) {
                    final_pos = match_group;
                } else {
                    final_pos = guess_pos;
                }
                reason = 2;
            }
        } else {//case 4: 只有match_pos存在
            final_pos = match_group;

            reason = 4;
        }
    } else {
        goto clear;
    }

replace:
    if (final_pos != -1) {
        reason = reason;
        awss_trace("\tX = %d, score=%d, match=%d, reason=%d\r\n", final_pos, match_score, max_match, reason);
        if (match_end != GROUP_NUMBER) {
            awss_trace("\t match from [1-%d]\r\n", match_end);
        }
        for (i = final_pos + 1, j = 1; i <= final_pos + match_end; i++, j++) {
            if (j > GROUP_NUMBER || i >= MAX_PKG_NUMS) {
                break;
            }
            if (pkg_score(i) < match_score && tmp_score(j)) {
                pkg_len(i) = tmp_len(j);
                pkg_score(i) = (match_score > tmp_score(j) - 1) ?
                               (match_score - (tmp_score(j) - 1)) : match_score;//TODO
                awss_trace("\t%d+%d [%d] %c %-3x\r\n", final_pos, j, pkg_score(i), flag_tods(tods), tmp_len(j));

                zc_replace = 1;
                if (zc_max_pos < i) {
                    zc_max_pos = i;
                }

                ret = 0;
            }
        }
    }

clear:
    zc_pos_unsync = 0;
    memset((uint8_t *)tmp(0), 0, sizeof(zconfig_data->tmp_pkg[0]));
    return ret;
}

/*
    判断同一个位置是否应发生替换
    调用该函数的前提: 同一个位置pos, 相同的得分score, 不同的数据
    替换条件: 在各分组相同位置下，旧的数据有重复，新的数据无重复
*/
static inline int try_to_replace_same_pos(int tods, int pos, int new_len)
{
    int replace = 0, i, old_match = 0, new_match = 0;

    for (i = pos % GROUP_NUMBER; i <= zconfig_get_data_len();
         i += GROUP_NUMBER) {
        if (i != pos && pkg_len(i) == pkg_len(pos)) {
            old_match = 1;
        }

        if (pkg_len(i) == new_len) {
            new_match = 1;
        }
    }

    if ((old_match && !new_match) || tods == 0) {
        replace = 1;
        pkg_len(pos) = new_len;
    }

    return replace;
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

static inline void zconfig_set_state(uint8_t state, uint8_t tods, uint8_t channel)
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

int zconfig_recv_callback_w(struct parser_res *res)
{
    uint8_t *data = res->u.wps.data;
    uint16_t len = res->u.wps.data_len;

    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    int ret = get_ssid_passwd_from_w(data, len, res->src, res->bssid);
    if (ret == GOT_CHN_LOCK) {
        awss_debug("callback for v2:%02x%02x%02x\r\n",
                   res->src[0], res->src[1], res->src[2]);
        goto chn_locked;
    } else if (ret == GOT_SSID_PASSWD) {
        goto rcv_done;
    } else if (ret == GOT_NOTHING) {
        return PKG_INVALID;
    } else {
        return PKG_INVALID;
    }

chn_locked:
    zconfig_set_state(STATE_CHN_LOCKED_BY_P2P, tods, channel);
    return PKG_START_FRAME;
rcv_done:
    zconfig_set_state(STATE_RCV_DONE, tods, channel);
    return PKG_END;
}

int zconfig_recv_callback_broadcast(struct parser_res *res)
{
    uint32_t timestamp = zconfig_get_time();

    uint8_t *src = res->src;
    uint8_t *dst = res->dst;
    uint8_t *bssid = res->bssid;
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    uint16_t sn = res->u.br.sn;
    uint16_t len = res->u.br.data_len;
    uint8_t encry_type = res->u.br.encry_type;

    int ret, pkg_type = PKG_INVALID;
    uint8_t score = 0, timeout = 0, equal = 0;

    uint16_t pos = 0, index = 0;
#if 0
    awss_debug("len=%d, %c, sn=%d, enc=%d, chn=%d, src=%02x%02x%02x%02x%02x%02x\r\n",
               len, flag_tods(tods), sn, encry_type, channel,
               src[0], src[1], src[2], src[3], src[4], src[5]);
#endif
    /*
     * STATE_CHN_LOCKED_BY_P2P is set by v2 wps/action frame, which means
     * APP is sending v2, but if v2 is fail, APP will rollback to v1,
     * so still need to parse br frame here
     * even zc_state == STATE_CHN_LOCKED_BY_P2P.
     */
    if (zc_state == STATE_CHN_LOCKED_BY_P2P ||
        zc_state == STATE_CHN_SCANNING) {
        if (is_hint_frame(encry_type, len, bssid, src, channel, tods, sn)) {
            awss_trace("hint frame: offset:%d, %c, sn:%x\r\n",
                      zc_frame_offset, flag_tods(tods), sn);

            awss_trace("src:%02x%02x%02x%02x%02x%02x, bssid:%02x%02x%02x%02x%02x%02x\r\n",
                      src[0], src[1], src[2], src[3], src[4], src[5],
                      bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

            pkg_type = PKG_START_FRAME;
            zconfig_set_state(STATE_CHN_LOCKED_BY_BR, tods, channel);

            goto update_sn;
        } else if (!memcmp(zc_android_src, src, ETH_ALEN)) {
            struct ap_info *ap_info = zconfig_get_apinfo(bssid);
            if (ap_info) {
                if (ap_info->ssid[0] != 0x00 && ap_info->ssid[0] != 0xFF) {
                    strncpy((char *)zc_android_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
                }
                memcpy(zc_android_bssid, bssid, ETH_ALEN);
                awss_trace("src %02x%02x%02x match %02x%02x%02x\r\n",
                          zc_android_src[0], zc_android_src[1], zc_android_src[2],
                          zc_android_bssid[0], zc_android_bssid[1], zc_android_bssid[2]);
            }
        }
    } else if (zc_state == STATE_CHN_LOCKED_BY_BR) {
        /* same src mac & br & bssid */
        if (memcmp(&src[0], zc_src_mac, ETH_ALEN) ||
            memcmp(&dst[0], br_mac, sizeof(br_mac)) ||
            memcmp(bssid, zc_bssid, ETH_ALEN)) { //in case of WDS
            goto drop;
        }

        if (timestamp - zc_timestamp > time_interval) {
            awss_debug("\t\t\t\t\ttimestamp = %d\r\n", timestamp - zc_timestamp);
            timeout = 1;
        }

        ret = sn_compare(sn, zc_prev_sn);
        if (ret <= 0) { // retry packet, update timestamp
            zc_timestamp = timestamp;
        }
        if (ret == 0) {
            awss_debug("drop: %3x == %3x\r\n", sn, zc_prev_sn);//log level, too many retry pkg
            goto drop;
        } else if (ret < 0 && !timeout) {//if timeout, goto pos_unsync
            awss_debug("drop: %3x < %3x\r\n", sn, zc_prev_sn);//TODO: better not drop
            goto update_sn;//FIXME: update sn???
        }

        //assert(sn > zc_prev_sn && !timeout);

        if (len <= zc_frame_offset) { /* length invalid */
            goto drop;
        }

        len -= zc_frame_offset;

        if (is_data_frame(len)) {
            pkg_type = PKG_DATA_FRAME;
            index = get_data_index(len);
            pos = zc_group_pos + index;

            if (index > GROUP_NUMBER || pos >= MAX_PKG_NUMS) {
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
            if (index < zc_last_index ||
                (index == zc_last_index && len != zc_last_len) || timeout) {
                if (zc_pos_unsync) {/* already in pos_unsync state */
                    awss_trace("\texit try_to_sync_pos: re-enter!\r\n");
                    try_to_sync_pos(tods, zc_prev_sn, sn, zc_group_pos, -1);
                }
                zc_pos_unsync = 1;/* also a new start */
                if (index < zc_last_index) {
                    awss_trace("\tenter try_to_sync_pos: rollback \r\n");
                } else if (timeout) {
                    awss_trace("\tenter try_to_sync_pos: timeout \r\n");
                } else {
                    awss_trace("\tenter try_to_sync_pos: != \r\n");
                }
            }
pos_unsync:
            if (zc_pos_unsync) {/* tmp save */
                package_save((uint8_t *)tmp(index), src, dst, tods, len);
                if (zc_pos_unsync == 1) {
                    tmp_score(index) = 1;
                } else {
                    tmp_score(index) = (sn - zc_prev_sn);    //TODO: index? last_tmp_score
                }
                zc_pos_unsync ++; /* unsync pkg counter */
                awss_trace("\tX+%d [%d] %-3x %c %-3x\r\n", index, tmp_score(index), sn, flag_tods(tods), len);
                goto update_sn;//FIXME: update prev_sn or not?
            }

            // assert(sn > zc_prev_sn && pos > zc_cur_pos)
            score = get_data_score(zc_group_sn, sn, zc_prev_sn, pos, zc_cur_pos, tods);
            if (score == score_min) {//better not drop any pkg here
                awss_trace("\t drop: group_sn:%x, sn:%x-%x=%x, pos:%d-%d, len:%x\r\n",
                          zc_group_sn, sn, zc_prev_sn, sn_minus(sn, zc_group_sn), pos, zc_cur_pos, len);
                goto update_sn;
            } else {
                if (zc_score_uplimit > score) {
                    zc_score_uplimit = score;    /* inherit last limit */
                }

                zc_group_sn = sn;//TODO
                awss_trace("%d+%d [%d] %-3x %c %-3x\r\n", zc_group_pos, index, score, sn, flag_tods(tods), len);
            }
        } else {
            if (is_start_frame(len) || is_group_frame(len)) {
                uint8_t group = get_group_index(len);

                if (zc_pos_unsync) {
                    awss_trace("\texit try_to_sync_pos: group frame\r\n");
                    try_to_sync_pos(tods, zc_prev_sn, sn, zc_group_pos, group);
                }

                zc_cur_pos = group;
                zc_group_pos = group;
                zc_group_sn = sn;
                zc_score_uplimit = score_max;

                awss_trace("%d+%d [%d] %-3x %c %-3x\r\n", group, 0, zc_score_uplimit, sn, flag_tods(tods), len);

                //ignore PKG_GROUP_FRAME here
                pkg_type = PKG_START_FRAME;

                /*
                 * keep calling zconfig_set_state(), see Note about
                 * zconfig_callback_channel_locked()
                 */
                zconfig_set_state(STATE_CHN_LOCKED_BY_BR, tods, channel);

                //zc_replace may happen in try_to_sync_pos(), so goto is_recv_completed
                goto is_recv_completed;
            } else {
                awss_trace("\t invalid len = %d\r\n", len + zc_frame_offset);
                goto drop;
            }
        }

        /* start from pkg(1), leave pkg(0) for start frame */
        if (pos >= MAX_PKG_NUMS || pos <= 0) {
            awss_debug("msg index(%d) out of range!\r\n", pos);
            goto drop;
        }

        zc_cur_pos = pos;

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
        for (tods = 0; tods < 2; tods ++) {
            equal = !package_cmp((uint8_t *)pkg(pos), src, dst, tods, len);

            if (score > pkg_score(pos)) {
                pkg_score(pos) = score;    //update score first
                if (equal) {
                    continue;
                }
                // not equal
                zc_replace = 1;
                package_save((uint8_t *)pkg(pos), src, dst, tods, len);
            } else if (score == pkg_score(pos)) {/* range check ? */
                if (equal) {
                    pkg_score(pos) ++;
                    continue;
                }
                // not equal
                int replace = try_to_replace_same_pos(tods, pos, len);
                if (replace) {
                    awss_trace("\t replace @ %d, len=%x\r\n", pos, len);
                    continue;
                }
                pkg_score(pos) /= 2;
                if (score >= score_mid)  // better not happen
                    awss_warn("xxxxxxxx warn: pos=%d, score=[%d], %x != %x\r\n",
                         pos, score, pkg_len(pos), len);

            } else if (tods == res->tods) {//pkg_score(pos) > score
                if (!equal) {/* data not equal */
                    if (zc_pos_unsync) {
                        continue;
                    }
                    zc_pos_unsync = 1;
                    awss_trace("\tenter try_to_sync_pos: data mismatch\r\n");
                    tods = res->tods;
                    goto pos_unsync;
                } else if (zc_score_uplimit >= score_mid && pkg_score(pos) - score < 10) { /* data equal */
                    uint8_t uplimit = (zc_score_uplimit + pkg_score(pos)) / 2;
                    if (zc_score_uplimit != uplimit) {
                        awss_trace("\t\t\t uplimit [%d] -> [%d]\r\n", zc_score_uplimit, uplimit);
                    }
                    zc_score_uplimit = uplimit;
                }
            }
        }
        tods = res->tods;

is_recv_completed:
        zc_max_pos = (zc_max_pos < zc_cur_pos) ? zc_cur_pos : zc_max_pos;
        if (zc_replace && zconfig_recv_completed(tods)) {
            zc_replace = 0;
            memcpy(zc_bssid, res->bssid, ETH_ALEN);
            if (!zconfig_get_ssid_passwd(tods)) {
                /* we got it! */
                zconfig_set_state(STATE_RCV_DONE, tods, channel);
                return PKG_END;
            }
        }
    }

update_sn:
    zc_prev_sn = sn;

    zc_timestamp = timestamp;
    zc_last_index = index;
    zc_last_len = len;

    return pkg_type;

drop:
    return PKG_INVALID;
}

const char *zc_default_ssid = "aha";
const char *zc_default_passwd = "12345678";
const char *zc_adha_ssid = "adha";
const char *zc_adha_passwd = "08d9f22c60157fd01f57645d791a0b610fe0a558c104d6a1f9d9c0a9913c";

int zconfig_recv_callback_adha_ssid(struct parser_res *res)
{
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    awss_debug("found adha ssid: %s\r\n", zc_adha_ssid);

    strncpy((char *)zc_ssid, zc_adha_ssid, ZC_MAX_SSID_LEN - 1);
    strncpy((char *)zc_passwd, zc_adha_passwd, ZC_MAX_PASSWD_LEN - 1);

    zconfig_set_state(STATE_RCV_DONE, tods, channel);
    return PKG_END;
}

extern struct adha_info *adha_aplist;
int zconfig_recv_callback_default_ssid(struct parser_res *res)
{
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    awss_debug("found default ssid: %s\r\n", zc_default_ssid);

    strncpy((char *)zc_ssid, zc_default_ssid, ZC_MAX_SSID_LEN - 1);
    strncpy((char *)zc_passwd, zc_default_passwd, ZC_MAX_PASSWD_LEN - 1);

    zconfig_set_state(STATE_RCV_DONE, tods, channel);
    return PKG_END;
}

/* return 0 for success, -1 devid not match, otherwise return -2 */
int (*vendor_decrypt_ssid_passwd)(uint8_t *ie, uint8_t ie_len,
                                  uint8_t out_ssid[ZC_MAX_SSID_LEN],
                                  uint8_t out_passwd[ZC_MAX_PASSWD_LEN],
                                  uint8_t out_bssid[6]);

int zconfig_recv_callback_zero_config(struct parser_res *res)
{
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    uint8_t *ie = res->u.ie.alink_ie;
    uint8_t ie_len = ie[1];
    int ret;

    if (!vendor_decrypt_ssid_passwd)
        return PKG_INVALID;

    if (res->u.ie.alink_ie_len < ie_len)
        return PKG_INVALID;

    ret = vendor_decrypt_ssid_passwd(ie, ie_len, zc_ssid, zc_passwd, zc_bssid);
    if (ret)
        return PKG_INVALID;

    zconfig_set_state(STATE_RCV_DONE, tods, channel);

    return PKG_END;
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
    int data_type = ALINK_INVALID;
    struct parser_res res;

    /* remove FCS filed */
    if (with_fcs) {
        pkt_length -= 4;
    }

    /* useless, will be removed */
    if (is_invalid_pkg(pkt_data, pkt_length)) {
        return PKG_INVALID;
    }

#ifdef AWSS_SUPPORT_HT40
    if (link_type == AWSS_LINK_TYPE_HT40_CTRL) {
        if (!g_user_press) {
            return PKG_INVALID;
        }
        struct ht40_ctrl *ctrl = pkt_data;
        return zconfig_recv_callback_ht40_ctrl(ctrl->filter, ctrl->rssi, pkt_length, channel);
    }
#endif

    memset(&res, 0, sizeof(res));
    res.channel = channel;

    data_type = ieee80211_data_extract(pkt_data, pkt_length, link_type, &res, rssi);

    switch (data_type) {
        case ALINK_WPS:
            return zconfig_recv_callback_w(&res);             // android p2p
        case ALINK_BROADCAST:
            return zconfig_recv_callback_broadcast(&res);     // smartconfig-bcast
        case ALINK_ZERO_CONFIG:
            return zconfig_recv_callback_zero_config(&res);   // zero-config with probe-req
        case ALINK_ADHA_SSID:
            return zconfig_recv_callback_adha_ssid(&res);     // adha
        case ALINK_DEFAULT_SSID:
            return zconfig_recv_callback_default_ssid(&res);  // aha
        default:
            return PKG_INVALID;
    }
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

    do {
        if (zconfig_aplist)
            break;
        zconfig_aplist = (struct ap_info *)os_zalloc(sizeof(struct ap_info) * MAX_APLIST_NUM);
        if (zconfig_aplist == NULL)
            goto ZCONFIG_INIT_FAIL;
        zconfig_aplist_num = 0;
    } while (0);

    do {
        if (adha_aplist)
            break;
        adha_aplist = (struct adha_info *)os_zalloc(sizeof(struct adha_info));
        if (adha_aplist == NULL)
            goto ZCONFIG_INIT_FAIL;
    } while (0);

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
    if (zconfig_aplist) {
        os_free(zconfig_aplist);
        zconfig_aplist = NULL;
    }
    if (adha_aplist) {
        os_free(adha_aplist);
        adha_aplist = NULL;
    }
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

    if (zconfig_aplist) {
        os_free((void *)zconfig_aplist);
        zconfig_aplist = NULL;
        zconfig_aplist_num = 0;
    }

    if (adha_aplist) {
        os_free((void *)adha_aplist);
        adha_aplist = NULL;
    }

    awss_stop_timer(clr_aplist_timer);
    clr_aplist_timer = NULL;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
