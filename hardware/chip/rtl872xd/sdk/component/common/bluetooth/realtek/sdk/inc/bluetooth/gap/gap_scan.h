/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      gap_scan.h
* @brief     Head file for Gap Observer role
* @details
* @author    jane
* @date      2016-02-18
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_SCAN_H
#define GAP_SCAN_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <bt_flags.h>
#if F_BT_LE_GAP_SCAN_SUPPORT
#include <gap_le.h>


/** @defgroup Observer_Role GAP Observer Role
  * @brief GAP observer role
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup Observer_Exported_Types GAP Observer Exported Types
  * @{
  */

/** @brief LE scan parameter type */
typedef enum
{
    GAP_PARAM_SCAN_LOCAL_ADDR_TYPE   = 0x240,  //!< The type of address being used in the scan request packets. Read/Write.
    GAP_PARAM_SCAN_MODE              = 0x241,  //!< Scan mode. Read/Write. Size is uint8. Default is GAP_SCAN_MODE_ACTIVE (@ref T_GAP_SCAN_MODE).
    GAP_PARAM_SCAN_INTERVAL          = 0x242,  //!< Scan Interval. Read/Write. Size is uint16_t. Default is 0x10. Value range: 0x0004 - 0x4000 (2.5ms - 10240ms 0.625ms/step).
    GAP_PARAM_SCAN_WINDOW            = 0x243,  //!< Scan Window. Read/Write. Size is uint16_t. Default is 0x10. Value range: 0x0004 - 0x4000 (2.5ms - 10240ms 0.625ms/step).
    GAP_PARAM_SCAN_FILTER_POLICY     = 0x244,  //!< Scan Filter Policy.Read/Write. Size is uint8_t. Default is GAP_SCAN_FILTER_ANY (@ref T_GAP_SCAN_FILTER_POLICY).
    GAP_PARAM_SCAN_FILTER_DUPLICATES = 0x245   //!< Scan Filter Duplicates.Read/Write. Size is uint8_t. Default is GAP_SCAN_FILTER_DUPLICATE_DISABLE (@ref T_GAP_SCAN_FILTER_DUPLICATE).
} T_LE_SCAN_PARAM_TYPE;


/** End of Observer_Exported_Types
  * @}
  */


/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup Observer_Exported_Functions GAP Observer Exported Functions
  * @{
  */

/**
 * @brief       Set a scan parameter.
 *
 *              NOTE: You can call this function with a scan parameter type and it will set the
 *              scan parameter.  Scan parameters are defined in @ref T_LE_SCAN_PARAM_TYPE.
 *              If the "len" field sets to the size of a "uint16_t" ,the "p_value" field must
 *              point to a data with type "uint16".
 *
 * @param[in]   param Scan parameter type: @ref T_LE_SCAN_PARAM_TYPE
 * @param[in]   len  Length of data to write
 * @param[in]   p_value  Pointer to data to write.  This is dependent on
 *              the parameter type and will be cast to the appropriate
 *              data type (For example: if data type param is uint16, p_value will be cast to
 *              pointer of uint16_t).
 *
 * @return Set result
 * @retval GAP_CAUSE_SUCCESS Set parameter success.
 * @retval other Set parameter failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_gap_init()
    {
        uint8_t device_name[GAP_DEVICE_NAME_LEN] = "BBPro_central";
        uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;

        uint8_t scan_mode = GAP_SCAN_MODE_ACTIVE;
        uint16_t scan_interval = DEFAULT_SCAN_INTERVAL;
        uint16_t scan_window = DEFAULT_SCAN_WINDOW;
        uint8_t scan_filter_policy = GAP_SCAN_FILTER_ANY;
        uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;

        uint8_t pair_mode = GAP_PAIRING_MODE_PAIRABLE;
        uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
        uint8_t io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
        uint8_t oob_enable = false;
        uint32_t passkey = 0;
        uint8_t fix_passkey_enable = false;

        le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
        le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);
        gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(uint8_t), &pair_mode);
        gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t), &auth_flags);
        gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &io_cap);
        gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(uint8_t), &oob_enable);

        le_scan_set_param(GAP_PARAM_SCAN_MODE, sizeof(scan_mode), &scan_mode);
        le_scan_set_param(GAP_PARAM_SCAN_INTERVAL, sizeof(scan_interval), &scan_interval);
        le_scan_set_param(GAP_PARAM_SCAN_WINDOW, sizeof(scan_window), &scan_window);
        le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
                         &scan_filter_policy);
        le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
                         &scan_filter_duplicate);

        le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(uint32_t), &passkey);
        le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(uint8_t), &fix_passkey_enable);
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_set_param(T_LE_SCAN_PARAM_TYPE param, uint8_t len, void *p_value);

/**
 * @brief       Get a scan parameter.
 *
 *              NOTE: You can call this function with a scan parameter type and it will get a
 *              scan parameter.  Scan parameters are defined in @ref T_LE_SCAN_PARAM_TYPE.
 *              Also, the "p_value" field must point to a data with type of "uint16_t".
 *
 * @param[in]   param   Scan parameter type: @ref T_LE_SCAN_PARAM_TYPE.
 * @param[in,out]  p_value Pointer to location to get the parameter value.  This is dependent on
 *                      the parameter type and will be cast to the appropriate
 *                      data type (For example: if data type of param is uint16_t, p_value will be cast to
 *                      pointer of uint16_t).
 *
 * @return  Get result
 * @retval  GAP_CAUSE_SUCCESS Get parameter success.
 * @retval  Others Get parameter failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint16_t scan_interval;
        le_scan_get_param(GAP_PARAM_SCAN_INTERVAL, &scan_interval);
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_get_param(T_LE_SCAN_PARAM_TYPE param, void *p_value);

/**
 * @brief   Start a device discovery scan. If device changes to scanning state, @ref app_handle_dev_state_evt
 *          will be called. And the advertising data or scan response data will be returned by @ref app_gap_callback
 *          with cb_type @ref GAP_MSG_LE_SCAN_INFO.
 *
 *          NOTE: This function can be called after @ref gap_start_bt_stack is invoked.
 *
 * @return Operation result.
 * @retval  GAP_CAUSE_SUCCESS: Operation success, scan started.
 * @retval  GAP_CAUSE_ALREADY_IN_REQ: Operation failure, scan state is GAP_SCAN_STATE_START.
 * @retval  GAP_CAUSE_INVALID_STATE: Operation failure, invalid device state.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        le_scan_start();
    }

    void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
    {
        APP_PRINT_INFO5("app_handle_dev_state_evt: init state = %d scan state = %d adv state = %d conn state = %d cause = 0x%x",
                       new_state.gap_init_state,
                       new_state.gap_scan_state, new_state.gap_adv_state, new_state.gap_conn_state, cause);

        if (gap_dev_state.gap_scan_state != new_state.gap_scan_state)
        {
           if (new_state.gap_scan_state == GAP_SCAN_STATE_IDLE)
           {
               APP_PRINT_INFO0("GAP scan stop");
               data_uart_print("GAP scan stop\r\n");
           }
           else if (new_state.gap_scan_state == GAP_SCAN_STATE_SCANNING)
           {
               APP_PRINT_INFO0("GAP scan start");
               data_uart_print("GAP scan start\r\n");
           }
        }
    }
    //Received advertising or scan rsp data will be handled in app_gap_callback
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type = %d", cb_type);
        switch (cb_type)
        {
        case GAP_MSG_LE_SCAN_INFO:
            APP_PRINT_INFO5("GAP_MSG_LE_SCAN_INFO:adv_type 0x%x, bd_addr %s, remote_addr_type %d, rssi %d, data_len %d",
                           cb_data.p_le_scan_info->adv_type,
                           TRACE_BDADDR(cb_data.p_le_scan_info->bd_addr),
                           cb_data.p_le_scan_info->remote_addr_type,
                           cb_data.p_le_scan_info->rssi,
                           cb_data.p_le_scan_info->data_len);
            break;
        }
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_start(void);

/**
  * @brief   Cancel a device discovery scan.
  *
  *          NOTE: This function can be called after @ref gap_start_bt_stack is invoked.
  *
  * @return Operation result.
  * @retval  GAP_CAUSE_SUCCESS: Operation success, Cancel started.
  * @retval  GAP_CAUSE_INVALID_STATE: Operation Failure. Invalid state, not in scan mode.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test()
    {
        le_scan_stop();
    }
    void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
    {
        APP_PRINT_INFO5("app_handle_dev_state_evt: init state = %d scan state = %d adv state = %d conn state = %d cause = 0x%x",
                       new_state.gap_init_state,
                       new_state.gap_scan_state, new_state.gap_adv_state, new_state.gap_conn_state, cause);

        if (gap_dev_state.gap_scan_state != new_state.gap_scan_state)
        {
            if (new_state.gap_scan_state == GAP_SCAN_STATE_IDLE)
            {
                APP_PRINT_INFO0("GAP scan stop");
                data_uart_print("GAP scan stop\r\n");
            }
            else if (new_state.gap_scan_state == GAP_SCAN_STATE_SCANNING)
            {
                APP_PRINT_INFO0("GAP scan start");
                data_uart_print("GAP scan start\r\n");
            }
        }
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_stop(void);
/** @} */ /* End of group Observer_Exported_Functions */
/** @} */ /* End of group Observer_Role */
#endif

#ifdef __cplusplus
}
#endif

#endif /* GAP_SCAN_H */
