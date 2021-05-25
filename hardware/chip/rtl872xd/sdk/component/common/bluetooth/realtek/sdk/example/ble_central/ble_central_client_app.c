/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      central_client_app.c
   * @brief     This file handles BLE central application routines.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <app_msg.h>
#include <string.h>
#include <trace_app.h>
#include <gap_scan.h>
#include <gap.h>
#include <gap_msg.h>
#include <gap_bond_le.h>
#include <ble_central_client_app.h>
#include <ble_central_link_mgr.h>
#include <user_cmd.h>
#include <user_cmd_parse.h>
#include <gcs_client.h>

/** @defgroup  CENTRAL_CLIENT_APP Central Client Application
    * @brief This file handles BLE central client application routines.
    * @{
    */
/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @addtogroup  CENTRAL_CLIIENT_CALLBACK
    * @{
    */
T_CLIENT_ID   ble_central_gcs_client_id;         /**< General Common Services client client id*/
/** @} */ /* End of group CENTRAL_CLIIENT_CALLBACK */

/** @defgroup  CENTRAL_CLIENT_GAP_MSG GAP Message Handler
    * @brief Handle GAP Message
    * @{
    */
T_GAP_DEV_STATE ble_central_gap_dev_state = {0, 0, 0, 0, 0};                /**< GAP device state */
/*============================================================================*
 *                              Functions
 *============================================================================*/
void ble_central_app_handle_gap_msg(T_IO_MSG  *p_gap_msg);
/**
 * @brief    All the application messages are pre-handled in this function
 * @note     All the IO MSGs are sent to this function, then the event handling
 *           function shall be called according to the MSG type.
 * @param[in] io_msg  IO message data
 * @return   void
 */
void ble_central_app_handle_io_msg(T_IO_MSG io_msg)
{
    uint16_t msg_type = io_msg.type;
    uint8_t rx_char;

    switch (msg_type)
    {
    case IO_MSG_TYPE_BT_STATUS:
        {
            ble_central_app_handle_gap_msg(&io_msg);
        }
        break;
#if CONFIG_BT_USER_COMMAND
    case IO_MSG_TYPE_UART:
        /* We handle user command informations from Data UART in this branch. */
        rx_char = (uint8_t)io_msg.subtype;
        user_cmd_collect(&user_cmd_if, &rx_char, sizeof(rx_char), user_cmd_table);
        break;
#endif
    default:
        break;
    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_DEV_STATE_CHANGE
 * @note     All the gap device state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] new_state  New gap device state
 * @param[in] cause GAP device state change cause
 * @return   void
 */
void ble_central_app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    APP_PRINT_INFO3("ble_central_app_handle_dev_state_evt: init state  %d, scan state %d, cause 0x%x",
                    new_state.gap_init_state,
                    new_state.gap_scan_state, cause);
    if (ble_central_gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            uint8_t bt_addr[6];
            APP_PRINT_INFO0("GAP stack ready");
            /*stack ready*/
            gap_get_param(GAP_PARAM_BD_ADDR, bt_addr);
#if CONFIG_BT_USER_COMMAND
            data_uart_print("local bd addr: 0x%2x:%2x:%2x:%2x:%2x:%2x\r\n",
                            bt_addr[5],
                            bt_addr[4],
                            bt_addr[3],
                            bt_addr[2],
                            bt_addr[1],
                            bt_addr[0]);
#endif
        }
    }

    if (ble_central_gap_dev_state.gap_scan_state != new_state.gap_scan_state)
    {
        if (new_state.gap_scan_state == GAP_SCAN_STATE_IDLE)
        {
            APP_PRINT_INFO0("GAP scan stop");
#if CONFIG_BT_USER_COMMAND
            data_uart_print("GAP scan stop\r\n");
#endif
			BLE_PRINT("GAP scan stop\r\n");
        }
        else if (new_state.gap_scan_state == GAP_SCAN_STATE_SCANNING)
        {
            APP_PRINT_INFO0("GAP scan start");
#if CONFIG_BT_USER_COMMAND
            data_uart_print("GAP scan start\r\n");
#endif
			BLE_PRINT("GAP scan start\r\n");
        }
    }

    ble_central_gap_dev_state = new_state;
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_STATE_CHANGE
 * @note     All the gap conn state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New gap connection state
 * @param[in] disc_cause Use this cause when new_state is GAP_CONN_STATE_DISCONNECTED
 * @return   void
 */
void ble_central_app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
{
    if (conn_id >= BLE_CENTRAL_APP_MAX_LINKS)
    {
        return;
    }

    APP_PRINT_INFO4("ble_central_app_handle_conn_state_evt: conn_id %d, conn_state(%d -> %d), disc_cause 0x%x",
                    conn_id, ble_central_app_link_table[conn_id].conn_state, new_state, disc_cause);

    ble_central_app_link_table[conn_id].conn_state = new_state;
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
            {
                APP_PRINT_ERROR2("ble_central_app_handle_conn_state_evt: connection lost, conn_id %d, cause 0x%x", conn_id,
                                 disc_cause);
            }

#if CONFIG_BT_USER_COMMAND
            data_uart_print("Disconnect conn_id %d\r\n", conn_id);
#endif
            BLE_PRINT("Disconnect conn_id %d\r\n", conn_id);
            memset(&ble_central_app_link_table[conn_id], 0, sizeof(T_APP_LINK));
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            le_get_conn_addr(conn_id, ble_central_app_link_table[conn_id].bd_addr,
                             &ble_central_app_link_table[conn_id].bd_type);
#if CONFIG_BT_USER_COMMAND
            data_uart_print("Connected success conn_id %d\r\n", conn_id);
#endif
			BLE_PRINT("Connected success conn_id %d\r\n", conn_id);
        }
        break;

    default:
        break;

    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_AUTHEN_STATE_CHANGE
 * @note     All the gap authentication state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New authentication state
 * @param[in] cause Use this cause when new_state is GAP_AUTHEN_STATE_COMPLETE
 * @return   void
 */
void ble_central_app_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t cause)
{
    APP_PRINT_INFO2("ble_central_app_handle_authen_state_evt:conn_id %d, cause 0x%x", conn_id, cause);

    switch (new_state)
    {
    case GAP_AUTHEN_STATE_STARTED:
        {
            APP_PRINT_INFO0("ble_central_app_handle_authen_state_evt: GAP_AUTHEN_STATE_STARTED");
        }
        break;

    case GAP_AUTHEN_STATE_COMPLETE:
        {
            if (cause == GAP_SUCCESS)
            {
#if CONFIG_BT_USER_COMMAND
                data_uart_print("Pair success\r\n");
#endif
                APP_PRINT_INFO0("ble_central_app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair success");

            }
            else
            {
#if CONFIG_BT_USER_COMMAND
                data_uart_print("Pair failed: cause 0x%x\r\n", cause);
#endif
                APP_PRINT_INFO0("ble_central_app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair failed");
            }
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("ble_central_app_handle_authen_state_evt: unknown newstate %d", new_state);
        }
        break;
    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_MTU_INFO
 * @note     This msg is used to inform APP that exchange mtu procedure is completed.
 * @param[in] conn_id Connection ID
 * @param[in] mtu_size  New mtu size
 * @return   void
 */
void ble_central_app_handle_conn_mtu_info_evt(uint8_t conn_id, uint16_t mtu_size)
{
    APP_PRINT_INFO2("ble_central_app_handle_conn_mtu_info_evt: conn_id %d, mtu_size %d", conn_id, mtu_size);
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_PARAM_UPDATE
 * @note     All the connection parameter update change  events are pre-handled in this function.
 * @param[in] conn_id Connection ID
 * @param[in] status  New update state
 * @param[in] cause Use this cause when status is GAP_CONN_PARAM_UPDATE_STATUS_FAIL
 * @return   void
 */
void ble_central_app_handle_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
{
    switch (status)
    {
    case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
        {
            uint16_t conn_interval;
            uint16_t conn_slave_latency;
            uint16_t conn_supervision_timeout;

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
            APP_PRINT_INFO4("ble_central_app_handle_conn_param_update_evt update success:conn_id %d, conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x",
                            conn_id, conn_interval, conn_slave_latency, conn_supervision_timeout);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
        {
            APP_PRINT_ERROR2("ble_central_app_handle_conn_param_update_evt update failed: conn_id %d, cause 0x%x",
                             conn_id, cause);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
        {
            APP_PRINT_INFO1("ble_central_app_handle_conn_param_update_evt update pending: conn_id %d", conn_id);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief    All the BT GAP MSG are pre-handled in this function.
 * @note     Then the event handling function shall be called according to the
 *           subtype of T_IO_MSG
 * @param[in] p_gap_msg Pointer to GAP msg
 * @return   void
 */
void ble_central_app_handle_gap_msg(T_IO_MSG *p_gap_msg)
{
    T_LE_GAP_MSG gap_msg;
    uint8_t conn_id;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));

    APP_PRINT_TRACE1("ble_central_app_handle_gap_msg: subtype %d", p_gap_msg->subtype);
    switch (p_gap_msg->subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
        {
            ble_central_app_handle_dev_state_evt(gap_msg.msg_data.gap_dev_state_change.new_state,
                                     gap_msg.msg_data.gap_dev_state_change.cause);
        }
        break;

    case GAP_MSG_LE_CONN_STATE_CHANGE:
        {
            ble_central_app_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                      (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                      gap_msg.msg_data.gap_conn_state_change.disc_cause);
        }
        break;

    case GAP_MSG_LE_CONN_MTU_INFO:
        {
            ble_central_app_handle_conn_mtu_info_evt(gap_msg.msg_data.gap_conn_mtu_info.conn_id,
                                         gap_msg.msg_data.gap_conn_mtu_info.mtu_size);
        }
        break;

    case GAP_MSG_LE_CONN_PARAM_UPDATE:
        {
            ble_central_app_handle_conn_param_update_evt(gap_msg.msg_data.gap_conn_param_update.conn_id,
                                             gap_msg.msg_data.gap_conn_param_update.status,
                                             gap_msg.msg_data.gap_conn_param_update.cause);
        }
        break;

    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
        {
            ble_central_app_handle_authen_state_evt(gap_msg.msg_data.gap_authen_state.conn_id,
                                        gap_msg.msg_data.gap_authen_state.new_state,
                                        gap_msg.msg_data.gap_authen_state.status);
        }
        break;

    case GAP_MSG_LE_BOND_JUST_WORK:
        {
            conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
            le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
            APP_PRINT_INFO0("GAP_MSG_LE_BOND_JUST_WORK");
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO2("GAP_MSG_LE_BOND_PASSKEY_DISPLAY: conn_id %d, passkey %d",
                            conn_id, display_value);
            le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
#if CONFIG_BT_USER_COMMAND
            data_uart_print("GAP_MSG_LE_BOND_PASSKEY_DISPLAY: conn_id %d, passkey %d\r\n",
                            conn_id,
                            display_value);
#endif
        }
        break;

    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO2("GAP_MSG_LE_BOND_USER_CONFIRMATION: conn_id %d, passkey %d",
                            conn_id, display_value);
#if CONFIG_BT_USER_COMMAND
            data_uart_print("GAP_MSG_LE_BOND_USER_CONFIRMATION: conn_id %d, passkey %d\r\n",
                            conn_id,
                            display_value);
#endif
            //le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
        {
            //uint32_t passkey = 888888;
            conn_id = gap_msg.msg_data.gap_bond_passkey_input.conn_id;
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d", conn_id);
#if CONFIG_BT_USER_COMMAND
            data_uart_print("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d\r\n", conn_id);
#endif
            //le_bond_passkey_input_confirm(conn_id, passkey, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_OOB_INPUT:
        {
            uint8_t oob_data[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            conn_id = gap_msg.msg_data.gap_bond_oob_input.conn_id;
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_OOB_INPUT: conn_id %d", conn_id);
            le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, oob_data);
            le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    default:
        APP_PRINT_ERROR1("ble_central_app_handle_gap_msg: unknown subtype %d", p_gap_msg->subtype);
        break;
    }
}
/** @} */ /* End of group CENTRAL_CLIENT_GAP_MSG */

/** @defgroup  CENTRAL_CLIENT_GAP_CALLBACK GAP Callback Event Handler
    * @brief Handle GAP callback event
    * @{
    */
/**
  * @brief Used to parse advertising data and scan response data
  * @param[in] scan_info point to scan information data.
  * @retval void
  */
void ble_central_app_parse_scan_info(T_LE_SCAN_INFO *scan_info)
{
    uint8_t buffer[32];
    uint8_t pos = 0;

    while (pos < scan_info->data_len)
    {
        /* Length of the AD structure. */
        uint8_t length = scan_info->data[pos++];
        uint8_t type;

        if ((length > 0x01) && ((pos + length) <= 31))
        {
            /* Copy the AD Data to buffer. */
            memcpy(buffer, scan_info->data + pos + 1, length - 1);
            /* AD Type, one octet. */
            type = scan_info->data[pos];

            APP_PRINT_TRACE2("ble_central_app_parse_scan_info: AD Structure Info: AD type 0x%x, AD Data Length %d", type,
                             length - 1);
//            BLE_PRINT("ble_central_app_parse_scan_info: AD Structure Info: AD type 0x%x, AD Data Length %d\n\r", type,
//                             length - 1);


            switch (type)
            {
            case GAP_ADTYPE_FLAGS:
                {
                    /* (flags & 0x01) -- LE Limited Discoverable Mode */
                    /* (flags & 0x02) -- LE General Discoverable Mode */
                    /* (flags & 0x04) -- BR/EDR Not Supported */
                    /* (flags & 0x08) -- Simultaneous LE and BR/EDR to Same Device Capable (Controller) */
                    /* (flags & 0x10) -- Simultaneous LE and BR/EDR to Same Device Capable (Host) */
                    uint8_t flags = scan_info->data[pos + 1];
                    APP_PRINT_INFO1("GAP_ADTYPE_FLAGS: 0x%x", flags);
					BLE_PRINT("GAP_ADTYPE_FLAGS: 0x%x\n\r", flags);

                }
                break;

            case GAP_ADTYPE_16BIT_MORE:
            case GAP_ADTYPE_16BIT_COMPLETE:
            case GAP_ADTYPE_SERVICES_LIST_16BIT:
                {
                    uint16_t *p_uuid = (uint16_t *)(buffer);
                    uint8_t i = length - 1;

                    while (i >= 2)
                    {
                        APP_PRINT_INFO1("GAP_ADTYPE_16BIT_XXX: 0x%x", *p_uuid++);
                        BLE_PRINT("GAP_ADTYPE_16BIT_XXX: 0x%x\n\r", *p_uuid++);

                        i -= 2;
                    }
                }
                break;

            case GAP_ADTYPE_32BIT_MORE:
            case GAP_ADTYPE_32BIT_COMPLETE:
                {
                    uint32_t *p_uuid = (uint32_t *)(buffer);
                    uint8_t    i     = length - 1;

                    while (i >= 4)
                    {
                        APP_PRINT_INFO1("GAP_ADTYPE_32BIT_XXX: 0x%x", *p_uuid++);
                        BLE_PRINT("GAP_ADTYPE_32BIT_XXX: 0x%x\n\r", *p_uuid++);


                        i -= 4;
                    }
                }
                break;

            case GAP_ADTYPE_128BIT_MORE:
            case GAP_ADTYPE_128BIT_COMPLETE:
            case GAP_ADTYPE_SERVICES_LIST_128BIT:
                {
                    uint32_t *p_uuid = (uint32_t *)(buffer);
                    APP_PRINT_INFO4("GAP_ADTYPE_128BIT_XXX: 0x%8.8x%8.8x%8.8x%8.8x",
                                    p_uuid[3], p_uuid[2], p_uuid[1], p_uuid[0]);
					BLE_PRINT("GAP_ADTYPE_128BIT_XXX: 0x%8.8x%8.8x%8.8x%8.8x\n\r",
									p_uuid[3], p_uuid[2], p_uuid[1], p_uuid[0]);

                }
                break;

            case GAP_ADTYPE_LOCAL_NAME_SHORT:
            case GAP_ADTYPE_LOCAL_NAME_COMPLETE:
                {
                    buffer[length - 1] = '\0';
                    APP_PRINT_INFO1("GAP_ADTYPE_LOCAL_NAME_XXX: %s", TRACE_STRING(buffer));
					BLE_PRINT("GAP_ADTYPE_LOCAL_NAME_XXX: %s\n\r", buffer);

                }
                break;

            case GAP_ADTYPE_POWER_LEVEL:
                {
                    APP_PRINT_INFO1("GAP_ADTYPE_POWER_LEVEL: 0x%x", scan_info->data[pos + 1]);
					BLE_PRINT("GAP_ADTYPE_POWER_LEVEL: 0x%x\n\r", scan_info->data[pos + 1]);

                }
                break;

            case GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE:
                {
                    uint16_t *p_min = (uint16_t *)(buffer);
                    uint16_t *p_max = p_min + 1;
                    APP_PRINT_INFO2("GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE: 0x%x - 0x%x", *p_min,
                                    *p_max);
					BLE_PRINT("GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE: 0x%x - 0x%x\n\r", *p_min,
									*p_max);

                }
                break;

            case GAP_ADTYPE_SERVICE_DATA:
                {
                    uint16_t *p_uuid = (uint16_t *)(buffer);
                    uint8_t data_len = length - 3;

                    APP_PRINT_INFO3("GAP_ADTYPE_SERVICE_DATA: UUID 0x%x, len %d, data %b", *p_uuid,
                                    data_len, TRACE_BINARY(data_len, &buffer[2]));
					BLE_PRINT("GAP_ADTYPE_SERVICE_DATA: UUID 0x%x, len %d, data %b\n\r", *p_uuid,
								data_len, TRACE_BINARY(data_len, &buffer[2]));

                }
                break;
            case GAP_ADTYPE_APPEARANCE:
                {
                    uint16_t *p_appearance = (uint16_t *)(buffer);
                    APP_PRINT_INFO1("GAP_ADTYPE_APPEARANCE: %d", *p_appearance);
					BLE_PRINT("GAP_ADTYPE_APPEARANCE: %d\n\r", *p_appearance);

                }
                break;

            case GAP_ADTYPE_MANUFACTURER_SPECIFIC:
                {
                    uint8_t data_len = length - 3;
                    uint16_t *p_company_id = (uint16_t *)(buffer);
                    APP_PRINT_INFO3("GAP_ADTYPE_MANUFACTURER_SPECIFIC: company_id 0x%x, len %d, data %b",
                                    *p_company_id, data_len, TRACE_BINARY(data_len, &buffer[2]));
					BLE_PRINT("GAP_ADTYPE_MANUFACTURER_SPECIFIC: company_id 0x%x, len %d, data %b\n\r",
									*p_company_id, data_len, TRACE_BINARY(data_len, &buffer[2]));

                }
                break;

            default:
                {
                    uint8_t i = 0;

                    for (i = 0; i < (length - 1); i++)
                    {
                        APP_PRINT_INFO1("  AD Data: Unhandled Data = 0x%x", scan_info->data[pos + i]);
//						BLE_PRINT("  AD Data: Unhandled Data = 0x%x\n\r", scan_info->data[pos + i]);

                    }
                }
                break;
            }
        }

        pos += length;
    }
}
/**
  * @brief Callback for gap le to notify app
  * @param[in] cb_type callback msy type @ref GAP_LE_MSG_Types.
  * @param[in] p_cb_data point to callback data @ref T_LE_CB_DATA.
  * @retval result @ref T_APP_RESULT
  */
T_APP_RESULT ble_central_app_gap_callback(uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;
	char adv_type[20];
	char remote_addr_type[10];

    switch (cb_type)
    {
    case GAP_MSG_LE_SCAN_INFO:
        APP_PRINT_INFO5("GAP_MSG_LE_SCAN_INFO:adv_type 0x%x, bd_addr %s, remote_addr_type %d, rssi %d, data_len %d",
                        p_data->p_le_scan_info->adv_type,
                        TRACE_BDADDR(p_data->p_le_scan_info->bd_addr),
                        p_data->p_le_scan_info->remote_addr_type,
                        p_data->p_le_scan_info->rssi,
                        p_data->p_le_scan_info->data_len);
        /* If you want to parse the scan info, please reference function ble_central_app_parse_scan_info. */
		sprintf(adv_type,"%s",(p_data->p_le_scan_info->adv_type ==GAP_ADV_EVT_TYPE_UNDIRECTED)? "CON_UNDIRECT":
							  (p_data->p_le_scan_info->adv_type ==GAP_ADV_EVT_TYPE_DIRECTED)? "CON_DIRECT":
							  (p_data->p_le_scan_info->adv_type ==GAP_ADV_EVT_TYPE_SCANNABLE)? "SCANABLE_UNDIRCT":
							  (p_data->p_le_scan_info->adv_type ==GAP_ADV_EVT_TYPE_NON_CONNECTABLE)? "NON_CONNECTABLE":
							  (p_data->p_le_scan_info->adv_type ==GAP_ADV_EVT_TYPE_SCAN_RSP)? "SCAN_RSP":"unknown");
		sprintf(remote_addr_type,"%s",(p_data->p_le_scan_info->remote_addr_type == GAP_REMOTE_ADDR_LE_PUBLIC)? "public":
							   (p_data->p_le_scan_info->remote_addr_type == GAP_REMOTE_ADDR_LE_RANDOM)? "random":"unknown");

		BLE_PRINT("ADVType\t\t\t| AddrType\t|%-17s\t|rssi\n\r","BT_Addr");
		BLE_PRINT("%-20s\t%-8s\t"BD_ADDR_FMT"\t%d\n\r",adv_type,remote_addr_type,BD_ADDR_ARG(p_data->p_le_scan_info->bd_addr),
												p_data->p_le_scan_info->rssi);

        ble_central_app_parse_scan_info(p_data->p_le_scan_info);
        break;

    case GAP_MSG_LE_CONN_UPDATE_IND:
        APP_PRINT_INFO5("GAP_MSG_LE_CONN_UPDATE_IND: conn_id %d, conn_interval_max 0x%x, conn_interval_min 0x%x, conn_latency 0x%x,supervision_timeout 0x%x",
                        p_data->p_le_conn_update_ind->conn_id,
                        p_data->p_le_conn_update_ind->conn_interval_max,
                        p_data->p_le_conn_update_ind->conn_interval_min,
                        p_data->p_le_conn_update_ind->conn_latency,
                        p_data->p_le_conn_update_ind->supervision_timeout);
        /* if reject the proposed connection parameter from peer device, use APP_RESULT_REJECT. */
        result = APP_RESULT_ACCEPT;
        break;

    default:
        APP_PRINT_ERROR1("ble_central_app_gap_callback: unhandled cb_type 0x%x", cb_type);
        break;
    }
    return result;
}
/** @} */ /* End of group CENTRAL_CLIENT_GAP_CALLBACK */

/** @defgroup  GCS_CLIIENT_CALLBACK GCS Client Callback Event Handler
    * @brief Handle profile client callback event
    * @{
    */
void ble_central_gcs_handle_discovery_result(uint8_t conn_id, T_GCS_DISCOVERY_RESULT discov_result)
{
    uint16_t i;
    T_GCS_DISCOV_RESULT *p_result_table;
    uint16_t    properties;
    switch (discov_result.discov_type)
    {
    case GCS_ALL_PRIMARY_SRV_DISCOV:
        APP_PRINT_INFO2("conn_id %d, GCS_ALL_PRIMARY_SRV_DISCOV, is_success %d",
                        conn_id, discov_result.is_success);
        for (i = 0; i < discov_result.result_num; i++)
        {
            p_result_table = &(discov_result.p_result_table[i]);
            switch (p_result_table->result_type)
            {
            case DISC_RESULT_ALL_SRV_UUID16:
                APP_PRINT_INFO4("ALL SRV UUID16[%d]: service range: 0x%x-0x%x, uuid16 0x%x",
                                i, p_result_table->result_data.srv_uuid16_disc_data.att_handle,
                                p_result_table->result_data.srv_uuid16_disc_data.end_group_handle,
                                p_result_table->result_data.srv_uuid16_disc_data.uuid16);
                BLE_PRINT("ALL SRV UUID16[%d]: service range: 0x%x-0x%x, uuid16 0x%x\n\r",
               				 i, p_result_table->result_data.srv_uuid16_disc_data.att_handle,
               				 p_result_table->result_data.srv_uuid16_disc_data.end_group_handle,
               				 p_result_table->result_data.srv_uuid16_disc_data.uuid16);
                break;
            case DISC_RESULT_ALL_SRV_UUID128:
                APP_PRINT_INFO4("ALL SRV UUID128[%d]: service range: 0x%x-0x%x, service=<%b>",
                                i, p_result_table->result_data.srv_uuid128_disc_data.att_handle,
                                p_result_table->result_data.srv_uuid128_disc_data.end_group_handle,
                                TRACE_BINARY(16, p_result_table->result_data.srv_uuid128_disc_data.uuid128));
                BLE_PRINT("ALL SRV UUID128[%d]: service range: 0x%x-0x%x, service=UUID_128_FORMAT\n\r",
                                i, p_result_table->result_data.srv_uuid128_disc_data.att_handle,
                                p_result_table->result_data.srv_uuid128_disc_data.end_group_handle,
                                UUID_128(p_result_table->result_data.srv_uuid128_disc_data.uuid128));

                break;

            default:
                APP_PRINT_ERROR0("Invalid Discovery Result Type!");
				BLE_PRINT("Invalid Discovery Result Type!\n\r");
                break;
            }
        }
        break;

    case GCS_BY_UUID128_SRV_DISCOV:
        APP_PRINT_INFO2("conn_id %d, GCS_BY_UUID128_SRV_DISCOV, is_success %d",
                        conn_id, discov_result.is_success);
        BLE_PRINT("conn_id %d, GCS_BY_UUID128_SRV_DISCOV, is_success %d\n\r",
                        conn_id, discov_result.is_success);

        for (i = 0; i < discov_result.result_num; i++)
        {
            p_result_table = &(discov_result.p_result_table[i]);
            switch (p_result_table->result_type)
            {
            case DISC_RESULT_SRV_DATA:
                APP_PRINT_INFO3("SRV DATA[%d]: service range: 0x%x-0x%x",
                                i, p_result_table->result_data.srv_disc_data.att_handle,
                                p_result_table->result_data.srv_disc_data.end_group_handle);
                BLE_PRINT("SRV DATA[%d]: service range: 0x%x-0x%x\n\r",
                                i, p_result_table->result_data.srv_disc_data.att_handle,
                                p_result_table->result_data.srv_disc_data.end_group_handle);

                break;

            default:
                APP_PRINT_ERROR0("Invalid Discovery Result Type!");
                BLE_PRINT("Invalid Discovery Result Type!\n\r");
                break;
            }
        }
        break;

    case GCS_BY_UUID_SRV_DISCOV:
        APP_PRINT_INFO2("conn_id %d, GCS_BY_UUID_SRV_DISCOV, is_success %d",
                        conn_id, discov_result.is_success);
        BLE_PRINT("conn_id %d, GCS_BY_UUID_SRV_DISCOV, is_success %d\n\r",
                        conn_id, discov_result.is_success);

        for (i = 0; i < discov_result.result_num; i++)
        {
            p_result_table = &(discov_result.p_result_table[i]);
            switch (p_result_table->result_type)
            {
            case DISC_RESULT_SRV_DATA:
                APP_PRINT_INFO3("SRV DATA[%d]: service range: 0x%x-0x%x",
                                i, p_result_table->result_data.srv_disc_data.att_handle,
                                p_result_table->result_data.srv_disc_data.end_group_handle);
                BLE_PRINT("SRV DATA[%d]: service range: 0x%x-0x%x\n\r",
                                i, p_result_table->result_data.srv_disc_data.att_handle,
                                p_result_table->result_data.srv_disc_data.end_group_handle);

                break;

            default:
                APP_PRINT_ERROR0("Invalid Discovery Result Type!");
                BLE_PRINT("Invalid Discovery Result Type!\n\r");
                break;
            }
        }
        break;

    case GCS_ALL_CHAR_DISCOV:
        APP_PRINT_INFO2("conn_id %d, GCS_ALL_CHAR_DISCOV, is_success %d",
                        conn_id, discov_result.is_success);
        BLE_PRINT("conn_id %d, GCS_ALL_CHAR_DISCOV, is_success %d\n\r",
                        conn_id, discov_result.is_success);

        for (i = 0; i < discov_result.result_num; i++)
        {
            p_result_table = &(discov_result.p_result_table[i]);
            switch (p_result_table->result_type)
            {
            case DISC_RESULT_CHAR_UUID16:
                properties = p_result_table->result_data.char_uuid16_disc_data.properties;
                APP_PRINT_INFO5("CHAR UUID16[%d]: decl_handle 0x%x, properties 0x%x, value_handle 0x%x, uuid16 0x%x",
                                i, p_result_table->result_data.char_uuid16_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid16_disc_data.properties,
                                p_result_table->result_data.char_uuid16_disc_data.value_handle,
                                p_result_table->result_data.char_uuid16_disc_data.uuid16);
                APP_PRINT_INFO5("properties:indicate %d, read %d, write cmd %d, write %d, notify %d\r\n",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY);
                BLE_PRINT("CHAR UUID16[%d]: decl_handle 0x%x, properties 0x%x, value_handle 0x%x, uuid16 0x%x\n\r",
                                i, p_result_table->result_data.char_uuid16_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid16_disc_data.properties,
                                p_result_table->result_data.char_uuid16_disc_data.value_handle,
                                p_result_table->result_data.char_uuid16_disc_data.uuid16);
                BLE_PRINT("properties:indicate %d, read %d, write cmd %d, write %d, notify %d\n\r",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY);


                break;

            case DISC_RESULT_CHAR_UUID128:
                properties = p_result_table->result_data.char_uuid128_disc_data.properties;
                APP_PRINT_INFO5("CHAR UUID128[%d]:  decl hndl=0x%x, prop=0x%x, value hndl=0x%x, uuid128=<%b>",
                                i, p_result_table->result_data.char_uuid128_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid128_disc_data.properties,
                                p_result_table->result_data.char_uuid128_disc_data.value_handle,
                                TRACE_BINARY(16, p_result_table->result_data.char_uuid128_disc_data.uuid128));
                APP_PRINT_INFO5("properties:indicate %d, read %d, write cmd %d, write %d, notify %d",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY
                               );
                BLE_PRINT("CHAR UUID128[%d]:  decl hndl=0x%x, prop=0x%x, value hndl=0x%x, uuid128=UUID_128_FORMAT\n\r",
                                i, p_result_table->result_data.char_uuid128_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid128_disc_data.properties,
                                p_result_table->result_data.char_uuid128_disc_data.value_handle,
                                UUID_128(p_result_table->result_data.char_uuid128_disc_data.uuid128));
                BLE_PRINT("properties:indicate %d, read %d, write cmd %d, write %d, notify %d\n\r",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY
                               );

                break;
            default:
                APP_PRINT_ERROR0("Invalid Discovery Result Type!");
                BLE_PRINT("Invalid Discovery Result Type!\n\r");
                break;
            }
        }
        break;

    case GCS_BY_UUID_CHAR_DISCOV:
        APP_PRINT_INFO2("conn_id %d, GCS_BY_UUID_CHAR_DISCOV, is_success %d",
                        conn_id, discov_result.is_success);
        BLE_PRINT("conn_id %d, GCS_BY_UUID_CHAR_DISCOV, is_success %d\n\r",
                        conn_id, discov_result.is_success);

        for (i = 0; i < discov_result.result_num; i++)
        {
            p_result_table = &(discov_result.p_result_table[i]);
            switch (p_result_table->result_type)
            {
            case DISC_RESULT_BY_UUID16_CHAR:
                properties = p_result_table->result_data.char_uuid16_disc_data.properties;
                APP_PRINT_INFO5("UUID16 CHAR[%d]: Characteristics by uuid16, decl hndl=0x%x, prop=0x%x, value hndl=0x%x, uuid16=<0x%x>",
                                i, p_result_table->result_data.char_uuid16_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid16_disc_data.properties,
                                p_result_table->result_data.char_uuid16_disc_data.value_handle,
                                p_result_table->result_data.char_uuid16_disc_data.uuid16);
                APP_PRINT_INFO5("properties:indicate %d, read %d, write cmd %d, write %d, notify %d",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY
                               );
                BLE_PRINT("UUID16 CHAR[%d]: Characteristics by uuid16, decl hndl=0x%x, prop=0x%x, value hndl=0x%x, uuid16=<0x%x>\n\r",
                                i, p_result_table->result_data.char_uuid16_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid16_disc_data.properties,
                                p_result_table->result_data.char_uuid16_disc_data.value_handle,
                                p_result_table->result_data.char_uuid16_disc_data.uuid16);
                BLE_PRINT("properties:indicate %d, read %d, write cmd %d, write %d, notify %d\n\r",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY
                               );

                break;

            default:
                APP_PRINT_ERROR0("Invalid Discovery Result Type!");
                BLE_PRINT("Invalid Discovery Result Type!");
                break;
            }
        }
        break;

    case GCS_BY_UUID128_CHAR_DISCOV:
        APP_PRINT_INFO2("conn_id %d, GCS_BY_UUID128_CHAR_DISCOV, is_success %d",
                        conn_id, discov_result.is_success);
        BLE_PRINT("conn_id %d, GCS_BY_UUID128_CHAR_DISCOV, is_success %d\n\r",
                        conn_id, discov_result.is_success);

        for (i = 0; i < discov_result.result_num; i++)
        {
            p_result_table = &(discov_result.p_result_table[i]);
            switch (p_result_table->result_type)
            {
            case DISC_RESULT_BY_UUID128_CHAR:
                properties = p_result_table->result_data.char_uuid128_disc_data.properties;
                APP_PRINT_INFO5("UUID128 CHAR[%d]: Characteristics by uuid128, decl hndl=0x%x, prop=0x%x, value hndl=0x%x, uuid128=<%b>",
                                i, p_result_table->result_data.char_uuid128_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid128_disc_data.properties,
                                p_result_table->result_data.char_uuid128_disc_data.value_handle,
                                TRACE_BINARY(16, p_result_table->result_data.char_uuid128_disc_data.uuid128));
                APP_PRINT_INFO5("properties:indicate %d, read %d, write cmd %d, write %d, notify %d",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY
                               );
                BLE_PRINT("UUID128 CHAR[%d]: Characteristics by uuid128, decl hndl=0x%x, prop=0x%x, value hndl=0x%x, uuid128=UUID_128_FORMAT\n\r",
                                i, p_result_table->result_data.char_uuid128_disc_data.decl_handle,
                                p_result_table->result_data.char_uuid128_disc_data.properties,
                                p_result_table->result_data.char_uuid128_disc_data.value_handle,
                                UUID_128(p_result_table->result_data.char_uuid128_disc_data.uuid128));
                BLE_PRINT("properties:indicate %d, read %d, write cmd %d, write %d, notify %d\n\r",
                                properties & GATT_CHAR_PROP_INDICATE,
                                properties & GATT_CHAR_PROP_READ,
                                properties & GATT_CHAR_PROP_WRITE_NO_RSP,
                                properties & GATT_CHAR_PROP_WRITE,
                                properties & GATT_CHAR_PROP_NOTIFY
                               );

                break;

            default:
                APP_PRINT_ERROR0("Invalid Discovery Result Type!");
                BLE_PRINT("Invalid Discovery Result Type!\n\r");
                break;
            }
        }
        break;

    case GCS_ALL_CHAR_DESC_DISCOV:
        APP_PRINT_INFO2("conn_id %d, GCS_ALL_CHAR_DESC_DISCOV, is_success %d\r\n",
                        conn_id, discov_result.is_success);
        BLE_PRINT("conn_id %d, GCS_ALL_CHAR_DESC_DISCOV, is_success %d\n\r",
                        conn_id, discov_result.is_success);
        for (i = 0; i < discov_result.result_num; i++)
        {
            p_result_table = &(discov_result.p_result_table[i]);
            switch (p_result_table->result_type)
            {
            case DISC_RESULT_CHAR_DESC_UUID16:
                APP_PRINT_INFO3("DESC UUID16[%d]: Descriptors handle=0x%x, uuid16=<0x%x>",
                                i, p_result_table->result_data.char_desc_uuid16_disc_data.handle,
                                p_result_table->result_data.char_desc_uuid16_disc_data.uuid16);
                BLE_PRINT("DESC UUID16[%d]: Descriptors handle=0x%x, uuid16=<0x%x>",
                                i, p_result_table->result_data.char_desc_uuid16_disc_data.handle,
                                p_result_table->result_data.char_desc_uuid16_disc_data.uuid16);

                break;
            case DISC_RESULT_CHAR_DESC_UUID128:
                APP_PRINT_INFO3("DESC UUID128[%d]: Descriptors handle=0x%x, uuid128=UUID_128_FORMAT",
                                i, p_result_table->result_data.char_desc_uuid128_disc_data.handle,
                                TRACE_BINARY(16, p_result_table->result_data.char_desc_uuid128_disc_data.uuid128));
                BLE_PRINT("DESC UUID128[%d]: Descriptors handle=0x%x, uuid128=<%b>\n\r",
                                i, p_result_table->result_data.char_desc_uuid128_disc_data.handle,
                                UUID_128(p_result_table->result_data.char_desc_uuid128_disc_data.uuid128));
                break;

            default:
                APP_PRINT_ERROR0("Invalid Discovery Result Type!");
                BLE_PRINT("Invalid Discovery Result Type!\n\r");
                break;
            }
        }
        break;

    default:
        APP_PRINT_ERROR2("Invalid disc type: conn_id %d, discov_type %d",
                         conn_id, discov_result.discov_type);
        BLE_PRINT("Invalid disc type: conn_id %d, discov_type %d\n\r",
                         conn_id, discov_result.discov_type);
        break;
    }
}
/**
 * @brief  Callback will be called when data sent from gcs client.
 * @param  client_id the ID distinguish which module sent the data.
 * @param  conn_id connection ID.
 * @param  p_data  pointer to data.
 * @retval   result @ref T_APP_RESULT
 */
T_APP_RESULT ble_central_gcs_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    APP_PRINT_INFO2("ble_central_gcs_client_callback: client_id %d, conn_id %d",
                    client_id, conn_id);
    if (client_id == ble_central_gcs_client_id)
    {
        T_GCS_CLIENT_CB_DATA *p_gcs_cb_data = (T_GCS_CLIENT_CB_DATA *)p_data;
        switch (p_gcs_cb_data->cb_type)
        {
        case GCS_CLIENT_CB_TYPE_DISC_RESULT:
            ble_central_gcs_handle_discovery_result(conn_id, p_gcs_cb_data->cb_content.discov_result);
            break;
        case GCS_CLIENT_CB_TYPE_READ_RESULT:
            APP_PRINT_INFO3("READ RESULT: cause 0x%x, handle 0x%x,  value_len %d",
                            p_gcs_cb_data->cb_content.read_result.cause,
                            p_gcs_cb_data->cb_content.read_result.handle,
                            p_gcs_cb_data->cb_content.read_result.value_size);
            BLE_PRINT("READ RESULT: cause 0x%x, handle 0x%x,  value_len %d\n\r",
                            p_gcs_cb_data->cb_content.read_result.cause,
                            p_gcs_cb_data->cb_content.read_result.handle,
                            p_gcs_cb_data->cb_content.read_result.value_size);

            if (p_gcs_cb_data->cb_content.read_result.cause == GAP_SUCCESS)
            {
                APP_PRINT_INFO1("READ VALUE: %b",
                                TRACE_BINARY(p_gcs_cb_data->cb_content.read_result.value_size,
                                             p_gcs_cb_data->cb_content.read_result.p_value));

				BLE_PRINT("REAR VALUE:");
				for(int i=0; i< p_gcs_cb_data->cb_content.read_result.value_size; i++)
					BLE_PRINT("0x%2X", *(p_gcs_cb_data->cb_content.read_result.p_value + i));
				BLE_PRINT("\n\r");

            }
            break;
        case GCS_CLIENT_CB_TYPE_WRITE_RESULT:
            APP_PRINT_INFO3("WRITE RESULT: cause 0x%x ,handle 0x%x, type %d",
                            p_gcs_cb_data->cb_content.write_result.cause,
                            p_gcs_cb_data->cb_content.write_result.handle,
                            p_gcs_cb_data->cb_content.write_result.type);
            break;
        case GCS_CLIENT_CB_TYPE_NOTIF_IND:
            if (p_gcs_cb_data->cb_content.notif_ind.notify == false)
            {
                APP_PRINT_INFO2("INDICATION: handle 0x%x, value_size %d",
                                p_gcs_cb_data->cb_content.notif_ind.handle,
                                p_gcs_cb_data->cb_content.notif_ind.value_size);
                APP_PRINT_INFO1("INDICATION VALUE: %b",
                                TRACE_BINARY(p_gcs_cb_data->cb_content.read_result.value_size,
                                             p_gcs_cb_data->cb_content.read_result.p_value));
            }
            else
            {
                APP_PRINT_INFO2("NOTIFICATION: handle 0x%x, value_size %d",
                                p_gcs_cb_data->cb_content.notif_ind.handle,
                                p_gcs_cb_data->cb_content.notif_ind.value_size);
                APP_PRINT_INFO1("NOTIFICATION VALUE: %b",
                                TRACE_BINARY(p_gcs_cb_data->cb_content.notif_ind.value_size,
                                             p_gcs_cb_data->cb_content.notif_ind.p_value));
            }
            break;
        default:
            break;
        }
    }

    return result;
}

/** @} */ /* End of group GCS_CLIIENT_CALLBACK */
/** @} */ /* End of group CENTRAL_CLIENT_APP */

