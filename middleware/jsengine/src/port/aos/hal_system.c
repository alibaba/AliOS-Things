/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hal/wifi.h>
#include <netmgr.h>
#include "aos/kernel.h"

#include <be_common.h>
#include "hal/system.h"

/***************************************************
 * @fn			hal_system_get_platform_type
 *
 * @brief		获取当前平台类型
 *
 * @param		无
 *
 * @return		字符串.
 **************************************************/
const char *hal_system_get_platform_type(void)
{
    return SYSINFO_DEVICE_NAME;
}

/**************************************************
 * @fn 		 hal_system_get_ip
 *
 * @brief     获取系统IP
 *
 * @param     ip：存储ip字符串的buffer
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/

int32_t hal_system_get_ip(char *ip)
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

/**************************************************
* @fn 		 hal_system_kv_init
*
* @brief     初始化key-value系统，如ESP使用nvs实现，
             需要先初始化nvs
*
* @param    无
*
* @return 	 成功返回０　失败返回其他
**************************************************/
int32_t hal_system_kv_init(void)
{
    /* kv init in aos kernel */
    return 0;
}

/**************************************************
 * @fn 		 hal_system_kv_get
 *
 * @brief     key-value获取函数
 *
 * @param     key: key-value的key指针
 *            buffer: 存储value的buffer
 *            buffer_len: buffer的len指针
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/

int32_t hal_system_kv_get(const char *key, void *buffer, uint32_t *buffer_len)
{
    return aos_kv_get(key, buffer, buffer_len);
}

/**************************************************
 * @fn 		 hal_system_kv_set
 *
 * @brief     key-value设置函数
 *
 * @param     key: key-value的key指针
 *            buffer: 待设置成value的buffer
 *            buffer_len: buffer的len指针
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/
int32_t hal_system_kv_set(const char *key, const void *value, int len, int sync)
{
    return aos_kv_set(key, value, len, sync);
}

/**************************************************
 * @fn 		 hal_system_wifi_connect
 *
 * @brief     连接wifi
 *
 * @param     ssid：wifi名称，32位的字符串
 *            password: wifi密码,64位的字符串
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/

int32_t hal_system_wifi_connect(uint8_t ssid[32], uint8_t password[64])
{
    netmgr_connect(ssid, password, 10 * 1000);

    return 0;
}

/**************************************************
 * @fn 		 hal_system_wifi_ssid_get
 *
 * @brief     获取wifi名称
 *
 * @param     ssid : buffer
 *            len: buffer len
 *
 * @return 	 成功返回0 失败返回-1
 **************************************************/
int32_t hal_system_wifi_ssid_get(char *ssid, int len)
{
    netmgr_ap_config_t apconfig;

    netmgr_get_ap_config(&apconfig);
    strncpy(ssid, (const char *)apconfig.ssid, len - 1);

    return 0;
}

int32_t hal_system_wifi_mac_get(uint8_t mac[6])
{
    netmgr_ap_config_t apconfig;

    netmgr_get_ap_config(&apconfig);
    memcpy(mac, apconfig.bssid, 6);

    return 0;
}

/**************************************************
 * @fn 		 hal_system_wifi_init
 *
 * @brief     wifi初始化
 *
 * @param     无
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/
int32_t hal_system_wifi_init(void)
{
    static uint8_t wifi_inited = 0;

    if (wifi_inited) {
        return 0;
    }
    wifi_inited = 1;

#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    netmgr_init();
    netmgr_start(false);
    aos_msleep(200);

    return 0;
}

/**************************************************
 * @fn 		 hal_system_fs_init
 *
 * @brief     文件系统初始化，可以在此根据平台初始化spiffs或者fatfs
 *
 * @param     无
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/
int32_t hal_system_fs_init(void)
{
    printf("Initializing SPIFFS\r\n");
    return vfs_spiffs_register();
}

/**************************************************
 * @fn 		 hal_system_reboot
 *
 * @brief     系统重启函数
 *
 * @param     无
 *
 * @return 	 无
 **************************************************/
void hal_system_reboot(void)
{
    printf("reboot!\n");
    aos_reboot();
}