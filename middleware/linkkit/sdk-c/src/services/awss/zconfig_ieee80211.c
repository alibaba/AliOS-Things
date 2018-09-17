/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "os.h"
#include <stddef.h>
#include <string.h>
#include "awss.h"
#include "awss_main.h"
#include "aws_lib.h"
#include "zconfig_utils.h"
#include "zconfig_ieee80211.h"
#include "zconfig_protocol.h"
#include "awss_timer.h"

/*
 * DS bit usage
 *
 * TA = transmitter address
 * RA = receiver address
 * DA = destination address
 * SA = source address
 *
 * ToDS    FromDS  A1(RA)  A2(TA)  A3      A4      Use
 * -----------------------------------------------------------------
 *  0       0       DA      SA      BSSID   -       IBSS/DLS
 *  0       1       DA      BSSID   SA      -       AP -> STA
 *  1       0       BSSID   SA      DA      -       AP <- STA
 *  1       1       RA      TA      DA      SA      unspecified (WDS)
 */

#define FCS_LEN                         (4)

#define IEEE80211_FCTL_VERS             (0x0003)
#define IEEE80211_FCTL_FTYPE            (0x000c)
#define IEEE80211_FCTL_STYPE            (0x00f0)
#define IEEE80211_FCTL_TODS             (0x0100)
#define IEEE80211_FCTL_FROMDS           (0x0200)
#define IEEE80211_FCTL_MOREFRAGS        (0x0400)
#define IEEE80211_FCTL_RETRY            (0x0800)
#define IEEE80211_FCTL_PM               (0x1000)
#define IEEE80211_FCTL_MOREDATA         (0x2000)
#define IEEE80211_FCTL_PROTECTED        (0x4000)
#define IEEE80211_FCTL_ORDER            (0x8000)
#define IEEE80211_FCTL_CTL_EXT          (0x0f00)

#define IEEE80211_SCTL_FRAG             (0x000F)
#define IEEE80211_SCTL_SEQ              (0xFFF0)

#define IEEE80211_FTYPE_MGMT            (0x0000)
#define IEEE80211_FTYPE_CTL             (0x0004)
#define IEEE80211_FTYPE_DATA            (0x0008)
#define IEEE80211_FTYPE_EXT             (0x000c)

#define IEEE80211_STYPE_DATA            (0x0000)
#define IEEE80211_STYPE_QOS_DATA        (0x0080)
#define IEEE80211_STYPE_PROBE_REQ       (0x0040)
#define IEEE80211_STYPE_PROBE_RESP      (0x0050)
#define IEEE80211_STYPE_BEACON          (0x0080)
#define IEEE80211_STYPE_ACTION          (0x00D0)

#define IEEE80211_QOS_CTL_LEN           (2)
#define IEEE80211_HT_CTL_LEN            (4)

#define IEEE80211_SCTL_SEQ              (0xFFF0)

/* beacon capab_info */
#define WLAN_CAPABILITY_PRIVACY         (1 << 4)

#define IEEE80211_SEQ_TO_SN(seq)        (((seq) & IEEE80211_SCTL_SEQ) >> 4)
#define IEEE80211_SN_TO_SEQ(ssn)        (((ssn) << 4) & IEEE80211_SCTL_SEQ)

#define WLAN_CATEGORY_VENDOR_SPECIFIC   (127)

#define WLAN_EID_SSID                   (0)
#define WLAN_EID_DS_PARAMS              (3)
#define WLAN_EID_RSN                    (48)
#define WLAN_EID_HT_OPERATION           (61)
#define WLAN_EID_VENDOR_SPECIFIC        (221)

#define WLAN_OUI_ALIBABA                (0xD896E0)
#define WLAN_OUI_TYPE_ALIBABA           (1)
#define WLAN_OUI_TYPE_ENROLLEE          (0xAA)
#define WLAN_OUI_TYPE_REGISTRAR         (0xAB)

#define WLAN_OUI_MICROSOFT              (0x0050F2)
#define WLAN_OUI_WPS                    (0x0050F2)
#define WLAN_OUI_TYPE_MICROSOFT_WPA     (1)
#define WLAN_OUI_TYPE_WPS               (4)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

uint8_t g_user_press = 0;

struct ieee80211_hdr {
    __le16 frame_control;
    __le16 duration_id;
    u8 addr1[ETH_ALEN];
    u8 addr2[ETH_ALEN];
    u8 addr3[ETH_ALEN];
    __le16 seq_ctrl;
    u8 addr4[ETH_ALEN];
};

/*
 * The radio capture header precedes the 802.11 header.
 *
 * Note well: all radiotap fields are little-endian.
 */
struct ieee80211_radiotap_header {
    uint8_t  it_version;     /* Version 0. Only increases
                     * for drastic changes,
                     * introduction of compatible
                     * new fields does not count.
                     */
    uint8_t  it_pad;
    uint16_t it_len;         /* length of the whole
                    * header in bytes, including
                    * it_version, it_pad,
                    * it_len, and data fields.
                    */
    uint32_t it_present;     /* A bitmap telling which
                    * fields are present. Set bit 31
                    * (0x80000000) to extend the
                    * bitmap by another 32 bits.
                    * Additional extensions are made
                    * by setting bit 31.
                    */
};

/**
 * struct ieee80211_ht_operation - HT operation IE
 *
 * This structure is the "HT operation element" as
 * described in 802.11n-2009 7.3.2.57
 */
struct ieee80211_ht_operation {
    u8 primary_chan;
    u8 ht_param;
    __le16 operation_mode;
    __le16 stbc_param;
    u8 basic_set[16];
};

#define cpu_to_le16        os_htole16
#define __le16_to_cpu    os_le16toh

/**
 * ieee80211_is_mgmt - check if type is IEEE80211_FTYPE_MGMT
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_mgmt(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
           cpu_to_le16(IEEE80211_FTYPE_MGMT);
}

/**
 * ieee80211_is_ctl - check if type is IEEE80211_FTYPE_CTL
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_ctl(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
           cpu_to_le16(IEEE80211_FTYPE_CTL);
}

/**
 * ieee80211_is_data - check if type is IEEE80211_FTYPE_DATA
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_data(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
           cpu_to_le16(IEEE80211_FTYPE_DATA);
}


/**
 * ieee80211_has_tods - check if IEEE80211_FCTL_TODS is set
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_has_tods(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_TODS)) != 0;
}

/**
 * ieee80211_has_fromds - check if IEEE80211_FCTL_FROMDS is set
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_has_fromds(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FROMDS)) != 0;
}

/**
 * ieee80211_has_a4 - check if IEEE80211_FCTL_TODS and IEEE80211_FCTL_FROMDS are set
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_has_a4(__le16 fc)
{
    __le16 tmp = cpu_to_le16(IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS);
    return (fc & tmp) == tmp;
}

/**
 * ieee80211_has_order - check if IEEE80211_FCTL_ORDER is set
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_has_order(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_ORDER)) != 0;
}

/**
 * ieee80211_has_protected - check if IEEE80211_FCTL_PROTECTED is set
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_has_protected(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_PROTECTED)) != 0;
}

/**
 * ieee80211_is_data_qos - check if type is IEEE80211_FTYPE_DATA and IEEE80211_STYPE_QOS_DATA is set
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_data_qos(__le16 fc)
{
    /*
     * mask with QOS_DATA rather than IEEE80211_FCTL_STYPE as we just need
     * to check the one bit
     */
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_STYPE_QOS_DATA)) ==
           cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_DATA);
}

/**
 * ieee80211_is_data_present - check if type is IEEE80211_FTYPE_DATA and has data
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_data_present(__le16 fc)
{
    /*
     * mask with 0x40 and test that that bit is clear to only return true
     * for the data-containing substypes.
     */
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | 0x40)) ==
           cpu_to_le16(IEEE80211_FTYPE_DATA);
}

/**
 * ieee80211_is_data_present - check if type is IEEE80211_FTYPE_DATA and only data
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_data_exact(__le16 fc)
{
    __le16 tmp = fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE);

    return (tmp == cpu_to_le16(IEEE80211_FTYPE_DATA)) ||
           (tmp == cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_DATA));
}

/**
 * ieee80211_is_beacon - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_BEACON
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_beacon(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
           cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON);
}

/**
 * ieee80211_is_action - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ACTION
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_action(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
           cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ACTION);
}

/**
 * ieee80211_is_probe_req - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_PROBE_REQ
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_probe_req(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
           cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ);
}

/**
 * ieee80211_is_probe_resp - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_PROBE_RESP
 * @fc: frame control bytes in little-endian byteorder
 */
static inline int ieee80211_is_probe_resp(__le16 fc)
{
    return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
           cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_RESP);
}


/**
 * ieee80211_get_SA - get pointer to SA
 * @hdr: the frame
 *
 * Given an 802.11 frame, this function returns the offset
 * to the source address (SA). It does not verify that the
 * header is long enough to contain the address, and the
 * header must be long enough to contain the frame control
 * field.
 */
static inline u8 *ieee80211_get_SA(struct ieee80211_hdr *hdr)
{
    if (ieee80211_has_a4(hdr->frame_control))
        return hdr->addr4;
    if (ieee80211_has_fromds(hdr->frame_control))
        return hdr->addr3;
    return hdr->addr2;
}

/**
 * ieee80211_get_DA - get pointer to DA
 * @hdr: the frame
 *
 * Given an 802.11 frame, this function returns the offset
 * to the destination address (DA). It does not verify that
 * the header is long enough to contain the address, and the
 * header must be long enough to contain the frame control
 * field.
 */
static inline u8 *ieee80211_get_DA(struct ieee80211_hdr *hdr)
{
    if (ieee80211_has_tods(hdr->frame_control))
        return hdr->addr3;
    else
        return hdr->addr1;
}

static inline u8 *ieee80211_get_BSSID(struct ieee80211_hdr *hdr)
{
    if (ieee80211_has_tods(hdr->frame_control)) {
        if (!ieee80211_has_fromds(hdr->frame_control))
            return hdr->addr1;
        else
            return NULL;
    } else {
        if (ieee80211_has_fromds(hdr->frame_control))
            return hdr->addr2;
        else
            return hdr->addr3;
    }
}

static inline int ieee80211_get_bssid(u8 *in, u8 *mac)
{
    u8 *bssid = ieee80211_get_BSSID((struct ieee80211_hdr *)in);

    if (bssid)
        memcpy(mac, bssid, ETH_ALEN);
    else
        return -1;

    return 0;
}

static inline int ieee80211_has_frags(__le16 fc)
{
    __le16 tmp = fc & cpu_to_le16(IEEE80211_FCTL_MOREFRAGS | IEEE80211_FCTL_ORDER);

    return !!tmp;
}

//DATA:        24B
//QOS-DATA:    26B
static inline unsigned int ieee80211_hdrlen(__le16 fc)
{
    unsigned int hdrlen = 24;

    if (ieee80211_is_data(fc)) {
        if (ieee80211_has_a4(fc))
            hdrlen = 30;
        if (ieee80211_is_data_qos(fc)) {
            hdrlen += IEEE80211_QOS_CTL_LEN;
            if (ieee80211_has_order(fc))
                hdrlen += IEEE80211_HT_CTL_LEN;
        }
        goto out;
    }

    if (ieee80211_is_ctl(fc)) {
        /*
         * ACK and CTS are 10 bytes, all others 16. To see how
         * to get this condition consider
         *   subtype mask:   0b0000000011110000 (0x00F0)
         *   ACK subtype:    0b0000000011010000 (0x00D0)
         *   CTS subtype:    0b0000000011000000 (0x00C0)
         *   bits that matter:         ^^^      (0x00E0)
         *   value of those: 0b0000000011000000 (0x00C0)
         */
        if ((fc & cpu_to_le16(0x00E0)) == cpu_to_le16(0x00C0))
            hdrlen = 10;
        else
            hdrlen = 16;
    }

out:
    return hdrlen;
}

/* helpers */
static inline int ieee80211_get_radiotap_len(unsigned char *data)
{
    struct ieee80211_radiotap_header *hdr =
            (struct ieee80211_radiotap_header *)data;

    return os_get_unaligned_le16((u8 *)&hdr->it_len);
}

struct ieee80211_vendor_ie {
    u8 element_id;
    u8 len;
    u8 oui[3];
    u8 oui_type;
};
/*
 * i.e.    alibaba ie
 *    @name        @len    @payload
 *    element_id    1    221
 *    len           1    22
 *    oui           3    0xD896E0
 *    oui_type      1    1 -- alink router service advertisement
 *    version       1    1
 *    challenge     16    non-zero-ascii code
 *    reserve       1    0
 */

struct ieee80211_mgmt {
    __le16 frame_control;
    __le16 duration;
    u8 da[ETH_ALEN];
    u8 sa[ETH_ALEN];
    u8 bssid[ETH_ALEN];
    __le16 seq_ctrl;
    union {
        struct {
            //__le64 timestamp;
            __le16 timestamp[4];
            __le16 beacon_int;
            __le16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params, TIM */
            u8 variable;
        } beacon;
        struct {
            /* only variable items: SSID, Supported rates */
            u8 variable;
        } probe_req;
        struct {
            //__le64 timestamp;
            __le16 timestamp[4];
            __le16 beacon_int;
            __le16 capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params */
           u8 variable;
        } probe_resp;
    } u;
};

const u8 *cfg80211_find_ie(u8 eid, const u8 *ies, int len)
{
    while (len > 2 && ies[0] != eid) {
        len -= ies[1] + 2;
            ies += ies[1] + 2;
    }
    if (len < 2)
        return NULL;
    if (len < 2 + ies[1])
        return NULL;
    return ies;
}

/**
 * cfg80211_find_vendor_ie - find vendor specific information element in data
 *
 * @oui: vendor OUI
 * @oui_type: vendor-specific OUI type
 * @ies: data consisting of IEs
 * @len: length of data
 *
 * Return: %NULL if the vendor specific element ID could not be found or if the
 * element is invalid (claims to be longer than the given data), or a pointer to
 * the first byte of the requested element, that is the byte containing the
 * element ID.
 *
 * Note: There are no checks on the element length other than having to fit into
 * the given data.
 */
const u8 *cfg80211_find_vendor_ie(
        unsigned int oui, u8 oui_type, const u8 *ies, int len)
{
    struct ieee80211_vendor_ie *ie;
    const u8 *pos = ies, *end = ies + len;
    int ie_oui;

    while (pos < end) {
        pos = cfg80211_find_ie(WLAN_EID_VENDOR_SPECIFIC, pos,
                               end - pos);
        if (!pos)
            return NULL;

        ie = (struct ieee80211_vendor_ie *)pos;

        /* make sure we can access ie->len */
        //BUILD_BUG_ON(offsetof(struct ieee80211_vendor_ie, len) != 1);

        if (ie->len < sizeof(*ie))
            goto cont;

        ie_oui = ie->oui[0] << 16 | ie->oui[1] << 8 | ie->oui[2];
        //log("oui=%x, type=%x, len=%d\r\n", ie_oui, oui_type, ie->len);
        if (ie_oui == oui && ie->oui_type == oui_type)
            return pos;
cont:
        pos += 2 + ie->len;
    }
    return NULL;
}

/**
 * extract ssid from beacon frame or probe resp frame
 *
 * @beacon_frame: [IN] original 80211 beacon frame
 * @frame_len: [IN] len of beacon frame
 * @ssid: [OUT] null-terminated string, max len 32 bytes
 *
 * Return:
 *     0/success, -1/failed
 */
static inline int ieee80211_get_ssid(u8 *beacon_frame, u16 frame_len, u8 *ssid)
{
    u16 ieoffset = offsetof(struct ieee80211_mgmt, u.beacon.variable);//same as u.probe_resp.variable
    const u8 *ptr = cfg80211_find_ie(WLAN_EID_SSID,
                beacon_frame + ieoffset, frame_len - ieoffset);
    if (ptr) {
        u8 ssid_len = ptr[1];
        if (ssid_len <= 32) {    /* ssid 32 octets at most */
            memcpy(ssid, ptr + 2, ssid_len);/* eating EID & len */
            ssid[ssid_len] = '\0';
            return 0;
        }
    }

    return -1;
}

/**
 * extract channel from beacon frame or probe resp frame
 *
 * @beacon_frame: [IN] original 80211 beacon frame
 * @frame_len: [IN] len of beacon frame
 *
 * Return:
 *     bss channel 1-13, 0--means invalid channel
 */
int cfg80211_get_bss_channel(u8 *beacon_frame, u16 frame_len)
{
    u16 ieoffset = offsetof(struct ieee80211_mgmt, u.beacon.variable);//same as u.probe_resp.variable
    const u8 *ie = beacon_frame + ieoffset;
    u16 ielen = frame_len - ieoffset;

    const u8 *tmp;
    int channel_number = 0;

    tmp = cfg80211_find_ie(WLAN_EID_DS_PARAMS, ie, ielen);
    if (tmp && tmp[1] == 1) {
        channel_number = tmp[2];
    } else {
        tmp = cfg80211_find_ie(WLAN_EID_HT_OPERATION, ie, ielen);
        if (tmp && tmp[1] >= sizeof(struct ieee80211_ht_operation)) {
            struct ieee80211_ht_operation *htop = (void *)(tmp + 2);

            channel_number = htop->primary_chan;
        }
    }

    return channel_number;
}

static const u8 WPA_OUI23A_TYPE[] = { 0x00, 0x50, 0xf2, 0x01 };
static const u8 RSN_SUITE_1X[] =    { 0x00, 0x0f, 0xac, 0x01 };

static const u8 WPA_CIPHER_SUITE_NONE23A[] =   { 0x00, 0x50, 0xf2, 0 };
static const u8 WPA_CIPHER_SUITE_WEP4023A[] =  { 0x00, 0x50, 0xf2, 1 };
static const u8 WPA_CIPHER_SUITE_TKIP23A[] =   { 0x00, 0x50, 0xf2, 2 };
//static const u8 WPA_CIPHER_SUITE_WRAP23A[] =   { 0x00, 0x50, 0xf2, 3 };
static const u8 WPA_CIPHER_SUITE_CCMP23A[] =   { 0x00, 0x50, 0xf2, 4 };
static const u8 WPA_CIPHER_SUITE_WEP10423A[] = { 0x00, 0x50, 0xf2, 5 };

static const u8 RSN_CIPHER_SUITE_NONE23A[] =   { 0x00, 0x0f, 0xac, 0 };
static const u8 RSN_CIPHER_SUITE_WEP4023A[] =  { 0x00, 0x0f, 0xac, 1 };
static const u8 RSN_CIPHER_SUITE_TKIP23A[] =   { 0x00, 0x0f, 0xac, 2 };
//static const u8 RSN_CIPHER_SUITE_WRAP23A[] =   { 0x00, 0x0f, 0xac, 3 };
static const u8 RSN_CIPHER_SUITE_CCMP23A[] =   { 0x00, 0x0f, 0xac, 4 };
static const u8 RSN_CIPHER_SUITE_WEP10423A[] = { 0x00, 0x0f, 0xac, 5 };

#define WPA_SELECTOR_LEN        (4)
#define RSN_SELECTOR_LEN        (4)

#define BIT(x)                  (1 << (x))
#define WPA_CIPHER_NONE         BIT(0)
#define WPA_CIPHER_WEP40        BIT(1)
#define WPA_CIPHER_WEP104       BIT(2)
#define WPA_CIPHER_TKIP         BIT(3)
#define WPA_CIPHER_CCMP         BIT(4)

static u8 map_cipher_to_encry(u8 cipher)
{
    switch (cipher) {
    case WPA_CIPHER_CCMP:
        return ZC_ENC_TYPE_AES;
    case WPA_CIPHER_TKIP:
        return ZC_ENC_TYPE_TKIP;
    case WPA_CIPHER_WEP40:
    case WPA_CIPHER_WEP104:
        return ZC_ENC_TYPE_WEP;
    case WPA_CIPHER_NONE:
        return ZC_ENC_TYPE_NONE;
    case (WPA_CIPHER_TKIP | WPA_CIPHER_CCMP):
        return ZC_ENC_TYPE_TKIPAES;
    default:
        warn_on(1, "unknow cipher type: %x\r\n", cipher);
        return ZC_ENC_TYPE_INVALID;
    }
}

static int get_wpa_cipher_suite(const u8 *s)
{
    if (!memcmp(s, WPA_CIPHER_SUITE_NONE23A, WPA_SELECTOR_LEN))
        return WPA_CIPHER_NONE;
    if (!memcmp(s, WPA_CIPHER_SUITE_WEP4023A, WPA_SELECTOR_LEN))
        return WPA_CIPHER_WEP40;
    if (!memcmp(s, WPA_CIPHER_SUITE_TKIP23A, WPA_SELECTOR_LEN))
        return WPA_CIPHER_TKIP;
    if (!memcmp(s, WPA_CIPHER_SUITE_CCMP23A, WPA_SELECTOR_LEN))
        return WPA_CIPHER_CCMP;
    if (!memcmp(s, WPA_CIPHER_SUITE_WEP10423A, WPA_SELECTOR_LEN))
        return WPA_CIPHER_WEP104;

    return 0;
}

static int get_wpa2_cipher_suite(const u8 *s)
{
    if (!memcmp(s, RSN_CIPHER_SUITE_NONE23A, RSN_SELECTOR_LEN))
        return WPA_CIPHER_NONE;
    if (!memcmp(s, RSN_CIPHER_SUITE_WEP4023A, RSN_SELECTOR_LEN))
        return WPA_CIPHER_WEP40;
    if (!memcmp(s, RSN_CIPHER_SUITE_TKIP23A, RSN_SELECTOR_LEN))
        return WPA_CIPHER_TKIP;
    if (!memcmp(s, RSN_CIPHER_SUITE_CCMP23A, RSN_SELECTOR_LEN))
        return WPA_CIPHER_CCMP;
    if (!memcmp(s, RSN_CIPHER_SUITE_WEP10423A, RSN_SELECTOR_LEN))
        return WPA_CIPHER_WEP104;

    return 0;
}

int cfg80211_parse_wpa_info(const u8 *wpa_ie, int wpa_ie_len,
        u8 *group_cipher, u8 *pairwise_cipher, u8 *is_8021x)
{
    int i, ret = 0;
    int left, count;
    const u8 *pos;

    if (wpa_ie_len <= 0) {
        /* No WPA IE - fail silently */
        return -1;
    }

    if (wpa_ie[1] != (u8)(wpa_ie_len - 2))
        return -1;

    pos = wpa_ie;

    pos += 8;
    left = wpa_ie_len - 8;

    /* group_cipher */
    if (left >= WPA_SELECTOR_LEN) {

        *group_cipher = get_wpa_cipher_suite(pos);

        pos += WPA_SELECTOR_LEN;
        left -= WPA_SELECTOR_LEN;
    } else if (left > 0) {
        return -1;
    }

    /* pairwise_cipher */
    if (left >= 2) {
        /* count = le16_to_cpu(*(u16*)pos); */
        count = os_get_unaligned_le16((u8 *)pos);
        pos += 2;
        left -= 2;

        if (count == 0 || left < count * WPA_SELECTOR_LEN) {
            return -1;
        }

        for (i = 0; i < count; i++) {
            *pairwise_cipher |= get_wpa_cipher_suite(pos);

            pos += WPA_SELECTOR_LEN;
            left -= WPA_SELECTOR_LEN;
        }
    } else if (left == 1) {
        return -1;
    }

    if (is_8021x) {
        if (left >= 6) {
            pos += 2;
            if (!memcmp(pos, WPA_OUI23A_TYPE, 4)) {
                *is_8021x = 1;
            }
        }
    }

    return ret;
}

int cfg80211_parse_wpa2_info(const u8* rsn_ie, int rsn_ie_len, u8 *group_cipher,
        u8 *pairwise_cipher, u8 *is_8021x)
{
    int i, ret = 0;
    int left, count;
    const u8 *pos;

    if (rsn_ie_len <= 0) {
        /* No RSN IE - fail silently */
        return -1;
    }

    if (*rsn_ie != WLAN_EID_RSN || *(rsn_ie+1) != (u8)(rsn_ie_len - 2)) {
        return -1;
    }

    pos = rsn_ie;
    pos += 4;
    left = rsn_ie_len - 4;

    /* group_cipher */
    if (left >= RSN_SELECTOR_LEN) {
        *group_cipher = get_wpa2_cipher_suite(pos);

        pos += RSN_SELECTOR_LEN;
        left -= RSN_SELECTOR_LEN;
    } else if (left > 0) {
        return -1;
    }

    /* pairwise_cipher */
    if (left >= 2) {
        /* count = le16_to_cpu(*(u16*)pos); */
        count = os_get_unaligned_le16((u8 *)pos);
        pos += 2;
        left -= 2;

        if (count == 0 || left < count * RSN_SELECTOR_LEN) {
            return -1;
        }

        for (i = 0; i < count; i++) {
            *pairwise_cipher |= get_wpa2_cipher_suite(pos);

            pos += RSN_SELECTOR_LEN;
            left -= RSN_SELECTOR_LEN;
        }
    } else if (left == 1) {
        return -1;
    }

    if (is_8021x) {
        if (left >= 6) {
            pos += 2;
            if (!memcmp(pos, RSN_SUITE_1X, 4)) {
                *is_8021x = 1;
            }
        }
    }

    return ret;
}

/**
 * extract auth/encry type from beacon frame or probe resp frame
 *
 * @beacon_frame: [IN] original 80211 beacon frame
 * @frame_len: [IN] len of beacon frame
 *
 * Return:
 *     bss channel 1-13, 0--means invalid channel
 */
static inline int cfg80211_get_cipher_info(u8 *beacon_frame, u16 frame_len,
        u8 *auth_type, u8 *pairwise_cipher_type, u8 *group_cipher_type)
{
    struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)beacon_frame;
    u8 is_privacy = !!(mgmt->u.beacon.capab_info & WLAN_CAPABILITY_PRIVACY);

    u16 ieoffset = offsetof(struct ieee80211_mgmt, u.beacon.variable);//same as u.probe_resp.variable
    const u8 *ie = beacon_frame + ieoffset;
    u16 ielen = frame_len - ieoffset;

    u8 auth = 0, group_cipher = 0, pairwise_cipher = 0, is80211X = 0;
    const u8 *tmp;
    int ret = 0;

    tmp = cfg80211_find_ie(WLAN_EID_RSN, ie, ielen);
    if (tmp && tmp[1]) {
        ret = cfg80211_parse_wpa2_info(tmp, tmp[1] + 2, &group_cipher, &pairwise_cipher, &is80211X);
        if (is80211X)
            auth = ZC_AUTH_TYPE_WPA28021X;
        else
            auth = ZC_AUTH_TYPE_WPA2PSK;
        group_cipher = map_cipher_to_encry(group_cipher);
        pairwise_cipher = map_cipher_to_encry(pairwise_cipher);
    } else {
        tmp = cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT, WLAN_OUI_TYPE_MICROSOFT_WPA, ie, ielen);
        if (tmp) {
            ret = cfg80211_parse_wpa_info(tmp, tmp[1] + 2, &group_cipher, &pairwise_cipher, &is80211X);
            if (is80211X)
                auth = ZC_AUTH_TYPE_WPA8021X;
            else
                auth = ZC_AUTH_TYPE_WPAPSK;
            group_cipher = map_cipher_to_encry(group_cipher);
            pairwise_cipher = map_cipher_to_encry(pairwise_cipher);
        } else {
            if (is_privacy) {
                auth = ZC_AUTH_TYPE_SHARED;//TODO: WEP
                pairwise_cipher = ZC_ENC_TYPE_WEP;
                group_cipher = ZC_ENC_TYPE_WEP;
            } else {
                auth = ZC_AUTH_TYPE_OPEN;
                pairwise_cipher = ZC_ENC_TYPE_NONE;
                group_cipher = ZC_ENC_TYPE_NONE;
            }
        }
    }

    if (auth_type)
        *auth_type = auth;
    if (pairwise_cipher_type)
        *pairwise_cipher_type = pairwise_cipher;
    if (group_cipher_type)
        *group_cipher_type = group_cipher;

    return ret;
}


/**
 * extract device name attribute from wps ie struct
 *
 * @wps_ie: [IN] wps ie struct
 * @len: [OUT] len of dev name attr if exist
 *
 * Return:
 *     %NULL if dev name attr could not be found, otherwise return a
 *     pointer to dev name attr
 */
static inline u8 *get_device_name_attr_from_w(u8 *wps_ie, u8 *len)
{
    /*  6 = 1(Element ID) + 1(Length) + 4(WPS OUI) */
    u8 *attr_ptr = wps_ie + 6; /* goto first attr */
    u8 wps_ielen = wps_ie[1];

#define device_name_id        (0x1011)
    while (attr_ptr - wps_ie < wps_ielen) {
        /*  4 = 2(Attribute ID) + 2(Length) */
        u16 attr_id = os_get_unaligned_be16(attr_ptr);
        u16 attr_data_len = os_get_unaligned_be16(attr_ptr + 2);
        u16 attr_len = attr_data_len + 4;

        if (attr_id == device_name_id) {
            *len = attr_len;
            return attr_ptr;
        } else {
            attr_ptr += attr_len; /* goto next */
        }
    }
    return NULL;
}

/* storage to store apinfo */
struct ap_info *zconfig_aplist = NULL;
struct adha_info *adha_aplist = NULL;
void *clr_aplist_timer = NULL;
/* aplist num, less than MAX_APLIST_NUM */
u8 zconfig_aplist_num = 0;

struct ap_info *zconfig_get_apinfo(u8 *mac)
{
    int i;

    for (i = 1; i < zconfig_aplist_num; i++) {
        if (!memcmp(zconfig_aplist[i].mac, mac, ETH_ALEN))
            return &zconfig_aplist[i];
    }

    return NULL;
}

struct ap_info *zconfig_get_apinfo_by_ssid(u8 *ssid)
{
    int i;

    for (i = 1; i < zconfig_aplist_num; i++) {
        if (!strcmp((char *)zconfig_aplist[i].ssid, (char *)ssid))
            return &zconfig_aplist[i];
    }

    return NULL;
}

/* 通过ssid前缀 */
struct ap_info *zconfig_get_apinfo_by_ssid_prefix(u8 *ssid_prefix)
{
    int i;
    int len = strlen((const char *)ssid_prefix);
    if (!len)
        return NULL;

    for (i = 1; i < zconfig_aplist_num; i++) {
        if (!strncmp((char *)zconfig_aplist[i].ssid, (char *)ssid_prefix, len)) {
            //TODO: first match or best match???
            return &zconfig_aplist[i];//first match
        }
    }

    return NULL;
}

int str_end_with(const char *str, const char *suffix)
{
    int lenstr, lensuffix;
    if (!str || !suffix)
        return 0;
    lenstr = strlen(str);
    lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

/* 通过ssid后缀 */
struct ap_info *zconfig_get_apinfo_by_ssid_suffix(u8 *ssid_suffix)
{
    int i;
    int len = strlen((const char *)ssid_suffix);
    if (!len)
        return NULL;

    for (i = 1; i < zconfig_aplist_num; i++) {
        if (str_end_with((char *)zconfig_aplist[i].ssid, (char *)ssid_suffix)) {
            //TODO: first match or best match???
            return &zconfig_aplist[i];//first match
        }
    }

    return NULL;
}

/**
 * save apinfo
 *
 * @ssid: [IN] ap ssid
 * @bssid: [IN] ap bssid
 * @channel: [IN] ap channel
 * @auth: [IN] optional, ap auth mode, like OPEN/WEP/WPA/WPA2/WPAWPA2
 * @encry: [IN], ap encryption mode, i.e. NONE/WEP/TKIP/AES/TKIP-AES
 *
 * Note:
 *     1) if ap num exceed zconfig_aplist[], always save at [0]
 *         but why...I forgot...
 *     2) always update channel if channel != 0
 *     3) if chn is locked, save ssid to zc_ssid, because zc_ssid
 *         can be used for ssid-auto-completion
 * Return:
 *     0/success, -1/invalid params(empty ssid/bssid)
 */

static inline int __zconfig_save_apinfo(u8 *ssid, u8* bssid, u8 channel, u8 auth,
                                        u8 pairwise_cipher, u8 group_cipher, signed char rssi)
{
    int i;

    /* ssid, bssid cannot empty, channel can be 0, auth/encry can be invalid */
    if (!(ssid && bssid))
        return -1;

    /* sanity check */
    if (channel > ZC_MAX_CHANNEL || channel < ZC_MIN_CHANNEL)
        channel = 0;
    else
        zconfig_add_active_channel(channel);

    if (auth > ZC_AUTH_TYPE_MAX)
        auth = ZC_AUTH_TYPE_INVALID;

    if (pairwise_cipher > ZC_ENC_TYPE_MAX)
        pairwise_cipher = ZC_ENC_TYPE_INVALID;
    if (group_cipher > ZC_ENC_TYPE_MAX)
        group_cipher = ZC_ENC_TYPE_INVALID;

    //FIXME:
    if (pairwise_cipher == ZC_ENC_TYPE_TKIPAES)
        pairwise_cipher = ZC_ENC_TYPE_AES;//tods

    /*
     * start from zconfig_aplist[1], leave [0] for temp use
     * if zconfig_aplist[] is full, always replace [0]
     */
    if (!zconfig_aplist_num) {
        zconfig_aplist_num = 1;
    }

    for (i = 1; i < zconfig_aplist_num; i++) {
        if(!strncmp(zconfig_aplist[i].ssid, (char *)ssid, ZC_MAX_SSID_LEN)
           && !memcmp(zconfig_aplist[i].mac, bssid, ETH_ALEN)) {
            //FIXME: useless?
            /* found the same bss */
            if (!zconfig_aplist[i].channel)
                zconfig_aplist[i].channel = channel;
            if (zconfig_aplist[i].auth == ZC_AUTH_TYPE_INVALID)
                zconfig_aplist[i].auth = auth;
            if (zconfig_aplist[i].encry[0] == ZC_ENC_TYPE_INVALID)
                zconfig_aplist[i].encry[0] = group_cipher;
            if (zconfig_aplist[i].encry[1] == ZC_ENC_TYPE_INVALID)
                zconfig_aplist[i].encry[1] = pairwise_cipher;

            return 0;//duplicated ssid
        }
    }

    if (!strcmp((void *)ssid, zc_adha_ssid) && g_user_press)
        return 0;
    if (!strcmp((void *)ssid, zc_default_ssid) && g_user_press == 0)
        return 0;

    if (i < MAX_APLIST_NUM) {
        zconfig_aplist_num ++;
    } else {
        i = 0;    /* [0] for temp use, always replace [0] */
    }

    strncpy((char *)&zconfig_aplist[i].ssid, (const char *)&ssid[0], ZC_MAX_SSID_LEN - 1);
    memcpy(&zconfig_aplist[i].mac, bssid, ETH_ALEN);
    zconfig_aplist[i].auth = auth;
    zconfig_aplist[i].rssi = rssi;
    zconfig_aplist[i].channel = channel;
    zconfig_aplist[i].encry[0] = group_cipher;
    zconfig_aplist[i].encry[1] = pairwise_cipher;

    if (!strcmp((void *)ssid, zc_adha_ssid) || !strcmp((void *)ssid, zc_default_ssid)) {
        if (adha_aplist->cnt < MAX_APLIST_NUM)
            adha_aplist->aplist[adha_aplist->cnt ++] = i;
    }

    os_printf("[%d] ssid:%s, mac:%02x%02x%02x%02x%02x%02x, chn:%d, auth:%s, %s, %s, rssi:%d, adha:%d\r\n",
        i, ssid, bssid[0], bssid[1], bssid[2],
        bssid[3], bssid[4], bssid[5], channel,
        zconfig_auth_str(auth),
        zconfig_encry_str(pairwise_cipher),
        zconfig_encry_str(group_cipher), rssi > 0 ? rssi - 256 : rssi, adha_aplist->cnt);
    /*
     * if chn already locked(zc_bssid set),
     * copy ssid to zc_ssid for ssid-auto-completion
     */
    if (!memcmp(zc_bssid, bssid, ETH_ALEN) && ssid[0] != '\0') {
        strncpy((char *)zc_ssid, (char const *)ssid, ZC_MAX_SSID_LEN - 1);
    }

    return 0;
}

/**
 * extract ssid/bssid from beacon frame
 *
 * @data: [IN] original 80211 beacon frame
 * @len: [IN] len of beacon frame
 *
 * @return:
 *  == 1, found default ssid
 *  == 0, ssid saved 
 *  < 0, error
 *
 */
static inline int zconfig_extract_apinfo_from_beacon(u8 *data, u16 len, signed char rssi)
{
    u8 ssid[ZC_MAX_SSID_LEN] = {0}, bssid[ETH_ALEN] = {0};
    int ret1, ret2, channel;
    u8 auth, pairwise_cipher, group_cipher;

    ret1 = ieee80211_get_bssid(data, bssid);
    if (ret1 < 0) {
        return -1;
    }
    ret2 = ieee80211_get_ssid(data, len, ssid);
    if (ret2 < 0) {
        return -1;
    }

    /*
     * when device try to connect current adha
     * skip the new adha and process the new adha in the next scope.
     */
    extern u8 zconfig_finished;
    if (zconfig_finished && strcmp((const char *)ssid, zc_adha_ssid) == 0)
        return 0;
    /*
     * we don't process aha until user press configure button
     */
    if (g_user_press == 0 && strcmp((const char *)ssid, zc_default_ssid) == 0)
        return 0;

    channel = cfg80211_get_bss_channel(data, len);
    rssi = rssi > 0 ? rssi - 256 : rssi;

    if (strcmp((const char *)ssid, zc_default_ssid) == 0 ||
        strcmp((const char *)ssid, zc_adha_ssid) == 0 ||
        rssi > (signed char)WIFI_RX_SENSITIVITY) {
        cfg80211_get_cipher_info(data, len, &auth, &pairwise_cipher, &group_cipher);
        __zconfig_save_apinfo(ssid, bssid, channel, auth,
                              pairwise_cipher, group_cipher, rssi);
    }

    /*
     * If user press the configure button,
     * device just process aha, and skip all the adha.
     */
    if (g_user_press && !strcmp((void *)ssid, zc_default_ssid)) {
        if (adha_aplist->cnt > adha_aplist->try_idx) {
            u8 ap_idx = adha_aplist->aplist[adha_aplist->try_idx ++];
            memcpy(zc_bssid, zconfig_aplist[ap_idx].mac, ETH_ALEN);
            g_user_press = 0;
            return ALINK_DEFAULT_SSID;
        }
    }

    if (!strcmp((void *)ssid, zc_adha_ssid) && g_user_press == 0) {
        if (adha_aplist->cnt > adha_aplist->try_idx) {
            u8 ap_idx = adha_aplist->aplist[adha_aplist->try_idx ++];
            memcpy(zc_bssid, zconfig_aplist[ap_idx].mac, ETH_ALEN);
            return ALINK_ADHA_SSID;
        }

    }

    return 0;
}

static void *press_timer = NULL;
static void awss_press_timeout()
{
    g_user_press = 0;
    awss_stop_timer(press_timer);
    press_timer = NULL;
}

#define AWSS_PRESS_TIMEOUT_MS  (60000)
int awss_config_press()
{
    int timeout = os_awss_get_timeout_interval_ms();

    os_printf("enable awss\r\n");

    g_user_press = 1;

    awss_event_post(AWSS_ENABLE);

    if (press_timer == NULL)
        press_timer = HAL_Timer_Create("press", (void (*)(void *))awss_press_timeout, NULL);
    HAL_Timer_Stop(press_timer);

    if (timeout < AWSS_PRESS_TIMEOUT_MS)
        timeout = AWSS_PRESS_TIMEOUT_MS;
    HAL_Timer_Start(press_timer, timeout);

    return 0;
}

uint8_t zconfig_get_press_status()
{
    return g_user_press;
}

/**
 * save apinfo
 *
 * @ssid: [IN] ap ssid
 * @bssid: [IN] ap bssid
 * @channel: [IN] ap channel
 * @auth: [IN] optional, ap auth mode, like OPEN/WEP/WPA/WPA2/WPAWPA2
 * @encry: [IN], ap encryption mode, i.e. NONE/WEP/TKIP/AES/TKIP-AES
 *
 * Return:
 *     0/success, -1/invalid params
 */
int zconfig_set_apinfo(u8 *ssid, u8* bssid, u8 channel, u8 auth,
                       u8 pairwise_cipher, u8 group_cipher, signed char rssi)
{
    return __zconfig_save_apinfo(ssid, bssid, channel,
            auth, pairwise_cipher, group_cipher, rssi);
}

/*
 * make sure 80211 frame is word align, otherwise struct ieee80211_hdr will bug
 * TODO: code refactor, avoid using memmove
 */
#define check_ieee80211_buf_alignment(buf_addr, len) \
do {\
    if (((unsigned long)(buf_addr) & 0x1) && len > 0) {\
        u8 *word_align_addr = (u8 *)((unsigned long)(buf_addr) & ~0x1);\
        memmove(word_align_addr, buf_addr, len);\
        buf_addr = word_align_addr;\
    }\
} while (0)

static inline u8 *zconfig_remove_link_header(u8 **in, int *len, int link_type)
{
    int lt_len = 0;

    switch (link_type) {
    case AWS_LINK_TYPE_NONE:
        break;
    case AWS_LINK_TYPE_PRISM:
#define PRISM_HDR_LEN           144
        *in += PRISM_HDR_LEN;
        *len -= PRISM_HDR_LEN;
        //144, no need to check buf aligment
        break;
    case AWS_LINK_TYPE_80211_RADIO:
        lt_len = ieee80211_get_radiotap_len(*in);
        *in += lt_len;
        *len -= lt_len;
        check_ieee80211_buf_alignment(*in, *len);
        break;
    case AWS_LINK_TYPE_80211_RADIO_AVS:
#define WLANCAP_MAGIC_COOKIE_V1 0x80211001
#define WLANCAP_MAGIC_COOKIE_V2 0x80211002
        lt_len = *(u32 *)(*in + 4);/* first 4 byte is magic code */
        *in += lt_len;
        *len -= lt_len;
        check_ieee80211_buf_alignment(*in, *len);
        break;
    default:
        awss_debug("un-supported link type!\r\n");
        break;
    }

    return *in;
}

/*
 *    Note: if encry is set, goto encry_collision, because
 *    the way here we used to detection encry mode may mixed tkip & aes
 *    in some cases.
 */
#define set_encry_type(encry, value, bssid, tods)    \
do {\
    if (encry != ZC_ENC_TYPE_INVALID) {\
        log("%02x%02x%02x%02x%02x%02x, enc[%c]:%s<->%s!!!\r\n",\
                bssid[0], bssid[1], bssid[2],\
                bssid[3], bssid[4], bssid[5],\
                flag_tods(tods),\
                zconfig_encry_str(encry),\
                zconfig_encry_str(value));\
        goto encry_collision;\
    } else {\
        encry = (value);\
    }\
} while (0)

#define update_apinfo_encry_type(encry_type, bssid, tods)    \
do {\
    struct ap_info *ap_info = zconfig_get_apinfo(bssid);\
    if (ap_info && (encry_type) != ap_info->encry[tods]) {\
        awss_debug("ssid:%s, enc[%c]:%s->%s\r\n",\
            ap_info->ssid, flag_tods(tods),\
            zconfig_encry_str(ap_info->encry[tods]),\
            zconfig_encry_str(encry_type));\
        \
        warn_on(ap_info->encry[tods] != ZC_ENC_TYPE_INVALID,\
                "encry type:%s\r\n",\
                zconfig_encry_str(ap_info->encry[tods]));\
        ap_info->encry[tods] = encry_type;\
    }\
} while (0)

/**
 * ieee80211_data_extratct - extract 80211 frame info
 *
 * @in: [IN] 80211 frame
 * @len: [IN] 80211 frame len
 * @link_type: [IN] link type @see enum AWS_LINK_TYPE
 * @res: [OUT] 80211 frame parser result, see struct parser_res.
 *
 * @warning: encry_type may collision with aes & tpip in some cases,
 *         then encry_type will be set to INVALID.
 * @Return:
 *     @see enum ALINK_TYPE
 *
 * @Note: howto deal with radio SSI signal
 */
int ieee80211_data_extract(u8 *in, int len, int link_type, struct parser_res *res, signed char rssi)
{
    struct ieee80211_hdr *hdr;
    int hdrlen, fc, seq_ctrl, ret;
    int alink_type = ALINK_INVALID;

    hdr = (struct ieee80211_hdr *)zconfig_remove_link_header(&in, &len, link_type);
    if (len <= 0)
        goto drop;

    fc = hdr->frame_control;
    seq_ctrl = hdr->seq_ctrl;

    //beacon frame check
    if (ieee80211_is_beacon(fc)) {
        ret = zconfig_extract_apinfo_from_beacon(in, len, rssi);
        if (ret <= 0)
            goto drop;
        alink_type = ret;
        goto output;
    } else if (ieee80211_is_probe_resp(fc)) {
        u16 ieoffset = offsetof(struct ieee80211_mgmt, u.probe_resp.variable);
        const u8 *registrar_ie = cfg80211_find_vendor_ie(WLAN_OUI_ALIBABA,
                WLAN_OUI_TYPE_REGISTRAR, in + ieoffset, len - ieoffset);
        ret = zconfig_extract_apinfo_from_beacon(in, len, rssi);

        if (registrar_ie && g_user_press) {
            alink_type = ALINK_ZERO_CONFIG;
            res->u.ie.alink_ie_len = len - (registrar_ie - in);
            res->u.ie.alink_ie = (u8 *)registrar_ie;
            /* Note: enrollee req should never include wps ie */
            goto output;
        }

        if (ret <= 0)
            goto drop;
        alink_type = ret;
        goto output;
    } else if (ieee80211_is_probe_req(fc)) {
        u16 ieoffset = offsetof(struct ieee80211_mgmt, u.probe_req.variable);
        const u8 *wps_ie = cfg80211_find_vendor_ie(WLAN_OUI_WPS, WLAN_OUI_TYPE_WPS,
                        in + ieoffset, len - ieoffset);
        const u8 *registrar_ie = cfg80211_find_vendor_ie(WLAN_OUI_ALIBABA,
                WLAN_OUI_TYPE_REGISTRAR, in + ieoffset, len - ieoffset);

        if (!g_user_press)
            goto drop;

        if (registrar_ie) {
            alink_type = ALINK_ZERO_CONFIG;
            res->u.ie.alink_ie_len = len - (registrar_ie - in);
            res->u.ie.alink_ie = (u8 *)registrar_ie;
            /* Note: enrollee req should never include wps ie */
            goto output;
        }

        if (wps_ie) {  // got wps
            u8 attr_len = 0;
            wps_ie = get_device_name_attr_from_w((u8 *)wps_ie, &attr_len);
            if (wps_ie) {//got device name
                alink_type = ALINK_WPS;
                res->u.wps.data_len = attr_len;
                res->u.wps.data = (u8 *)wps_ie;
                goto output;
            }
        }

        goto drop;
    }

    if (!g_user_press)
        goto drop;

    /* tods = 1, fromds = 0 || tods = 0, fromds = 1 */
    if (ieee80211_has_tods(fc) == ieee80211_has_fromds(fc)) {
        goto drop;
    }
#if 1
    //frag: more, order
    if (ieee80211_has_frags(fc)) {
        goto drop;
    }
#endif
    if (!ieee80211_is_data_exact(fc)) {
        goto drop;
    } else {
        struct ap_info *ap_info;
        u8 *data, *bssid_mac, *dst_mac;
        u8 encry = ZC_ENC_TYPE_INVALID, tods;

        dst_mac = ieee80211_get_DA(hdr);
        if (memcmp(dst_mac, br_mac, ETH_ALEN))
            goto drop;/* only handle br frame */

        bssid_mac = ieee80211_get_BSSID(hdr);

        /*
         * payload len = frame.len - (radio_header + wlan_hdr)
         */
        hdrlen = ieee80211_hdrlen(fc);

#ifdef _PLATFORM_QCOM_
        //Note: http://stackoverflow.com/questions/17688710/802-11-qos-data-frames
        hdrlen = (hdrlen + 3) & 0xFC;/* align header to 32bit boundary */
#endif

        res->u.br.data_len = len - hdrlen;       /* eating the hdr */
        res->u.br.sn = IEEE80211_SEQ_TO_SN(__le16_to_cpu(seq_ctrl));
        alink_type = ALINK_BROADCAST;

        data = in + hdrlen;               /* eating the hdr */
        tods = ieee80211_has_tods(fc);

        ap_info = zconfig_get_apinfo(bssid_mac);
        if (ap_info && ZC_ENC_TYPE_INVALID != ap_info->encry[tods])
            encry = ap_info->encry[tods];
        else {
            if (!ieee80211_has_protected(fc))
                set_encry_type(encry, ZC_ENC_TYPE_NONE, bssid_mac, tods);//open
            else {
                /* Note: avoid empty null data */
                if (len < 8)        //IV + ICV + DATA >= 8
                    goto drop;
                if (!(data[3] & 0x3F))
                    set_encry_type(encry, ZC_ENC_TYPE_WEP, bssid_mac, tods);//wep
                else if (data[3] & (1 << 5)) {//Extended IV
                    if (data[1] == ((data[0] | 0x20) & 0x7F)) //tkip, WEPSeed  = (TSC1 | 0x20 ) & 0x7F
                        set_encry_type(encry, ZC_ENC_TYPE_TKIP, bssid_mac, tods);
                    if (data[2] == 0 && (!(data[3] & 0x0F)))
                        set_encry_type(encry, ZC_ENC_TYPE_AES, bssid_mac, tods);//ccmp

                    /*
                     * Note: above code use if(tkip) and if(ase)
                     * instead of if(tkip) else if(aes)
                     * beacause two condition may bother match.
                     */
                }
            }
        }

        warn_on(encry == ZC_ENC_TYPE_INVALID, "invalid encry type!\r\n");
        res->u.br.encry_type = encry;
        //apinfo's encry field updated only from beacon/probe resp frame
        //update_apinfo_encry_type(encry, bssid_mac, tods);

        goto output;
encry_collision:

        res->u.br.encry_type = ZC_ENC_TYPE_INVALID;//set encry type to invalid
    }//end of br frame

output:
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

    return alink_type;

drop:
    return ALINK_INVALID;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
