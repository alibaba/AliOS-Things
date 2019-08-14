/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "hal/wifi.h"
#include "netmgr.h"
#include "fs/spiffs.h"

#include "jse_port.h"
#include "be_common.h"

/**
 * @brief       get platform type
 *
 * @return      platform type(string)
 */
const char *jse_system_get_platform_type(void)
{
    return SYSINFO_DEVICE_NAME;
}

/**
 * @brief       get ip address
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_get_ip(char *ip)
{
    /* netmgr_get_ip_state not work in esp32
       if(false == netmgr_get_ip_state()){
           return -1;
       } */
    netmgr_wifi_get_ip(ip);

    if (0 == strcmp(ip, "0.0.0.0")) {
        return -1;
    }
    return 0;
}

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_kv_init(void)
{
    /* kv init in aos kernel */
    return 0;
}

/**
 * @brief       get value based on key
 *
 * @param[in]   key: key(string)
 * @param[out]  buffer: store value to buffer
 * @param[out]  buffer_len:  value length
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_kv_get(const char *key, void *buffer, uint32_t *buffer_len)
{
    return aos_kv_get(key, buffer, buffer_len);
}

/**
 * @brief       set key-value
 *
 * @param[in]   key: key(string)
 * @param[in]   value: value
 * @param[in]   sync: 1: sync, 2: not sync
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_kv_set(const char *key, const void *value, int len, int sync)
{
    return aos_kv_set(key, value, len, sync);
}

/**
 * @brief       connect wifi with ssid & password
 *
 * @param[in]   ssid: wifi ssid
 * @param[in]   password: wifi password
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_connect(uint8_t ssid[32], uint8_t password[64])
{
    netmgr_connect((const char *)ssid, (const char *)password, 10 * 1000);

    return 0;
}

/**
 * @brief       get wifi ssid
 *
 * @param[out]   ssid: wifi ssid
 * @param[in]   len: wifi ssid len
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_ssid_get(char *ssid, int len)
{
    netmgr_ap_config_t apconfig;

    netmgr_get_ap_config(&apconfig);
    strncpy(ssid, (const char *)apconfig.ssid, len - 1);

    return 0;
}

/**
 * @brief       get wifi mac address
 *
 * @param[out]   mac: wifi mac address
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_mac_get(uint8_t mac[6])
{
    netmgr_ap_config_t apconfig;

    netmgr_get_ap_config(&apconfig);
    memcpy(mac, apconfig.bssid, 6);

    return 0;
}

/**
 * @brief       wifi initialize
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_wifi_init(void)
{
    static uint8_t wifi_inited = 0;

    if (wifi_inited) {
        return 0;
    }
    wifi_inited = 1;

    LOGI("", "Wifi initializing...\r\n");
#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    netmgr_init();
    netmgr_start(false);
    aos_msleep(200);

    LOGI("", "Wifi initialized.\r\n");
    return 0;
}

/**
 * @brief       filesystem initialize
 *
 * @return      0: success, -1: failed
 */
int32_t jse_system_fs_init(void)
{
    LOGI("", "SPIFFS initializing...\r\n");
    return spiffs_register();
}

/**
 * @brief       system reboot
 *
 * @return      void
 */
void jse_system_reboot(void)
{
    LOGI("", "reboot!\n");
    aos_reboot();
}