/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "smartbt_smartbridge.h"
#include "smartbt_smartbridge_gatt.h"
#include "bt_smartbridge_stack_interface.h"
#include "bt_smartbridge_helper.h"

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

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus aos_bt_smartbridge_gatt_discover_all_primary_services( const aos_bt_smartbridge_socket_t *socket,
                                                                aos_bt_smart_attribute_list_t *service_list )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || service_list == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );

    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_discover_all_primary_services( socket->connection_handle, service_list );
}

OSStatus aos_bt_smartbridge_gatt_discover_primary_services_by_uuid( const aos_bt_smartbridge_socket_t *socket,
                                                                    const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_list_t *service_list )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || uuid == NULL || service_list == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_discover_primary_services_by_uuid( socket->connection_handle, uuid, service_list );
}

OSStatus aos_bt_smartbridge_gatt_find_included_services( const aos_bt_smartbridge_socket_t *socket,
                                                         uint16_t start_handle, uint16_t end_handle, aos_bt_smart_attribute_list_t *include_list )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || include_list == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_find_included_services( socket->connection_handle, start_handle, end_handle,
                                                            include_list );
}

OSStatus aos_bt_smartbridge_gatt_discover_all_characteristics_in_a_service( const aos_bt_smartbridge_socket_t *socket,
                                                                            uint16_t start_handle, uint16_t end_handle, aos_bt_smart_attribute_list_t *characteristic_list )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || characteristic_list == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }
    return smartbridge_bt_interface_discover_all_characteristics_in_a_service( socket->connection_handle, start_handle,
                                                                               end_handle, characteristic_list );
}

OSStatus aos_bt_smartbridge_gatt_discover_characteristic_by_uuid( const aos_bt_smartbridge_socket_t *socket,
                                                                  const aos_bt_uuid_t *uuid, uint16_t start_handle, uint16_t end_handle,
                                                                  aos_bt_smart_attribute_list_t *characteristic_list )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || uuid == NULL || characteristic_list == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_discover_characteristic_by_uuid( socket->connection_handle, uuid, start_handle,
                                                                     end_handle, characteristic_list );
}

OSStatus aos_bt_smartbridge_gatt_discover_handle_and_type_of_all_characteristic_descriptors(
    const aos_bt_smartbridge_socket_t *socket, uint16_t start_handle, uint16_t end_handle,
    aos_bt_smart_attribute_list_t *descriptor_list )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || descriptor_list == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_discover_all_characteristic_descriptors( socket->connection_handle, start_handle,
                                                                             end_handle, descriptor_list );
}

OSStatus aos_bt_smartbridge_gatt_read_characteristic_descriptor( const aos_bt_smartbridge_socket_t *socket,
                                                                 uint16_t handle, const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **descriptor )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || uuid == NULL || descriptor == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_read_characteristic_descriptor( socket->connection_handle, handle, uuid, descriptor );
}

OSStatus aos_bt_smartbridge_gatt_read_long_characteristic_descriptor( const aos_bt_smartbridge_socket_t *socket,
                                                                      uint16_t handle, const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **descriptor )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || uuid == NULL || descriptor == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_read_long_characteristic_descriptor( socket->connection_handle, handle, uuid,
                                                                         descriptor );
}

OSStatus aos_bt_smartbridge_gatt_write_characteristic_descriptor( const aos_bt_smartbridge_socket_t *socket,
                                                                  const aos_bt_smart_attribute_t *descriptor )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || descriptor == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_write_characteristic_descriptor( socket->connection_handle,
                                                                     (aos_bt_smart_attribute_t *)descriptor );
}

OSStatus aos_bt_smartbridge_gatt_write_long_characteristic_descriptor( const aos_bt_smartbridge_socket_t *socket,
                                                                       const aos_bt_smart_attribute_t *descriptor )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || descriptor == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_write_long_characteristic_descriptor( socket->connection_handle,
                                                                          (aos_bt_smart_attribute_t *)descriptor );
}

OSStatus aos_bt_smartbridge_gatt_read_characteristic_value( const aos_bt_smartbridge_socket_t *socket, uint16_t handle,
                                                            const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **characteristic_value )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || uuid == NULL || characteristic_value == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_read_characteristic_value( socket->connection_handle, handle, uuid,
                                                               characteristic_value );
}

OSStatus aos_bt_smartbridge_gatt_read_characteristic_values_using_uuid( const aos_bt_smartbridge_socket_t *socket,
                                                                        const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_list_t *characteristic_value_list )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || uuid == NULL || characteristic_value_list == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_read_characteristic_values_using_uuid( socket->connection_handle, uuid,
                                                                           characteristic_value_list );
}

OSStatus aos_bt_smartbridge_gatt_read_long_characteristic_value( const aos_bt_smartbridge_socket_t *socket,
                                                                 uint16_t handle, const aos_bt_uuid_t *uuid, aos_bt_smart_attribute_t **characteristic_value )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || uuid == NULL || characteristic_value == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_read_long_characteristic_value( socket->connection_handle, handle, uuid,
                                                                    characteristic_value );
}

OSStatus aos_bt_smartbridge_gatt_write_characteristic_value_without_response( const aos_bt_smartbridge_socket_t *socket,
                                                                              const aos_bt_smart_attribute_t *characteristic_value )
{
    UNUSED_PARAMETER( socket );
    UNUSED_PARAMETER( characteristic_value );
    return AOS_BT_UNSUPPORTED;
}

OSStatus aos_bt_smartbridge_gatt_signed_write_characteristic_value_without_response(
    const aos_bt_smartbridge_socket_t *socket, const aos_bt_smart_attribute_t *characteristic_value )
{
    UNUSED_PARAMETER( socket );
    UNUSED_PARAMETER( characteristic_value );
    return AOS_BT_UNSUPPORTED;
}

OSStatus aos_bt_smartbridge_gatt_write_characteristic_value( const aos_bt_smartbridge_socket_t *socket,
                                                             const aos_bt_smart_attribute_t *characteristic_value )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || characteristic_value == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }
    return smartbridge_bt_interface_write_characteristic_value( socket->connection_handle,
                                                                (aos_bt_smart_attribute_t *)characteristic_value );
}

OSStatus aos_bt_smartbridge_gatt_write_long_characteristic_value( const aos_bt_smartbridge_socket_t *socket,
                                                                  const aos_bt_smart_attribute_t *characteristic_value )
{
    aos_bt_smartbridge_socket_status_t status;

    if ( socket == NULL || characteristic_value == NULL ) {
        return AOS_BT_BADARG;
    }

    aos_bt_smartbridge_get_socket_status( (aos_bt_smartbridge_socket_t *)socket, &status );
    if ( status != SMARTBRIDGE_SOCKET_CONNECTED ) {
        return AOS_BT_SOCKET_NOT_CONNECTED;
    }

    return smartbridge_bt_interface_write_long_characteristic_value( socket->connection_handle,
                                                                     (aos_bt_smart_attribute_t *)characteristic_value );
}

OSStatus aos_bt_smartbridge_gatt_reliable_write_characteristic_value( const aos_bt_smartbridge_socket_t *socket,
                                                                      const aos_bt_smart_attribute_t *characteristic_value )
{
    UNUSED_PARAMETER( socket );
    UNUSED_PARAMETER( characteristic_value );
    return AOS_BT_UNSUPPORTED;
}
