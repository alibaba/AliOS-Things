/**
 * @file    wm_wifi.h
 *
 * @brief   Wi-Fi API
 *
 * @author  WinnerMicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef TLS_WIFI_FUNC_H
#define TLS_WIFI_FUNC_H

#include "wm_type_def.h"

/* Return Error definition*/
/** invalid SSID */
#define	WM_WIFI_ERR_SSID		   -1
/** invalid key */
#define	WM_WIFI_ERR_KEY			   -2
/** wps is busing */
#define WM_WIFI_WPS_BUSY		   -3
/** scan is busing */
#define WM_WIFI_SCANNING_BUSY	   -4
/** station is connecting */
#define WM_WIFI_STA_BUSY	       -5

/* error number definition */
/** no error */
#define     WM_WIFI_ENOERR          0
/** ap not exist */
#define     WM_WIFI_ENOAP           1
/** pre-shared key may be incorrect */
#define     WM_WIFI_EKEY            2
/** Low signal intensity */
#define     WM_WIFI_ELSI            3


#ifndef ETH_ALEN
/** mac address length */
#define ETH_ALEN 6
#endif

/* bss type definition*/
#ifndef IEEE80211_MODE_INFRA
/** station mode */
#define IEEE80211_MODE_INFRA	1
/** ibss mode */
#define IEEE80211_MODE_IBSS		2
/** softap mode */
#define IEEE80211_MODE_AP		4
#endif

/** authenticate mode : open */
#define IEEE80211_ENCRYT_NONE			0
/** authenticate mode : WEP40 */
#define IEEE80211_ENCRYT_WEP40			1
/** authenticate mode : WEP104 */
#define	IEEE80211_ENCRYT_WEP104			2
/** authenticate mode : WPA_PSK_TKIP */
#define	IEEE80211_ENCRYT_TKIP_WPA		3
/** authenticate mode : WPA_PSK_CCMP */
#define	IEEE80211_ENCRYT_CCMP_WPA		4
/** authenticate mode : WPA2_PSK_TKIP */
#define	IEEE80211_ENCRYT_TKIP_WPA2		5
/** authenticate mode : WPA2_PSK_CCMP */
#define	IEEE80211_ENCRYT_CCMP_WPA2		6
/** authenticate mode : WPA_PSK_TKIP&AES */
#define	IEEE80211_ENCRYT_AUTO_WPA		7
/** authenticate mode : WPA2_PSK_TKIP&AES */
#define	IEEE80211_ENCRYT_AUTO_WPA2		8

#ifdef TLS_CONFIG_WPS
/** length of WPS pin code */
#define WPS_PIN_LEN 	                8
#endif

/** set auto connecting flag */
#define WIFI_AUTO_CNT_FLAG_SET		    1
/** get auto connecting flag */
#define WIFI_AUTO_CNT_FLAG_GET		    0

/** disable Wi-Fi auto connecting */
#define WIFI_AUTO_CNT_OFF			    0x0
/** enable Wi-Fi auto connecting */
#define WIFI_AUTO_CNT_ON			    0x1
/** disable Wi-Fi auto connecting temporary */
#define WIFI_AUTO_CNT_TMP_OFF		    0x3

/** Wi-Fi join net successfully */
#define WIFI_JOIN_SUCCESS               0x1
/** Wi-Fi join net failed */
#define WIFI_JOIN_FAILED                0x2
/** Wi-Fi disconnected */
#define WIFI_DISCONNECTED               0x3
/** Wi-Fi create ap successfully */
#define WIFI_SOFTAP_SUCCESS     0x4
/** Wi-Fi create softap failure */
#define WIFI_SOFTAP_FAILED      0x5
/** Wi-Fi close softap */
#define     WIFI_SOFTAP_CLOSED              0x6


/** Wi-Fi states */
enum tls_wifi_states {
	WM_WIFI_DISCONNECTED,      /**< Disconnected state */
	WM_WIFI_SCANNING,          /**< Scanning for a network */
	WM_WIFI_JOINING,           /**< Trying to join with a BSS/SSID */
	WM_WIFI_JOINED             /**< All authentication completed */
};

/** frame type of the manager */
enum tls_wifi_mgmt_type {
    WM_WIFI_MGMT_TYPE_ASSOC_REQ	   = 0x0000,    /**< association request frame */
    WM_WIFI_MGMT_TYPE_ASSOC_RESP   = 0x0010,    /**< association response frame */
    WM_WIFI_MGMT_TYPE_REASSOC_REQ  = 0x0020,    /**< reassociation request frame */
    WM_WIFI_MGMT_TYPE_REASSOC_RESP = 0x0030,    /**< reassociation response frame */
    WM_WIFI_MGMT_TYPE_PROBE_REQ    = 0x0040,    /**< probe request frame */
    WM_WIFI_MGMT_TYPE_PROBE_RESP   = 0x0050,    /**< probe response frame */
    WM_WIFI_MGMT_TYPE_BEACON       = 0x0080,    /**< beacon frame */
    WM_WIFI_MGMT_TYPE_ATIM         = 0x0090,    /**< ATIM frame */
    WM_WIFI_MGMT_TYPE_DISASSOC     = 0x00A0,    /**< disassociation frame */
    WM_WIFI_MGMT_TYPE_AUTH         = 0x00B0,    /**< authentication frame */
    WM_WIFI_MGMT_TYPE_DEAUTH       = 0x00C0,    /**< deauthentication frame */
    WM_WIFI_MGMT_TYPE_ACTION       = 0x00D0     /**< action frame */
};

/** Wi-Fi ransport rate */
enum tls_wifi_tx_rate {
    WM_WIFI_TX_RATEIDX_1M = 0,    /**< 11b, 1M */
    WM_WIFI_TX_RATEIDX_2M,        /**< 11b, 2M */
    WM_WIFI_TX_RATEIDX_5_5M,      /**< 11b, 5.5M */
    WM_WIFI_TX_RATEIDX_11M,       /**< 11b, 11M */
    WM_WIFI_TX_RATEIDX_6M,        /**< 11g, 6M */
    WM_WIFI_TX_RATEIDX_9M,        /**< 11g, 9M */
    WM_WIFI_TX_RATEIDX_12M,       /**< 11g, 12M */
    WM_WIFI_TX_RATEIDX_18M,       /**< 11g, 18M */
    WM_WIFI_TX_RATEIDX_24M,       /**< 11g, 24M */
    WM_WIFI_TX_RATEIDX_36M,       /**< 11g, 36M */
    WM_WIFI_TX_RATEIDX_48M,       /**< 11g, 48M */
    WM_WIFI_TX_RATEIDX_54M,       /**< 11g, 54M */
    WM_WIFI_TX_RATEIDX_MCS0,      /**< 11n-20MHz, mcs0 */
    WM_WIFI_TX_RATEIDX_MCS1,      /**< 11n-20MHz, mcs1 */
    WM_WIFI_TX_RATEIDX_MCS2,      /**< 11n-20MHz, mcs2 */
    WM_WIFI_TX_RATEIDX_MCS3,      /**< 11n-20MHz, mcs3 */
    WM_WIFI_TX_RATEIDX_MCS4,      /**< 11n-20MHz, mcs4 */
    WM_WIFI_TX_RATEIDX_MCS5,      /**< 11n-20MHz, mcs5 */
    WM_WIFI_TX_RATEIDX_MCS6,      /**< 11n-20MHz, mcs6 */
    WM_WIFI_TX_RATEIDX_MCS7,      /**< 11n-20MHz, mcs7 */
    WM_WIFI_TX_RATEIDX_MCS8,      /**< 11n-40MHz, mcs8 */
    WM_WIFI_TX_RATEIDX_MCS9,      /**< 11n-40MHz, mcs9 */
    WM_WIFI_TX_RATEIDX_MCS10,     /**< 11n-40MHz, mcs10 */
    WM_WIFI_TX_RATEIDX_MCS11,     /**< 11n-40MHz, mcs11 */
    WM_WIFI_TX_RATEIDX_MCS12,     /**< 11n-40MHz, mcs12 */
    WM_WIFI_TX_RATEIDX_MCS13,     /**< 11n-40MHz, mcs13 */
    WM_WIFI_TX_RATEIDX_MCS14,     /**< 11n-40MHz, mcs14 */
    WM_WIFI_TX_RATEIDX_MCS15,     /**< 11n-40MHz, mcs15 */
    WM_WIFI_TX_RATEIDX_MCS32      /**< invalid rate */
};

enum tls_wifi_oneshot_result_type {
    WM_WIFI_ONESHOT_TYPE_SSIDPWD,
    WM_WIFI_ONESHOT_TYPE_CUSTOMDATA
};

enum tls_wifi_client_event_type {
    WM_WIFI_CLIENT_EVENT_ONLINE,
    WM_WIFI_CLIENT_EVENT_OFFLINE
};

enum tls_wifi_op_mode{
    STATION_MODE = 1,
    SOFTAP_MODE,        
    STATIONAP_MODE
};

/** current bss information */
struct tls_curr_bss_t{
	u8 bssid[ETH_ALEN];    /**< BSSID of connected AP */
	u8 ssid[32];           /**< SSID of connected AP */
	u8 ssid_len;           /**< SSID length of connected AP */
	u8 channel;            /**< channel of connected AP */
	u8 type;               /**< BSS's type of connected AP, value is:
	                            IEEE80211_MODE_INFRA, IEEE80211_MODE_IBSS,
	                            IEEE80211_MODE_AP,    IEEE80211_MODE_APSTA */
	u8 encryptype;         /**< BSS's encryption type of connected AP, value is: IEEE80211_ENCRYT_NONE,
                                IEEE80211_ENCRYT_WEP40,     IEEE80211_ENCRYT_WEP104,
                                IEEE80211_ENCRYT_TKIP_WPA,  IEEE80211_ENCRYT_CCMP_WPA,
                                IEEE80211_ENCRYT_TKIP_WPA2, IEEE80211_ENCRYT_CCMP_WPA2,
                                IEEE80211_ENCRYT_AUTO_WPA,  IEEE80211_ENCRYT_AUTO_WPA2 */
	u8 rssi;               /**< single strength of AP */
};

/** secret key information */
struct tls_key_info_t{
	u8 format;     /**< key format, value is: 0-hex, 1-ascii */
	u8 index;      /**< key index, value is: 1-4 (only wep) */
	u8 key_len;    /**< key length */
	u8 key[64];    /**< key content */
};

/** Wi-Fi configuration of softap */
struct tls_softap_info_t{
	u8 ssid[33];    /**< SSID of softap */
	u8 encrypt;     /**< encryption mode of softap, value is: IEEE80211_ENCRYT_NONE,
	                     IEEE80211_ENCRYT_WEP40,     IEEE80211_ENCRYT_WEP104,
	                     IEEE80211_ENCRYT_TKIP_WPA,  IEEE80211_ENCRYT_CCMP_WPA,
	                     IEEE80211_ENCRYT_TKIP_WPA2, IEEE80211_ENCRYT_CCMP_WPA2,
	                     IEEE80211_ENCRYT_AUTO_WPA,  IEEE80211_ENCRYT_AUTO_WPA2 */
	u8 channel;     /**< channel of softap */
	struct tls_key_info_t keyinfo;  /**< Password (key) of softap */
};

/** ip address information */
struct tls_ip_info_t{
    u8 ip_addr[4];     /**< IP address */
    u8 netmask[4];     /**< netmask */
    u8 dnsname[32];    /**< DNS server name */
};

/** Wi-Fi configuration of ibss */
struct tls_ibss_info_t{
	u8 ssid[33];    /**< SSID of ibss */
	u8 encrypt;     /**< encryption mode of ibss, value is: IEEE80211_ENCRYT_NONE,
	                     IEEE80211_ENCRYT_WEP40,     IEEE80211_ENCRYT_WEP104,
	                     IEEE80211_ENCRYT_TKIP_WPA,  IEEE80211_ENCRYT_CCMP_WPA,
	                     IEEE80211_ENCRYT_TKIP_WPA2, IEEE80211_ENCRYT_CCMP_WPA2,
	                     IEEE80211_ENCRYT_AUTO_WPA,  IEEE80211_ENCRYT_AUTO_WPA2 */
	u8 channel;     /**< channel of ibss */
	struct tls_key_info_t keyinfo;    /**< Password (key) of ibss */
};

/** ip address information of ibss */
struct tls_ibssip_info_t{
	u8 ip[4];         /**< IP address */
	u8 netmask[4];    /**< netmask */
	u8 gateway[4];    /**< gateway */
	u8 dns1[4];       /**< DNS1 IP address */
	u8 dns2[4];       /**< DNS2 IP address */
};

/** bss information */
struct tls_bss_info_t {
    u8 bssid[ETH_ALEN];    /**< MAC address of AP */
    u8 mode;               /**< AP type, value is: 1-ibss, 2-ess */
    u8 channel;            /**< channel of AP */
    u8 privacy;            /**< whether is encrypted */
    u8 ssid_len;           /**< SSID length */
    u8 rssi;               /**< single strength of AP */
    u8 ssid[32];           /**< SSID of AP */
};

/** scan result */
struct tls_scan_bss_t {
    u32     count;                   /**< total count */
    u32     length;                  /**< bss info total length */
    struct tls_bss_info_t bss[1];    /**< list of bss found*/
};

/** station information */
struct tls_sta_info_t {
    u8  mac_addr[ETH_ALEN];    /**< MAC address of station */
};

/** 802.11 packet information from the physical layer */
struct tls_wifi_ext_t {
    u8 rssi;    /**< single strength */
};

/** 802.11 mac address */
struct tls_wifi_hdr_mac_t {
    u8  da_addr[ETH_ALEN];    /**< MAC address of destination */
    u8  sa_addr[ETH_ALEN];    /**< MAC address of source */
    u8  bssid[ETH_ALEN];      /**< MAC address of AP */
};

/** transport rate and gain */
struct tls_wifi_tx_rate_t {
    enum tls_wifi_tx_rate tx_rate;    /**< Wi-Fi ransport rate */
    u8 tx_gain;                       /**< Wi-Fi ransport gain */
};

/** callback function of receive Wi-Fi data */
typedef void (*tls_wifi_data_recv_callback)(u8* data, u32 data_len);

/** callback function of receive ETHERNET data */
typedef int (*net_rx_data_cb)(const u8 *bssid, u8 *buf, u32 buf_len);

/** callback function of receive Wi-Fi data with some information of the physical layer */
typedef void (*tls_wifi_data_ext_recv_callback)(u8* data, u32 data_len, struct tls_wifi_ext_t *ext);

/**
 * @defgroup Wi-Fi_APIs Wi-Fi APIs
 * @brief Wi-Fi related APIs
 */

/**
 * @addtogroup Wi-Fi_APIs
 * @{
 */

/**
 * @defgroup WM_Wi-Fi_APIs WM_Wi-Fi APIs
 * @brief WM_Wi-Fi APIs
 */

/**
 * @addtogroup WM_Wi-Fi_APIs
 * @{
 */

/**
 * @brief          This function is used to enable/disable listen mode
 *
 * @param[in]      enable:non-zero, disable: 0
 *
 * @return         None
 *
 * @note           This function used when oneshot start.
 */
void tls_wifi_set_listen_mode(u8 enable);

/**
 * @brief          This function is used to get listen mode
 *
 * @param          None
 *
 * @retval         0        normal mode
 * @retval         non-zero listen mode
 *
 * @note           None
 */
u8 tls_wifi_get_listen_mode(void);

/**
 * @brief          This function is used to enable/disable special listen mode
 *
 * @param[in]      enable:non-zero, disable: 0
 *
 * @return         None
 *
 * @note           This function used when special oneshot start.
 */
void tls_wifi_set_special_mode(u8 enable);

/**
 * @brief          This function is used to get special listen mode
 *
 * @param          None
 *
 * @retval         0        		normal mode
 * @retval         non-zero 	special listen mode
 *
 * @note           None
 */
u8 tls_wifi_get_special_mode(void);


/**
* @brief          This function is used to filter multicast frames
* @param mac:     The multicast frame that we want receive or filter.
*
* @param receive: 1, receive this multicast frame
*                 0, filter this multicast frame
* 
* @note usage:    For example: u8 mac[6]={01, 00, 5e, 7f, ff, fa},if receive
*                 is set to 0,the 802.11 multicast frames whose hdr->addr1 is
*                 [01 00 5e 7f ff fa] will be filtered.
*/
u8 tls_filter_mcast_mac(u8 *mac, u8 receive);

/**
 * @brief         This function is used to set mac filter.
 * @param[in] mac:     mac: to be filtered or received
 *
 * @param[in] receive: 1:not filter,zero:filter
 *
 * @param[in] clear:   1:clear all, 0:do not clear, only add new filter
 *
 * @return 		None 
 * 
 * @note usage:    normally, it is used to oneshot config
 */ 
void tls_wifi_set_bcast_mac_filter(u8 *mac, u8 receive, u8 clear);

/**
 * @brief		   This function is used to restore mac filter to normal mode.
 *
 * @param[in]	   None
 *
 * @return		   None
 *
 * @note		   normally, it is used to restore mac filter after oneshot config
 */
void tls_wifi_restore_bcast_mac_filter(void);


/**
 * @brief          This function is used to register recv wifi data callback function
 *
 * @param[in]      callback    point to receive Wi-Fi data function
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_data_recv_cb_register(tls_wifi_data_recv_callback callback);

/**
 * @brief          This function is used to register recv wifi extended
 *                 data callback function
 *
 * @param[in]      callback   point to receive Wi-Fi extended data function
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_data_ext_recv_cb_register(tls_wifi_data_ext_recv_callback callback);



/**
 * @brief          This function is used to set oneshot config flag
 *
 * @param[in]      flag 0: closed oneshot non-zero:open oneshot
 *
 * @return         None
 *
 * @note           None
 */
void   tls_wifi_set_oneshot_flag(u8 flag);

/**
 * @brief          This function is used to get one shot flag
 *
 * @param          None
 *
 * @retval         0     oneshot closed
 * @retval         other oneshot open
 *
 * @note           None
 */
int     tls_wifi_get_oneshot_flag(void);


/*********************************************************************************************************
* Description: 	before calling tls_wifi_get_oneshot_ssidpwd or tls_wifi_get_oneshot_customdata, 
*                 application should call this function to register the call back function;
* 
* Arguments: 		callback function pointer:
* 
* Return: 		None 
* Note:    		callback function, suggest user send out a MSG and return immediately.
*********************************************************************************************************/
typedef void (*tls_wifi_oneshot_result_callback)(enum tls_wifi_oneshot_result_type type);

void tls_wifi_oneshot_result_cb_register(tls_wifi_oneshot_result_callback callback);

/**********************************************************************************************************
* Description: 	This function is used to get one shot ssid and password information.
*
* Arguments  : 	ssid,  wifi name
*                 pwd,   pwssword
*
* Returns    : 	None
**********************************************************************************************************/
void tls_wifi_get_oneshot_ssidpwd(u8 *ssid, u8 *pwd);

/**********************************************************************************************************
* Description: 	This function is used to get one shot custom data
*
* Arguments  : 	data,  custom data
*
* Returns    : 	None
**********************************************************************************************************/
void tls_wifi_get_oneshot_customdata(u8 *data);

/**
 * @brief          This function is used to change channel actively
 *
 * @param[in]      chanid
 *
 * @return         None
 *
 * @note           Normally, it is just used in listen mode;
 */
void tls_wifi_change_chanel(u32 chanid);

/**
 * @brief          This function is used to trigger scan AP
 *
 * @param          None
 *
 * @retval         WM_SUCCESS     				start scan
 * @retval         WM_WIFI_SCANNING_BUSY     	scanning
 * @retval         WM_FAILED					failed
 *
 * @note           If not SUCCESS, user needs to call this function again
 *                 to trigger scan
 */
int tls_wifi_scan(void);

/**
 * @brief          before calling tls_wifi_scan() , application should call
 *                 this function to register the call back function;
 *
 * @param[in]      callback  point to callback function
 *
 * @return         None
 *
 * @note           In callback function, user should send a message
 *                 and return immediately.
 *                 After callback called, scan result can be get by
 *                 calling function tls_wifi_get_scan_rslt
 */
void tls_wifi_scan_result_cb_register(void (*callback)(void));

/**
 * @brief          get scan result
 *
 * @param[out]     buf          buf to be used to store returned BSS info
 * @param[in]      buffer_size  buf size
 *
 * @retval         WM_SUCCESS     success
 * @retval         WM_FAILED	  failed
 *
 * @note           user need to alloc buffer in advance.
 *                 size for one item of scan result is 43Bytes;
 *				   the buffer size depends how many items user wants.
 */
int tls_wifi_get_scan_rslt(u8* buf, u32 buffer_size);


/**
 * @brief          This function is used to create soft ap
 *
 * @param[in]      apinfo   softap Wi-Fi configuration
 * @param[in]      ipinfo   softap ip address
 *
 * @retval         WM_WIFI_ERR_SSID     SSID is NULL
 * @retval         WM_WIFI_ERR_KEY      key info not correct
 * @retval         WM_SUCCESS       	soft ap create OK
 *
 * @note           None
 */
int tls_wifi_softap_create(struct tls_softap_info_t* apinfo, struct tls_ip_info_t* ipinfo);


/**
 * @brief          This function is used to destroy soft ap
 *
 * @param          None
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_softap_destroy(void);

/**
 * @brief          This function is used to get soft ap's state
 *
 * @param          None
 *
 * @return         soft ap's state
 *
 * @note           None
 */
enum tls_wifi_states tls_wifi_softap_get_state(void);

typedef void (*tls_wifi_client_event_callback)(u8 *mac, enum tls_wifi_client_event_type event);

/**
 * @brief          This function is used to register client event
 *
 * @param[in]    client event callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_softap_client_event_register(tls_wifi_client_event_callback callback);

/**
 * @brief		   This function is used to get the authed sta list
 *
 * @param[out]	 sta_num the authed's station number
 *
 * @param[out]	 buf address to store returned station list info(struct tls_sta_info_t[])
 *
 * @param[in]	 buf_size
 *
 * @return		   None
 *
 * @note		   None
 */
void tls_wifi_get_authed_sta_info(u32 *sta_num, u8 *buf, u32 buf_size);

/**
 * @brief          This function is used to create or join ibss
 *
 * @param[in]      *ibssinfo    ibss Wi-Fi configuration
 * @param[in]      *ipinfo      ibss ip address
 *
 * @retval         WM_SUCCESS     	IBSS join or create ok
 * @retval         WM_WIFI_ERR_SSID SSID is NULL
 * @retval         WM_WIFI_ERR_KEY  key info not correct
 *
 * @note           Join IBSS if existed
 *                 Create IBSS by SSID when not existed.
 */
int tls_wifi_ibss_create(struct tls_ibss_info_t *ibssinfo, struct tls_ibssip_info_t *ipinfo);

/**
 * @brief          This function is used to destroy or leave Wi-Fi network
 *
 * @param          None
 *
 * @return         None
 *
 * @note           For AP,   destroy soft AP
 *                 For STA,  leave the network by AP
 *                 For IBSS, destroy or leave the IBSS network.
 */
void tls_wifi_disconnect(void);

/**
 * @brief          This function is used to connect AP
 *
 * @param[in]      *ssid       Network  Name to connect
 * @param[in]      ssid_len    length of SSID
 * @param[in]      *pwd        password to connect AP
 * @param[in]      pwd_len     length of password
 *
 * @retval         WM_SUCCESS    config ok, wifi will start to connect AP;
 * @retval         WM_FAILED     parameter wrong
 *
 * @note           User should register Wi-Fi status callback function
 *                 to get result;
 *				   wifi_status_change_cb just return WIFI MAC layer status;
 *				   User should register netif status callback
 *                 to get TCP/IP layer status;
 */
int tls_wifi_connect(u8 *ssid, u8 ssid_len, u8 *pwd, u8 pwd_len);

/**
 * @brief          This function is used to connect AP by BSSID
 *
 * @param[in]      *bssid    BSSID to connect
 * @param[in]      *pwd      password to connect AP
 * @param[in]      pwd_len   length of password
 *
 * @retval         WM_SUCCESS     success
 * @retval         WM_FAILED      failed
 *
 * @note           When SSID hided,this function can not be used.
 * 				   User should register Wi-Fi status callback function
 *                 to get result;
 *				   wifi_status_change_cb just return WIFI MAC layer status;
 *				   User should register netif status callback
 *                 to get TCP/IP layer status;
 */
int tls_wifi_connect_by_bssid(u8 *bssid, u8 *pwd, u8 pwd_len);

/**
 * @brief          This function is used to connect AP by SSID an BSSID
 *
 * @param[in]      *ssid      Network  Name to connect
 * @param[in]      ssid_len   length of SSID
 * @param[in]      *bssid     BSSID to connect with SSID
 * @param[in]      *pwd       password to connect AP
 * @param[in]      pwd_len    length of password
 *
 * @retval         WM_SUCCESS     success
 * @retval         WM_FAILED      failed
 *
 * @note           When SSID&BSSID is knonw, user can use this function
 *                 to connect Wi-Fi AP.
 * 				   User should register Wi-Fi status callback function
 *                 to get result;
 *				   wifi_status_change_cb just return WIFI MAC layer status;
 *				   User should register netif status callback
 *                 to get TCP/IP layer status;
 */
int tls_wifi_connect_by_ssid_bssid(u8 *ssid, u8 ssid_len, u8 *bssid, u8 *pwd, u8 pwd_len );


/**
 * @brief          Set auto connect mode: Enable/Disable.
 *                 Get auto connect mode parameter
 *
 * @param[in]      opt     WIFI_AUTO_CNT_FLAG_SET or WIFI_AUTO_CNT_FLAG_GET
 * @param[in out]  mode    WIFI_AUTO_CNT_OFF WIFI_AUTO_CNT_ON
 *                         or WIFI_AUTO_CNT_TMP_OFF
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note
 * 				   WIFI_AUTO_CNT_OFF		 Disable/d;
 * 				   WIFI_AUTO_CNT_ON 		 Enable/d;
 *                 WIFI_AUTO_CNT_TMP_OFF	 For user initiated "DISCONNECT",
 *                 such as AT CMD; In such case, user might expect
 *                 "disconnect witout reconnection, even in WIFI_AUTO_CNT_ON
 *                 status; WIFI_AUTO_CNT_TMP_OFF flag just be effective
 *                 for one time of "DISCONNECT";
 *                 After that or reboot, the FLAG will be set to the previous value;
 */
int tls_wifi_auto_connect_flag(u8 opt, u8* mode);

/**
 * @brief          This function is used to register wifi status changed callback function
 *
 * @param[in]      callback   point to wifi status function
 *
 * @return         None
 *
 * @note
 *			 WIFI_JOIN_SUCCESS connect with wifi AP correctly in Wifi layer;
 * 			 No IP address
 *			 WIFI_JOIN_FAILED  did not connect with wifi AP;
 *                             normally,  timeout in 20s after start connection
 *			 WIFI_DISCONNECTED STA is disconnected with AP for any case,
 *                             such as wifi AP shut dow, Wi-Fi AP
 *                             changed password, and so on;
 */
void tls_wifi_status_change_cb_register(void (*callback)(u8 status));

/**
 * @brief          This function is used to register Wi-Fi status changed callback function
 *
 * @param[out]     bss    address where the network parameters will be write;
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_get_current_bss(struct tls_curr_bss_t* bss);

/*********************************************************************************************************
					Wifi WPS API
*********************************************************************************************************/
#ifdef TLS_CONFIG_WPS

/**
 * @brief          This function generate random PIN code
 *
 * @param[out]     pin  buf to store pin code, WPS_PIN_LEN Bytes
 *
 * @retval         WM_SUCCESS   success
 * @retval         other 		failed
 *
 * @note           None
 */
int tls_wps_get_pin(u8* pin);

/**
 * @brief          set PIN code into system
 *
 * @param[in]      pin     8bytes string
 * @param[in]      pin_len length of pin, should be 8 by current WPS spec
 *
 * @retval         WM_SUCCESS   success
 * @retval         WM_FAILED    failed
 *
 * @note           normally, the @pin code will be hard coded during manufacturing. should not use this function;
 */
int tls_wps_set_pin(u8* pin, u8 pin_len);

/**
 * @brief          Start WPS process via PIN mode
 *
 * @param          None
 *
 * @retval         WM_SUCCESS        success
 * @retval         WM_FAILED	     failed
 * @retval         WM_WIFI_WPS_BUSY	 last WPS process is not finished;
 *
 * @note           Normally, 120s for WPS protocol, but for us, 180s totally;
 *                 Adapter will use the PIN code in system for WPS process
 *                 and before that, the PIN code should be input
 *                 into AP/Registrar mannually
 */
int tls_wps_start_pin(void);

/**
 * @brief          Start WPS process via PBC/PushButton mode
 *
 * @param          None
 *
 * @retval         WM_SUCCESS       success
 * @retval         WM_FAILED 	    failed
 * @retval         WM_WIFI_WPS_BUSY last WPS process is not finished;
 *
 * @note           Normally, 120s for WPS protocol, but for us, 180s totally;
 *                 if OK, adaptor will start the WPS connection;
 *                 APP should register Wi-Fi/netif status callback to get result
 */
int tls_wps_start_pbc(void);
#endif

/**
 * @brief          This function is used to register ETHERNET data rx callback function
 *
 * @param[in]      callback    point to receive ETHERNET data function
 *
 * @return         None
 *
 * @note           None
 */
void tls_ethernet_data_rx_callback(net_rx_data_cb callback);
#if TLS_CONFIG_AP_OPT_FWD
/**
 * @brief          This function is used to register IP data rx
 *                 callback function in APSTA mode
 *
 * @param[in]      callback   point to receive ETHERNET data function
 *
 * @return         None
 *
 * @note           Only in APSTA mode, this function to deal with IP data
 */
void tls_ethernet_ip_rx_callback(net_rx_data_cb callback);
#endif

/**
 * @brief          set powersave flag
 *
 * @param[in]      enable     non-zero: enable  0: disable
 * @param[in]      alwaysflag 0: only once, lost when restart;  !0: always
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_set_psflag(bool enable, bool alwaysflag);

/**
 * @brief          get current powersave flag
 *
 * @param          None
 *
 * @retval         0        power-saving disable
 * @retval         non-zero power-saving enable
 *
 * @note           None
 */
u32 tls_wifi_get_psflag(void);

/**
 * @brief          This function is used to send oneshot data to other sta
 *
 * @param[in]      *dst      NULL , send broadcast info
 *                           !NULL, send unicast   info
 * @param[in]      *ssid     SSID to be sent
 * @param[in]      ssid_len  SSID length to be sent
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_send_oneshotdata(u8 *dst, const u8 *ssid, u8 ssid_len);

/**
 * @brief          This function is used to get max gain by rate index
 *
 * @param[in]      tx_rate    rate index (enum)
 *
 * @retval         Max Gain
 *
 * @note           None
 */
u8 tls_wifi_get_tx_gain_max(enum tls_wifi_tx_rate tx_rate);

/**
 * @brief          This function is used to send 802.11 management packet
 *
 * @param[in]      type      management packet type
 * @param[in]      *mac      mac address
 * @param[in]      *ie       information elements, if have no ie can fill NULL
 * @param[in]      ie_len    information elements length, it's zero if ie is NULL.
 * @param[in]      *tx       rate and gain to transfer
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_wifi_send_mgmt(enum tls_wifi_mgmt_type type, struct tls_wifi_hdr_mac_t *mac, u8 *ie, u16 ie_len, struct tls_wifi_tx_rate_t *tx);

/**
 * @brief          This function is used to send 802.11 data packet
 *
 * @param[in]      *mac       mac address
 * @param[in]      *data      data packet buffer
 * @param[in]      data_len   data packet length
 * @param[in]      *tx        rate and gain
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_wifi_send_data(struct tls_wifi_hdr_mac_t *mac, u8 *data, u16 data_len, struct tls_wifi_tx_rate_t *tx);

#if TLS_CONFIG_AP
/**
 * @brief          This function is used to get authed sta list
 *
 * @param[out]     *sta_num    authed's station number
 * @param[out]     *buf        address to store returned station list info, tls_sta_info_t
 * @param[in]      buf_size    buffer size
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_get_authed_sta_info(u32 *sta_num, u8 *buf, u32 buf_size);
#endif
/**
 * @brief          This function is used to get current Wi-Fi State
 *
 * @param          None
 *
 * @retval         Wi-Fi State(enum)
 *
 * @note           None
 */
enum tls_wifi_states tls_wifi_get_state(void);

/**
 * @brief          This function is used to get current error number during joining AP
 *
 * @param          None
 *
 * @retval         Error Number(WM_WIFI_ENOERR,WM_WIFI_ENOAP,
 *								WM_WIFI_EKEY,WM_WIFI_ELSI)
 *
 * @note           None
 */
int tls_wifi_get_errno(void);

/**
 * @brief          This Function is used to print current error during joining AP
 *
 * @param[in]      *info   prefix information
 *
 * @return         None
 *
 * @note           None
 */
void tls_wifi_perror(const char *info);

/**
 * @brief          This function is used to get wifi error
 *
 * @param[in]      eno  error number(WM_WIFI_ENOERR,WM_WIFI_ENOAP,
 *                                   WM_WIFI_EKEY,WM_WIFI_ELSI)
 *
 * @retval         error description
 *
 * @note           None
 */
const char *tls_wifi_get_errinfo(int eno);


/**
 * @}
 */
void tls_wifi_set_opmode(u8 mode);

/**
 * @}
 */
u8 tls_wifi_get_opmode(void);
/**
 * @brief          This function is used to customize wifi tx&rx memory
 *
 * @param[in]      startmem: memory addr, only used from 0x20028000 to 0x20048000
 *                       
 * @param[in]      txcnt: wifi tx buf cnt, non -zero value
 *                       
 * @param[in]      rxcnt: wifi rx buf cnt, greater than 2
 *                       
 * @retval         0  :successfullly
 *                    <0:failure
 *
 * @note           None
 */
int tls_wifi_mem_cfg(u32 startmem, u8 txcnt, u8 rxcnt);

/**
 * @}
 */
int tls_wl_get_isr_count(void);


#endif /* TLS_WIFI_FUNC_H */

