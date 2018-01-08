#pragma once

#include "mico_bt_smart_interface.h"
#include "LinkListUtils.h"
#include "mico_bt_gatt.h"
/** @file
 *  Defines functions for bridging Bluetooth Smart with Wi-Fi
 */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * BT smart peripheral socket status
 */
typedef enum {
    PERIPHERAL_SOCKET_DISCONNECTED, /**< Socket is disconnected                   */
    PERIPHERAL_SOCKET_CONNECTING,   /**< Socket is in connecting state            */
    PERIPHERAL_SOCKET_CONNECTED,    /**< Socket is connected with a remote device */
} mico_bt_peripheral_socket_status_t;

/**
 * Advertising filter policy
 */
typedef enum {
    PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ               = 0x00,    /**< Process scan and connection requests from all devices (i.e., the White List is not in use) (default) */
    PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ         = 0x01,    /**< Process connection requests from all devices and only scan requests from devices that are in the White List. */
    PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ         = 0x02,    /**< Process scan requests from all devices and only connection requests from devices that are in the White List */
    PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ   = 0x03,    /**< Process scan and connection requests only from devices in the White List. */
    PERIPHERAL_ADVERT_FILTER_MAX
} mico_bt_peripheral_adv_filter_policy_t;


/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct _bt_ext_attribute_value_t mico_bt_ext_attribute_value_t;

/**
 * Socket to create a BT smart peripheral connection
 */
typedef struct mico_bt_peripheral_socket mico_bt_peripheral_socket_t;

/**
 * Socket connection callback
 */
typedef OSStatus (* mico_bt_peripheral_connection_callback_t) ( mico_bt_peripheral_socket_t *socket );

/**
 * Socket disconnection callback
 */
typedef OSStatus (* mico_bt_peripheral_disconnection_callback_t) ( mico_bt_peripheral_socket_t *socket );

/**
 * Attrubute request callback
 */
typedef mico_bt_gatt_status_t (* mico_bt_peripheral_attribute_handler)( mico_bt_ext_attribute_value_t *attribute,
                                                                        mico_bt_gatt_request_type_t op );


/******************************************************
 *                    Structures
 ******************************************************/


#define BT_SMART_S_NONE            ( 0x0 )
#define BT_SMART_S_INDICATE        ( 0x1 << 0 )
#define BT_SMART_S_NOTIFICATION    ( 0x1 << 1 )

typedef uint8_t bt_smart_server_send_type_t;

struct _bt_ext_attribute_value_t {
    linked_list_node_t                      this_node;           /* Linked-list node of this characteristic */
    uint16_t                                handle;              /* Attribute handle */
    uint16_t                                value_length;        /* Attribute value length */
    uint16_t                                value_buffer_length; /* Attribute value buffer length */
    uint8_t                                *p_value;             /* Pointer to characteristic value */
    mico_bt_peripheral_attribute_handler    attribute_handler;
};


/**
 * Socket to create a Smart Peripheral connection
 * @warning The content of the socket structure is for INTERNAL USE only. Modifying
 * the content of this structure is prohibited. Please use the Bluetooth SmartBridge
 * API to retrieve socket information.
 */
struct mico_bt_peripheral_socket {
    mico_bt_smart_device_t
    remote_device;                  /**< Remote Bluetooth device MiCO is connected with (BLE server)   */
    uint16_t
    connection_handle;              /**< Connection handle                                             */
    uint8_t
    state;                          /**< Internal state                                                */
    uint8_t
    actions;                        /**< Internal socket actions                                       */
    mico_bt_peripheral_connection_callback_t
    connection_callback;            /**< Callback for handling connection event by remote device       */
    mico_bt_peripheral_disconnection_callback_t
    disconnection_callback;         /**< Callback for handling disconnection event by remote device    */
    mico_bt_smart_bonding_callback_t
    bonding_callback;               /**< Callback for handling bonding evnet by remote device          */
    mico_bt_smart_security_settings_t
    security_settings;              /**< Security settings                                             */
    mico_bt_smart_bond_request_t
    bond_req;                       /**< Bond Request Structure                                        */
    mico_semaphore_t
    semaphore;                      /**< Semaphore                                                     */
    linked_list_t
    attribute_database;             /**< Attribute database                                            */
    uint16_t                                        mtu;
};

/******************************************************
 *             Function declarations
 ******************************************************/

/*****************************************************************************/
/** @addtogroup smartbridge  SmartBridge
 *  @ingroup micobt
 *
 *  Bluetooth SmartBridge Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/*****************************************************************************/
/** @addtogroup sbmgmt SmartBridge Management
 *  @ingroup smartbridge
 *
 *  SmartBridge Management Functions
 *
 *
 *  @{
 */
/*****************************************************************************/


/** Initialise the MiCO BT peripheral
 *
 * @note
 * This function initialises:
 * \li Generic Attribute Profile (GATT) Server
 * \li Generic Access Profile (GAP) Peripheral Role
 * \li Security settings used when conneted by BT client
 * \li Initialises the socket internals to make it ready to connect to
 *     a Bluetooth Smart Central
 *
 *
 * @return MICO_BT_SUCCESS: success , else @ref OSStatus
 */
OSStatus mico_bt_peripheral_init(   mico_bt_peripheral_socket_t                   *socket,
                                    const mico_bt_smart_security_settings_t       *settings,
                                    mico_bt_peripheral_connection_callback_t       connection_callback,
                                    mico_bt_peripheral_disconnection_callback_t    disconnection_callback,
                                    mico_bt_smart_bonding_callback_t               bonding_callback );


/** Deinitialise the MiCO BT peripheral
 *
 * @note
 * This function deinitialises:
 * \li Generic Attribute Profile (GATT) Server
 * \li Generic Access Profile (GAP) Peripheral Role
 * \li Create BT peripheral Socket ready to be connected
 *
 * @return MICO_BT_SUCCESS: success
 */
OSStatus mico_bt_peripheral_deinit( void );


/** @} */


/*****************************************************************************/
/** @addtogroup sbsock BT peripheral Socket and Connection Management
 *  @ingroup SmartPeripheral
 *
 *  BT peripheral Socket and Connection Functions
 *
 *
 *  @{
 */
/*****************************************************************************/


/** Get BT peripheral socket status
 *
 * @param[in]  socket : pointer to the socket to get the status
 * @param[out] status : socket status
 *
 * @return MICO_BT_SUCCESS: success
 *         MICO_BT_SMART_APPL_UNINITIALISED: Smart peripheral framework is uninitialized
 */
OSStatus mico_bt_peripheral_get_socket_status( mico_bt_peripheral_socket_t *socket,
                                               mico_bt_peripheral_socket_status_t *status );

/** Disconnect BT peripheral connection
 *
 * @note
 * This function disconnects a connection with remote client.
 *
 * @return MICO_BT_SUCCESS: success
 *         MICO_BT_SMART_APPL_UNINITIALISED: Smart peripheral framework is uninitialized
 */
OSStatus mico_bt_peripheral_disconnect( void );


/** @} */

/*****************************************************************************/
/** @addtogroup sbscan Smart Peripheral Advert
 *  @ingroup SmartPeripheral
 *
 *  Smart Peripheral Advertising Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Start advertising local Bluetooth Smart devices
 *
 * @note
 * This function instructs the Bluetooth controller to start advertising. Advertising data
 * sould be set first use mico_bt_ble_set_advertisement_data().
 *
 * @warning
 * \li complete_callback is an intermediate report callback.
 *
 * @param[in]  settings                     : advertising settings
 * @param[in]  complete_callback            : callback function which is called when advertising is
 *                                            complete，running under MICO_BT_EVT_WORKER_THREAD
 *
 * @return MICO_BG_SUCCESS, else @ref OSStatus
 */
OSStatus mico_bt_peripheral_start_advertisements( mico_bt_smart_advertising_settings_t *settings,
                                                  mico_bt_smart_advertising_complete_callback_t complete_callback);

/** Stop the ongoing advertising process
 *
 * This function instructs the Bluetooth controller to advertising local
 * Bluetooth Smart devices.
 *
 * @return MICO_BG_SUCCESS, else @ref OSStatus
 */
OSStatus mico_bt_peripheral_stop_advertisements( void );

/** @} */

/*****************************************************************************/
/** @addtogroup sbwhitelist SmartBridge Whitelist Filter
 *  @ingroup smartbridge
 *
 *  SmartBridge Whitelist Filter Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Update the devices in white list.
 *
 * @param[in] add            : Add or remove this device specified by device_address.
 * @param[in] device_address : Bluetooth address of the device to add to the whitelist
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_update_advertisements_white_list( mico_bool_t add,
                                                              mico_bt_device_address_t device_address );

/** Get the number of devices in white list
 *
 * @param[out] size : The number of devices in white list.
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_get_advertisements_white_list_size( uint8_t *size );

/** Set Advertisements Filter Policy
 *
 * @param[in] policy : Advertisements filter policy
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_set_advertisements_filter_policy( mico_bt_peripheral_adv_filter_policy_t policy );

/** @} */

/*****************************************************************************/
/** @addtogroup sbattr Smart peripheral Attribute Value database
 *  @ingroup SmartPeripheral
 *
 *  Smart Peripheral External Attribute Value Database Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Add an external attribute vale to BT peripheral
 *
 * @param handle[in]       : Handle of an attribution
 * @param length[in]       : Attribute value length (0, if value is not existed)
 * @param value[in]        : Point to the Attribute value (NULL, if value is not existed)
 * @param handler[in]      : Attribute request handler is synchronized triggerd
 *                           after an attribute write by remote GATT write operation
 *                           or before anattribute read by remote GATT read operation
 *
 * @return The address of the external attrbute value object, NULL if failed
 */
mico_bt_ext_attribute_value_t *mico_bt_peripheral_ext_attribute_add( uint16_t handle, uint16_t length,
                                                                     const uint8_t *value, mico_bt_peripheral_attribute_handler handler );


/** Remove an external attribute vale from BT peripheral
 *
 * @param attribute[in]   : The address of the external attrbute value object
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_ext_attribute_remove( mico_bt_ext_attribute_value_t *attribute );

/** Find an external attribute value from BT peripheral using handle
 *
 * @param handle[in]                : Handle of an attribute
 * @param attribute_found[in,out]   : Pointer to the external attribute address find by handle
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_ext_attribute_find_by_handle( uint16_t handle,
                                                          mico_bt_ext_attribute_value_t **attribute_found );


/** Write or update data to the external attribute value object
 *
 * @note
 * The value will copy to attrubute object, free after write
 *
 * @param handle[in]       : Handle of an attribute
 * @param length[in]       : Data length
 * @param length[in]       : Attrubute value offset where data is written to
 * @param value[in]        : Point to the data
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_ext_attribute_value_write( mico_bt_ext_attribute_value_t *attribute, uint16_t length,
                                                       uint16_t value_offset, const uint8_t *value );


/** Remove all external attribute vale from BT peripheral
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_ext_attribute_remove_all( void );


/** Send external attribute value to BT client using indicate
 *
 *
 * @param socket[in]       : Pointer to the socket to send attribute value
 * @param attribute[in]    : Pointer to the external attribute that hold the value to be sent
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_gatt_indicate_attribute_value ( mico_bt_peripheral_socket_t *socket,
                                                            const mico_bt_ext_attribute_value_t *attribute );


/** Send external attribute value to BT client using notify
 *
 *
 * @param socket[in]       : Pointer to the socket to send attribute value
 * @param attribute[in]    : Pointer to the external attribute that hold the value to be sent
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_peripheral_gatt_notify_attribute_value( mico_bt_peripheral_socket_t *socket,
                                                         const mico_bt_ext_attribute_value_t *attribute );


/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

