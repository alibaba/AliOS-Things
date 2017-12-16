/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sdpdefs.h"
#include "smartbt_cfg.h"
#include "smartbt_peripheral.h"

#define MANUFACTURER "BleTestManufacturer"
#define MODEL "BleTestModel"

/******************************************************************************
 *                                Constants
******************************************************************************/

/******************************************************************************
 *                                Enumerations
******************************************************************************/

enum {
    HDLS_DEV_INFO = 0x50,
    HDLC_DEV_INFO_MFR_NAME,
    HDLC_DEV_INFO_MFR_NAME_VALUE,

    HDLC_DEV_INFO_MODEL_NUM,
    HDLC_DEV_INFO_MODEL_NUM_VALUE,

    HDLC_DEV_INFO_SYSTEM_ID,
    HDLC_DEV_INFO_SYSTEM_ID_VALUE,
};

/******************************************************************************
 *                           Function Prototypes
 ******************************************************************************/

/******************************************************************************
 *                                Structures
 ******************************************************************************/

/* SmartBridge auto scan settings */
static aos_bt_smart_advertising_settings_t advertising_settings = {
    .type                     =  BT_SMART_UNDIRECTED_ADVERTISING,                   /**< Advertising type                                               */
    .use_high_duty            =  AOS_TRUE,                                         /**< Using high duty to start advertising                           */
    .high_duty_interval       =  AOS_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL,    /**< High duty advertising interval                                 */
    .high_duty_duration       =  30,                                                /**< High duty advertising duration in seconds (0 for infinite)     */
    .low_duty_interval        =  AOS_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL,     /**< Low duty advertising interval                                  */
    .low_duty_duration        =  0,                                                 /**< Low duty advertising duration in seconds (0 for infinite)      */
};

/******************************************************************************
 *                                Variables Definitions
 ******************************************************************************/
extern aos_bt_cfg_settings_t   aos_bt_cfg_settings;
extern aos_bt_smart_security_settings_t security_settings;

static uint8_t hello_sensor_char_system_id_value[] = { 0xbb, 0xb8, 0xa1, 0x80, 0x5f, 0x9f, 0x91, 0x71 };

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
    /* Declare Device info service */
    PRIMARY_SERVICE_UUID16( HDLS_DEV_INFO, UUID_SERVCLASS_DEVICE_INFO ),

    /* Handle 0x4e: characteristic Manufacturer Name */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_MFR_NAME, HDLC_DEV_INFO_MFR_NAME_VALUE,
    GATT_UUID_MANU_NAME, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Handle 0x50: characteristic Model Number */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_MODEL_NUM, HDLC_DEV_INFO_MODEL_NUM_VALUE,
    GATT_UUID_MODEL_NUMBER_STR, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Handle 0x52: characteristic System ID */
    CHARACTERISTIC_UUID16( HDLC_DEV_INFO_SYSTEM_ID, HDLC_DEV_INFO_SYSTEM_ID_VALUE,
    GATT_UUID_SYSTEM_ID, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),
};

/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/

static void hello_peripheral_create_attribute_db(void);
static OSStatus connection_handler( aos_bt_peripheral_socket_t *socket );
static OSStatus disconnection_handler( aos_bt_peripheral_socket_t *socket );
static OSStatus hello_peripheral_set_advertisement_data( void );

/*
 *  Entry point to the ble hello peripheral demo.
 */

OSStatus ble_hello_peripheral_init( void )
{
    OSStatus err = oNoErr;

    err = aos_bt_peripheral_init( &peripheral_socket, &security_settings, connection_handler, disconnection_handler, NULL );
    require_noerr(err, exit);

    /* Build BT stack layer GATT database (handle, uuid, permission, properity)*/
    aos_bt_gatt_db_init( hello_sensor_gatt_database, sizeof( hello_sensor_gatt_database ) );
    require_noerr(err, exit);

    /* Build BT application layer GATT database ( extenal value, callback functions )*/
    hello_peripheral_create_attribute_db();

    /* Set the advertising parameters and make the device discoverable */
    hello_peripheral_set_advertisement_data();

    aos_bt_peripheral_start_advertisements( &advertising_settings, NULL );

    printf( "Hello Peripheral started." );

exit:
    return err;
}

OSStatus hello_peripheral_set_advertisement_data( void )
{
    OSStatus err = oNoErr;

    aos_bt_ble_advert_data_t adv_data;

    adv_data.flag =  BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    aos_bt_ble_set_advertisement_data( BTM_BLE_ADVERT_BIT_DEV_NAME |
                                       BTM_BLE_ADVERT_BIT_FLAGS, &adv_data );

    return err;
}


void hello_peripheral_create_attribute_db(void)
{
    // ***** Primary service 'Device info'
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_MFR_NAME_VALUE, strlen((char *)MANUFACTURER),
                                         (uint8_t *)MANUFACTURER, NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_MODEL_NUM_VALUE, strlen((char *)MODEL), (uint8_t *)MODEL, NULL );
    aos_bt_peripheral_ext_attribute_add( HDLC_DEV_INFO_SYSTEM_ID_VALUE, sizeof(hello_sensor_char_system_id_value),
                                         hello_sensor_char_system_id_value, NULL );
}

OSStatus connection_handler( aos_bt_peripheral_socket_t *socket )
{
    /* Stop advertising */
    printf( "Peripheral Connected." );
    return aos_bt_peripheral_stop_advertisements( );
}

OSStatus disconnection_handler( aos_bt_peripheral_socket_t *socket )
{
    /*
     * If we are configured to stay connected, disconnection was
     * caused by the peer, start high advertisements, so that peer
     * can connect when it wakes up
     */
    printf( "Peripheral Disconected." );
    return aos_bt_peripheral_start_advertisements( &advertising_settings, NULL );
}
