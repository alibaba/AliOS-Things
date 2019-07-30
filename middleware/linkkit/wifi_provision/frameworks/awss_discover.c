/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"
#include "linkkit/infra/infra_defs.h"

#if defined(AWSS_SUPPORT_DISCOVER)
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define AWS_DISCOVER_MIN_DURATION  100
#define MAX_MNGMT_FRAME_LEN        80
#define PRINT_DISCOVER_DEBUG       0
#define AWSS_DISCOVER_USE_QUICK_CRC
#define VER 0

#define POLY           0x04C11DB7
#define MNGMT_BEACON   0x80
#define MNGMT_RESPONSE 0x50

static const uint8_t BCAST_ADDR[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8_t BEACON_INTER[] = {0x2c, 0x01, 0x01, 0x00};

typedef struct simulate_ap {
    uint8_t bssid[6];
    uint16_t  seq_id;
    uint8_t essid_len;
    char essid[32];
} simulate_ap_t;

static uint64_t reflect(uint64_t ref, uint8_t ch)
{
    int i;
    uint64_t value = 0;
    for (i = 1; i < (ch + 1); i++) {
        if (ref & 1) {
            value |= 1 << (ch - i);
        }
        ref >>= 1;
    }
    return value;
}

static void print_hex_array(uint8_t *array, uint16_t len)
{
#if PRINT_DISCOVER_DEBUG
    int i;
    for (i = 0; i < len; i++) {
        HAL_Printf("%02x ", array[i]);
        if (i != 0 && i % 10 == 0) {
            HAL_Printf("\r\n");
        }
    }
#endif
}

#ifndef AWSS_DISCOVER_USE_QUICK_CRC
static uint32_t crc32_bit(uint8_t *ptr, uint32_t len)
{
    uint8_t i;
    uint32_t crc = 0xffffffff;
    while (len--) {
        for (i = 1; i != 0; i <<= 1) {
            if ((crc & 0x80000000) != 0) {
                crc <<= 1;
                crc ^= POLY;
            } else {
                crc <<= 1;
            }
            if ((*ptr & i) != 0) {
                crc ^= POLY;
            }
        }
        ptr++;
    }
    return (reflect(crc, 32) ^ 0xffffffff);
}

#else
static uint32_t Table2[256];

static void gen_normal_table(uint32_t *table)
{
    uint32_t gx = 0x04c11db7;
    uint32_t temp;
    int i, j;
    for (i = 0; i <= 0xFF; i++) {
        temp = reflect(i, 8);
        table[i] = temp << 24;
        for (j = 0; j < 8; j++) {
            unsigned long int t1, t2;
            unsigned long int flag = table[i] & 0x80000000;
            t1 = (table[i] << 1);
            if (flag == 0) {
                t2 = 0;
            } else {
                t2 = gx;
            }
            table[i] = t1 ^ t2;
        }
        table[i] = reflect(table[i], 32);
    }
}

static uint32_t reverse_table_crc(uint8_t *data, int32_t len, uint32_t *table)
{
    uint32_t crc = 0xffffffff;
    uint8_t *p = data;
    int i;
    for (i = 0; i < len; i++) {
        crc = table[(crc ^ (*(p + i))) & 0xff] ^ (crc >> 8);
    }
    return  ~crc;
}

static uint32_t crc32_bit(uint8_t *ptr, uint32_t len)
{
    uint32_t crc;
    static uint8_t init_table = 0;
    if (init_table == 0) {
        init_table = 1;
        gen_normal_table(Table2);
    }
    crc =  reverse_table_crc(ptr, len, Table2);
    return crc;
}
#endif

static void get_fcs(uint8_t *p_buffer, uint16_t length)
{
    uint32_t crc;
    if (length < 4) {
        return;
    }

    crc = crc32_bit(p_buffer, length - 4);

    p_buffer[length - 4] = (crc >> 24) & 0xff;
    p_buffer[length - 3] = (crc >> 16) & 0xff;
    p_buffer[length - 2] = (crc >> 8) & 0xff;
    p_buffer[length - 1] = (crc) & 0xff;
}

static uint16_t create_mng_frame(uint8_t *p_buffer, uint8_t type, uint8_t dst[6], simulate_ap_t  *p_ap)
{
    uint16_t len = 0;
    uint8_t t_i;
    static uint16_t seq_id = 0;
    uint64_t  t_timestamp = HAL_UptimeMs() * 1000;

    if (p_buffer == NULL || p_ap == NULL || dst == NULL) {
        return 0;
    }
    p_ap->seq_id = seq_id;
    /* memcpy(p_buffer, HEADER, 10);*/
    p_buffer[len] = type;
    len += 1; /*len = 1 */
    p_buffer[len] = 0;
    len += 1; /*len = 2 */
    p_buffer[len] = 0;
    len += 1; /*len = 3 */
    p_buffer[len] = 0;
    len += 1; /*len = 4 */
    memcpy(p_buffer + len, dst, 6);
    len += 6; /*len = 10 */
    memcpy(p_buffer + len, p_ap->bssid, 6);
    len += 6; /*len = 16 */
    memcpy(p_buffer + len, p_ap->bssid, 6);
    len += 6; /*len = 22 */
    p_buffer[len] = (uint8_t)(p_ap->seq_id & 0xFF);
    len += 1; /*len = 23 */
    p_buffer[len] = (uint8_t)((p_ap->seq_id >> 8) & 0xFF);
    len += 1; /*len = 24 */
    seq_id += 0x10;

    for (t_i = 0; t_i < 8; t_i++) {
        p_buffer[len + t_i] = (uint8_t)((t_timestamp >> (t_i << 3)) & 0xFF);
    }
    len += 8; /*len = 32 */

    memcpy(p_buffer + len, BEACON_INTER, 4);
    len += 4; /*len = 36 */
    p_buffer[len] = 0;
    len += 1; /*len = 37 */
    p_buffer[len] = p_ap->essid_len;
    len += 1; /*len = 38 */
    memcpy(p_buffer + len, p_ap->essid, p_ap->essid_len);
    len += p_ap->essid_len; /*len = 38+p_ap->essid_len */

    len += 4;
    get_fcs(p_buffer, len);
    print_hex_array(p_buffer, len);
    return len;
}

static int awss_set_discover_content(simulate_ap_t *ap, char *data)
{
    uint8_t len;
    if (data == NULL || ap == NULL) {
        return -1;
    }

    len = strlen(data);
    if (len > 32) {
        len = 32;
    }
    ap->essid_len = len;
    memcpy(ap->essid, data, len);
    return 0;
}

static int get_ability(char *src, uint8_t mac[6])
{
    uint32_t mac_org_bits = 0;
    uint32_t ab_org_bits = 0;
    uint32_t mac_dst_bits = 0;
    uint32_t ab_dst_bits = 0;
    uint8_t i;

    if (src == NULL || mac == NULL) {
        return 0;
    }

    mac_org_bits = mac[3] | (mac[4] << 8) | mac[5] << 16;
    mac_dst_bits = (mac_org_bits & 0x3f)  | (((mac_org_bits >> 6) & 0x3f) << 8) |
                   (((mac_org_bits >> 12) & 0x3f) << 16)  | (((mac_org_bits >> 18) & 0x3f) << 24);
    ab_org_bits |= VER & 0x03;
#ifdef AWSS_SUPPORT_SMARTCONFIG
    ab_org_bits |= 0x01 << 3;
#endif
#ifdef AWSS_SUPPORT_SMARTCONFIG_MCAST
    ab_org_bits |= 0x01 << 4;
#endif
#ifdef AWSS_SUPPORT_SMARTCONFIG_WPS
    ab_org_bits |= 0x01 << 5;
#endif
#ifdef AWSS_SUPPORT_AHA
    ab_org_bits |= 0x01 << 6;
#endif
#ifdef AWSS_SUPPORT_DEV_AP
    ab_org_bits |= 0x01 << 7;
#endif
#ifdef AWSS_SUPPORT_ZEROCONFIG
    ab_org_bits |= 0x01 << 8;
#endif
    ab_dst_bits = (ab_org_bits & 0x3f) | (((ab_org_bits >> 6) & 0x3f) << 8);

    for (i = 0; i < 4; i++) {
        src[i] = ((mac_dst_bits >> i * 8) & 0x3f) + 32;
    }

    for (i = 0; i < 4; i++) {
        src[i + 4] = ((ab_dst_bits >> i * 8) & 0x3f) + 32;
    }
    return 8;
}

static int aws_discover_send(uint8_t ftype, uint8_t dst[6])
{
    int len;
    int ssid_len;
    uint8_t probe[MAX_MNGMT_FRAME_LEN] = {0};
    char _product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char ssid[33] = {0};
    simulate_ap_t ap;
    memset(&ap, 0, sizeof(simulate_ap_t));

    os_wifi_get_mac(ap.bssid);
    HAL_GetProductKey(_product_key);
    strcpy(ssid, "adn");
    ssid_len = 3;
    strncpy(ssid + ssid_len, _product_key, IOTX_PRODUCT_KEY_LEN);
    ssid_len += strlen(_product_key);
    strcpy(ssid + ssid_len, "_");
    ssid_len += 1;
    ssid_len += get_ability(ssid + ssid_len, ap.bssid);

    awss_set_discover_content(&ap, ssid);

    len = create_mng_frame(probe, ftype, dst, &ap);

    HAL_Wifi_Send_80211_Raw_Frame(FRAME_BEACON, probe, len);
    return 0;
}
/**
 * @brief management frame handler
 *
 * @param[in] buffer @n 80211 raw frame or ie(information element) buffer
 * @param[in] len @n buffer length
 * @param[in] buffer_type @n 0 when buffer is a 80211 frame,
 *                          1 when buffer only contain IE info
 * @return None.
 * @see None.
 * @note None.
 */
static void aws_discover_callback(uint8_t *buffer, int length, signed char rssi, int buffer_type)
{
#define MGMT_BEACON     (0x80)
#define MGMT_PROBE_REQ  (0x40)
#define MGMT_PROBE_RESP (0x50)
#define MGMT_HDR_LEN     (24)

    static uint32_t last_time = 0;
    /* fc(2) + dur(2) + da(6) + sa(6) + bssid(6) + seq(2) */
    uint8_t dst[6] = {0};
    int type = buffer[0];
    switch (type) {
        case MGMT_BEACON:
            break;
        case MGMT_PROBE_REQ: {

            if (time_elapsed_ms_since(last_time) < AWS_DISCOVER_MIN_DURATION) {
                break;
            }
            last_time = os_get_time_ms();
            if (length >= 16) {
                memcpy(dst, buffer + 10, 6);
            }
            print_hex_array(buffer, length);
            aws_discover_send(MNGMT_RESPONSE, dst);
        }
        break;
        case MGMT_PROBE_RESP:
            break;
        default:
            break;
    }
}

int aws_discover_send_beacon()
{
    return aws_discover_send(MNGMT_BEACON, (uint8_t *)BCAST_ADDR);
}

int aws_discover_init()
{
    return HAL_Wifi_Enable_Mgmt_Frame_Filter(FRAME_PROBE_REQ_MASK,
            NULL, aws_discover_callback);
}

int aws_discover_deinit()
{
    return HAL_Wifi_Enable_Mgmt_Frame_Filter(FRAME_PROBE_REQ_MASK,
            NULL, NULL);
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
