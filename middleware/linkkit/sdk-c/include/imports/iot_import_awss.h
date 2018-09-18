/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __IOT_IMPORT_AWSS_H__
#define __IOT_IMPORT_AWSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _IN_
#define _IN_
#endif
#ifndef _OU_
#define _OU_
#endif
#ifndef _IN_OPT_
#define _IN_OPT_
#endif

#define STR_SHORT_LEN                   (32)
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif
#define HAL_MAX_SSID_LEN                (32 + 1)    /* ssid: 32 octets at most, include the NULL-terminated */
#define HAL_MAX_PASSWD_LEN              (64 + 1)    /* password: 8-63 ascii */
#define HAL_MAC_LEN                     (17 + 1)


#define HAL_CONFIG_SIZE                 (2048)

/**
 * @brief   在设备的持久化外部存储器比如Flash上, 从配置区域起始位置读取数据到指定的内存缓冲区中
 *
 * @param   buffer : 存放读取配置信息的缓冲区起始地址
 * @param   length : 将要读取的数据长度, 单位是字节(Byte)
 * @retval  -1 : 读取失败
 * @retval  0 : 读取成功
 */
int HAL_Config_Read(_IN_ char *buffer, _IN_ int length);

/**
 * @brief   在设备的持久化外部存储器比如Flash上, 把指定的内存缓冲区向配置区域起始位置写入
 *
 * @param   buffer : 存放要写到外存的数据的缓冲区
 * @param   length : 将要写入的数据长度, 单位是字节(Byte)
 * @retval  -1 : 写入失败
 * @retval  0 : 写入成功
 */
int HAL_Config_Write(_IN_ const char *buffer, _IN_ int length);

/**
 * @brief   获取Wi-Fi的接受信号强度(`RSSI`)
 *
 * @return  信号强度数值, 单位是dBm
 */
int HAL_Wifi_Get_Rssi_Dbm(void);

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
int HAL_Wifi_Low_Power(_IN_ int timeout_ms);

/**
 * @brief   获取RF433的接收信号强度(`RSSI`)
 *
 * @return  信号强度数值, 单位是dBm
 */
int HAL_RF433_Get_Rssi_Dbm(void);

/**
 * @brief   获取Wi-Fi网口的MAC地址, 格式应当是"XX:XX:XX:XX:XX:XX"
 *
 * @param   mac_str : 用于存放MAC地址字符串的缓冲区数组
 * @return  指向缓冲区数组起始位置的字符指针
 */
char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN]);

#define HAL_IP_LEN    (15 + 1)
/**
 * @brief   获取Wi-Fi网口的IP地址, 点分十进制格式保存在字符串数组出参, 二进制格式则作为返回值, 并以网络字节序(大端)表达
 *
 * @param   ifname : 指定Wi-Fi网络接口的名字
 * @param   ip_str : 存放点分十进制格式的IP地址字符串的数组
 * @return  二进制形式的IP地址, 以网络字节序(大端)组织
 */
uint32_t HAL_Wifi_Get_IP(_OU_ char ip_str[HAL_IP_LEN], _IN_ const char *ifname);

/**
 * @brief   获取Wi-Fi模块上的操作系统版本字符串
 *
 * @param   version_str : 存放版本字符串的缓冲区数组
 * @return  指向缓冲区数组的起始地址
 */
char *HAL_Wifi_Get_Os_Version(_OU_ char version_str[STR_SHORT_LEN]);

/**
 * @brief   获取配网服务(`AWSS`)的超时时间长度, 单位是毫秒
 *
 * @return  超时时长, 单位是毫秒
 * @note    推荐时长是60,0000毫秒
 */
int HAL_Awss_Get_Timeout_Interval_Ms(void);

/**
 * @brief   获取配网服务(`AWSS`)超时时长到达之后, 去连接默认SSID时的超时时长, 单位是毫秒
 *
 * @return  超时时长, 单位是毫秒
 * @note    推荐时长是0毫秒, 含义是永远持续
 */
int HAL_Awss_Get_Connect_Default_Ssid_Timeout_Interval_Ms(void);

/**
 * @brief   获取在每个信道(`channel`)上扫描的时间长度, 单位是毫秒
 *
 * @return  时间长度, 单位是毫秒
 * @note    推荐时长是200毫秒到400毫秒
 */
int HAL_Awss_Get_Channelscan_Interval_Ms(void);

/* link type */
enum AWSS_LINK_TYPE {
    /* rtos HAL choose this type */
    AWSS_LINK_TYPE_NONE,

    /* linux HAL may choose the following type */
    AWSS_LINK_TYPE_PRISM,
    AWSS_LINK_TYPE_80211_RADIO,
    AWSS_LINK_TYPE_80211_RADIO_AVS,
    AWSS_LINK_TYPE_HT40_CTRL /* for espressif HAL, see struct ht40_ctrl */
};

struct HAL_Ht40_Ctrl {
    uint16_t    length;
    uint8_t     filter;
    signed char rssi;
};

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
 * @param[in] rssi @n rssi of packet, range of [-127, -1]
 */
typedef int (*awss_recv_80211_frame_cb_t)(char *buf, int length,
        enum AWSS_LINK_TYPE link_type, int with_fcs, signed char rssi);

/**
 * @brief   设置Wi-Fi网卡工作在监听(Monitor)模式, 并在收到802.11帧的时候调用被传入的回调函数
 *
 * @param[in] cb @n A function pointer, called back when wifi receive a frame.
 */
void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb);

/**
 * @brief   设置Wi-Fi网卡离开监听(Monitor)模式, 并开始以站点(Station)模式工作
 */
void HAL_Awss_Close_Monitor(void);

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
            _IN_OPT_ uint8_t bssid[ETH_ALEN]);

/* auth type */
enum AWSS_AUTH_TYPE {
    AWSS_AUTH_TYPE_OPEN,
    AWSS_AUTH_TYPE_SHARED,
    AWSS_AUTH_TYPE_WPAPSK,
    AWSS_AUTH_TYPE_WPA8021X,
    AWSS_AUTH_TYPE_WPA2PSK,
    AWSS_AUTH_TYPE_WPA28021X,
    AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWSS_AUTH_TYPE_MAX = AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWSS_AUTH_TYPE_INVALID = 0xff,
};

/* encryt type */
enum AWSS_ENC_TYPE {
    AWSS_ENC_TYPE_NONE,
    AWSS_ENC_TYPE_WEP,
    AWSS_ENC_TYPE_TKIP,
    AWSS_ENC_TYPE_AES,
    AWSS_ENC_TYPE_TKIPAES,
    AWSS_ENC_TYPE_MAX = AWSS_ENC_TYPE_TKIPAES,
    AWSS_ENC_TYPE_INVALID = 0xff,
};

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
 *      If bssid specifies the dest AP, HAL should use bssid to connect dest AP.
 */
int HAL_Awss_Connect_Ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[HAL_MAX_SSID_LEN],
            _IN_ char passwd[HAL_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel);

/**
 * @brief check system network is ready(get ip address) or not.
 *
 * @param None.
 * @return 0, net is not ready; 1, net is ready.
 * @see None.
 * @note None.
 */
int HAL_Sys_Net_Is_Ready();

/* 80211 frame type */
typedef enum HAL_Awss_Frame_Type {
    FRAME_ACTION,
    FRAME_BEACON,
    FRAME_PROBE_REQ,
    FRAME_PROBE_RESPONSE,
    FRAME_DATA
} HAL_Awss_Frame_Type_t;

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
                                  _IN_ uint8_t *buffer, _IN_ int len);

/**
 * @brief   管理帧的处理回调函数
 *
 * @param[in] buffer @n 80211 raw frame or ie(information element) buffer
 * @param[in] len @n buffer length
 * @param[in] rssi_dbm @n rssi in dbm, range of [-127, -1], set it to -1 if not supported
 * @param[in] buffer_type @n 0 when buffer is a 80211 frame,
 *                          1 when buffer only contain IE info
 * @return None.
 * @see None.
 * @note None.
 */
typedef void (*awss_wifi_mgmt_frame_cb_t)(_IN_ uint8_t *buffer, _IN_ int len,
        _IN_ signed char rssi_dbm, _IN_ int buffer_type);

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
            _IN_ awss_wifi_mgmt_frame_cb_t callback);

typedef struct {
    enum AWSS_AUTH_TYPE auth;
    enum AWSS_ENC_TYPE encry;
    uint8_t channel;
    signed char rssi_dbm;
    char ssid[HAL_MAX_SSID_LEN];
    uint8_t mac[ETH_ALEN];
} awss_ap_info_t;

/**
 * @brief handle one piece of AP information from wifi scan result
 *
 * @param[in] ssid @n name of AP
 * @param[in] bssid @n mac address of AP
 * @param[in] channel @n AP channel
 * @param[in] rssi @n rssi range[-127, -1].
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
            uint8_t channel, signed char rssi,
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
int HAL_Wifi_Scan(awss_wifi_scan_result_cb_t cb);

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
 * @note
 *     If the STA dosen't connect AP successfully, HAL should return -1 and not touch the ssid/passwd/bssid buffer.
 */
int HAL_Wifi_Get_Ap_Info(
            _OU_ char ssid[HAL_MAX_SSID_LEN],
            _OU_ char passwd[HAL_MAX_PASSWD_LEN],
            _OU_ uint8_t bssid[ETH_ALEN]);

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
 */
int HAL_Awss_Get_Encrypt_Type();

/**
 * @brief    Get Security level for wifi configuration with connection.
 *           Used for AP solution of router and App.
 *
 * @param None.
 * @return The security level:
   @verbatim
    3: aes128cfb with aes-key per product and aes-iv = random
    4: aes128cfb with aes-key per device and aes-iv = random
    5: aes128cfb with aes-key per manufacture and aes-iv = random
    others: invalid
   @endverbatim
 * @see None.
 */
int HAL_Awss_Get_Conn_Encrypt_Type();


#ifdef __cplusplus
}
#endif

#endif  /* __IOT_IMPORT_AWSS_H__ */

