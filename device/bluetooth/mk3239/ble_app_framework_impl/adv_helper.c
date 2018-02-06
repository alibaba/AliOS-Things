/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "internal/ble_helper_internal.h"

/* Peripheral auto advertising settings */
static mico_bt_smart_advertising_settings_t advertising_settings = {
    .type               = BT_SMART_UNDIRECTED_ADVERTISING,                  /**< Advertising type                                               */
    .use_high_duty      = MICO_TRUE,                                        /**< Using high duty to start advertising                           */
    .high_duty_interval = MICO_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL,   /**< High duty advertising interval                                 */
    .high_duty_duration = 5,                                                /**< High duty advertising duration in seconds (0 for infinite)     */
    .low_duty_interval  = MICO_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL,    /**< Low duty advertising interval                                  */
    .low_duty_duration  = 60,                                               /**< Low duty advertising duration in seconds (0 for infinite)      */

    /* Used Directed Advertisement */
    .directed_advertisement_addr_type   = BT_SMART_ADDR_TYPE_PUBLIC,
    .directed_advertisement_addr        = { 11, 22, 33, 44, 55, 66 },
};

/* Set advertisement data and scanning response data */
static OSStatus set_advertisement_data(const char *manufacture)
{
    OSStatus err = kNoErr;

    uint16_t uuid[1] = { UUID_SERVCLASS_DEVICE_INFO };
    mico_bt_ble_advert_data_t adv_data;

    mico_bt_ble_service_t advert_services_16 = {
        .num_service = 1,
        .list_cmpl  = true,
        .p_uuid     = uuid,
    };

    mico_bt_ble_manu_t advert_manufacture = {
        .len    = strlen(manufacture),
        .p_val  = (uint8_t *)manufacture,
    };

    adv_data.flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    adv_data.p_services = &advert_services_16;
    adv_data.p_manu = &advert_manufacture;

    mico_bt_ble_set_advertisement_data(
        BTM_BLE_ADVERT_BIT_DEV_NAME | BTM_BLE_ADVERT_BIT_SERVICE | BTM_BLE_ADVERT_BIT_FLAGS,
        &adv_data );
    mico_bt_ble_set_scan_response_data( BTM_BLE_ADVERT_BIT_MANUFACTURER, &adv_data );

    return err;
}

/* Display Advertisement Arguments */
static void start_advertisements(peripheral_hdl_t hdl,
                                 mico_bt_smart_advertising_complete_callback_t advertisement_complete_handle)
{
    mico_bt_dev_status_t status;
    mico_bt_peripheral_socket_status_t socket_status = PERIPHERAL_SOCKET_CONNECTED;
    peripheral_socket_t *p = g_skt_head;

    printf("%d, hdl (%d) to operate\r\n", __LINE__, hdl);

    for (; p; p = p->next) {
        printf("%d iterate on hdl (%d)\r\n", __LINE__, p->hdl);
        if (p->hdl == hdl) {
            break;
        }
    }

    if (!p) {
        printf("%s %d no hdl found\r\n", __func__, __LINE__);
        return;
    }

    /* Check current connection state. */
    mico_bt_peripheral_get_socket_status(&p->skt, &socket_status );
    if ( socket_status != PERIPHERAL_SOCKET_DISCONNECTED ) {
        printf( "Advertisements Started unsuccessfully! Existed connection" );
        printf( "" );
        return;
    }

    /* Start advertisements */
    status = mico_bt_peripheral_start_advertisements( &advertising_settings,
                                                      advertisement_complete_handle );

    /* LOG */
    printf("");
    if ( status == MICO_BT_SUCCESS ) {
        printf( "Advertisements Started successfully! Arguments: " );
    } else if ( status == MICO_BT_ILLEGAL_VALUE ) {
        printf( "Advertisements Started unsuccessfully! Illegal value: " );
    } else {
        printf( "Advertisements Started unsuccessfully! Unknown error: " );
    }
    printf( "" );
    //printf( "\t\t type              : %s", pBT_AdvertStr( advertising_settings.type ) );
    printf( "\t\t duty              : %s",
            advertising_settings.use_high_duty == MICO_TRUE ? "High duty" : "Low duty" );
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

void ble_adv_start(mico_bt_smart_advertising_complete_callback_t adv_handler,
                   const char *manufacture, peripheral_hdl_t hdl)
{
    /* Set the advertising parameters and make the device discoverable */
    set_advertisement_data(manufacture);

    /* Start advertising */
    start_advertisements(hdl, adv_handler);
}

void ble_adv_stop()
{

}

void ble_set_ad_data
(
    peripheral_hdl_t hdl,
    const struct adv_data *ad,
    size_t ad_siz
)
{

}

void ble_set_sd_data
(
    peripheral_hdl_t hdl,
    const struct adv_data *sd,
    size_t sd_siz
)
{

}
