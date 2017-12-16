/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/** @file
 *
 */
#include "smartbt_smartbridge.h"

#include "smartbt.h"
#include "aos_bt_gatt.h"
#include "aos_bt_ble.h"
#include "smartbt_cfg.h"

#include "bt_smartbridge_socket_manager.h"
#include "bt_smartbridge_att_cache_manager.h"
#include "bt_smartbridge_helper.h"
#include "bt_smartbridge_stack_interface.h"

#include "StringUtils.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

gatt_subprocedure_t smartbridge_subprocedure;

extern aos_bt_cfg_settings_t                aos_bt_cfg_settings;
aos_bt_smart_scan_complete_callback_t      app_scan_complete_callback;
aos_bt_smart_advertising_report_callback_t app_scan_report_callback;

aos_bt_dev_ble_io_caps_req_t  default_io_caps_ble  = {
    .bd_addr      = { 0 },
    .local_io_cap = BTM_IO_CAPABILITIES_NONE,
    .oob_data     = 0,
    .auth_req     = BTM_LE_AUTH_REQ_BOND | BTM_LE_AUTH_REQ_MITM, /* BTM_LE_AUTH_REQ_SC_MITM_BOND */
    .max_key_size = 16,
    .init_keys    = (BTM_LE_KEY_PENC | BTM_LE_KEY_PID | BTM_LE_KEY_PCSRK | BTM_LE_KEY_PLK), // init_keys - Keys to be distributed, bit mask
    .resp_keys    = (BTM_LE_KEY_PENC | BTM_LE_KEY_PID | BTM_LE_KEY_PCSRK | BTM_LE_KEY_PLK) // resp_keys - Keys to be distributed, bit mask
};

extern aos_bt_gatt_status_t   smartbridge_gatt_callback                   ( aos_bt_gatt_evt_t event,
                                                                            aos_bt_gatt_event_data_t *p_event_data );

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus smartbridge_bt_interface_initialize( void )
{
    OSStatus result;

    bt_smartbridge_log( "Initializing Bluetooth Interface..." );

    result = aos_rtos_init_mutex( &smartbridge_subprocedure.mutex );
    if ( result  != AOS_BT_SUCCESS ) {
        bt_smartbridge_log( "Error creating mutex" );
        return result;
    }

    result = aos_rtos_init_semaphore( &smartbridge_subprocedure.done_semaphore, 1 );
    if ( result != AOS_BT_SUCCESS ) {
        bt_smartbridge_log( "Error creating semaphore" );
        return result;
    }
    subprocedure_reset( &smartbridge_subprocedure );

    aos_bt_gatt_register( GATT_IF_CLIENT, smartbridge_gatt_callback );
    return AOS_BT_SUCCESS;
}

OSStatus smartbridge_bt_interface_deinitialize( void )
{
    bt_smartbridge_log( "Deinitializing Bluetooth Interface..." );

    subprocedure_reset( &smartbridge_subprocedure );
    aos_rtos_deinit_mutex( &smartbridge_subprocedure.mutex );
    aos_rtos_deinit_semaphore( &smartbridge_subprocedure.done_semaphore );

    return AOS_BT_SUCCESS;
}

OSStatus smartbridge_bt_interface_discover_all_primary_services( uint16_t connection_handle,
                                                                 aos_bt_smart_attribute_list_t *service_list )
{
    aos_bt_gatt_discovery_param_t parameter;
    bt_smartbridge_log( "Discover all Primary Services" );

    subprocedure_lock( &smartbridge_subprocedure );

    subprocedure_reset( &smartbridge_subprocedure );

    /* FIXME : reset() above is not taking effect. Need to be debugged */
    smartbridge_subprocedure.attr_count = 0;
    memset( &parameter.uuid, 0, sizeof( parameter.uuid ) );
    parameter.s_handle = 1;
    parameter.e_handle = 0xffff;

    smartbridge_subprocedure.end_handle = 0xffff;
    smartbridge_subprocedure.start_handle = 1;

    aos_bt_gatt_send_discover( connection_handle, GATT_DISCOVER_SERVICES_ALL, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        service_list->count = smartbridge_subprocedure.attr_count;
        service_list->list  = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_list_t list;

        list.count = smartbridge_subprocedure.attr_count;
        list.list  = smartbridge_subprocedure.attr_head;

        aos_bt_smart_attribute_delete_list( &list );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_discover_primary_services_by_uuid( uint16_t connection_handle,
                                                                     const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_list_t *service_list )
{
    aos_bt_gatt_discovery_param_t parameter;
    bt_smartbridge_log( "Discover all Primary Services(By-UUID)" );

    subprocedure_lock( &smartbridge_subprocedure );

    subprocedure_reset( &smartbridge_subprocedure );

    /* FIXME : reset() above is not taking effect. Need to be debugged */
    smartbridge_subprocedure.attr_count = 0;
    memcpy( &parameter.uuid, uuid, sizeof( parameter.uuid ) );

    parameter.s_handle = smartbridge_subprocedure.end_handle = 0x0001;
    parameter.e_handle = smartbridge_subprocedure.start_handle = 0xffff;

    aos_bt_gatt_send_discover( connection_handle, GATT_DISCOVER_SERVICES_BY_UUID, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        service_list->count = smartbridge_subprocedure.attr_count;
        service_list->list  = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_list_t list;

        list.count = smartbridge_subprocedure.attr_count;
        list.list  = smartbridge_subprocedure.attr_head;

        aos_bt_smart_attribute_delete_list( &list );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_find_included_services( uint16_t connection_handle, uint16_t start_handle,
                                                          uint16_t end_handle, aos_bt_smart_attribute_list_t *include_list )
{

    aos_bt_gatt_discovery_param_t parameter;
    bt_smartbridge_log( "Discover all Included Services" );

    subprocedure_lock( &smartbridge_subprocedure );

    subprocedure_reset( &smartbridge_subprocedure );

    /* FIXME : reset() above is not taking effect. Need to be debugged */
    smartbridge_subprocedure.attr_count = 0;
    memset( &parameter.uuid, 0, sizeof( parameter.uuid ) );

    parameter.s_handle = smartbridge_subprocedure.end_handle = start_handle;
    parameter.e_handle = smartbridge_subprocedure.start_handle = end_handle;

    aos_bt_gatt_send_discover( connection_handle, GATT_DISCOVER_INCLUDED_SERVICES, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        include_list->count = smartbridge_subprocedure.attr_count;
        include_list->list  = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_list_t list;

        list.count = smartbridge_subprocedure.attr_count;
        list.list  = smartbridge_subprocedure.attr_head;

        aos_bt_smart_attribute_delete_list( &list );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_discover_all_characteristics_in_a_service( uint16_t connection_handle,
                                                                             uint16_t start_handle, uint16_t end_handle, aos_bt_smart_attribute_list_t *characteristic_list )
{
    aos_bt_gatt_discovery_param_t parameter;

    subprocedure_lock( &smartbridge_subprocedure );
    bt_smartbridge_log( "Discover Characteristics by Service[%x %x]", start_handle, end_handle );

    subprocedure_reset( &smartbridge_subprocedure );
    smartbridge_subprocedure.attr_count = 0;

    memset( &parameter.uuid, 0, sizeof( parameter.uuid ) );
    parameter.s_handle = start_handle;
    parameter.e_handle = end_handle;

    smartbridge_subprocedure.end_handle = end_handle;
    smartbridge_subprocedure.start_handle = start_handle;

    aos_bt_gatt_send_discover( connection_handle, GATT_DISCOVER_CHARACTERISTICS, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );


    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        characteristic_list->count = smartbridge_subprocedure.attr_count;
        characteristic_list->list  = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_list_t list;

        list.count = smartbridge_subprocedure.attr_count;
        list.list  = smartbridge_subprocedure.attr_head;

        aos_bt_smart_attribute_delete_list( &list );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_discover_characteristic_by_uuid( uint16_t connection_handle,
                                                                   const aos_bt_uuid_t *uuid, uint16_t start_handle, uint16_t end_handle,
                                                                   aos_bt_smart_attribute_list_t *characteristic_list )
{
    UNUSED_PARAMETER(characteristic_list);
    aos_bt_gatt_discovery_param_t parameter;

    subprocedure_lock( &smartbridge_subprocedure );
    bt_smartbridge_log( "Discover Characteristics by UUID [%x %x]", start_handle, end_handle );

    subprocedure_reset( &smartbridge_subprocedure );
    smartbridge_subprocedure.attr_count = 0;

    memcpy( &parameter.uuid, uuid, sizeof( parameter.uuid ) );

    parameter.s_handle = smartbridge_subprocedure.start_handle  = start_handle;
    parameter.e_handle = smartbridge_subprocedure.end_handle    = end_handle;

    aos_bt_gatt_send_discover( connection_handle, GATT_DISCOVER_CHARACTERISTICS, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );


    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        characteristic_list->count = smartbridge_subprocedure.attr_count;
        characteristic_list->list  = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_list_t list;

        list.count = smartbridge_subprocedure.attr_count;
        list.list  = smartbridge_subprocedure.attr_head;

        aos_bt_smart_attribute_delete_list( &list );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_discover_all_characteristic_descriptors( uint16_t connection_handle,
                                                                           uint16_t start_handle, uint16_t end_handle, aos_bt_smart_attribute_list_t *no_value_descriptor_list )
{

    aos_bt_gatt_discovery_param_t parameter;
    bt_smartbridge_log( "Discover all Characteristic Descriptors[%x %x]", start_handle, end_handle );
    subprocedure_lock( &smartbridge_subprocedure );
    subprocedure_reset( &smartbridge_subprocedure );

    smartbridge_subprocedure.attr_count = 0;

    memset( &parameter.uuid, 0, sizeof( parameter.uuid ) );
    parameter.s_handle = start_handle;
    parameter.e_handle = end_handle;

    smartbridge_subprocedure.start_handle      = start_handle;
    smartbridge_subprocedure.end_handle        = end_handle;
    smartbridge_subprocedure.connection_handle = connection_handle;

    aos_bt_gatt_send_discover( connection_handle, GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS, &parameter );
    subprocedure_wait_for_completion( &smartbridge_subprocedure);

    bt_smartbridge_log( "Discover complete: %d", smartbridge_subprocedure.result );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        no_value_descriptor_list->count = smartbridge_subprocedure.attr_count;
        no_value_descriptor_list->list  = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_list_t list;

        list.count = smartbridge_subprocedure.attr_count;
        list.list  = smartbridge_subprocedure.attr_head;

        aos_bt_smart_attribute_delete_list( &list );
    }

    subprocedure_unlock( &smartbridge_subprocedure );
    return smartbridge_subprocedure.result;

}

OSStatus smartbridge_bt_interface_read_characteristic_descriptor( uint16_t connection_handle, uint16_t handle,
                                                                  const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **descriptor )
{
    aos_bt_gatt_read_param_t parameter;
    bt_smartbridge_log( "Read Characteristic Descriptor" );

    subprocedure_lock( &smartbridge_subprocedure );
    subprocedure_reset( &smartbridge_subprocedure );


    parameter.by_handle.auth_req = GATT_AUTH_REQ_NONE;
    parameter.by_handle.handle   = handle;

    smartbridge_subprocedure.attr_count     = 0;
    smartbridge_subprocedure.subprocedure   = GATT_READ_CHARACTERISTIC_DESCRIPTORS;
    smartbridge_subprocedure.start_handle   = handle;
    smartbridge_subprocedure.uuid.len       = uuid->len;
    smartbridge_subprocedure.connection_handle = connection_handle;

    if ( uuid->len == UUID_16BIT ) {
        smartbridge_subprocedure.uuid.uu.uuid16 = uuid->uu.uuid16;
    }

    else if ( uuid->len == UUID_128BIT ) {
        memcpy( smartbridge_subprocedure.uuid.uu.uuid128, uuid->uu.uuid128,  UUID_128BIT );
    }

    aos_bt_gatt_send_read( connection_handle, GATT_READ_BY_HANDLE, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        *descriptor = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_delete( smartbridge_subprocedure.attr_head );
    }

    subprocedure_unlock( &smartbridge_subprocedure );
    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_read_characteristic_value( uint16_t connection_handle, uint16_t handle,
                                                             const aos_bt_uuid_t *type, aos_bt_smart_attribute_t **characteristic_value )
{
    aos_bt_gatt_read_param_t parameter;
    bt_smartbridge_log( "Read Characteristic Value" );

    subprocedure_lock( &smartbridge_subprocedure );
    subprocedure_reset( &smartbridge_subprocedure );

    parameter.by_handle.auth_req    = GATT_AUTH_REQ_NONE;
    parameter.by_handle.handle      = handle;

    smartbridge_subprocedure.subprocedure       = GATT_READ_CHARACTERISTIC_VALUE;
    smartbridge_subprocedure.attr_count         = 0;
    smartbridge_subprocedure.start_handle       = handle;
    smartbridge_subprocedure.uuid.len           = type->len;
    smartbridge_subprocedure.connection_handle  = connection_handle;

    if ( type->len == UUID_16BIT ) {
        smartbridge_subprocedure.uuid.uu.uuid16 = type->uu.uuid16;
    } else if ( type->len == UUID_128BIT ) {
        memcpy( (uint8_t *)smartbridge_subprocedure.uuid.uu.uuid128, (uint8_t *)type->uu.uuid128,  UUID_128BIT );
    }

    aos_bt_gatt_send_read( connection_handle, GATT_READ_BY_HANDLE, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        *characteristic_value = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_delete( smartbridge_subprocedure.attr_head );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_read_characteristic_values_using_uuid( uint16_t connection_handle,
                                                                         const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_list_t *characteristic_value_list )
{
    aos_bt_gatt_read_param_t parameter;
    bt_smartbridge_log( "Read Characteristic Value" );

    subprocedure_lock( &smartbridge_subprocedure );
    subprocedure_reset( &smartbridge_subprocedure );

    parameter.char_type.auth_req    = GATT_AUTH_REQ_NONE;
    parameter.char_type.s_handle    = smartbridge_subprocedure.start_handle = 0x0001;
    parameter.char_type.e_handle    = smartbridge_subprocedure.end_handle   = 0xffff;

    memcpy( &parameter.char_type.uuid, uuid,  sizeof(aos_bt_uuid_t) );

    smartbridge_subprocedure.subprocedure       = GATT_READ_USING_CHARACTERISTIC_UUID;
    smartbridge_subprocedure.attr_count         = 0;
    smartbridge_subprocedure.uuid.len           = uuid->len;
    smartbridge_subprocedure.connection_handle  = connection_handle;

    if ( uuid->len == UUID_16BIT ) {
        smartbridge_subprocedure.uuid.uu.uuid16 = uuid->uu.uuid16;
    } else if ( uuid->len == UUID_128BIT ) {
        memcpy( smartbridge_subprocedure.uuid.uu.uuid128, uuid->uu.uuid128,  UUID_128BIT );
    }

    aos_bt_gatt_send_read( connection_handle, GATT_READ_CHAR_VALUE, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        characteristic_value_list->count = smartbridge_subprocedure.attr_count;
        characteristic_value_list->list  = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_delete( smartbridge_subprocedure.attr_head );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    bt_smartbridge_log( "Read Long Characteristic Value result: %d", smartbridge_subprocedure.result );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_read_long_characteristic_value( uint16_t connection_handle, uint16_t handle,
                                                                  const aos_bt_uuid_t *type, aos_bt_smart_attribute_t **characteristic_value )
{
    aos_bt_gatt_read_param_t parameter;
    bt_smartbridge_log( "Read Long Characteristic Value" );

    subprocedure_lock( &smartbridge_subprocedure );
    subprocedure_reset( &smartbridge_subprocedure );

    parameter.partial.auth_req    = GATT_AUTH_REQ_NONE;
    parameter.partial.handle      = handle;
    parameter.partial.offset      = 0;

    smartbridge_subprocedure.subprocedure       = GATT_READ_LONG_CHARACTERISTIC_VALUES;
    smartbridge_subprocedure.attr_count         = 0;
    smartbridge_subprocedure.start_handle       = handle;
    smartbridge_subprocedure.uuid.len           = type->len;
    smartbridge_subprocedure.connection_handle  = connection_handle;

    if ( type->len == UUID_16BIT ) {
        smartbridge_subprocedure.uuid.uu.uuid16 = type->uu.uuid16;
    } else if ( type->len == UUID_128BIT ) {
        memcpy( smartbridge_subprocedure.uuid.uu.uuid128, type->uu.uuid128,  UUID_128BIT );
    }

    aos_bt_gatt_send_read( connection_handle, GATT_READ_PARTIAL, &parameter );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    if ( smartbridge_subprocedure.result == AOS_BT_SUCCESS ) {
        *characteristic_value = smartbridge_subprocedure.attr_head;
    } else {
        /* Clean up */
        aos_bt_smart_attribute_delete( smartbridge_subprocedure.attr_head );
    }

    subprocedure_unlock( &smartbridge_subprocedure );

    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_read_long_characteristic_descriptor( uint16_t connection_handle, uint16_t handle,
                                                                       const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **descriptor )
{

    UNUSED_PARAMETER(connection_handle);
    UNUSED_PARAMETER(handle);
    UNUSED_PARAMETER(uuid);
    UNUSED_PARAMETER(descriptor);
    return AOS_BT_UNSUPPORTED;
}

OSStatus smartbridge_bt_interface_write_characteristic_descriptor(  uint16_t connection_handle,
                                                                    aos_bt_smart_attribute_t *attribute )
{
    uint8_t                buffer[100] = { 0 };
    aos_bt_gatt_value_t  *write_value = (aos_bt_gatt_value_t *)buffer;

    bt_smartbridge_log( "Write Characteristic Descriptor" );
#if 0
    if ( attribute->value_length > LEATT_ATT_MTU - sizeof(LEATT_PDU_WRITE_HDR) ) {
        BT_DEBUG_PRINT( ( "[GATT] Write Characteristic Value: value too long\n" ) );
        return AOS_BT_ATTRIBUTE_VALUE_TOO_LONG;
    }
#endif
    subprocedure_lock( &smartbridge_subprocedure );
    subprocedure_reset( &smartbridge_subprocedure );

    smartbridge_subprocedure.subprocedure       = GATT_WRITE_CHARACTERISTIC_DESCRIPTORS;
    smartbridge_subprocedure.connection_handle  = connection_handle;

    write_value->auth_req           = GATT_AUTH_REQ_NONE;
    write_value->handle             = attribute->handle;
    write_value->len                = attribute->value_length;
    write_value->offset             = 0;

    memcpy( write_value->value, attribute->value.value, attribute->value_length );

    aos_bt_gatt_send_write( connection_handle, GATT_WRITE, write_value );

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    subprocedure_unlock( &smartbridge_subprocedure );
    return smartbridge_subprocedure.result;
}

OSStatus smartbridge_bt_interface_write_characteristic_value( uint16_t connection_handle,
                                                              aos_bt_smart_attribute_t *attribute )
{
    OSStatus               err = oNoErr;
    uint8_t                buffer[100] = { 0 };
    aos_bt_gatt_value_t  *write_value = (aos_bt_gatt_value_t *)buffer;

    bt_smartbridge_log( "Write Characteristic value" );

    subprocedure_lock( &smartbridge_subprocedure );

    subprocedure_reset( &smartbridge_subprocedure );

    write_value->auth_req = GATT_AUTH_REQ_NONE;
    write_value->handle   = attribute->handle;
    write_value->len      = attribute->value_length;
    write_value->offset   = 0;

    memcpy( write_value->value, attribute->value.value, attribute->value_length );

    err = aos_bt_gatt_send_write( connection_handle, GATT_WRITE, write_value );
    require_noerr(err, exit);

    subprocedure_wait_for_completion( &smartbridge_subprocedure );

    err = smartbridge_subprocedure.result;

exit:
    subprocedure_unlock( &smartbridge_subprocedure );
    return err;
}

OSStatus smartbridge_bt_interface_write_long_characteristic_value( uint16_t connection_handle,
                                                                   aos_bt_smart_attribute_t *attribute )
{
    UNUSED_PARAMETER(connection_handle);
    UNUSED_PARAMETER(attribute);
    return AOS_BT_UNSUPPORTED;
}

OSStatus smartbridge_bt_interface_write_long_characteristic_descriptor( uint16_t connection_handle,
                                                                        const aos_bt_smart_attribute_t *descriptor )
{

    UNUSED_PARAMETER( connection_handle );
    UNUSED_PARAMETER( descriptor );

    return AOS_BT_UNSUPPORTED;
}

static void smartbridge_scan_result_callback( aos_bt_ble_scan_results_t *p_scan_result, uint8_t *p_adv_data )
{
    if ( p_scan_result ) {
        aos_bt_smart_advertising_report_t advertising_report;
        aos_bt_smart_scan_result_t       *current_scan_result = NULL;

        uint8_t         adv_data_length;
        uint8_t         *p = p_adv_data;
        uint8_t         adv_type;
        uint8_t         i;

        memset( &advertising_report, 0x0, sizeof(aos_bt_smart_advertising_report_t) );
        advertising_report.remote_device.address[0] = p_scan_result->remote_bd_addr[0];
        advertising_report.remote_device.address[1] = p_scan_result->remote_bd_addr[1];
        advertising_report.remote_device.address[2] = p_scan_result->remote_bd_addr[2];
        advertising_report.remote_device.address[3] = p_scan_result->remote_bd_addr[3];
        advertising_report.remote_device.address[4] = p_scan_result->remote_bd_addr[4];
        advertising_report.remote_device.address[5] = p_scan_result->remote_bd_addr[5];


        advertising_report.remote_device.address_type = (aos_bt_smart_address_type_t)p_scan_result->ble_addr_type;
        advertising_report.signal_strength            = p_scan_result->rssi;
        advertising_report.event                      = (aos_bt_smart_advertising_event_t)p_scan_result->ble_evt_type;
        advertising_report.eir_data_length = 0;

        bt_smartbridge_log( "address:%x %x %x %x %x %x rssi:%d bdaddrtype:%d event_type:%x length:%u",
                            p_scan_result->remote_bd_addr[0], p_scan_result->remote_bd_addr[1], p_scan_result->remote_bd_addr[2],
                            p_scan_result->remote_bd_addr[3], p_scan_result->remote_bd_addr[4], p_scan_result->remote_bd_addr[5],
                            p_scan_result->rssi, p_scan_result->ble_addr_type, p_scan_result->ble_evt_type, p_scan_result->length );

        if (p_scan_result->ble_evt_type > 4) {
            return;
        }

        if ( smartbridge_helper_find_device_in_scan_result_list( &advertising_report.remote_device.address,
                                                                 advertising_report.remote_device.address_type, &current_scan_result ) == AOS_BT_ITEM_NOT_IN_LIST ) {
            /* This is a new result. Create new result object and add to the list */
            current_scan_result = (aos_bt_smart_scan_result_t *)malloc_named( "scanres", sizeof( *current_scan_result ) );
            if ( !current_scan_result ) {
                bt_smartbridge_log( "Failed to alloc memory for scan-list" );
                return;
            }

            memset( current_scan_result, 0, sizeof( *current_scan_result ) );

            current_scan_result->remote_device.address[0] = p_scan_result->remote_bd_addr[0];
            current_scan_result->remote_device.address[1] = p_scan_result->remote_bd_addr[1];
            current_scan_result->remote_device.address[2] = p_scan_result->remote_bd_addr[2];
            current_scan_result->remote_device.address[3] = p_scan_result->remote_bd_addr[3];
            current_scan_result->remote_device.address[4] = p_scan_result->remote_bd_addr[4];
            current_scan_result->remote_device.address[5] = p_scan_result->remote_bd_addr[5];

            current_scan_result->remote_device.address_type = (aos_bt_smart_address_type_t)p_scan_result->ble_addr_type;

            smartbridge_helper_add_scan_result_to_list( current_scan_result );
        }


        current_scan_result->signal_strength  = p_scan_result->rssi;

        if ( p_scan_result->length > 0 ) {
            STREAM_TO_UINT8( adv_data_length, p );
            while ( adv_data_length && (p - p_adv_data <= p_scan_result->length ) ) {
                advertising_report.eir_data_length += ( adv_data_length + 1 );

                STREAM_TO_UINT8( adv_type, p );

                if ( adv_type == 0x09 || adv_type == 0x08 ) {
                    uint8_t j;
                    for ( i = 0, j = 0; i < ( adv_data_length - 1 ) && j <= p_scan_result->length; i++, j++ ) {
                        advertising_report.remote_device.name[j] = p[i];
                        current_scan_result->remote_device.name[j] = p[i];
                    }
                    bt_smartbridge_log( "Name of Device: %s", advertising_report.remote_device.name );
                }

                p += adv_data_length - 1; /* skip the type of data */
                STREAM_TO_UINT8( adv_data_length, p );
            }
        }
        memcpy(advertising_report.eir_data, p_adv_data, advertising_report.eir_data_length);

        if ( p_scan_result->ble_evt_type == BT_SMART_SCAN_RESPONSE_EVENT ) {
            bt_smartbridge_log( "Received SCAN_RSP\r\n" );
            memcpy( &current_scan_result->last_scan_response_received, &advertising_report,
                    sizeof(aos_bt_smart_advertising_report_t) );
            if ( app_scan_report_callback != NULL ) {
                bt_smartbridge_log( "advertising callback reported" );
                aos_rtos_send_asynchronous_event( AOS_BT_EVT_WORKER_THREAD,
                                                  (event_handler_t)app_scan_report_callback,
                                                  &current_scan_result->last_scan_response_received );
            }
        } else {
            bt_smartbridge_log( "Received ADV[%lu], %d\r\n", (uint32_t)p_scan_result->ble_evt_type,
                                advertising_report.eir_data_length );
            memcpy( &current_scan_result->last_advertising_event_received, &advertising_report,
                    sizeof(aos_bt_smart_advertising_report_t) );
            if ( app_scan_report_callback != NULL ) {
                bt_smartbridge_log( "advertising callback reported" );
                aos_rtos_send_asynchronous_event( AOS_BT_EVT_WORKER_THREAD,
                                                  (event_handler_t)app_scan_report_callback,
                                                  &current_scan_result->last_advertising_event_received );
            }
        }
    }

    else {
        bt_smartbridge_log( "LE scan completed." );
        if ( app_scan_complete_callback != NULL ) {
            aos_rtos_send_asynchronous_event( AOS_BT_EVT_WORKER_THREAD, app_scan_complete_callback, NULL );
        }
    }
}

OSStatus smartbridge_bt_interface_set_attribute_timeout( uint32_t timeout_seconds )
{
    UNUSED_PARAMETER(timeout_seconds);
    return AOS_BT_UNSUPPORTED;
}

OSStatus smartbridge_bt_interface_update_background_connection_device( aos_bool_t add,
                                                                       aos_bt_device_address_t device_address )
{
    if ( device_address == 0 ) {
        return oParamErr;
    }
    if ( add ) {
        if ( aos_bt_ble_update_background_connection_device( AOS_TRUE, device_address ) != TRUE ) {
            return oGeneralErr;
        }
    } else {
        if ( aos_bt_ble_update_background_connection_device( AOS_FALSE, device_address ) != TRUE ) {
            return oGeneralErr;
        }
    }
    return oNoErr;
}

OSStatus smartbridge_bt_interface_get_background_connection_device_size( uint8_t *size )
{
    OSStatus status = oNoErr;
    if (TRUE != aos_bt_ble_get_background_connection_device_size(size)) {
        status = oGeneralErr;
    }
    return status;
}

OSStatus smartbridge_bt_interface_set_background_connection_type(aos_bt_smartbridge_auto_connection_type_t type,
                                                                 const aos_bt_smart_scan_settings_t *settings, aos_bt_smartbridge_auto_connection_parms_cback_t p_select_cback)
{
    OSStatus status = oNoErr;
    aos_bt_ble_conn_type_t conn_type;

    switch (type) {
        case SMARTBRIDGE_CONN_AUTO:
            conn_type = BTM_BLE_CONN_AUTO;
            break;
        case SMARTBRIDGE_CONN_SELECTIVE:
            conn_type = BTM_BLE_CONN_SELECTIVE;
            break;
        case SMARTBRIDGE_CONN_NONE:
            conn_type = BTM_BLE_CONN_NONE;
            break;
        default:
            return oParamErr;
    }

    /* fill with the settings provided by the smartbridge-application */
    if (settings != NULL) {
        aos_bt_cfg_settings.ble_scan_cfg.scan_mode               = settings->type;
        aos_bt_cfg_settings.ble_scan_cfg.high_duty_scan_window   = settings->window;
        aos_bt_cfg_settings.ble_scan_cfg.high_duty_scan_interval = settings->interval;
    } else {
        /* Used the default. */
    }

    if (!aos_bt_ble_set_background_connection_type(conn_type, *(aos_bt_ble_selective_conn_cback_t *)p_select_cback)) {
        status = oGeneralErr;
    }
    return status;
}

OSStatus smartbridge_bt_interface_cancel_last_connect( aos_bt_device_address_t address )
{
    return aos_bt_gatt_cancel_connect( address, AOS_TRUE );
}

OSStatus smartbridge_bt_interface_set_connection_tx_power( uint16_t connection_handle, int8_t transmit_power_dbm )
{
    return AOS_BT_UNSUPPORTED;
}

aos_bool_t smartbridge_bt_interface_is_scanning( void )
{
    aos_bt_ble_scan_type_t  scan_type;

    scan_type = aos_bt_ble_get_current_scan_state();

    if ( scan_type != BTM_BLE_SCAN_TYPE_NONE ) {
        return AOS_TRUE;
    }

    return AOS_FALSE;
}

OSStatus smartbridge_bt_interface_set_max_concurrent_connections( uint8_t count )
{
    /* Just update the Stack's configuration settings */
    aos_bt_cfg_settings.max_simultaneous_links = count;
    aos_bt_cfg_settings.gatt_cfg.client_max_links = count;
    return AOS_BT_SUCCESS;
}

OSStatus smartbridge_bt_interface_stop_scan( )
{
    app_scan_complete_callback = NULL;
    app_scan_report_callback   = NULL;

    return aos_bt_ble_scan( BTM_BLE_SCAN_TYPE_NONE, AOS_TRUE, smartbridge_scan_result_callback );
}

OSStatus smartbridge_bt_interface_start_scan( const aos_bt_smart_scan_settings_t *settings,
                                              aos_bt_smart_scan_complete_callback_t complete_callback,
                                              aos_bt_smart_advertising_report_callback_t advertising_report_callback )
{
    aos_bool_t duplicate_filter_enabled = AOS_FALSE;

    /* First delete the previous scan result list */
    smartbridge_helper_delete_scan_result_list();

    /* fill with the settings provided by the smartbridge-application */
    aos_bt_cfg_settings.ble_scan_cfg.scan_mode               = settings->type;
    aos_bt_cfg_settings.ble_scan_cfg.high_duty_scan_window   = settings->window;
    aos_bt_cfg_settings.ble_scan_cfg.high_duty_scan_duration = settings->duration_second;
    aos_bt_cfg_settings.ble_scan_cfg.high_duty_scan_interval = settings->interval;
    aos_bt_cfg_settings.ble_scan_cfg.scan_filter_policy      = settings->filter_policy;
    duplicate_filter_enabled                                  = settings->filter_duplicates;

    app_scan_complete_callback = complete_callback;
    app_scan_report_callback   = advertising_report_callback;

    return aos_bt_ble_scan( BTM_BLE_SCAN_TYPE_HIGH_DUTY, duplicate_filter_enabled, smartbridge_scan_result_callback );
}

OSStatus smartbridge_bt_interface_connect( const aos_bt_smart_device_t *remote_device,
                                           const aos_bt_smart_connection_settings_t *settings, aos_bt_smartbridge_disconnection_callback_t disconnection_callback,
                                           aos_bt_smartbridge_notification_callback_t notification_callback )
{
    aos_bool_t gatt_connect_result;

    UNUSED_PARAMETER(disconnection_callback);
    UNUSED_PARAMETER(notification_callback);

    /* Update the Stack's configuration */
    aos_bt_cfg_settings.ble_scan_cfg.conn_min_interval = settings->interval_min;
    aos_bt_cfg_settings.ble_scan_cfg.conn_max_interval = settings->interval_max;
    aos_bt_cfg_settings.ble_scan_cfg.conn_latency = settings->latency;
    aos_bt_cfg_settings.ble_scan_cfg.conn_supervision_timeout = settings->supervision_timeout;

    /* Send connection request */
    gatt_connect_result = aos_bt_gatt_le_connect( (uint8_t *)remote_device->address, remote_device->address_type,
                                                  BLE_CONN_MODE_HIGH_DUTY, AOS_TRUE);

    bt_smartbridge_log( "LE-connect, result:%d", gatt_connect_result );

    return gatt_connect_result;
}

OSStatus smartbridge_bt_interface_disconnect( uint16_t connection_handle )
{
    /* First delete the previous scan result list */
    smartbridge_helper_delete_scan_result_list();

    return aos_bt_gatt_disconnect( connection_handle );
}

