/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BT_MESH_ADV_H
#define BT_MESH_ADV_H

#include <bluetooth/bluetooth.h>

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
int bt_mesh_adv_start(const struct bt_le_adv_param *param,
                      const struct bt_data *ad, size_t ad_len,
                      const struct bt_data *sd, size_t sd_len);

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
int bt_mesh_scan_start(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb);

/** @brief Stop (LE) scanning.
 *
 *  Stops ongoing LE scanning.
 *
 *  @return Zero on success or error code otherwise, positive in case
 *  of protocol error or negative (POSIX) in case of stack internal error
 */
int bt_mesh_scan_stop(void);

#endif //BT_MESH_ADV_H
