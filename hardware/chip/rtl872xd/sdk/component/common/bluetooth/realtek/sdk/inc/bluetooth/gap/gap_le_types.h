/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_le_types.h
  * @brief   This file contains all the constants and functions prototypes for GAP protocol.
  * @details This file is used both bredr and le.
  * @author  jane
  * @date    2016-02-18
  * @version v1.0
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_LE_TYPES_H
#define GAP_LE_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#include <bt_types.h>
#include <stdint.h>
#include <stdbool.h>

/** @addtogroup GAP_LE_TYPES GAP LE Related Definitions
  * @brief Defines GAP related definitions.
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @addtogroup GAP_LE_TYPES_Exported_Macros
  * @{
  */
#define GAP_LE_SUPPORTED_FEATURES_LEN           8 //!< LE supported feature length.

/** @defgroup RANDOM_ADDR_MASK Random Address Mask
 * @{
 */
#define RANDOM_ADDR_MASK_STATIC                 0xC0 /**< Random address mask for static random address  */
#define RANDOM_ADDR_MASK_RESOLVABLE             0x40 /**< Random address mask for resolvable random address  */
#define RANDOM_ADDR_MASK_NON_RESOLVABLE         0x00 /**< Random address mask for non-resolvable random address  */
#define RANDOM_ADDR_MASK                        0xC0 /**< Mask for random address */
/** End of RANDOM_ADDR_MASK
  * @}
  */

/** @defgroup ADV_CHANNEL_MAP Advertising Channel Map
 * @{
 */
#define GAP_ADVCHAN_37                          0x01  //!< Advertisement Channel 37
#define GAP_ADVCHAN_38                          0x02  //!< Advertisement Channel 38
#define GAP_ADVCHAN_39                          0x04  //!< Advertisement Channel 39
#define GAP_ADVCHAN_ALL (GAP_ADVCHAN_37 | GAP_ADVCHAN_38 | GAP_ADVCHAN_39) //!< All Advertisement Channels Enabled
/** End of ADV_CHANNEL_MAP
  * @}
  */


/** @defgroup ADV_DATA_TYPE EIR Data Type, Advertising Data Type (AD Type) and OOB Data Type Definitions
 * @{
 */
#define GAP_ADTYPE_FLAGS                        0x01 //!< The Flags data type contains one bit Boolean flags. Please reference @ref ADV_TYPE_FLAGS for details.
#define GAP_ADTYPE_16BIT_MORE                   0x02 //!< Service: More 16-bit UUIDs available
#define GAP_ADTYPE_16BIT_COMPLETE               0x03 //!< Service: Complete list of 16-bit UUIDs
#define GAP_ADTYPE_32BIT_MORE                   0x04 //!< Service: More 32-bit UUIDs available
#define GAP_ADTYPE_32BIT_COMPLETE               0x05 //!< Service: Complete list of 32-bit UUIDs
#define GAP_ADTYPE_128BIT_MORE                  0x06 //!< Service: More 128-bit UUIDs available
#define GAP_ADTYPE_128BIT_COMPLETE              0x07 //!< Service: Complete list of 128-bit UUIDs
#define GAP_ADTYPE_LOCAL_NAME_SHORT             0x08 //!< Shortened local name
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09 //!< Complete local name
#define GAP_ADTYPE_POWER_LEVEL                  0x0A //!< TX Power Level: 0xXX: -127 to +127 dBm
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE          0x0D //!< Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC     0x0E //!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR     0x0F //!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADTYPE_SM_TK                        0x10 //!< Security Manager TK Value
#define GAP_ADTYPE_SM_OOB_FLAG                  0x11 //!< Secutiry Manager OOB Flags
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE    0x12 //!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
#define GAP_ADTYPE_SIGNED_DATA                  0x13 //!< Signed Data field
#define GAP_ADTYPE_SERVICES_LIST_16BIT          0x14 //!< Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_128BIT         0x15 //!< Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADTYPE_SERVICE_DATA                 0x16 //!< Service Data
#define GAP_ADTYPE_PUBLIC_TGT_ADDR              0x17 //!< Public Target Address
#define GAP_ADTYPE_RANDOM_TGT_ADDR              0x18 //!< Random Target Address
#define GAP_ADTYPE_APPEARANCE                   0x19 //!< Appearance
#define GAP_ADTYPE_ADV_INTERVAL                 0x1A //!< Advertising Interval
#define GAP_ADTYPE_LE_BT_ADDR                   0x1B //!< LE Bluetooth Device Address
#define GAP_ADTYPE_LE_ROLE                      0x1C //!< LE Role
#define GAP_ADTYPE_SP_HASH_C256                 0x1D //!< Simple Pairing Hash C-256
#define GAP_ADTYPE_SP_RAND_R256                 0x1E //!< Simple Pairing Randomizer R-256
#define GAP_ADTYPE_LIST_32BIT_SILI              0x1F //!< List of 32-bit Service Solicitation UUIDs
#define GAP_ADTYPE_SERVICE_DATA_32BIT           0x20 //!< Service Data - 32-bit UUID
#define GAP_ADTYPE_SERVICE_DATA_128BIT          0x21 //!< Service Data - 128-bit UUID
#define GAP_ADTYPE_SC_CONF_VALUE                0x22 //!< LE Secure Connections Confirmation Value
#define GAP_ADTYPE_SC_RAND_VALUE                0x23 //!< LE Secure Connections Random Value
#define GAP_ADTYPE_URI                          0x24 //!< URI
#define GAP_ADTYPE_INDOOR_POSITION              0x25 //!< Indoor Positioning
#define GAP_ADTYPE_TRANSPORT_DISCOVERY_DATA     0x26 //!< Transport Discovery Data
#define GAP_ADTYPE_LE_SUPPORTED_FEATURES        0x27 //!< LE Supported Features
#define GAP_ADTYPE_CHAN_MAP_UPDATE_IND          0x28 //!< Channel Map Update Indication
#define GAP_ADTYPE_MESH_PB_ADV                  0x29 //!< Mesh Pb-Adv
#define GAP_ADTYPE_MESH_PACKET                  0x2A //!< Mesh Packet
#define GAP_ADTYPE_MESH_BEACON                  0x2B //!< Mesh Beacon
#define GAP_ADTYPE_3D_INFO_DATA                 0x3D //!< 3D Information Data
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC        0xFF //!< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data
/** End of ADV_DATA_TYPE
  * @}
  */

/** @defgroup ADV_TYPE_FLAGS The Flags data type
 * @{
 */
#define GAP_ADTYPE_FLAGS_LIMITED                            0x01 //!< Discovery Mode: LE Limited Discoverable Mode
#define GAP_ADTYPE_FLAGS_GENERAL                            0x02 //!< Discovery Mode: LE General Discoverable Mode
#define GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED                0x04 //!< Discovery Mode: BR/EDR Not Supported
#define GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_CONTROLLER   0x08 //!< Discovery Mode: Simultaneous LE and BR/EDR Controller Supported
#define GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_HOST         0x10 //!< Discovery Mode: Simultaneous LE and BR/EDR Host Supported
/** @} End ADV_TYPE_FLAGS */

/** @defgroup GAP_LE_APPEARANCE_VALUES GAP Appearance Values
 * @{
 */
#define GAP_GATT_APPEARANCE_UNKNOWN                                0
#define GAP_GATT_APPEARANCE_GENERIC_PHONE                          64
#define GAP_GATT_APPEARANCE_GENERIC_COMPUTER                       128
#define GAP_GATT_APPEARANCE_GENERIC_WATCH                          192
#define GAP_GATT_APPEARANCE_WATCH_SPORTS_WATCH                     193

#define GAP_GATT_APPEARANCE_GENERIC_CLOCK                          256
#define GAP_GATT_APPEARANCE_GENERIC_DISPLAY                        320
#define GAP_GATT_APPEARANCE_GENERIC_REMOTE_CONTROL                 384
#define GAP_GATT_APPEARANCE_GENERIC_EYE_GLASSES                    448
#define GAP_GATT_APPEARANCE_GENERIC_TAG                            512
#define GAP_GATT_APPEARANCE_GENERIC_KEYRING                        576
#define GAP_GATT_APPEARANCE_GENERIC_MEDIA_PLAYER                   640
#define GAP_GATT_APPEARANCE_GENERIC_BARCODE_SCANNER                704

#define GAP_GATT_APPEARANCE_GENERIC_THERMOMETER                    768
#define GAP_GATT_APPEARANCE_THERMOMETER_EAR                        769

#define GAP_GATT_APPEARANCE_GENERIC_HEART_RATE_SENSOR              832
#define GAP_GATT_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT      833

#define GAP_GATT_APPEARANCE_GENERIC_BLOOD_PRESSURE                 896
#define GAP_GATT_APPEARANCE_BLOOD_PRESSURE_ARM                     897
#define GAP_GATT_APPEARANCE_BLOOD_PRESSURE_WRIST                   898

#define GAP_GATT_APPEARANCE_HUMAN_INTERFACE_DEVICE                 960
#define GAP_GATT_APPEARANCE_KEYBOARD                               961
#define GAP_GATT_APPEARANCE_MOUSE                                  962
#define GAP_GATT_APPEARANCE_JOYSTICK                               963
#define GAP_GATT_APPEARANCE_GAMEPAD                                964
#define GAP_GATT_APPEARANCE_DIGITIZER_TABLET                       965
#define GAP_GATT_APPEARANCE_CARD_READER                            966
#define GAP_GATT_APPEARANCE_DIGITAL_PEN                            967
#define GAP_GATT_APPEARANCE_BARCODE_SCANNER                        968

#define GAP_GATT_APPEARANCE_GENERIC_GLUCOSE_METER                  1024

#define GAP_GATT_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR         1088
#define GAP_GATT_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE         1089
#define GAP_GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE         1090
#define GAP_GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP          1091

#define GAP_GATT_APPEARANCE_GENERIC_CYCLING                        1152
#define GAP_GATT_APPEARANCE_CYCLING_CYCLING_COMPUTER               1153
#define GAP_GATT_APPEARANCE_CYCLING_SPEED_SENSOR                   1154
#define GAP_GATT_APPEARANCE_CYCLING_CADENCE_SENSOR                 1155
#define GAP_GATT_APPEARANCE_CYCLING_POWER_SENSOR                   1156
#define GAP_GATT_APPEARANCE_CYCLING_SPEED_AND_CADENCE_SENSOR       1157

#define GAP_GATT_APPEARANCE_GENERIC_PULSE_OXIMETER                 3136
#define GAP_GATT_APPEARANCE_FINGERTIP                              3137
#define GAP_GATT_APPEARANCE_WRIST_WORN                             3138
#define GAP_GATT_APPEARANCE_GENERIC_WEIGHT_SCALE                   3200

#define GAP_GATT_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACTIVITY        5184
#define GAP_GATT_APPEARANCE_LOCATION_DISPLAY_DEVICE                5185
#define GAP_GATT_APPEARANCE_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE 5186
#define GAP_GATT_APPEARANCE_LOCATION_POD                           5187
#define GAP_GATT_APPEARANCE_LOCATION_AND_NAVIGATION_POD            5188
/** @} End GAP_LE_APPEARANCE_VALUES */

#if F_BT_LE_5_0_SET_PHYS_SUPPORT
#define GAP_PHYS_PREFER_ALL           0
#define GAP_PHYS_NO_PREFER_TX_BIT     0x01 /**< The Host has no preference among the transmitter PHYs supported by the Controller */
#define GAP_PHYS_NO_PREFER_RX_BIT     0x02 /**< The Host has no preference among the receiver PHYs supported by the Controller */

#define GAP_PHYS_PREFER_1M_BIT        0x01 /**< The Host prefers to use the LE 1M transmitter PHY (possibly among others)  */
#define GAP_PHYS_PREFER_2M_BIT        0x02 /**< The Host prefers to use the LE 2M transmitter PHY (possibly among others)  */
#define GAP_PHYS_PREFER_CODED_BIT     0x04 /**< The Host prefers to use the LE Coded transmitter PHY (possibly among others)  */
#endif

/** @defgroup GAP_PHYS_CONN_INIT Connection Initiating PHYs
 * @{
 */
#define GAP_PHYS_CONN_INIT_1M_BIT     0x01/**< Scan connectable advertisements on the LE 1M PHY. Connection
parameters for the LE 1M PHY are provided. */
#if F_BT_LE_5_0_SET_PHYS_SUPPORT
#define GAP_PHYS_CONN_INIT_2M_BIT     0x02 /**< Connection parameters for the LE 2M PHY are provided.*/
#define GAP_PHYS_CONN_INIT_CODED_BIT  0x04 /**< Scan connectable advertisements on the LE Coded PHY. Connection
parameters for the LE Coded PHY are provided.*/
#endif
/** End of GAP_PHYS_CONN_INIT
  * @}
  */

/** End of GAP_LE_TYPES_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @addtogroup GAP_LE_TYPES_Exported_Types
  * @{
  */

/** @brief define local adress type */
typedef enum
{
    GAP_LOCAL_ADDR_LE_PUBLIC           = 0x00,    /**<  Bluetooth low energy public address. */
    GAP_LOCAL_ADDR_LE_RANDOM           = 0x01,    /**<  Bluetooth low energy random address. */
#if F_BT_LE_PRIVACY_SUPPORT
    GAP_LOCAL_ADDR_LE_RAP_OR_PUBLIC    = 0x02,    /**< Controller generates the Resolvable Private Address based on the local
IRK from the resolving list. If the resolving list contains no matching entry, then use the public address. */
    GAP_LOCAL_ADDR_LE_RAP_OR_RAND      = 0x03     /**< Controller generates the Resolvable Private Address based on the local
IRK from the resolving list. If the resolving list contains no matching entry, then use the random address from the most recent successful LE_Set_Random_Address Command. */
                                         /** @} End GAP_LE_LOCAL_ADDR_TYPE_DEFINES */
#endif
} T_GAP_LOCAL_ADDR_TYPE;

/** @brief Define random adress type */
typedef enum
{
    GAP_RAND_ADDR_STATIC            = 0x00,/**<  Static random device address. */
    GAP_RAND_ADDR_NON_RESOLVABLE    = 0x01,/**<  Non resolvable random device address. */
    GAP_RAND_ADDR_RESOLVABLE        = 0x02 /**<  Resolvable random device address. */
} T_GAP_RAND_ADDR_TYPE;

/** @brief Define indentify address type */
typedef enum
{
    GAP_IDENT_ADDR_PUBLIC      = 0x00,    /*  low energy public address. */
    GAP_IDENT_ADDR_RAND        = 0x01,    /*  low energy random address. */
} T_GAP_IDENT_ADDR_TYPE;

#if F_BT_LE_PRIVACY_SUPPORT
/** @brief  Definition of LE direct address type.*/
typedef enum
{
    GAP_DIRECT_ADDR_LE_PUBLIC              = 0x00, /**<  Public Device Address (default) */
    GAP_DIRECT_ADDR_LE_RAND                = 0x01, /**<  Random Device Address */
    GAP_DIRECT_ADDR_LE_PUBLIC_IDENT        = 0x02, /**<  Public Identity Address (Corresponds to Resolved Private Address ) */
    GAP_DIRECT_ADDR_LE_RAND_IDENT          = 0x03, /**<  Random (static) Identity Address (Corresponds to Resolved Private Address) */
    GAP_DIRECT_ADDR_LE_RAND_UNABLE_RESOLV  = 0xFE  /**<  No address provided (anonymous advertisement) */
} T_GAP_DIRECT_ADDR_TYPE;
#endif

/** @brief  Definition of LE white list operation.*/
typedef enum
{
    GAP_WHITE_LIST_OP_CLEAR = 0,    /**<  Clear white list. */
    GAP_WHITE_LIST_OP_ADD,          /**<  Add a device to the white list. */
    GAP_WHITE_LIST_OP_REMOVE        /**<  Remove a device from the white list. */
} T_GAP_WHITE_LIST_OP;

#if F_BT_LE_GAP_PERIPHERAL_SUPPORT
/** @brief  Advertising Event Types.*/
typedef enum
{
    GAP_ADTYPE_ADV_IND,             //!< Connectable undirected advertisement
    GAP_ADTYPE_ADV_HDC_DIRECT_IND,  //!< Connectable high duty cycle directed advertisement
    GAP_ADTYPE_ADV_SCAN_IND,        //!< Scannable undirected advertisement
    GAP_ADTYPE_ADV_NONCONN_IND,     //!< Non-Connectable undirected advertisement
    GAP_ADTYPE_ADV_LDC_DIRECT_IND,  //!< Connectable low duty cycle directed advertisement
} T_GAP_ADTYPE;

/** @brief  Definition of LE advertising filter policy.*/
typedef enum
{
    GAP_ADV_FILTER_ANY = 0, /**<  Process scan and connection requests from all devices (i.e., the White List is not in use) (default). */
    GAP_ADV_FILTER_WHITE_LIST_SCAN,/**<  Process connection requests from all devices and only scan requests from
devices that are in the White List. */
    GAP_ADV_FILTER_WHITE_LIST_CONN,/**<  Process scan requests from all devices and only connection requests from
devices that are in the White List. */
    GAP_ADV_FILTER_WHITE_LIST_ALL,/**<  Process scan and connection requests only from devices in the White List. */
} T_GAP_ADV_FILTER_POLICY;
#endif

#if F_BT_LE_GAP_SCAN_SUPPORT
/** @brief  Definition of LE scan type.*/
typedef enum
{
    GAP_SCAN_MODE_PASSIVE, /**<   Passive Scanning. No scan request PDUs shall be sent. */
    GAP_SCAN_MODE_ACTIVE   /**<   Active Scanning. Scan request PDUs may be sent. */
} T_GAP_SCAN_MODE;

/** @brief  Definition of LE scan filter policy.*/
typedef enum
{
    GAP_SCAN_FILTER_ANY = 0, /**<  Accept all advertising packets except directed advertising packets not
addressed to this device (default). */
    GAP_SCAN_FILTER_WHITE_LIST,/**<  Accept only advertising packets from devices where the advertiser's
address is in the White List. Directed advertising packets which are not
addressed to this device shall be ignored. */
    GAP_SCAN_FILTER_ANY_RPA,    /**<  Accept all advertising packets except directed advertising packets
where the initiator's identity address does not address this device.
Note: Directed advertising packets where the initiator's address is a
resolvable private address that cannot be resolved are also accepted. */
    GAP_SCAN_FILTER_WHITE_LIST_RPA,/**<  Accept all advertising packets except:
1) advertising packets where the advertiser's identity address is not in
the White List; and
2) directed advertising packets where the initiator's identity address
does not address this device
Note: Directed advertising packets where the initiator's address is a
resolvable private address that cannot be resolved are also accepted. */
} T_GAP_SCAN_FILTER_POLICY;

/** @brief  Definition of LE extended scan duplicate filter type.*/
typedef enum
{
    GAP_SCAN_FILTER_DUPLICATE_DISABLE = 0, /**<  Duplicate filtering disabled. */
    GAP_SCAN_FILTER_DUPLICATE_ENABLE = 1,  /**<  Duplicate filtering enabled. */
    GAP_SCAN_FILTER_DUPLICATE_ENABLED_RESET_FOR_EACH_PERIOD = 2,    /**<  Duplicate filtering enabled, reset for each scan period. */
} T_GAP_SCAN_FILTER_DUPLICATE;

/** This enum type describes adv type for T_GAP_ADV_EVT_TYPE. */
typedef enum
{
    GAP_ADV_EVT_TYPE_UNDIRECTED = 0,    /**<  Connectable  undirected advertising. */
    GAP_ADV_EVT_TYPE_DIRECTED   = 1,    /**<  Connectable directed advertising. */
    GAP_ADV_EVT_TYPE_SCANNABLE  = 2,    /**<  Scanable undirected advertising. */
    GAP_ADV_EVT_TYPE_NON_CONNECTABLE,   /**<  Nonconnectable undirected advertising. */
    GAP_ADV_EVT_TYPE_SCAN_RSP           /**<  scan response. */
} T_GAP_ADV_EVT_TYPE;
#endif

/** @brief Definition of security level. */
typedef enum
{
    GAP_SEC_LEVEL_NO          = 0x00,/**< No security (No authentication and no encryption).*/
    GAP_SEC_LEVEL_UNAUTHEN    = 0x04,/**< Unauthenticated pairing with encryption.*/
    GAP_SEC_LEVEL_AUTHEN      = 0x05,/**< Authenticated pairing with encryption.*/
    GAP_SEC_LEVEL_SC_UNAUTHEN = 0x07,/**< Unuthenticated LE Secure Connections pairing with encryption using a 128-
bit strength encryption key.*/
    GAP_SEC_LEVEL_SC_AUTHEN   = 0x08/**<  Authenticated LE Secure Connections pairing with encryption using a 128-
bit strength encryption key.*/
} T_GAP_SEC_LEVEL;

#if F_BT_LE_PRIVACY_SUPPORT
/** @brief  Definition of LE resolving list operation.*/
typedef enum
{
    GAP_RESOLV_LIST_OP_CLEAR = 0,    /**< Clear resolving list operation. */
    GAP_RESOLV_LIST_OP_ADD,          /**< Add a device to the resolving list operation. */
    GAP_RESOLV_LIST_OP_REMOVE        /**< Remove a device from the resolving list operation. */
} T_GAP_RESOLV_LIST_OP;

/** @brief  Definition of LE privacy mode.*/
typedef enum
{
    GAP_PRIVACY_MODE_NETWORK = 0,    /**< Network privacy mode. */
    GAP_PRIVACY_MODE_DEVICE          /**< Device privacy mode. */
} T_GAP_PRIVACY_MODE;
#endif

#if F_BT_LE_5_0_SET_PHYS_SUPPORT
/** @brief  Definition of LE PHY type.*/
typedef enum
{
    GAP_PHYS_1M       = 0x01,    /**< LE PHY 1M used. */
    GAP_PHYS_2M       = 0x02,    /**< LE PHY 2M used. */
    GAP_PHYS_CODED    = 0x03     /**< LE Coded PHY  used. */
} T_GAP_PHYS_TYPE;

/** @brief  Definition of LE primary advertising PHY type.*/
typedef enum
{
    GAP_PHYS_PRIM_ADV_1M      = 1, /**<  Primary advertisement PHY is LE 1M */
    GAP_PHYS_PRIM_ADV_CODED   = 3  /**<  Primary advertisement PHY is LE Coded */
} T_GAP_PHYS_PRIM_ADV_TYPE;

/** @brief  Definition of LE Coded PHY preference options.*/
typedef enum
{
    GAP_PHYS_OPTIONS_CODED_PREFER_NO   = 0,/**< Host has no preferred coding when transmitting on the LE Coded PHY */
    GAP_PHYS_OPTIONS_CODED_PREFER_S2   = 1,/**< Host prefers that S=2 coding be used when transmitting on the LE Coded PHY */
    GAP_PHYS_OPTIONS_CODED_PREFER_S8   = 2 /**< Host prefers that S=8 coding be used when transmitting on the LE Coded PHY */
} T_GAP_PHYS_OPTIONS;
#endif

/** End of GAP_LE_TYPES_Exported_Types
  * @}
  */

/** @} */ /* End of group GAP_LE_TYPES */


/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_H */
