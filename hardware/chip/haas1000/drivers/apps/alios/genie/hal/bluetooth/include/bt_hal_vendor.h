/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#ifndef _BT_HAL_VENDOR_H_
#define _BT_HAL_VENDOR_H_

#define TG_BT_VENDOR_SUCCESS               0
#define TG_BT_VENDOR_FAILED                -1

/**
 * @brief set mesh active state to vendor
 * @param[in]: state: a2dp active state
 * @return: TG_BT_VENDOR_SUCCESS: success/TG_BT_VENDOR_FAILED: failed
 */
int32_t tg_bt_hal_vendor_set_a2dp_state(int32_t state);

/**
 * @brief set mesh active state to vendor
 * @param[in]: state: mesh active state
 * @return: TG_BT_VENDOR_SUCCESS: success/TG_BT_VENDOR_FAILED: failed
 */
int32_t tg_bt_hal_vendor_set_mesh_state(int32_t state);

/**
 * @brief set ble active state to vendor
 * @param[in] state: ble state
 * @return: TG_BT_VENDOR_SUCCESS: success/TG_BT_VENDOR_FAILED: failed
 */
int32_t tg_bt_hal_vendor_set_ble_state(int32_t state);

/**
 * @brief vendor chip bringup function
 * @param[in]: None.
 * @return: TG_BT_VENDOR_SUCCESS: success/TG_BT_VENDOR_FAILED: failed
 */
int32_t tg_bt_hal_vendor_bringup(void);

/**
 * @brief vendor chip shutdown function
 * @param[in]: None.
 * @return: TG_BT_VENDOR_SUCCESS: success/TG_BT_VENDOR_FAILED: failed
 */
int32_t tg_bt_hal_vendor_shutdown(void);

#endif

