/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>

#include "smartbt.h"
#include "smartbt_cfg.h"
#include "smartbt_smart_interface.h"
#include "smartbt_smartbridge.h"
#include "smartbt_smartbridge_gatt.h"

#include "StringUtils.h"
#include "LinkListUtils.h"

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/

#define MAX_CONCURRENT_CONNECTIONS          (10)

#define MAX_ATTRIBUTE_CACHE_COUNTS          (10)

#define OUT_OF_BAND_AUTHENTICATION          BT_SMART_OOB_AUTH_NONE

#define AUTHENTICATION_REQUIREMENTS         BT_SMART_AUTH_REQ_BONDING

/* UUID value of the Hello Sensor Service */
#define UUID_HELLO_SERVICE                  0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x51, 0x82, 0x7e, 0x1b

/* UUID value of the Hello Sensor Characteristic, Value Notification */
#define UUID_HELLO_CHARACTERISTIC_NOTIFY    0x26, 0xf6, 0x69, 0x91, 0x68, 0xee, 0xc2, 0xbe, 0x44, 0x4d, 0xb9, 0x5c, 0x3f, 0x2d, 0xc3, 0x8a

/* UUID value of the Hello Sensor Characteristic, Color Configuration */
#define UUID_HELLO_CHARACTERISTIC_COLOR     0x1a, 0x89, 0x07, 0x4a, 0x2f, 0x3b, 0x7e, 0xa6, 0x81, 0x44, 0x3f, 0xf9, 0xa8, 0xf2, 0x9b, 0x5e


#define LENGTH_HELLO_CHARACTERISTIC_COLOR   (1)


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/* Device info ready to connect */
typedef struct {
    linked_list_node_t          this_node;     /* Linked-list node of this deivice */
    aos_bt_smart_device_t      device;        /* Remote BT device */
} connecting_device_t;

/******************************************************
 *                    Structures
 ******************************************************/

/* SmartBridge security settings */
const aos_bt_smart_security_settings_t security_settings = {
    .timeout_second              = 15,
    .io_capabilities             = BT_SMART_IO_DISPLAY_ONLY,
    .authentication_requirements = AUTHENTICATION_REQUIREMENTS,
    .oob_authentication          = OUT_OF_BAND_AUTHENTICATION,
    .max_encryption_key_size     = 16,
    .master_key_distribution     = BT_SMART_DISTRIBUTE_ENCRYPTION_AND_SIGN_KEYS,
    .slave_key_distribution      = BT_SMART_DISTRIBUTE_ALL_KEYS,
};

/* SmartBridge connection settings */
static const aos_bt_smart_connection_settings_t connection_settings = {
    .timeout_second                = 10,
    .filter_policy                 = FILTER_POLICY_NONE,
    .interval_min                  = AOS_BT_CFG_DEFAULT_CONN_MIN_INTERVAL,
    .interval_max                  = AOS_BT_CFG_DEFAULT_CONN_MAX_INTERVAL,
    .latency                       = AOS_BT_CFG_DEFAULT_CONN_LATENCY,
    .supervision_timeout           = AOS_BT_CFG_DEFAULT_CONN_SUPERVISION_TIMEOUT,
    .ce_length_min                 = 0,
    .ce_length_max                 = 0,
    .attribute_protocol_timeout_ms = 10000,
};

/* SmartBridge auto scan settings */
static const aos_bt_smart_scan_settings_t scan_settings = {
    .type              = BT_SMART_PASSIVE_SCAN,
    .filter_policy     = FILTER_POLICY_NONE,
    .filter_duplicates = DUPLICATES_FILTER_ENABLED,
    .interval          = 512,
    .window            = 48,
    .duration_second   = 10,
};

/******************************************************
 *               Static Function Declarations
 ******************************************************/
static OSStatus connect_handler                 ( void *arg );
static OSStatus disconnection_handler           ( aos_bt_smartbridge_socket_t *socket );
static OSStatus notification_handler            ( aos_bt_smartbridge_socket_t *socket, uint16_t attribute_handle );
static OSStatus scan_complete_handler           ( void *arg );
static OSStatus scan_result_handler             ( const aos_bt_smart_advertising_report_t *result );
static OSStatus sensor_trigger_handler_t        ( void *arg );

/* Remote device list management */
static OSStatus connect_list_init   ( void );
static OSStatus connect_list_add    ( aos_bt_smart_device_t  remote_device );
static OSStatus connect_list_get    ( aos_bt_smart_device_t **address );
static OSStatus connect_list_remove ( aos_bt_smart_device_t *device );

/******************************************************
 *               Function Declarations
 ******************************************************/

extern void ble_commands_init ( aos_bt_smartbridge_socket_t *socket, uint8_t socket_count );
extern OSStatus ble_hello_peripheral_init( void );

/* Wlan test thread, connect to a TCP server and echo */
extern OSStatus wlan_coexist_test_start( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/

const char desired_peer_device[] = "AOSKit-3239";      /* Name of desired peer device to connect to */
const aos_bt_uuid_t hello_service_uuid = { .len = LEN_UUID_128, .uu.uuid128 = { UUID_HELLO_SERVICE } };  /* Service for operation */
const aos_bt_uuid_t hello_color_uuid = { .len = LEN_UUID_128, .uu.uuid128 = { UUID_HELLO_CHARACTERISTIC_COLOR } }; /* Characteristic for AOSKit led control */

static aos_bt_smartbridge_socket_t  smartbridge_socket[MAX_CONCURRENT_CONNECTIONS]; /* Sockets to manage connections */
static uint32_t
hello_color_value_handle[MAX_CONCURRENT_CONNECTIONS]; /* Handles to manage characteristic for each connection */

static aos_worker_thread_t  hello_center_worker_thread; /* Worker thread to manage connection events */

static aos_worker_thread_t
hello_sensor_color_toggle_worker_thread;  /* Worker thread to manage timed event for AOSKit led control */
static aos_timed_event_t   hello_sensor_color_toggle_event;

static uint8_t color_idx = 0; /* Currnet color index for every connected AOSKit */
static linked_list_t connecting_device_list;

/******************************************************
 *               Function Definitions
 ******************************************************/
int application_start(void)
{
    OSStatus err = oNoErr;
    uint32_t a;

    /* start a wlan function */
#ifdef WLAN_COEXIST_TEST
    err = wlan_coexist_test_start();
    require_noerr( err, exit );
#endif

    /* Initialise AOS Bluetooth Framework */
    err = aos_bt_init( AOS_BT_HCI_MODE, "SmartBridge Device", MAX_CONCURRENT_CONNECTIONS,
                       1 );  //Client + server connections
    require_noerr_string( err, exit, "Error initialising AOS Bluetooth Framework" );

    err = ble_hello_peripheral_init( );
    require_noerr_string( err, exit, "Failed to init ble hello peripheral demo!" );

    /* Initialise AOS SmartBridge */
    err = aos_bt_smartbridge_init( MAX_CONCURRENT_CONNECTIONS );
    require_noerr( err, exit );

    printf( "Initialize success");

    /* Enable Attribute Cache and set maximum number of caches */
    err = aos_bt_smartbridge_enable_attribute_cache( MAX_ATTRIBUTE_CACHE_COUNTS, (aos_bt_uuid_t *)&hello_service_uuid, 1 );
    require_noerr_string( err, exit, "Failed to enable ATT Cache" );

    /* Create all sockets and make them ready to connect. A socket can connect and disconnect multiple times. */
    for ( a = 0; a < MAX_CONCURRENT_CONNECTIONS; a++ ) {
        aos_bt_smartbridge_create_socket( &smartbridge_socket[a] );
    }

    /* Create a worker thread for making a connection and control event */
    aos_rtos_create_worker_thread( &hello_center_worker_thread, AOS_APPLICATION_PRIORITY, 2048, 1 );

    aos_rtos_create_worker_thread( &hello_sensor_color_toggle_worker_thread, AOS_APPLICATION_PRIORITY, 1024, 1 );

    aos_rtos_register_timed_event( &hello_sensor_color_toggle_event, &hello_sensor_color_toggle_worker_thread,
                                   sensor_trigger_handler_t, 1000, NULL );

    printf("Scanning for %s...\n", desired_peer_device);

    err = connect_list_init( );
    require_noerr( err, exit );

    aos_bt_smartbridge_start_scan( &scan_settings, scan_complete_handler, scan_result_handler );

#ifdef AOS_CLI_ENABLE
    /* Register ble helper commands to cli interface */
    ble_commands_init( smartbridge_socket, MAX_CONCURRENT_CONNECTIONS );
#endif

exit:
    aos_rtos_delete_thread( NULL );
    return err;
}

OSStatus sensor_trigger_handler_t(void *arg)
{
    UNUSED_PARAMETER(arg);
    uint32_t i;
    aos_bt_smartbridge_socket_status_t status;
    OSStatus err = oNoErr;

    aos_bt_smart_attribute_t *characteristic_value = NULL;

    aos_bt_smart_attribute_create( &characteristic_value, AOS_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE,
                                   LENGTH_HELLO_CHARACTERISTIC_COLOR );

    for ( i = 0; i < MAX_CONCURRENT_CONNECTIONS; i++ ) {
        aos_bt_smartbridge_get_socket_status( &smartbridge_socket[i], &status );

        if ( status == SMARTBRIDGE_SOCKET_CONNECTED && smartbridge_socket[i].att_cache != NULL ) {
            err = aos_bt_smartbridge_get_attribute_cache_by_handle( &smartbridge_socket[i], hello_color_value_handle[i],
                                                                    characteristic_value, ATTR_CHARACTERISTIC_VALUE_SIZE( LENGTH_HELLO_CHARACTERISTIC_COLOR ) );
            if ( err == oNoErr ) {
                memcpy( characteristic_value->value.value,  &color_idx, 1);
                aos_bt_smartbridge_write_attribute_cache_characteristic_value( &smartbridge_socket[i], characteristic_value );
            } else if ( err == AOS_BT_DISCOVER_IN_PROGRESS ) {
                printf( "ATT cache discover is in progress");
            } else {
                printf( "Hello color characteristic not found, disconnect");
                aos_bt_smartbridge_disconnect( &smartbridge_socket[i], FALSE );
            }
        }
    }

    color_idx ++;

    if ( characteristic_value != NULL ) {
        aos_bt_smart_attribute_delete( characteristic_value );
    }

    return oNoErr;
}

/* Scan complete handler. Scan complete event reported via this callback.
 * It runs on the AOS_NETWORKING_WORKER_THREAD context.
 */
OSStatus scan_complete_handler( void *arg )
{
    UNUSED_PARAMETER(arg);
    /* Scan complete, start a new scan. Donot use a infinit scan, it may store every result in RAM. */
    aos_bt_smartbridge_start_scan( &scan_settings, scan_complete_handler, scan_result_handler );
    return oNoErr;
}


OSStatus scan_result_handler( const aos_bt_smart_advertising_report_t *scan_result )
{
    /* If connection not initiated yet, and this device has the name we are looking for, then initiate teh connection */
    if ( memcmp( scan_result->remote_device.name, desired_peer_device, strlen(desired_peer_device)) == 0) {
        connect_list_add( scan_result->remote_device );
        aos_rtos_send_asynchronous_event( &hello_center_worker_thread, connect_handler, (void *)scan_result );
    }
    return oNoErr;
}

/* Connect handler. Smartbridge connect is executed in this callback.
 * It runs on the connect_worker_thread context
 */
static OSStatus connect_handler( void *arg )
{
    UNUSED_PARAMETER(arg);
    uint32_t i;
    OSStatus err = oNoErr;
    aos_bt_smartbridge_socket_status_t status;

    uint8_t attribute_buffer[100];
    aos_bt_smart_attribute_t *attribute = (aos_bt_smart_attribute_t *)attribute_buffer;
    char *bt_addr_str = NULL;

    aos_bt_smart_device_t *remote_device;

    while ( connect_list_get( &remote_device ) == oNoErr ) {
        bt_addr_str = DataToHexStringWithColons( (uint8_t *)remote_device->address, 6 );
        printf("Opening GATT connection to [%s] (addr type=%s)...", bt_addr_str,
               (remote_device->address_type == BT_SMART_ADDR_TYPE_PUBLIC ) ? "Public" : "Random");
        free( bt_addr_str );

        /* Iterate all sockets and look for the first available socket */
        for ( i = 0; i < MAX_CONCURRENT_CONNECTIONS; i++ ) {
            aos_bt_smartbridge_get_socket_status( &smartbridge_socket[i], &status );

            /* A free socket is found. Use it to connect */
            if ( status == SMARTBRIDGE_SOCKET_DISCONNECTED ) {
                /* If there is a previously stored device, then connect to it */
                if (security_settings.authentication_requirements != BT_SMART_AUTH_REQ_NONE) {
                    if ( aos_bt_dev_find_bonded_device( (uint8_t *)remote_device->address ) == AOS_FALSE ) {
                        printf( "Bond info not found. Initiate pairing request." );
                        aos_bt_smartbridge_enable_pairing( &smartbridge_socket[i], &security_settings, NULL );
                    } else {
                        printf( "Bond info found. Encrypt use bond info." );
                        aos_bt_smartbridge_set_bond_info( &smartbridge_socket[i], &security_settings, NULL );
                    }
                }

                /* Connect */
                err = aos_bt_smartbridge_connect( &smartbridge_socket[i], remote_device, &connection_settings, disconnection_handler,
                                                  notification_handler );
                require_noerr_string( err, exit, "Hello sensor connect failed." );
                printf( "Smartbridge connection established." );

                /* Find service */
                err = aos_bt_smartbridge_get_service_from_attribute_cache_by_uuid( &smartbridge_socket[i], &hello_service_uuid, 0x0,
                                                                                   0xFFFF, attribute, 100 );
                require_noerr_action_string( err, exit, aos_bt_smartbridge_disconnect( &smartbridge_socket[i], FALSE ),
                                             "Hello service not found, disconnect." );

                /* Find characteristic, and save characteristic value handle */
                err = aos_bt_smartbridge_get_characteritics_from_attribute_cache_by_uuid( &smartbridge_socket[i], &hello_color_uuid,
                                                                                          attribute->value.service.start_handle, attribute->value.service.end_handle,
                                                                                          (aos_bt_smart_attribute_t *)attribute_buffer, 100 );
                if ( err != oNoErr ) {
                    printf( "Hello color characteristic not found, remove att cache and disconnect." );
                    aos_bt_smartbridge_remove_attribute_cache( &smartbridge_socket[i] );
                    aos_bt_smartbridge_disconnect( &smartbridge_socket[i], FALSE );
                }

                hello_color_value_handle[i] = attribute->value.characteristic.value_handle;

                /* Enable Attribute Cache notification */
                //err = aos_bt_smartbridge_enable_attribute_cache_notification( &smartbridge_socket[i], AOS_TRUE );
                //require_noerr_string( err, exit, "Attribute cache notification failed." );

                goto exit;
            }
        }
exit:
        connect_list_remove( remote_device );
        continue;
    }
    return err;
}

/* Disconnection handler. Disconnection by remote device is reported via this callback.
 * It runs on the AOS_NETWORKING_WORKER_THREAD context.
 */
static OSStatus disconnection_handler( aos_bt_smartbridge_socket_t *socket )
{
    char *bt_addr_str = DataToHexStringWithColons( (uint8_t *)socket->remote_device.address, 6 );
    printf( "Disconnected from [%s]", bt_addr_str );
    free( bt_addr_str );
    return oNoErr;
}

/* Notification handler. GATT notification by remote device is reported via this callback.
 * It runs on the AOS_NETWORKING_WORKER_THREAD context.
 */
static OSStatus notification_handler( aos_bt_smartbridge_socket_t *socket, uint16_t attribute_handle )
{
    /* GATT value notification event. attribute_handle is the handle
     * which value of the attribute is updated by the remote device.
     */
    OSStatus err = oNoErr;
    char *bt_addr_str = NULL;
    char *data_str = NULL;
    aos_bt_smart_attribute_t *characteristic_value = NULL;

    /* Read sender's address */
    bt_addr_str = DataToHexStringWithColons( (uint8_t *)socket->remote_device.address, 6 );

    /* Read cached data */
    err = aos_bt_smart_attribute_create( &characteristic_value, AOS_ATTRIBUTE_TYPE_CHARACTERISTIC_VALUE,
                                         MAX_CHARACTERISTIC_VALUE_LENGTH );
    require_noerr(err, exit);
    err = aos_bt_smartbridge_get_attribute_cache_by_handle( socket, attribute_handle, characteristic_value,
                                                            ATTR_CHARACTERISTIC_VALUE_SIZE( MAX_CHARACTERISTIC_VALUE_LENGTH ) );
    require_noerr_string(err, exit, "This is not a cached value handle, ignore...");
    data_str = DataToHexStringWithSpaces( characteristic_value->value.value, characteristic_value->value_length );

    printf( "Recv data from [%s], handle 0x%x: %s", bt_addr_str, attribute_handle, data_str );

exit:
    if ( bt_addr_str != NULL ) {
        free( bt_addr_str );
    }
    if ( data_str != NULL ) {
        free( data_str );
    }
    if ( characteristic_value != NULL ) {
        aos_bt_smart_attribute_delete( characteristic_value );
    }
    return err;
}

static OSStatus connect_list_init( void )
{
    return linked_list_init( &connecting_device_list );
}

static bool compare_device_by_address( linked_list_node_t *node_to_compare, void *user_data )
{
    connecting_device_t *device = (connecting_device_t * )node_to_compare;
    aos_bt_device_address_t *device_address  = (aos_bt_device_address_t *)user_data;

    if ( memcmp( device->device.address, device_address, BD_ADDR_LEN ) == 0 ) {
        return true;
    } else {
        return false;
    }
}

static OSStatus connect_list_add( aos_bt_smart_device_t remote_device )
{
    OSStatus err = oNoErr;
    connecting_device_t *device_found, *new_device;

    err = linked_list_find_node( &connecting_device_list, compare_device_by_address, remote_device.address,
                                 (linked_list_node_t **)&device_found );
    require_quiet( err == oNotFoundErr, exit );

    new_device = malloc( sizeof(connecting_device_t) );
    require_action( new_device, exit, err = oNoMemoryErr );

    memcpy( &new_device->device, &remote_device, sizeof(aos_bt_smart_device_t) );

    err = linked_list_insert_node_at_rear( &connecting_device_list, &new_device->this_node );
    require_noerr(err, exit);

exit:
    return err;
}

static OSStatus connect_list_get( aos_bt_smart_device_t **device )
{
    OSStatus err = oNoErr;
    connecting_device_t *current_device;

    err = linked_list_get_front_node( &connecting_device_list, (linked_list_node_t **)&current_device );
    require_noerr_quiet(err, exit);

    *device = &current_device->device;

exit:
    return err;
}

static OSStatus connect_list_remove( aos_bt_smart_device_t *device )
{
    OSStatus err = oNoErr;
    connecting_device_t *current_device;

    err = linked_list_find_node( &connecting_device_list, compare_device_by_address, device->address,
                                 (linked_list_node_t **)&current_device );
    require_noerr( err, exit );

    err = linked_list_remove_node( &connecting_device_list, &current_device->this_node );
    require_noerr( err, exit );

    free( current_device );
exit:
    return err;
}










