
#pragma once

#include "mico_bt_smart_interface.h"
#include "LinkListUtils.h"

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
 * SmartBridge socket status
 */
typedef enum {
    SMARTBRIDGE_SOCKET_DISCONNECTED, /**< Socket is disconnected                   */
    SMARTBRIDGE_SOCKET_CONNECTING,   /**< Socket is in connecting state            */
    SMARTBRIDGE_SOCKET_CONNECTED,    /**< Socket is connected with a remote device */
} mico_bt_smartbridge_socket_status_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/**
 * Socket to create a SmartBridge connection
 */
typedef struct mico_bt_smartbridge_socket mico_bt_smartbridge_socket_t;

/**
 * Auto connection callback parameters
 */
typedef struct mico_bt_smartbridge_auto_conn_cback_parm mico_bt_smartbridge_auto_conn_cback_parms_t;

/**
 * Socket disconnection callback
 */
typedef OSStatus (*mico_bt_smartbridge_disconnection_callback_t)       ( mico_bt_smartbridge_socket_t *socket );

/**
 * Socket GATT notification callback
 */
typedef OSStatus (*mico_bt_smartbridge_notification_callback_t)        ( mico_bt_smartbridge_socket_t *socket,
                                                                         uint16_t attribute_handle );

/**
 * BLE Auto connection callback
 */
typedef OSStatus (*mico_bt_smartbridge_auto_connection_callback_t)     ( mico_bt_smartbridge_socket_t *socket );

/**
 * BLE Auto connection parameters callback -- auto connection procedure callback
 */
typedef OSStatus (*mico_bt_smartbridge_auto_connection_parms_cback_t)   ( const mico_bt_device_address_t random_bda,
                                                                          const char *name, const uint8_t *p_data, uint8_t len, mico_bt_smartbridge_auto_conn_cback_parms_t *parm );


/******************************************************
 *                    Structures
 ******************************************************/

/**
 * Socket to create a SmartBridge connection
 * @warning The content of the socket structure is for INTERNAL USE only. Modifying
 * the content of this structure is prohibited. Please use the Bluetooth SmartBridge
 * API to retrieve socket information.
 */
struct mico_bt_smartbridge_socket {
    linked_list_node_t
    node;                           /**< Socket list node                                              */
    mico_bt_smart_device_t
    remote_device;                  /**< Remote Bluetooth device MICO is connected with (bridging)    */
    uint16_t
    connection_handle;              /**< Connection handle                                             */
    uint16_t
    last_notified_attribute_handle; /**< Last notified attribute handle                                */
    uint8_t
    state;                          /**< Internal state                                                */
    uint8_t
    actions;                        /**< Internal socket actions                                       */

    mico_bt_smartbridge_auto_connection_callback_t
    auto_connection_callback;       /**< Callback for handing connection event by Auto Connection      */
    mico_bt_smartbridge_disconnection_callback_t
    disconnection_callback;         /**< Callback for handling disconnection event by remote device    */
    mico_bt_smart_bonding_callback_t
    bonding_callback;               /**< Callback for handling pairing/bonding successful event        */
    mico_bt_smartbridge_notification_callback_t
    notification_callback;          /**< Callback for handling GATT notification from remote device    */
    mico_bt_smart_connection_settings_t
    connection_settings;            /**< Connection settings                                           */
    mico_bt_smart_security_settings_t
    security_settings;              /**< Security settings                                             */
    mico_bt_smart_bond_info_t
    bond_info;                      /**< Bond Info                                                     */
    mico_bt_smart_bond_request_t
    bond_req;                       /**< Bond Request Structure                                        */
    void
    *att_cache;                      /**< Pointer to Attribute Cache                                    */
    mico_semaphore_t
    semaphore;                      /**< Semaphore                                                     */
};

struct mico_bt_smartbridge_auto_conn_cback_parm {
    mico_bt_smartbridge_socket_t
    *socket;                         /**< A socket associated with the auto connection */
    mico_bt_smart_connection_settings_t
    conn_settings;                  /**< The connection settings associated with a socket */
    mico_bt_smart_security_settings_t
    security_settings;              /**< Security settings                                */

    mico_bt_smartbridge_auto_connection_callback_t
    auto_connection_callback;       /**< Callback for handing connection event by Auto connection */
    mico_bt_smartbridge_disconnection_callback_t
    auto_disconn_callback;          /**< Callback for handing disconnection event */
    mico_bt_smartbridge_notification_callback_t
    notification_callback;          /**< Callback for handing GATT notification from a remote device */
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


/** Initialise the MICO SmartBridge
 *
 * @note
 * This function initialises:
 * \li Generic Attribute Profile (GATT) Client
 * \li Generic Access Profile (GAP) Central Role
 * \li SmartBridge Socket Manager
 * \li Set the number of concurrent connections
 *
 * After calling @ref mico_bt_smartbridge_init, you may call:
 * \li  @ref mico_bt_smartbridge_enable_attribute_cache() to enable Attribute Cache
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_init( uint8_t count );


/** Deinitialise the MICO SmartBridge
 *
 * @note
 * This function deinitialises:
 * \li GATT Client
 * \li GAP Central Role
 * \li SmartBridge Socket Manager
 * \li SmartBridge Attribute Cache Manager (if enabled)
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_deinit( void );


/** Enable Attribute Cache
 *
 * @note
 * This function enables the attribute caching feature as specified in the Bluetooth
 * Specification version 4.0 Volume 3 Part G Section 2.5.2. Attribute Cache allows
 * SmartBridge to discover server's Attribute information once and use cached information
 * across reconnections without rediscovery, thus saving time and reducing the amount
 * of packet exhange required between MICO SmartBridge and the server.
 *
 * On a reconnection, SmartBridge searches for matching Attribute information in
 * the cache. If not found, SmartBridge  discovers server's Attribute
 * information and adds the information to the cache when completed. If SmartBridge
 * runs out of cache space, the first non-active cache in the list is replaced.
 *
 * Specified services can be used in ATT cache generation, it makes MICO SmartBridge
 * only cache the services that application can support and operate.
 *
 * @param[in]  cache_count    : the number of caches that will be supported by MICO SmartBridge
 * @param[in]  cache_services : define service list to generate caches
 * @param[in]  service_count  : the number of services to generate caches, pass 0 to generate
 *                              all primary services
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_enable_attribute_cache( uint32_t cache_count, mico_bt_uuid_t cache_services[],
                                                     uint32_t service_count );


/** Disable Attribute Cache
 *
 * @note
 * This function disables the attribute caching feature
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_disable_attribute_cache( void );


/** @} */

/*****************************************************************************/
/** @addtogroup sbscan SmartBridge Scan
 *  @ingroup smartbridge
 *
 *  SmartBridge Scan Functions
 *
 *
 *  @{
 */
/*****************************************************************************/


/** Check if the MICO SmartBridge is currently scanning for Bluetooth Smart devices
 *
 * @note
 * This function checks if the Bluetooth Controller is currently scanning for Bluetooth
 * Smart devices
 *
 * @return @ref mico_bool_t
 */
mico_bool_t   mico_bt_smartbridge_is_scanning( void );


/** Start scanning for remote Bluetooth Smart devices
 *
 * @note
 * This function instructs the Bluetooth controller to start scanning for remote
 * Bluetooth Smart devices within proximity with the settings specified.
 * Scan results are reported via the given callback handlers.
 *
 * @warning
 * \li result_callback is an intermediate report callback. The complete scan results
 *     are retrieved using @ref mico_bt_smartbridge_get_scan_result_list once scan
 *     is complete
 * \li advertising_report_callback runs on the context of Bluetooth transport thread. Please refrain
 *     from executing a long task in the callback
 * \li complete_callback runs on the context of MICO_BT_EVT_WORKER_THREAD
 * \li If the whitelist filter is enabled in the scan settings, only devices
 *     in the whitelist appear in the scan results. Call @ref mico_bt_smartbridge_add_device_to_whitelist()
 *     to add a device to the whitelist
 *
 * @param[in]  settings                     : scan settings
 * @param[in]  complete_callback            : callback function which is called when scan is
 *                                            complete
 * @param[in]  advertising_report_callback  : callback function which is called when an advertising
 *                                            report is received
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_start_scan( const mico_bt_smart_scan_settings_t        *settings,
                                         mico_bt_smart_scan_complete_callback_t      complete_callback,
                                         mico_bt_smart_advertising_report_callback_t advertising_report_callback );


/** Stop the ongoing scan process
 *
 * This function instructs the Bluetooth controller to stop scanning for remote
 * Bluetooth Smart devices.
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_stop_scan( void );


/** Retrieve the most recent scan results
 *
 * @note
 * This function returns a linked-list of the most recent scan results
 *
 * @warning:
 * Application must not modify the list
 *
 * @param[out]  result_list : pointer that will receive the result linked-list
 * @param[out]  count       : variable that will receive the result count
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_get_scan_result_list( mico_bt_smart_scan_result_t **result_list, uint32_t *count );

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

/** Get the number of devices specified by type in the whitelist
 *
 * @note
 * This function retrieves the number of Bluetooth Smart devices which can
 * be used to auto connection procedure in the whitelist.
 *
 * @param[out]  size : device count
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_get_background_connection_devices_size( uint8_t *size );

/** Set auto connection action - start or stop.
 *
 * @param[in] start_stop        : Start or stop auto connection establishment procedure by
 *                                the White List.
 * @param[in] scan_settings     : Used to set up Scanning Parameters during Auto Connection
 *                                Procedure when the 'start_stop' is BT_TRUE.
 *                                And 'scan_settings.interval', 'scan_settings.window'
 *                                and 'scan_settings.duration_second' are only used.
 *                                If 'duration_second' is 0xFFFF, it will be forever until recalling
 *                                this function by "start_stop = FALSE".
 * @param[in] p_auto_conn_cback : Callback function called when a auto connection
 *                                will be established. It is used to query
 *                                user some information and configuration by lower
 *                                stack. Users should fill a socket entity,
 *                                connection settings and callback function.
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_set_auto_connection_action( mico_bool_t start_stop,
                                                         const mico_bt_smart_scan_settings_t *scan_settings,
                                                         mico_bt_smartbridge_auto_connection_parms_cback_t p_auto_conn_cback );


/** @} */


/*****************************************************************************/
/** @addtogroup sbsock SmartBridge Socket and Connection Management
 *  @ingroup smartbridge
 *
 *  SmartBridge Socket and Connection Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Check if the Bluetooth SmartBridge is ready to connect
 *
 * @note
 * This function checks if the Bluetooth SmartBridge is ready to make a connection
 * to a Bluetooth Smart device. The Bluetooth Controller does not support concurrent
 * connection requests.
 *
 * @return @ref mico_bool_t
 */
mico_bool_t   mico_bt_smartbridge_is_ready_to_connect( void );


/** Get SmartBridge socket status
 *
 * @param[in]  socket : pointer to the socket to get the status
 * @param[out] status : socket status
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_get_socket_status( mico_bt_smartbridge_socket_t *socket,
                                                mico_bt_smartbridge_socket_status_t *status );


/** Create a SmartBridge socket
 *
 * @note
 * \li This function initialises the socket internals to make it ready to connect to
 *     a Bluetooth Smart device
 * \li Socket reconnections are allowed without re-creations
 *
 * @param[out]  socket : pointer to the socket to initialise
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_create_socket( mico_bt_smartbridge_socket_t *socket );


/** Delete a SmartBridge socket
 *
 * @note
 * This function deinitialises the socket internals.
 *
 * @param[in,out]  socket : pointer to the socket to deinitialise
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_delete_socket( mico_bt_smartbridge_socket_t *socket );


/** Initiate a SmartBridge connection with a remote Bluetooth Smart device
 *
 * @note
 * This function creates a connection with the specified remote device.
 *
 * \li If Bond Information is loaded, the socket uses the Long-Term Key (LTK), Random
 *     Number (RAND), and Encryption Diversifier (EDIV) of the Bond Information to
 *     encrypt the connection. Call @ref mico_bt_smartbridge_set_bond_info() before
 *     connecting to load Bond information.
 * \li If Pairing is enforced, the socket initiates Pairing Request and a new Bond
 *     is created. Call @ref mico_bt_smartbridge_enable_pairing() before connecting
 *     to initiate Pairing Request.
 * \li If Attribute Cache is enabled, the socket starts a discovery of the server's
 *     Attribute information.
 *
 * @warning
 * \li Callback functions run on the context of MICO_BT_EVT_WORKER_THREAD.
 * \li If Pairing and Attribute Cache are enabled, this function may block for a
 *     few seconds.
 *
 * @param[in,out]  socket                 : pointer to the socket to create the
 *                                          connection
 * @param[in]      remote_device          : remote device to connect
 * @param[in]      settings               : connection settings
 * @param[in]      disconnection_callback : callback function that is called when
 *                                          the connection is disconnected by
 *                                          remote device
 * @param[in]      notification_callback  : callback function that is called when
 *                                          a GATT notification or indication is
 *                                          received from the remote device
 *

 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_connect( mico_bt_smartbridge_socket_t                *socket,
                                      const mico_bt_smart_device_t                *remote_device,
                                      const mico_bt_smart_connection_settings_t   *settings,
                                      mico_bt_smartbridge_disconnection_callback_t disconnection_callback,
                                      mico_bt_smartbridge_notification_callback_t  notification_callback );


/** Disconnect a SmartBridge connection
 *
 * @note
 * This function disconnects a connection with a remote device.
 *
 * @param[in,out]  socket : pointer to the socket of the connection to disconnect
 * @param[in] remove_it_from_whitelist: this device is remove from the white list if TRUE.
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_disconnect( mico_bt_smartbridge_socket_t *socket, mico_bool_t remove_it_from_whitelist );


/** Set transmit power during connection
 *
 * @note
 * This function set the transmit power of the connection
 *
 * @param[in]  socket             : pointer to the socket of the connection
 * @param[in]  transmit_power_dbm : transmit power in dBm
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_set_transmit_power( mico_bt_smartbridge_socket_t *socket, int8_t transmit_power_dbm );

/** @} */

/*****************************************************************************/
/** @addtogroup sbsec SmartBridge Security
 *  @ingroup smartbridge
 *
 *  SmartBridge Security Functions
 *
 *
 *  @{
 */
/*****************************************************************************/


/** Set a peer device's bond information
 *
 * @note
 * This function loads Bond (security) information to the socket which will be used
 * to reestablish an encrypted link with the peer device without having to perform
 * the complete handshake procedure (pairing)
 *
 * @warning
 * This function must be called before calling @ref mico_bt_smartbridge_connect
 *
 * @param[in,out]  socket    : socket to load the Bond information into
 * @param[in]      settings  : security settings
 * @param[in]      bond_info : Bond information
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_set_bond_info( mico_bt_smartbridge_socket_t            *socket,
                                            const mico_bt_smart_security_settings_t *settings,
                                            const mico_bt_smart_bond_info_t         *bond_info );


/** Reset the bond information stored in the socket
 *
 * @warning
 * This function must be called before calling @ref mico_bt_smartbridge_connect
 *
 * @param[in,out]  socket : pointer to the socket of the connection to disconnect
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_clear_bond_info( mico_bt_smartbridge_socket_t *socket );


/** Force a SmartBridge socket to initiate Pairing Request with a Bluetooth Smart device
 *
 * @note
 * This function prepares the socket to initiate Pairing Request with the device.
 * Previously loaded bond information is deleted and is replaced with a new bond
 * if the pairing process succeeds.
 *
 * @warning
 * \li This function must be called before calling @ref mico_bt_smartbridge_connect
 * \li Callback functions run on the context of MICO_BT_EVT_WORKER_THREAD
 *
 * @param[in,out]  socket           : pointer to the socket to enable pairing
 * @param[in]      settings         : security settings
 * @param[in]      numeric_passkey  : NUL-terminated string of numeric passkey. Maximum is 6 digits.
 * @param[in]      bonding callback : callback that is called when bonding with remote device occurs
 *
 * @note:
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_enable_pairing( mico_bt_smartbridge_socket_t             *socket,
                                             const mico_bt_smart_security_settings_t *settings,
                                             mico_bt_smart_bonding_callback_t         bonding_callback );


/** Set a SmartBridge socket to disable pairing with a remote device
 *
 * @warning
 * This function must be called before calling @ref mico_bt_smartbridge_connect
 *
 * @param[in,out]  socket : pointer to the socket to disable pairing
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_disable_pairing( mico_bt_smartbridge_socket_t *socket );

/** @} */

/*****************************************************************************/
/** @addtogroup sbattr SmartBridge Attribute Cache
 *  @ingroup smartbridge
 *
 *  SmartBridge Attribute Cache Functions
 *
 *
 *  @{
 */
/*****************************************************************************/


/** Enable all GATT notifications supported by the server
 *
 * @note
 * This function searches for client configuration Attribute in the cache and enables
 * all notifications from the server.
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in] socket : socket to enable notifications
 * @param[in] is_notification_or_indication: Enable Notification or Indication
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_enable_attribute_cache_notification( mico_bt_smartbridge_socket_t *socket,
                                                                  mico_bool_t is_notification_or_indication );


/** Disable all GATT notifications supported by the server
 *
 * @note
 * This function searches for client configuration Attribute in the cache and disables
 * all notifications from the server.
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in] socket : socket to disable notifications
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_disable_attribute_cache_notification( mico_bt_smartbridge_socket_t *socket );

/** Release attribute cache data
 *
 * @note
 * This function release all data in the cache and put attribute cache to free list.
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in] socket : socket to delete attribute cache
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_remove_attribute_cache( mico_bt_smartbridge_socket_t *socket );

/** Retrieve the list of cached Attributes
 *
 * @note
 * This function retrieves the list of cached Attributes of the socket
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in] socket          : socket to get the Attribute list from
 * @param[out] att_cache_list : pointer that will receive the Attribute list
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_get_attribute_cache_list( mico_bt_smartbridge_socket_t *socket,
                                                       mico_bt_smart_attribute_list_t **att_cache_list );


/** Find and read attribute with the handle provided from the Attribute Cache
 *
 * @note
 * This function searches for an attribute with the given handle in the cache and
 * copies the content to the attribute buffer provided.
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in]      socket    : pointer to the socket to search the attribute
 * @param[in]      handle    : handle of the attribute to find
 * @param[in,out]  attribute : pointer to the buffer which will contain the attribute
 * @param[in]      size      : buffer size in bytes
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_get_attribute_cache_by_handle( mico_bt_smartbridge_socket_t *socket, uint16_t handle,
                                                            mico_bt_smart_attribute_t *attribute, uint16_t size );


/** Find and read attribute with the UUID provided from the local attribute database
 *
 * @note
 * This function searches for an attribute with the given UUID in the local
 * attribute database and copies the content to the attribute buffer provided.
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in]      socket          : pointer to the socket to search the attribute from
 * @param[in]      uuid            : UUID of the attribute to find
 * @param[in]      starting_handle : the search starting handle
 * @param[in]      ending_handle   : the search ending handle
 * @param[in,out]  attribute       : pointer to the buffer which will contain the attribute content
 * @param[in]      size            : buffer size in bytes
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_smartbridge_get_attribute_cache_by_uuid( mico_bt_smartbridge_socket_t *socket,
                                                          const mico_bt_uuid_t *uuid, uint16_t starting_handle, uint16_t ending_handle, mico_bt_smart_attribute_t *attribute,
                                                          uint32_t size );
OSStatus mico_bt_smartbridge_get_service_from_attribute_cache_by_uuid( mico_bt_smartbridge_socket_t *socket,
                                                                       const mico_bt_uuid_t *uuid, uint16_t starting_handle, uint16_t ending_handle, mico_bt_smart_attribute_t *attribute,
                                                                       uint32_t size );
OSStatus mico_bt_smartbridge_get_characteritics_from_attribute_cache_by_uuid( mico_bt_smartbridge_socket_t *socket,
                                                                              const mico_bt_uuid_t *uuid, uint16_t starting_handle, uint16_t ending_handle, mico_bt_smart_attribute_t *attribute,
                                                                              uint32_t size );


/** Find and refresh Characteristic Value with the given handle in the Attribute Cache
 *
 * @note
 * This function reads the Characteristic Value from the server and updates the
 * cache.
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in] socket : socket with the Characteristic Value to refresh
 * @param[in] handle : handle of the Characteritic Value to refresh
 *
 * @return OSStatus
 */
OSStatus mico_bt_smartbridge_refresh_attribute_cache_characteristic_value( mico_bt_smartbridge_socket_t *socket,
                                                                           uint16_t handle );


/** Write Characteristic Value in the Attribute Cache to the server
 *
 * @note
 * This function writes the given Characteristic Value to the server and updates the
 * cache.
 *
 * @warning This function returns error if Attribute Cache is not enabled
 *
 * @param[in] socket               : socket with the Characteristic Value to write to the server
 * @param[in] characteristic_value : Characteritic Value to write to the server
 *
 * @return OSStatus
 */
OSStatus mico_bt_smartbridge_write_attribute_cache_characteristic_value( mico_bt_smartbridge_socket_t *socket,
                                                                         const mico_bt_smart_attribute_t *characteristic_value );

/** @} */

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

