
/** @file
 *
 * Generic types
 *
 */
#pragma once
#include "common.h"
#include "data_types.h"


#ifdef __cplusplus
extern "C" {
#endif

#define BD_ADDR_LEN     6
typedef uint8_t         mico_bt_device_address_t[BD_ADDR_LEN]; /**< Device address length */

typedef uint8_t *mico_bt_device_address_ptr_t;                 /**< Device address Pointer */

#define DEV_CLASS_LEN   3
typedef uint8_t         mico_bt_dev_class_t[DEV_CLASS_LEN];    /**< Device class */

#define MAX_UUID_SIZE              16  /**< Maximum UUID size - 16 bytes, and structure to hold any type of UUID. */

/** UUID Type */
typedef struct {
#define LEN_UUID_16     2
#define LEN_UUID_32     4
#define LEN_UUID_128    16

    uint16_t        len;     /**< UUID length */

    union {
        uint16_t    uuid16; /**< 16-bit UUID */
        uint32_t    uuid32; /**< 32-bit UUID */
        uint8_t     uuid128[MAX_UUID_SIZE]; /**< 128-bit UUID */
    } uu;

} mico_bt_uuid_t;

#define BT_OCTET16_LEN    16                /**<  length: 16 */
typedef uint8_t BT_OCTET16[BT_OCTET16_LEN]; /**< octet array: size 16 */

#define BT_OCTET32_LEN    32
typedef uint8_t BT_OCTET32[BT_OCTET32_LEN];   /* octet array: size 32 */

/** Bluetooth QoS defintions */
typedef struct {
    uint8_t         qos_flags;              /**< TBD */
    uint8_t         service_type;           /**< service type (NO_TRAFFIC, BEST_EFFORT, or GUARANTEED) */
    uint32_t        token_rate;             /**< token rate (bytes/second) */
    uint32_t        token_bucket_size;      /**< token bucket size (bytes) */
    uint32_t        peak_bandwidth;         /**< peak bandwidth (bytes/second) */
    uint32_t        latency;                /**< latency (microseconds) */
    uint32_t        delay_variation;        /**< delay variation (microseconds) */
} mico_bt_flow_spec_t;

/* Values for smico_bt_flow_spec_t service_type */
#define NO_TRAFFIC      0
#define BEST_EFFORT     1
#define GUARANTEED      2

/**
 * @anchor MICO_BT_TRANSPORT_TYPE
 * @name Transport types
 * @{
 */
#define BT_TRANSPORT_BR_EDR         1       /**< BR/EDR transport */
#define BT_TRANSPORT_LE             2       /**< BLE transport */
typedef uint8_t mico_bt_transport_t;       /**< Transport type (see @ref MICO_BT_TRANSPORT_TYPE "BT Transport Types") */

/**
 * @anchor MICO_BT_DEVICE_TYPE
 * @name Device Types
 * @{
 */
#define BT_DEVICE_TYPE_BREDR        0x01    /**< BR/EDR device */
#define BT_DEVICE_TYPE_BLE          0x02    /**< LE device */
#define BT_DEVICE_TYPE_BREDR_BLE    0x03    /**< Dual Mode device */
typedef uint8_t
mico_bt_device_type_t;     /**< Bluetooth device type (see @ref MICO_BT_DEVICE_TYPE "BT Device Types") */
/** @} MICO_BT_DEVICE_TYPE */

/**
 * @anchor MICO_BT_ADDR_TYPE
 * @name Address Types
 * @{
 */
#define BLE_ADDR_PUBLIC             0x00        /**< Public address */
#define BLE_ADDR_RANDOM             0x01        /**< Random address */
#define BLE_ADDR_PUBLIC_ID          0x02        /**< Public ID      */
#define BLE_ADDR_RANDOM_ID          0x03        /**< Random ID      */
typedef uint8_t
mico_bt_ble_address_type_t;    /**< BLE device address type (see @ref MICO_BT_ADDR_TYPE "BT Address Types")*/
#define BLE_ADDR_TYPE_MASK          (BLE_ADDR_RANDOM | BLE_ADDR_PUBLIC)
/** @} MICO_BT_ADDR_TYPE */

typedef struct {
    mico_bt_ble_address_type_t type;
    mico_bt_device_address_t   bda;
} mico_bt_ble_address_t;

#define LINK_KEY_LEN    16
typedef uint8_t mico_bt_link_key_t[LINK_KEY_LEN];

#define BT_ROLE_MASTER             0x00
#define BT_ROLE_SLAVE              0x01
typedef uint8_t mico_bt_ble_link_role_t;


#ifdef __cplusplus
}
#endif
