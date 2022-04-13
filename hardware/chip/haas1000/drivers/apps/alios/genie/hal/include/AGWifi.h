/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGWifi.h
 * @brief
 * @author zhibin.yzb@alibaba-inc.com
 * @data 2018/03/09
 * @version 1.0
 */

#ifndef AG_WIFI_H
#define AG_WIFI_H

#define ALI_STUB_METHOD_MAX_LENGTH      (32)
#define ALI_STUB_STATUS_MAX_LENGTH      (32)
#define ALI_STUB_SSID_MAX_LENGTH        (32)
#define ALI_STUB_PASSWORD_MAX_LENGTH    (64)

enum {
    WIFI_DISCONNECTED = 1, // Connection start
    WIFI_SCAN_STARTED, // Scan start
    WIFI_SCAN_FAILED, // Scan failed
    WIFI_NETWORK_NOT_FOUND, // no AP found
    WIFI_AUTHENTICATION,//Authentication start
    WIFI_AUTH_REJECT, // Authentication rejected by AP
    WIFI_AUTH_TIMEOUT, // Authentication timeout with AP
    WIFI_ASSOCIATING, // Association starts
    WIFI_ASSOC_REJECT, // Association rejected by AP
    WIFI_ASSOCIATED, //Authentication succeed
    WIFI_4WAY_HANDSHAKE, //4way-handshark start
    WIFI_HANDSHAKE_FAILED, // 4way-handshake fails
    WIFI_4WAY_HANDSHAKE_DONE, //4way-handshark done
    WIFI_GROUP_HANDSHAKE, //group-handshark start
    WIFI_GROUP_HANDSHAKE_DONE, //group-handshark done = completed
    WIFI_CONNECTED, // Connection to AP done
    WIFI_CONN_TIMEOUT, // Connection timeout
    WIFI_DEAUTH, // Deauth received from AP
    DHCP_START_FAILED, // DHCP start fails
    DHCP_TIMEOUT, // DHCP timeout
    DHCP_SUCCESS // DHCP success
};

enum
{
    WLAN0_STATUS_DOWN,
    WLAN0_STATUS_UP,
    WLAN0_STATUS_MAX
};

typedef enum
{
    WIFI_MSG_ID_MIN = 0,
    WIFI_MSG_ID_WIFI_STATUS = WIFI_MSG_ID_MIN,
    WIFI_MSG_ID_WIFI_STATUS_FROM_IMPL,
    WIFI_MSG_ID_WIFI_TRACE_FROM_IMPL,
    WIFI_MSG_ID_NETWORK_STATUS,
    WIFI_MSG_ID_MAX
} WifiMsgId_E;

typedef struct
{
    int id;
    union
    {
        int   wlanStatus;
        void *networkStatusChange;
        void *wifiTrace;
    } data;
} WifiMsgData_T;

class AGWifiMsgListener
{
public:
    virtual ~AGWifiMsgListener() {}
    /**
     * @brief Receive wifi message callback
     * @param[in] pWifiMsgData
     * @return void
     */
    virtual void onRecvWifiMsg(WifiMsgData_T *pWifiMsgData) = 0;
};

class AGWifi
{
public:
    virtual ~AGWifi() {}
    /**
     * @brief Init AGWifi with listener
     * @param[in] pWifiMsgListener Register listener for wifi message
     * @return void
     */
    virtual void init(AGWifiMsgListener* pWifiMsgListener) = 0;
    /**
     * @brief Start AGWifi
     * @return void
     */
    virtual void start() = 0;
    /**
     * @brief Get mac for wifi
     * @param[out] mac Output mac address
     * @return int 0 : success; other: fail
     */
    virtual int getMac(char *mac)=0;
    /**
     * @brief Connect to specific wifi
     * @param[in] ssid
     * @param[in] password
     * @param[in] bssid
     * @param[in] time_ms Timeout value
     * @return int 0 : success; other: fail
     */
    virtual int connect(const char *ssid, const char *password,
    	const char *bssid, int time_ms)=0;
    /**
     * @brief
     * @param[in] result
     * @return void
     */

    virtual int disconnect() = 0;

    virtual void setupResult(int result)=0;
    /**
     * @brief Get Rssi for wifi
     * @param[out] rss Output rssi value
     * @return int 0 : success; other: fail
     */
    virtual int getRssi(int *rssi)=0;

	virtual int getChannelByBssid(const char *bssid,
		unsigned int *channel)=0;
    /**
     * @brief start_smartconfig to stop netmgr
     * @param[out]
     * @return void
     */
#if defined (_T_HEAD_) || defined (HAL_ABSTRACTION)
    virtual void start_smartconfig()=0;
    /**
     * @brief stop_smartconfig to start netmgr
     * @param[out]
     * @return void
     */
    virtual void stop_smartconfig()=0;
    /**
     * @brief wifi switch_channel
     * @param[out]
     * @return void
     */
    virtual void switch_channel(char primary_channel)=0;
#endif

#ifdef HAL_ABSTRACTION

    virtual int needWifiSetup() = 0;

    virtual void setWifiSetup(int val) = 0;

#endif

    /**
     * @brief switch next AP
     * @param[out]
     * @return int 0 : success; other: fail
     */
    virtual int switchNextAP(void) {return 0;}
};
#endif


