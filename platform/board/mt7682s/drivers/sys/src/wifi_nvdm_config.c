/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdio.h>
#include <string.h>
#ifdef USE_FREERTOS_ENABLE
#include "FreeRTOS.h"
#else
#include "freertos_to_aos.h"
#endif
#include "nvdm.h"
#include "syslog.h"
#include "lwip/sockets.h"
#include "connsys_profile.h"
#include "connsys_util.h"
#include "get_profile_string.h"
#include "wifi_nvdm_config.h"
#include "type_def.h"
#include "wifi_init.h"
#include "wifi_inband.h"
#include "nvdm_config.h"
#include "wifi_default_config.h"

/* common config */
static const group_data_item_t g_common_data_item_array[] = {
    NVDM_DATA_ITEM("OpMode",             WIFI_DEFAULT_OPMODE),
    NVDM_DATA_ITEM("CountryCode",        WIFI_DEFAULT_COUNTRY_CODE),
    NVDM_DATA_ITEM("CountryRegion",      WIFI_DEFAULT_COUNTRY_REGION),
    NVDM_DATA_ITEM("CountryRegionABand", WIFI_DEFAULT_COUNTRY_REGION_A_BAND),
    NVDM_DATA_ITEM("RadioOff",           WIFI_DEFAULT_RADIO_ONOFF),
    NVDM_DATA_ITEM("DbgLevel",           WIFI_DEFAULT_N9_DEBUG_LEVEL),
    NVDM_DATA_ITEM("RTSThreshold",       WIFI_DEFAULT_RTS_THRESHOLD),
    NVDM_DATA_ITEM("FragThreshold",      WIFI_DEFAULT_FRAGMENT_THRESHOLD),
    NVDM_DATA_ITEM("BGChannelTable",     WIFI_DEFAULT_BG_CHANNEL_TABLE),
    NVDM_DATA_ITEM("AChannelTable",      WIFI_DEFAULT_A_CHANNEL_TABLE),
    NVDM_DATA_ITEM("syslog_filters",     WIFI_DEFAULT_SYSLOG_FILTERS),
    NVDM_DATA_ITEM("WiFiPrivilegeEnable",WIFI_DEFAULT_WIFI_PRIVILEGE_ENABLE),
    NVDM_DATA_ITEM("StaFastLink",        WIFI_DEFAULT_STA_FAST_LINK),
    
#ifdef MTK_SINGLE_SKU_SUPPORT
    NVDM_DATA_ITEM("SupportSingleSKU",   WIFI_DEFAULT_SUPPORT_SINGLE_SKU),
    NVDM_DATA_ITEM("SingleSKU2G_0",      WIFI_DEFAULT_SKU_2G_GROUP0),
    NVDM_DATA_ITEM("SingleSKU5G_L0",     WIFI_DEFAULT_SKU_5G_L0),
    NVDM_DATA_ITEM("SingleSKU5G_M0",     WIFI_DEFAULT_SKU_5G_M0),
    NVDM_DATA_ITEM("SingleSKU5G_H0",     WIFI_DEFAULT_SKU_5G_H0),
#endif
};

/* STA config */
static const group_data_item_t g_sta_data_item_array[] = {
    NVDM_DATA_ITEM("LocalAdminMAC",      WIFI_DEFAULT_STA_LOCAL_ADMIN_MAC),
    NVDM_DATA_ITEM("MacAddr",            WIFI_DEFAULT_STA_MAC_ADDR),
    NVDM_DATA_ITEM("Ssid",               WIFI_DEFAULT_STA_SSID),
    NVDM_DATA_ITEM("SsidLen",            WIFI_DEFAULT_STA_SSID_LEN),
    NVDM_DATA_ITEM("BssType",            WIFI_DEFAULT_STA_BSS_TYPE),
    NVDM_DATA_ITEM("Channel",            WIFI_DEFAULT_STA_CHANNEL),
    NVDM_DATA_ITEM("BW",                 WIFI_DEFAULT_STA_BANDWIDTH),
    NVDM_DATA_ITEM("WirelessMode",       WIFI_DEFAULT_STA_WIRELESS_MODE),
    NVDM_DATA_ITEM("BADecline",          WIFI_DEFAULT_STA_BA_DECLINE),
    NVDM_DATA_ITEM("AutoBA",             WIFI_DEFAULT_STA_AUTO_BA),
    NVDM_DATA_ITEM("HT_MCS",             WIFI_DEFAULT_STA_HT_MCS),
    NVDM_DATA_ITEM("HT_BAWinSize",       WIFI_DEFAULT_STA_HT_BA_WINDOW_SIZE),
    NVDM_DATA_ITEM("HT_GI",              WIFI_DEFAULT_STA_HT_GI),
    NVDM_DATA_ITEM("HT_PROTECT",         WIFI_DEFAULT_STA_HT_PROTECT),
    NVDM_DATA_ITEM("HT_EXTCHA",          WIFI_DEFAULT_STA_HT_EXT_CHANNEL),
    NVDM_DATA_ITEM("WmmCapable",         WIFI_DEFAULT_STA_WMM_CAPABLE),
    NVDM_DATA_ITEM("ListenInterval",     WIFI_DEFAULT_STA_LISTEN_INTERVAL),
    NVDM_DATA_ITEM("AuthMode",           WIFI_DEFAULT_STA_AUTH_MODE),
    NVDM_DATA_ITEM("EncrypType",         WIFI_DEFAULT_STA_ENCRYPT_TYPE),
    NVDM_DATA_ITEM("WpaPsk",             WIFI_DEFAULT_STA_WPA_PSK),
    NVDM_DATA_ITEM("WpaPskLen",          WIFI_DEFAULT_STA_WPA_PSK_LEN),
    NVDM_DATA_ITEM("PMK_INFO",           "0"),
    NVDM_DATA_ITEM("PairCipher",         WIFI_DEFAULT_STA_PAIR_CIPHER),
    NVDM_DATA_ITEM("GroupCipher",        WIFI_DEFAULT_STA_GROUP_CIPHER),
    NVDM_DATA_ITEM("DefaultKeyId",       WIFI_DEFAULT_STA_DEFAULT_KEY_ID),
    NVDM_DATA_ITEM("SharedKey",          WIFI_DEFAULT_STA_SHARE_KEY),
    NVDM_DATA_ITEM("SharedKeyLen",       WIFI_DEFAULT_STA_SHARE_KEY_LEN),
    NVDM_DATA_ITEM("PSMode",             WIFI_DEFAULT_STA_POWER_SAVE_MODE),
    NVDM_DATA_ITEM("KeepAlivePeriod",    WIFI_DEFAULT_STA_KEEP_ALIVE_PERIOD),
    NVDM_DATA_ITEM("BeaconLostTime",     WIFI_DEFAULT_STA_BEACON_LOST_TIME),
    NVDM_DATA_ITEM("ApcliBWAutoUpBelow", WIFI_DEFAULT_APCLI_BW_AUTO_UP_BELOW),
    NVDM_DATA_ITEM("StaKeepAlivePacket", WIFI_DEFAULT_STA_KEEP_ALIVE_PACKET),
};

/* AP config */
static const group_data_item_t g_ap_data_item_array[] = {
    NVDM_DATA_ITEM("LocalAdminMAC",      WIFI_DEFAULT_AP_LOCAL_ADMIN_MAC),
    NVDM_DATA_ITEM("MacAddr",            WIFI_DEFAULT_AP_MAC_ADDR),
    NVDM_DATA_ITEM("Ssid",               WIFI_DEFAULT_AP_SSID),
    NVDM_DATA_ITEM("SsidLen",            WIFI_DEFAULT_AP_SSID_LEN),
    NVDM_DATA_ITEM("Channel",            WIFI_DEFAULT_AP_CHANNEL),
    NVDM_DATA_ITEM("BW",                 WIFI_DEFAULT_AP_BANDWIDTH),
    NVDM_DATA_ITEM("WirelessMode",       WIFI_DEFAULT_AP_WIRELESS_MODE),
    NVDM_DATA_ITEM("AutoBA",             WIFI_DEFAULT_AP_AUTO_BA),
    NVDM_DATA_ITEM("HT_MCS",             WIFI_DEFAULT_AP_HT_MCS),
    NVDM_DATA_ITEM("HT_BAWinSize",       WIFI_DEFAULT_AP_HT_BA_WINDOW_SIZE),
    NVDM_DATA_ITEM("HT_GI",              WIFI_DEFAULT_AP_HT_GI),
    NVDM_DATA_ITEM("HT_PROTECT",         WIFI_DEFAULT_AP_HT_PROTECT),
    NVDM_DATA_ITEM("HT_EXTCHA",          WIFI_DEFAULT_AP_HT_EXT_CHANNEL),
    NVDM_DATA_ITEM("WmmCapable",         WIFI_DEFAULT_AP_WMM_CAPABLE),
    NVDM_DATA_ITEM("DtimPeriod",         WIFI_DEFAULT_AP_DTIM_PERIOD),
    NVDM_DATA_ITEM("AuthMode",           WIFI_DEFAULT_AP_AUTH_MODE),
    NVDM_DATA_ITEM("EncrypType",         WIFI_DEFAULT_AP_ENCRYPT_TYPE),
    NVDM_DATA_ITEM("WpaPsk",             WIFI_DEFAULT_AP_WPA_PSK),
    NVDM_DATA_ITEM("WpaPskLen",          WIFI_DEFAULT_AP_WPA_PSK_LEN),
    NVDM_DATA_ITEM("PairCipher",         WIFI_DEFAULT_AP_PAIR_CIPHER),
    NVDM_DATA_ITEM("GroupCipher",        WIFI_DEFAULT_AP_GROUP_CIPHER),
    NVDM_DATA_ITEM("DefaultKeyId",       WIFI_DEFAULT_AP_DEFAULT_KEY_ID),
    NVDM_DATA_ITEM("SharedKey",          WIFI_DEFAULT_AP_SHARE_KEY),
    NVDM_DATA_ITEM("SharedKeyLen",       WIFI_DEFAULT_AP_SHARE_KEY_LEN),
    NVDM_DATA_ITEM("HideSSID",           WIFI_DEFAULT_AP_HIDDEN_SSID),
    NVDM_DATA_ITEM("RekeyInterval",      WIFI_DEFAULT_AP_REKEY_INTERVAL),
    NVDM_DATA_ITEM("AutoChannelSelect",  WIFI_DEFAULT_AP_AUTO_CHANNEL_SELECT),
    NVDM_DATA_ITEM("BcnDisEn",           WIFI_DEFAULT_AP_BEACON_DISABLE),
};

/* user defined callback functions for each group */
static void common_check_default_value(void)
{
    check_default_value("common",
                        g_common_data_item_array,
                        sizeof(g_common_data_item_array) / sizeof(g_common_data_item_array[0]));
}

static void common_reset_to_default(void)
{
    reset_to_default("common",
                     g_common_data_item_array,
                     sizeof(g_common_data_item_array) / sizeof(g_common_data_item_array[0]));
}

static void common_show_value(void)
{
    show_group_value("common",
                     g_common_data_item_array,
                     sizeof(g_common_data_item_array) / sizeof(g_common_data_item_array[0]));
}

static void sta_check_default_value(void)
{
    check_default_value("STA",
                        g_sta_data_item_array,
                        sizeof(g_sta_data_item_array) / sizeof(g_sta_data_item_array[0]));
}

static void sta_reset_to_default(void)
{
    reset_to_default("STA",
                     g_sta_data_item_array,
                     sizeof(g_sta_data_item_array) / sizeof(g_sta_data_item_array[0]));
}

static void sta_show_value(void)
{
    show_group_value("STA",
                     g_sta_data_item_array,
                     sizeof(g_sta_data_item_array) / sizeof(g_sta_data_item_array[0]));
}

static void ap_check_default_value(void)
{
    check_default_value("AP",
                        g_ap_data_item_array,
                        sizeof(g_ap_data_item_array) / sizeof(g_ap_data_item_array[0]));
}

static void ap_reset_to_default(void)
{
    reset_to_default("AP",
                     g_ap_data_item_array,
                     sizeof(g_ap_data_item_array) / sizeof(g_ap_data_item_array[0]));
}

static void ap_show_value(void)
{
    show_group_value("AP",
                     g_ap_data_item_array,
                     sizeof(g_ap_data_item_array) / sizeof(g_ap_data_item_array[0]));
}

const user_data_item_operate_t wifi_data_item_operate_array[] = {
    {
        "common",
        common_check_default_value,
        common_reset_to_default,
        common_show_value,
    },
    {
        "STA",
        sta_check_default_value,
        sta_reset_to_default,
        sta_show_value,
    },
    {
        "AP",
        ap_check_default_value,
        ap_reset_to_default,
        ap_show_value,
    },
};

static void save_wep_key_length(uint8_t *length, char *wep_key_len, uint8_t key_id)
{
    uint8_t id = 0;
    uint8_t index = 0;

    do {
        if ('\0' == wep_key_len[index]) {
            LOG_E(wifi, "index not found");
            return;
        }
        if (key_id == id) {
            *length = (uint8_t)atoi(&wep_key_len[index]);
            return;
        }
        if (',' == wep_key_len[index++]) {
            id++;
        }
    } while (id < 4);
    LOG_E(wifi, "index not found: %d", key_id);
}

static void save_shared_key(uint8_t *wep_key, uint8_t *raw_wep_key, uint8_t length, uint8_t key_id)
{
    uint8_t id = 0;
    uint8_t index = 0;

    do {
        if ('\0' == raw_wep_key[index]) {
            LOG_E(wifi, "index not found");
            return;
        }
        if (key_id == id) {
            memcpy(wep_key, &raw_wep_key[index], length);
            wep_key[length] = '\0';
            LOG_E(wifi, "obtained wep key: %s", wep_key);
            return;
        }
        if (',' == raw_wep_key[index++]) {
            id++;
        }
    } while (id < 4);
    LOG_E(wifi, "index not found: %d", key_id);
}

int32_t wifi_config_init(wifi_cfg_t *wifi_config)
{
#ifdef MTK_WIFI_PROFILE_ENABLE

    // init wifi profile
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);

    // common
    len = sizeof(buff);
    nvdm_read_data_item("common", "OpMode", buff, &len);
    wifi_config->opmode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryCode", buff, &len);
    if (len <= sizeof(wifi_config->country_code)) {
        memcpy(wifi_config->country_code, buff, len);
    } else {
        LOG_E(wifi, "Invalid length of country code: %d", len);
        return -1;
    }
    // STA
    len = sizeof(buff);
    nvdm_read_data_item("STA", "SsidLen", buff, &len);
    wifi_config->sta_ssid_len = (uint8_t)atoi((char *)buff);
    if(wifi_config->sta_ssid_len > sizeof(wifi_config->sta_ssid)) {
        LOG_E(wifi, "Invalid length of STA SSID: %d", wifi_config->sta_ssid_len);
        return -1;
    }
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Ssid", buff, &len);
    memcpy(wifi_config->sta_ssid, buff, wifi_config->sta_ssid_len);

    len = sizeof(buff);
    nvdm_read_data_item("STA", "EncrypType", buff, &len);
    if (WIFI_ENCRYPT_TYPE_WEP_ENABLED == (uint8_t)atoi((char *)buff)) {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "DefaultKeyId", buff, &len);
        wifi_config->sta_default_key_id = (uint8_t)atoi((char *)buff);

        len = sizeof(buff);
        nvdm_read_data_item("STA", "SharedKeyLen", buff, &len);
        save_wep_key_length(&wifi_config->sta_wpa_psk_len, (char *)buff, wifi_config->sta_default_key_id);

        len = sizeof(buff);
        nvdm_read_data_item("STA", "SharedKey", buff, &len);
        save_shared_key(wifi_config->sta_wpa_psk, buff, wifi_config->sta_wpa_psk_len, wifi_config->sta_default_key_id);
    } else {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WpaPskLen", buff, &len);
        wifi_config->sta_wpa_psk_len = (uint8_t)atoi((char *)buff);
        if(wifi_config->sta_wpa_psk_len >= sizeof(wifi_config->sta_wpa_psk)) {
            LOG_E(wifi, "Invalid length of STA PSK: %d", wifi_config->sta_wpa_psk_len);
            return -1;
        }
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WpaPsk", buff, &len);
        memcpy(wifi_config->sta_wpa_psk, buff, wifi_config->sta_wpa_psk_len);
    }
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BW", buff, &len);
    wifi_config->sta_bandwidth = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WirelessMode", buff, &len);
    wifi_config->sta_wireless_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "ListenInterval", buff, &len);
    wifi_config->sta_listen_interval = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    wifi_config->sta_power_save_mode = (uint8_t)atoi((char *)buff);

    // AP
#ifdef MTK_WIFI_REPEATER_ENABLE
    if (wifi_config->opmode == WIFI_MODE_REPEATER) {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "Channel", buff, &len);
        wifi_config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "BW", buff, &len);
        wifi_config->ap_bw = (uint8_t)atoi((char *)buff);
    } else {
#endif
        /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
        len = sizeof(buff);
        nvdm_read_data_item("AP", "Channel", buff, &len);
        wifi_config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "BW", buff, &len);
        wifi_config->ap_bw = (uint8_t)atoi((char *)buff);
#ifdef MTK_WIFI_REPEATER_ENABLE
    }
#endif /* MTK_WIFI_REPEATER_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("AP", "SsidLen", buff, &len);
    wifi_config->ap_ssid_len = (uint8_t)atoi((char *)buff);
    if(wifi_config->ap_ssid_len >= sizeof(wifi_config->ap_ssid)) {
        LOG_E(wifi, "Invalid length of AP SSID: %d", wifi_config->ap_ssid_len);
        return -1;
    }
    len = sizeof(buff);
    nvdm_read_data_item("AP", "Ssid", buff, &len);
    memcpy(wifi_config->ap_ssid, buff, wifi_config->ap_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HideSSID", buff, &len);
    wifi_config->ap_hide_ssid = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AuthMode", buff, &len);
    wifi_config->ap_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "EncrypType", buff, &len);
    wifi_config->ap_encryp_type = (uint8_t)atoi((char *)buff);

    if (WIFI_ENCRYPT_TYPE_WEP_ENABLED == wifi_config->ap_encryp_type) {
        len = sizeof(buff);
        nvdm_read_data_item("AP", "DefaultKeyId", buff, &len);
        wifi_config->ap_default_key_id = (uint8_t)atoi((char *)buff);

        len = sizeof(buff);
        nvdm_read_data_item("AP", "SharedKeyLen", buff, &len);
        save_wep_key_length(&wifi_config->ap_wpa_psk_len, (char *)buff, wifi_config->ap_default_key_id);

        len = sizeof(buff);
        nvdm_read_data_item("AP", "SharedKey", buff, &len);
        save_shared_key(wifi_config->ap_wpa_psk, buff, wifi_config->ap_wpa_psk_len, wifi_config->ap_default_key_id);
    } else {
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WpaPskLen", buff, &len);
        wifi_config->ap_wpa_psk_len = (uint8_t)atoi((char *)buff);
        if(wifi_config->ap_wpa_psk_len >= sizeof(wifi_config->ap_wpa_psk)) {
            LOG_E(wifi, "Invalid length of AP PSK: %d", wifi_config->ap_wpa_psk_len);
            return -1;
        }
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WpaPsk", buff, &len);
        memcpy(wifi_config->ap_wpa_psk, buff, wifi_config->ap_wpa_psk_len);
    }
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WirelessMode", buff, &len);
    wifi_config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DtimPeriod", buff, &len);
    wifi_config->ap_dtim_interval = (uint8_t)atoi((char *)buff);

#else
    //wifi profile is disabled, take the user

#endif
    return 0;
}

