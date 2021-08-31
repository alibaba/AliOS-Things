#ifndef GAP_H_
#define GAP_H_
/**
 ****************************************************************************************
 * @addtogroup HOST
 * @ingroup ROOT
 * @brief Bluetooth Low Energy Host
 *
 * The HOST layer of the stack contains the higher layer protocols (@ref ATT "ATT",
 * @ref SMP "SMP") and transport module (@ref L2C "L2C"). It also includes the Generic
 * Access Profile (@ref GAP "GAP"), used for scanning/connection operations.
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @addtogroup GAP Generic Access Profile
 * @ingroup HOST
 * @brief Generic Access Profile.
 *
 * The GAP module is responsible for providing an API to the application in order to
 * configure the device in the desired mode (discoverable, connectable, etc.) and perform
 * required actions (scanning, connection, pairing, etc.). To achieve this, the GAP
 * interfaces with both the @ref SMP "SMP", @ref L2C "L2C" and the @ref CONTROLLER "CONTROLLER"
 *
 * @{
 ****************************************************************************************
 */

#include <stdint.h>
#include "compiler.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// BD address length
#define GAP_BD_ADDR_LEN       (6)
/// LE Channel map length
#define GAP_LE_CHNL_MAP_LEN   (0x05)
/// LE Feature Flags Length
#define GAP_LE_FEATS_LEN      (0x08)
/// ADV Data and Scan Response length
#define GAP_ADV_DATA_LEN      (0x1F)
#define GAP_SCAN_RSP_DATA_LEN (0x1F)
/// Random number length
#define GAP_RAND_NB_LEN       (0x08)
/// Key length
#define GAP_KEY_LEN           (16)
/// P256 Key Len
#define GAP_P256_KEY_LEN      (0x20)


///***** AD Type Flag - Bit set *******/
/// Limited discovery flag - AD Flag
#define GAP_LE_LIM_DISCOVERABLE_FLG             0x01
/// General discovery flag - AD Flag
#define GAP_LE_GEN_DISCOVERABLE_FLG             0x02
/// Legacy BT not supported - AD Flag
#define GAP_BR_EDR_NOT_SUPPORTED                0x04
/// Dual mode for controller supported (BR/EDR/LE) - AD Flag
#define GAP_SIMUL_BR_EDR_LE_CONTROLLER          0x08
/// Dual mode for host supported (BR/EDR/LE) - AD Flag
#define GAP_SIMUL_BR_EDR_LE_HOST                0x10

/*********** GAP Miscellaneous Defines *************/
/// Invalid connection index
#define GAP_INVALID_CONIDX                      0xFF

/// Invalid connection handle
#define GAP_INVALID_CONHDL                      0xFFFF

/// Connection interval min (N*1.250ms)
#define GAP_CNX_INTERVAL_MIN            6       //(0x06)
/// Connection interval Max (N*1.250ms)
#define GAP_CNX_INTERVAL_MAX            3200    //(0xC80)
/// Connection latency min (N*cnx evt)
#define GAP_CNX_LATENCY_MIN             0       //(0x00)
/// Connection latency Max (N*cnx evt
#define GAP_CNX_LATENCY_MAX             499     //(0x1F3)
/// Supervision TO min (N*10ms)
#define GAP_CNX_SUP_TO_MIN              10      //(0x0A)
/// Supervision TO Max (N*10ms)
#define GAP_CNX_SUP_TO_MAX              3200    //(0xC80)



/// Length of resolvable random address prand part
#define GAP_ADDR_PRAND_LEN            (3)
/// Length of resolvable random address hash part
#define GAP_ADDR_HASH_LEN             (3)

/*
 * DEFINES - Optional for BLE application usage
 ****************************************************************************************
 */

/// Central idle timer
/// TGAP(conn_pause_central)
/// recommended value: 1 s: (100 for ke timer)
#define GAP_TMR_CONN_PAUSE_CT                               0x0064

/// Minimum time upon connection establishment before the peripheral
/// starts a connection update procedure: TGAP(conn_pause_peripheral)
/// recommended value: 5 s: (500 for ke timer)
#define GAP_TMR_CONN_PAUSE_PH                               0x01F4

/// Minimum time to perform scanning when user initiated
/// TGAP(scan_fast_period)
/// recommended value: 30.72 s: (3072 for ke timer)
#define GAP_TMR_SCAN_FAST_PERIOD                            0x0C00

/// Minimum time to perform advertising when user initiated
/// TGAP(adv_fast_period)
/// recommended value: 30 s: (3000 for ke timer)
#define GAP_TMR_ADV_FAST_PERIOD                             0x0BB8

/// Scan interval used during Link Layer Scanning State when
/// performing the Limited Discovery procedure
/// TGAP(lim_disc_scan_int)
/// recommended value: 11.25ms; (18 decimal)
#define GAP_LIM_DISC_SCAN_INT                               0x0012

/// Scan interval in any discovery or connection establishment
/// procedure when user initiated: TGAP(scan_fast_interval)
/// recommended value: 30 to 60 ms; N * 0.625
#define GAP_SCAN_FAST_INTV                                  0x0030

/// Scan window in any discovery or connection establishment
/// procedure when user initiated: TGAP(scan_fast_window)
/// recommended value: 30 ms; N * 0.625
#define GAP_SCAN_FAST_WIND                                  0x0030

/// Scan interval in any discovery or connection establishment
/// procedure when background scanning: TGAP(scan_slow_interval1)
/// recommended value: 1.28 s : 0x00CD (205); N * 0.625
#define GAP_SCAN_SLOW_INTV1                                 0x00CD

/// Scan interval in any discovery or connection establishment
/// procedure when background scanning: TGAP(scan_slow_interval2)
/// recommended value: 2.56 s : 0x019A (410); N * 0.625
#define GAP_SCAN_SLOW_INTV2                                 0x019A

/// Scan window in any discovery or connection establishment
/// procedure when background scanning: TGAP(scan_slow_window1)
/// recommended value: 11.25 ms : 0x0012 (18); N * 0.625
#define GAP_SCAN_SLOW_WIND1                                 0x0012

/// Scan window in any discovery or connection establishment
/// procedure when background scanning: TGAP(scan_slow_window2)
/// recommended value: 22.5 ms : 0x0024 (36); N * 0.625
#define GAP_SCAN_SLOW_WIND2                                 0x0024

/// Minimum to maximum advertisement interval in any discoverable
/// or connectable mode when user initiated: TGAP(adv_fast_interval1)
/// recommended value: 30 to 60 ms; N * 0.625
#define GAP_ADV_FAST_INTV1                                  0x0030

/// Minimum to maximum advertisement interval in any discoverable
/// or connectable mode when user initiated: TGAP(adv_fast_interval2)
/// recommended value: 100 to 150 ms; N * 0.625
#define GAP_ADV_FAST_INTV2                                  0x0064

/// Minimum to maximum advertisement interval in any discoverable or
/// connectable mode when background advertising: TGAP(adv_slow_interval)
/// recommended value: 1 to 1.2 s : 0x00B0 (176); N * 0.625
#define GAP_ADV_SLOW_INTV                                   0x00B0

/// Minimum to maximum connection interval upon any connection
/// establishment: TGAP(initial_conn_interval)
/// recommended value: 30 to 50 ms ; N * 1.25 ms
#define GAP_INIT_CONN_MIN_INTV                              0x0018
#define GAP_INIT_CONN_MAX_INTV                              0x0028

/// RW Defines
#define GAP_INQ_SCAN_INTV                                   0x0012
#define GAP_INQ_SCAN_WIND                                   0x0012

/// Connection supervision timeout
/// recommended value: 20s
#define GAP_CONN_SUPERV_TIMEOUT                             0x07D0

/// Minimum connection event
/// default value: 0x0000
#define GAP_CONN_MIN_CE                                     0x0000

/// Maximum connection event
/// default value: 0xFFFF
#define GAP_CONN_MAX_CE                                     0xFFFF

/// Connection latency
/// default value: 0x0000
#define GAP_CONN_LATENCY                                    0x0000

/// GAP Device name Characteristic
/// Default device name
#define GAP_DEV_NAME                                        "RIVIERAWAVES-BLE"

/// GAP Appearance or Icon Characteristic - 2 octets
/// Current appearance value is 0x0000 (unknown appearance)
/// Description:
/// http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
#define GAP_APPEARANCE                                      0x0000

///GAP Peripheral Preferred Connection Parameter - 8 octets
#define GAP_PPCP_CONN_INTV_MAX                              0x0064
#define GAP_PPCP_CONN_INTV_MIN                              0x00C8
#define GAP_PPCP_SLAVE_LATENCY                              0x0000
#define GAP_PPCP_STO_MULT                                   0x07D0

/*
 * Enumerations
 ****************************************************************************************
 */
/// GAP Advertising Flags
enum gap_ad_type
{
    /// Flag
    GAP_AD_TYPE_FLAGS                      = 0x01,
    /// Use of more than 16 bits UUID
    GAP_AD_TYPE_MORE_16_BIT_UUID           = 0x02,
    /// Complete list of 16 bit UUID
    GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID  = 0x03,
    /// Use of more than 32 bit UUD
    GAP_AD_TYPE_MORE_32_BIT_UUID           = 0x04,
    /// Complete list of 32 bit UUID
    GAP_AD_TYPE_COMPLETE_LIST_32_BIT_UUID  = 0x05,
    /// Use of more than 128 bit UUID
    GAP_AD_TYPE_MORE_128_BIT_UUID          = 0x06,
    /// Complete list of 128 bit UUID
    GAP_AD_TYPE_COMPLETE_LIST_128_BIT_UUID = 0x07,
    /// Shortened device name
    GAP_AD_TYPE_SHORTENED_NAME             = 0x08,
    /// Complete device name
    GAP_AD_TYPE_COMPLETE_NAME              = 0x09,
    /// Transmit power
    GAP_AD_TYPE_TRANSMIT_POWER             = 0x0A,
    /// Class of device
    GAP_AD_TYPE_CLASS_OF_DEVICE            = 0x0D,
    /// Simple Pairing Hash C
    GAP_AD_TYPE_SP_HASH_C                  = 0x0E,
    /// Simple Pairing Randomizer
    GAP_AD_TYPE_SP_RANDOMIZER_R            = 0x0F,
    /// Temporary key value
    GAP_AD_TYPE_TK_VALUE                   = 0x10,
    /// Out of Band Flag
    GAP_AD_TYPE_OOB_FLAGS                  = 0x11,
    /// Slave connection interval range
    GAP_AD_TYPE_SLAVE_CONN_INT_RANGE       = 0x12,
    /// Require 16 bit service UUID
    GAP_AD_TYPE_RQRD_16_BIT_SVC_UUID       = 0x14,
    /// Require 32 bit service UUID
    GAP_AD_TYPE_RQRD_32_BIT_SVC_UUID       = 0x1F,
    /// Require 128 bit service UUID
    GAP_AD_TYPE_RQRD_128_BIT_SVC_UUID      = 0x15,
    /// Service data 16-bit UUID
    GAP_AD_TYPE_SERVICE_16_BIT_DATA        = 0x16,
    /// Service data 32-bit UUID
    GAP_AD_TYPE_SERVICE_32_BIT_DATA        = 0x20,
    /// Service data 128-bit UUID
    GAP_AD_TYPE_SERVICE_128_BIT_DATA       = 0x21,
    /// Public Target Address
    GAP_AD_TYPE_PUB_TGT_ADDR               = 0x17,
    /// Random Target Address
    GAP_AD_TYPE_RAND_TGT_ADDR              = 0x18,
    /// Appearance
    GAP_AD_TYPE_APPEARANCE                 = 0x19,
    /// Advertising Interval
    GAP_AD_TYPE_ADV_INTV                   = 0x1A,
    /// LE Bluetooth Device Address
    GAP_AD_TYPE_LE_BT_ADDR                 = 0x1B,
    /// LE Role
    GAP_AD_TYPE_LE_ROLE                    = 0x1C,
    /// Simple Pairing Hash C-256
    GAP_AD_TYPE_SPAIR_HASH                 = 0x1D,
    /// Simple Pairing Randomizer R-256
    GAP_AD_TYPE_SPAIR_RAND                 = 0x1E,
    /// 3D Information Data
    GAP_AD_TYPE_3D_INFO                    = 0x3D,

    /// Manufacturer specific data
    GAP_AD_TYPE_MANU_SPECIFIC_DATA         = 0xFF,
};


/// Random Address type
enum gap_rnd_addr_type
{
    /// Static random address           - 11 (MSB->LSB)
    GAP_STATIC_ADDR     = 0xC0,
    /// Private non resolvable address  - 01 (MSB->LSB)
    GAP_NON_RSLV_ADDR   = 0x00,
    /// Private resolvable address      - 01 (MSB->LSB)
    GAP_RSLV_ADDR       = 0x40,
};

/// Boolean value set
enum
{
    /// Disable
    GAP_DISABLE = 0x00,
    /// Enable
    GAP_ENABLE
};


/// GAP Attribute database handles
/// Generic Access Profile Service
enum
{
    GAP_IDX_PRIM_SVC,
    GAP_IDX_CHAR_DEVNAME,
    GAP_IDX_DEVNAME,
    GAP_IDX_CHAR_ICON,
    GAP_IDX_ICON,
    GAP_IDX_CHAR_SLAVE_PREF_PARAM,
    GAP_IDX_SLAVE_PREF_PARAM,
    GAP_IDX_CHAR_CNT_ADDR_RESOL,
    GAP_IDX_CNT_ADDR_RESOL,
    GAP_IDX_NUMBER
};



/****************** GAP Role **********************/
enum gap_role
{
    /// No role set yet
    GAP_ROLE_NONE        = 0x00,

    /// Observer role
    GAP_ROLE_OBSERVER    = 0x01,

    /// Broadcaster role
    GAP_ROLE_BROADCASTER = 0x02,

    /// Master/Central role
    GAP_ROLE_CENTRAL     = (0x04 | GAP_ROLE_OBSERVER),

    /// Peripheral/Slave role
    GAP_ROLE_PERIPHERAL  = (0x08 | GAP_ROLE_BROADCASTER),

    /// Device has all role, both peripheral and central
    GAP_ROLE_ALL         = (GAP_ROLE_CENTRAL | GAP_ROLE_PERIPHERAL),

    /// Debug mode used to force LL configuration on BLE 4.0
    GAP_ROLE_DBG_LE_4_0      = 0x80,
};

/// Advertising mode
enum gap_adv_mode
{
    /// Mode in non-discoverable
    GAP_NON_DISCOVERABLE,
    /// Mode in general discoverable
    GAP_GEN_DISCOVERABLE,
    /// Mode in limited discoverable
    GAP_LIM_DISCOVERABLE,
    /// Broadcaster mode which is a non discoverable and non connectable mode.
    GAP_BROADCASTER_MODE
};

/// Scan mode
enum gap_scan_mode
{
    /// Mode in general discovery
    GAP_GEN_DISCOVERY,
    /// Mode in limited discovery
    GAP_LIM_DISCOVERY,
    /// Observer mode
    GAP_OBSERVER_MODE,
    /// Invalid mode
    GAP_INVALID_MODE
};



/// IO Capability Values
enum gap_io_cap
{
    /// Display Only
    GAP_IO_CAP_DISPLAY_ONLY = 0x00,
    /// Display Yes No
    GAP_IO_CAP_DISPLAY_YES_NO,
    /// Keyboard Only
    GAP_IO_CAP_KB_ONLY,
    /// No Input No Output
    GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
    /// Keyboard Display
    GAP_IO_CAP_KB_DISPLAY,
    GAP_IO_CAP_LAST
};

/// TK Type
enum gap_tk_type
{
    ///  TK get from out of band method
    GAP_TK_OOB         = 0x00,
    /// TK generated and shall be displayed by local device
    GAP_TK_DISPLAY,
    /// TK shall be entered by user using device keyboard
    GAP_TK_KEY_ENTRY
};

/// OOB Data Present Flag Values
enum gap_oob
{
    /// OOB Data not present
    GAP_OOB_AUTH_DATA_NOT_PRESENT = 0x00,
    /// OOB data present
    GAP_OOB_AUTH_DATA_PRESENT,
    GAP_OOB_AUTH_DATA_LAST
};

/// Authentication mask
enum gap_auth_mask
{
    /// No Flag set
    GAP_AUTH_NONE    = 0,
    /// Bond authentication
    GAP_AUTH_BOND    = (1 << 0),
    /// Man In the middle protection
    GAP_AUTH_MITM    = (1 << 2),
    /// Secure Connection
    GAP_AUTH_SEC_CON = (1 << 3),
    /// Key Notification
    GAP_AUTH_KEY_NOTIF = (1 << 4)
};

/// Security Link Level
enum gap_lk_sec_lvl
{
    /// No authentication
    GAP_LK_NO_AUTH             = 0,
    /// Unauthenticated link
    GAP_LK_UNAUTH,
    /// Authenticated link
    GAP_LK_AUTH,
    /// Secure Connection link
    GAP_LK_SEC_CON,
};

/// Authentication Requirements
enum gap_auth
{
    /// No MITM No Bonding
    GAP_AUTH_REQ_NO_MITM_NO_BOND  = (GAP_AUTH_NONE),
    /// No MITM Bonding
    GAP_AUTH_REQ_NO_MITM_BOND     = (GAP_AUTH_BOND),
    /// MITM No Bonding
    GAP_AUTH_REQ_MITM_NO_BOND     = (GAP_AUTH_MITM),
    /// MITM and Bonding
    GAP_AUTH_REQ_MITM_BOND        = (GAP_AUTH_MITM | GAP_AUTH_BOND),
    /// SEC_CON and No Bonding
    GAP_AUTH_REQ_SEC_CON_NO_BOND  = (GAP_AUTH_SEC_CON),
    /// SEC_CON and Bonding
    GAP_AUTH_REQ_SEC_CON_BOND     = (GAP_AUTH_SEC_CON | GAP_AUTH_BOND),

    GAP_AUTH_REQ_LAST,

    /// Mask of  authentication features without reserved flag
    GAP_AUTH_REQ_MASK             = 0x1F,
};

/// Key Distribution Flags
enum gap_kdist
{
    /// No Keys to distribute
    GAP_KDIST_NONE = 0x00,
    /// Encryption key in distribution
    GAP_KDIST_ENCKEY = (1 << 0),
    /// IRK (ID key)in distribution
    GAP_KDIST_IDKEY  = (1 << 1),
    /// CSRK(Signature key) in distribution
    GAP_KDIST_SIGNKEY= (1 << 2),
    /// LTK in distribution
    GAP_KDIST_LINKKEY=   (1 << 3),

    GAP_KDIST_LAST =   (1 << 4)
};

/// Security Defines
enum gap_sec_req
{
    /// No security (no authentication and encryption)
    GAP_NO_SEC = 0x00,
    /// Unauthenticated pairing with encryption
    GAP_SEC1_NOAUTH_PAIR_ENC,
    /// Authenticated pairing with encryption
    GAP_SEC1_AUTH_PAIR_ENC,
    /// Unauthenticated pairing with data signing
    GAP_SEC2_NOAUTH_DATA_SGN,
    /// Authentication pairing with data signing
    GAP_SEC2_AUTH_DATA_SGN,
    /// Secure Connection pairing with encryption
    GAP_SEC1_SEC_CON_PAIR_ENC,
};

/// Bit field use to select the preferred TX or RX LE PHY Rate. 0 means no preferences
enum gap_rate
{
    /// No preferred rate
    GAP_RATE_ANY               = 0x00,
    /// LE PHY 1mb/s preferred rate for an active link
    GAP_RATE_LE_1MBPS          = (1 << 0),
    /// LE PHY 2mb/s preferred rate for an active link
    GAP_RATE_LE_2MBPS          = (1 << 1),
};

/// API settings bit field values
enum gap_api_settings
{
    /// Use deprecated API and do not use extended air operations
    GAP_ADD_INFO_USE_DEPRECATED_API_BIT = (1 << 0),
};

/// Privacy configuration
enum gap_priv_cfg
{
    /// Indicate if public or static private random address must be used by default
    GAP_PRIV_CFG_PRIV_ADDR_BIT = (1 << 0),
    /// Bit 1 is reserved
    GAP_PRIV_CFG_RSVD          = (1 << 1),
    /// Indicate if controller privacy is enabled
    GAP_PRIV_CFG_CTNL_PRIV_BIT = (1 << 2),
};

/// Type of own address
enum gap_own_addr_type
{
    /// Use address configured using GAPM_SET_DEV_CONFIG_CMD.
    /// Can be either a public address or a random static address
    GAP_OWN_ADDR_TYPE_DEFAULT = 0,
    /// Use a non-resolvable private address
    GAP_OWN_ADDR_TYPE_NRPA,
    /// Use a resolvable private address
    GAP_OWN_ADDR_TYPE_RPA,
};

/// Type of activities that can be created
enum gap_actv_type
{
    /// Advertising activity
    GAP_ACTV_TYPE_ADV = 0,
    /// Scanning activity
    GAP_ACTV_TYPE_SCAN,
    /// Initiating activity
    GAP_ACTV_TYPE_INIT,
    /// Periodic synchronization activity
    GAP_ACTV_TYPE_PERIOD_SYNC,
};

/// Type of advertising that can be created
enum gap_adv_type
{
    /// Legacy advertising
    GAP_ADV_TYPE_LEGACY = 0,
    /// Extended advertising
    GAP_ADV_TYPE_EXTENDED,
    /// Periodic advertising
    GAP_ADV_TYPE_PERIODIC,
};

/// Advertising report type
enum gap_adv_report_type
{
    /// Extended advertising report
    GAP_REPORT_TYPE_ADV_EXT = 0,
    /// Legacy advertising report
    GAP_REPORT_TYPE_ADV_LEG,
    /// Extended scan response report
    GAP_REPORT_TYPE_SCAN_RSP_EXT,
    /// Legacy scan response report
    GAP_REPORT_TYPE_SCAN_RSP_LEG,
    /// Periodic advertising report
    GAP_REPORT_TYPE_PER_ADV,
};

/// Advertising properties bit field bit value
enum gap_adv_prop
{
    /// Indicate that advertising is connectable, reception of CONNECT_REQ or AUX_CONNECT_REQ
    /// PDUs is accepted. Not applicable for periodic advertising.
    GAP_ADV_PROP_CONNECTABLE_BIT     = (1 << 0),
    /// Indicate that advertising is scannable, reception of SCAN_REQ or AUX_SCAN_REQ PDUs is
    /// accepted
    GAP_ADV_PROP_SCANNABLE_BIT       = (1 << 1),
    /// Indicate that advertising targets a specific device. Only apply in following cases:
    ///   - Legacy advertising: if connectable
    ///   - Extended advertising: connectable or (non connectable and non discoverable)
    GAP_ADV_PROP_DIRECTED_BIT        = (1 << 2),
    /// Indicate that High Duty Cycle has to be used for advertising on primary channel
    /// Apply only if created advertising is not an extended advertising
    GAP_ADV_PROP_HDC_BIT             = (1 << 3),
    /// Bit 4 is reserved
    GAP_ADV_PROP_RESERVED_4_BIT      = (1 << 4),
    /// Enable anonymous mode. Device address won't appear in send PDUs
    /// Valid only if created advertising is an extended advertising
    GAP_ADV_PROP_ANONYMOUS_BIT       = (1 << 5),
    /// Include TX Power in the extended header of the advertising PDU.
    /// Valid only if created advertising is not a legacy advertising
    GAP_ADV_PROP_TX_PWR_BIT          = (1 << 6),
    /// Include TX Power in the periodic advertising PDU.
    /// Valid only if created advertising is a periodic advertising
    GAP_ADV_PROP_PER_TX_PWR_BIT      = (1 << 7),
    /// Indicate if application must be informed about received scan requests PDUs
    GAP_ADV_PROP_SCAN_REQ_NTF_EN_BIT = (1 << 8),
};

/// PHY Type
enum gap_phy_type
{
    /// LE 1M
    GAP_PHY_TYPE_LE_1M = 0,
    /// LE 2M
    GAP_PHY_TYPE_LE_2M,
    /// LE Coded
    GAP_PHY_TYPE_LE_CODED,
};

/// Advertising discovery mode
enum gap_adv_disc_mode
{
    /// Mode in non-discoverable
    GAP_ADV_MODE_NON_DISC = 0,
    /// Mode in general discoverable
    GAP_ADV_MODE_GEN_DISC,
    /// Mode in limited discoverable
    GAP_ADV_MODE_LIM_DISC,
};

/// Scanning Types
enum gap_scan_type
{
    /// General discovery
    GAP_SCAN_TYPE_GEN_DISC = 0,
    /// Limited discovery
    GAP_SCAN_TYPE_LIM_DISC,
    /// Observer
    GAP_SCAN_TYPE_OBSERVER,
    /// Selective observer
    GAP_SCAN_TYPE_SEL_OBSERVER,
    /// Connectable discovery
    GAP_SCAN_TYPE_CONN_DISC,
    /// Selective connectable discovery
    GAP_SCAN_TYPE_SEL_CONN_DISC,
};

/// Scanning properties bit field bit value
enum gap_scan_prop
{
    /// Scan advertisement on the LE 1M PHY
    GAP_SCAN_PROP_PHY_1M_BIT       = (1 << 0),
    /// Scan advertisement on the LE Coded PHY
    GAP_SCAN_PROP_PHY_CODED_BIT    = (1 << 1),
    /// Active scan on LE 1M PHY (Scan Request PDUs may be sent)
    GAP_SCAN_PROP_ACTIVE_1M_BIT    = (1 << 2),
    /// Active scan on LE Coded PHY (Scan Request PDUs may be sent)
    GAP_SCAN_PROP_ACTIVE_CODED_BIT = (1 << 3),
    /// Accept directed advertising packets if we use a RPA and target address cannot be solved by the
    /// controller
    GAP_SCAN_PROP_ACCEPT_RPA_BIT   = (1 << 4),
    /// Filter truncated advertising or scan response reports
    GAP_SCAN_PROP_FILT_TRUNC_BIT   = (1 << 5),
};

/// Initiating Types
enum gap_init_type
{
    /// Direct connection establishment, establish a connection with an indicated device
    GAPM_INIT_TYPE_DIRECT_CONN_EST = 0,
    /// Automatic connection establishment, establish a connection with all devices whose address is
    /// present in the white list
    GAPM_INIT_TYPE_AUTO_CONN_EST,
    /// Name discovery, Establish a connection with an indicated device in order to read content of its
    /// Device Name characteristic. Connection is closed once this operation is stopped.
    GAPM_INIT_TYPE_NAME_DISC,
};

/// Initiating Properties
enum gap_init_prop
{
    /// Scan connectable advertisements on the LE 1M PHY. Connection parameters for the LE 1M PHY are provided
    GAPM_INIT_PROP_1M_BIT       = (1 << 0),
    /// Connection parameters for the LE 2M PHY are provided
    GAPM_INIT_PROP_2M_BIT       = (1 << 1),
    /// Scan connectable advertisements on the LE Coded PHY. Connection parameters for the LE Coded PHY are provided
    GAPM_INIT_PROP_CODED_BIT    = (1 << 2),
};

/// Advertising report information
enum gap_adv_report_info
{
    /// Report Type
    GAP_REPORT_INFO_REPORT_TYPE         = 0x07,
    /// Report is complete
    GAP_REPORT_INFO_COMPLETE            = (1 << 3),
    /// Connectable advertising
    GAP_REPORT_INFO_CONN_ADV            = (1 << 4),
    /// Scannable advertising
    GAP_REPORT_INFO_SCAN_ADV            = (1 << 5),
    /// Directed advertising
    GAP_REPORT_INFO_DIR_ADV             = (1 << 6),
};

/// Filtering policy for duplicated packets
enum gap_dup_filter_pol
{
    /// Disable filtering of duplicated packets
    GAP_DUP_FILT_DIS = 0,
    /// Enable filtering of duplicated packets
    GAP_DUP_FILT_EN,
    /// Enable filtering of duplicated packets, reset for each scan period
    GAP_DUP_FILT_EN_PERIOD,
};

/*
 * Masks for advertising properties
 ****************************************************************************************
 */

/// Advertising properties configurations for legacy advertising
enum gap_leg_adv_prop
{
    /// Non connectable non scannable advertising
    GAP_ADV_PROP_NON_CONN_NON_SCAN_MASK  = 0x0000,
    /// Broadcast non scannable advertising - Discovery mode must be Non Discoverable
    GAP_ADV_PROP_BROADCAST_NON_SCAN_MASK = GAP_ADV_PROP_NON_CONN_NON_SCAN_MASK,
    /// Non connectable scannable advertising
    GAP_ADV_PROP_NON_CONN_SCAN_MASK      = GAP_ADV_PROP_SCANNABLE_BIT,
    /// Broadcast non scannable advertising - Discovery mode must be Non Discoverable
    GAP_ADV_PROP_BROADCAST_SCAN_MASK     = GAP_ADV_PROP_NON_CONN_SCAN_MASK,
    /// Undirected connectable advertising
    GAP_ADV_PROP_UNDIR_CONN_MASK         = GAP_ADV_PROP_CONNECTABLE_BIT | GAP_ADV_PROP_SCANNABLE_BIT,
    /// Directed connectable advertising
    GAP_ADV_PROP_DIR_CONN_MASK           = GAP_ADV_PROP_DIRECTED_BIT | GAP_ADV_PROP_CONNECTABLE_BIT,
    /// Directed connectable with Low Duty Cycle
    GAP_ADV_PROP_DIR_CONN_LDC_MASK       = GAP_ADV_PROP_DIR_CONN_MASK,
    /// Directed connectable with High Duty Cycle
    GAP_ADV_PROP_DIR_CONN_HDC_MASK       = GAP_ADV_PROP_DIR_CONN_MASK | GAP_ADV_PROP_HDC_BIT,
};

/// Advertising properties configurations for extended advertising
enum gap_ext_adv_prop
{
    /// Non connectable non scannable extended advertising
    GAP_EXT_ADV_PROP_NON_CONN_NON_SCAN_MASK = 0x0000,
    /// Non connectable scannable extended advertising
    GAP_EXT_ADV_PROP_NON_CONN_SCAN_MASK     = GAP_ADV_PROP_SCANNABLE_BIT,
    /// Undirected connectable extended advertising
    GAP_EXT_ADV_PROP_UNDIR_CONN_MASK        = GAP_ADV_PROP_CONNECTABLE_BIT,
    /// Directed connectable extended advertising
    GAP_EXT_ADV_PROP_DIR_CONN_MASK          = GAP_ADV_PROP_CONNECTABLE_BIT | GAP_ADV_PROP_DIRECTED_BIT,
};

/*************** GAP Structures ********************/

/// Device name
struct gap_dev_name
{
    /// name length
    uint16_t length;
    /// name value
    uint8_t value[__ARRAY_EMPTY];
};

/// Slave preferred connection parameters
struct gap_slv_pref
{
    /// Connection interval minimum
    uint16_t con_intv_min;
    /// Connection interval maximum
    uint16_t con_intv_max;
    /// Slave latency
    uint16_t slave_latency;
    /// Connection supervision timeout multiplier
    uint16_t conn_timeout;
};

///BD Address structure
#define BLE_BD_ADDR_T
typedef struct
{
    ///6-byte array address value
    uint8_t  addr[GAP_BD_ADDR_LEN];
} bd_addr_t;

///Channel map structure
typedef struct
{
    ///5-byte channel map array
    uint8_t map[GAP_LE_CHNL_MAP_LEN];
} le_chnl_map_t;


///Random number structure
typedef struct
{
    ///8-byte array for random number
    uint8_t     nb[GAP_RAND_NB_LEN];
} rand_nb_t;

///Advertising report structure
typedef struct
{
    ///Event type:
    /// - ADV_CONN_UNDIR: Connectable Undirected advertising
    /// - ADV_CONN_DIR: Connectable directed advertising
    /// - ADV_DISC_UNDIR: Discoverable undirected advertising
    /// - ADV_NONCONN_UNDIR: Non-connectable undirected advertising
    uint8_t        evt_type;
    ///Advertising address type: public/random
    uint8_t        adv_addr_type;
    ///Advertising address value
    bd_addr_t      adv_addr;
    ///Data length in advertising packet
    uint8_t        data_len;
    ///Data of advertising packet
    uint8_t        data[GAP_ADV_DATA_LEN];
    ///RSSI value for advertising packet
    uint8_t        rssi;
} adv_report_t;


/// P256 Public key data format
typedef struct
{
    /// X Coordinate of the key
    uint8_t x[GAP_P256_KEY_LEN];
    /// X Coordinate of the key
    uint8_t y[GAP_P256_KEY_LEN];
} public_key_t;
/// P256 private key data format
typedef struct
{
    ///MSB->LSB
    uint8_t Secrt_key[GAP_P256_KEY_LEN];
} private_key_t;



/// Address information about a device address
struct gap_bdaddr
{
    /// BD Address of device
    bd_addr_t addr;
    /// Address type of the device 0=public/1=private random
    uint8_t addr_type;
};

/// Resolving list device information
struct gap_ral_dev_info
{
    /// Address type of the device 0=public/1=private random
    uint8_t addr_type;
    /// BD Address of device
    bd_addr_t addr;
    /// Peer IRK
    uint8_t peer_irk[GAP_KEY_LEN];
    /// Local IRK
    uint8_t local_irk[GAP_KEY_LEN];
};

/// Generic Security key structure
struct gap_sec_key
{
    /// Key value MSB -> LSB
    uint8_t key[GAP_KEY_LEN];
};

/// Configuration for advertising on primary channel
struct gap_adv_prim_cfg
{
    /// Minimum advertising interval (in unit of 625us). Must be greater than 20ms
    uint32_t adv_intv_min;
    /// Maximum advertising interval (in unit of 625us). Must be greater than 20ms
    uint32_t adv_intv_max;
    /// Bit field indicating the channel mapping
    uint8_t chnl_map;
    /// Indicate on which PHY primary advertising has to be performed (@see enum gap_phy_type)
    /// Note that LE 2M PHY is not allowed and that legacy advertising only support LE 1M PHY
    uint8_t phy;
};

/// Configuration for advertising on secondary channel
struct gap_adv_second_cfg
{
    /// Maximum number of advertising events the controller can skip before sending the
    /// AUX_ADV_IND packets. 0 means that AUX_ADV_IND PDUs shall be sent prior each
    /// advertising events
    uint8_t max_skip;
    /// Indicate on which PHY secondary advertising has to be performed (@see enum gap_phy_type)
    uint8_t phy;
    /// Advertising SID
    uint8_t adv_sid;
};

/// Configuration for periodic advertising
struct gap_adv_period_cfg
{
    /// Minimum advertising interval (in unit of 1.25ms). Must be greater than 20ms
    uint16_t adv_intv_min;
    /// Maximum advertising interval (in unit of 1.25ms). Must be greater than 20ms
    uint16_t adv_intv_max;
};

/// Advertising parameters
struct gap_adv_param
{
    /// Advertising type (@see enum gap_adv_type)
    uint8_t type;
    /// Discovery mode (@see enum gap_adv_mode)
    uint8_t disc_mode;
    /// Bit field value provided advertising properties (@see enum gap_adv_prop for bit signification)
    uint16_t prop;
    /// Maximum power level at which the advertising packets have to be transmitted
    /// (between -127 and 126 dBm)
    int8_t max_tx_pwr;
    /// Advertising filtering policy
    uint8_t filter_pol;
    /// Peer address configuration (only used in case of directed advertising)
    struct gap_bdaddr peer_addr;
    /// Configuration for primary advertising
    struct gap_adv_prim_cfg prim_cfg;
    /// Configuration for secondary advertising (valid only if advertising type is
    /// GAPM_ADV_TYPE_EXTENDED or GAPM_ADV_TYPE_PERIODIC)
    struct gap_adv_second_cfg second_cfg;
    /// Configuration for periodic advertising (valid only if advertising type os
    /// GAPM_ADV_TYPE_PERIODIC)
    struct gap_adv_period_cfg period_cfg;
};

/// Additional advertising parameters
struct gap_adv_add_param
{
    /// Advertising duration (in unit of 10ms). 0 means that advertising continues
    /// until the host disable it
    uint16_t duration;
    /// Maximum number of extended advertising events the controller shall attempt to send prior to
    /// terminating the extending advertising
    /// Valid only if extended advertising
    uint8_t max_adv_evt;
};

/// Scan Window operation parameters
struct gap_scan_wd_op_param
{
    /// Scan interval
    uint16_t scan_intv;
    /// Scan window
    uint16_t scan_wd;
};

/// Scanning parameters
struct gap_scan_param
{
    /// Type of scanning to be started (@see enum gap_scan_type)
    uint8_t type;
    /// Properties for the scan procedure (@see enum gap_scan_prop for bit signification)
    uint8_t prop;
    /// Duplicate packet filtering policy
    uint8_t dup_filt_pol;
    /// Reserved for future use
    uint8_t rsvd;
    /// Scan window opening parameters for LE 1M PHY
    struct gap_scan_wd_op_param scan_param_1m;
    /// Scan window opening parameters for LE Coded PHY
    struct gap_scan_wd_op_param scan_param_coded;
    /// Scan duration (in unit of 10ms). 0 means that the controller will scan continuously until
    /// reception of a stop command from the application
    uint16_t duration;
    /// Scan period (in unit of 1.28s). Time interval betweem two consequent starts of a scan duration
    /// by the controller. 0 means that the scan procedure is not periodic
    uint16_t period;
};

/// Connection parameters
struct gap_conn_param
{
    /// Minimum value for the connection interval (in unit of 1.25ms). Shall be less than or equal to
    /// conn_intv_max value. Allowed range is 7.5ms to 4s.
    uint16_t conn_intv_min;
    /// Maximum value for the connection interval (in unit of 1.25ms). Shall be greater than or equal to
    /// conn_intv_min value. Allowed range is 7.5ms to 4s.
    uint16_t conn_intv_max;
    /// Slave latency. Number of events that can be missed by a connected slave device
    uint16_t conn_latency;
    /// Link supervision timeout (in unit of 10ms). Allowed range is 100ms to 32s
    uint16_t supervision_to;
    /// Recommended minimum duration of connection events (in unit of 625us)
    uint16_t ce_len_min;
    /// Recommended maximum duration of connection events (in unit of 625us)
    uint16_t ce_len_max;
};

/// Initiating parameters
struct gap_init_param
{
    /// Initiating type (@see enum gap_init_type)
    uint8_t type;
    /// Properties for the initiating procedure (@see enum gap_init_prop for bit signification)
    uint8_t prop;
    /// Timeout for automatic connection establishment (in unit of 10ms). Cancel the procedure if not all
    /// indicated devices have been connected when the timeout occurs. 0 means there is no timeout
    uint16_t conn_to;
    /// Scan window opening parameters for LE 1M PHY
    struct gap_scan_wd_op_param scan_param_1m;
    /// Scan window opening parameters for LE Coded PHY
    struct gap_scan_wd_op_param scan_param_coded;
    /// Connection parameters for LE 1M PHY
    struct gap_conn_param conn_param_1m;
    /// Connection parameters for LE 2M PHY
    struct gap_conn_param conn_param_2m;
    /// Connection parameters for LE Coded PHY
    struct gap_conn_param conn_param_coded;
    /// Address of peer device in case white list is not used for connection
    struct gap_bdaddr peer_addr;
};

/// Periodic advertising information
struct gap_period_adv_addr_cfg
{
    /// Advertiser address information
    struct gap_bdaddr addr;
    /// Advertising SID
    uint8_t adv_sid;
};

/// Periodic synchronization parameters
struct gap_period_sync_param
{
    /// Number of periodic advertising that can be skipped after a successful receive. Maximum authorized
    /// value is 499
    uint16_t skip;
    /// Synchronization timeout for the periodic advertising (in unit of 10ms between 100ms and 163.84s)
    uint16_t sync_to;
    /// Indicate if Periodic Advertiser List has to be used to determine which advertiser to listen to, else
    /// provided advertiser information are used
    uint8_t use_pal;
    /// Reserved for future used
    uint8_t rsvd;
    /// Address of advertiser with which synchronization has to be established (used only if use_pal is false)
    struct gap_period_adv_addr_cfg adv_addr;
};

/// @} GAP
#endif // GAP_H_
