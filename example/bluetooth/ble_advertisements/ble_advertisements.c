/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <ble_app_framework.h>
#include <aos/aos.h>

static int connection_handler()
{

}

static int disconnection_handler()
{

}

typedef enum {
    HDLS_GENERIC_ATTRIBUTE = 0x1,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,

    HDLS_GENERIC_ACCESS = 0x14,
    HDLC_GENERIC_ACCESS_DEVICE_NAME,
    HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,

    HDLC_GENERIC_ACCESS_APPEARANCE,
    HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,

    HDLS_DEV_INFO = 0x50,
    HDLC_DEV_INFO_MFR_NAME,
    HDLC_DEV_INFO_MFR_NAME_VALUE,

    HDLC_DEV_INFO_MODEL_NUM,
    HDLC_DEV_INFO_MODEL_NUM_VALUE,

    HDLC_DEV_INFO_SYSTEM_ID,
    HDLC_DEV_INFO_SYSTEM_ID_VALUE,
} adv_db_tags;

static const uint8_t adv_gatt_db[] = {
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

static int adv_complete_cb(void *arg)
{

}

#define DEVICE_MANUFACURE_NAME "BleAdvertisementsSampleManufacture"
#define DEVICE_MODEL_NUM "BleAdvertismentsSampleDeviceModel"
static uint8_t sys_id[] = {0x12, 0x34};

static void app_delayed_action(void *arg)
{
    peripheral_hdl_t hdl;

    peripheral_init_t p = {CONFIG_BT_DEVICE_NAME, 0, 1};

    hdl = ble_peripheral_init(&p, connection_handler, disconnection_handler,
                              adv_gatt_db, sizeof(adv_gatt_db));

    ble_adv_start(adv_complete_cb, DEVICE_MANUFACURE_NAME, hdl);

    ble_attr_add(HDLC_DEV_INFO_MFR_NAME_VALUE,
                 sizeof(DEVICE_MANUFACURE_NAME) - 1, DEVICE_MANUFACURE_NAME);
    ble_attr_add(HDLC_DEV_INFO_MODEL_NUM_VALUE,
                 sizeof(DEVICE_MODEL_NUM) - 1, DEVICE_MODEL_NUM);
    ble_attr_add(HDLC_DEV_INFO_SYSTEM_ID_VALUE, sizeof(sys_id),
                 (const uint8_t *)&sys_id);
}

int application_start( void )
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
