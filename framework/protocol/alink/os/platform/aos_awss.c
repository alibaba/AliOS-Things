/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <hal/hal.h>
#include <netmgr.h>
#include <aos/aos.h>

#include "platform.h"
#include "platform_config.h"
#include "os.h"

#include "ali_crypto.h"

platform_awss_recv_80211_frame_cb_t g_ieee80211_handler;
autoconfig_plugin_t g_alink_smartconfig;
platform_wifi_mgnt_frame_cb_t monitor_cb;

//一键配置超时时间, 建议超时时间1-3min, APP侧一键配置1min超时
int platform_awss_get_timeout_interval_ms(void)
{
    return 3 * 60 * 1000;
}

//默认热点配网超时时间
int platform_awss_get_connect_default_ssid_timeout_interval_ms( void )
{
    return 0;
}

//一键配置每个信道停留时间, 建议200ms-400ms
int platform_awss_get_channelscan_interval_ms(void)
{
    return 200;
}

//wifi信道切换，信道1-13
void platform_awss_switch_channel(char primary_channel,
                                  char secondary_channel, uint8_t bssid[ETH_ALEN])
{
    hal_wifi_module_t *module;

    module = hal_wifi_get_default_module();
    if (module == NULL) {
        return;
    }

    hal_wifi_set_channel(module, (int)primary_channel);
}

static void monitor_data_handler(uint8_t *buf, int len,
                                 hal_wifi_link_info_t *info)
{
    int with_fcs = 0;
    int link_type = AWSS_LINK_TYPE_NONE;

    (*g_ieee80211_handler)(buf, len, link_type, with_fcs);
}

//进入monitor模式, 并做好一些准备工作，如
//设置wifi工作在默认信道6
//若是linux平台，初始化socket句柄，绑定网卡，准备收包
//若是rtos的平台，注册收包回调函数aws_80211_frame_handler()到系统接口
void platform_awss_open_monitor(platform_awss_recv_80211_frame_cb_t cb)
{
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    if (module == NULL) {
        return;
    }

    g_ieee80211_handler = cb;
    hal_wifi_register_monitor_cb(module, monitor_data_handler);
    hal_wifi_start_wifi_monitor(module);
    platform_awss_switch_channel(6, 0, NULL);
}

//退出monitor模式，回到station模式, 其他资源回收
void platform_awss_close_monitor(void)
{
    hal_wifi_module_t *module;

    module = hal_wifi_get_default_module();
    if (module == NULL) {
        return;
    }

    hal_wifi_register_monitor_cb(module, NULL);
    hal_wifi_stop_wifi_monitor(module);
}

int platform_awss_connect_ap(
    _IN_ uint32_t connection_timeout_ms,
    _IN_ char ssid[PLATFORM_MAX_SSID_LEN],
    _IN_ char passwd[PLATFORM_MAX_PASSWD_LEN],
    _IN_OPT_ enum AWSS_AUTH_TYPE auth,
    _IN_OPT_ enum AWSS_ENC_TYPE encry,
    _IN_OPT_ uint8_t bssid[ETH_ALEN],
    _IN_OPT_ uint8_t channel)
{
    int ret, ms_cnt = 0;
    netmgr_ap_config_t config;

    strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    strncpy(config.pwd, passwd, sizeof(config.pwd) - 1);
    ret = netmgr_set_ap_config(&config);

    hal_wifi_suspend_station(NULL);
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

// This API needs to block before return
int platform_wifi_scan(platform_wifi_scan_result_cb_t cb)
{
    netmgr_register_wifi_scan_result_callback((netmgr_wifi_scan_result_cb_t)cb);
    hal_wifi_start_scan_adv(NULL);
    while (netmgr_get_scan_cb_finished() != true) { // block
        aos_msleep(500);
    }
    return 0;
}

#define KEY_LEN 16 // aes 128 cbc
p_aes128_t platform_aes128_init(
    const uint8_t *key,
    const uint8_t *iv,
    AES_DIR_t dir)
{
    ali_crypto_result result;
    void *aes_ctx;
    size_t aes_ctx_size;
    bool en_dec = true; // encrypto by default

    if (dir == PLATFORM_AES_DECRYPTION) {
        en_dec = false;
    }

    result = ali_aes_get_ctx_size(AES_CBC, &aes_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "get ctx size fail(%08x)", result);
        return NULL;
    }

    aes_ctx = os_malloc(aes_ctx_size);
    if (aes_ctx == NULL) {
        LOGE("aos_awss", "kmalloc(%d) fail", (int)aes_ctx_size);
        return NULL;
    }

    result = ali_aes_init(AES_CBC, en_dec,
                          key, NULL, KEY_LEN, iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "ali_aes_init fail(%08x)", result);
        return NULL;
    }

    return aes_ctx;
}

int platform_aes128_destroy(
    p_aes128_t aes)
{
    if (aes) {
        os_free(aes);
    }

    return 0;
}

int platform_aes128_cbc_encrypt_decrypt(
    p_aes128_t aes,
    const void *src,
    size_t blockNum,
    void *dst )
{
    ali_crypto_result result;
    size_t dlen = PLATFORM_MAX_PASSWD_LEN;

    result = ali_aes_finish(src, blockNum << 4, dst,
                            &dlen, SYM_NOPAD, aes);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "aes_cbc finish fail(%08x)", result);
        return -1;
    }

    return 0;
}

int platform_aes128_cbc_encrypt(
    p_aes128_t aes,
    const void *src,
    size_t blockNum,
    void *dst )
{
    return platform_aes128_cbc_encrypt_decrypt(aes, src, blockNum, dst);
}

int platform_aes128_cbc_decrypt(
    p_aes128_t aes,
    const void *src,
    size_t blockNum,
    void *dst )
{
    return platform_aes128_cbc_encrypt_decrypt(aes, src, blockNum, dst);
}

int platform_wifi_get_ap_info(
    char ssid[PLATFORM_MAX_SSID_LEN],
    char passwd[PLATFORM_MAX_PASSWD_LEN],
    uint8_t bssid[ETH_ALEN])
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
        strncpy(bssid, config.bssid, ETH_ALEN);
    }

    return 0;
}


int platform_wifi_low_power(int timeout_ms)
{
    //wifi_enter_power_saving_mode();
    aos_msleep(timeout_ms);
    //wifi_exit_power_saving_mode();

    return 0;
}

static void mgnt_rx_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    if (monitor_cb) {
        monitor_cb(data, len, 0, 0);
    }
}

int platform_wifi_enable_mgnt_frame_filter(
    _IN_ uint32_t filter_mask,
    _IN_OPT_ uint8_t vendor_oui[3],
    _IN_ platform_wifi_mgnt_frame_cb_t callback)
{
    monitor_cb = callback;
    if (callback != NULL) {
        hal_wlan_register_mgnt_monitor_cb(NULL, mgnt_rx_cb);
    } else {
        hal_wlan_register_mgnt_monitor_cb(NULL, NULL);
    }
    return 0;
}

int platform_wifi_send_80211_raw_frame(_IN_ enum platform_awss_frame_type type,
                                       _IN_ uint8_t *buffer, _IN_ int len)
{
    return hal_wlan_send_80211_raw_frame(NULL, buffer, len);
}

int platform_ble_start(platform_ble_dev_config *dev_conf)
{
    // Vendor to implement <TODO>
    return 0;
}

int platform_ble_end()
{
    // Vendor to implement <TODO>
    return 0;
}

int platform_ble_post(uint8_t *buffer, int length)
{
    // Vendor to implement <TODO>
    return 0;
}

int platform_ble_post_fast(uint8_t *buffer, int length)
{
    // Vendor to implement <TODO>
    return 0;
}

int platform_ble_update_adv(uint8_t *buffer, int length)
{
    // Vendor to implement <TODO>
    return 0;
}

#ifdef CONFIG_YWSS
static int smart_config_start(void)
{
    awss_start();
    return 0;
}

static void smart_config_stop(void)
{
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
