/** @file
 *
 */
#include "mico.h"
#include "StringUtils.h"
#include "mico_bt_smartbridge.h"

#include "mico_bt_gatt.h"
#include "mico_bt_ble.h"
#include "mico_bt_cfg.h"

#include "mico_bt.h"
#include "bt_smartbridge_socket_manager.h"
#include "bt_smartbridge_att_cache_manager.h"
#include "bt_smartbridge_helper.h"
#include "bt_smartbridge_stack_interface.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define SOCKET_INVALID_CONNECTION_HANDLE       ( 0xFFFF )

#define MAX_CONNECTION_TIMEOUT                 ( 10000 )

#define MAX_AUTO_CONNECTION_SEMA_TIMEOUT       ( 200 )

#define SMARTBRIDGE_AUTO_CONN_SCAN_INTERVAL    ( 600 )

/******************************************************
 *                   Enumerations
 ******************************************************/

/* Auto-connection precedure */
typedef enum {
    /* A device is scanned and ready to report to user. */
    SMARTBRIDGE_AUTO_CONN_STATE_SCANED = 1,
    /* A device is reported to wait user action */
    SMARTBRIDGE_AUTO_CONN_STATE_REPORTED,
    /* A device is not allowed connecting to by user and it will
     * be translate to FREE state.
     */
    SMARTBRIDGE_AUTO_CONN_STATE_IDLE,
    /* A device is alloed connecting to by user. */
    SMARTBRIDGE_AUTO_CONN_STATE_CONNING,
    /* A device is free and it can be reused by SCAN procedure. */
    SMARTBRIDGE_AUTO_CONN_STATE_FREE,
} smartbridge_auto_conn_state_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct {
    uint8_t      bdaddr[6];
    uint8_t      name[16];
    uint8_t      adv_data[31];
    uint8_t      length;
    uint8_t      handle;
} smartbridge_auto_conn_dev_info_t;

/* Local auto connection settings */
typedef struct {
    mico_bt_smartbridge_socket_t
    *socket;                         /**< A socket associated with the auto connection */
    mico_bt_smart_connection_settings_t
    conn_settings;                  /**< The connection settings associated with a socket */
    smartbridge_auto_conn_dev_info_t        device_info;                    /**< The scanned device information */
} smartbridge_auto_conn_entity_t;

/* Device info ready to connect */
typedef struct {
    linked_list_node_t                  this_node;     /* Linked-list node of this device */
    mico_bt_smart_advertising_report_t  report;        /* Remote BT device */
    smartbridge_auto_conn_state_t       state;         /* Auto-connection state */
} smartbridge_auto_conn_report_t;

/* Device info ready to connect */
typedef struct {
    linked_list_t   list;
    mico_mutex_t    mutex;
} smartbridge_auto_conn_list_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static OSStatus smartbridge_app_notification_handler             ( void *arg );
static OSStatus smartbridge_app_disconnection_handler            ( void *arg );

static void        smartbridge_gatt_auto_conn_handler            ( mico_bt_device_address_t bdaddr,
                                                                   uint16_t connection_handle );
static mico_bool_t smartbridge_gap_auto_conn_user_cfg            ( const mico_bt_device_address_t device_address,
                                                                   const uint8_t *device_name, const uint8_t *p_data, uint8_t length );
static OSStatus    smartbridge_gap_auto_conn_asyn_event_handler  ( void *arg );
static OSStatus    smartbridge_gap_auto_conn_scan_report_handler ( const mico_bt_smart_advertising_report_t *report );
static OSStatus    smartbridge_gap_auto_conn_scan_cmpl_handler   ( void *arg );
static OSStatus    smartbridge_gap_auto_conn_cryption            ( void *arg );

static OSStatus    smartbridge_auto_conn_user_parms              ( void *arg );
static int8_t      smartbridge_auto_conn_dev_alloc               ( void );
static int8_t      smartbridge_auto_conn_find_dev_by_addr        ( const mico_bt_device_address_t device_address );
static OSStatus    smartbridge_auto_conn_list_init               ( void );
static OSStatus    smartbridge_auto_conn_list_deinit             ( void );
static OSStatus    smartbridge_auto_conn_list_add                ( const mico_bt_smart_advertising_report_t *report );
static OSStatus    smartbridge_auto_conn_list_remove             ( mico_bt_device_address_t bdaddr );
static OSStatus    smartbridge_auto_conn_list_clear              ( void );
static OSStatus    smartbridge_auto_conn_list_remove_by_state    ( smartbridge_auto_conn_state_t state );
static OSStatus    smartbridge_auto_conn_list_set_state          ( const mico_bt_device_address_t bdaddr,
                                                                   smartbridge_auto_conn_state_t state );
static OSStatus    smartbridge_auto_conn_list_get_by_state       ( mico_bt_smart_advertising_report_t **report,
                                                                   smartbridge_auto_conn_state_t state );

/******************************************************
 *               Variable Definitions
 ******************************************************/

mico_bt_smartbridge_socket_t           *connecting_socket = NULL;
static mico_bool_t                      initialised       = MICO_FALSE;
extern mico_bool_t                      bt_initialised;
extern mico_bt_dev_ble_io_caps_req_t    local_io_caps_ble;
extern gatt_subprocedure_t              smartbridge_subprocedure;

mico_bt_gatt_char_declaration_t         current_characteristic;

/** Auto Connection Establishment Procedure */
static mico_semaphore_t                                       g_auto_conn_sem = NULL;
static smartbridge_auto_conn_entity_t                         g_auto_conn_dev_table[BTM_BLE_MAX_BG_CONN_DEV_NUM];
static mico_bt_smartbridge_auto_connection_parms_cback_t      g_auto_conn_cback = NULL;
static mico_bt_smart_scan_settings_t                          g_auto_conn_scan_cfg;
static uint16_t                                               g_auto_conn_scan_duration = 0;
static smartbridge_auto_conn_list_t                           g_auto_conn_list;

/******************************************************
 *               Function Definitions
 ******************************************************/

static void smartbridge_gatt_connection_handler( uint16_t connection_handle )
{
    bt_smartbridge_log( "GATT connection was SUCCESS" );
    /* Update connection handle and state of the socket */
    connecting_socket->state = SOCKET_STATE_LINK_CONNECTED;

    connecting_socket->connection_handle = connection_handle;

    /* Add socket to the connected list */
    bt_smartbridge_socket_manager_insert_socket( connecting_socket );

    /* Notify app thread that link is connected */
    mico_rtos_set_semaphore( &connecting_socket->semaphore );
}

static void smartbridge_gatt_auto_conn_handler( mico_bt_device_address_t bdaddr, uint16_t connection_handle )
{
    int8_t                                   idx = 0;
    mico_bt_smartbridge_socket_t            *p_socket;

    bt_smartbridge_log("Auto connection [%02x:%02x:%02x:%02x:%02x:%02x] established.",
                       bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5]);

    /* Check connection object */
    idx = smartbridge_auto_conn_find_dev_by_addr(bdaddr);
    if (idx < 0) {
        smartbridge_bt_interface_disconnect(connection_handle);
        bt_smartbridge_log("There is not device in the white list.");
        return;
    }

    /* Check connection state. */
    p_socket = g_auto_conn_dev_table[idx].socket;
    if (p_socket->state == SOCKET_STATE_LINK_CONNECTED
        || p_socket->state == SOCKET_STATE_LINK_ENCRYPTED) {
        bt_smartbridge_log("Incredible error");
        return;
    }
    p_socket->state = SOCKET_STATE_LINK_CONNECTED;

    /* Manage the socket state. */
    smartbridge_helper_socket_clear_actions(p_socket, SOCKET_ACTION_ENCRYPT_USING_BOND_INFO
                                            | SOCKET_ACTION_INITIATE_PAIRING);
    p_socket->connection_handle = connection_handle;
    bt_smartbridge_socket_manager_insert_socket(p_socket);

    /* Tell smartbridge_gap_auto_conn_asyn_event_handler a connection is ready. */
    mico_rtos_set_semaphore(&g_auto_conn_sem);
}

static void smartbridge_gatt_disconnection_handler( uint16_t connection_handle )
{
    bt_smartbridge_log( "GATT disconnection" );

    mico_bt_smartbridge_socket_t *removed_socket = NULL;

    /* Remove socket from the connected list */
    if ( bt_smartbridge_socket_manager_remove_socket( connection_handle, &removed_socket ) == MICO_BT_SUCCESS ) {
        /* Reset connection handle to invalid value */
        removed_socket->connection_handle = SOCKET_INVALID_CONNECTION_HANDLE;

        /* Reset socket state */
        removed_socket->state = SOCKET_STATE_DISCONNECTED;

        /* Mark att cache as inactive and reset reference to cache */
        bt_smartbridge_att_cache_set_active_state( (bt_smartbridge_att_cache_t *)removed_socket->att_cache, MICO_FALSE );
        removed_socket->att_cache = NULL;

        /* Check if disconnection is from host or remote device */
        if ( smartbridge_helper_socket_check_actions_enabled( removed_socket, SOCKET_ACTION_HOST_DISCONNECT ) == MICO_TRUE ) {
            /* Disconnection is originated from the host. Notify app thread that disconnection is complete */
            mico_rtos_set_semaphore( &removed_socket->semaphore );
        } else {
            /* Notify app that connection is disconnected by the remote device */
            if ( removed_socket->disconnection_callback != NULL ) {
                mico_rtos_send_asynchronous_event( MICO_BT_EVT_WORKER_THREAD, smartbridge_app_disconnection_handler,
                                                   (void *)removed_socket );
            }

            /* If disconnection happens when connection is still being established. Notify app */
            if ( connecting_socket == removed_socket ) {
                mico_rtos_set_semaphore( &connecting_socket->semaphore );
            }
        }
    } else {
        /* If disconnection happens when connection is still being established. Notify app */
        if ( connecting_socket != NULL ) {
            mico_rtos_set_semaphore( &connecting_socket->semaphore );
        }
    }
}

static void smartbridge_gatt_read_operation_complete_handler( mico_bt_gatt_data_t *response_data )
{
    mico_bt_smart_attribute_t *attr;
    uint8_t *data = NULL;
    /* Create a new attribute */
    mico_bt_smart_attribute_create( &attr, MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE, response_data->len );

    if ( attr != NULL ) {
        attr->next         = NULL;
        attr->handle       = smartbridge_subprocedure.start_handle;
        attr->type.len     = smartbridge_subprocedure.uuid.len;
        data               = response_data->p_data;
        attr->value_length = response_data->len;

        if ( smartbridge_subprocedure.uuid.len == UUID_16BIT ) {
            attr->type.uu.uuid16 = smartbridge_subprocedure.uuid.uu.uuid16;
        } else if ( smartbridge_subprocedure.uuid.len == UUID_128BIT ) {
            memcpy( attr->type.uu.uuid128, smartbridge_subprocedure.uuid.uu.uuid128,  UUID_128BIT );
        }

        if ( smartbridge_subprocedure.subprocedure == GATT_READ_CHARACTERISTIC_VALUE ) {
            memcpy( attr->value.characteristic_value.value, data, attr->value_length );
        } else if ( smartbridge_subprocedure.subprocedure == GATT_READ_CHARACTERISTIC_DESCRIPTORS) {
            memcpy( &attr->value, data, attr->value_length );
        }

        //memcpy( &attr->type, &subprocedure.uuid, sizeof(mico_bt_uuid_t));

        /* Update temporary variables */
        if ( smartbridge_subprocedure.attr_head == NULL ) {
            smartbridge_subprocedure.attr_head = attr;
        }
        smartbridge_subprocedure.result = MICO_BT_SUCCESS;

        char *debug_str = NULL;

        if ( smartbridge_subprocedure.subprocedure == GATT_READ_CHARACTERISTIC_VALUE ) {
            debug_str = DataToHexStringWithColons( attr->value.characteristic_value.value, attr->value_length );
            bt_smartbridge_log( "Read value: UUID:%x length:%d: [%s]", attr->type.uu.uuid16, attr->type.len, debug_str );
            free( debug_str );
        } else if ( smartbridge_subprocedure.subprocedure == GATT_READ_CHARACTERISTIC_DESCRIPTORS) {
            debug_str = DataToHexStringWithColons( attr->value.value, attr->value_length );
            bt_smartbridge_log( "Read value: UUID:%x length:%d: [%s]", attr->type.uu.uuid16, attr->type.len, debug_str );
            free( debug_str );
        }
    } else {
        smartbridge_subprocedure.result = MICO_BT_OUT_OF_HEAP_SPACE;
    }

}

static OSStatus smartbridge_gatt_notification_indication_handler( mico_bt_gatt_operation_complete_t
                                                                  *operation_complete )
{
    mico_bt_smartbridge_socket_t *socket;
    uint16_t connection_handle  = operation_complete->conn_id;
    uint16_t attribute_handle   = operation_complete->response_data.att_value.handle;
    uint8_t *data               = operation_complete->response_data.att_value.p_data;
    uint8_t  length             = operation_complete->response_data.att_value.len;

    bt_smartbridge_log("Process peer device[0x%04x] %s...",
                       connection_handle,
                       operation_complete->op == GATTC_OPTYPE_INDICATION ? "INDICATION" : "NOTIFICATION");

    /* Send Indication ACK to peer device. */
    if ( operation_complete->op == GATTC_OPTYPE_INDICATION ) {
        bt_smartbridge_log("Confirm peer device's INDICATION");
        if ( mico_bt_gatt_send_indication_confirm( connection_handle, attribute_handle ) != 0 ) {
            bt_smartbridge_log("Send INDICATION confirm failed!!!");
        }
    }

    /* Search for socket with indicated connection handle in the connected list */
    if ( bt_smartbridge_socket_manager_find_socket_by_handle( connection_handle, &socket ) == MICO_BT_SUCCESS ) {
        if ( bt_smartbridge_att_cache_is_enabled() == MICO_TRUE && socket->att_cache != NULL ) {

            bt_smartbridge_att_cache_t      *cache          = (bt_smartbridge_att_cache_t *)socket->att_cache;
            mico_bt_smart_attribute_list_t *att_cache_list = NULL;
            mico_bt_smart_attribute_t      *att            = NULL;

            bt_smartbridge_att_cache_get_list( cache, &att_cache_list );

            /* Socket found. lock mutex for protected access */
            bt_smartbridge_att_cache_lock( cache );

            /* Search for att in the socket's att list */
            if ( mico_bt_smart_attribute_search_list_by_handle( att_cache_list, attribute_handle, &att ) == MICO_BT_SUCCESS ) {
                mico_bt_uuid_t uuid       = att->type;
                mico_bool_t    is_new_att = MICO_FALSE;

                /* Check if existing att memory length is sufficient */
                if ( length > att->value_length ) {
                    /* length isn't sufficient. Remove existing from the list */
                    mico_bt_smart_attribute_remove_from_list( att_cache_list, attribute_handle );
                    att = NULL;

                    /* Create a new one and marked as new */
                    mico_bt_smart_attribute_create( &att, MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE, length );
                    is_new_att = MICO_TRUE;
                }

                /* Copy new value to the att */
                att->handle       = attribute_handle;
                att->type         = uuid;
                att->value_length = length;
                memcpy( att->value.value, data, length );

                if ( is_new_att == MICO_TRUE ) {
                    /* Add newly created att to the list */
                    mico_bt_smart_attribute_add_to_list( att_cache_list, att );
                }
            }

            /* Socket found. lock mutex for protected access */
            bt_smartbridge_att_cache_unlock( cache );
        }

        socket->last_notified_attribute_handle = attribute_handle;

        /* Notification callback is called regardless of att cache is enabled or not */
        if ( socket->notification_callback != NULL ) {
            if ( kNoErr != mico_rtos_send_asynchronous_event( MICO_BT_EVT_WORKER_THREAD, smartbridge_app_notification_handler,
                                                              (void *)socket ) ) {
                bt_smartbridge_log("Send asynchronous event for INDICATION/NOTIFICATION handler failed!!!");
                return MICO_BT_ERROR;
            }
        }

        return MICO_BT_SUCCESS;
    } else {
        bt_smartbridge_log("Unknown connection handle: 0x%04x", connection_handle);
    }

    return MICO_BT_ERROR;
}

static void smartbridge_gatt_discover_characteristic_descriptor_result( mico_bt_gatt_event_data_t *p_event_data )
{
    /* Create attribute(s) based on information included in the response PDU */
    mico_bt_smart_attribute_t *attr;

    mico_bt_smart_attribute_create( &attr, MICO_ATTRIBUTE_TYPE_NO_VALUE, 0 );

    if ( attr != NULL ) {
        attr->next = NULL;
        attr->handle = GATT_DISCOVERY_RESULT_CHARACTERISTIC_DESCRIPTOR_VALUE_HANDLE(p_event_data);
        attr->type.len = GATT_DISCOVERY_RESULT_CHARACTERISTIC_DESCRIPTOR_UUID_LEN(p_event_data);

        if ( attr->type.len == UUID_16BIT ) {
            attr->type.uu.uuid16 = GATT_DISCOVERY_RESULT_CHARACTERISTIC_DESCRIPTOR_UUID16(p_event_data);
        } else if ( attr->type.len == UUID_32BIT ) {
            attr->type.uu.uuid32 = GATT_DISCOVERY_RESULT_CHARACTERISTIC_DESCRIPTOR_UUID32(p_event_data);
        } else if ( attr->type.len == UUID_128BIT ) {
            memcpy( (uint8_t *)attr->type.uu.uuid128, &(GATT_DISCOVERY_RESULT_CHARACTERISTIC_DESCRIPTOR_UUID32(p_event_data)),
                    UUID_128BIT );
        }

        if ( smartbridge_subprocedure.attr_head == NULL ) {
            smartbridge_subprocedure.attr_head = attr;
        }

        if ( smartbridge_subprocedure.attr_tail != NULL ) {
            smartbridge_subprocedure.attr_tail->next = attr;
        }

        smartbridge_subprocedure.attr_tail = attr;
        smartbridge_subprocedure.attr_count++;

        bt_smartbridge_log( "Characteristic Descriptor handle:%x uuid:%x list-count:%u", attr->handle, attr->type.uu.uuid16,
                            (unsigned int)smartbridge_subprocedure.attr_count );
    }
}

static void smartbridge_gatt_discover_services_result( mico_bt_gatt_event_data_t *p_event_data )
{
    /* Create attribute(s) based on information included in the response PDU */
    mico_bt_smart_attribute_t *attr;

    mico_bt_smart_attribute_create( &attr, MICO_ATTRIBUTE_TYPE_PRIMARY_SERVICE, 0 );

    if ( attr != NULL ) {
        attr->next                          = NULL;
        attr->type.len                      = UUID_16BIT;
        attr->type.uu.uuid16                = GATT_UUID_PRI_SERVICE;
        attr->value_length                  = 2;
        attr->handle                        = p_event_data->discovery_result.discovery_data.group_value.s_handle;
        attr->value.service.start_handle    = attr->handle;
        attr->value.service.end_handle      = p_event_data->discovery_result.discovery_data.group_value.e_handle;

        memcpy( &attr->value.service.uuid, &p_event_data->discovery_result.discovery_data.group_value.service_type,
                sizeof(mico_bt_uuid_t) );

        if ( smartbridge_subprocedure.attr_head == NULL ) {
            smartbridge_subprocedure.attr_head = attr;
        }

        if ( smartbridge_subprocedure.attr_tail != NULL ) {
            smartbridge_subprocedure.attr_tail->next = attr;
        }

        smartbridge_subprocedure.attr_tail = attr;

        smartbridge_subprocedure.attr_count++;

        bt_smartbridge_log( "Service [Start %x - End %x] uuid:%x len:%d list-count:%u", attr->handle,
                            attr->value.service.end_handle, attr->value.service.uuid.uu.uuid16, attr->value.service.uuid.len,
                            (unsigned int)smartbridge_subprocedure.attr_count );
    }
}

static void smartbridge_gatt_discover_included_services_result( mico_bt_gatt_event_data_t *p_event_data )
{
    /* Create attribute(s) based on information included in the response PDU */
    mico_bt_smart_attribute_t *attr;
    //uint16_t start_handle = 0x0;

    mico_bt_smart_attribute_create( &attr, MICO_ATTRIBUTE_TYPE_INCLUDE, 0 );

    if ( attr != NULL ) {
        attr->next                                      = NULL;
        attr->type.len                                  = UUID_16BIT;
        attr->type.uu.uuid16                            = GATT_UUID_INCLUDE_SERVICE;
        attr->value_length                              = 2;
        attr->handle                                    = p_event_data->discovery_result.discovery_data.included_service.handle;
        attr->value.include.included_service_handle     = p_event_data->discovery_result.discovery_data.included_service.handle;
        attr->value.include.end_group_handle            =
            p_event_data->discovery_result.discovery_data.included_service.e_handle;
        //start_handle                                    = p_event_data->discovery_result.discovery_data.included_service.s_handle;

        memcpy( &attr->value.include.uuid, &p_event_data->discovery_result.discovery_data.included_service.service_type,
                sizeof(mico_bt_uuid_t) );

        if ( smartbridge_subprocedure.attr_head == NULL ) {
            smartbridge_subprocedure.attr_head = attr;
        }

        if ( smartbridge_subprocedure.attr_tail != NULL ) {
            smartbridge_subprocedure.attr_tail->next = attr;
        }

        smartbridge_subprocedure.attr_tail = attr;

        smartbridge_subprocedure.attr_count++;
        //bt_smartbridge_log( "Included Service handle:%x [Start %x - End %x] uuid:%x", attr->handle, start_handle, attr->value.include.end_group_handle, attr->value.service.uuid.uu.uuid16 );
    }
}

static void smartbridge_gatt_discover_characteristic_result(  mico_bt_gatt_event_data_t *p_event_data )
{
    /* Create attribute(s) based on information included in the response PDU */
    mico_bt_smart_attribute_t *attr;

    mico_bt_smart_attribute_create( &attr, MICO_ATTRIBUTE_TYPE_CHARACTERISTIC, 0 );

    memcpy( &current_characteristic, &p_event_data->discovery_result.discovery_data.characteristic_declaration,
            sizeof( current_characteristic ) );

    if ( attr != NULL ) {
        attr->next                      = NULL;
        attr->handle                    = current_characteristic.handle;
        attr->type.len                  = UUID_16BIT;
        attr->type.uu.uuid16            = GATT_UUID_CHAR_DECLARE;
        attr->value_length              = 2;
        attr->value.characteristic.properties = current_characteristic.characteristic_properties;

        attr->value.characteristic.value_handle = current_characteristic.val_handle;
        attr->value.characteristic.descriptor_start_handle = attr->value.characteristic.value_handle + 1;
        /* FIXME: descriptor_end_handle need to be calculated correclty */
        attr->value.characteristic.descriptor_end_handle = attr->value.characteristic.descriptor_start_handle;

        memcpy( &attr->value.characteristic.uuid, &current_characteristic.char_uuid, sizeof(mico_bt_uuid_t) );

        if ( smartbridge_subprocedure.attr_head == NULL ) {
            smartbridge_subprocedure.attr_head = attr;
        }

        if ( smartbridge_subprocedure.attr_tail != NULL ) {
            smartbridge_subprocedure.attr_tail->next = attr;
        }

        smartbridge_subprocedure.attr_tail = attr;
        smartbridge_subprocedure.attr_count++;
        bt_smartbridge_log( "Characteristic value_handle:%x handle:%x uuid:%x list-count:%u properties:%u",
                            current_characteristic.val_handle, current_characteristic.handle, current_characteristic.char_uuid.uu.uuid16,
                            (unsigned int)smartbridge_subprocedure.attr_count, (int)attr->value.characteristic.properties );
    }
}

static void smartbridge_gatt_discovery_complete_handler( mico_bt_gatt_event_data_t *p_event_data )
{
    uint16_t discovery_complete_type = p_event_data->discovery_complete.disc_type;

    switch ( discovery_complete_type ) {
        case GATT_DISCOVER_SERVICES_ALL:
        case GATT_DISCOVER_SERVICES_BY_UUID:
        case GATT_DISCOVER_INCLUDED_SERVICES:
        case GATT_DISCOVER_CHARACTERISTICS:
        case GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS:
            if ( smartbridge_subprocedure.attr_count != 0 ) {
                smartbridge_subprocedure.result = MICO_BT_SUCCESS;
            } else {
                smartbridge_subprocedure.result = MICO_BT_ITEM_NOT_IN_LIST;
            }

            bt_smartbridge_log( "Discovery Completed ( result:%d type:%d )\r\n", smartbridge_subprocedure.result,
                                discovery_complete_type );

            subprocedure_notify_complete( &smartbridge_subprocedure );
            break;

        default:
            bt_smartbridge_log( "Unhandled Discovery Completed ( type:%d )", discovery_complete_type );
            break;
    }
}

mico_bt_gatt_status_t smartbridge_gatt_callback( mico_bt_gatt_evt_t event, mico_bt_gatt_event_data_t *p_event_data )
{
    mico_bt_gatt_status_t status = MICO_BT_GATT_SUCCESS;

    switch (event) {
        case GATT_CONNECTION_STATUS_EVT: {
            if ( p_event_data->connection_status.link_role == BT_SMART_LINK_ROLE_MASTER) {

                /* Connection */
                if ( p_event_data->connection_status.connected == MICO_TRUE ) {
                    if ( connecting_socket
                         && memcmp(connecting_socket->remote_device.address, p_event_data->connection_status.bd_addr, BD_ADDR_LEN) == 0) {
                        smartbridge_gatt_connection_handler( p_event_data->connection_status.conn_id );
                    } else {
                        smartbridge_gatt_auto_conn_handler( p_event_data->connection_status.bd_addr,
                                                            p_event_data->connection_status.conn_id );
                    }
                } else {
                    /* Auto connection: drop a connecton object. */
                    int8_t idx = smartbridge_auto_conn_find_dev_by_addr(p_event_data->connection_status.bd_addr);
                    if (idx >= 0) {
                        g_auto_conn_dev_table[idx].socket = 0;
                    }

                    /* Common connection drop */
                    smartbridge_gatt_disconnection_handler( p_event_data->connection_status.conn_id );
                }
            }
            break;
        }

        case GATT_ATTRIBUTE_REQUEST_EVT: {
            bt_smartbridge_log( "Gatt attribute status event" );
            break;
        }

        case GATT_OPERATION_CPLT_EVT: {
            if (p_event_data != NULL ) {
                if ( p_event_data->operation_complete.op == GATTC_OPTYPE_READ ) {
                    smartbridge_gatt_read_operation_complete_handler( &p_event_data->operation_complete.response_data.att_value );
                    subprocedure_notify_complete( &smartbridge_subprocedure );
                }

                else if ( p_event_data->operation_complete.op == GATTC_OPTYPE_WRITE ) {
                    bt_smartbridge_log( "Write-Callback event for handle:%x status:%d",
                                        p_event_data->operation_complete.response_data.handle, (unsigned int)p_event_data->operation_complete.status );
                    subprocedure_notify_complete( &smartbridge_subprocedure );
                } else if ( p_event_data->operation_complete.op == GATTC_OPTYPE_NOTIFICATION ||
                            p_event_data->operation_complete.op == GATTC_OPTYPE_INDICATION ) {
                    bt_smartbridge_log( "Notification/Indication Event");
                    smartbridge_gatt_notification_indication_handler( &p_event_data->operation_complete );
                }
            }
            break;
        }

        case GATT_DISCOVERY_RESULT_EVT: {
            if ( p_event_data != NULL ) {
                if ( p_event_data->discovery_result.discovery_type == GATT_DISCOVER_SERVICES_ALL ||
                     p_event_data->discovery_result.discovery_type == GATT_DISCOVER_SERVICES_BY_UUID ) {
                    smartbridge_gatt_discover_services_result( p_event_data );
                } else if ( p_event_data->discovery_result.discovery_type == GATT_DISCOVER_INCLUDED_SERVICES ) {
                    smartbridge_gatt_discover_included_services_result( p_event_data );
                } else if ( p_event_data->discovery_result.discovery_type == GATT_DISCOVER_CHARACTERISTICS ) {
                    smartbridge_gatt_discover_characteristic_result( p_event_data);
                } else if ( p_event_data->discovery_result.discovery_type == GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS ) {
                    smartbridge_gatt_discover_characteristic_descriptor_result( p_event_data );
                }

            }
            break;
        }

        case GATT_DISCOVERY_CPLT_EVT: {
            if ( ( p_event_data != NULL ) ) {
                smartbridge_gatt_discovery_complete_handler( p_event_data );
            }
            break;
        }

        default: {
            bt_smartbridge_log( "Gatt callback event:%d", event );
            break;
        }
    }

    return status;
}

OSStatus mico_bt_smartbridge_init( uint8_t count )
{
    OSStatus result;

    if ( initialised == MICO_TRUE ) {
        return MICO_BT_SUCCESS;
    }

    bt_smartbridge_log( "Initialising MICO SmartBridge ..." );

    /* Initialise internal settings for Auto Connection. */
    memset((void *)g_auto_conn_dev_table, 0, sizeof(g_auto_conn_dev_table));
    smartbridge_auto_conn_list_init();

    result = mico_rtos_init_semaphore(&g_auto_conn_sem, 1);
    if (result != kNoErr) {
        bt_smartbridge_log( "Error initialising a semaphore used to Auto connection" );
        return result;
    }

    /* Initialise SmartBridge Socket Manager */
    result = bt_smartbridge_socket_manager_init();
    if ( result != MICO_BT_SUCCESS ) {
        bt_smartbridge_log( "Error initialising SmartBridge Socket Manager" );
        return result;
    }

    /* Set socket manager's connections limit */
    bt_smartbridge_socket_manager_set_max_concurrent_connections( count );

    /* Initialise bt stack operation interface */
    smartbridge_bt_interface_initialize();

    initialised = MICO_TRUE;
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_deinit( void )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SUCCESS;
    }

    /* Disable Attribute Cache (The function checks if it's enabled) */
    bt_smartbridge_att_cache_disable();
    /* Deinitialise socket manager */
    bt_smartbridge_socket_manager_deinit();

    /* Deinitialise GATT */
    smartbridge_bt_interface_deinitialize();

    /* Delete semaphore */
    mico_rtos_deinit_semaphore(&g_auto_conn_sem);
    g_auto_conn_sem = NULL;

    /* Delete Auto-connection list. */
    smartbridge_auto_conn_list_deinit();

    initialised = MICO_FALSE;

    return MICO_BT_SUCCESS;
}

mico_bool_t mico_bt_smartbridge_is_scanning( void )
{
    return ( initialised == MICO_TRUE ) ? smartbridge_bt_interface_is_scanning( ) : MICO_FALSE;
}

mico_bool_t mico_bt_smartbridge_is_ready_to_connect( void )
{
    return ( initialised == MICO_FALSE || connecting_socket != NULL ) ? MICO_FALSE : MICO_TRUE;
}

OSStatus mico_bt_smartbridge_start_scan( const mico_bt_smart_scan_settings_t *settings,
                                         mico_bt_smart_scan_complete_callback_t complete_callback,
                                         mico_bt_smart_advertising_report_callback_t advertising_report_callback )
{

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    return smartbridge_bt_interface_start_scan( settings, complete_callback, advertising_report_callback );
}

OSStatus mico_bt_smartbridge_stop_scan( void )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }
    return smartbridge_bt_interface_stop_scan();
}

OSStatus mico_bt_smartbridge_get_scan_result_list( mico_bt_smart_scan_result_t **result_list, uint32_t *count )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }
    return smartbridge_helper_get_scan_results( result_list, count );
}

OSStatus mico_bt_smartbridge_get_background_connection_devices_size( uint8_t *size )
{
    if ( initialised == MICO_FALSE ) {
        return kNotInitializedErr;
    }
    if ( size == (uint8_t *)0 ) {
        return kGeneralErr;
    }
    return smartbridge_bt_interface_get_background_connection_device_size( size );
}

OSStatus mico_bt_smartbridge_set_auto_connection_action( mico_bool_t start_stop,
                                                         const mico_bt_smart_scan_settings_t *scan_settings,
                                                         mico_bt_smartbridge_auto_connection_parms_cback_t p_auto_conn_cback )
{
    OSStatus                            err = kNoErr;
    uint32_t                            duration;

    if (start_stop && !scan_settings) {
        return kParamErr;
    }
    if (initialised == MICO_FALSE) {
        return kNotInitializedErr;
    }
    if (connecting_socket != NULL) {
        return kInProgressErr;
    }
    if (start_stop && g_auto_conn_cback) {
        return kInProgressErr;
    }

    /* Clear connection list */
    smartbridge_auto_conn_list_clear();

    if (start_stop) {
        /* Start a scanning procedure */
        duration = scan_settings->duration_second;
        memcpy(&g_auto_conn_scan_cfg, scan_settings, sizeof(mico_bt_smart_scan_settings_t));
        g_auto_conn_scan_cfg.filter_duplicates  = DUPLICATES_FILTER_DISABLED;
        g_auto_conn_scan_cfg.filter_policy      = FILTER_POLICY_WHITE_LIST;
        g_auto_conn_scan_cfg.duration_second    = MIN(SMARTBRIDGE_AUTO_CONN_SCAN_INTERVAL, duration);

        err = mico_bt_smartbridge_start_scan(&g_auto_conn_scan_cfg,
                                             smartbridge_gap_auto_conn_scan_cmpl_handler,
                                             smartbridge_gap_auto_conn_scan_report_handler);

        if (err == MICO_BT_PENDING) {
            if (scan_settings->duration_second < (uint16_t)(-1)) {
                /* An limited timeout */
                g_auto_conn_scan_duration = duration - MIN(SMARTBRIDGE_AUTO_CONN_SCAN_INTERVAL, duration);
            } else {
                /* Forever until user request to stop it. */
                g_auto_conn_scan_duration = (uint16_t)(-1);
            }

            /* Selective Procedure Callback */
            if (p_auto_conn_cback) {
                g_auto_conn_cback = p_auto_conn_cback;
            }
            err = kNoErr;
        } else {
            bt_smartbridge_log("%s: start scanning failed", __FUNCTION__);
        }
    } else {
        g_auto_conn_cback = NULL;
        memset(&g_auto_conn_scan_cfg, 0, sizeof(mico_bt_smart_scan_settings_t));
        err = mico_bt_smartbridge_stop_scan();
    }
    return err;
}

OSStatus mico_bt_smartbridge_create_socket( mico_bt_smartbridge_socket_t *socket )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Reset socket fields */
    memset( socket, 0, sizeof( *socket ) );
    socket->connection_handle = SOCKET_INVALID_CONNECTION_HANDLE;

    /* Point node data to socket */
    socket->node.data = (void *)socket;

    /* Initialise socket semaphore */
    return mico_rtos_init_semaphore( &socket->semaphore, 1 );
}

OSStatus mico_bt_smartbridge_delete_socket( mico_bt_smartbridge_socket_t *socket )
{
    OSStatus result;
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    result = mico_rtos_deinit_semaphore( &socket->semaphore );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    memset( socket, 0, sizeof( *socket ) );
    socket->connection_handle = SOCKET_INVALID_CONNECTION_HANDLE;
    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_get_socket_status( mico_bt_smartbridge_socket_t *socket,
                                                mico_bt_smartbridge_socket_status_t *status )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( socket->state == SOCKET_STATE_LINK_ENCRYPTED ) {
        *status = SMARTBRIDGE_SOCKET_CONNECTED;
    } else if ( socket->state == SOCKET_STATE_LINK_CONNECTED ) {
        /* Status is connected if socket does not have loaded bond info and does not initiate pairing */
        if ( smartbridge_helper_socket_check_actions_disabled( socket,
                                                               SOCKET_ACTION_ENCRYPT_USING_BOND_INFO | SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE ) {
            *status = SMARTBRIDGE_SOCKET_CONNECTED;
        } else {
            *status = SMARTBRIDGE_SOCKET_CONNECTING;
        }
    } else if ( socket->state == SOCKET_STATE_LINK_CONNECTING ) {
        *status = SMARTBRIDGE_SOCKET_CONNECTING;
    } else {
        *status = SMARTBRIDGE_SOCKET_DISCONNECTED;
    }

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_connect( mico_bt_smartbridge_socket_t *socket, const mico_bt_smart_device_t *remote_device,
                                      const mico_bt_smart_connection_settings_t *settings,
                                      mico_bt_smartbridge_disconnection_callback_t disconnection_callback,
                                      mico_bt_smartbridge_notification_callback_t notification_callback )
{
    mico_bt_smartbridge_socket_t *found_socket;
    OSStatus result = MICO_BT_SUCCESS;

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( connecting_socket != NULL ) {
        /* Only 1 connecting socket is allowed */
        return MICO_BT_CONNECT_IN_PROGRESS;
    }

    if ( bt_smartbridge_socket_manager_is_full() == MICO_TRUE ) {
        return MICO_BT_MAX_CONNECTIONS_REACHED;
    }

    if ( bt_smartbridge_socket_manager_find_socket_by_address( &remote_device->address,
                                                               &found_socket ) == MICO_BT_SUCCESS ) {
        /* device is already connected */
        return MICO_BT_SOCKET_IN_USE;
    }

    bt_smartbridge_log( "connect()...socket things are okay" );

    /* Clean-up accidentally set semaphores */
    while ( mico_rtos_get_semaphore( &socket->semaphore, MICO_NO_WAIT ) == MICO_BT_SUCCESS ) {
    }

    /* Store socket pointer in a temporary global variable so it can be referenced in smartbridge_gap_connection_handler */
    connecting_socket = socket;

    /* Store connection settings */
    memcpy( &socket->connection_settings, settings, sizeof( *settings ) );

    /* Store remote device information */
    memcpy( &socket->remote_device, remote_device, sizeof( *remote_device ) );

    /* Set callback functions */
    socket->disconnection_callback = disconnection_callback;
    socket->notification_callback  = notification_callback;

    /* Reset connection handle to invalid value */
    socket->connection_handle = SOCKET_INVALID_CONNECTION_HANDLE;

    /* Reset state */
    socket->state = SOCKET_STATE_DISCONNECTED;

    /* Set socket action to connecting */
    smartbridge_helper_socket_set_actions( socket, SOCKET_ACTION_HOST_CONNECT );
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_HOST_DISCONNECT );

    /* Set attribute protocol timeout */
    smartbridge_bt_interface_set_attribute_timeout( settings->attribute_protocol_timeout_ms );

    if ( smartbridge_helper_socket_check_actions_enabled( socket, SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE ) {
        /* Tell GAP to initiate pairing on the next connection attempt */
        mico_bt_start_pairing( socket->remote_device.address, socket->remote_device.address_type, &socket->security_settings );
    } else {
        /* Tell GAP to not send pairing request on the next connection attempt */
        mico_bt_stop_pairing( socket->remote_device.address );
    }


    smartbridge_bt_interface_connect( remote_device, settings, disconnection_callback, notification_callback );

    /* Wait for connection */
    mico_rtos_get_semaphore( &socket->semaphore, socket->connection_settings.timeout_second * 1000 );

    /* Check if link is connected. Otherwise, return error */
    if ( socket->state == SOCKET_STATE_LINK_CONNECTED ) {

        if ( smartbridge_helper_socket_check_actions_enabled( socket, SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE ) {
            /* Wait until pairing is complete */
            mico_rtos_get_semaphore( &socket->semaphore, MICO_NEVER_TIMEOUT );
        }

        /* Check if encryption is required */
        if ( smartbridge_helper_socket_check_actions_enabled( socket, SOCKET_ACTION_INITIATE_PAIRING ) == MICO_TRUE ||
             smartbridge_helper_socket_check_actions_enabled( socket, SOCKET_ACTION_ENCRYPT_USING_BOND_INFO ) == MICO_TRUE ) {
            mico_bt_start_encryption( &socket->remote_device.address );

            /* Wait until link is encrypted */
            mico_rtos_get_semaphore( &socket->semaphore, MICO_NEVER_TIMEOUT );

            if ( socket->state != SOCKET_STATE_LINK_ENCRYPTED ) {
                result = MICO_BT_ENCRYPTION_FAILED;
                mico_bt_dev_delete_bonded_device( (uint8_t *)remote_device->address );
                goto error;
            }
        }
    } else {
        result = MICO_BT_SOCKET_NOT_CONNECTED;
        goto error;
    }

    /* Successful */
    if ( bt_smartbridge_att_cache_is_enabled() == MICO_TRUE ) {
        bt_smartbridge_att_cache_t *cache = NULL;

        result = bt_smartbridge_att_cache_find( remote_device, &cache );
        if ( result == MICO_BT_SUCCESS ) {
            bt_smartbridge_log( "USING ATT CACHE ..." );
        } else {
            bt_smartbridge_log( "GENERATING ATT CACHE ..." );

            result = bt_smartbridge_att_cache_generate( remote_device, socket->connection_handle, &cache );

            if ( result != MICO_BT_SUCCESS ) {
                bt_smartbridge_log( "Error Generating Cache result:%d", result );
                goto error;
            }
        }

        /* Successful. Mark cache as active and store reference in socket */
        bt_smartbridge_att_cache_set_active_state( cache, MICO_TRUE );
        socket->att_cache = (void *)cache;
    }

    /* Add this device to the white list. */
    if (connecting_socket->connection_settings.filter_policy == FILTER_POLICY_WHITE_LIST) {
        if (connecting_socket->remote_device.address_type == BT_SMART_ADDR_TYPE_PUBLIC) {
            result = smartbridge_bt_interface_update_background_connection_device(TRUE, connecting_socket->remote_device.address);
            if (result != kNoErr) {
                bt_smartbridge_log("Add device to white list unsuccessfully, status: %d", result);
            }
        } else {
            bt_smartbridge_log("Cannot add device to white list: random address type");
        }
    }

    /* Clear connect action as it's no longer needed */
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_HOST_CONNECT );

    /* Reset connecting socket pointer */
    connecting_socket = NULL;

    /* Link is connected. Return success */
    return result;

#if 1
error:
    /* Link is not connected nor encrypted. Issue disconnection attempt to clean-up */
    smartbridge_helper_socket_clear_actions( socket,
                                             SOCKET_ACTION_INITIATE_PAIRING | SOCKET_ACTION_ENCRYPT_USING_BOND_INFO );
    mico_rtos_delay_milliseconds(
        200);  //A quick disconnection from an established connection may cause a duplicated gatt connected callback, by william
    mico_bt_smartbridge_disconnect( socket, TRUE );

    /* Clear connect action as it's no longer needed */
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_HOST_CONNECT );

    /* Reset connecting socket pointer */
    connecting_socket = NULL;

    /* Link is not connected. Return error */
    return result;
#endif
}

OSStatus mico_bt_smartbridge_disconnect( mico_bt_smartbridge_socket_t *socket, mico_bool_t remove_it_from_whitelist )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Mark disconnection flag that it's coming from the host */
    smartbridge_helper_socket_set_actions( socket, SOCKET_ACTION_HOST_DISCONNECT );

    /* Clean-up accidentally set semaphores */
    while ( mico_rtos_get_semaphore( &socket->semaphore, MICO_NO_WAIT ) == MICO_BT_SUCCESS ) {
    }

    /* Check if either link is encrypted or connected */
    if ( socket->state >= SOCKET_STATE_LINK_CONNECTED ) {
        smartbridge_bt_interface_disconnect( socket->connection_handle );
        /* Wait for disconnection */
        mico_rtos_get_semaphore( &socket->semaphore, socket->connection_settings.timeout_second * 1000 );
    } else {
        /* Link is not yet connected. Cancel last */
        smartbridge_bt_interface_cancel_last_connect( socket->remote_device.address );
    }

    /* Clear socket disconnect action */
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_HOST_DISCONNECT );

    /* Proper clean-up if socket isn't properly disconnected */
    if ( socket->state != SOCKET_STATE_DISCONNECTED ) {
        mico_bt_smartbridge_socket_t *removed_socket;

        bt_smartbridge_socket_manager_remove_socket( socket->connection_handle, &removed_socket );

        /* Reset connection handle to invalid value */
        socket->connection_handle = SOCKET_INVALID_CONNECTION_HANDLE;

        /* Clear socket state */
        socket->state = SOCKET_STATE_DISCONNECTED;

        /* Mark att cache as inactive and reset reference to cache */
        bt_smartbridge_att_cache_set_active_state( (bt_smartbridge_att_cache_t *)socket->att_cache, MICO_FALSE );
        socket->att_cache = NULL;
    }

    /* Remove this device from the white list */
    if (remove_it_from_whitelist) {
        smartbridge_bt_interface_update_background_connection_device(FALSE, socket->remote_device.address);
    }

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_set_transmit_power( mico_bt_smartbridge_socket_t *socket, int8_t transmit_power_dbm )
{
    if ( initialised == MICO_FALSE || socket->state == SOCKET_STATE_DISCONNECTED ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    return smartbridge_bt_interface_set_connection_tx_power( socket->connection_handle, transmit_power_dbm );
}

OSStatus mico_bt_smartbridge_set_bond_info( mico_bt_smartbridge_socket_t *socket,
                                            const mico_bt_smart_security_settings_t *settings, const mico_bt_smart_bond_info_t *bond_info )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Set local copies of security settings and bond info */
    memcpy( &socket->bond_info, bond_info, sizeof( *bond_info ) );
    memcpy( &socket->security_settings, settings, sizeof( *settings ) );

    /* Clear socket action to initiate pairing request */
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_INITIATE_PAIRING );

    /* Set socket action to encrypt using loaded bond info */
    smartbridge_helper_socket_set_actions( socket, SOCKET_ACTION_ENCRYPT_USING_BOND_INFO );

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_clear_bond_info( mico_bt_smartbridge_socket_t *socket )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Reset bond info */
    memset( &socket->bond_info, 0, sizeof( socket->bond_info ) );

    /* Clear socket action to encrypt using loaded bond info */
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_ENCRYPT_USING_BOND_INFO );

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_enable_pairing( mico_bt_smartbridge_socket_t *socket,
                                             const mico_bt_smart_security_settings_t *settings, mico_bt_smart_bonding_callback_t bonding_callback )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Store security settings in local copy */
    memcpy( &socket->security_settings, settings, sizeof( *settings ) );

    /* Reset bond info */
    memset( &socket->bond_info, 0, sizeof( socket->bond_info ) );

    /* Set pairing callback */
    socket->bonding_callback = bonding_callback;

    /* Clear socket action to encrypt using loaded bond info */
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_ENCRYPT_USING_BOND_INFO );

    /* Set socket action to iniatiate pairing request */
    smartbridge_helper_socket_set_actions( socket, SOCKET_ACTION_INITIATE_PAIRING );

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_disable_pairing( mico_bt_smartbridge_socket_t *socket )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Clear socket action to iniatiate pairing request */
    smartbridge_helper_socket_clear_actions( socket, SOCKET_ACTION_INITIATE_PAIRING );

    return MICO_BT_SUCCESS;
}

OSStatus mico_bt_smartbridge_enable_attribute_cache( uint32_t cache_count, mico_bt_uuid_t cache_services[],
                                                     uint32_t service_count )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Call internal function */
    return bt_smartbridge_att_cache_enable( cache_count, cache_services, service_count );
}

OSStatus mico_bt_smartbridge_disable_attribute_cache( void )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Call internal function */
    return bt_smartbridge_att_cache_disable();
}

OSStatus mico_bt_smartbridge_remove_attribute_cache( mico_bt_smartbridge_socket_t *socket )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    /* Call internal function */
    return bt_smartbridge_att_cache_release( socket->att_cache );
}

OSStatus mico_bt_smartbridge_enable_attribute_cache_notification( mico_bt_smartbridge_socket_t *socket,
                                                                  mico_bool_t is_notification_or_indication )
{
    mico_bt_smart_attribute_list_t *list;
    mico_bt_smart_attribute_t      *iterator;
    bt_smartbridge_att_cache_t      *cache;
    OSStatus                         result;

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( bt_smartbridge_att_cache_is_enabled( ) == MICO_FALSE ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    if ( socket == NULL || socket->att_cache == NULL ) {
        return MICO_BT_BADARG;
    }

    cache = (bt_smartbridge_att_cache_t *)socket->att_cache;

    if ( bt_smartbridge_att_cache_is_discovering( cache ) == MICO_TRUE ) {
        return MICO_BT_DISCOVER_IN_PROGRESS;
    }

    result = bt_smartbridge_att_cache_get_list( cache, &list );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    result = mico_bt_smart_attribute_get_list_head( list, &iterator );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    while ( iterator != NULL ) {
        if ( iterator->type.uu.uuid16 == 0x2902 ) {
            /* Switch on notification from server */
            bt_smartbridge_att_cache_lock( cache );

            if (is_notification_or_indication) {
                iterator->value.client_config.config_bits = 0x0001;    // Notification
            } else {
                iterator->value.client_config.config_bits = 0x0002;    // Indication
            }

            bt_smartbridge_att_cache_unlock( cache );
            result = smartbridge_bt_interface_write_characteristic_descriptor( socket->connection_handle, iterator );
        }

        iterator = iterator->next;
    }

    return result;
}

OSStatus mico_bt_smartbridge_disable_attribute_cache_notification( mico_bt_smartbridge_socket_t *socket )
{
    mico_bt_smart_attribute_list_t *list;
    mico_bt_smart_attribute_t      *iterator;
    bt_smartbridge_att_cache_t      *cache;
    OSStatus                         result;

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( bt_smartbridge_att_cache_is_enabled( ) == MICO_FALSE ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    if ( socket == NULL || socket->att_cache == NULL ) {
        return MICO_BT_BADARG;
    }

    cache = (bt_smartbridge_att_cache_t *)socket->att_cache;

    if ( bt_smartbridge_att_cache_is_discovering( cache ) == MICO_TRUE ) {
        return MICO_BT_DISCOVER_IN_PROGRESS;
    }

    result = bt_smartbridge_att_cache_get_list( cache, &list );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    result = mico_bt_smart_attribute_get_list_head( list, &iterator );
    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    while ( iterator != NULL ) {
        if ( iterator->type.uu.uuid16 == 0x2902 ) {
            /* Swith on notification from server */
            bt_smartbridge_att_cache_lock( cache );

            iterator->value.client_config.config_bits = 0;

            bt_smartbridge_att_cache_unlock( cache );

            result = smartbridge_bt_interface_write_characteristic_descriptor( socket->connection_handle, iterator );
        }

        iterator = iterator->next;
    }

    return result;
}

OSStatus mico_bt_smartbridge_get_attribute_cache_list( mico_bt_smartbridge_socket_t *socket,
                                                       mico_bt_smart_attribute_list_t **att_cache_list )
{
    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( bt_smartbridge_att_cache_is_enabled( ) == MICO_FALSE ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    if ( socket == NULL || socket->att_cache == NULL || att_cache_list == NULL ) {
        return MICO_BT_BADARG;
    }

    if ( bt_smartbridge_att_cache_is_discovering( (bt_smartbridge_att_cache_t *)socket->att_cache ) == MICO_TRUE ) {
        return MICO_BT_DISCOVER_IN_PROGRESS;
    }

    /* Call internal function */
    return bt_smartbridge_att_cache_get_list( (bt_smartbridge_att_cache_t *)socket->att_cache, att_cache_list );
}

OSStatus mico_bt_smartbridge_get_attribute_cache_by_handle( mico_bt_smartbridge_socket_t *socket, uint16_t handle,
                                                            mico_bt_smart_attribute_t *attribute, uint16_t size )
{
    bt_smartbridge_att_cache_t      *cache          = NULL;
    mico_bt_smart_attribute_list_t *att_cache_list = NULL;
    mico_bt_smart_attribute_t      *att            = NULL;
    OSStatus                         result;

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( bt_smartbridge_att_cache_is_enabled( ) == MICO_FALSE ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    if ( socket == NULL || socket->att_cache == NULL || attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    cache = (bt_smartbridge_att_cache_t *)socket->att_cache;

    if ( bt_smartbridge_att_cache_is_discovering( cache ) == MICO_TRUE ) {
        return MICO_BT_DISCOVER_IN_PROGRESS;
    }

    bt_smartbridge_att_cache_get_list( cache, &att_cache_list );

    bt_smartbridge_att_cache_lock( cache );

    result = mico_bt_smart_attribute_search_list_by_handle( att_cache_list, handle, &att );

    if ( result == MICO_BT_SUCCESS ) {
        if ( att->value_struct_size + ATTR_COMMON_FIELDS_SIZE > size ) {
            result = MICO_BT_ATTRIBUTE_VALUE_TOO_LONG;
        } else {
            memcpy( attribute, att, att->value_struct_size + ATTR_COMMON_FIELDS_SIZE );
        }
    }

    bt_smartbridge_att_cache_unlock( cache );
    return result;
}

OSStatus mico_bt_smartbridge_get_attribute_cache_by_uuid( mico_bt_smartbridge_socket_t *socket,
                                                          const mico_bt_uuid_t *uuid, uint16_t starting_handle, uint16_t ending_handle, mico_bt_smart_attribute_t *attribute,
                                                          uint32_t size )
{
    bt_smartbridge_att_cache_t      *cache          = NULL;
    mico_bt_smart_attribute_list_t *att_cache_list = NULL;
    mico_bt_smart_attribute_t      *att            = NULL;
    OSStatus                         result;

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( bt_smartbridge_att_cache_is_enabled( ) == MICO_FALSE ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    if ( socket == NULL || socket->att_cache == NULL || uuid == NULL || attribute == NULL ) {
        return MICO_BT_BADARG;
    }

    cache = (bt_smartbridge_att_cache_t *)socket->att_cache;

    if ( bt_smartbridge_att_cache_is_discovering( cache ) == MICO_TRUE ) {
        return MICO_BT_DISCOVER_IN_PROGRESS;
    }

    bt_smartbridge_att_cache_get_list( cache, &att_cache_list );

    bt_smartbridge_att_cache_lock( cache );

    result = mico_bt_smart_attribute_search_list_by_uuid( att_cache_list, uuid, starting_handle, ending_handle, &att );
    if ( result == MICO_BT_SUCCESS ) {
        if ( att->value_struct_size + ATTR_COMMON_FIELDS_SIZE > size ) {
            result = MICO_BT_ATTRIBUTE_VALUE_TOO_LONG;
        } else {
            memcpy( attribute, att, att->value_struct_size + ATTR_COMMON_FIELDS_SIZE );
        }
    }

    bt_smartbridge_att_cache_unlock( cache );
    return result;
}

OSStatus mico_bt_smartbridge_get_service_from_attribute_cache_by_uuid( mico_bt_smartbridge_socket_t *socket,
                                                                       const mico_bt_uuid_t *uuid, uint16_t starting_handle, uint16_t ending_handle, mico_bt_smart_attribute_t *attribute,
                                                                       uint32_t size )
{
    OSStatus result;
    uint16_t _starting_handle = starting_handle;
    uint16_t _ending_handle = ending_handle;
    mico_bt_uuid_t service_uuid = { .len = LEN_UUID_16, .uu.uuid16 = GATT_UUID_PRI_SERVICE };

    /* Find service */
    while ( ( result = mico_bt_smartbridge_get_attribute_cache_by_uuid( socket, &service_uuid, _starting_handle,
                                                                        _ending_handle, attribute, size ) ) == MICO_BT_SUCCESS ) {
        if ( memcmp( (void *)&attribute->value.service.uuid.uu, &uuid->uu, uuid->len ) == 0 ) {
            return MICO_BT_SUCCESS;
        }

        /* Fixed a bug for a service without characteristics. */
        if (attribute->value.service.end_handle > attribute->value.service.start_handle) {
            _starting_handle = attribute->value.service.end_handle;
        } else {
            _starting_handle = attribute->handle + 1;
        }
    }
    return result;
}

OSStatus mico_bt_smartbridge_get_characteritics_from_attribute_cache_by_uuid( mico_bt_smartbridge_socket_t *socket,
                                                                              const mico_bt_uuid_t *uuid, uint16_t starting_handle, uint16_t ending_handle, mico_bt_smart_attribute_t *attribute,
                                                                              uint32_t size )
{
    OSStatus result;
    uint16_t _starting_handle = starting_handle;
    uint16_t _ending_handle = ending_handle;
    mico_bt_uuid_t characteristic_uuid = { .len = LEN_UUID_16, .uu.uuid16 = GATT_UUID_CHAR_DECLARE };

    /* Find service */
    while ( ( result = mico_bt_smartbridge_get_attribute_cache_by_uuid( socket, &characteristic_uuid, _starting_handle,
                                                                        _ending_handle, attribute, size ) ) == MICO_BT_SUCCESS ) {
        if ( memcmp( (void *)&attribute->value.characteristic.uuid.uu, &uuid->uu, uuid->len ) == 0 ) {
            return MICO_BT_SUCCESS;
        }
        _starting_handle = attribute->handle + 1;
    }
    return result;
}



OSStatus mico_bt_smartbridge_refresh_attribute_cache_characteristic_value( mico_bt_smartbridge_socket_t *socket,
                                                                           uint16_t handle )
{
    bt_smartbridge_att_cache_t      *cache          = NULL;
    mico_bt_smart_attribute_list_t *att_cache_list = NULL;
    mico_bt_smart_attribute_t      *current_att    = NULL;
    mico_bt_smart_attribute_t      *refreshed_att  = NULL;
    OSStatus                         result;

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( bt_smartbridge_att_cache_is_enabled( ) == MICO_FALSE ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    if ( socket == NULL || socket->att_cache == NULL ) {
        return MICO_BT_BADARG;
    }

    cache = (bt_smartbridge_att_cache_t *)socket->att_cache;

    if ( bt_smartbridge_att_cache_is_discovering( cache ) == MICO_TRUE ) {
        return MICO_BT_DISCOVER_IN_PROGRESS;
    }

    bt_smartbridge_att_cache_get_list( cache, &att_cache_list );

    bt_smartbridge_att_cache_lock( cache );

    result = mico_bt_smart_attribute_search_list_by_handle( att_cache_list, handle, &current_att );
    if ( result == MICO_BT_SUCCESS ) {
        /* Check if length is longer than what read characteristic value can handle
         * If longer, use read long characteristic value
         */
        if ( current_att->value_length <= ATT_STANDARD_VALUE_LENGTH ) {
            result = smartbridge_bt_interface_read_characteristic_value( socket->connection_handle, current_att->handle,
                                                                         &current_att->type, &refreshed_att );
        } else {
            result = smartbridge_bt_interface_read_long_characteristic_value( socket->connection_handle, current_att->handle,
                                                                              &current_att->type, &refreshed_att );
        }

        /* If read is successful, replace attribute with the refreshed one
         */
        if ( result == MICO_BT_SUCCESS ) {
            /* This function removes and also deletes the attribute with handle specified */
            result = mico_bt_smart_attribute_remove_from_list( att_cache_list, current_att->handle );
            if ( result == MICO_BT_SUCCESS ) {
                result = mico_bt_smart_attribute_add_to_list( att_cache_list, refreshed_att );
            }
        }
    }

    bt_smartbridge_att_cache_unlock( cache );
    return result;
}

OSStatus mico_bt_smartbridge_write_attribute_cache_characteristic_value( mico_bt_smartbridge_socket_t *socket,
                                                                         const mico_bt_smart_attribute_t *char_value )
{
    bt_smartbridge_att_cache_t      *cache          = NULL;
    mico_bt_smart_attribute_list_t *att_cache_list = NULL;
    mico_bt_smart_attribute_t      *att            = NULL;
    OSStatus                         result;

    if ( initialised == MICO_FALSE ) {
        return MICO_BT_SMART_APPL_UNINITIALISED;
    }

    if ( bt_smartbridge_att_cache_is_enabled( ) == MICO_FALSE ) {
        return MICO_BT_ATT_CACHE_UNINITIALISED;
    }

    if ( socket == NULL || socket->att_cache == NULL || char_value == NULL ) {
        return MICO_BT_BADARG;
    }

    cache = (bt_smartbridge_att_cache_t *)socket->att_cache;

    if ( bt_smartbridge_att_cache_is_discovering( cache ) == MICO_TRUE ) {
        return MICO_BT_DISCOVER_IN_PROGRESS;
    }

    bt_smartbridge_att_cache_get_list( cache, &att_cache_list );


    if ( char_value->value_length <= ATT_STANDARD_VALUE_LENGTH ) {
        result = smartbridge_bt_interface_write_characteristic_value( socket->connection_handle,
                                                                      (mico_bt_smart_attribute_t *)char_value );
    } else {
        result = smartbridge_bt_interface_write_long_characteristic_value( socket->connection_handle,
                                                                           (mico_bt_smart_attribute_t *)char_value );
    }

    if ( result != MICO_BT_SUCCESS ) {
        return result;
    }

    bt_smartbridge_att_cache_lock( cache );

    /* Find characteristic value in local attribute list. Add to the list if not found */

    result = mico_bt_smart_attribute_search_list_by_handle( att_cache_list, char_value->handle, &att );

    if ( result == MICO_BT_SUCCESS ) {
        /* Found. Compare lengths first.
         * If new length is not equal old length, replace old attribute with new one.
         * If equal, copy content directly.
         */
        if ( char_value->value_length != att->value_length ) {
            result = mico_bt_smart_attribute_remove_from_list( att_cache_list, att->handle );

            if ( result != MICO_BT_SUCCESS ) {
                goto exit;
            }

            att = NULL; /* Reuse attribute pointer */

            if ( result != MICO_BT_SUCCESS ) {
                goto exit;
            }

            result = mico_bt_smart_attribute_create( &att, MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE, char_value->value_length );

            if ( result != MICO_BT_SUCCESS ) {
                goto exit;
            }

            att->handle            = char_value->handle;
            att->type              = char_value->type;
            att->value_length      = char_value->value_length;
            att->value_struct_size = char_value->value_struct_size;

            memcpy( att->value.value, char_value->value.value, char_value->value_length );

            result = mico_bt_smart_attribute_add_to_list( att_cache_list, att );
        } else {
            memcpy( att->value.value, char_value->value.value, char_value->value_length );
        }
    } else if ( result == MICO_BT_ITEM_NOT_IN_LIST ) {
        /* Not found. Create new one and add attribute to the list */
        result = mico_bt_smart_attribute_create( &att, MICO_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE, char_value->value_length );

        if ( result != MICO_BT_SUCCESS ) {
            goto exit;
        }

        att->handle            = char_value->handle;
        att->type              = char_value->type;
        att->value_length      = char_value->value_length;
        att->value_struct_size = char_value->value_struct_size;

        memcpy( att->value.value, char_value->value.value, char_value->value_length );

        result = mico_bt_smart_attribute_add_to_list( att_cache_list, att );
    }

exit:
    bt_smartbridge_att_cache_unlock( cache );
    return result;
}

/******************************************************
 *               Callback Definitions
 ******************************************************/

static OSStatus smartbridge_app_notification_handler( void *arg )
{
    mico_bt_smartbridge_socket_t *socket = (mico_bt_smartbridge_socket_t *)arg;

    if ( socket != NULL && socket->notification_callback != NULL ) {
        socket->notification_callback( socket, socket->last_notified_attribute_handle );
        return MICO_BT_SUCCESS;
    }

    return MICO_BT_ERROR;
}

static OSStatus smartbridge_app_disconnection_handler( void *arg )
{
    mico_bt_smartbridge_socket_t *socket = (mico_bt_smartbridge_socket_t *)arg;

    if ( socket != NULL && socket->disconnection_callback != NULL ) {
        socket->disconnection_callback( socket );
        return MICO_BT_SUCCESS;
    }

    return MICO_BT_ERROR;
}

// OSStatus mico_bt_smartbridge_bond_info_update( mico_bt_device_link_keys_t paired_device_keys )
// {
//     mico_bt_smart_bond_info_t          bond_info;
//     mico_bt_ble_keys_t                  *le_security_keys;
//     mico_bt_device_sec_keys_t           *security_keys;

//     OSStatus result = MICO_BT_ERROR;

//     memcpy(bond_info.peer_address, paired_device_keys.bd_addr, BD_ADDR_LEN );

//     security_keys       = (mico_bt_device_sec_keys_t*)&paired_device_keys.key_data;

//     if( security_keys == NULL )
//     {
//         WPRINT_LIB_ERROR( ("[SmartBridge] Security Keys is NULL\n"));
//         return MICO_BT_ERROR;
//     }

//     le_security_keys    = ( mico_bt_ble_keys_t* )&security_keys->le_keys;

//     if ( le_security_keys == NULL )
//     {
//         WPRINT_LIB_ERROR( ("[SmartBridge] Security LE-Keys is NULL\n"));
//         return MICO_BT_ERROR;
//     }

//     bond_info.address_type  = (mico_bt_smart_address_type_t)security_keys->ble_addr_type;

//     bond_info.ediv          = le_security_keys->ediv;

//     /* fill bond_info structure as expected by the application */
//     memcpy( bond_info.irk,  le_security_keys->irk,    16 );
//     memcpy( bond_info.csrk, le_security_keys->pcsrk,  16 );
//     memcpy( bond_info.ltk,  le_security_keys->pltk,   16 );
//     memcpy( bond_info.rand, le_security_keys->rand,   8  );

//     result = smartbridge_gap_bonding_handler( connecting_socket->connection_handle, &bond_info );

//     WPRINT_LIB_INFO( ( "[SmartBridge] Bond-Info updated result: %u\n", (unsigned int) result ) );

//     return result;
// }

// static OSStatus smartbridge_gap_bonding_handler( uint16_t connection_handle, const mico_bt_smart_bond_info_t* bond_info )
// {
//     mico_bt_smartbridge_socket_t* socket;

//     if ( bt_smartbridge_socket_manager_find_socket_by_handle( connection_handle, &socket ) == MICO_BT_SUCCESS )
//     {
//         /* Bonding successful. Update socket's bond info and post callback to MICO_BT_EVT_WORKER_THREAD */
//         memcpy( &socket->bond_info, bond_info, sizeof( *bond_info ) );

//         if ( socket != NULL && socket->bonding_callback != NULL )
//         {
//             mico_rtos_send_asynchronous_event( MICO_BT_EVT_WORKER_THREAD, smartbridge_app_pairing_handler, (void*)socket );
//         }

//         return MICO_BT_SUCCESS;
//     }

//     return MICO_BT_ERROR;
// }

// static OSStatus smartbridge_app_pairing_handler( void* arg )
// {
//     mico_bt_smartbridge_socket_t* socket = (mico_bt_smartbridge_socket_t*)arg;

//     if ( socket != NULL && socket->bonding_callback != NULL )
//     {
//         socket->bonding_callback( socket, &socket->bond_info );
//         return MICO_BT_SUCCESS;
//     }

//     return MICO_BT_ERROR;
// }

/* Auto-Connection Connecting Event Handler */
static OSStatus smartbridge_gap_auto_conn_asyn_event_handler(void *arg)
{
    OSStatus                             err = kNoErr;
    mico_bt_gatt_status_t                gatt_status;
    mico_bt_smart_advertising_report_t  *report = NULL;

    /* Find a scanned device to create a connection. */
    err = smartbridge_auto_conn_list_get_by_state(&report, SMARTBRIDGE_AUTO_CONN_STATE_SCANED);
    if (err == kNoErr) {
        /* Update current state */
        smartbridge_auto_conn_list_set_state(report->remote_device.address,
                                             SMARTBRIDGE_AUTO_CONN_STATE_REPORTED);

        /* Check user configuration */
        if (smartbridge_gap_auto_conn_user_cfg(report->remote_device.address,
                                               (uint8_t *)report->remote_device.name,
                                               report->eir_data,
                                               report->eir_data_length)) {
            /* Get socket */
            int8_t idx = smartbridge_auto_conn_find_dev_by_addr(report->remote_device.address);
            mico_bt_smartbridge_socket_t *p_socket = g_auto_conn_dev_table[idx].socket;

            /* Clear semaphore */
            while (mico_rtos_get_semaphore(&g_auto_conn_sem, MICO_NO_WAIT) == kNoErr);

            /* Update current state */
            smartbridge_auto_conn_list_set_state(report->remote_device.address,
                                                 SMARTBRIDGE_AUTO_CONN_STATE_CONNING);

            /* Update Socket State */
            p_socket->state = SOCKET_STATE_LINK_CONNECTING;
            smartbridge_helper_socket_clear_actions(p_socket, SOCKET_ACTION_HOST_DISCONNECT);
            smartbridge_helper_socket_clear_actions(p_socket, SOCKET_ACTION_HOST_CONNECT);

            /* Start to create a connection */
            gatt_status = mico_bt_gatt_le_connect((uint8_t *)report->remote_device.address,
                                                  report->remote_device.address_type,
                                                  BLE_CONN_MODE_HIGH_DUTY,
                                                  MICO_TRUE);
            if (!gatt_status) {
                bt_smartbridge_log("%s: Create LE Connection failed", __FUNCTION__);
                err = kConnectionErr;
                goto err_exit;
            }

            /* Wait for completion */
            err = mico_rtos_get_semaphore(&g_auto_conn_sem, 5000);
            if (err != kNoErr) {
                bt_smartbridge_log("%s: Create LE Connection failed: %s",
                                   __FUNCTION__,
                                   err == kTimeoutErr ? "Timeout" : "Unknown Error");
err_exit:
                p_socket->state = SOCKET_STATE_DISCONNECTED;
                smartbridge_auto_conn_list_set_state(report->remote_device.address,
                                                     SMARTBRIDGE_AUTO_CONN_STATE_FREE);
                mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                  (event_handler_t)p_socket->auto_connection_callback,
                                                  (void *)p_socket);
            } else {
                bt_smartbridge_log("%s: Craete LE Connection succesfully", __FUNCTION__);

                /* Start encryption and cache */
                smartbridge_gap_auto_conn_cryption((void *)p_socket);

                /* Create connection successfully and remove it from the list. */
                smartbridge_auto_conn_list_remove(report->remote_device.address);

                /* Notify user */
                mico_rtos_send_asynchronous_event(MICO_BT_EVT_WORKER_THREAD,
                                                  (event_handler_t)p_socket->auto_connection_callback,
                                                  (void *)p_socket);
            }
            /* Next Connection */
            mico_rtos_send_asynchronous_event(MICO_BT_WORKER_THREAD,
                                              smartbridge_gap_auto_conn_asyn_event_handler,
                                              NULL);
        } else {
            /* Update current state */
            smartbridge_auto_conn_list_set_state(report->remote_device.address,
                                                 SMARTBRIDGE_AUTO_CONN_STATE_IDLE);
        }
    }
    return err;
}

/* Auto-Connection Scanning Procedure Report Handler */
static OSStatus smartbridge_gap_auto_conn_scan_report_handler(const mico_bt_smart_advertising_report_t *report)
{
    OSStatus                                err = kNoErr;
    mico_bt_smart_advertising_report_t     *match_report = NULL;

    bt_smartbridge_log("%s: scan result: %s[%02x:%02x:%02x:%02x:%02x:%02x]",
                       __FUNCTION__,
                       report->remote_device.name,
                       report->remote_device.address[0],
                       report->remote_device.address[1],
                       report->remote_device.address[2],
                       report->remote_device.address[3],
                       report->remote_device.address[4],
                       report->remote_device.address[5]);

    /* Add it to the list. */
    err = smartbridge_auto_conn_list_add(report);
    if (err != kNoErr) {
        //bt_smartbridge_log("%s: add list failed", __FUNCTION__);
        goto exit;
    }
    smartbridge_auto_conn_list_set_state(report->remote_device.address,
                                         SMARTBRIDGE_AUTO_CONN_STATE_SCANED);

    /* Check current state. */
    err = smartbridge_auto_conn_list_get_by_state(&match_report,
                                                  SMARTBRIDGE_AUTO_CONN_STATE_CONNING);
    if (err == kNoErr) {
        bt_smartbridge_log("%s: A connection is busy!", __FUNCTION__);
        /* It is busy for Auto-Connection Procedure and we should wait. */
    } else {
        /* Create connection */
        mico_rtos_send_asynchronous_event(MICO_BT_WORKER_THREAD,
                                          smartbridge_gap_auto_conn_asyn_event_handler,
                                          NULL);
    }

exit:
    return err;
}

/* Auto-Connection Scanning Procedure Completion Handler */
static OSStatus smartbridge_gap_auto_conn_scan_cmpl_handler(void *arg)
{
    OSStatus err = kNoErr;

    UNUSED_PARAMETER(arg);

    /* Clear some IDLE report. */
    smartbridge_auto_conn_list_remove_by_state(SMARTBRIDGE_AUTO_CONN_STATE_IDLE);

    /* Restart scanning procedure... */
    if (g_auto_conn_scan_duration > 0) {
        bt_smartbridge_log("restart scanning for auto-connection");
        g_auto_conn_scan_cfg.duration_second = MIN(SMARTBRIDGE_AUTO_CONN_SCAN_INTERVAL, g_auto_conn_scan_duration);
        err = mico_bt_smartbridge_start_scan(&g_auto_conn_scan_cfg,
                                             smartbridge_gap_auto_conn_scan_cmpl_handler,
                                             smartbridge_gap_auto_conn_scan_report_handler);

        /* Update remain duration */
        if (err == MICO_BT_PENDING && g_auto_conn_scan_duration < (uint16_t)(-1)) {
            g_auto_conn_scan_duration -= MIN(SMARTBRIDGE_AUTO_CONN_SCAN_INTERVAL, g_auto_conn_scan_duration);
        }
    } else {
        bt_smartbridge_log("scanning for auto-connection finished");
    }

    return err;
}

static OSStatus smartbridge_gap_auto_conn_cryption( void *arg )
{
    OSStatus result = kNoErr;
    mico_bt_smartbridge_socket_t *socket = (mico_bt_smartbridge_socket_t *)arg;

    if (socket != NULL && socket->auto_connection_callback != NULL) {
        if (socket->security_settings.authentication_requirements != BT_SMART_AUTH_REQ_NONE) {
            /* Encryption */
            bt_smartbridge_log("Encryption started.");
            mico_bt_start_encryption( &socket->remote_device.address );
            mico_rtos_get_semaphore(&g_auto_conn_sem, MICO_NEVER_TIMEOUT);
            socket->state = SOCKET_STATE_LINK_ENCRYPTED;
            bt_smartbridge_log("Encryption completed.");
        }

        /* Cache if enabled. */
        if (bt_smartbridge_att_cache_is_enabled() == MICO_TRUE) {
            bt_smartbridge_log("ATT Cache for an auto connection.");

            bt_smartbridge_att_cache_t *cache = NULL;

            result = bt_smartbridge_att_cache_find(&socket->remote_device, &cache);
            if (result == MICO_BT_SUCCESS) {
                bt_smartbridge_log("USING ATT CACHE ...");
            } else {
                bt_smartbridge_log("GENERATING ATT CACHE ...");

                result = bt_smartbridge_att_cache_generate(&socket->remote_device, socket->connection_handle, &cache);

                if (result != MICO_BT_SUCCESS) {
                    bt_smartbridge_log("Error Generating Cache result:%d", result );
                    goto error;
                }
            }

            /* Successful. Mark cache as active and store reference in socket */
            bt_smartbridge_att_cache_set_active_state(cache, MICO_TRUE);
            socket->att_cache = (void *)cache;
        }

        /* Notify user */
        return result;
    }

error:
    return MICO_BT_ERROR;
}

static mico_bool_t smartbridge_gap_auto_conn_user_cfg( const mico_bt_device_address_t device_address,
                                                       const uint8_t *device_name, const uint8_t *p_data, uint8_t length )
{
    extern mico_bt_cfg_settings_t           mico_bt_cfg_settings;
    int8_t                                  idx;
    smartbridge_auto_conn_dev_info_t *device_info = NULL;

    /* Allocate an entity for storing this connection object. */
    if ((idx = smartbridge_auto_conn_find_dev_by_addr(device_address)) < 0) {
        bt_smartbridge_log("Device is not Existed!!");
        idx = smartbridge_auto_conn_dev_alloc();
        require_string(idx >= 0, exit, "No resource for the auto connection");
    }

    /* Prepare Device Information Structure */
    device_info = &g_auto_conn_dev_table[idx].device_info;
    memset(device_info, 0, sizeof(smartbridge_auto_conn_dev_info_t));

    device_info->handle = (uint8_t)idx;
    device_info->length = length;
    memcpy(device_info->bdaddr, device_address, 6);
    memcpy(device_info->name, device_name, MIN(strlen((const char *)device_name), 16 - 1));
    memcpy(device_info->adv_data, p_data, length);

    /* Request socket information and connection configuration. */
    smartbridge_auto_conn_user_parms((void *)device_info);

    /* Assign connection configuration parameters for auto connection procedure. */
    require_string(g_auto_conn_dev_table[idx].socket != NULL, exit, "User not allow this auto connection");
    mico_bt_cfg_settings.ble_scan_cfg.conn_min_interval        = g_auto_conn_dev_table[idx].conn_settings.interval_min;
    mico_bt_cfg_settings.ble_scan_cfg.conn_max_interval        = g_auto_conn_dev_table[idx].conn_settings.interval_max;
    mico_bt_cfg_settings.ble_scan_cfg.conn_latency             = g_auto_conn_dev_table[idx].conn_settings.latency;
    mico_bt_cfg_settings.ble_scan_cfg.conn_supervision_timeout =
        g_auto_conn_dev_table[idx].conn_settings.supervision_timeout;
    return MICO_TRUE;
exit:
    return MICO_FALSE;
}


static OSStatus smartbridge_auto_conn_user_parms( void *arg )
{
    OSStatus err = kNoErr;
    mico_bt_smartbridge_auto_conn_cback_parms_t     parm;
    smartbridge_auto_conn_dev_info_t          *p_dev_info = (smartbridge_auto_conn_dev_info_t *) arg;
    smartbridge_auto_conn_entity_t          *p_entity;

    /* Request socket information and connection configuration. */

    p_entity = &g_auto_conn_dev_table[p_dev_info->handle];

    /* Release the socket resource */
    if (p_entity->socket != NULL) {
        p_entity->socket->state = SOCKET_STATE_DISCONNECTED;
    }

    /* Get a free socket and connection configuration */
    if ( (err = g_auto_conn_cback(p_dev_info->bdaddr,
                                  (char *) p_dev_info->name,
                                  p_dev_info->adv_data,
                                  p_dev_info->length,
                                  &parm)
         ) != kNoErr ) {
        /* Clear */
        p_entity->socket = (void *) 0;
        memset( (void *) &p_entity->conn_settings, 0, sizeof(mico_bt_smart_connection_settings_t) );
        bt_smartbridge_log(" Not allow this auto connection");
    } else {
        /* Socket */
        p_entity->socket = parm.socket;
        p_entity->socket->state = SOCKET_STATE_LINK_CONNECTING;
        p_entity->socket->auto_connection_callback = parm.auto_connection_callback;
        p_entity->socket->disconnection_callback = parm.auto_disconn_callback;
        p_entity->socket->notification_callback = parm.notification_callback;

        /* Connection settings */
        memcpy( (void *) &p_entity->conn_settings, (void *) &parm.conn_settings,
                sizeof(mico_bt_smart_connection_settings_t) );
        /* Security settings */
        memcpy( (void *) &p_entity->socket->security_settings, (void *) &parm.security_settings,
                sizeof(mico_bt_smart_security_settings_t) );
    }
    return err;
}


/* Find a valid auto connection entity by address. */
static int8_t smartbridge_auto_conn_find_dev_by_addr(const mico_bt_device_address_t device_address)
{
    int8_t idx = 0;
    mico_bt_smartbridge_socket_t *temp_socket;

    for (idx = 0; idx < sizeof(g_auto_conn_dev_table) / sizeof(g_auto_conn_dev_table[0]); idx++) {
        temp_socket = g_auto_conn_dev_table[idx].socket;
        if ((temp_socket != (void *)0)
            && (memcmp((void *)temp_socket->remote_device.address,
                       (void *)device_address,
                       sizeof(mico_bt_device_address_t)) == 0)) {
            return idx;
        }
    }
    return -1;
}

/* Allocate a free auto connection entity. */
static int8_t smartbridge_auto_conn_dev_alloc(void)
{
    int8_t idx = 0;

    for (idx = 0; idx < sizeof(g_auto_conn_dev_table) / sizeof(g_auto_conn_dev_table[0]); idx++) {
        if (g_auto_conn_dev_table[idx].socket == (void *)0) {
            memset((void *)&g_auto_conn_dev_table[idx].conn_settings,
                   0,
                   sizeof(mico_bt_smart_connection_settings_t));
            return idx;
        }
    }
    return -1;
}

void smartbridge_auto_connection_encryption_check(const mico_bt_dev_pairing_cplt_t *p_pairing_cplt)
{
    if (p_pairing_cplt == NULL) {
        bt_smartbridge_log("Check auto connection encryption: invalid parameters");
        return;
    }
    int idx = smartbridge_auto_conn_find_dev_by_addr(p_pairing_cplt->bd_addr);

    if (idx >= 0) {
        if (g_auto_conn_dev_table[idx].socket->security_settings.authentication_requirements
            != BT_SMART_AUTH_REQ_NONE) {
            mico_rtos_set_semaphore(&g_auto_conn_sem);
        }
    }
}

static OSStatus smartbridge_auto_conn_list_init(void)
{
    linked_list_init(&g_auto_conn_list.list);
    return mico_rtos_init_mutex(&g_auto_conn_list.mutex);
}

static OSStatus smartbridge_auto_conn_list_deinit(void)
{
    return mico_rtos_deinit_mutex(&g_auto_conn_list.mutex);
}

static mico_bool_t smartbridge_auto_conn_compare_bdaddr(linked_list_node_t *node_to_compare, void *user_data)
{
    smartbridge_auto_conn_report_t *report = (smartbridge_auto_conn_report_t *)node_to_compare;
    mico_bt_device_address_t *device_address  = (mico_bt_device_address_t *)user_data;

    if (memcmp(report->report.remote_device.address, device_address, BD_ADDR_LEN) == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static mico_bool_t smartbridge_auto_conn_compare_state(linked_list_node_t *node_to_compare, void *user_data)
{
    smartbridge_auto_conn_report_t *report = (smartbridge_auto_conn_report_t *)node_to_compare;
    smartbridge_auto_conn_state_t state = *(smartbridge_auto_conn_state_t *)user_data;

    if (state != report->state) {
        return FALSE;
    } else {
        return TRUE;
    }
}

static OSStatus smartbridge_auto_conn_list_add(const mico_bt_smart_advertising_report_t *report)
{
    OSStatus                        err = kNoErr;
    smartbridge_auto_conn_report_t *report_found = NULL, *new_report = NULL;

    uint32_t                        count = 0;
    smartbridge_auto_conn_state_t   state = SMARTBRIDGE_AUTO_CONN_STATE_FREE;

    require_action(report != NULL, exit, err = kParamErr);

    /* Check if this report is existed. */
    mico_rtos_lock_mutex(&g_auto_conn_list.mutex);
    err = linked_list_find_node(&g_auto_conn_list.list,
                                (linked_list_compare_callback_t)smartbridge_auto_conn_compare_bdaddr,
                                (void *)report->remote_device.address,
                                (linked_list_node_t **)&report_found);

    /* If already existed, we should update its EIR Data. */
    if (err == kNoErr) {
        bt_smartbridge_log("%s: already existed, state[%d], event[%d]",
                           __FUNCTION__, report_found->state, report_found->report.event);

        if (report_found->state == SMARTBRIDGE_AUTO_CONN_STATE_FREE
            || (report_found->state == SMARTBRIDGE_AUTO_CONN_STATE_SCANED
                && report_found->report.event != report->event)) {
            /* Remove it from the list. */
            linked_list_remove_node(&g_auto_conn_list.list, &report_found->this_node);

            /* Already existed and if its state is FREE or SCANED we should update it to reuse. */
            report_found->report.signal_strength = report->signal_strength;
            report_found->report.event = report->event;
            report_found->report.eir_data_length = report->eir_data_length;
            memcpy(&report_found->report.eir_data, report->eir_data, report->eir_data_length);

            /* Add it to the list rear. */
            err = linked_list_insert_node_at_rear(&g_auto_conn_list.list, &report_found->this_node);
        } else {
            /* Already existed and we may release it after next scaning procedure
             * in smartbridge_auto_conn_scan_cmpl_handler.
             */
            err = kAlreadyInUseErr;
        }
        mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);
        return err;
    } else {
        linked_list_get_count(&g_auto_conn_list.list, &count);
        bt_smartbridge_log("%s: not existed, count[%lu]", __FUNCTION__, count);

        if (count > 0) {
            /* Are there a free report ? */
            err = linked_list_find_node(&g_auto_conn_list.list,
                                        (linked_list_compare_callback_t)smartbridge_auto_conn_compare_state,
                                        (void *)&state,
                                        (linked_list_node_t **)&report_found);
            if (err == kNoErr) {
                /* Yeah, we should reuse it. */
                linked_list_remove_node(&g_auto_conn_list.list, &report_found->this_node);
                new_report = report_found;
            }
        }
    }
    mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);

    /* Not existed. */
    if (new_report == NULL) {
        new_report = malloc(sizeof(smartbridge_auto_conn_report_t));
        if (!new_report) {
            bt_smartbridge_log("%s: malloc failed", __FUNCTION__);
            return kNoMemoryErr;
        }
    }

    memset(new_report, 0, sizeof(smartbridge_auto_conn_report_t));
    new_report->state = SMARTBRIDGE_AUTO_CONN_STATE_SCANED;
    new_report->this_node.data = new_report;
    memcpy(&new_report->report, report, sizeof(mico_bt_smart_advertising_report_t));

    mico_rtos_lock_mutex(&g_auto_conn_list.mutex);
    err = linked_list_insert_node_at_rear(&g_auto_conn_list.list, &new_report->this_node);
    mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);

exit:
    return err;
}

static OSStatus smartbridge_auto_conn_list_get_by_state(mico_bt_smart_advertising_report_t **report,
                                                        smartbridge_auto_conn_state_t state)
{
    OSStatus err = kNoErr;
    smartbridge_auto_conn_report_t *current_report;

    require_action(report != NULL, exit, err = kParamErr);

    mico_rtos_lock_mutex(&g_auto_conn_list.mutex);
    err = linked_list_find_node(&g_auto_conn_list.list,
                                (linked_list_compare_callback_t)smartbridge_auto_conn_compare_state,
                                (void *)&state,
                                (linked_list_node_t **)&current_report);

    *report = &current_report->report;
    mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);

exit:
    return err;
}

static OSStatus smartbridge_auto_conn_list_set_state(const mico_bt_device_address_t bdaddr,
                                                     smartbridge_auto_conn_state_t state)
{
    OSStatus err = kNoErr;
    smartbridge_auto_conn_report_t *report_found;

    require_action(bdaddr != NULL, exit, err = kParamErr);

    mico_rtos_lock_mutex(&g_auto_conn_list.mutex);
    err = linked_list_find_node(&g_auto_conn_list.list,
                                (linked_list_compare_callback_t)smartbridge_auto_conn_compare_bdaddr,
                                (void *)bdaddr,
                                (linked_list_node_t **)&report_found);
    if (err == kNoErr) {
        report_found->state = state;
    }
    mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);

exit:
    return err;
}

static OSStatus smartbridge_auto_conn_list_remove(mico_bt_device_address_t bdaddr)
{
    OSStatus err = kNoErr;
    smartbridge_auto_conn_report_t *current_report;

    mico_rtos_lock_mutex(&g_auto_conn_list.mutex);
    err = linked_list_find_node(&g_auto_conn_list.list,
                                (linked_list_compare_callback_t)smartbridge_auto_conn_compare_bdaddr,
                                bdaddr,
                                (linked_list_node_t **)&current_report);
    if (err != kNoErr) {
        goto exit;
    }
    current_report->state = SMARTBRIDGE_AUTO_CONN_STATE_FREE;
exit:
    mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);
    return err;
}

static OSStatus smartbridge_auto_conn_list_remove_by_state(smartbridge_auto_conn_state_t state)
{
    OSStatus err = kNoErr;
    smartbridge_auto_conn_report_t *current_report;

    mico_rtos_lock_mutex(&g_auto_conn_list.mutex);
    do {
        err = linked_list_find_node(&g_auto_conn_list.list,
                                    (linked_list_compare_callback_t)smartbridge_auto_conn_compare_state,
                                    &state,
                                    (linked_list_node_t **)&current_report);
        if (err != kNoErr) {
            break;
        }
        current_report->state = SMARTBRIDGE_AUTO_CONN_STATE_FREE;
    } while (1);
    mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);

    return kNoErr;
}

static OSStatus smartbridge_auto_conn_list_clear(void)
{
    OSStatus err = kNoErr;
    smartbridge_auto_conn_report_t *current_report = NULL;

    mico_rtos_lock_mutex(&g_auto_conn_list.mutex);
    do {
        err = linked_list_get_front_node(&g_auto_conn_list.list, (linked_list_node_t **)&current_report);
        if (err != kNoErr) {
            break;
        }
        linked_list_remove_node(&g_auto_conn_list.list, &current_report->this_node);

        free(current_report);
    } while (1);
    mico_rtos_unlock_mutex(&g_auto_conn_list.mutex);

    return kNoErr;
}


