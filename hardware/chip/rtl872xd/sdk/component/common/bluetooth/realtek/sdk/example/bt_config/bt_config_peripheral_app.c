/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <os_msg.h>
#include <os_task.h>
#include <gap.h>
#include <gap_le.h>
#include <gap_msg.h>
#include <app_msg.h>
#include "FreeRTOS.h"
#include "wifi_conf.h"
#include "wifi_util.h"
#include "wifi_structures.h"
#include <lwip_netconf.h>
#include "dhcp/dhcps.h"
#include <trace_app.h>
#include <string.h>
#include <gap.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include <profile_server.h>
#include <gap_msg.h>
#include <bas.h>
#include <app_msg.h>
#include <gap_conn_le.h>
#include "bt_config_service.h"
#include "bt_config_peripheral_app.h"
#include "bt_config_wifi.h"


T_SERVER_ID bt_config_srv_id; /**< BT Config Wifi service id*/
T_GAP_DEV_STATE bt_config_gap_dev_state = {0, 0, 0, 0, 0};                 /**< GAP device state */
T_GAP_CONN_STATE bt_config_gap_conn_state = GAP_CONN_STATE_DISCONNECTED; /**< GAP connection state */
uint8_t bt_config_conn_id = 0;

/*============================================================================*
 *                              Functions
 *============================================================================*/
void bt_config_app_handle_gap_msg(T_IO_MSG  *p_gap_msg);

/**
 * @brief    All the application messages are pre-handled in this function
 * @note     All the IO MSGs are sent to this function, then the event handling
 *           function shall be called according to the MSG type.
 * @param[in] io_msg  IO message data
 * @return   void
 */
void bt_config_app_handle_io_msg(T_IO_MSG  io_msg)
{
    uint16_t msg_type = io_msg.type;

    switch (msg_type)
    {
    case IO_MSG_TYPE_BT_STATUS:
        {
            bt_config_app_handle_gap_msg(&io_msg);
        }
        break;
    default:
        break;
    }
}

/** @defgroup  PERIPH_DEMO_MAIN Peripheral Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */

/*============================================================================*
 *                              Variables
 *============================================================================*/

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static uint8_t adv_data[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Service */
    0x03,             /* length */
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(GATT_UUID_BT_CONFIG_PROFILE),
    HI_WORD(GATT_UUID_BT_CONFIG_PROFILE),
    /* Local name */
    0x0D,             /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'A', 'm', 'e', 'b', 'a', '_', 'x', 'x', 'y', 'y', 'z', 'z',
};

/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
  * @brief  Initialize peripheral and gap bond manager related parameters
  * @return void
  */
void bt_config_app_set_adv_data(void)
{
	/* Modify Device name according to BD addr*/
	uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "Ameba_xxyyzz";
	uint8_t bt_addr[6];
	gap_get_param(GAP_PARAM_BD_ADDR, bt_addr);
	
	sprintf((char *)device_name,"Ameba_%02X%02X%02X",bt_addr[2],bt_addr[1],bt_addr[0]);
	memcpy(adv_data+9,device_name,strlen((char const*)device_name));
	//printf("Device name: \"%s\" (BD Address %02X:%02X:%02X:%02X:%02X:%02X) \n\r",
	//		device_name,bt_addr[5],bt_addr[4],bt_addr[3],bt_addr[2],bt_addr[1],bt_addr[0]);

	le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
	le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(adv_data), (void *)adv_data);
}

/**
 * @brief    Handle msg GAP_MSG_LE_DEV_STATE_CHANGE
 * @note     All the gap device state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] new_state  New gap device state
 * @param[in] cause GAP device state change cause
 * @return   void
 */
extern void wifi_btcoex_set_bt_on(void);
extern void set_bt_config_state(uint8_t state);
void bt_config_app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    APP_PRINT_INFO3("bt_config_app_handle_dev_state_evt: init state %d, adv state %d, cause 0x%x",
                    new_state.gap_init_state, new_state.gap_adv_state, cause);
    if (bt_config_gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            APP_PRINT_INFO0("GAP stack ready");
			
            /*stack ready*/
			wifi_btcoex_set_bt_on();
			bt_config_app_set_adv_data();
            le_adv_start();
			set_bt_config_state(BC_DEV_IDLE); // BT Config Ready
			BC_printf("BT Config Wifi ready\n\r");
        }
    }

    if (bt_config_gap_dev_state.gap_adv_state != new_state.gap_adv_state)
    {
        if (new_state.gap_adv_state == GAP_ADV_STATE_IDLE)
        {
            if (new_state.gap_adv_sub_state == GAP_ADV_TO_IDLE_CAUSE_CONN)
            {
                APP_PRINT_INFO0("GAP adv stoped: because connection created");
            }
            else
            {
                APP_PRINT_INFO0("GAP adv stopped");
				BC_printf("ADV stopped\n\r");
            }
        }
        else if (new_state.gap_adv_state == GAP_ADV_STATE_ADVERTISING)
        {
            APP_PRINT_INFO0("GAP adv start");
			BC_printf("ADV started\n\r");
        }
    }

    bt_config_gap_dev_state = new_state;
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
void bt_config_app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
{
    APP_PRINT_INFO4("bt_config_app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                    conn_id, bt_config_gap_conn_state, new_state, disc_cause);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
            {
                APP_PRINT_ERROR1("bt_config_app_handle_conn_state_evt: connection lost cause 0x%x", disc_cause);
            }
			bt_config_conn_id = 0;
			BC_printf("Bluetooth Connection Disconnected\n\r");
			if (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS) {		
				bt_config_app_set_adv_data();
				le_adv_start();
				set_bt_config_state(BC_DEV_IDLE); // BT Config Ready
			}
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            uint16_t conn_interval;
            uint16_t conn_latency;
            uint16_t conn_supervision_timeout;
            uint8_t  remote_bd[6];
            T_GAP_REMOTE_ADDR_TYPE remote_bd_type;

            T_GAP_CAUSE cause; 

            uint16_t conn_interval_min = 12; // 15ms
            uint16_t conn_interval_max = 24; // 30ms
            uint16_t supervision_timeout = 500; 
            uint16_t ce_length_min = 2 * (conn_interval_min - 1); 
            uint16_t ce_length_max = 2 * (conn_interval_max - 1); 

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
            le_get_conn_addr(conn_id, remote_bd,  (void *)&remote_bd_type);
			
            conn_latency = 0;
            cause = le_update_conn_param(conn_id, 
                                    conn_interval_min, 
                                    conn_interval_max, 
                                    conn_latency, 
                                    supervision_timeout, 
                                    ce_length_min, 
                                    ce_length_max 
                                   ); 
            if (cause == GAP_CAUSE_NON_CONN) {
                BC_printf("No Bluetooth Connection\n\r");
                break;
            }
			//update_connection_time
            APP_PRINT_INFO5("GAP_CONN_STATE_CONNECTED:remote_bd %s, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                            TRACE_BDADDR(remote_bd), remote_bd_type,
                            conn_interval, conn_latency, conn_supervision_timeout);
			BC_printf("Bluetooth Connection Established\n\r");
			bt_config_conn_id = conn_id;
			set_bt_config_state(BC_DEV_BT_CONNECTED); // BT Config Bluetooth Connected
        }
        break;

    default:
        break;
    }
    bt_config_gap_conn_state = new_state;
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
void bt_config_app_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t cause)
{
    APP_PRINT_INFO2("bt_config_app_handle_authen_state_evt:conn_id %d, cause 0x%x", conn_id, cause);

    switch (new_state)
    {
    case GAP_AUTHEN_STATE_STARTED:
        {
            APP_PRINT_INFO0("bt_config_app_handle_authen_state_evt: GAP_AUTHEN_STATE_STARTED");
        }
        break;

    case GAP_AUTHEN_STATE_COMPLETE:
        {
            if (cause == GAP_SUCCESS)
            {
                APP_PRINT_INFO0("bt_config_app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair success");
            }
            else
            {
                APP_PRINT_INFO0("bt_config_app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair failed");
            }
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("bt_config_app_handle_authen_state_evt: unknown newstate %d", new_state);
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
void bt_config_app_handle_conn_mtu_info_evt(uint8_t conn_id, uint16_t mtu_size)
{
    APP_PRINT_INFO2("bt_config_app_handle_conn_mtu_info_evt: conn_id %d, mtu_size %d", conn_id, mtu_size);
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_PARAM_UPDATE
 * @note     All the connection parameter update change  events are pre-handled in this function.
 * @param[in] conn_id Connection ID
 * @param[in] status  New update state
 * @param[in] cause Use this cause when status is GAP_CONN_PARAM_UPDATE_STATUS_FAIL
 * @return   void
 */
void bt_config_app_handle_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
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
            APP_PRINT_INFO3("bt_config_app_handle_conn_param_update_evt update success:conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x",
                            conn_interval, conn_slave_latency, conn_supervision_timeout);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
        {
            APP_PRINT_ERROR1("bt_config_app_handle_conn_param_update_evt update failed: cause 0x%x", cause);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
        {
            APP_PRINT_INFO0("bt_config_app_handle_conn_param_update_evt update pending.");
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
void bt_config_app_handle_gap_msg(T_IO_MSG *p_gap_msg)
{
    T_LE_GAP_MSG gap_msg;
    uint8_t conn_id;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));
	//printf("bt_config_app_handle_gap_msg: subtype %d\n\r", p_gap_msg->subtype);

    APP_PRINT_TRACE1("bt_config_app_handle_gap_msg: subtype %d", p_gap_msg->subtype);
    switch (p_gap_msg->subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
        {
            bt_config_app_handle_dev_state_evt(gap_msg.msg_data.gap_dev_state_change.new_state,
                                     gap_msg.msg_data.gap_dev_state_change.cause);
        }
        break;

    case GAP_MSG_LE_CONN_STATE_CHANGE:
        {
            bt_config_app_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                      (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                      gap_msg.msg_data.gap_conn_state_change.disc_cause);
        }
        break;

    case GAP_MSG_LE_CONN_MTU_INFO:
        {
            bt_config_app_handle_conn_mtu_info_evt(gap_msg.msg_data.gap_conn_mtu_info.conn_id,
                                         gap_msg.msg_data.gap_conn_mtu_info.mtu_size);
        }
        break;

    case GAP_MSG_LE_CONN_PARAM_UPDATE:
        {
            bt_config_app_handle_conn_param_update_evt(gap_msg.msg_data.gap_conn_param_update.conn_id,
                                             gap_msg.msg_data.gap_conn_param_update.status,
                                             gap_msg.msg_data.gap_conn_param_update.cause);
        }
        break;

    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
        {
            bt_config_app_handle_authen_state_evt(gap_msg.msg_data.gap_authen_state.conn_id,
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
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %d", display_value);
            le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %d", display_value);
            le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
        {
            uint32_t passkey = 888888;
            conn_id = gap_msg.msg_data.gap_bond_passkey_input.conn_id;
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d", conn_id);
            le_bond_passkey_input_confirm(conn_id, passkey, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    default:
        APP_PRINT_ERROR1("bt_config_app_handle_gap_msg: unknown subtype %d", p_gap_msg->subtype);
        break;
    }
}
/** @} */ /* End of group PERIPH_GAP_MSG */

/** @defgroup  PERIPH_GAP_CALLBACK GAP Callback Event Handler
    * @brief Handle GAP callback event
    * @{
    */
/**
  * @brief Callback for gap le to notify app
  * @param[in] cb_type callback msy type @ref GAP_LE_MSG_Types.
  * @param[in] p_cb_data point to callback data @ref T_LE_CB_DATA.
  * @retval result @ref T_APP_RESULT
  */
T_APP_RESULT bt_config_app_gap_callback(uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;

    switch (cb_type)
    {
#if defined(CONFIG_PLATFORM_8721D)
    case GAP_MSG_LE_DATA_LEN_CHANGE_INFO:
	//printf("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id %d, tx octets 0x%x, max_tx_time 0x%x", p_data->p_le_data_len_change_info->conn_id, p_data->p_le_data_len_change_info->max_tx_octets,  p_data->p_le_data_len_change_info->max_tx_time);
        APP_PRINT_INFO3("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id %d, tx octets 0x%x, max_tx_time 0x%x",
                        p_data->p_le_data_len_change_info->conn_id,
                        p_data->p_le_data_len_change_info->max_tx_octets,
                        p_data->p_le_data_len_change_info->max_tx_time);
        break;
#endif
    case GAP_MSG_LE_MODIFY_WHITE_LIST:
        APP_PRINT_INFO2("GAP_MSG_LE_MODIFY_WHITE_LIST: operation %d, cause 0x%x",
                        p_data->p_le_modify_white_list_rsp->operation,
                        p_data->p_le_modify_white_list_rsp->cause);
        break;

    default:
        APP_PRINT_ERROR1("bt_config_app_gap_callback: unhandled cb_type 0x%x", cb_type);
        break;
    }
    return result;
}
/** @} */ /* End of group PERIPH_GAP_CALLBACK */

/** @defgroup  PERIPH_SEVER_CALLBACK Profile Server Callback Event Handler
    * @brief Handle profile server callback event
    * @{
    */
/**
    * @brief    All the BT Profile service callback events are handled in this function
    * @note     Then the event handling function shall be called according to the
    *           service_id
    * @param    service_id  Profile service ID
    * @param    p_data      Pointer to callback data
    * @return   T_APP_RESULT, which indicates the function call is successful or not
    * @retval   APP_RESULT_SUCCESS  Function run successfully
    * @retval   others              Function run failed, and return number indicates the reason
    */
T_APP_RESULT bt_config_app_profile_callback(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        T_SERVER_APP_CB_DATA *p_param = (T_SERVER_APP_CB_DATA *)p_data;
        switch (p_param->eventId)
        {
        case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
            APP_PRINT_INFO1("PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                            p_param->event_data.service_reg_result);
            break;
        default:
            break;
        }
    }
    else  if (service_id == bt_config_srv_id)
    {
        TBTCONFIG_CALLBACK_DATA *p_simp_cb_data = (TBTCONFIG_CALLBACK_DATA *)p_data;
        switch (p_simp_cb_data->msg_type)
        {
        case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
            {
				uint8_t *read_buf = NULL;
				uint32_t read_buf_len = 0;
				// Customized command:
				// Handle your own Read Request here
				// Prepare your own read_buf & read_buf_len
				// Otherwise, use BC_handle_read_request to get read response from BT Config
				BC_handle_read_request(&read_buf, &read_buf_len);
				if(read_buf != NULL) {
					bt_config_service_set_parameter(BTCONFIG_SERVICE_PARAM_V1_READ_CHAR_VAL, read_buf_len, read_buf);
				}
            }
            break;
        case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
            {
                switch (p_simp_cb_data->msg_data.write.opcode)
                {
                case BTCONFIG_WRITE_V1:
                    {
						// Customized command:
						// Parse data first. (p_simp_cb_data->msg_data.write.p_value, p_simp_cb_data->msg_data.write.len)
						// If it's a customized command, handle it here (call customized function to do specific actions)
						// Otherwise, use BC_send_cmd to send data (BT Config command) to BT Config
						BC_send_cmd( p_simp_cb_data->msg_data.write.p_value,  p_simp_cb_data->msg_data.write.len);
                    }
                    break;
                default:
                    break;
                }
            }
            break;

        default:
            break;
        }
    }

    return app_result;
}
