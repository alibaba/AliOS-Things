/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#if defined(DEV_BIND_ENABLED)

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <hal/wifi.h>

//#include <netmgr.h>
#include "iot_import.h"
#include "ali_crypto.h"

/*
 * @brief   获取Wi-Fi网口的MAC地址, 格式应当是"XX:XX:XX:XX:XX:XX"
 *
 * @param  mac_str : 用于存放MAC地址字符串的缓冲区数组
 * @return  指向缓冲区数组起始位置的字符指针
 * @note   基础通用HAL，都需要对接
 */
char *HAL_Wifi_Get_Mac(char mac_str[HAL_MAC_LEN])
{
#if 0
    uint8_t mac[6] = {0};

    hal_wifi_get_mac_addr(NULL, mac);

    snprintf(mac_str, HAL_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return mac_str;
#else
    return NULL;
#endif
}

//extern void wifi_get_ip(char ips[16]);

/*
 * @brief   获取Wi-Fi网口的IP地址, 点分十进制格式保存在字符串数组,
 *          二进制格式则作为返回值, 并以网络字节序(大端)表达
 *
 * @param   ifname : 指定Wi-Fi网络接口的名字，单端口Wi-Fi可以忽略
 *                   此参数，该参数可能为NULL
 * @param   ip_str : 存放点分十进制格式的IP地址字符串的数组
 * @return  二进制形式的IP地址, 以网络字节序(大端)组织
 * @note   基础通用HAL，都需要对接
 */
uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN],
                         const char *ifname)
{
    //(void *)ifname;
//    wifi_get_ip(ip_str);
    return 0;
}

/*
 * @brief   获取配网服务(`AWSS`)扫描的超时时间长度, 单位是毫秒
 *
 * @return  超时时长, 单位是毫秒
 * @note  1）推荐时长是180,000毫秒
 *        2）厂商可以根据项目需要实现该函数，如天猫项目中都统一设置成了10分钟
 *        3）除设备热点配网不需要对接外，其他各种配网都需要对接
 */
int HAL_Awss_Get_Timeout_Interval_Ms(void)
{
    return 3 * 60 * 1000;
}

/*
 * @brief   获取在每个信道(channel)上扫描的时间长度, 单位是毫秒
 *
 * @return  时间长度, 单位是毫秒
 * @note
 *     1） 推荐时长是200毫秒到400毫秒，建议250毫秒，各模组或芯片
 *         厂商可以根据自己模组或芯片质量的好坏决定，模组质量好的，
 *         可以靠近200ms，模组质量不太好的，建议放大到250ms以上
 *     2）除设备热点配网不需要对接外，其他各种配网都需要对接
 */
int HAL_Awss_Get_Channelscan_Interval_Ms(void)
{
    return 250;
}

/*
 * @brief   802.11帧的处理函数, 可以将802.11 Frame传递给这个函数
 *
 * @param[in] buf @n 80211 frame buffer, or pointer to struct ht40_ctrl
 * @param[in] length @n 80211 frame buffer length
 * @param[in] link_type @n AWSS_LINK_TYPE_NONE for most rtos HAL,
 *            and for linux HAL, do the following step to check
 *            which header type the driver supported.
 * @verbatim
 *          a) iwconfig wlan0 mode monitor    #open monitor mode
 *          b) iwconfig wlan0 channel 6    #switch channel 6
 *          c) tcpdump -i wlan0 -s0 -w file.pacp    #capture 80211 frame
 *          d) open file.pacp with wireshark or omnipeek check the link header
 *             type and fcs included or not
 * @endverbatim
 * @param[in] with_fcs @n 80211 frame buffer include fcs(4 byte) or not
 * @param[in] rssi @n rssi of packet
 */
//awss_recv_80211_frame_cb_t g_ieee80211_handler;

static void monitor_data_handler(uint8_t *buf, int len, hal_wifi_link_info_t *info)
{
#if 0
    int with_fcs  = 0;
    unsigned char rssi = -1;
    int link_type = AWSS_LINK_TYPE_NONE;

    if (info) rssi = info->rssi;

    (*g_ieee80211_handler)((char *)buf, len, link_type, with_fcs, rssi);
#endif
}

/*
 * @brief   设置Wi-Fi网卡工作在监听(Monitor)模式,
 * 并在收到802.11帧的时候调用被传入的回调函数
 *
 * @param[in] cb @n A function pointer, called back when wifi receive a frame.
 */
void HAL_Awss_Open_Monitor(awss_recv_80211_frame_cb_t cb)
{
#if 0
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    if (module == NULL) {
        return;
    }

    g_ieee80211_handler = cb;
    hal_wifi_register_monitor_cb(module, monitor_data_handler);
    hal_wifi_start_wifi_monitor(module);
    HAL_Awss_Switch_Channel(6, 0, NULL);
#endif
}

/*
 * @brief   设置Wi-Fi网卡离开监听(Monitor)模式,
 * 并开始以站点(Station)模式工作
 */
void HAL_Awss_Close_Monitor(void)
{
#if 0
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    if (module == NULL) {
        return;
    }

    hal_wifi_register_monitor_cb(module, NULL);
    hal_wifi_stop_wifi_monitor(module);
#endif
}

/*
 * @brief   开启设备热点（SoftAP模式）
 *
 * @param[in] ssid @n 热点的ssid字符；
 * @param[in] passwd @n 热点的passwd字符；
 * @param[in] beacon_interval @n 热点的Beacon广播周期（广播间隔）；
 * @param[in] hide @n 是否是隐藏热点，hide:0, 非隐藏, 其它值：隐藏；
 * @return，
 * @verbatim
 *   = 0: success
 *   = -1: unsupported
 *   = -2: failure with system error
 *   = -3: failure with no memory
 *   = -4: failure with invalid parameters
 * @endverbatim
 * @Note:
 *       1）ssid和passwd都是以'\0'结尾的字符串，如果passwd字符串的
 *          长度为0，表示该热点采用Open模式（不加密）；
 *       2）beacon_interval表示热点的Beacon广播间隔（或周期），单
 *          位为毫秒，一般会采用默认100ms；
 *       3）hide表示创建的热点是否是隐藏热点，hide=0表示非隐藏热
 *         点，其他值表示隐藏热点；
 */

int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval, int hide)
{
#if 0
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    if (module == NULL || module->start_ap == NULL) {
        return -1;
    }
    return module->start_ap(module, ssid, passwd, beacon_interval, hide);
#else
    return 0;
#endif
}

/*
 * @brief   关闭当前设备热点，并把设备由SoftAP模式切换到Station模式
 *
 * @return，
 * @verbatim
 *   = 0: success
 *   = -1: unsupported
 *   = -2: failure
 * @endverbatim
 * @Note:
 *       1）如果当前设备已经开启热点，关闭当前热点，如果当前设备正
 *          在开热点，取消开热点的操作；
 *       2）如果当前设备不是以Station模式（包括Station+SoftAP模式和
 *          SoftAP模式）工作，设备必须切换到Station模式；
 *       3）Wi-Fi状态机需要切换到初始化状态，因为接下来很可能进行
 *          连接某一个路由器操作；
 */
int HAL_Awss_Close_Ap()
{
#if 0
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    if (module == NULL || module->stop_ap == NULL) {
        return -1;
    }
    return module->stop_ap(module);
#else
    return 0;
#endif
}

/*
 * @brief   设置Wi-Fi网卡、模组或芯片切换到指定的信道(channel)上
 *
 * @param[in] primary_chan @n 首选信道
 * @param[in] sec_chan @n 辅助信道（信道带宽为40MHz是才会
 *                        使用，信道宽度为20MHz时可以忽略该参数）
 * @param[in] bssid @n 该参数原计划HAL可以根据bssid过滤包，目前大部
 *                    分模组或芯片厂商都没有实现，已经废弃，可以忽略
 * @note   除设备热点配网不需要对接外，其他各种配网都需要对接
 */
void HAL_Awss_Switch_Channel(char primary_channel,
                             char secondary_channel,
                             uint8_t bssid[ETH_ALEN])
{
#if 0
    hal_wifi_module_t *module = hal_wifi_get_default_module();

    if (module == NULL) {
        return;
    }

    hal_wifi_set_channel(module, (int)primary_channel);
#endif
}

/*
 * @brief 检查Wi-Fi网卡、芯片或模组当前的IP地址是否有效，AliOS内部
 *        已经对接完成
 *
 * @param None.
 * @return, 0: IP地址无效; 1: IP地址有效，可以发起网络连接
 * @see None.
 * @note
 *    1）该API对接有问题，可能导致AWSS一致无法退出，有可能出现设
 *       备成功连接AP后又重新开始扫描
 *    2）各种配网都需要对接
 */
int HAL_Sys_Net_Is_Ready()
{
#if 0
    return netmgr_get_ip_state() == true ? 1 : 0;
#else
    return 1;
#endif
}

/*
 * @brief   要求Wi-Fi网卡、模组或芯片连接指定热点(Access Point)的函数
 *
 * @param[in] connection_timeout_ms @n 连接AP的超时时间
 * @param[in] ssid @n 目的AP的SSID
 * @param[in] passwd @n 目的AP的PASSWD
 * @param[in] auth @n 目的AP的加密方式，HAL可以忽略
 * @param[in] encry @n目的AP的认证方式，HAL可以忽略
 * @param[in] bssid @n 目的AP的BSSID，该字段可能为NULL或设置为全
 *                     零，如果该字段为NULL或设置为全零，HAL根据
 *                     SSID和PASSWD匹配热点，如果该字段为有效值，
 *                     HAL根据SSID、PASSWD、BSSID匹配热点
 * @param[in] channel @n 目的热点的信道，该字段可以忽略
 * @return
 *   @verbatim
 *      = 0: connect AP & DHCP success
 *      = -1: connect AP or DHCP fail/timeout
 *    @endverbatim
 * @see None.
 * @note
 *      1）如果芯片或模组当前已经连接到某热点，HAL首先检查当前连接
 *         的热点是否与目的热点相同（匹配SSID、PASSWD和BSSID（如
 *         果有效）），如果相同可以直接返回成功；反之，HAL应该先断开
 *         当前的连接，然后再去连接目的热点；
 *      2）如果BSSID字段有效，HAL应该根据SSID和BSSID匹配热点；
 *      3）各种配网都需要对接
 */
int HAL_Awss_Connect_Ap(uint32_t connection_timeout_ms,
                        char ssid[HAL_MAX_SSID_LEN],
                        char passwd[HAL_MAX_PASSWD_LEN],
                        enum AWSS_AUTH_TYPE auth,
                        enum AWSS_ENC_TYPE  encry,
                        uint8_t bssid[ETH_ALEN],
                        uint8_t channel)
{
#if 0
    int ms_cnt = 0;
    netmgr_ap_config_t config = { 0 };
    if (ssid != NULL) {
        strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    }
    if (passwd != NULL) {
        strncpy(config.pwd, passwd, sizeof(config.pwd) - 1);
    }
    if (bssid != NULL) {
        memcpy(config.bssid, bssid, ETH_ALEN);
    }
    netmgr_set_ap_config(&config);
#ifndef ESP8266_CONFIG
    printf("------------------------suspend station\n");
    hal_wifi_suspend_station(NULL);
#endif
    // LOGI("aos_awss", "Will reconnect wifi: %s %s", ssid, passwd);
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
#endif
    return -1;
}

#define FRAME_ACTION_MASK     (1 << FRAME_ACTION)
#define FRAME_BEACON_MASK     (1 << FRAME_BEACON)
#define FRAME_PROBE_REQ_MASK  (1 << FRAME_PROBE_REQ)
#define FRAME_PROBE_RESP_MASK (1 << FRAME_PROBE_RESPONSE)
#define FRAME_DATA_MASK       (1 << FRAME_DATA)

/*
 * @brief   在当前信道(channel)上以基本数据速率(1Mbps)发送裸的802.11帧(raw 802.11 frame)
 *
 * @param[in] type @n see enum HAL_Awss_frame_type, currently only FRAME_BEACON
 *                      FRAME_PROBE_REQ is used
 * @param[in] buffer @n 80211 raw frame, include complete mac header & FCS field
 * @param[in] len @n 80211 raw frame length
 * @return
 *      @verbatim
 *          =  0, send success.
 *          = -1, send failure.
 *          = -2, unsupported.
 *      @endverbatim
 * @see None.
 * @note awss use this API send raw frame in wifi monitor mode & station mode
 */
int HAL_Wifi_Send_80211_Raw_Frame(enum HAL_Awss_Frame_Type type,
                                  uint8_t *buffer, int len)
{
#if 0
    return hal_wlan_send_80211_raw_frame(NULL, buffer, len);
#else
    return 0;
#endif
}

/*
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
typedef void (*awss_wifi_mgmt_frame_cb_t)(uint8_t *buffer, int len,
                                          signed char rssi_dbm,
                                          int buffer_type);

//static awss_wifi_mgmt_frame_cb_t monitor_cb = NULL;
static void mgnt_rx_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
#if 0
    if (monitor_cb) {
        monitor_cb(data, len, info->rssi, 0);
    }
#endif
}

/*
 * @brief   在站点(Station)模式下使能或禁用对管理帧的过滤
 *
 * @param[in] filter_mask @n see mask macro in enum HAL_Awss_frame_type,
 *                      currently only FRAME_PROBE_REQ_MASK & FRAME_BEACON_MASK is used
 * @param[in] vendor_oui @n oui can be used for precise frame match,optional
 * @param[in] callback @n see awss_wifi_mgmt_frame_cb_t, passing 80211
 *                      frame or ie to callback. when callback is NULL
 *                      disable sniffer feature, otherwise enable it.
 * @return
 *      @verbatim
 *      =  0, success
 *      = -1, fail
 *      = -2, unsupported.
 *      @endverbatim
 * @see None.
 * @note awss use this API to filter specific mgnt frame in wifi station mode
 */
int HAL_Wifi_Enable_Mgmt_Frame_Filter(uint32_t filter_mask, uint8_t vendor_oui[3],
                                      awss_wifi_mgmt_frame_cb_t callback)
{
#if 0
    monitor_cb = callback;

    if (callback != NULL) {
        hal_wlan_register_mgnt_monitor_cb(NULL, mgnt_rx_cb);
    } else {
        hal_wlan_register_mgnt_monitor_cb(NULL, NULL);
    }
#endif
    return 0;
}

/*
 * @brief handle one piece of AP information from wifi scan result
 *
 * @param[in] ssid @n name of AP
 * @param[in] bssid @n mac address of AP
 * @param[in] channel @n AP channel
 * @param[in] rssi @n rssi range[-127, -1].
 *          the higher the RSSI number, the stronger the signal.
 * @param[in] is_last_ap @n this AP information is the last one if is_last_ap > 0.
 *            this AP information is not the last one if is_last_ap == 0.
 * @return 0 for wifi scan is done, otherwise return -1
 * @see None.
 * @note None.
 */
typedef int (*awss_wifi_scan_result_cb_t)(const char ssid[HAL_MAX_SSID_LEN],
                                          const uint8_t bssid[ETH_ALEN],
                                          enum AWSS_AUTH_TYPE auth,
                                          enum AWSS_ENC_TYPE  encry,
                                          uint8_t channel, signed char rssi,
                                          int is_last_ap);

/*
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
#if 0
    netmgr_register_wifi_scan_result_callback((netmgr_wifi_scan_result_cb_t)cb);
    hal_wifi_start_scan_adv(NULL);

    while (netmgr_get_scan_cb_finished() != true) {
        aos_msleep(50);
    }
#endif
    return 0;
}

/*
 * @brief   获取所连接的热点(Access Point)的信息
 *
 * @param[out] ssid: array to store ap ssid. It will be null if ssid is not required.
 * @param[out] passwd: array to store ap password. It will be null if ap password is not required.
 * @param[out] bssid: array to store ap bssid. It will be null if bssid is not required.
 * @return
 *      @verbatim
 *        = 0: succeeded
 *        = -1: failed
 *      @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Wifi_Get_Ap_Info(char ssid[HAL_MAX_SSID_LEN],
                         char passwd[HAL_MAX_PASSWD_LEN],
                         uint8_t bssid[ETH_ALEN])
{
#if 0
    netmgr_ap_config_t config = {0};

    netmgr_get_ap_config(&config);
    if (ssid) {
        strncpy(ssid, config.ssid, HAL_MAX_SSID_LEN - 1);
    }
    if (passwd) {
        strncpy(passwd, config.pwd, HAL_MAX_PASSWD_LEN - 1);
    }
    if (bssid) {
        memcpy(bssid, config.bssid, ETH_ALEN);
    }
#endif
    return 0;
}

/*
 * @brief   获取smartconfig一键配网的安全等级
 *
 * @param None.
 * @return The security level:
 *    @verbatim
 *     0~2: 废弃
 *     3: 一型一密
 *     4: 一机一密
 *    @endverbatim
 * @Note，1）默认采用一型一密
 *        2）各种配网都需要对接（基本没有工作量）
 */
int HAL_Awss_Get_Encrypt_Type()
{
    return 3;
}

/*
 * @brief    获取零配、手机热点配网和路由器配网的安全等级
 *
 * @param None.
 * @return The security level:
 *    @verbatim
 *     3: 一型一密
 *     4: 一机一密
 *    @endverbatim
 * @Note，
 *     1） 默认采用一机一密，如果设备在飞燕平台创建时选择的安全等级
 *         是一型一密的，并且设备出厂烧录的信息只包含ProductKey，
 *         ProductSecret，DeviceName，不包含DeviceSecret，此函数一定
 *         要返回一型一密，否则第一次零配、热点配网和路由器配网会失
 *         败，第一个和天猫精灵的语音配网也会失败；
 *     2） 各种配网都需要对接（基本没有工作量）
 */
int HAL_Awss_Get_Conn_Encrypt_Type()
{
#if 0
    char invalid_ds[DEVICE_SECRET_LEN + 1] = {0};
    char ds[DEVICE_SECRET_LEN + 1] = {0};

    HAL_GetDeviceSecret(ds);

    if (memcmp(invalid_ds, ds, sizeof(ds)) == 0)
        return 3;

    memset(invalid_ds, 0xff, sizeof(invalid_ds));
    if (memcmp(invalid_ds, ds, sizeof(ds)) == 0)
        return 3;

    return 4;
#else
    return 3;
#endif
}

#ifdef __cplusplus
}
#endif
#endif
