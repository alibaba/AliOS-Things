/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BLE_APP_FRAMEWORK_H_
#define _BLE_APP_FRAMEWORK_H_

#include <stddef.h>
#include "ble_app_framework_def.h"

/**
 * Initialize peripheral device.
 * This functions will initialize the BLE stack and peripheral, 
 * as well as build up a GATT database.
 *
 * @param[in]  p          The parameters used to initialze the stack 
 *                        and peripheral.
 * @param[in]  c          The callback function which will be called when 
 *                        client device connected.
 * @param[in]  disc       The callback function which will be called when 
 *                        client device disconnected.
 * @param[in]  gatt_db    The GATT database which is to be built.
 * @param[in]  db_len     The size of the GATT database.
 *
 * @return  peripheral_hdl_t The peripheral device handle.
 */
peripheral_hdl_t
ble_peripheral_init
(
    peripheral_init_t *p,
    ble_peripheral_conn_cb_t c,
    ble_peripheral_disconn_cb_t disc,
    const uint8_t *gatt_db,
    int db_len
);

/**
 * De-initialize the perripheral device.
 *
 * @param[in]  hdl The peripheral device handle.
 */
void ble_peripheral_deinit
(
    peripheral_hdl_t hdl
);

/**
 * Start the advertisement.
 *
 * @param[in]  adv_handler  The functio to be called when the 
                            advertisement completed.
 * @param[in]  manufacture  The string of the manufacture name.
 * @param[in]  hdl          The peripheral device handle.
 */
void ble_adv_start
(
    ble_adv_complete_cb_t adv_handler,
    const char *manufacture,
    peripheral_hdl_t hdl
);

/**
 * Stop the advertisement.
 */
void ble_adv_stop();

/**
 * Add an GATT attribute.
 *
 * @param[in]  hdl      The peripheral device handle.
 * @param[in]  val_len  The length of the attribute value in bytes.
 * @param[in]  val      The value of the attribute.
 *
 * @return  ble_gatt_attr_t  The GATT attribute structure.
 */
ble_gatt_attr_t *
ble_attr_add
(
    uint16_t hdl,
    uint16_t val_len,
    const uint8_t *val
);

/**
 * Send indication of a GATT attribute value to client device.
 *
 * @param[in]  attr  The attribute structure.
 * @param[in]  hdl   The peripheral device handle.
 * @param[in]  len   The length of the data to indicate.
 * @param[in]  data  The data to indicate.
 */
void ble_attr_indicate
(
    ble_gatt_attr_t *attr,
    peripheral_hdl_t hdl,
    uint16_t len,
    const uint8_t *data
);

/**
 * Send notification of a GATT attribute value to client device.
 *
 * @param[in]  attr  The attribute structure.
 * @param[in]  hdl   The peripheral device handle.
 * @param[in]  len   The length of the data to nofity.
 * @param[in]  data  The data to notify.
 */
void ble_attr_notify
(
    ble_gatt_attr_t *attr,
    peripheral_hdl_t hdl,
    uint16_t len,
    const uint8_t *data
);

/**
 * Set the advertisment data. This step is optional to user.
 * The default adv data will be used if adv data is not set explicitly
 * set by calling this func.
 *
 * @param[in]  hdl     The peripheral device handle.
 * @param[in]  ad      The array of the adv data.
 * @param[in]  ad_siz  The number of elements in the ad data array.
 */
void ble_set_ad_data
(
    peripheral_hdl_t hdl,
    const struct adv_data *ad,
    size_t ad_siz
);

/**
 * Set the scan response data. This step is optional to user.
 * The default sd data will be used if sd data is not set explicitly
 * set by calling this func.
 *
 * @param[in]  hdl     The peripheral device handle.
 * @param[in]  sd      The array of the scan response data.
 * @param[in]  sd_siz  The number of elements in the sd data array.
 */
void ble_set_sd_data
(
    peripheral_hdl_t hdl,
    const struct adv_data *sd,
    size_t sd_siz
);
#endif
