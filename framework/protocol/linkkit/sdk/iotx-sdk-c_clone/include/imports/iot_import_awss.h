/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOT_IMPORT_AWSS_H__
#define __IOT_IMPORT_AWSS_H__

#ifdef __cplusplus
extern "C" {
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
#define WLAN_CONNECTION_TIMEOUT_MS      (30 * 1000)

/**
 * @brief   获取配网服务(`AWSS`)的超时时间长度, 单位是毫秒
 *
 * @return  超时时长, 单位是毫秒
 * @note    推荐时长是60,0000毫秒
 */
DLL_HAL_API int HAL_Awss_Get_Timeout_Interval_Ms(void);

/**
 * @brief   获取在每个信道(`channel`)上扫描的时间长度, 单位是毫秒
 *
 * @return  时间长度, 单位是毫秒
 * @note    推荐时长是200毫秒到400毫秒
 */
DLL_HAL_API int HAL_Awss_Get_Channelscan_Interval_Ms(void);

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
DLL_HAL_API void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb);

/**
 * @brief   设置Wi-Fi网卡离开监听(Monitor)模式, 并开始以站点(Station)模式工作
 */
DLL_HAL_API void HAL_Awss_Close_Monitor(void);

/**
  * @brief   开启设备热点（SoftAP模式）
  *
  * @param[in] ssid @n 热点的ssid字符；
  * @param[in] passwd @n 热点的passwd字符；
  * @param[in] beacon_interval @n 热点的Beacon广播周期（广播间隔）；
  * @param[in] hide @n 是否是隐藏热点，hide:0, 非隐藏, 其它值：隐藏；
  * @return，
 @verbatim
    = 0: success
    = -1: unsupported
    = -2: failure with system error
    = -3: failure with no memory
    = -4: failure with invalid parameters
 @endverbatim
  * @Note:
  *       1）ssid和passwd都是以'\0'结尾的字符串，如果passwd字符串的
  *          长度为0，表示该热点采用Open模式（不加密）；
  *       2）beacon_interval表示热点的Beacon广播间隔（或周期），单
  *          位为毫秒，一般会采用默认100ms；
  *       3）hide表示创建的热点是否是隐藏热点，hide=0表示非隐藏热
  *         点，其他值表示隐藏热点；
  */

DLL_HAL_API int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval, int hide);

/**
  * @brief   关闭当前设备热点，并把设备由SoftAP模式切换到Station模式
  *
  * @return，
 @verbatim
    = 0: success
    = -1: unsupported
    = -2: failure
 @endverbatim
  * @Note:
  *       1）如果当前设备已经开启热点，关闭当前热点，如果当前设备正
  *          在开热点，取消开热点的操作；
  *       2）如果当前设备不是以Station模式（包括Station+SoftAP模式和
  *          SoftAP模式）工作，设备必须切换到Station模式；
  *       3）Wi-Fi状态机需要切换到初始化状态，因为接下来很可能进行
  *          连接某一个路由器操作；
  */

DLL_HAL_API int HAL_Awss_Close_Ap();

/**
 * @brief   设置Wi-Fi网卡切换到指定的信道(channel)上
 *
 * @param[in] primary_channel @n Primary channel.
 * @param[in] secondary_channel @n Auxiliary channel if 40Mhz channel is supported, currently
 *              this param is always 0.
 * @param[in] bssid @n A pointer to wifi BSSID on which awss lock the channel, most HAL
 *              may ignore it.
 */
DLL_HAL_API void HAL_Awss_Switch_Channel(
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
DLL_HAL_API int HAL_Awss_Connect_Ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[HAL_MAX_SSID_LEN],
            _IN_ char passwd[HAL_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel);

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
DLL_HAL_API int HAL_Wifi_Send_80211_Raw_Frame(_IN_ enum HAL_Awss_Frame_Type type,
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
DLL_HAL_API int HAL_Wifi_Enable_Mgmt_Frame_Filter(
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
DLL_HAL_API int HAL_Wifi_Scan(awss_wifi_scan_result_cb_t cb);

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
DLL_HAL_API int HAL_Wifi_Get_Ap_Info(
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
DLL_HAL_API int HAL_Awss_Get_Encrypt_Type(void);

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
DLL_HAL_API int HAL_Awss_Get_Conn_Encrypt_Type(void);

#ifdef __cplusplus
}
#endif

#endif  /* __IOT_IMPORT_AWSS_H__ */

