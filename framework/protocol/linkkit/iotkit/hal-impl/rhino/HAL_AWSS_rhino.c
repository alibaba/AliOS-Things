/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <hal/wifi.h>
#include <hal/hal.h>
#include <netmgr.h>
#include <aos/aos.h>
#include "platform/platform.h"
#include "iot_import_awss.h"
#include "ali_crypto.h"
//#include "os.h"

autoconfig_plugin_t g_alink_smartconfig;
platform_wifi_mgnt_frame_cb_t monitor_cb;

#if 0
/**
 * @brief   在设备的持久化外部存储器比如Flash上, 从配置区域起始位置读取数据到指定的内存缓冲区中
 *
 * @param   buffer : 存放读取配置信息的缓冲区起始地址
 * @param   length : 将要读取的数据长度, 单位是字节(Byte)
 * @retval  -1 : 读取失败
 * @retval  0 : 读取成功
 */
int HAL_Config_Read(_IN_ char *buffer, _IN_ int length)
{
    LOGW("awss", "%s not implemented yet!", __func__);
    return 0;
}

/**
 * @brief   在设备的持久化外部存储器比如Flash上, 把指定的内存缓冲区向配置区域起始位置写入
 *
 * @param   buffer : 存放要写到外存的数据的缓冲区
 * @param   length : 将要写入的数据长度, 单位是字节(Byte)
 * @retval  -1 : 写入失败
 * @retval  0 : 写入成功
 */
int HAL_Config_Write(_IN_ const char *buffer, _IN_ int length)
{
    LOGW("awss", "%s not implemented yet!", __func__);
    return 0;
}
#endif

/**
 * @brief   获取Wi-Fi的接受信号强度(`RSSI`)
 *
 * @return  信号强度数值, 单位是dBm
 */
int HAL_Wifi_Get_Rssi_Dbm(void)
{
    LOGW("awss", "%s not implemented yet!", __func__);
    return 0;
}

/**
 * @brief   使WiFi模组进入省电模式, 并持续一段时间
 *
 * @param   指定在多长时间内, WiFi模组都处于省电模式, 单位是毫秒
 * @retval  0 : 设置成功
 * @retval  -1 : 设置失败
 *
 * @note sample code
 * int HAL_Wifi_Low_Power(int timeout_ms)
 * {
 *      wifi_enter_power_saving_mode();
 *      msleep(timeout_ms);
 *      wifi_exit_power_saving_mode();
 *
 *      return 0;
 * }
 */
int HAL_Wifi_Low_Power(_IN_ int timeout_ms)
{
    aos_msleep(timeout_ms);
    return 0;
}

/**
 * @brief   获取RF433的接收信号强度(`RSSI`)
 *
 * @return  信号强度数值, 单位是dBm
 */
int HAL_RF433_Get_Rssi_Dbm(void)
{
    LOGW("awss", "%s not implemented yet!", __func__);
    return 0;
}

/**
 * @brief   获取Wi-Fi网口的MAC地址, 格式应当是"XX:XX:XX:XX:XX:XX"
 *
 * @param   mac_str : 用于存放MAC地址字符串的缓冲区数组
 * @return  指向缓冲区数组起始位置的字符指针
 */
char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN])
{
    uint8_t mac[6];

    hal_wifi_get_mac_addr(NULL, mac);

    snprintf(mac_str, PLATFORM_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return mac_str;
}

extern void wifi_get_ip(char ips[16]);

#define HAL_IP_LEN    (15 + 1)
/**
 * @brief   获取Wi-Fi网口的IP地址, 点分十进制格式保存在字符串数组出参, 二进制格式则作为返回值, 并以网络字节序(大端)表达
 *
 * @param   ifname : 指定Wi-Fi网络接口的名字
 * @param   ip_str : 存放点分十进制格式的IP地址字符串的数组
 * @return  二进制形式的IP地址, 以网络字节序(大端)组织
 */
uint32_t HAL_Wifi_Get_IP(_OU_ char ip_str[HAL_IP_LEN], _IN_ const char *ifname)
{
    (void *)ifname;
    wifi_get_ip(ip_str);
}

/**
 * @brief   获取Wi-Fi模块上的操作系统版本字符串
 *
 * @param   version_str : 存放版本字符串的缓冲区数组
 * @return  指向缓冲区数组的起始地址
 */
#define AOS_VERSION_STR "aos1.2.0"
char *HAL_Wifi_Get_Os_Version(_OU_ char version_str[STR_SHORT_LEN])
{
    return strncpy(version_str, AOS_VERSION_STR, sizeof(AOS_VERSION_STR));
}

/**
 * @brief   获取配网服务(`AWSS`)的超时时间长度, 单位是毫秒
 *
 * @return  超时时长, 单位是毫秒
 * @note    推荐时长是60,0000毫秒
 */
int HAL_Awss_Get_Timeout_Interval_Ms(void)
{
    return 3 * 60 * 1000;
}

/**
 * @brief   获取配网服务(`AWSS`)超时时长到达之后, 去连接默认SSID时的超时时长, 单位是毫秒
 *
 * @return  超时时长, 单位是毫秒
 * @note    推荐时长是0毫秒, 含义是永远持续
 */
int HAL_Awss_Get_Connect_Default_Ssid_Timeout_Interval_Ms(void)
{
    return 0;
}

/**
 * @brief   获取在每个信道(`channel`)上扫描的时间长度, 单位是毫秒
 *
 * @return  时间长度, 单位是毫秒
 * @note    推荐时长是200毫秒到400毫秒
 */
int HAL_Awss_Get_Channelscan_Interval_Ms(void)
{
    return 200;
}

/**
 * @brief   802.11帧的处理函数, 可以将802.11 Frame传递给这个函数
 *
 * @param[in] buf @n 80211 frame buffer, or pointer to struct ht40_ctrl
 * @param[in] length @n 80211 frame buffer length
 * @param[in] link_type @n AWSS_LINK_TYPE_NONE for most rtos HAL,
 *              and for linux HAL, do the following step to check
 *              which header type the driver supported.
   @verbatim
               a) iwconfig wlan0 mode monitor    #open monitor mode
               b) iwconfig wlan0 channel 6    #switch channel 6
               c) tcpdump -i wlan0 -s0 -w file.pacp    #capture 80211 frame & save
               d) open file.pacp with wireshark or omnipeek
                   check the link header type and fcs included or not
   @endverbatim
 * @param[in] with_fcs @n 80211 frame buffer include fcs(4 byte) or not
 * @param[in] rssi @n rssi of packet
 */
typedef int (*awss_recv_80211_frame_cb_t)(char *buf, int length,
        enum AWSS_LINK_TYPE link_type, int with_fcs, char rssi);

awss_recv_80211_frame_cb_t g_ieee80211_handler;

static void monitor_data_handler(uint8_t *buf, int len,
                                 hal_wifi_link_info_t *info)
{
    int with_fcs = 0;
    int link_type = AWSS_LINK_TYPE_NONE;

    (*g_ieee80211_handler)(buf, len, link_type, with_fcs, info == NULL ? 0 : info->rssi);
}

/**
 * @brief   设置Wi-Fi网卡工作在监听(Monitor)模式, 并在收到802.11帧的时候调用被传入的回调函数
 *
 * @param[in] cb @n A function pointer, called back when wifi receive a frame.
 */
void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb)
{
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    if (module == NULL) {
        return;
    }

    g_ieee80211_handler = cb;
    hal_wifi_register_monitor_cb(module, monitor_data_handler);
    hal_wifi_start_wifi_monitor(module);
    HAL_Awss_Switch_Channel(6, 0, NULL);
}

/**
 * @brief   设置Wi-Fi网卡离开监听(Monitor)模式, 并开始以站点(Station)模式工作
 */
void HAL_Awss_Close_Monitor(void)
{
    hal_wifi_module_t *module;

    module = hal_wifi_get_default_module();
    if (module == NULL) {
        return;
    }

    hal_wifi_register_monitor_cb(module, NULL);
    hal_wifi_stop_wifi_monitor(module);
}

/**
 * @brief   设置Wi-Fi网卡切换到指定的信道(channel)上
 *
 * @param[in] primary_channel @n Primary channel.
 * @param[in] secondary_channel @n Auxiliary channel if 40Mhz channel is supported, currently
 *              this param is always 0.
 * @param[in] bssid @n A pointer to wifi BSSID on which awss lock the channel, most HAL
 *              may ignore it.
 */
void HAL_Awss_Switch_Channel(
            _IN_ char primary_channel,
            _IN_OPT_ char secondary_channel,
            _IN_OPT_ uint8_t bssid[ETH_ALEN])
{
    hal_wifi_module_t *module;

    module = hal_wifi_get_default_module();
    if (module == NULL) {
        return;
    }

    hal_wifi_set_channel(module, (int)primary_channel);
}

int HAL_Sys_Net_Is_Ready()
{
    return netmgr_get_ip_state() == true ? 1 : 0;
}

/**
 * @brief   要求Wi-Fi网卡连接指定热点(Access Point)的函数
 *
 * @param[in] connection_timeout_ms @n AP connection timeout in ms or HAL_WAIT_INFINITE
 * @param[in] ssid @n AP ssid
 * @param[in] passwd @n AP passwd
 * @param[in] auth @n optional(AWSS_AUTH_TYPE_INVALID), AP auth info
 * @param[in] encry @n optional(AWSS_ENC_TYPE_INVALID), AP encry info
 * @param[in] bssid @n optional(NULL or zero mac address), AP bssid info
 * @param[in] channel @n optional, AP channel info
 * @return
   @verbatim
     = 0: connect AP & DHCP success
     = -1: connect AP or DHCP fail/timeout
   @endverbatim
 * @see None.
 * @note
 *      If the STA connects the old AP, HAL should disconnect from the old AP firstly.
 */
int HAL_Awss_Connect_Ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[HAL_MAX_SSID_LEN],
            _IN_ char passwd[HAL_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel)
{
    int ret, ms_cnt = 0;
    netmgr_ap_config_t config;
    if(ssid != NULL) {
        strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    }
    if(passwd != NULL) {
        strncpy(config.pwd, passwd, sizeof(config.pwd) - 1);
    }
    if(bssid != NULL) {
        memcpy(config.bssid, bssid, ETH_ALEN);
    }
    ret = netmgr_set_ap_config(&config);
#ifndef ESP8266_CONFIG
    printf("------------------------suspend station\n");
    hal_wifi_suspend_station(NULL);
#endif
    LOGI("aos_awss", "Will reconnect wifi: %s %s", ssid, passwd);
    netmgr_reconnect_wifi();

    while (ms_cnt < connection_timeout_ms) {
        if (netmgr_get_ip_state() == false) {
            LOGD("[waitConnAP]", "waiting for connecting AP");
            aos_msleep(500);
            ms_cnt += 500;
        } else {
            LOGI("[waitConnAP]", "AP connected");
            return 0;
        }
    }

    return -1;
}

#define FRAME_ACTION_MASK       (1 << FRAME_ACTION)
#define FRAME_BEACON_MASK       (1 << FRAME_BEACON)
#define FRAME_PROBE_REQ_MASK    (1 << FRAME_PROBE_REQ)
#define FRAME_PROBE_RESP_MASK   (1 << FRAME_PROBE_RESPONSE)
#define FRAME_DATA_MASK         (1 << FRAME_DATA)

/**
 * @brief   在当前信道(channel)上以基本数据速率(1Mbps)发送裸的802.11帧(raw 802.11 frame)
 *
 * @param[in] type @n see enum HAL_Awss_frame_type, currently only FRAME_BEACON
 *                      FRAME_PROBE_REQ is used
 * @param[in] buffer @n 80211 raw frame, include complete mac header & FCS field
 * @param[in] len @n 80211 raw frame length
 * @return
   @verbatim
   =  0, send success.
   = -1, send failure.
   = -2, unsupported.
   @endverbatim
 * @see None.
 * @note awss use this API send raw frame in wifi monitor mode & station mode
 */
int HAL_Wifi_Send_80211_Raw_Frame(_IN_ enum HAL_Awss_Frame_Type type,
                                  _IN_ uint8_t *buffer, _IN_ int len)
{
    return hal_wlan_send_80211_raw_frame(NULL, buffer, len);
}

/**
 * @brief   管理帧的处理回调函数
 *
 * @param[in] buffer @n 80211 raw frame or ie(information element) buffer
 * @param[in] len @n buffer length
 * @param[in] rssi_dbm @n rssi in dbm, set it to 0 if not supported
 * @param[in] buffer_type @n 0 when buffer is a 80211 frame,
 *                          1 when buffer only contain IE info
 * @return None.
 * @see None.
 * @note None.
 */
typedef void (*awss_wifi_mgmt_frame_cb_t)(_IN_ uint8_t *buffer, _IN_ int len,
        _IN_ char rssi_dbm, _IN_ int buffer_type);

static void mgnt_rx_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    if (monitor_cb) {
        monitor_cb(data, len, info->rssi, 0);
    }
}

/**
 * @brief   在站点(Station)模式下使能或禁用对管理帧的过滤
 *
 * @param[in] filter_mask @n see mask macro in enum HAL_Awss_frame_type,
 *                      currently only FRAME_PROBE_REQ_MASK & FRAME_BEACON_MASK is used
 * @param[in] vendor_oui @n oui can be used for precise frame match, optional
 * @param[in] callback @n see awss_wifi_mgmt_frame_cb_t, passing 80211
 *                      frame or ie to callback. when callback is NULL
 *                      disable sniffer feature, otherwise enable it.
 * @return
   @verbatim
   =  0, success
   = -1, fail
   = -2, unsupported.
   @endverbatim
 * @see None.
 * @note awss use this API to filter specific mgnt frame in wifi station mode
 */
int HAL_Wifi_Enable_Mgmt_Frame_Filter(
            _IN_ uint32_t filter_mask,
            _IN_OPT_ uint8_t vendor_oui[3],
            _IN_ awss_wifi_mgmt_frame_cb_t callback)
{
    monitor_cb = callback;

    if (callback != NULL) {
        hal_wlan_register_mgnt_monitor_cb(NULL, mgnt_rx_cb);
    } else {
        hal_wlan_register_mgnt_monitor_cb(NULL, NULL);
    }

    return 0;
}

/**
 * @brief handle one piece of AP information from wifi scan result
 *
 * @param[in] ssid @n name of AP
 * @param[in] bssid @n mac address of AP
 * @param[in] channel @n AP channel
 * @param[in] rssi @n rssi range[-100, 0].
 *          the higher the RSSI number, the stronger the signal.
 * @param[in] is_last_ap @n this AP information is the last one if is_last_ap > 0.
 *          this AP information is not the last one if is_last_ap == 0.
 * @return 0 for wifi scan is done, otherwise return -1
 * @see None.
 * @note None.
 */
typedef int (*awss_wifi_scan_result_cb_t)(
            const char ssid[HAL_MAX_SSID_LEN],
            const uint8_t bssid[ETH_ALEN],
            enum AWSS_AUTH_TYPE auth,
            enum AWSS_ENC_TYPE encry,
            uint8_t channel, char rssi,
            int is_last_ap);

/**
 * @brief   启动一次Wi-Fi的空中扫描(Scan)
 *
 * @param[in] cb @n pass ssid info(scan result) to this callback one by one
 * @return 0 for wifi scan is done, otherwise return -1
 * @see None.
 * @note
 *      This API should NOT exit before the invoking for cb is finished.
 *      This rule is something like the following :
 *      HAL_Wifi_Scan() is invoked...
 *      ...
 *      for (ap = first_ap; ap <= last_ap; ap = next_ap){
 *        cb(ap)
 *      }
 *      ...
 *      HAL_Wifi_Scan() exit...
 */
int HAL_Wifi_Scan(awss_wifi_scan_result_cb_t cb)
{
    netmgr_register_wifi_scan_result_callback((netmgr_wifi_scan_result_cb_t)cb);
    hal_wifi_start_scan_adv(NULL);

    while (netmgr_get_scan_cb_finished() != true) { // block
        aos_msleep(500);
    }

    return 0;
}

/**
 * @brief   获取所连接的热点(Access Point)的信息
 *
 * @param[out] ssid: array to store ap ssid. It will be null if ssid is not required.
 * @param[out] passwd: array to store ap password. It will be null if ap password is not required.
 * @param[out] bssid: array to store ap bssid. It will be null if bssid is not required.
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Wifi_Get_Ap_Info(
            _OU_ char ssid[HAL_MAX_SSID_LEN],
            _OU_ char passwd[HAL_MAX_PASSWD_LEN],
            _OU_ uint8_t bssid[ETH_ALEN])
{
    netmgr_ap_config_t config;

    netmgr_get_ap_config(&config);
    if (ssid) {
        strncpy(ssid, config.ssid, PLATFORM_MAX_SSID_LEN);
    }
    if (passwd) {
        strncpy(passwd, config.pwd, PLATFORM_MAX_PASSWD_LEN);
    }
    if (bssid) {
        memcpy(bssid, config.bssid, ETH_ALEN);
    }

    return 0;
}

#define KEY_LEN 16 // aes 128 cbc

static void dump_content(const uint8_t *data, size_t len)
{
    while (len--) printf("0x%02x ", *data++);
    printf("\r\n");
}

/**
 * @brief   初始化AES加密的结构体
 *
 * @param[in] key:
 * @param[in] iv:
 * @param[in] dir: AES_ENCRYPTION or AES_DECRYPTION
 * @return AES128_t
   @verbatim None
   @endverbatim
 * @see None.
 * @note None.
 */
p_HAL_Aes128_t HAL_Aes128_Init(
            _IN_ const uint8_t *key,
            _IN_ const uint8_t *iv,
            _IN_ AES_DIR_t dir)
{
    ali_crypto_result result;
    void *aes_ctx;
    size_t aes_ctx_size, alloc_siz;
    uint8_t *p;
    bool is_en = true; // encrypto by default

    if (dir == PLATFORM_AES_DECRYPTION) {
        is_en = false;
    }

    result = ali_aes_get_ctx_size(AES_CBC, &aes_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "get ctx size fail(%08x)", result);
        return NULL;
    }

    alloc_siz = aes_ctx_size + KEY_LEN * 2 + sizeof(bool);
    aes_ctx = HAL_Malloc(alloc_siz);
    if (aes_ctx == NULL) {
        LOGE("aos_awss", "kmalloc(%d) fail", (int)aes_ctx_size);
        return NULL;
    }
    memset(aes_ctx, 0, alloc_siz);

    p = (uint8_t *)aes_ctx + aes_ctx_size;
    memcpy(p, key, KEY_LEN);
    p += KEY_LEN;
    memcpy(p, iv, KEY_LEN);
    p += KEY_LEN;
    *((bool *)p) = is_en;

    return aes_ctx;
}

/**
 * @brief   销毁AES加密的结构体
 *
 * @param[in] aes:
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Destroy(_IN_ p_HAL_Aes128_t aes)
{
    if (aes) {
        HAL_Free(aes);
    }

    return 0;
}

static int platform_aes128_encrypt_decrypt(
    p_HAL_Aes128_t aes_ctx,
    const void *src,
    size_t siz,
    void *dst,
    aes_type_t t)
{
    ali_crypto_result result;
    size_t dlen, in_len = siz, ctx_siz;
    uint8_t *p, *key, *iv;
    bool is_en;
    if(aes_ctx == NULL) {
        LOGE("aos_awss", "platform_aes128_encrypt_decrypt aes_ctx is NULL");
        return -1;
    }
    result = ali_aes_get_ctx_size(AES_CBC, &ctx_siz);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "get ctx size fail(%08x)", result);
        return 0;
    }

    p = (uint8_t *)aes_ctx + ctx_siz;
    key = p;
    p += KEY_LEN;
    iv = p;
    p += KEY_LEN;
    is_en = *((uint8_t *)p);

    in_len <<= t == AES_CBC ? 4 : 0;
    dlen = in_len;

    result = ali_aes_init(t, is_en,
                          key, NULL, KEY_LEN, iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "ali_aes_init fail(%08x)", result);
        return 0;
    }

    result = ali_aes_finish(src, in_len, dst,
                            &dlen, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "aes_finish fail(%08x)", result);
        return -1;
    }

    return 0;
}

/**
 * @brief   以`AES-CBC-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥加密指定的明文
 *
 * @param[in] aes: AES handler
 * @param[in] src: plain data
 * @param[in] blockNum: plain data number of 16 bytes size
 * @param[out] dst: cipher data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Cbc_Encrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, blockNum, dst, AES_CBC);
}

/**
 * @brief   以`AES-CBC-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥解密指定的密文
 *
 * @param[in] aes: AES handler
 * @param[in] src: cipher data
 * @param[in] blockNum: plain data number of 16 bytes size
 * @param[out] dst: plain data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Cbc_Decrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, blockNum, dst, AES_CBC);
}

/**
 * @brief   以`AES-CFB-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥加密指定的明文
 *
 * @param[in] aes: AES handler
 * @param[in] src: plain data
 * @param[in] length: plain data number of 16 bytes size
 * @param[out] dst: cipher data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */

int HAL_Aes128_Cfb_Encrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t length,
            _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, length, dst, AES_CFB128);
}

/**
 * @brief   以`AES-CFB-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥解密指定的密文
 *
 * @param[in] aes: AES handler
 * @param[in] src: cipher data
 * @param[in] length: plain data number of 16 bytes size
 * @param[out] dst: plain data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Cfb_Decrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t length,
            _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, length, dst, AES_CFB128);
}

/**
 * @brief   获取`smartconfig`服务的安全等级
 *
 * @param None.
 * @return The security level:
   @verbatim
    0: open (no encrypt)
    1: aes256cfb with default aes-key and aes-iv
    2: aes128cfb with default aes-key and aes-iv
    3: aes128cfb with aes-key per product and aes-iv = 0
    4: aes128cfb with aes-key per device and aes-iv = 0
    5: aes128cfb with aes-key per manufacture and aes-iv = 0
    others: invalid
   @endverbatim
 * @see None.
 * @note The recommended value is 60,000ms.
 */
int HAL_Awss_Get_Encrypt_Type()
{
    return 3;
}

int HAL_Awss_Get_Conn_Encrypt_Type()
{
    return 4;
}

void HAL_Sys_reboot(void)
{
    aos_reboot();
}

#if 0
int platform_sys_net_is_ready(void)
{
    int ret;

    hal_wifi_link_stat_t link_stat;

    ret = hal_wifi_get_link_stat(NULL, &link_stat);
    if (ret != 0) {
        return 0;
    }

    return !!link_stat.is_connected;
}
#endif

#ifdef CONFIG_YWSS
static int smart_config_start(void)
{
    awss_start();
    return 0;
}

static void smart_config_stop(void)
{
    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);

    if(strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        return;
    }

    printf("%s %d\r\n", __func__, __LINE__);
    awss_stop();
}

static void smart_config_result_cb(int result, uint32_t ip)
{
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0u);
}

autoconfig_plugin_t g_alink_smartconfig = {
    .description = "alink_smartconfig",
    .autoconfig_start = smart_config_start,
    .autoconfig_stop = smart_config_stop,
    .config_result_cb = smart_config_result_cb
};
#endif

#ifdef __cplusplus
}
#endif
