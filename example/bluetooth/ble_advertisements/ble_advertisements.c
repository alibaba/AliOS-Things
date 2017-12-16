/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "sdpdefs.h"
#include "smartbt_cfg.h"
#include "smartbt.h"
#include "smartbt_peripheral.h"

#include "ble_advertisements.h"

#define MANUFACTURER "BleTestManufacturer"
#define MODEL "BleTestModel"

//#define printf(M, ...) custom_log( "LE ADVERT", M, ##__VA_ARGS__ )

/******************************************************************************
 *                                Constants
 ******************************************************************************/

/******************************************************************************
 *                           Function Prototypes
 ******************************************************************************/

static void hello_sensor_change_advertisements( CLI_ARGS );
static void hello_sensor_change_filter_policy ( CLI_ARGS );
static void hello_sensor_get_whitelist_size   ( CLI_ARGS );

static void hello_sensor_start_advertisements( void );
static void hello_sensor_stop_advertisements( void );

#ifdef USE_AOSKit_EXT
static const char *pBT_ShortStr( aos_bt_smart_advertising_type_t type );
#endif

static const char *pBT_AdvertStr( aos_bt_smart_advertising_type_t type );

/******************************************************************************
 *                                Structures
 ******************************************************************************/

/* Peripheral auto advertising settings */
static aos_bt_smart_advertising_settings_t advertising_settings = {
    .type               = BT_SMART_UNDIRECTED_ADVERTISING,                  /**< Advertising type                                               */
    .use_high_duty      = AOS_TRUE,                                        /**< Using high duty to start advertising                           */
    .high_duty_interval = AOS_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL,   /**< High duty advertising interval                                 */
    .high_duty_duration = 5,                                                /**< High duty advertising duration in seconds (0 for infinite)     */
    .low_duty_interval  = AOS_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL,    /**< Low duty advertising interval                                  */
    .low_duty_duration  = 60,                                               /**< Low duty advertising duration in seconds (0 for infinite)      */

    /* Used Directed Advertisement */
    .directed_advertisement_addr_type   = BT_SMART_ADDR_TYPE_PUBLIC,
    .directed_advertisement_addr        = { 11, 22, 33, 44, 55, 66 },
};

/* Peripheral security settings */
static const aos_bt_smart_security_settings_t security_settings = {
    .timeout_second                 = 15,
    .io_capabilities                = BT_SMART_IO_DISPLAY_ONLY,
    .authentication_requirements    = BT_SMART_AUTH_REQ_BONDING,
    .oob_authentication             = BT_SMART_OOB_AUTH_NONE,
    .max_encryption_key_size        = 16,
    .master_key_distribution        = BT_SMART_DISTRIBUTE_ALL_KEYS,
    .slave_key_distribution         = BT_SMART_DISTRIBUTE_ALL_KEYS,
};

struct cli_command hello_sensor_send_message_cmd[] = {
    {
        "advert",
        "<start/stop> <0:Generic undirected/1:Discoverable advertising/2:Directed/3:Non-connectable Undirected> <0:Low Duty/1:High Duty>",
        hello_sensor_change_advertisements
    },
    {
        "policy",
        "<0:Not use white list/1:filter all connection request and whitelist scan reuqest/2:filter whitelist connection request and all scan request/3:filter whitelist connection request and whitelist scan request>",
        hello_sensor_change_filter_policy
    },
    {
        "wlsize",
        "Get the number of devices in the white list",
        hello_sensor_get_whitelist_size
    }
};

#define USER_CLI_COMMAND_SIZE   ( sizeof( hello_sensor_send_message_cmd ) / sizeof( hello_sensor_send_message_cmd[0] ) )

/******************************************************************************
 *                                Variables Definitions
 ******************************************************************************/

/* Initialized attribute value */
static uint8_t hello_sensor_appearance_name[2] = { BIT16_TO_8( APPEARANCE_GENERIC_TAG ) };
static uint8_t hello_sensor_char_system_id_value[] = { 0xbb, 0xb8, 0xa1, 0x80, 0x5f, 0x9f, 0x91,
                                                       0x71
                                                     };

/* AOS BT Smart peripheral connection controller */
static aos_bt_peripheral_socket_t peripheral_socket;

/******************************************************************************
 *                                GATT DATABASE
 ******************************************************************************/

const uint8_t hello_sensor_gatt_database[] = {
    /* Declare mandatory GATT service */
    PRIMARY_SERVICE_UUID16( HDLS_GENERIC_ATTRIBUTE, UUID_SERVCLASS_GATT_SERVER ),

    CHARACTERISTIC_UUID16( HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,
    GATT_UUID_GATT_SRV_CHGD,
    LEGATTDB_CHAR_PROP_INDICATE,
    LEGATTDB_PERM_NONE ),

    /* Declare mandatory GAP service. Device Name and Appearance are mandatory
     * characteristics of GAP service                                        */
    PRIMARY_SERVICE_UUID16( HDLS_GENERIC_ACCESS,
    UUID_SERVCLASS_GAP_SERVER ),

    /* Declare mandatory GAP service characteristic: Dev Name */
    CHARACTERISTIC_UUID16(
        HDLC_GENERIC_ACCESS_DEVICE_NAME,
        HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,
        GATT_UUID_GAP_DEVICE_NAME,
        LEGATTDB_CHAR_PROP_READ,
        LEGATTDB_PERM_READABLE ),

    /* Declare mandatory GAP service characteristic: Appearance */
    CHARACTERISTIC_UUID16( HDLC_GENERIC_ACCESS_APPEARANCE,
    HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,
    GATT_UUID_GAP_ICON,
    LEGATTDB_CHAR_PROP_READ,
    LEGATTDB_PERM_READABLE ),

    /* Declare Device info service */
    PRIMARY_SERVICE_UUID16( HDLS_DEV_INFO,
    UUID_SERVCLASS_DEVICE_INFO ),

    /* Handle 0x4e: characteristic Manufacturer Name */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_MFR_NAME,
    HDLC_DEV_INFO_MFR_NAME_VALUE,
    GATT_UUID_MANU_NAME,
    LEGATTDB_CHAR_PROP_READ,
    LEGATTDB_PERM_READABLE ),

    /* Handle 0x50: characteristic Model Number */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_MODEL_NUM,
    HDLC_DEV_INFO_MODEL_NUM_VALUE,
    GATT_UUID_MODEL_NUMBER_STR,
    LEGATTDB_CHAR_PROP_READ,
    LEGATTDB_PERM_READABLE ),

    /* Handle 0x52: characteristic System ID */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_SYSTEM_ID,
    HDLC_DEV_INFO_SYSTEM_ID_VALUE,
    GATT_UUID_SYSTEM_ID,
    LEGATTDB_CHAR_PROP_READ,
    LEGATTDB_PERM_READABLE ),
};

/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/

static void hello_sensor_create_attribute_db( void );

/* Peripheral connection handlers */
static OSStatus connection_handler( aos_bt_peripheral_socket_t *socket );
static OSStatus disconnection_handler( aos_bt_peripheral_socket_t *socket );

static OSStatus hello_sensor_set_advertisement_data( void );
static OSStatus advertisement_complete_handle( void *arg );

/*
 * Entry point to the application.
 */

int application_start( void )
{
    OSStatus err = oNoErr;

#ifdef USE_AOSKit_EXT
    OLED_ShowString( OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_2, "BLE Advertising" );
#endif

    /* Initialize AOS bluetooth Framework */
    err = aos_bt_init( AOS_BT_HCI_MODE, "MK3239 Advert", 0, 1 );
    require_noerr_string( err, exit, "Error initialising AOS Bluetooth Framework" );

    /* Initialize AOS bluetooth peripheral */
    aos_bt_peripheral_init( &peripheral_socket, &security_settings, connection_handler,
                            disconnection_handler, NULL );

    /* Build BT Stack layer GATT database */
    aos_bt_gatt_db_init( hello_sensor_gatt_database, sizeof(hello_sensor_gatt_database) );

    /* Build BT Application Layer GATT database */
    hello_sensor_create_attribute_db( );

    /* Set the advertising parameters and make the device discoverable */
    hello_sensor_set_advertisement_data( );

    /* Start advertising */
    hello_sensor_start_advertisements( );

#ifdef AOS_CLI_ENABLE
    cli_register_commands( hello_sensor_send_message_cmd, USER_CLI_COMMAND_SIZE );
#endif

exit:
    aos_rtos_delete_thread( NULL );
    return err;
}

/***********************************************************************************************************
 * Handler or Callback function
 */

/* This function is invoked when advertisements changed. */
static OSStatus advertisement_complete_handle( void *arg )
{
    aos_bt_peripheral_socket_status_t status;

    UNUSED_PARAMETER( arg );

    printf( "Advertisements Stopped successfully!" );
    printf( "" );

    aos_bt_peripheral_get_socket_status( &peripheral_socket, &status );
    if ( status == PERIPHERAL_SOCKET_DISCONNECTED ) {
        hello_sensor_start_advertisements( );
    }
    return oNoErr;
}

/* Peripheral connection handlers */
static OSStatus connection_handler( aos_bt_peripheral_socket_t *socket )
{
    hello_sensor_stop_advertisements( );

    printf( "Connection established! id: 0x%x", socket->connection_handle );
    printf( "" );

#ifdef USE_AOSKit_EXT
    OLED_ShowString( OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, "     STOP!!     " );
    OLED_ShowString( OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "   Connected    " );
#endif

    /* Add device to whitelist */
    aos_bt_peripheral_update_advertisements_white_list( AOS_TRUE, socket->remote_device.address);

    return oNoErr;
}

static OSStatus disconnection_handler( aos_bt_peripheral_socket_t *socket )
{
    printf( "Connection disconnected!\r\n" );
    printf( "" );

    /* Start advertising */
    hello_sensor_start_advertisements( );

    return oNoErr;
}

/**************************************************************************************************************
 * Command line handle
 */

/* This function will be triggered by command "advert" on command line */
static void hello_sensor_change_advertisements( char *pcWriteBuffer, int xWriteBufferLen, int argc,
                                                char **argv )
{
    aos_bt_smart_advertising_type_t type = BT_SMART_UNDIRECTED_ADVERTISING; //default
    aos_bool_t duty = AOS_TRUE; //default

    require( argc >= 2, err );

    /* Change current advertisement type and state. */
    if ( (strcmp( argv[1], "stop" ) == 0) ) {
        /* Stop advertisement */
        hello_sensor_stop_advertisements( );

    } else if ( strcmp( argv[1], "start" ) == 0 ) {

        /* Check the second option parameter -- advertisement type */
        if ( argc >= 3 ) {
            require( strlen( argv[2] ) == 1, err );
            type = argv[2][0] - '0';
            require( type <= BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING, err );
        }

        /* Check the third option parameter -- advertisement duty */
        if ( argc >= 4 ) {
            require( strlen( argv[3] ) == 1, err );
            duty = argv[3][0] - '0';
            require( duty == AOS_TRUE || duty == AOS_FALSE, err );
        }

        switch ( type ) {
            case BT_SMART_UNDIRECTED_ADVERTISING:
                advertising_settings.high_duty_interval = AOS_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL;
                advertising_settings.low_duty_interval  = AOS_BT_CFG_DEFAULT_LOW_DUTY_ADV_MAX_INTERVAL;
                break;
            case BT_SMART_DISCOVERABLE_ADVERTISING:
            case BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING:
                advertising_settings.high_duty_interval = AOS_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MIN_INTERVAL;
                advertising_settings.low_duty_interval  = AOS_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MAX_INTERVAL;
                break;
            case BT_SMART_DIRECTED_ADVERTISING:
                advertising_settings.high_duty_interval = AOS_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MIN_INTERVAL;
                advertising_settings.low_duty_interval  = AOS_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MIN_INTERVAL;
                break;
            default:
                break;
        }

        /* Start Advertisement */
        advertising_settings.use_high_duty = duty;
        advertising_settings.type = type;

        hello_sensor_start_advertisements( );
    } else {
err:
        printf( "CLI: Unknown command or Unknown arguments" );
        printf( "" );
    }
}

/* This function will be triggered by command "policy" on command line */
static void hello_sensor_change_filter_policy( char *pcWriteBuffer, int xWriteBufferLen, int argc,
                                               char **argv )
{
    aos_bt_peripheral_adv_filter_policy_t policy;

    require( argc == 2, err );

    switch ( argv[1][0] ) {
        case '0': // PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ
            printf( "PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ" );
            policy = PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ;
            break;
        case '1': // PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ
            printf(  "PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ" );
            policy = PERIPHERAL_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ;
            break;
        case '2': // PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ
            printf( "PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ" );
            policy = PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ;
            break;
        case '3': // PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ
            printf( "PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ" );
            policy = PERIPHERAL_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ;
            break;
        default:
err:
            printf( "hello_sensor_change_filter_policy: Unknown command or Unknown arguments" );
            return;
    }

    if ( aos_bt_peripheral_set_advertisements_filter_policy( policy ) != oNoErr ) {
        printf( "update advertisement filter policy unsuccessfully!!" );
    } else {
        printf( "update advertisement filter policy successfully!!" );
    }
}

/* This function will be triggered by command "wlsize" on command line */
static void hello_sensor_get_whitelist_size( char *pcWriteBuffer, int xWriteBufferLen, int argc,
                                             char **argv )
{
    uint8_t size = 0;

    require( argc == 2, err );

    switch ( argv[1][0] ) {
        case '1':
            aos_bt_peripheral_get_advertisements_white_list_size( &size );
            snprintf( pcWriteBuffer, xWriteBufferLen, "size: %d", size );
            break;

        case '2':
            aos_bt_get_whitelist_capability( &size );
            snprintf( pcWriteBuffer, xWriteBufferLen, "capability: %d", size );
            break;

        default:
err:
            printf( "hello_sensor_get_whitelist_size: Unknown command or Unknown arguments" );
            return;
    }
}

/*
 * Local function
 */

/* Set advertisement data and scanning response data */
static OSStatus hello_sensor_set_advertisement_data( void )
{
    OSStatus err = oNoErr;

    uint16_t uuid[1] = { UUID_SERVCLASS_DEVICE_INFO };
    aos_bt_ble_advert_data_t adv_data;

    aos_bt_ble_service_t advert_services_16 = {
        .num_service = 1,
        .list_cmpl  = true,
        .p_uuid     = uuid,
    };

    aos_bt_ble_manu_t advert_manufacture = {
        .len    = strlen( "MXCHIP" ),
        .p_val  = (uint8_t *) "MXCHIP",
    };

    adv_data.flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    adv_data.p_services = &advert_services_16;
    adv_data.p_manu = &advert_manufacture;

    aos_bt_ble_set_advertisement_data(
        BTM_BLE_ADVERT_BIT_DEV_NAME | BTM_BLE_ADVERT_BIT_SERVICE | BTM_BLE_ADVERT_BIT_FLAGS,
        &adv_data );
    aos_bt_ble_set_scan_response_data( BTM_BLE_ADVERT_BIT_MANUFACTURER, &adv_data );

    return err;
}

/* Display Advertisement Arguments */
static void hello_sensor_start_advertisements( void )
{
#ifdef USE_AOSKit_EXT
    char display_str[20];
#endif

    aos_bt_dev_status_t status;
    aos_bt_peripheral_socket_status_t socket_status = PERIPHERAL_SOCKET_CONNECTED;

    /* Check current connection state. */
    aos_bt_peripheral_get_socket_status( &peripheral_socket, &socket_status );
    if ( socket_status != PERIPHERAL_SOCKET_DISCONNECTED ) {
        printf( "Advertisements Started unsuccessfully! Existed connection" );
        printf( "" );
        return;
    }

    /* Start advertisements */
    status = aos_bt_peripheral_start_advertisements( &advertising_settings,
                                                     advertisement_complete_handle );

    /* AOSKit LCD Display */
#ifdef USE_AOSKit_EXT
    sprintf( display_str, "Type: %s", pBT_ShortStr( advertising_settings.type ) );
    OLED_ShowString( OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, display_str );
    sprintf( display_str, "Duty: %s  ", advertising_settings.use_high_duty == AOS_TRUE ? "High" : "Low" );
    OLED_ShowString( OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, display_str );
#endif

    /* LOG */
    printf("");
    if ( status == AOS_BT_SUCCESS ) {
        printf( "Advertisements Started successfully! Arguments: " );
    } else if ( status == AOS_BT_ILLEGAL_VALUE ) {
        printf( "Advertisements Started unsuccessfully! Illegal value: " );
    } else {
        printf( "Advertisements Started unsuccessfully! Unknown error: " );
    }
    printf( "" );
    printf( "\t\t type              : %s", pBT_AdvertStr( advertising_settings.type ) );
    printf( "\t\t duty              : %s",
            advertising_settings.use_high_duty == AOS_TRUE ? "High duty" : "Low duty" );
    printf( "\t\t high_duty_interval: %-4d (slots, 0.625ms)",
            advertising_settings.high_duty_interval );
    printf( "\t\t high_duty_duration: %-4d (seconds)",
            advertising_settings.high_duty_duration );
    printf( "\t\t low_duty_interval : %-4d (slots, 0.625ms)",
            advertising_settings.low_duty_interval );
    printf( "\t\t low_duty_duration : %-4d (seconds)",
            advertising_settings.low_duty_duration );
    printf( "" );
}

/* Stop Advertisement Arguments */
static void hello_sensor_stop_advertisements( void )
{
    aos_bt_dev_status_t status;

    /* AOSKit LCD Display */
#ifdef USE_AOSKit_EXT
    OLED_ShowString( OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, "     STOP!!     " );
    OLED_ShowString( OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_4, "                " );
#endif

    status = aos_bt_peripheral_stop_advertisements( );
    if ( status == AOS_BT_SUCCESS ) {
        printf( "Advertisements Stopped successfully!" );
    } else {
        printf( "Advertisements Stopped unsuccessfully!" );
    }
    printf( "" );
}

/* Add attribute value for some characteristic attribute */
static void hello_sensor_create_attribute_db( void )
{
    extern aos_bt_cfg_settings_t aos_bt_cfg_settings;

    /*
     * Create BLE GATT external value database
     */

    // Primary service 'Generic Attribute'
    aos_bt_peripheral_ext_attribute_add( HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,
                                         0,
                                         NULL, NULL );

    // Primary service 'Generic Access'
    aos_bt_peripheral_ext_attribute_add( HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,
                                         strlen( (char *) aos_bt_cfg_settings.device_name ),
                                         aos_bt_cfg_settings.device_name,
                                         NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,
                                         sizeof(hello_sensor_appearance_name),
                                         hello_sensor_appearance_name,
                                         NULL );

    // Primary service 'Device info'
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_MFR_NAME_VALUE,
                                         strlen( (char *) MANUFACTURER ),
                                         (uint8_t *) MANUFACTURER,
                                         NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_MODEL_NUM_VALUE,
                                         strlen( (char *) MODEL ),
                                         (uint8_t *) MODEL,
                                         NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_SYSTEM_ID_VALUE,
                                         sizeof(hello_sensor_char_system_id_value),
                                         hello_sensor_char_system_id_value,
                                         NULL );
}

#ifdef USE_AOSKit_EXT

/* Used in OLED Display */
static const char *pBT_ShortStr(aos_bt_smart_advertising_type_t type)
{
    switch ( type ) {
        case BT_SMART_UNDIRECTED_ADVERTISING:
            return "UNDIRECT";

        case BT_SMART_DISCOVERABLE_ADVERTISING:
            return "DISCOVER";

        case BT_SMART_DIRECTED_ADVERTISING:
            return "DIRECTED";

        case BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING:
            return "NON-CONN";

        default:
            return "Unknown ";
    }
}

#endif

/* Convert advertising type to friendly information. */
static const char *pBT_AdvertStr( aos_bt_smart_advertising_type_t type )
{
    switch ( type ) {
        case BT_SMART_UNDIRECTED_ADVERTISING:
            return "Generic undirected advertising";

        case BT_SMART_DISCOVERABLE_ADVERTISING:
            return "Discoverable advertising";

        case BT_SMART_DIRECTED_ADVERTISING:
            return "Directed advertising";

        case BT_SMART_NON_CONNECTABLE_UNDIRECTED_ADVERTISING:
            return "Non-connectable Undirected advertising";

        default:
            return "Unknown advertising type";
    }
}

