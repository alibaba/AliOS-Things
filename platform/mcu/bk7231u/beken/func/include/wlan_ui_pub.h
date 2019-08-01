#ifndef _WLAN_UI_PUB_
#define _WLAN_UI_PUB_

#pragma once

#include "include.h"
#include "rtos_pub.h"
#include "rw_msg_pub.h"

#if (CFG_SOC_NAME != SOC_BK7231)
#include "ble_pub.h"
#endif

#define ICU_BASE                                     (0x00802000)
#define ICU_INT_STATUS                               (ICU_BASE + 19 * 4)

enum {
    WLAN_ENC_OPEN,
    WLAN_ENC_WEP,
    WLAN_ENC_CCMP,
    WLAN_ENC_TKIP,
};

#define WiFi_Interface  wlanInterfaceTypedef

#define DHCP_DISABLE  (0)   /**< Disable DHCP service. */
#define DHCP_CLIENT   (1)   /**< Enable DHCP client which get IP address from DHCP server automatically,  
								reset Wi-Fi connection if failed. */
#define DHCP_SERVER   (2)   /**< Enable DHCP server, needs assign a static address as local address. */

typedef enum _wifi_dis_reason 
{
    UNSUPPORT_ENCRYPT = 0,  
    WRONG_PASSPHRASE,      
    DEAUTHENTICATION,     
    DISASSOCIATION,
    AP_MISSING,
    DISCONNECT,
} WIFI_DISCONN_REASON;

typedef struct _wifi_link_info
{
    uint8_t bssid[6];
    int16_t rssi;
    
    uint32_t data_rate;
    WIFI_DISCONN_REASON reason;
} WIFI_LINK_INFO_T;

/**
 *  @brief  wlan network interface enumeration definition.
 */
typedef enum
{
    BK_SOFT_AP,  /**< Act as an access point, and other station can connect, 4 stations Max*/
    BK_STATION   /**< Act as a station which can connect to an access point*/
} wlanInterfaceTypedef;

/**
 *  @brief  Wi-Fi security type enumeration definition.
 */
enum bk_wlan_sec_type_e
{
    BK_SECURITY_TYPE_NONE,        /**< Open system. */
    BK_SECURITY_TYPE_WEP,         /**< Wired Equivalent Privacy. WEP security. */
    BK_SECURITY_TYPE_WPA_TKIP,    /**< WPA /w TKIP */
    BK_SECURITY_TYPE_WPA_AES,     /**< WPA /w AES */
    BK_SECURITY_TYPE_WPA2_TKIP,   /**< WPA2 /w TKIP */
    BK_SECURITY_TYPE_WPA2_AES,    /**< WPA2 /w AES */
    BK_SECURITY_TYPE_WPA2_MIXED,  /**< WPA2 /w AES or TKIP */
    BK_SECURITY_TYPE_AUTO,        /**< It is used when calling @ref bkWlanStartAdv, _BK_ read security type from scan result. */
};

enum
{
    WLAN_RX_BEACON,    /* receive beacon packet */
    WLAN_RX_PROBE_REQ, /* receive probe request packet */
    WLAN_RX_PROBE_RES, /* receive probe response packet */
    WLAN_RX_ACTION,    /* receive action packet */
    WLAN_RX_MANAGEMENT,/* receive ALL management packet */
    WLAN_RX_DATA,      /* receive ALL data packet */
    WLAN_RX_MCAST_DATA,/* receive ALL multicast and broadcast packet */

    WLAN_RX_ALL,       /* receive ALL 802.11 packet */
};

typedef uint8_t wlan_sec_type_t;

/**
 *  @brief  wlan local IP information structure definition.
 */
typedef struct
{
	uint8_t dhcp;       /**< DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server.*/
	char    ip[16];     /**< Local IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    gate[16];   /**< Router IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    mask[16];   /**< Netmask on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    dns[16];    /**< DNS server IP address.*/
    char    mac[16];    /**< MAC address, example: "C89346112233".*/
    char    broadcastip[16];
} IPStatusTypedef;

/**
 *  @brief  Scan result using normal scan.
 */
typedef  struct  _ScanResult
{
    char ApNum;       /**< The number of access points found in scanning. */
    struct
    {
        char ssid[32];  /**< The SSID of an access point. */
        char ApPower;   /**< Signal strength, min:0, max:100. */
    } *ApList;
} ScanResult;

/**
 *	@brief	Scan result using advanced scan.
 */
typedef  struct  _ScanResult_adv
{
	char ApNum; 	  /**< The number of access points found in scanning.*/
	struct
	{
		char ssid[32];	/**< The SSID of an access point.*/
		char ApPower;	/**< Signal strength, min:0, max:100*/
		char bssid[6];	/**< The BSSID of an access point.*/
		char channel;	/**< The RF frequency, 1-13*/
		wlan_sec_type_t security;	/**< Security type, @ref wlan_sec_type_t*/
	} *ApList;
} ScanResult_adv;

/**
 *  @brief  Input network paras, used in bk_wlan_start function.
 */
typedef struct _network_InitTypeDef_st
{
    char wifi_mode;               /**< DHCP mode: @ref wlanInterfaceTypedef.*/
    char wifi_ssid[33];           /**< SSID of the wlan needs to be connected.*/
    char wifi_key[64];            /**< Security key of the wlan needs to be connected, ignored in an open system.*/
    char local_ip_addr[16];       /**< Static IP configuration, Local IP address. */
    char net_mask[16];            /**< Static IP configuration, Netmask. */
    char gateway_ip_addr[16];     /**< Static IP configuration, Router IP address. */
    char dns_server_ip_addr[16];   /**< Static IP configuration, DNS server IP address. */
    char dhcp_mode;                /**< DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
    char reserved[32];
    int  wifi_retry_interval;     /**< Retry interval if an error is occured when connecting an access point,
                                     time unit is millisecond. */
} network_InitTypeDef_st;

/**
 *  @brief  Advanced precise wlan parameters, used in @ref network_InitTypeDef_adv_st.
 */
typedef struct
{
    char    ssid[32];    /**< SSID of the wlan that needs to be connected. Example: "SSID String". */
    char    bssid[6];    /**< BSSID of the wlan needs to be connected. Example: {0xC8 0x93 0x46 0x11 0x22 0x33}. */
    uint8_t channel;     /**< Wlan's RF frequency, channel 0-13. 1-13 means a fixed channel
                            that can speed up a connection procedure, 0 is not a fixed input
                            means all channels are possible*/
    wlan_sec_type_t security;
}   apinfo_adv_t;

/**
 *  @brief  Input network precise paras in bkWlanStartAdv function.
 */
typedef struct _network_InitTypeDef_adv_st
{
    apinfo_adv_t ap_info;         /**< @ref apinfo_adv_t. */
    char  key[64];                /**< Security key or PMK of the wlan. */
    int   key_len;                /**< The length of the key. */
    char  local_ip_addr[16];      /**< Static IP configuration, Local IP address. */
    char  net_mask[16];           /**< Static IP configuration, Netmask. */
    char  gateway_ip_addr[16];    /**< Static IP configuration, Router IP address. */
    char  dns_server_ip_addr[16];  /**< Static IP configuration, DNS server IP address. */
    char  dhcp_mode;               /**< DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
    char  reserved[32];
    int   wifi_retry_interval;    /**< Retry interval if an error is occured when connecting an access point,
                                  time unit is millisecond. */
} network_InitTypeDef_adv_st;

typedef struct _network_InitTypeDef_ap_st
{
    char wifi_ssid[32];
    char wifi_key[64];
    uint8_t channel;
    wlan_sec_type_t security;
	uint8_t ssid_hidden;
	uint8_t max_con;
    char local_ip_addr[16];
    char net_mask[16];
    char gateway_ip_addr[16];
    char dns_server_ip_addr[16];
    char dhcp_mode;
    char reserved[32];
    int  wifi_retry_interval;
} network_InitTypeDef_ap_st;

/**
 *  @brief  Current link status in station mode.
 */
typedef struct _linkStatus_t
{
    msg_sta_states conn_state;       /**< The link to wlan is established or not, 0: disconnected, 1: connected. */
    int wifi_strength;      /**< Signal strength of the current connected AP */
    uint8_t  ssid[32];      /**< SSID of the current connected wlan */
    uint8_t  bssid[6];      /**< BSSID of the current connected wlan */
    int      channel;       /**< Channel of the current connected wlan */
    wlan_sec_type_t security;
} LinkStatusTypeDef;

/*WiFi Monitor */
/* @brief define the monitor callback function.
  * @param data: the 802.11 packet
  * @param len: the length of this packet, include FCS
  * @param rssi: the rssi of the received packet.
  */
typedef struct
{
	int8_t rssi;
}wifi_link_info_t; 

typedef struct vif_addcfg_st {
    char *ssid; 
    char *key;
    char *name;  
    u8 wlan_role;
    u8 adv; 
} VIF_ADDCFG_ST, *VIF_ADDCFG_PTR;

typedef void (*monitor_cb_t)(uint8_t *data, int len, wifi_link_info_t *info);

/**
  * @brief     configure country info
  *
  * @attention 1. The default country is {.cc="CN", .schan=1, .nchan=13, policy=WIFI_COUNTRY_POLICY_AUTO}
  * @attention 2. When the country policy is WIFI_COUNTRY_POLICY_AUTO, the country info of the AP to which
  *               the station is connected is used. E.g. if the configured country info is {.cc="USA", .schan=1, .nchan=11}
  *               and the country info of the AP to which the station is connected is {.cc="JP", .schan=1, .nchan=14}
  *               then the country info that will be used is {.cc="JP", .schan=1, .nchan=14}. If the station disconnected
  *               from the AP the country info is set back back to the country info of the station automatically,
  *               {.cc="USA", .schan=1, .nchan=11} in the example.
  * @attention 3. When the country policy is WIFI_COUNTRY_POLICY_MANUAL, always use the configured country info.
  * @attention 4. When the country info is changed because of configuration or because the station connects to a different
  *               external AP, the country IE in probe response/beacon of the soft-AP is changed also.
  * @attention 5. The country configuration is not stored into flash
  * @attention 6. This API doesn't validate the per-country rules, it's up to the user to fill in all fields according to 
  *               local regulations.
  *
  * @param     country   the configured country info
  *
  * @return
  *    - kNoErr: succeed
  *    - kNotInitializedErr: WiFi is not initialized
  *    - kParamErr: invalid argument
  */
OSStatus bk_wlan_set_country(const wifi_country_t *country);

/**
  * @brief     get the current country info
  *
  * @param     country  country info
  *
  * @return
  *    - kNoErr: succeed
  *    - kNotInitializedErr: WiFi is not initialized
  *    - kParamErr: invalid argument
  */
OSStatus bk_wlan_get_country(wifi_country_t *country);
/** @brief  Connect or establish a Wi-Fi network in normal mode (station or soft ap mode).
 * 
 *  @detail This function can establish a Wi-Fi connection as a station or create
 *          a soft AP that other staions can connect (4 stations Max). In station mode,  
 *          _BK_ first scan all of the supported Wi-Fi channels to find a wlan that 
 *          matchs the input SSID, and read the security mode. Then try to connect    
 *          to the target wlan. If any error occurs in the connection procedure or  
 *          disconnected after a successful connection, _BK_ start the reconnection 
 *          procedure in backgound after a time interval defined in inNetworkInitPara.
 *          Call this function twice when setup coexistence mode (staion + soft ap). 
 *          This function retruns immediately in station mode, and the connection will 
 *          be executed in background.
 *
 *  @param  inNetworkInitPara: Specifies wlan parameters. 
 *
 *  @return In station mode, allways retrurn kWlanNoErr.
 *          In soft ap mode, return kWlanXXXErr
 */
OSStatus bk_wlan_start(network_InitTypeDef_st* inNetworkInitPara);
/** @brief  Connect to a Wi-Fi network with advantage settings (station mode only)
 * 
 *  @detail This function can connect to an access point with precise settings,
 *          that greatly speed up the connection if the input settings are correct
 *          and fixed. If this fast connection is failed for some reason, _BK_ 
 *          change back to normal: scan + connect mode refer to @ref bkWlanStart.
 *          This function returns after the fast connection try.
 *
 *  @note   This function cannot establish a soft ap, use StartNetwork() for this
 *          purpose. 
 *          If input SSID length is 0, _BK_ use BSSID to connect the target wlan.
 *          If both SSID and BSSID are all wrong, the connection will be failed.
 *
 *  @param  inNetworkInitParaAdv: Specifies the precise wlan parameters.
 *
 *  @retrun Allways return kWlanNoErr although error occurs in first fast try 
 *          kWlanTimeoutErr: DHCP client timeout
 */
OSStatus bk_wlan_start_sta_adv(network_InitTypeDef_adv_st* inNetworkInitParaAdv);
/** @brief  Read current IP status on a network interface.
 * 
 *  @param  outNetpara: Point to the buffer to store the IP address. 
 *  @param  inInterface: Specifies wlan interface. 
 *             @arg Soft_AP: The soft AP that established by bkWlanStart()
 *             @arg Station: The interface that connected to an access point
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus bk_wlan_get_ip_status(IPStatusTypedef *outNetpara, WiFi_Interface inInterface);
/** @brief  Read current wireless link status on station interface.
 * 
 *  @param  outStatus: Point to the buffer to store the link status. 
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus bk_wlan_get_link_status(LinkStatusTypeDef *outStatus);
/** @brief  Start a wlan scanning in 2.4GHz in _BK_ backfround.
 *  
 *  @detail Once the scan is completed, _BK_ sends a notify: 
 *          bk_notify_WIFI_SCAN_COMPLETED, with callback function:
 *          void (*function)(ScanResult *pApList, bk_Context_t * const inContext)
 *          Register callback function using @ref bk_add_notification() before scan.
 */
void bk_wlan_start_scan(void);

/** @brief  Start wifi monitor mode
 * 
 *  @detail This function disconnect wifi station and softAP. 
 *       
 */
int bk_wlan_start_monitor(void);

/** @brief  Stop wifi monitor mode
 * 
 */
int bk_wlan_stop_monitor(void);

/** @brief  Set the monitor channel
 * 
 *  @detail This function change the monitor channel (from 1~13).
 *       it can change the channel dynamically, don't need restart monitor mode.
 */
int bk_wlan_set_channel_sync(int channel);
int bk_wlan_set_channel(int channel);

/** @brief  Register the monitor callback function
 *        Once received a 802.11 packet call the registered function to return the packet.
 */
OSStatus bk_wlan_start_sta(network_InitTypeDef_st *inNetworkInitPara);

OSStatus bk_wlan_start_ap(network_InitTypeDef_st *inNetworkInitPara);

OSStatus bk_wlan_set_ip_status(IPStatusTypedef *inNetpara, WiFi_Interface inInterface);

OSStatus bk_wlan_start_ap_adv(network_InitTypeDef_ap_st *inNetworkInitParaAP);
void bk_wlan_ap_para_info_get(network_InitTypeDef_ap_st *ap_info);
void bk_wlan_register_monitor_cb(monitor_cb_t fn);
monitor_cb_t bk_wlan_get_monitor_cb(void);
monitor_cb_t bk_wlan_get_bcn_cb(void);
void bk_wlan_enable_lsig(void);
void bk_wlan_disable_lsig(void);
int bk_wlan_is_monitor_mode(void);
uint32_t bk_sta_cipher_is_open(void);
uint32_t bk_sta_cipher_is_wep(void);
int bk_sta_cipher_type(void);
int bk_wlan_stop(char mode);
void bk_wlan_ap_set_channel_config(uint8_t channel);
uint8_t bk_wlan_ap_get_channel_config(void);
uint32_t bk_wlan_ap_get_frequency(void);
uint32_t bk_wlan_get_INT_status(void);

extern int is_apm_bss_config_empty(void);
/** @brief  Request deep sleep,and wakeup by gpio.
 *
 *  @param  gpio_index_map:The gpio bitmap which set 1 enable wakeup deep sleep.
 *              gpio_index_map is hex and every bits is map to gpio0-gpio31.
 *          gpio_edge_map:The gpio edge bitmap for wakeup gpios,
 *              gpio_edge_map is hex and every bits is map to gpio0-gpio31.
 *              0:rising,1:falling.
 */
void bk_enter_deep_sleep(UINT32 gpio_index_map,UINT32 gpio_edge_map);

/** @brief  Enable dtim power save,close rf,and wakeup by ieee dtim dynamical
 *
 */
extern int bk_wlan_dtim_rf_ps_mode_enable(void );

/** @brief  Request exit power save by ieee dtim
 */
int bk_wlan_dtim_rf_ps_mode_disable(void);

/** @brief  Send exit dtim dynamical ps mode msg
 */
int power_save_dtim_rf_ps_disable_send_msg(void);

extern int bk_wlan_dtim_rf_ps_timer_start(void);
extern int bk_wlan_dtim_rf_ps_timer_pause(void);

/** @brief  Open dtim with normal flag
 */
extern int bk_wlan_dtim_with_normal_open(void);
/** @brief  Close dtim with normal flag
 */
extern int bk_wlan_dtim_with_normal_close(void);
extern int bk_wlan_mcu_ps_mode_disable(void);
/** @brief  When in dtim rf off mode,user can manual wakeup before dtim wakeup time.
 */
extern int bk_wlan_dtim_rf_ps_mode_do_wakeup();
extern int bk_wlan_dtim_rf_ps_disable_send_msg(void);
extern int bk_wlan_dtim_rf_ps_set_linger_time(UINT32 );
extern UINT32 bk_wlan_dtim_rf_ps_get_sleep_time(void);
extern int bk_wlan_mcu_suppress_and_sleep(UINT32);
extern void bk_wlan_set_max_txpwr(FP32 max_tx_pwr);
extern void user_callback_register(void);

/** @brief  Enable mcu power save,close mcu ,and wakeup by irq
 *
 */
extern int bk_wlan_mcu_ps_mode_enable(void);

extern int bk_wlan_mcu_ps_mode_disable(void);
enum
{
    RF_SLEEP        = 0,
    MCU_SLEEP,
    LOW_SLEEP,
    DEEP_PS,
} ;
#define     PS_RF_SLEEP_BIT         CO_BIT(RF_SLEEP) 
#define     PS_MCU_SLEEP_BIT        CO_BIT(MCU_SLEEP) 
#define     PS_DEEP_SLEEP_BIT       CO_BIT(DEEP_PS) 
typedef     UINT32      BK_PS_LEVEL;

void bk_wlan_connection_loss(void);
void bk_wlan_start_assign_scan(UINT8 **ssid_ary, UINT8 ssid_num);

void bk_wlan_scan_ap_reg_cb(FUNC_2PARAM_PTR ind_cb);
unsigned char bk_wlan_get_scan_ap_result_numbers(void);
void bk_wlan_get_scan_ap_result(SCAN_RST_ITEM_PTR scan_rst_table,unsigned char get_scanu_num);
void bk_wlan_ap_set_default_channel(uint8_t channel);
uint8_t bk_wlan_has_role(uint8_t role);
void bk_wlan_phy_open_cca(void);
void bk_wlan_phy_close_cca(void);
void bk_wlan_phy_show_cca(void);

#if (CFG_SUPPORT_ALIOS)
/**********************for alios*******************************/
void bk_wifi_get_mac_address(char *mac);
void bk_wifi_set_mac_address(char *mac);
static void bk_monitor_callback(uint8_t *data, int len, wifi_link_info_t *info);
void bk_wlan_register_mgnt_monitor_cb(monitor_cb_t fn);
monitor_cb_t bk_wlan_get_mgnt_monitor_cb(void);
int bk_wlan_send_80211_raw_frame(uint8_t *buffer, int len);
int bk_wlan_suspend(void);
int bk_wlan_suspend_station(void);
int bk_wlan_suspend_softap(void);
uint32_t bk_wlan_max_power_level_get(void);
OSStatus bk_wlan_get_bssid_info(apinfo_adv_t *ap, uint8_t **key, int *key_len);
#ifdef CONFIG_AOS_MESH
void wlan_register_mesh_monitor_cb(monitor_cb_t fn);
monitor_cb_t wlan_get_mesh_monitor_cb(void);
int wlan_is_mesh_monitor_mode(void);
int wlan_set_mesh_bssid(uint8_t *bssid);
uint8_t *wlan_get_mesh_bssid(void);
#endif
#endif

#endif// _WLAN_UI_PUB_

