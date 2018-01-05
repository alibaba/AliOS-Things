/** @file
 *  Defines functions for using the MICO Bluetooth Framework
 */

#pragma once

#include "mico.h"
#include "mico_bt_types.h"
#include "mico_bt_dev.h"
#include "mico_bt_smart_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/** MICO Bluetooth Framework mode of operation
 */
typedef enum {
    MICO_BT_MPAF_MODE, /**< The framework uses Multi-Profile Application Framework (MPAF). The entire Bluetooth stack runs on the controller. The host controls the controller using remote procedure calls (RPC) */
    MICO_BT_HCI_MODE,  /**< The framework uses standard Host Controller Interface (HCI). The upper stack runs on the host and the lower stack runs on the controller                                                         */
} mico_bt_mode_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/** MICO Bluetooth packet.
 *  An opaque data type representing a generic, zero-copy packet used for transporting data to/from the Bluetooth controller.
 */
typedef struct bt_packet mico_bt_packet_t;
typedef platform_uart_config_t                  mico_uart_config_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

extern mico_worker_thread_t mico_bt_worker_thread;
extern mico_worker_thread_t mico_bt_evt_worker_thread;

#define MICO_BT_WORKER_THREAD       ((mico_worker_thread_t *)&mico_bt_worker_thread)
#define MICO_BT_EVT_WORKER_THREAD   ((mico_worker_thread_t *)&mico_bt_evt_worker_thread)

/******************************************************
 *               Function Declarations
 ******************************************************/

/*****************************************************************************/
/** @defgroup micobt      Bluetooth
 *
 *  MICO Bluetooth Framework Functions
 */
/*****************************************************************************/

/*****************************************************************************/
/** @addtogroup btfwmgmt  Framework
 *  @ingroup micobt
 *
 *  Bluetooth Framework Management Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initialise the MICO Bluetooth Framework
 *
 * This function initialises the local Bluetooth device and the framework core
 * components to operate in the mode specified. Upon return, the device is
 * powered.
 *
 * @note To switch mode, invoke @ref mico_bt_deinit to tear down the current
 *       operating mode, and call this function with the desired mode.
 *
 * @param mode        : The framework mode of operation, only MICO_BT_HCI_MODE is
 *                      supported now
 * @param device_name : A user-friendly name of the local Bluetooth device. A
 *                      name longer than 21 characters will be truncated.
 * @param client_links :  Max cocurrent connections as a BLE client
 * @param server_links :
 *
 * @return    MICO_SUCCESS : on success;
 *            MICO_ERROR   : if an error occurred
 */

OSStatus mico_bt_init( mico_bt_mode_t mode, const char *device_name, uint8_t client_links, uint8_t server_links );

/** Deinitialise the MICO Bluetooth Framework
 *
 * This function tears down all active framework components. Depending on the
 * hardware platform used, it may also power down the Bluetooth device.
 *
 * @return    MICO_SUCCESS : on success;
 *            MICO_ERROR   : if an error occurred
 */
OSStatus mico_bt_deinit( void );

/** Initialise the device address of the local Bluetooth device
 *
 * This function provides users with a option to overwrite the default address of
 * the local Bluetooth device with the address provided. Once called, @ref mico_bt_init()
 * overwrites the default address with the address provided. Users can selectively
 * overwrite bits of the default address by setting the correspondping bits in the
 * 'mask' argument to 1.
 *
 * @warning When used, this function *MUST* be called before mico_bt_init()
 *
 * @param[in] address : new address
 * @param[in] mask    : masking bits
 *
 * @return    MICO_SUCCESS : on success;
 *            MICO_ERROR   : if an error occurred
 */
OSStatus mico_bt_init_address( const mico_bt_device_address_t *address, const mico_bt_device_address_t *mask );


/** Start manufacturing test mode
 *
 * @param[in] config : Configuration of the UART peripheral that connects to the host PC
 *
 * @return    MICO_SUCCESS : on success;
 *            MICO_ERROR   : if an error occurred
 */
OSStatus mico_bt_start_mfgtest_mode( const mico_uart_config_t *config );

/** @} */

/*****************************************************************************/
/** @addtogroup btdevmgmt  Device
 *  @ingroup micobt
 *
 *  Bluetooth Device Management Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Retrieve the device address of the local Bluetooth device
 *
 * @param[out] address : device address
 *
 * @return    MICO_TRUE  : is device address successfully retrieved;
 *            MICO_FALSE : if not or if an error occurred
 */
OSStatus mico_bt_device_get_address( mico_bt_device_address_t *address );

/** Retrieve the user-friendly name of the local Bluetooth device
 *
 * @return pointer to the device name string
 */
const char    *mico_bt_device_get_name( void );

/** Check if the local Bluetooth device is powered
 *
 * @return    MICO_TRUE  : if powered on;
 *            MICO_FALSE : if not powered or if an error occurred
 */
mico_bool_t   mico_bt_device_is_on( void );

/** Check if the local Bluetooth device is connectable state
 *
 * @return    MICO_TRUE  : is in connectable state;
 *            MICO_FALSE : if not or if an error occurred
 */
mico_bool_t   mico_bt_device_is_connectable( void );

/** Check if the local Bluetooth device is discoverable state
 *
 * @return    MICO_TRUE  : is in discoverable state;
 *            MICO_FALSE : if not or if an error occurred
 */
mico_bool_t   mico_bt_device_is_discoverable( void );


/** @} */

/*****************************************************************************/
/** @addtogroup btdevmgmt  Device
 *  @ingroup micobt
 *
 *  Bluetooth Pairing Management Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Start a MiCO bluetooth LE pairing procedure
 *
 *
 * @param address   : The remote device address
 * @param type      : The remote device address type
 * @param settings  : Security settings used in pairing procedure
 *
 * @return  MICO_BT_PENDING if successfully initiated,
 *          MICO_BT_SUCCESS if already paired to the device, else
 *          error code
 */
OSStatus mico_bt_start_pairing( mico_bt_device_address_t address, mico_bt_smart_address_type_t type,
                                const mico_bt_smart_security_settings_t *settings );

/** Stop a MiCO bluetooth LE pairing procedure
 *
 * @param address   : The remote device address
 *
 * @return   MICO_BT_PENDING if cancel initiated,
 *           MICO_BT_SUCCESS if cancel has completed already, else error code.
 */
OSStatus mico_bt_stop_pairing( mico_bt_device_address_t address );

/** Satrt a MiCO bluetooth LE encryption procedure
 *
 * @param address   : The remote device address
 *
 * @return   MICO_BT_SUCCESS            : already encrypted
 *           MICO_BT_PENDING            : command will be returned in the callback
 *           MICO_BT_WRONG_MODE         : connection not up.
 *           MICO_BT_BUSY               : security procedures are currently active
 */
OSStatus mico_bt_start_encryption( mico_bt_device_address_t *address );

/** @} */

/*****************************************************************************/
/** @addtogroup btpktmgmt  Packet
 *  @ingroup micobt
 *
 *  Bluetooth Packet Management Functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Delete a MICO Bluetooth packet
 *
 * This function returns the packet's memory space back to the source, allowing
 * for reuse.
 *
 * @param packet : The pointer to the packet to delete

 * @return    MICO_SUCCESS : on success;
 *            MICO_BADARG  : if bad argument(s) are inserted;
 *            MICO_ERROR   : if an error occurred.
 */
OSStatus mico_bt_packet_delete( mico_bt_packet_t *packet );

/** Get a pointer to the packet data
 *
 * This function retrieves a pointer to the start of the data section in the
 * packet. It also returns the current data size and the remaining data space
 * in the packet.
 *
 * @param packet            : The pointer to the packet
 * @param data              : A pointer that will receive the pointer to the
 *                            start of the data section in the packet
 * @param current_data_size : A pointer that will receive the size of the data
 *                            in the packet in bytes
 * @param available_space   : A pointer that will receive the available data
 *                            space in the packet in bytes
 *
 * @return    MICO_SUCCESS : on success;
 *            MICO_BADARG  : if bad argument(s) are inserted;
 *            MICO_ERROR   : if an error occurred.
 */
OSStatus mico_bt_packet_get_data( const mico_bt_packet_t *packet, uint8_t **data, uint32_t *current_data_size,
                                  uint32_t *available_space );

/** Set the end of the packet data
 *
 * This function updates the end of the data section and the data size in the
 * packet.
 *
 * @param packet   : The pointer to the packet
 * @param data_end : The pointer to the end of the data section in the packet

 * @return    MICO_SUCCESS : on success;
 *            MICO_BADARG  : if bad argument(s) are inserted;
 *            MICO_ERROR   : if an error occurred
 */
OSStatus mico_bt_packet_set_data_end( mico_bt_packet_t *packet, const uint8_t *data_end );

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


/** Get the maximum number of devices supported by the whitelist
 *
 * @note
 * This function retrieves the maximum number of Bluetooth Smart devices which can
 * be added to the whitelist.
 *
 * @param[out]  size : device count
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_get_whitelist_capability( uint8_t *size );


/** Clear the whitelist
 *
 * @note
 * This function instructs the Bluetooth Controller to remove all devices from the
 * whitelist
 *
 * @return @ref OSStatus
 */
OSStatus mico_bt_clear_whitelist( void );

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
