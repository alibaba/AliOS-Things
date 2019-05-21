/**
 ****************************************************************************************
 *
 * @file mac.h
 *
 * @brief MAC related definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MAC_H_
#define _MAC_H_

/**
 ****************************************************************************************
 * @defgroup CO_MAC CO_MAC
 * @ingroup COMMON
 * @brief  Common defines,structures
 *
 * This module contains defines commonaly used for MAC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"
#include "co_bool.h"
#include "co_math.h"
#include "compiler.h"

// for memcmp
#include <string.h>

#include "compiler.h"


/*
 * DEFINES
 ****************************************************************************************
 */
/// duration of a Time Unit in microseconds
#define TU_DURATION                     1024

/// max number of channels in the 2.4 GHZ band
#define MAC_DOMAINCHANNEL_24G_MAX       14

/// max number of channels in the 5 GHZ band
#define MAC_DOMAINCHANNEL_5G_MAX        45

/// Mask to test if it's a basic rate - BIT(7)
#define MAC_BASIC_RATE                  0x80
/// Mask for extracting/checking word alignment
#define WORD_ALIGN                      3

#define MAX_AMSDU_LENGTH                7935

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/// Access Category enumeration
enum
{
    AC_BK = 0,
    AC_BE,
    AC_VI,
    AC_VO,
    AC_MAX
};

/// Traffic ID enumeration
enum
{
    TID_0,
    TID_1,
    TID_2,
    TID_3,
    TID_4,
    TID_5,
    TID_6,
    TID_7,
    TID_MGT,
    TID_MAX
};

/// SCAN type
enum
{
    SCAN_PASSIVE,
    SCAN_ACTIVE
};

/// rates
enum
{
    MAC_RATE_1MBPS   =   2,
    MAC_RATE_2MBPS   =   4,
    MAC_RATE_5_5MBPS =  11,
    MAC_RATE_6MBPS   =  12,
    MAC_RATE_9MBPS   =  18,
    MAC_RATE_11MBPS  =  22,
    MAC_RATE_12MBPS  =  24,
    MAC_RATE_18MBPS  =  36,
    MAC_RATE_24MBPS  =  48,
    MAC_RATE_36MBPS  =  72,
    MAC_RATE_48MBPS  =  96,
    MAC_RATE_54MBPS  = 108
};

///hostapd data type
enum
{
	HOSTAPD_MGMT = 0,
	HOSTAPD_DATA
};
/**
 ****************************************************************************************
 * Compare two MAC addresses.
 * The MAC addresses MUST be 16 bit aligned.
 * @param[in] addr1_ptr Pointer to the first MAC address.
 * @param[in] addr2_ptr Pointer to the second MAC address.
 * @return True if equal, false if not.
 ****************************************************************************************
 */
#define MAC_ADDR_CMP(addr1_ptr, addr2_ptr)                                              \
    ((*(((uint8_t*)(addr1_ptr)) + 0) == *(((uint8_t*)(addr2_ptr)) + 0)) &&            \
     (*(((uint8_t*)(addr1_ptr)) + 1) == *(((uint8_t*)(addr2_ptr)) + 1)) &&            \
     (*(((uint8_t*)(addr1_ptr)) + 2) == *(((uint8_t*)(addr2_ptr)) + 2)) &&            \
     (*(((uint8_t*)(addr1_ptr)) + 3) == *(((uint8_t*)(addr2_ptr)) + 3)) &&            \
     (*(((uint8_t*)(addr1_ptr)) + 4) == *(((uint8_t*)(addr2_ptr)) + 4)) &&            \
     (*(((uint8_t*)(addr1_ptr)) + 5) == *(((uint8_t*)(addr2_ptr)) + 5)))
	
#define MAC_ADDR_NULL(addr1_ptr)                                              \
		((*(((uint8_t*)(addr1_ptr)) + 0) == 0) &&			  \
		 (*(((uint8_t*)(addr1_ptr)) + 1) == 0) &&			  \
		 (*(((uint8_t*)(addr1_ptr)) + 2) == 0) &&			  \
		 (*(((uint8_t*)(addr1_ptr)) + 3) == 0) &&			  \
		 (*(((uint8_t*)(addr1_ptr)) + 4) == 0) &&			  \
		 (*(((uint8_t*)(addr1_ptr)) + 5) == 0))
	
#define MAC_ADDR_IS_BSCT(addr1_ptr)                              \
			((*(((uint8_t*)(addr1_ptr)) + 0) == 0xFF) &&		  \
			 (*(((uint8_t*)(addr1_ptr)) + 1) == 0xFF) &&		  \
			 (*(((uint8_t*)(addr1_ptr)) + 2) == 0xFF) &&		  \
			 (*(((uint8_t*)(addr1_ptr)) + 3) == 0xFF) &&		  \
			 (*(((uint8_t*)(addr1_ptr)) + 4) == 0xFF) &&		  \
			 (*(((uint8_t*)(addr1_ptr)) + 5) == 0xFF))
	
#define MAC_ADDR_IS_MULTI(addr1_ptr)                        \
				((*(((uint8_t*)(addr1_ptr)) + 0) == 0x01) &&		  \
				 (*(((uint8_t*)(addr1_ptr)) + 1) == 0x00) &&		  \
				 (*(((uint8_t*)(addr1_ptr)) + 2) == 0x5E))

/**
 ****************************************************************************************
 * Compare two MAC addresses whose alignment is not known.
 * @param[in] __a1 Pointer to the first MAC address.
 * @param[in] __a2 Pointer to the second MAC address.
 * @return True if equal, false if not.
 ****************************************************************************************
 */
#define MAC_ADDR_CMP_PACKED(__a1, __a2)                                                 \
    (memcmp(__a1, __a2, MAC_ADDR_LEN) == 0)

/**
 ****************************************************************************************
 * Copy a MAC address.
 * The MAC addresses MUST be 16 bit aligned.
 * @param[in] addr1_ptr Pointer to the destination MAC address.
 * @param[in] addr2_ptr Pointer to the source MAC address.
 ****************************************************************************************
 */
#define MAC_ADDR_CPY(addr1_ptr, addr2_ptr)                                              \
    *(((uint8_t*)(addr1_ptr)) + 0) = *(((uint8_t*)(addr2_ptr)) + 0);                  \
    *(((uint8_t*)(addr1_ptr)) + 1) = *(((uint8_t*)(addr2_ptr)) + 1);                  \
    *(((uint8_t*)(addr1_ptr)) + 2) = *(((uint8_t*)(addr2_ptr)) + 2);                  \
    *(((uint8_t*)(addr1_ptr)) + 3) = *(((uint8_t*)(addr2_ptr)) + 3);                  \
    *(((uint8_t*)(addr1_ptr)) + 4) = *(((uint8_t*)(addr2_ptr)) + 4);                  \
    *(((uint8_t*)(addr1_ptr)) + 5) = *(((uint8_t*)(addr2_ptr)) + 5)

/**
 ****************************************************************************************
 * Compare two SSID.
 * @param[in] ssid1_ptr Pointer to the first SSID structure.
 * @param[in] ssid2_ptr Pointer to the second SSID structure.
 * @return True if equal, false if not.
 ****************************************************************************************
 */
#define MAC_SSID_CMP(ssid1_ptr,ssid2_ptr)                                               \
    (((ssid1_ptr)->length == (ssid2_ptr)->length) &&                                    \
     (memcmp((&(ssid1_ptr)->array[0]), (&(ssid2_ptr)->array[0]), (ssid1_ptr)->length) == 0))

/// Check if MAC address is a group address: test the multicast bit.
#define MAC_ADDR_GROUP(mac_addr_ptr) ((*((uint8_t *)(mac_addr_ptr))) & 1)

/// MAC address length in bytes.
#define MAC_ADDR_LEN             6

/// MAC address structure.
__PACKED struct mac_addr
{
    /// Array of 16-bit words that make up the MAC address.
    uint16_t array[MAC_ADDR_LEN/2];
};

/// SSID maximum length.
#define MAC_SSID_LEN              32

/// SSID.
struct mac_ssid
{
    /// Actual length of the SSID.
    uint8_t length;
    /// Array containing the SSID name.
    uint8_t array[MAC_SSID_LEN];
};

/// MAC RATE-SET
#define MAC_RATESET_LEN             12
#define MAC_SUPP_RATES_IE_LEN       8
#define MAC_OFDM_PHY_RATESET_LEN    8
#define MAC_EXT_RATES_OFF      8
struct mac_rateset
{
    uint8_t     length;
    uint8_t     array[MAC_RATESET_LEN];
};

/// MAC RATES
#define MAC_MCS_WORD_CNT            3
struct mac_rates
{
    /// MCS 0 to 76
    uint32_t mcs[MAC_MCS_WORD_CNT];
    /// Legacy rates (1Mbps to 54Mbps)
    uint16_t legacy;
};

/// Max number of default keys for given VIF
#define MAC_DEFAULT_KEY_COUNT 4
#define MAC_DEFAULT_MFP_KEY_COUNT 6

/// Structure containing the information about a key
struct key_info_tag
{
    /// Replay counters for RX
    uint64_t rx_pn[TID_MAX];
    /// Replay counter for TX
    uint64_t tx_pn;
    union
    {
        struct
        {
            /// TX MIC key
            uint32_t tx_key[2];
            /// RX MIC key
            uint32_t rx_key[2];
        } mic;
        struct
        {
            uint32_t key[4];
        } mfp;
    }u;
    /// Type of security currently used
    uint8_t cipher;
    /// Key index as specified by the authenticator/supplicant
    uint8_t key_idx;
    /// Index of the key in the HW memory
    uint8_t hw_key_idx;
    /// Flag indicating if the key is valid
    bool valid;
};

/// IV/EIV data
#define MAC_IV_LEN  4
#define MAC_EIV_LEN 4
struct rx_seciv
{
    uint8_t iv[MAC_IV_LEN];
    uint8_t ext_iv[MAC_EIV_LEN];
};

/// MAC MCS SET
#define MAX_MCS_LEN 16 // 16 * 8 = 128
struct mac_mcsset
{
    uint8_t length;
    uint8_t array[MAX_MCS_LEN];
};

/// MAC Secret Key
#define MAC_WEP_KEY_CNT          4  // Number of WEP keys per virtual device
#define MAC_WEP_KEY_LEN         13  // Max size of a WEP key (104/8 = 13)
struct mac_wep_key
{
    uint8_t array[MAC_WEP_KEY_LEN]; // Key material
};


/// MAC Secret Key
#define MAC_SEC_KEY_LEN         32  // TKIP keys 256 bits (max length) with MIC keys
struct mac_sec_key
{
    uint8_t length;                         // Key material length
    uint32_t array[MAC_SEC_KEY_LEN/4];      // Key material
};

struct mac_country_subband
{
    // First channel number of the triplet.
    uint8_t first_chn;
    // Max number of channel number for the triplet.
    uint8_t nbr_of_chn;
    // Maximum allowed transmit power.
    uint8_t max_tx_power;
};

#define MAX_COUNTRY_LEN         3
#define MAX_COUNTRY_SUBBAND     5
struct mac_country
{
    // Length of the country string
    uint8_t length;
    // Country  string 2 char.
    uint8_t string[MAX_COUNTRY_LEN];
    // channel info triplet
    struct mac_country_subband subband[MAX_COUNTRY_SUBBAND];
};


/// MAC HT CAPABILITY
struct mac_htcapability
{
    uint16_t         ht_capa_info;
    uint8_t          a_mpdu_param;
    uint8_t          mcs_rate[MAX_MCS_LEN];
    uint16_t         ht_extended_capa;
    uint32_t         tx_beamforming_capa;
    uint8_t          asel_capa;
};


/// MAC VHT CAPABILITY
struct mac_vhtcapability
{
    uint32_t         vht_capa_info;
    uint16_t         rx_mcs_map;
    uint16_t         rx_highest;
    uint16_t         tx_mcs_map;
    uint16_t         tx_highest;
};



/// MAC HT CAPABILITY
struct mac_htoprnelmt
{
    uint8_t     prim_channel;
    uint8_t     ht_oper_1;
    uint16_t    ht_oper_2;
    uint16_t    ht_oper_3;
    uint8_t     mcs_rate[MAX_MCS_LEN];

};

/// MAC QOS CAPABILITY
struct mac_qoscapability
{
    uint8_t  qos_info;
};

/// RSN information element
#define MAC_RAW_RSN_IE_LEN 34
struct mac_raw_rsn_ie
{
    uint8_t data[2 + MAC_RAW_RSN_IE_LEN];
};

#define MAC_RAW_ENC_LEN 0x1A
struct mac_wpa_frame
{
    uint8_t array[MAC_RAW_ENC_LEN];
};

#define MAC_WME_PARAM_LEN          16
struct mac_wmm_frame
{
    uint8_t array [MAC_WME_PARAM_LEN];
};

/// BSS load element
struct mac_bss_load
{
    uint16_t sta_cnt;
    uint8_t  ch_utilization;
    uint16_t avail_adm_capacity;
};

///EDCA Parameter Set Element
struct  mac_edca_param_set
{
    uint8_t         qos_info;
    uint8_t         acm;
    uint32_t        ac_param[AC_MAX];
};

/// Protection Status field (Bit indexes, Masks, Offsets)
#define MAC_QOS_INFO_STA_UAPSD_ENABLED_VO      CO_BIT(0)
#define MAC_QOS_INFO_STA_UAPSD_ENABLED_VI      CO_BIT(1)
#define MAC_QOS_INFO_STA_UAPSD_ENABLED_BK      CO_BIT(2)
#define MAC_QOS_INFO_STA_UAPSD_ENABLED_BE      CO_BIT(3)
#define MAC_QOS_INFO_STA_UAPSD_ENABLED_ALL     0x0F

#define MAC_QOS_INFO_AP_UAPSD_ENABLED          CO_BIT(7)

///MAC Twenty Forty BSS

struct mac_twenty_fourty_bss
{
    uint8_t bss_coexistence;
};

/// MAC BA PARAMETERS
struct mac_ba_param
{
    struct mac_addr   peer_sta_address;     /// Peer STA MAC Address to which BA is Setup
    uint16_t          buffer_size;          /// Number of buffers available for this BA
    uint16_t          ssn;                  /// Start Sequence Number of BA
    uint16_t          ba_timeout;           /// BA Setup timeout value
    uint8_t           dev_type;             /// BA Device Type Originator/Responder
    uint8_t           block_ack_policy;     /// BLOCK-ACK Policy Setup Immedaite/Delayed
    uint8_t           buffer_cnt;           /// Number of buffers required for BA Setup
};

/// MAC TS INFO field
struct mac_ts_info
{
    uint8_t   traffic_type;
    uint8_t   ack_policy;
    uint8_t   access_policy;
    uint8_t   dir;
    uint8_t   tsid;
    uint8_t   user_priority;
    bool      aggregation;
    bool      apsd;
    bool      schedule;
};

/// MAC TSPEC PARAMETERS
struct mac_tspec_param
{
    struct mac_ts_info ts_info;
    uint16_t  nominal_msdu_size;
    uint16_t  max_msdu_size;
    uint32_t  min_service_interval;
    uint32_t  max_service_interval;
    uint32_t  inactivity_interval;
    uint32_t  short_inactivity_interval;
    uint32_t  service_start_time;
    uint32_t  max_burst_size;
    uint32_t  min_data_rate;
    uint32_t  mean_data_rate;
    uint32_t  min_phy_rate;
    uint32_t  peak_data_rate;
    uint32_t  delay_bound;
    uint16_t  medium_time;
    uint8_t   surplusbwallowance;
};

/// Scan result element, parsed from beacon or probe response frames.
struct mac_scan_result
{
    /// Network BSSID.
    struct mac_addr bssid;
    /// Network name.
    struct mac_ssid ssid;
    /// Network type (IBSS or ESS).
    uint16_t bsstype;
    /// Network channel number.
    struct scan_chan_tag *chan;
    /// Network beacon period.
    uint16_t beacon_period;
    uint16_t cap_info;
    bool valid_flag;
    int8_t rssi;
};

enum
{
	BSS_QOS_VALID     = CO_BIT(0),
	BSS_HT_VALID      = CO_BIT(1),
	BSS_VHT_VALID     = CO_BIT(2),
	BSS_COUNTRY_VALID = CO_BIT(3),

	/*
	CO_BIT(31)
	warning:  #66-D: enumeration value is out of "int" range
	solution: from CO_BIT(31) to CO_BIT(30)
	*/
	BSS_INFO_VALID    = CO_BIT(30),
};

/// Protection Status field (Bit indexes, Masks, Offsets)
#define MAC_PROT_NONERP_PRESENT_OFT        (0)
#define MAC_PROT_USE_PROTECTION_OFT        (1)
#define MAC_PROT_BARKER_PREAMB_OFT         (2)
#define MAC_PROT_ERP_STATUS_MASK           (0x0007)
#define MAC_PROT_OBSS_NONHT_PRESENT_OFT    (3)
#define MAC_PROT_HT_OPERATION_MODE_OFT     (4)
#define MAC_PROT_HT_OPERATION_MASK         (0x3)
#define MAC_PROT_NON_GF_HT_STA_OFT         (7)
#define MAC_PROT_HT_STATUS_MASK            (0x0078)

/// Station flags
enum
{
    /// Bit indicating that a STA has QoS (WMM) capability
    STA_QOS_CAPA = CO_BIT(0),
    /// Bit indicating that a STA has HT capability
    STA_HT_CAPA = CO_BIT(1),
    /// Bit indicating that a STA has VHT capability
    STA_VHT_CAPA = CO_BIT(2),
    /// Bit indicating that a STA has MFP capability
    STA_MFP_CAPA = CO_BIT(3),
    /// Bit indicating that the STA included the Operation Notification IE
    STA_OPMOD_NOTIF = CO_BIT(4),
};

/// Connection flags
enum
{
    /// Flag indicating whether the control port is controlled by host or not
    CONTROL_PORT_HOST = CO_BIT(0),
    /// Flag indicating whether the control port frame shall be sent unencrypted
    CONTROL_PORT_NO_ENC = CO_BIT(1),
    /// Flag indicating whether HT shall be disabled or not
    DISABLE_HT = CO_BIT(2),
    /// Flag indicating whether WPA or WPA2 authentication is in use
    WPA_WPA2_IN_USE = CO_BIT(3),
    /// Flag indicating whether MFP is in use
    MFP_IN_USE = CO_BIT(4),
};

/// Scan result element, parsed from beacon or probe response frames.
struct mac_sta_info
{
    /// Legacy rate set supported by the STA
    struct mac_rateset rate_set;
    /// HT capabilities
    struct mac_htcapability ht_cap;
    /// VHT capabilities
    struct mac_vhtcapability vht_cap;
    /// Bitfield showing some capabilities of the STA (@ref STA_QOS_CAPA, @ref STA_HT_CAPA,
    /// @ref STA_VHT_CAPA, @ref STA_MFP_CAPA)
    uint32_t capa_flags;
    /// Maximum channel bandwidth supported by the STA
    uint8_t bw_max;
    /// Current channel bandwidth for the STA
    uint8_t bw_cur;
    /// Bit field indicating which queues have to be delivered upon U-APSD trigger
    uint8_t uapsd_queues;
    /// Maximum number of spatial streams supported for STBC reception
    uint8_t stbc_nss;
};

/// Structure containing the information required to perform a measurement request
struct mac_request_set
{

    uint8_t             mode;       ///<As specified by standard
    uint8_t             type;       ///< 0: Basic request, 1: CCA request, 2: RPI histogram request
    uint16_t            duration;   ///< In TU
    uint64_t            start_time; ///< TSF time
    uint8_t             ch_number;  ///< channel to be measured
};

/// Structure containing the information returned from a measurement process
struct mac_report_set
{
    uint8_t             mode;       ///<As specified by standard
    uint8_t             type;       ///< 0: Basic request, 1: CCA request, 2: RPI histogram request
    uint16_t            duration;   ///< In TU
    uint64_t            start_time; ///< TSF time
    uint8_t             ch_number;  ///< channel to be measured
    uint8_t             map;        ///< As specified by standard
    uint8_t             cca_busy_fraction;  ///<As specified by standard
    uint8_t             rpi_histogram[8];   ///<As specified by standard
};

/// Structure containing the MAC SW and MAC HW version information
struct mac_version
{
    char mac_sw_version[16];
    char mac_sw_version_date[48];
    char mac_sw_build_date[48];
    uint32_t mac_hw_version1;
    uint32_t mac_hw_version2;
};

/// Structure containing some of the properties of a BSS. @todo Add required fields during
/// AP/IBSS mode implementation
struct mac_bss_conf
{
    /// Flags (ERP, QoS, etc.).
    uint32_t flags;
    /// Beacon period
    uint16_t beacon_period;
};

/*
* GLOBAL VARIABLES
****************************************************************************************
*/
extern const uint8_t mac_tid2ac[];

extern const uint8_t mac_ac2uapsd[AC_MAX];

extern const uint8_t mac_id2rate[];

extern const uint16_t mac_mcs_params_20[];

extern const uint16_t mac_mcs_params_40[];

extern const struct mac_addr mac_addr_bcst;


/// @}

#endif // _MAC_H_
