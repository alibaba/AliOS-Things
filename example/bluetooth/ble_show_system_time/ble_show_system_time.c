/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <ble_app_framework.h>

static int connection_handler()
{
    LOG("Hello %s", __func__);
}

static int disconnection_handler()
{
    LOG("Hello %s", __func__);
}

/* UUID value of the TIME Service */
#define UUID_TIME_SERVICE 0x5E, 0x67, 0x21, 0x8A, 0x3f, 0x4b, 0x4D, 0x32, 0x91, 0x36, 0x38, 0xE3, 0xD8, 0xED, 0x63, 0x71
/* UUID value of the TIME Characteristic, Data OUT */
#define UUID_TIME_SERVICE_CHARACTERISTIC_OUT 0x32, 0x15, 0x1a, 0x5e, 0x82, 0x2e, 0x12, 0x2a, 0x91, 0x43, 0x27, 0x52, 0xba, 0x1d, 0xf3, 0x30

typedef enum {
    HDLS_GENERIC_ATTRIBUTE = 0x1,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,

    HDLS_GENERIC_ACCESS = 0x14,
    HDLC_GENERIC_ACCESS_DEVICE_NAME,
    HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,

    HDLC_GENERIC_ACCESS_APPEARANCE,
    HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,

    HDLS_TIME = 0x40,
    HDLC_TIME_OUT,
    HDLC_TIME_OUT_VALUE,
    HDLC_TIME_OUT_CCC,
    HDLC_TIME_OUT_DESCRIPTION,

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
    PRIMARY_SERVICE_UUID16( HDLS_GENERIC_ACCESS, UUID_SERVCLASS_GAP_SERVER ),

    /* Declare mandatory GAP service characteristic: Dev Name */
    CHARACTERISTIC_UUID16( HDLC_GENERIC_ACCESS_DEVICE_NAME, HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,
    GATT_UUID_GAP_DEVICE_NAME,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    /* Declare mandatory GAP service characteristic: Appearance */
    CHARACTERISTIC_UUID16( HDLC_GENERIC_ACCESS_APPEARANCE, HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,
    GATT_UUID_GAP_ICON,
    LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    // /* Declare TIME Service with 128 byte UUID */
    PRIMARY_SERVICE_UUID128( HDLS_TIME, UUID_TIME_SERVICE ),

    /* Declare characteristic used to send time data to client */
    CHARACTERISTIC_UUID128( HDLC_TIME_OUT, HDLC_TIME_OUT_VALUE,
    UUID_TIME_SERVICE_CHARACTERISTIC_OUT,
    LEGATTDB_CHAR_PROP_INDICATE | LEGATTDB_CHAR_PROP_NOTIFY, LEGATTDB_PERM_NONE ),

    CHAR_DESCRIPTOR_UUID16_WRITABLE( HDLC_TIME_OUT_CCC, GATT_UUID_CHAR_CLIENT_CONFIG,
    LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_REQ ),

    CHAR_DESCRIPTOR_UUID16( HDLC_TIME_OUT_DESCRIPTION, GATT_UUID_CHAR_DESCRIPTION,
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
    LEGATTDB_CHAR_PROP_READ|LEGATTDB_CHAR_PROP_WRITE, LEGATTDB_PERM_READABLE|LEGATTDB_PERM_WRITABLE ),
};

static int adv_complete_cb(void *arg)
{

}

struct indicate_arg_s {
    ble_gatt_attr_t *attr;
    peripheral_hdl_t hdl;
};

static void indicate_handler(void *arg)
{
    struct indicate_arg_s *ind = (struct indicate_arg_s *)arg;
    long long time = aos_now_ms();

    ble_attr_notify(ind->attr, ind->hdl, sizeof(time), (uint8_t *)&time);
    aos_post_delayed_action(1000, indicate_handler, arg);
}

#define DEVICE_MANUFACURE_NAME "BleAdvertisementsSampleManufacture"
#define DEVICE_MODEL_NUM "BleAdvertismentsSampleDeviceModel"
static uint8_t sys_id[] = {0x12, 0x34};

static const struct adv_data ad[] = {
    ADV_DATA_BYTES(EIRADV_DATA_FLAGS, (AD_FLAG_GENERAL | AD_FLAG_NO_BREDR)),
    ADV_DATA_BYTES(EIRADV_DATA_UUID16_ALL, 0x0a, 0x18),
};

static const struct adv_data sd[] = {
    ADV_DATA(EIRADV_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

int application_start( void )
{
    peripheral_hdl_t hdl;
    ble_gatt_attr_t *attr;
    struct indicate_arg_s ind_arg;

    peripheral_init_t p = {CONFIG_BT_DEVICE_NAME, 0, 1};

    hdl = ble_peripheral_init(&p, connection_handler, disconnection_handler,
                              adv_gatt_db, sizeof(adv_gatt_db));

    ble_set_ad_data(hdl, ad, sizeof(ad) / sizeof(ad[0]));
    ble_set_sd_data(hdl, sd, sizeof(sd) / sizeof(sd[0]));

    ble_adv_start(adv_complete_cb, DEVICE_MANUFACURE_NAME, hdl);

    ble_attr_add(HDLC_DEV_INFO_MFR_NAME_VALUE,
                 sizeof(DEVICE_MANUFACURE_NAME) - 1, DEVICE_MANUFACURE_NAME);
    ble_attr_add(HDLC_DEV_INFO_MODEL_NUM_VALUE,
                 sizeof(DEVICE_MODEL_NUM) - 1, DEVICE_MODEL_NUM);
    ble_attr_add(HDLC_DEV_INFO_SYSTEM_ID_VALUE, sizeof(sys_id),
                 (const uint8_t *)&sys_id);

    attr = ble_attr_add(HDLC_TIME_OUT_VALUE, 0, NULL);

    ind_arg.hdl = hdl;
    ind_arg.attr = attr;
    aos_post_delayed_action(1000, indicate_handler, &ind_arg);

    aos_loop_run();

    return 0;
}
