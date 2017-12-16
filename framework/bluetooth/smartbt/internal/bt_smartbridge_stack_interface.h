/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BT_SMART_BRIDGE_STACK_INTERFACE_H_
#define _BT_SMART_BRIDGE_STACK_INTERFACE_H_

#pragma once

/** @file
 *  Smartbridge's Interface Header with Bluetooth Stack
 */

#include "smartbt_smartbridge.h"
#include "smartbt_smart_interface.h"

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

/**
 * SmartBridge background connection type
 */
typedef enum {
    SMARTBRIDGE_CONN_NONE,                          /**< No background connection */
    SMARTBRIDGE_CONN_AUTO,                          /**< Auto connection */
    SMARTBRIDGE_CONN_SELECTIVE                      /**< Selective connection */
} aos_bt_smartbridge_auto_connection_type_t;


/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus smartbridge_bt_interface_initialize( void );
OSStatus smartbridge_bt_interface_deinitialize( void );

OSStatus  smartbridge_bt_interface_stop_scan( void  );

OSStatus  smartbridge_bt_interface_start_scan( const aos_bt_smart_scan_settings_t *setting,
                                               aos_bt_smart_scan_complete_callback_t complete_callback,
                                               aos_bt_smart_advertising_report_callback_t advertising_report_callback );
aos_bool_t    smartbridge_bt_interface_is_scanning( void );

OSStatus  smartbridge_bt_interface_connect( const aos_bt_smart_device_t *remote_device,
                                            const aos_bt_smart_connection_settings_t *settings, aos_bt_smartbridge_disconnection_callback_t disconnection_callback,
                                            aos_bt_smartbridge_notification_callback_t notification_callback );

OSStatus  smartbridge_bt_interface_cancel_last_connect( aos_bt_device_address_t address );

OSStatus  smartbridge_bt_interface_disconnect( uint16_t connection_handle );

OSStatus  smartbridge_bt_interface_update_background_connection_device( aos_bool_t add,
                                                                        aos_bt_device_address_t device_address );

OSStatus  smartbridge_bt_interface_get_background_connection_device_size( uint8_t *size );

OSStatus  smartbridge_bt_interface_set_background_connection_type(aos_bt_smartbridge_auto_connection_type_t type,
                                                                  const aos_bt_smart_scan_settings_t *settings, aos_bt_smartbridge_auto_connection_parms_cback_t );

OSStatus  smartbridge_bt_interface_set_attribute_timeout( uint32_t timeout_seconds );

OSStatus  smartbridge_bt_interface_set_connection_tx_power( uint16_t connection_handle, int8_t transmit_power_dbm );

OSStatus  smartbridge_bt_interface_set_max_concurrent_connections( uint8_t count );


OSStatus smartbridge_bt_interface_discover_all_primary_services( uint16_t connection_handle,
                                                                 aos_bt_smart_attribute_list_t *service_list );
OSStatus smartbridge_bt_interface_discover_all_characteristics_in_a_service( uint16_t connection_handle,
                                                                             uint16_t start_handle, uint16_t end_handle, aos_bt_smart_attribute_list_t *characteristic_list );
OSStatus smartbridge_bt_interface_discover_all_characteristic_descriptors( uint16_t connection_handle,
                                                                           uint16_t start_handle, uint16_t end_handle, aos_bt_smart_attribute_list_t *no_value_descriptor_list );
OSStatus smartbridge_bt_interface_discover_primary_services_by_uuid( uint16_t connection_handle,
                                                                     const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_list_t *service_list );
OSStatus smartbridge_bt_interface_find_included_services( uint16_t connection_handle, uint16_t start_handle,
                                                          uint16_t end_handle, aos_bt_smart_attribute_list_t *include_list );
OSStatus smartbridge_bt_interface_discover_characteristic_by_uuid( uint16_t connection_handle,
                                                                   const aos_bt_uuid_t *uuid, uint16_t start_handle, uint16_t end_handle,
                                                                   aos_bt_smart_attribute_list_t *characteristic_list );

OSStatus smartbridge_bt_interface_read_characteristic_value( uint16_t connection_handle, uint16_t handle,
                                                             const aos_bt_uuid_t *type, aos_bt_smart_attribute_t **characteristic_value );
OSStatus smartbridge_bt_interface_read_long_characteristic_value( uint16_t connection_handle, uint16_t handle,
                                                                  const aos_bt_uuid_t *type, aos_bt_smart_attribute_t **characteristic_value );
OSStatus smartbridge_bt_interface_read_characteristic_descriptor( uint16_t connection_handle, uint16_t handle,
                                                                  const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **descriptor );
OSStatus smartbridge_bt_interface_read_long_characteristic_descriptor( uint16_t connection_handle, uint16_t handle,
                                                                       const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **descriptor );
OSStatus smartbridge_bt_interface_read_characteristic_values_using_uuid( uint16_t connection_handle,
                                                                         const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_list_t *characteristic_value_list );

OSStatus smartbridge_bt_interface_write_characteristic_value( uint16_t connection_handle,
                                                              aos_bt_smart_attribute_t *attribute );
OSStatus smartbridge_bt_interface_write_long_characteristic_value( uint16_t connection_handle,
                                                                   aos_bt_smart_attribute_t *attribute );
OSStatus smartbridge_bt_interface_write_long_characteristic_descriptor( uint16_t connection_handle,
                                                                        const aos_bt_smart_attribute_t *descriptor );
OSStatus smartbridge_bt_interface_write_characteristic_descriptor(  uint16_t connection_handle,
                                                                    aos_bt_smart_attribute_t *attribute );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
