/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BT_MESH_ADV_H
#define BT_MESH_ADV_H

#include <stdint.h>
#include <net/buf.h>

typedef struct {
        uint8_t  val[6];
} bt_mesh_addr_t;

typedef struct {
        uint8_t      type;
        bt_mesh_addr_t a;
} bt_mesh_addr_le_t;

/** LE Advertising Parameters. */
struct bt_mesh_le_adv_param {
        /** Bit-field of advertising options */
        uint8_t  options;

        /** Minimum Advertising Interval (N * 0.625) */
        uint16_t interval_min;

        /** Maximum Advertising Interval (N * 0.625) */
        uint16_t interval_max;

        /** Optional predefined (random) own address. Currently
         *  the only permitted use of this is for NRPA with
         *  non-connectable advertising.
         */
        const bt_mesh_addr_t *own_addr;
};

/** Description of different data types that can be encoded into
  * advertising data. Used to form arrays that are passed to the
  * bt_mesh_le_adv_start() function.
  */
struct bt_mesh_data {
        uint8_t type;
        uint8_t data_len;
        const uint8_t *data;
};

/** LE scan parameters */
struct bt_mesh_le_scan_param {
        /** Scan type (BT_HCI_LE_SCAN_ACTIVE or BT_HCI_LE_SCAN_PASSIVE) */
        uint8_t  type;

        /** Duplicate filtering (BT_HCI_LE_SCAN_FILTER_DUP_ENABLE or
         *  BT_HCI_LE_SCAN_FILTER_DUP_DISABLE)
         */
        uint8_t  filter_dup;

        /** Scan interval (N * 0.625 ms) */
        uint16_t interval;

        /** Scan window (N * 0.625 ms) */
        uint16_t window;
};

/** @typedef bt_mesh_le_scan_cb_t
 *  @brief Callback type for reporting LE scan results.
 *
 *  A function of this type is given to the bt_le_scan_start() function
 *  and will be called for any discovered LE device.
 *
 *  @param addr Advertiser LE address and type.
 *  @param rssi Strength of advertiser signal.
 *  @param adv_type Type of advertising response from advertiser.
 *  @param data Buffer containing advertiser data.
 */
typedef void bt_mesh_le_scan_cb_t(const bt_mesh_addr_le_t *addr, int8_t rssi,
                                  uint8_t adv_type, struct net_buf_simple *buf);

/** @brief Start advertising
 *
 *  Set advertisement data, scan response data, advertisement parameters
 *  and start advertising.
 *
 *  @param param Advertising parameters.
 *  @param ad Data to be used in advertisement packets.
 *  @param ad_len Number of elements in ad
 *  @param sd Data to be used in scan response packets.
 *  @param sd_len Number of elements in sd
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_adv_start(const struct bt_mesh_le_adv_param *param,
                      const struct bt_mesh_data *ad, size_t ad_len,
                      const struct bt_mesh_data *sd, size_t sd_len);

/** @brief Stop advertising
 *
 *  Stops ongoing advertising.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_adv_stop(void);

/** @brief Start (LE) scanning
 *
 *  Start LE scanning with given parameters and provide results through
 *  the specified callback.
 *
 *  @param param Scan parameters.
 *  @param cb Callback to notify scan results.
 *
 *  @return Zero on success or error code otherwise, positive in case
 *  of protocol error or negative (POSIX) in case of stack internal error
 */
int bt_mesh_scan_start(const struct bt_mesh_le_scan_param *param, bt_mesh_le_scan_cb_t cb);

/** @brief Stop (LE) scanning.
 *
 *  Stops ongoing LE scanning.
 *
 *  @return Zero on success or error code otherwise, positive in case
 *  of protocol error or negative (POSIX) in case of stack internal error
 */
int bt_mesh_scan_stop(void);

#endif //BT_MESH_ADV_H
