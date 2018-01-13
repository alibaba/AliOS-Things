#pragma once

/** @file
 *  Peripheral's Interface Header with Bluetooth Stack
 */

//#include "mico_utilities.h"
#include "mico_bt_peripheral.h"
#include "mico_bt_smart_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define ATT_DEFAULT_MTU           (23)
#define ATT_STANDARD_VALUE_LENGTH (ATT_DEFAULT_MTU - 3)
#define ATT_STANDARD_TIMEOUT      (500)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus peripheral_bt_interface_initialize( void );

OSStatus peripheral_bt_interface_deinitialize( void );

OSStatus peripheral_bt_interface_cancel_last_connect( mico_bt_device_address_t address );

OSStatus peripheral_bt_interface_disconnect( uint16_t connection_handle );

OSStatus peripheral_bt_interface_set_security_settings( const mico_bt_smart_security_settings_t *settings );

OSStatus peripheral_bt_interface_start_advertisements( mico_bt_smart_advertising_settings_t *settings,
                                                       mico_bt_smart_advertising_complete_callback_t complete_callback );

OSStatus peripheral_bt_interface_stop_advertisements( void );

OSStatus peripheral_bt_interface_indicate_attribute_value ( uint16_t connection_handle,
                                                            const mico_bt_ext_attribute_value_t *attribute );

OSStatus peripheral_bt_interface_notify_attribute_value( uint16_t connection_handle,
                                                         const mico_bt_ext_attribute_value_t *attribute );

OSStatus peripheral_bt_interface_update_advertisements_white_list( mico_bool_t add,
                                                                   mico_bt_device_address_t device_address );

OSStatus peripheral_bt_interface_get_advertisements_white_list_size( uint8_t *size );

OSStatus peripheral_bt_interface_set_advertisements_filter_policy(mico_bt_peripheral_adv_filter_policy_t type);

#ifdef __cplusplus
} /* extern "C" */
#endif
