/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <hal/soc/uart.h>
#include "ble_hello_sensor.h"
#include "sdpdefs.h"
#include "smartbt_cfg.h"
#include "smartbt.h"
#include "smartbt_peripheral.h"

#define DEFAULT_NAME "BleTestHelloSensor"
#define MANUFACTURER "BleTestHelloSensorManufacturer"
#define MODEL "BleTestHelloSensorModle"
#define UART_FOR_APP 1

/******************************************************************************
 *                                Constants
******************************************************************************/

/******************************************************************************
 *                           Function Prototypes
 ******************************************************************************/

static void hello_sensor_send_notification( CLI_ARGS );
static void hello_sensor_send_indication( CLI_ARGS );

/******************************************************************************
 *                                Structures
 ******************************************************************************/

typedef struct {
uint8_t                   flag_stay_connected;    /* stay connected or disconnect after all messages are sent */
uint8_t                   battery_level;          /* dummy battery level */
uint8_t                   led_color_idx;          /* RGB led color index */
} hello_sensor_state_t;

typedef struct {
aos_bt_device_address_t  bdaddr;
uint16_t                  service_changed;                        /* BD address of the bonded host */
uint16_t
characteristic_client_configuration;    /* Current value of the client configuration descriptor */
} host_info_t;

/* Peripheral auto advertising settings */
static aos_bt_smart_advertising_settings_t advertising_settings = {
.type                     =  BT_SMART_UNDIRECTED_ADVERTISING,                 /**< Advertising type                                               */
.use_high_duty            =  AOS_TRUE,                                       /**< Using high duty to start advertising                           */
.high_duty_interval       =  AOS_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL,  /**< High duty advertising interval                                 */
.high_duty_duration       =  5,                                               /**< High duty advertising duration in seconds (0 for infinite)     */
.low_duty_interval        =  AOS_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL,   /**< Low duty advertising interval                                  */
.low_duty_duration        =  60,                                              /**< Low duty advertising duration in seconds (0 for infinite)      */
};

/* Peripheral security settings */
static const aos_bt_smart_security_settings_t security_settings = {
    .timeout_second              = 15,
    .io_capabilities             = BT_SMART_IO_NO_INPUT_NO_OUTPUT,
    .authentication_requirements = BT_SMART_AUTH_REQ_BONDING,
    .oob_authentication          = BT_SMART_OOB_AUTH_NONE,
    .max_encryption_key_size     = 16,
    .master_key_distribution     = BT_SMART_DISTRIBUTE_ALL_KEYS,
    .slave_key_distribution      = BT_SMART_DISTRIBUTE_ALL_KEYS,
};

struct cli_command  hello_sensor_send_message_cmd[] = {
    { "notify", "Send Notification", hello_sensor_send_notification },
    { "indicate", "Send Indication", hello_sensor_send_indication },
};


/******************************************************************************
 *                                Variables Definitions
 ******************************************************************************/
/* Initialized attribute value */
static uint8_t hello_sensor_appearance_name[2]     = { BIT16_TO_8(APPEARANCE_GENERIC_TAG) };
static uint8_t hello_sensor_char_system_id_value[] = { 0xbb, 0xb8, 0xa1, 0x80, 0x5f, 0x9f, 0x91, 0x71 };
static char    hello_sensor_char_disable_value[]   = { 'D', 'I', 'S', 'A', 'B', 'L', 'E', 'D' };
static char    hello_sensor_char_notify_value[]    = { 'H', 'E', 'L', 'L', 'O', '0' };
static char    hello_sensor_char_indicate_value[]  = { 'I', 'N', 'D', 'I', 'C', 'A', 'T', 'E', '0' };

static hello_sensor_state_t     hello_sensor_state;
static host_info_t              hello_sensor_hostinfo;

/* Attributes used to send notify or indicate to le client */
static aos_bt_ext_attribute_value_t *hello_notify_indicate_attribute = NULL;
static aos_bt_ext_attribute_value_t *spp_out_attribute = NULL;

/* AOS BT smart peripheral connection controller */
static aos_bt_peripheral_socket_t peripheral_socket;


/******************************************************************************
 *                                GATT DATABASE
 ******************************************************************************/
/*
 * This is the GATT database for the Hello Sensor application.  It defines
 * services, characteristics and descriptors supported by the sensor.  Each
 * attribute in the database has a handle, (characteristic has two, one for
 * characteristic itself, another for the value).  The handles are used by
 * the peer to access attributes, and can be used locally by application for
 * example to retrieve data written by the peer.  Definition of characteristics
 * and descriptors has GATT Properties (read, write, notify...) but also has
 * permissions which identify if and how peer is allowed to read or write
 * into it.  All handles do not need to be sequential, but need to be in
 * ascending order.
 */
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
    PRIMARY_SERVICE_UUID16( HDLS_GENERIC_ACCESS, UUID_SERVCLASS_GAP_SERVER ),

    /* Declare mandatory GAP service characteristic: Dev Name */
    CHARACTERISTIC_UUID16( HDLC_GENERIC_ACCESS_DEVICE_NAME, HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,
    GATT_UUID_GAP_DEVICE_NAME,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Declare mandatory GAP service characteristic: Appearance */
    CHARACTERISTIC_UUID16( HDLC_GENERIC_ACCESS_APPEARANCE, HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,
    GATT_UUID_GAP_ICON,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Declare proprietary Hello Service with 128 byte UUID */
    PRIMARY_SERVICE_UUID128( HDLS_HELLO_SENSOR, UUID_HELLO_SERVICE ),

    /* Declare characteristic used to notify/indicate change */
    CHARACTERISTIC_UUID128( HDLC_HELLO_SENSOR_NOTIFY, HDLC_HELLO_SENSOR_NOTIFY_VALUE,
    UUID_HELLO_CHARACTERISTIC_NOTIFY,
    LEGATTDB_CHAR_PROP_READ | LEGATTDB_CHAR_PROP_NOTIFY | LEGATTDB_CHAR_PROP_INDICATE,
    LEGATTDB_PERM_READABLE ),

    /* Declare client characteristic configuration descriptor
     * Value of the descriptor can be modified by the client
     * Value modified shall be retained during connection and across connection
     * for bonded devices.  Setting value to 1 tells this application to send notification
     * when value of the characteristic changes.  Value 2 is to allow indications. */
    CHAR_DESCRIPTOR_UUID16_WRITABLE( HDLC_HELLO_SENSOR_NOTIFY_CFG_DESC, GATT_UUID_CHAR_CLIENT_CONFIG,
    LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_REQ ),

    /* Declare characteristic Hello Configuration */
    CHARACTERISTIC_UUID128_WRITABLE( HDLC_HELLO_SENSOR_COLOR, HDLC_HELLO_SENSOR_COLOR_VALUE,
    UUID_HELLO_CHARACTERISTIC_COLOR,
    LEGATTDB_CHAR_PROP_READ | LEGATTDB_CHAR_PROP_WRITE,
    LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ ),

    // /* Declare SPP Service with 128 byte UUID */
    PRIMARY_SERVICE_UUID128( HDLS_SPP, UUID_SPP_SERVICE ),

    /* Declare characteristic used to write spp data to server */
    CHARACTERISTIC_UUID128_WRITABLE( HDLC_SPP_IN, HDLC_SPP_IN_VALUE,
    UUID_SPP_SERVICE_CHARACTERISTIC_IN,
    LEGATTDB_CHAR_PROP_WRITE, LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ ),

    CHAR_DESCRIPTOR_UUID16( HDLC_SPP_IN_DESCRIPTION, GATT_UUID_CHAR_DESCRIPTION,
    LEGATTDB_PERM_READABLE ),

    /* Declare characteristic used to send spp data to client */
    CHARACTERISTIC_UUID128( HDLC_SPP_OUT, HDLC_SPP_OUT_VALUE,
    UUID_SPP_SERVICE_CHARACTERISTIC_OUT,
    LEGATTDB_CHAR_PROP_INDICATE | LEGATTDB_CHAR_PROP_NOTIFY, LEGATTDB_PERM_NONE ),

    CHAR_DESCRIPTOR_UUID16( HDLC_SPP_OUT_DESCRIPTION, GATT_UUID_CHAR_DESCRIPTION,
    LEGATTDB_PERM_READABLE ),

    /* Declare Device info service */
    PRIMARY_SERVICE_UUID16( HDLS_DEV_INFO, UUID_SERVCLASS_DEVICE_INFO ),

    /* Handle 0x4e: characteristic Manufacturer Name */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_MFR_NAME, HDLC_DEV_INFO_MFR_NAME_VALUE,
    GATT_UUID_MANU_NAME,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Handle 0x50: characteristic Model Number */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_MODEL_NUM, HDLC_DEV_INFO_MODEL_NUM_VALUE,
    GATT_UUID_MODEL_NUMBER_STR,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Handle 0x52: characteristic System ID */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_SYSTEM_ID, HDLC_DEV_INFO_SYSTEM_ID_VALUE,
    GATT_UUID_SYSTEM_ID,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Declare Battery service */
    PRIMARY_SERVICE_UUID16( HDLS_BAT, UUID_SERVCLASS_BATTERY ),

    /* Handle 0x62: characteristic Battery Level, handle 0x63 characteristic value */
    CHARACTERISTIC_UUID16( HDLC_BAT_LEVEL, HDLC_BAT_LEVEL_VALUE,
    GATT_UUID_BATTERY_LEVEL,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),
};

static uart_dev_t uart_1;
/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/

static void hello_sensor_create_attribute_db(void);

/* Peripheral connection handlers */
static OSStatus connection_handler( aos_bt_peripheral_socket_t *socket );
static OSStatus disconnection_handler( aos_bt_peripheral_socket_t *socket );

static OSStatus hello_sensor_set_advertisement_data( void );
static OSStatus advertisement_complete_handle( void *arg );

/* Peripheral attribute operation handlers */
static aos_bt_gatt_status_t battery_level_callback     ( aos_bt_ext_attribute_value_t *attribute,
                                                         aos_bt_gatt_request_type_t op );
static aos_bt_gatt_status_t color_val_callback         ( aos_bt_ext_attribute_value_t *attribute,
                                                         aos_bt_gatt_request_type_t op );
static aos_bt_gatt_status_t notification_char_callback ( aos_bt_ext_attribute_value_t *attribute,
                                                         aos_bt_gatt_request_type_t op );
static aos_bt_gatt_status_t char_cfg_callback          ( aos_bt_ext_attribute_value_t *attribute,
                                                         aos_bt_gatt_request_type_t op );
static aos_bt_gatt_status_t spp_data_in_callback       ( aos_bt_ext_attribute_value_t *attribute,
                                                         aos_bt_gatt_request_type_t op );

/* Read UART data in SPP service */
static void uartRecv_thread(uint32_t arg);

/*
 *  Entry point to the application.
 */

int application_start( void )
{
    OSStatus err = oNoErr;

    /* Initialise AOS Bluetooth Framework */
    err = aos_bt_init( AOS_BT_HCI_MODE, DEFAULT_NAME, 0, 1 );  //Client + server connections
    require_noerr_string( err, exit, "Error initialising AOS Bluetooth Framework" );

    /* Initialise AOS Bluetooth Peripheral interface */
    aos_bt_peripheral_init( &peripheral_socket, &security_settings, connection_handler, disconnection_handler, NULL );

    /* Build BT stack layer GATT database (handle, uuid, permission, properity)*/
    aos_bt_gatt_db_init( hello_sensor_gatt_database, sizeof( hello_sensor_gatt_database ) );

    /* Build BT application layer GATT database ( extenal value, callback functions )*/
    hello_sensor_create_attribute_db();

    printf( "hello_sensor_application_init" );

    /* Set the advertising parameters and make the device discoverable */
    hello_sensor_set_advertisement_data();

    aos_bt_peripheral_start_advertisements( &advertising_settings, advertisement_complete_handle );

    /*
     * Set flag_stay_connected to remain connected after all messages are sent
     * Reset flag to 0, to disconnect
     */
    hello_sensor_state.flag_stay_connected = 1;
    hello_sensor_state.battery_level = 0;
    hello_sensor_state.led_color_idx = 0;

#ifdef USE_AOSKit_EXT
    hsb2rgb_led_open( (hello_sensor_state.led_color_idx * 60) % 360 , 100, 5 );
#endif

#if defined(AOS_CLI_ENABLE)
    if ( UART_FOR_APP == CLI_UART ) {
        printf( "BLE SPP demo disabled by limited UART peripherals" );
    } else {
        // SPP UART data receive thread
        err = aos_rtos_create_thread( NULL, AOS_APPLICATION_PRIORITY, "UART Recv", uartRecv_thread, 500, 0 );
        require_noerr_string( err, exit, "ERROR: Unable to start the uart recv thread." );
    }
#else
    err = aos_rtos_create_thread( NULL, AOS_APPLICATION_PRIORITY, "UART Recv", uartRecv_thread, 500, 0 );
    require_noerr_string( err, exit, "ERROR: Unable to start the uart recv thread." );
#endif

#if defined AOS_CLI_ENABLE
    cli_register_commands( hello_sensor_send_message_cmd, 2 );
#endif

exit:
    aos_rtos_delete_thread( NULL );
    return err;
}


OSStatus hello_sensor_set_advertisement_data( void )
{
    OSStatus err = oNoErr;

    uint16_t uuid[1] = { UUID_SERVCLASS_BATTERY };
    aos_bt_ble_service_t adver_services_16 = { .num_service = 1, .list_cmpl = false, .p_uuid = uuid };
    aos_bt_ble_128service_t adver_services_128 = { .list_cmpl = false, .uuid128 = { UUID_HELLO_SERVICE  }};

    aos_bt_ble_advert_data_t adv_data;

    adv_data.flag =  BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    adv_data.p_services = &adver_services_16;
    adv_data.p_services_128b = &adver_services_128;

    aos_bt_ble_set_advertisement_data(   BTM_BLE_ADVERT_BIT_DEV_NAME |
                                         BTM_BLE_ADVERT_BIT_SERVICE |
                                         BTM_BLE_ADVERT_BIT_FLAGS, &adv_data);

    aos_bt_ble_set_scan_response_data( BTM_BLE_ADVERT_BIT_SERVICE_128, &adv_data);

    return err;
}

/*
 * This function is invoked when advertisements changed.  If we are configured to stay connected,
 * disconnection was caused by the peer, start low advertisements, so that peer can connect
 * when it wakes up
 */

OSStatus advertisement_complete_handle( void *arg )
{
    UNUSED_PARAMETER(arg);
    OSStatus result = oNoErr;
    aos_bt_peripheral_socket_status_t status;

    aos_bt_peripheral_get_socket_status( &peripheral_socket, &status );
    if ( hello_sensor_state.flag_stay_connected && status == PERIPHERAL_SOCKET_DISCONNECTED ) {
        advertising_settings.use_high_duty = AOS_FALSE;
        result = aos_bt_peripheral_start_advertisements( &advertising_settings, advertisement_complete_handle );
        printf( "aos_bt_start_advertisements: %d", result );
    } else {
        printf( "ADV stop" );
    }
    return result;
}

void hello_sensor_create_attribute_db(void)
{
    extern aos_bt_cfg_settings_t   aos_bt_cfg_settings;

    /* Create BLE GATT value database */
    // ***** Primary service 'Generic Attribute'
    aos_bt_peripheral_ext_attribute_add( HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE, 0, NULL, NULL );

    // ***** Primary service 'Generic Access'
    aos_bt_peripheral_ext_attribute_add( HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,
                                         strlen((char *)aos_bt_cfg_settings.device_name), aos_bt_cfg_settings.device_name, NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_GENERIC_ACCESS_APPEARANCE_VALUE, sizeof(hello_sensor_appearance_name),
                                         hello_sensor_appearance_name, NULL );

    // ***** Primary service 'Device info'
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_MFR_NAME_VALUE, strlen((char *)MANUFACTURER),
                                         (uint8_t *)MANUFACTURER, NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_MODEL_NUM_VALUE, strlen((char *)MODEL), (uint8_t *)MODEL, NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_SYSTEM_ID_VALUE, sizeof(hello_sensor_char_system_id_value),
                                         hello_sensor_char_system_id_value, NULL );

    // ***** Primary service 'Battery'
    aos_bt_peripheral_ext_attribute_add( HDLC_BAT_LEVEL_VALUE, 0, NULL, battery_level_callback );

    // ***** Primary service 'Hello' (Vender specific)
    hello_notify_indicate_attribute = aos_bt_peripheral_ext_attribute_add( HDLC_HELLO_SENSOR_NOTIFY_VALUE, 0, NULL,
                                                                           notification_char_callback );
    aos_bt_peripheral_ext_attribute_add( HDLC_HELLO_SENSOR_NOTIFY_CFG_DESC, 2,
                                         (uint8_t *)&hello_sensor_hostinfo.characteristic_client_configuration, char_cfg_callback );

    aos_bt_peripheral_ext_attribute_add( HDLC_HELLO_SENSOR_COLOR_VALUE, 0, NULL, color_val_callback );

    // ***** Primary service 'SPP' (Vender specific)
    aos_bt_peripheral_ext_attribute_add( HDLC_SPP_IN_VALUE, 0, NULL, spp_data_in_callback );
    aos_bt_peripheral_ext_attribute_add( HDLC_SPP_IN_DESCRIPTION, strlen("SPP Data IN"), (uint8_t *)"SPP Data IN", NULL );

    spp_out_attribute = aos_bt_peripheral_ext_attribute_add( HDLC_SPP_OUT_VALUE, 0, NULL, NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_SPP_OUT_DESCRIPTION, strlen("SPP Data OUT"), (uint8_t *)"SPP Data OUT",
                                         NULL );

    aos_bt_peripheral_ext_attribute_find_by_handle( HDLC_HELLO_SENSOR_NOTIFY_VALUE, &hello_notify_indicate_attribute );
    aos_bt_peripheral_ext_attribute_find_by_handle( HDLC_SPP_OUT_VALUE, &spp_out_attribute );
}

/* TX Power report handler */
static void hello_sensor_tx_power_callback( aos_bt_tx_power_result_t *p_tx_power )
{
    if ( ( p_tx_power->status == AOS_BT_SUCCESS ) && ( p_tx_power->hci_status == HCI_SUCCESS ) ) {
        printf( "Local TX: %d", p_tx_power->tx_power );
#ifdef USE_AOSKit_EXT
        char display_str[20];
        sprintf( display_str, "Local TX: %d  ", p_tx_power->tx_power );
        OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_2, display_str);
#endif
    } else {
        printf( "Unable to read Local TX power. (btm_status=0x%x, hci_status=0x%x)", p_tx_power->status,
                p_tx_power->hci_status );
    }
}

/* RSSI report handler */
static void hello_sensor_rssi_callback( aos_bt_dev_rssi_result_t *p_rssi )
{
    if ( ( p_rssi->status == AOS_BT_SUCCESS ) && ( p_rssi->hci_status == HCI_SUCCESS ) ) {
        printf( "RSSI: %d", p_rssi->rssi );
#ifdef USE_AOSKit_EXT
        char display_str[20];
        sprintf( display_str, "RSSI: %d    ", p_rssi->rssi );
        OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_3, display_str);
#endif
    } else {
        printf( "Unable to read RSSI. (btm_status=0x%x, hci_status=0x%x)", p_rssi->status, p_rssi->hci_status );
    }
}


OSStatus connection_handler( aos_bt_peripheral_socket_t *socket )
{
    OSStatus result;

    printf( "hello_sensor_conn_up id:0x%4x:", socket->connection_handle );

    /* Stop advertising */
    result =  aos_bt_peripheral_stop_advertisements( );

    printf( "Stopping Advertisements%d", result);

    aos_bt_dev_read_tx_power( socket->remote_device.address, BT_TRANSPORT_LE,
                              (aos_bt_dev_cmpl_cback_t *) hello_sensor_tx_power_callback );

    hello_sensor_hostinfo.characteristic_client_configuration = GATT_CLIENT_CONFIG_NONE;
    hello_sensor_hostinfo.service_changed = 1;

    return oNoErr;
}

OSStatus disconnection_handler( aos_bt_peripheral_socket_t *socket )
{
    OSStatus result;

    printf( "hello_sensor_conn_down id:0x%4x:", socket->connection_handle );

    /*
     * If we are configured to stay connected, disconnection was
     * caused by the peer, start low advertisements, so that peer
     * can connect when it wakes up
     */
    if ( hello_sensor_state.flag_stay_connected ) {
        advertising_settings.use_high_duty = TRUE;
        result = aos_bt_peripheral_start_advertisements( &advertising_settings, advertisement_complete_handle );
        printf( "aos_bt_start_advertisements %d", result );
    }

    return oNoErr;
}

/*
 * Hello Sensor functions
 */

/*
 * This function will be triggered by command "notify" on command line
 */
void hello_sensor_send_notification( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    uint8_t *p_attr = (uint8_t *)hello_sensor_char_notify_value;
    aos_bt_peripheral_socket_status_t status;

    aos_bt_peripheral_get_socket_status( &peripheral_socket, &status);
    if ( status == PERIPHERAL_SOCKET_CONNECTED ) {
        if ( hello_sensor_hostinfo.characteristic_client_configuration & GATT_CLIENT_CONFIG_NOTIFICATION ) {
            /*incrementing the value field , to ensure that notification values are changing*/
            hello_sensor_char_notify_value[5]++;
            aos_bt_peripheral_ext_attribute_value_write(hello_notify_indicate_attribute, sizeof(hello_sensor_char_notify_value), 0,
                                                        p_attr );
            aos_bt_peripheral_gatt_notify_attribute_value( &peripheral_socket, hello_notify_indicate_attribute );
            snprintf( pcWriteBuffer, xWriteBufferLen, "Notify is sent" );
        } else {
            snprintf(pcWriteBuffer, xWriteBufferLen, "Client configuration not allow");
        }
    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Cannot send notification, hello sensor not connected");
    }
}

/*
 * This function will be triggered by command "inticate" on command line
 */
void hello_sensor_send_indication( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    uint8_t *p_attr = (uint8_t *)hello_sensor_char_indicate_value;
    aos_bt_peripheral_socket_status_t status;

    aos_bt_peripheral_get_socket_status( &peripheral_socket, &status);
    if ( status == PERIPHERAL_SOCKET_CONNECTED ) {
        if ( hello_sensor_hostinfo.characteristic_client_configuration & GATT_CLIENT_CONFIG_INDICATION ) {
            /*incrementing the value field , to ensure that indication values are changing*/
            hello_sensor_char_indicate_value[8]++;
            aos_bt_peripheral_ext_attribute_value_write(hello_notify_indicate_attribute, sizeof(hello_sensor_char_indicate_value),
                                                        0, p_attr );
            aos_bt_peripheral_gatt_indicate_attribute_value( &peripheral_socket, hello_notify_indicate_attribute );
            snprintf( pcWriteBuffer, xWriteBufferLen, "Indicate is sent" );
        } else {
            snprintf(pcWriteBuffer, xWriteBufferLen, "client configuration not allow");
        }
    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Cannot send indication, hello sensor not connected");
    }
}


aos_bt_gatt_status_t color_val_callback( aos_bt_ext_attribute_value_t *attribute, aos_bt_gatt_request_type_t op )
{
    if ( op == GATTS_REQ_TYPE_READ ) {

        printf( "hello_sensor_read_handler: led_color_idx: %d", hello_sensor_state.led_color_idx );
        aos_bt_peripheral_ext_attribute_value_write( attribute, 1, 0, &hello_sensor_state.led_color_idx );
        return AOS_BT_GATT_SUCCESS;
    } else if ( op == GATTS_REQ_TYPE_WRITE ) {
        if ( attribute->value_length != 1 ) {
            return AOS_BT_GATT_INVALID_ATTR_LEN;
        }
        hello_sensor_state.led_color_idx = attribute->p_value[0];
        printf( "hello_sensor_write_handler: led_color_idx: %d", hello_sensor_state.led_color_idx );
        aos_bt_dev_read_rssi( peripheral_socket.remote_device.address, BT_TRANSPORT_LE,
                              (aos_bt_dev_cmpl_cback_t *) hello_sensor_rssi_callback );

#ifdef USE_AOSKit_EXT
        hsb2rgb_led_open( (hello_sensor_state.led_color_idx * 60) % 360 , 100, 5);
#endif

        return AOS_BT_GATT_SUCCESS;
    } else {
        return AOS_BT_GATT_ERROR;
    }
}

static aos_bt_gatt_status_t notification_char_callback( aos_bt_ext_attribute_value_t *attribute,
                                                        aos_bt_gatt_request_type_t op )
{
    uint8_t *p_attr;

    if ( op == GATTS_REQ_TYPE_READ ) {
        /* If client has not registered for indication or notification, no action */
        if ( hello_sensor_hostinfo.characteristic_client_configuration == 0 ) {
            p_attr = (uint8_t *)hello_sensor_char_disable_value;
            aos_bt_peripheral_ext_attribute_value_write(attribute, sizeof(hello_sensor_char_disable_value), 0, p_attr );
        } else if ( hello_sensor_hostinfo.characteristic_client_configuration & GATT_CLIENT_CONFIG_NOTIFICATION ) {
            p_attr = (uint8_t *)hello_sensor_char_notify_value;
            aos_bt_peripheral_ext_attribute_value_write(attribute, sizeof(hello_sensor_char_notify_value), 0, p_attr );
        } else {
            p_attr = (uint8_t *)hello_sensor_char_indicate_value;
            aos_bt_peripheral_ext_attribute_value_write(attribute, sizeof(hello_sensor_char_indicate_value), 0, p_attr );
        }
        return AOS_BT_GATT_SUCCESS;
    } else {
        return AOS_BT_GATT_ERROR;
    }
}

aos_bt_gatt_status_t char_cfg_callback( aos_bt_ext_attribute_value_t *attribute, aos_bt_gatt_request_type_t op )
{

    if ( op == GATTS_REQ_TYPE_READ ) {
        return AOS_BT_GATT_SUCCESS;
    } else if ( op == GATTS_REQ_TYPE_WRITE ) {
        if ( attribute->value_length != 2 ) {
            return AOS_BT_GATT_INVALID_ATTR_LEN;
        }
        hello_sensor_hostinfo.characteristic_client_configuration = attribute->p_value[0] | ( attribute->p_value[1] << 8 );
        return AOS_BT_GATT_SUCCESS;
    } else {
        return AOS_BT_GATT_ERROR;
    }
}

/*
 * UART SPP Functions
 */
static aos_bt_gatt_status_t spp_data_in_callback( aos_bt_ext_attribute_value_t *attribute,
                                                  aos_bt_gatt_request_type_t op )
{
    if ( op == GATTS_REQ_TYPE_WRITE ) {
        hal_uart_send(&uart_1, attribute->p_value, attribute->value_length, 1000);
        return AOS_BT_GATT_SUCCESS;
    } else {
        return AOS_BT_GATT_ERROR;
    }
}


size_t _uart_get_one_packet(uint8_t *inBuf, int inBufLen)
{
    int datalen;

    while (1) {
        if (hal_uart_recv(&uart_1, inBuf, inBufLen, &datalen, SPP_UART_RECV_TIMEOUT) == 0) {
            return datalen;
        }
    }
}

void uartRecv_thread(uint32_t arg)
{
    UNUSED_PARAMETER( arg );
    int recvlen;
    uint8_t *spp_data_recv_buffer;

    spp_data_recv_buffer = malloc(SPP_UART_ONE_PACKAGE_LENGTH);
    require(spp_data_recv_buffer, exit);

    uart_1.port                = UART_FOR_APP;
    uart_1.config.baud_rate    = 115200;
    uart_1.config.data_width   = DATA_WIDTH_8BIT;
    uart_1.config.parity       = NO_PARITY;
    uart_1.config.stop_bits    = STOP_BITS_1;
    uart_1.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_1);
    while (1) {
        recvlen = _uart_get_one_packet(spp_data_recv_buffer, SPP_UART_ONE_PACKAGE_LENGTH);
        if (recvlen <= 0) {
            continue;
        }

        aos_bt_peripheral_ext_attribute_value_write(spp_out_attribute, recvlen, 0, spp_data_recv_buffer );
        aos_bt_peripheral_gatt_indicate_attribute_value( &peripheral_socket, spp_out_attribute );
    }

exit:
    if (spp_data_recv_buffer) {
        free(spp_data_recv_buffer);
    }
    aos_rtos_delete_thread(NULL);
}

/*
 * Demo battery functions
 */
aos_bt_gatt_status_t battery_level_callback( aos_bt_ext_attribute_value_t *attribute, aos_bt_gatt_request_type_t op )
{
    if ( op == GATTS_REQ_TYPE_READ ) {
        hello_sensor_state.battery_level = (hello_sensor_state.battery_level + 1) % 100;
        printf( "Read battery level %d", hello_sensor_state.battery_level );
        aos_bt_peripheral_ext_attribute_value_write( attribute, 1, 0, &hello_sensor_state.battery_level );
        return AOS_BT_GATT_SUCCESS;
    } else {
        return AOS_BT_GATT_ERROR;
    }
}


