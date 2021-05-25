#ifndef VNR_CMD_BT_H
#define VNR_CMD_BT_H

#include <gap.h>

/** @brief  Supported LE Vendor Commands*/
#define BT_VENDOR_CMD_ONE_SHOT_SUPPORT      1
#define BT_VENDOR_CMD_ADV_TX_POWER_SUPPORT  1
#define BT_VENDOR_CMD_CONN_TX_POWER_SUPPORT 1
#define BT_VENDOR_CMD_SLAVE_LATENCY_SUPPORT 1

/** @brief  LE Vendor Command Opcode*/
#define HCI_LE_VENDOR_EXTENSION_FEATURE2        0xFC87
#define HCI_EXT_SUB_ONE_SHOT_ADV                1

#define HCI_LE_VENDOR_EXTENSION_FEATURE         0xFD80
#define HCI_EXT_SUB_SET_ADV_TX_POWER            0
#define HCI_EXT_SUB_DISABLE_LATENCY             1
#define HCI_EXT_SUB_SET_LINK_TX_POW             0xC

/**
  * @brief  Trigger One Shot Advertising.
  *               This function can be called after @ref vendor_cmd_init is invoked.
  *               This function cannot be used if advertising type is connectable high duty cycle directed advertising.
  *
  * @return Operation result.
  * @retval GAP_CAUSE_SUCCESS: Operation success.
  * @retval GAP_CAUSE_SEND_REQ_FAILED: Operation fail.
  *
  * <b>Example usage</b>
  * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_oneshot(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
#if BT_VENDOR_CMD_ONE_SHOT_SUPPORT
        cause = le_vendor_one_shot_adv();
#endif
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
  * \endcode
  */
#if BT_VENDOR_CMD_ONE_SHOT_SUPPORT
T_GAP_CAUSE le_vendor_one_shot_adv(void);
#endif

/**
  * @brief  Set the advertising tx power for the device, or reset advertising tx power to default value.
  *
  *         NOTE: This function can be called after @ref vendor_cmd_init is invoked.
  *
  * @param[in] option  Set to 0.
  * @param[in] tx_gain index for power level. NOTE: The following tx gain table may be changed in future version.
                       tx_gain   Power
                       0x06      -10 dBm
                       0x1A      0   dBm
                       0x23      4.5 dBm

  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval GAP_CAUSE_SEND_REQ_FAILED Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
     void test()
     {
         T_GAP_CAUSE cause;
         uint8_t tx_gain = 0x06;
         cause = le_adv_set_tx_power(0, tx_gain);
     }
     void app_vendor_callback(uint8_t cb_type, void *p_cb_data)
     {
        T_GAP_VENDOR_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_GAP_VENDOR_CB_DATA));
        APP_PRINT_INFO1("app_vendor_callback: command 0x%x", cb_data.p_gap_vendor_cmd_rsp->command);
        switch (cb_type)
        {
        case GAP_MSG_VENDOR_CMD_RSP:
            switch(cb_data.p_gap_vendor_cmd_rsp->command)
            { 
                case HCI_LE_VENDOR_EXTENSION_FEATURE:
                    switch(cb_data.p_gap_vendor_cmd_rsp->param[0])
                    {
#if BT_VENDOR_CMD_ADV_TX_POWER_SUPPORT
                        case HCI_EXT_SUB_SET_ADV_TX_POWER:
                            APP_PRINT_INFO1("HCI_EXT_SUB_SET_ADV_TX_POWER: cause 0x%x", cb_data.p_gap_vendor_cmd_rsp->cause);
                            break;
#endif
                    }
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
        }

        return;
    }
  * \endcode
  */
#if BT_VENDOR_CMD_ADV_TX_POWER_SUPPORT
T_GAP_CAUSE le_adv_set_tx_power(uint8_t option, uint8_t tx_gain);
#endif

/**
  * @brief  Set the link tx power for the device.
  *
  *         Set LE link tx power. Or reset link tx power to default value.
  *         Tx power is set to default every time a link is connect. Use this command
  *         after the link is connected.
  *         Tx gain default value, the order of precedence is Logical efuse -> Physical efuse -> driver config
  *
  *         NOTE: This function can be called after @ref vendor_cmd_init is invoked.
  *
  * @param[in] conn_id  Connection ID for this link.
  * @param[in] reset    Whether to reset to default value and restore power track function.
  * @param[in] tx_gain index for power level. NOTE: The following tx gain table may be changed in future version.
                       tx_gain   Power
                       0x06      -10 dBm
                       0x1A      0   dBm
                       0x23      4.5 dBm

  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval GAP_CAUSE_SEND_REQ_FAILED Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
     void test()
     {
        T_GAP_CAUSE cause;
        uint8_t conn_id = 0;
        bool reset = false;
        uint8_t tx_pwr = 0x06;

        cause = le_set_conn_tx_power(conn_id, reset, tx_pwr);
     }
     void app_vendor_callback(uint8_t cb_type, void *p_cb_data)
     {
        T_GAP_VENDOR_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_GAP_VENDOR_CB_DATA));
        APP_PRINT_INFO1("app_vendor_callback: command 0x%x", cb_data.p_gap_vendor_cmd_rsp->command);
        switch (cb_type)
        {
        case GAP_MSG_VENDOR_CMD_RSP:
            switch(cb_data.p_gap_vendor_cmd_rsp->command)
            { 
                case HCI_LE_VENDOR_EXTENSION_FEATURE:
                    switch(cb_data.p_gap_vendor_cmd_rsp->param[0])
                    {
#if BT_VENDOR_CMD_CONN_TX_POWER_SUPPORT
                        case HCI_EXT_SUB_SET_LINK_TX_POW:
                            APP_PRINT_INFO1("HCI_EXT_SUB_SET_LINK_TX_POW: cause 0x%x", cb_data.p_gap_vendor_cmd_rsp->cause);
                            break;
#endif
                    }
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
        }

        return;
    }
  * \endcode
  */
#if BT_VENDOR_CMD_CONN_TX_POWER_SUPPORT
T_GAP_CAUSE le_set_conn_tx_power(uint8_t conn_id, bool reset, uint8_t tx_gain);
#endif

#if BT_VENDOR_CMD_SLAVE_LATENCY_SUPPORT
T_GAP_CAUSE le_disable_slave_latency(uint8_t conn_id, bool disable);
#endif

/**
  * @brief  Initialize vendor command module.
  * @param[in] link_num Initialize link number.
  * @retval true Success.
  * @retval false Failed because of invalid parameter.
 *
 * <b>Example usage</b>
 * \code{.c}

 * \endcode
 */
/**
 * @brief  Initialize vendor command module and register callback to gap, when messages in @ref GAP_VENDOR_MSG_TYPE happens, it will callback to app.
 * @param[in] app_cb Callback function provided by the APP to handle gap vendor command messages sent from the vendor command module.
 *              @arg NULL -> Not send vendor command messages to APP.
 *              @arg Other -> Use application defined callback function.
 * @return void
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_gap_init(void)
    {
        ......
        vendor_cmd_init(app_vendor_callback);
    }
    void app_vendor_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_GAP_VENDOR_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_GAP_VENDOR_CB_DATA));
        APP_PRINT_INFO1("app_vendor_callback: command 0x%x", cb_data.p_gap_vendor_cmd_rsp->command);
        switch (cb_type)
        {
        case GAP_MSG_VENDOR_CMD_RSP:
            switch(cb_data.p_gap_vendor_cmd_rsp->command)
            { 
                case HCI_LE_VENDOR_EXTENSION_FEATURE:
                    switch(cb_data.p_gap_vendor_cmd_rsp->param[0])
                    {
#if BT_VENDOR_CMD_ADV_TX_POWER_SUPPORT
                        case HCI_EXT_SUB_SET_ADV_TX_POWER:
                            APP_PRINT_INFO1("HCI_EXT_SUB_SET_ADV_TX_POWER: cause 0x%x", cb_data.p_gap_vendor_cmd_rsp->cause);
                            break;
#endif
#if BT_VENDOR_CMD_CONN_TX_POWER_SUPPORT
                        case HCI_EXT_SUB_SET_LINK_TX_POW:
                            APP_PRINT_INFO1("HCI_EXT_SUB_SET_LINK_TX_POW: cause 0x%x", cb_data.p_gap_vendor_cmd_rsp->cause);
                            break;
#endif
                    }
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
        }

        return;
    }
   \endcode
 */
void vendor_cmd_init(P_FUN_GAP_APP_CB app_cb);

#endif /* VNR_CMD_H */
