/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"
#include "infra_defs.h"

#if defined(AWSS_SUPPORT_BEACON_ANNOUNCE)
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define POLY  0x04C11DB7

#define MAX_BEACON_ANNOUNCE_LEN 80

#define AWSS_USE_QUICK_CRC

static const char HEADER[] = {0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const char BEACON_INTER[] = {0x2c, 0x01, 0x01, 0x00};

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
#ifdef PRINT_BEACON_DEBUG
    int i;
    for (i = 0; i < len; i++) {
        HAL_Printf("%02x ", array[i]);
        if (i != 0 && i % 10 == 0) {
            HAL_Printf("\r\n");
        }
    }
#endif
}


#ifndef AWSS_USE_QUICK_CRC
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
    uint32_t temp, crc;
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
        crc = table[i];
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

static uint16_t  create_beacon_frame(uint8_t *p_buffer, simulate_ap_t  *p_ap)
{
    uint16_t len = 0;
    uint8_t t_i;
    uint64_t  t_timestamp = HAL_UptimeMs() * 1000;

    if (p_buffer == NULL || p_ap == NULL) {
        return 0;
    }
    memcpy(p_buffer, HEADER, 10);
    memcpy(p_buffer + 10, p_ap->bssid, 6);
    memcpy(p_buffer + 16, p_ap->bssid, 6);
    p_buffer[22] = (uint8_t)(p_ap->seq_id & 0xFF);
    p_buffer[23] = (uint8_t)((p_ap->seq_id >> 8) & 0xFF);
    p_ap->seq_id += 0x10;


    for (t_i = 0; t_i < 8; t_i++) {
        p_buffer[24 + t_i] = (uint8_t)((t_timestamp >> (t_i << 3)) & 0xFF);
    }
    memcpy(p_buffer + 32, BEACON_INTER, 4);
    p_buffer[36] = 0;
    p_buffer[37] = p_ap->essid_len;
    memcpy(p_buffer + 38, p_ap->essid, p_ap->essid_len);
    len = 38 + p_ap->essid_len + 4;
    get_fcs(p_buffer, len);
    print_hex_array(p_buffer, len);
    return len;
}

static int awss_set_announce_content(simulate_ap_t *ap, char *data)
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

#define VER 0
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
int aws_send_beacon_announce(void)
{
    int len;
    int ssid_len;
    uint8_t probe[MAX_BEACON_ANNOUNCE_LEN] = {0};
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

    awss_set_announce_content(&ap, ssid);

    len = create_beacon_frame(probe, &ap);

    HAL_Wifi_Send_80211_Raw_Frame(FRAME_BEACON, probe, len);
    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
