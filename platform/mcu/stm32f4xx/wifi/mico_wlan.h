
#ifndef __MICOWLAN_H__
#define __MICOWLAN_H__


#ifdef __cplusplus
extern "C" {
#endif


  
#define micoWlanStart             StartNetwork
#define micoWlanStartAdv          StartAdvNetwork
#define micoWlanGetIPStatus       getNetPara
#define micoWlanGetLinkStatus     CheckNetLink
#define micoWlanStartScan         mxchipStartScan
#define micoWlanStartScanAdv      mxchipStartAdvScan
#define micoWlanPowerOff          wifi_power_down
#define micoWlanPowerOn           wifi_power_up
#define micoWlanSuspend           wlan_disconnect
#define micoWlanSuspendStation    sta_disconnect
#define micoWlanSuspendSoftAP     uap_stop
#define micoWlanStartEasyLink     OpenEasylink2_withdata
#define micoWlanStopEasyLink      CloseEasylink2
#define micoWlanStartEasyLinkPlus OpenEasylink
#define micoWlanStopEasyLinkPlus  CloseEasylink
#define micoWlanStartWPS          OpenConfigmodeWPS
#define micoWlanStopWPS           CloseConfigmodeWPS
#define micoWlanEnablePowerSave   ps_enable
#define micoWlanDisablePowerSave  ps_disable
#define micoWlanStartAirkiss      OpenAirkiss
#define micoWlanStopAirkiss       CloseAirkiss

#define mico_wlan_get_mac_address wlan_get_mac_address

#define WiFi_Interface  wlanInterfaceTypedef
#define net_para_st     IPStatusTypedef

/** @addtogroup MICO_Core_APIs
  * @{
  */

/** \defgroup MICO_Wlan MiCO Wlan operations
  * @brief Provide management APIs for MiCO wlan function
  * @{
  */

// ==== OSStatus extension for MiCO wlan ====
#define kWlanNoErr                            0     /**< No error occurred in wlan operation. */
#define kWlanPendingErr                       1     /**< Pending. */
#define kWlanTimeoutErr                       2     /**< Timeout occurred in wlan operation. */
#define kWlanPartialResultsErr                3     /**< Partial results */
#define kWlanInvalidKeyErr                    4     /**< Invalid key */
#define kWlanNotExistErr                      5     /**< Does not exist */
#define kWlanAuthenticatedErr                 6     /**< Not authenticated */
#define kWlanNotKeyedErr                      7     /**< Not keyed */
#define kWlanIOCtlErr                         8     /**< IOCTL fail */
#define kWlanBUFFER_UNAVAILABLE_TEMPORARY     9     /**< Buffer unavailable temporarily */
#define kWlanUFFER_UNAVAILABLE_PERMANENT      10    /**< Buffer unavailable permanently */
#define kWlanWPS_PBC_OVERLAP                  11    /**< WPS PBC overlap */
#define kWlanConnectionLost                   12    /**< Connection lost */


#define kWlanGeneralErr                       -1    /**< General error in wlan operation. */
#define kWlanArgErr                           -2    /**< Wlan parameter is incorrect, missing, or not appropriate. */
#define kWlanOptionErr                        -3    /**< Bad option */
#define kWlanNotUp                            -4    /**< Not up */
#define kWlanNOotDown                         -5    /**< Not down */
#define kWlanNotAP                            -6    /**< Not AP */
#define kWlanNotSTA                           -7    /**< Not STA  */
#define kWlanKeyIndexErr                      -8    /**< BAD Key Index */
#define kWlanRadioOff                         -9    /**< Radio Off */
#define kWlanNotBandLocked                    -10   /**< Not  band locked */
#define kWlanClkErr                           -11   /**< No Clock */
#define kWlanRateSetErr                       -12   /**< BAD Rate valueset */
#define kWlanBandErr                          -13   /**< BAD Band */
#define kWlanBufTooShort                      -14   /**< Buffer too short */
#define kWlanBufTooLong                       -15   /**< Buffer too long */
#define kWlanBusy                             -16   /**< Busy */
#define kWlanNotAssociated                    -17   /**< Not Associated */
#define kWlanSSIDLenErr                       -18   /**< Bad SSID len */
#define kWlanOutOfRangeChannelderr            -19   /**< Out of Range Channel */
#define kWlanChannelErr                       -20   /**< Bad Channel */
#define kWlanAddressErr                       -21   /**< Bad Address */
#define kWlanResourcesErr                     -22   /**< Not Enough Resources */
#define kWlanUnUnsupported                    -23   /**< Unsupported */
#define kWlanLengthErr                        -24   /**< Bad length */
#define kWlanNotReadyErr                      -25   /**< Not Ready */
#define kWlanNotPermittedErr                  -26   /**< Not Permitted */
#define kWlanMemoryErr                        -27   /**< No Memory */
#define kWlanAssociated                       -28   /**< Associated */
#define kWlanNotInRangeErr                    -29   /**< Not In Range */
#define kWlanNotFoundErr                      -30   /**< Not Found */
#define kWlanWMENotEnabled                    -31   /**< WME Not Enabled */
#define kWlanTSPECNotFound                    -32   /**< TSPEC Not Found */
#define kWlanACMNotSupported                  -33   /**< ACM Not Supported */
#define kWlanNOTWMEAssociation                -34   /**< Not WME Association */
#define kWlanSDIOBusErr                       -35   /**< SDIO Bus Error */
#define kWlanNotAccessible                    -36   /**< WLAN Not Accessible */
#define kWlanVersionErr                       -37   /**< Incorrect version */
#define kWlanTXErr                            -38   /**< TX failure */
#define kWlanRXErr                            -39   /**< RX failure */
#define kWlanNoDeviceErr                      -40   /**< Device not present */
#define kWlanUnFinishedErr                    -41   /**< To be finished */
#define kWlanDisabled                         -43   /**< Disabled in this build */
#define kWlanErrLast                          -44


#define DHCP_Disable  (0)   /**< Disable DHCP service. */
#define DHCP_Client   (1)   /**< Enable DHCP client which get IP address from DHCP server automatically,  
                                 reset Wi-Fi connection if failed. */
#define DHCP_Server   (2)   /**< Enable DHCP server, needs assign a static address as local address. */

/** 
 *  @brief  wlan network interface enumeration definition.
 */ 
typedef enum { 
  Soft_AP,  /**< Act as an access point, and other station can connect, 4 stations Max*/
  Station   /**< Act as a station which can connect to an access point*/
} wlanInterfaceTypedef; 

typedef uint8_t wlan_if_t;


typedef uint8_t wlan_sec_type_t;

/** 
 *  @brief  wlan local IP information structure definition.  
 */ 
typedef struct {
  uint8_t dhcp;       /**< DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server.*/
  char    ip[16];     /**< Local IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
  char    gate[16];   /**< Router IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
  char    mask[16];   /**< Netmask on the target wlan interface: @ref wlanInterfaceTypedef.*/
  char    dns[16];    /**< DNS server IP address.*/
  char    mac[16];    /**< MAC address, example: "C89346112233".*/
  char    broadcastip[16];
} IPStatusTypedef;
 
/** 
 *  @brief  Scan result using advanced scan.  
 */  
typedef  struct  _ScanResult_adv 
{ 
  char ApNum;       /**< The number of access points found in scanning.*/
  struct { 
    char ssid[32];  /**< The SSID of an access point.*/
    char bssid[6];  /**< The BSSID of an access point.*/
    char channel;   /**< The RF frequency, 1-13*/
    wlan_sec_type_t security;   /**< Security type, @ref wlan_sec_type_t*/
    int16_t rssi;   /**< Signal strength*/
  } * ApList;
} ScanResult_adv; 

/** 
 *  @brief  Scan result using normal scan.  
 */
typedef  struct  _ScanResult 
{  
  char ApNum;       /**< The number of access points found in scanning. */
  struct {  
    char ssid[32];  /**< The SSID of an access point. */
    int16_t rssi;   /**< Signal strength*/
  } * ApList; 
} ScanResult;  

/** 
 *  @brief  Input network paras, used in micoWlanStart function.  
 */
typedef struct _network_InitTypeDef_st 
{ 
  char wifi_mode;               /**< DHCP mode: @ref wlanInterfaceTypedef.*/
  char wifi_ssid[32];           /**< SSID of the wlan needs to be connected.*/
  char wifi_key[64];            /**< Security key of the wlan needs to be connected, ignored in an open system.*/
  char local_ip_addr[16];       /**< Static IP configuration, Local IP address. */
  char net_mask[16];            /**< Static IP configuration, Netmask. */
  char gateway_ip_addr[16];     /**< Static IP configuration, Router IP address. */
  char dnsServer_ip_addr[16];   /**< Static IP configuration, DNS server IP address. */
  char dhcpMode;                /**< DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
  char reserved[32];            
  int  wifi_retry_interval;     /**< Retry interval if an error is occured when connecting an access point, 
                                     time unit is millisecond. */
} network_InitTypeDef_st; 

typedef struct _network_Enterprise_st 
{ 
  char wifi_ssid[32];           /**< SSID of the wlan needs to be connected.*/

  char identity[32];
  unsigned char *ca_cert;
  unsigned int ca_cert_size;
  unsigned char *client_cert;
  unsigned int client_cert_size;
  unsigned char *client_key;
  unsigned int client_key_size;
  
  char local_ip_addr[16];       /**< Static IP configuration, Local IP address. */
  char net_mask[16];            /**< Static IP configuration, Netmask. */
  char gateway_ip_addr[16];     /**< Static IP configuration, Router IP address. */
  char dnsServer_ip_addr[16];   /**< Static IP configuration, DNS server IP address. */
  char dhcpMode;                /**< DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
} network_Enterprise_st; 

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
 *  @brief  Input network precise paras in micoWlanStartAdv function.  
 */
typedef struct _network_InitTypeDef_adv_st
{
  apinfo_adv_t ap_info;         /**< @ref apinfo_adv_t. */
  char  key[64];                /**< Security key or PMK of the wlan. */
  int   key_len;                /**< The length of the key. */
  char  local_ip_addr[16];      /**< Static IP configuration, Local IP address. */
  char  net_mask[16];           /**< Static IP configuration, Netmask. */
  char  gateway_ip_addr[16];    /**< Static IP configuration, Router IP address. */
  char  dnsServer_ip_addr[16];  /**< Static IP configuration, DNS server IP address. */
  char  dhcpMode;               /**< DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
  char  reserved[32]; 
  int   wifi_retry_interval;    /**< Retry interval if an error is occured when connecting an access point, 
                                  time unit is millisecond. */
} network_InitTypeDef_adv_st;

/** 
 *  @brief  Current link status in station mode.  
 */
typedef struct _linkStatus_t{
  int is_connected;       /**< The link to wlan is established or not, 0: disconnected, 1: connected. */
  int rssi;               /**< Signal strength of the current connected AP */
  uint8_t  ssid[32];      /**< SSID of the current connected wlan */
  uint8_t  bssid[6];      /**< BSSID of the current connected wlan */
  int      channel;       /**< Channel of the current connected wlan */
} LinkStatusTypeDef;


typedef struct {
    char bssid[6];
    char ssid[33];
    char key[65];
    int  user_data_len;
    char user_data[65];
} easylink_result_t;
    
#pragma pack(1)
typedef struct _wifi_mgmt_frame_tx
{
#ifdef MW310
        /** Packet Length */
        uint16_t frm_len;
#endif
        /** Frame Control */
        uint16_t frm_ctl;
        /** Duration ID */
        uint16_t duration_id;
        /** Address1 */
        uint8_t addr1[6];
        /** Address2 */
        uint8_t addr2[6];
        /** Address3 */
        uint8_t addr3[6];
#ifdef MW310
        /** Address4 */
        uint8_t addr4[6];
#endif
        /** Sequence Control */
        uint16_t seq_ctl;
        
        /** Frame payload */
        uint8_t payload[2];
} wifi_mgmt_frame_tx;
#pragma pack()

/** @defgroup MICO_WLAN_GROUP_1 MiCO Basic Wlan Functions
  * @brief Provide Basic APIs for MiCO wlan functions
  * @{
  */

void mico_wlan_get_mac_address( uint8_t *mac );

/** @brief  Connect or establish a Wi-Fi network in normal mode (station or soft ap mode).
 * 
 *  @detail This function can establish a Wi-Fi connection as a station or create
 *          a soft AP that other staions can connect (4 stations Max). In station mode,  
 *          MICO first scan all of the supported Wi-Fi channels to find a wlan that 
 *          matchs the input SSID, and read the security mode. Then try to connect    
 *          to the target wlan. If any error occurs in the connection procedure or  
 *          disconnected after a successful connection, MICO start the reconnection 
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
OSStatus micoWlanStart(network_InitTypeDef_st* inNetworkInitPara);

/** @brief  Connect to a Wi-Fi network with advantage settings (station mode only)
 * 
 *  @detail This function can connect to an access point with precise settings,
 *          that greatly speed up the connection if the input settings are correct
 *          and fixed. If this fast connection is failed for some reason, MICO 
 *          change back to normal: scan + connect mode refer to @ref micoWlanStart.
 *          This function returns after the fast connection try.
 *
 *  @note   This function cannot establish a soft ap, use StartNetwork() for this
 *          purpose. 
 *          If input SSID length is 0, MICO use BSSID to connect the target wlan.
 *          If both SSID and BSSID are all wrong, the connection will be failed.
 *
 *  @param  inNetworkInitParaAdv: Specifies the precise wlan parameters.
 *
 *  @retrun Allways return kWlanNoErr although error occurs in first fast try 
 *          kWlanTimeoutErr: DHCP client timeout
 */
OSStatus micoWlanStartAdv(network_InitTypeDef_adv_st* inNetworkInitParaAdv);

/** @brief  Read current IP status on a network interface.
 * 
 *  @param  outNetpara: Point to the buffer to store the IP address. 
 *  @param  inInterface: Specifies wlan interface. 
 *             @arg Soft_AP: The soft AP that established by micoWlanStart()
 *             @arg Station: The interface that connected to an access point
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanGetIPStatus(IPStatusTypedef *outNetpara, WiFi_Interface inInterface);

/** @brief  Read current wireless link status on station interface.
 * 
 *  @param  outStatus: Point to the buffer to store the link status. 
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanGetLinkStatus(LinkStatusTypeDef *outStatus);

/** @brief  Start a wlan scanning in 2.4GHz in MICO backfround.
 *  
 *  @detail Once the scan is completed, MICO sends a notify: 
 *          mico_notify_WIFI_SCAN_COMPLETED, with callback function:
 *          void (*function)(ScanResult *pApList, mico_Context_t * const inContext)
 *          Register callback function using @ref mico_add_notification() before scan.
 */
void micoWlanStartScan(void);

/** @brief  Start a wlan scanning in 2.4GHz in MICO backfround. 
 *  
 *  @detail Once the scan is completed, MICO sends a notify: 
 *          mico_notify_WIFI_SCAN_ADV_COMPLETED, with callback function:
 *          void (*function)(ScanResultAdv *pApList, mico_Context_t * const inContext)
 *          Register callback function using @ref mico_add_notification() before scan.
 */
void micoWlanStartScanAdv(void);

/** @brief  Start a wlan scanning specified SSID in 2.4GHz in MICO backfround.
 *
 *  @detail Once the scan is completed, MICO sends a notify:
 *          mico_notify_WIFI_SCAN_ADV_COMPLETED, with callback function:
 *          void (*function)(ScanResultAdv *pApList, mico_Context_t * const inContext)
 *          Register callback function using @ref mico_add_notification() before scan.
 */
int mxchip_active_scan(char*ssid, int is_adv);


/** @brief  Close the RF chip's power supply, all network connection is lost.
 * 
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanPowerOff(void);

/** @brief  Open the RF's power supply and do some necessary initialization.
 *
 *  @note   The defaut RF state is powered on after @ref micoInit, so this function is 
 *          not needed after micoInit.
 * 
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanPowerOn(void);

/**@brief  Close all the Wi-Fi connections, station mode and soft ap mode
 * 
 * @note   This function also stop the background retry mechanism started by 
 *         MICOWlanStart() and MICOWlanStartAdv()
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanSuspend(void);

/** @brief  Close the connection in station mode
 * 
 *  @note   This function also stop the background retry mechanism started by 
 *          StartNetwork() and StartAdvNetwork()
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanSuspendStation(void);

/** @brief  Stop soft ap and close all stations' connections
 * 
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanSuspendSoftAP(void);
/**
  * @}
  */

/** @defgroup MICO_WLAN_GROUP_2 MiCO Wlan Easylink Functions
  * @brief Provide management APIs for Easylink function in MiCO.
  * @{
  */

/** @brief  Start EasyLink configuration with user extra data
 * 
 *  @detail This function can read SSID, password and extra user data sent from
 *          Easylink APP.  
 *          MICO sends a callback: mico_notify_EASYLINK_WPS_COMPLETED
 *          with function:
 *          void (*function)(network_InitTypeDef_st *nwkpara, mico_Context_t * const inContext);
 *          that provide SSID and password, nwkpara is NULL if timeout or get an error
 *          More
 *          MICO sends a callback: mico_notify_EASYLINK_GET_EXTRA_DATA
 *          with function:
 *          void (*function)(int datalen, char*data, mico_Context_t * const inContext);
 *          that provide a buffer where the data is saved
 *
 *  @param  inTimeout: If easylink is executed longer than this parameter in backgound.
 *          MICO stops EasyLink and sends a callback where nwkpara is NULL
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanStartEasyLink(int inTimeout);

/** @brief  Start EasyLink plus configuration with user extra data
 *
 *  @detail This function has the same function as OpenEasylink2(), but it can
 *          read more data besides SSID and password, these data is sent from
 *          Easylink APP.
 *          MICO sends a callback: mico_notify_EASYLINK_GET_EXTRA_DATA
 *          with function:
 *          void (*function)(int datalen, char*data, mico_Context_t * const inContext);
 *          that provide a buffer where the data is saved
 *
 *  @param  inTimeout: If easylink is executed longer than this parameter in backgound.
 *          MICO stops EasyLink and sends a callback where nwkpara is NULL
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */

OSStatus micoWlanStartEasyLinkPlus(int inTimeout);
OSStatus micoWlanStartAws(int inTimeout);


/** @brief  Start EasyLink plus configuration with UAP coexistence
 *
 *  @param  inTimeout: If easylink is executed longer than this parameter in backgound.
 *          MICO stops EasyLink and sends a callback where nwkpara is NULL
 *  @param  ssid: SSID of UAP
 *  @param  key: Password of UAP
 *  @param  channel: Wi-Fi channel of UAP
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus mico_wlan_easylink_uap_start(int timeout, char *ssid, char*key, int channel);


/** @brief  Stop EasyLink configuration procedure
 *  
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanStopEasyLink(void);
OSStatus micoWlanStopEasyLinkPlus(void);
OSStatus micoWlanStopAws(void);

/**
  * @}
  */



/** @defgroup MICO_WLAN_GROUP_3 MiCO Wlan WPS Functions
  * @brief Provide management APIs for WPS function in MiCO.
  * @{
  */
/** @brief  Start WPS configuration procedure
 *
 *  @param  inTimeout: If WPS is executed longer than this parameter in backgound.
 *          MICO stops WPS and sends a callback where nwkpara is NULL
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanStartWPS(int inTimeout);

/** @brief  Stop WPS configuration procedure
 *  
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus micoWlanStopWPS(void);


/**
  * @}
  */


/** @defgroup MICO_WLAN_GROUP_4 MiCO Wlan Airkiss Functions
  * @brief Provide management APIs for Airkiss function in MiCO.
  * @{
  */

/** @brief  Start wechat airkiss configuration procedure
 *
 *  @detail This function can read SSID, password and extra user data sent from
 *          Easylink APP.  
 *          MICO sends a callback: mico_notify_EASYLINK_WPS_COMPLETED
 *          with function:
 *          void (*function)(network_InitTypeDef_st *nwkpara, mico_Context_t * const inContext);
 *          that provide SSID and password, nwkpara is NULL if timeout or get an error
 *
 *  @param  inTimeout: If airkiss is executed longer than this parameter in backgound.
 *          MICO stops airkiss and sends a callback where nwkpara is NULL
 *
 *  @retval kNoErr.
 */
OSStatus micoWlanStartAirkiss(int inTimeout);

/** @brief  Stop wechat airkiss configuration procedure
 *  
 *  @retval kNoErr.
 */
OSStatus micoWlanStopAirkiss(void);
/**
  * @}
  */

/** @defgroup MICO_WLAN_GROUP_5 MiCO Wlan Power Functions
  * @brief Provide management APIs for powersave function in MiCO wlan.
  * @{
  */
/** @brief  Enable IEEE power save mode
 * 
 *  @detail When this function is enabled, MICO enter IEEE power save mode if
 *          MICO is in station mode and has connected to an AP, and do not need 
 *          any other control from application. To save more power, use 
 *          @ref mico_mcu_powersave_config
 * @retval  None
 */

void micoWlanEnablePowerSave(void);

/**
 * @brief  Disable IEEE power save mode
 *
 * @retval  None
 */
void micoWlanDisablePowerSave(void); 

/**
  * @}
  */

/** @defgroup MICO_WLAN_GROUP_6 MiCO Wlan debug output Functions
  * @brief Provide management APIs for wlan debug info output in MiCO wlan.
  * @{
  */
/**
 * @brief  Enable debug info output on stdio
 *
 * @param  enable: Enable or disable this function
 *
 * @retval None
 *
 */
void wifimgr_debug_enable(bool enable);

/*WiFi Monitor */
/* @brief define the monitor callback function.
  * @param data: the 802.11 packet
  * @param len: the length of this packet, include FCS
  * @param rssi: the rssi of the received packet.
  */
typedef void (*monitor_cb_t)(uint8_t*data, int len);
enum {
	WLAN_RX_BEACON,    /* receive beacon packet */
	WLAN_RX_PROBE_REQ, /* receive probe request packet */
	WLAN_RX_PROBE_RES, /* receive probe response packet */
	WLAN_RX_ACTION,    /* receive action packet */
	WLAN_RX_MANAGEMENT,/* receive ALL management packet */
	WLAN_RX_DATA,      /* receive ALL data packet */
	WLAN_RX_MCAST_DATA,/* receive ALL multicast and broadcast packet */

	WLAN_RX_ALL,       /* receive ALL 802.11 packet */
};

/** @brief  Add the packet type which monitor should receive
 * 
 *  @detail This function can be called many times to receive different wifi packets.
 */
int mico_wlan_monitor_rx_type(int type);

/** @brief  Start wifi monitor mode
 * 
 *  @detail This function disconnect wifi station and softAP. 
 *       
 */
int mico_wlan_start_monitor(void);

/** @brief  Stop wifi monitor mode
 * 
 */
int mico_wlan_stop_monitor(void);

/** @brief  Set the monitor channel
 * 
 *  @detail This function change the monitor channel (from 1~13).
 *       it can change the channel dynamically, don't need restart monitor mode.
 */
OSStatus mico_wlan_monitor_set_channel( uint8_t channel );

/** @brief  Get the monitor channel
 * 
 *  @detail This function get the monitor channel (from 1~13).
 *       it can change the channel dynamically, don't need restart monitor mode.
 */
OSStatus mico_wlan_monitor_get_channel( uint8_t *channel );

/** @brief  Register the monitor callback function
 *        Once received a 802.11 packet call the registered function to return the packet.
 */
void mico_wlan_register_monitor_cb(monitor_cb_t fn);

/** @brief  Send management frame
 */
OSStatus mico_wlan_send_mgnt(uint8_t *buffer, uint32_t length);

/**@brief Add a custom IE to a WLAN interface
 *
 * @param[in]  wlan_if    : WLAN interface, @ref wlanInterfaceTypedef
 * @param[in]  custom_ie  : custome IE data buffer
 * @param[in]  len        : custome IE data length
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus mico_wlan_custom_ie_add(wlan_if_t wlan_if, uint8_t *custom_ie, uint32_t len);

enum custom_ie_delete_op_e
{
  CUSTOM_IE_DELETE_ALL = 0,
  CUSTOM_IE_DELETE_BY_OUI = 1,
};
typedef uint8_t custom_ie_delete_op_t;

/**@brief Delete all custom IE on a WLAN interface
 *
 * @param[in]  wlan_if      : WLAN interface, @ref wlanInterfaceTypedef
 * @param[in]  op           : delete opration, @ref custom_ie_delete_op_e
 * @param[in]  option_data  : optional data, set to NULL if no need
 * @param[in]  len          : optional data length, set to 0 if no optional data
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus mico_wlan_custom_ie_delete(wlan_if_t wlan_if, custom_ie_delete_op_t op, uint8_t *option_data, uint32_t len);

/**@brief Get the MAC address of an WLAN interface
 *
 * @param[in]   wlan_if   : WLAN interface, @ref wlanInterfaceTypedef
 * @param[out]  mac       : pointer to the buffer which will store the MAC address
 *
 * @return    None
 */
void mico_wlan_get_mac_address_by_interface( wlan_if_t wlan_if, uint8_t *mac );
/**
  * @}
  */
#ifdef __cplusplus
    }
#endif

#endif //__MICOWLAN_H__

/**
  * @}
  */

/**
  * @}
  */



