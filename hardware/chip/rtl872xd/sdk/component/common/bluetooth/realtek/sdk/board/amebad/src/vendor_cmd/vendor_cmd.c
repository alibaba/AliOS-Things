#include <gap.h>
#include <bt_types.h>
#include <string.h>
#include <trace_app.h>
#include "vendor_cmd.h"
#include "vendor_cmd_bt.h"
#include "rtk_coex.h"
#include <gap_conn_le.h> 

P_FUN_GAP_APP_CB ext_app_cb = NULL;

bool mailbox_to_bt(uint8_t *data, uint8_t len)
{
	T_GAP_DEV_STATE new_state;
	le_get_gap_param(GAP_PARAM_DEV_STATE , &new_state );   
	if (new_state.gap_init_state != GAP_INIT_STATE_STACK_READY) {
		APP_PRINT_ERROR1("mailbox_to_bt: gap_init_state: 0x%x", new_state.gap_init_state);
		return false;
	}
    if(gap_vendor_cmd_req(HCI_VENDOR_MAILBOX_CMD, len, data) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        APP_PRINT_ERROR0("mailbox_to_bt: failed");
        return false;
    }
}

bool mailbox_to_bt_set_profile_report(uint8_t *data, uint8_t len)
{
	T_GAP_DEV_STATE new_state;
	le_get_gap_param(GAP_PARAM_DEV_STATE , &new_state );   
	if (new_state.gap_init_state != GAP_INIT_STATE_STACK_READY) {
		APP_PRINT_INFO1("mailbox_to_bt_set_profile_report: gap_init_state: 0x%x", new_state.gap_init_state);
		return false;
	}
    if(gap_vendor_cmd_req(HCI_VENDOR_SET_PROFILE_REPORT_COMMAND, len, data) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        APP_PRINT_ERROR0("mailbox_to_bt_set_profile_report: failed");
        return false;
    }
}

#if BT_VENDOR_CMD_ONE_SHOT_SUPPORT
T_GAP_CAUSE le_vendor_one_shot_adv(void)
{
    uint8_t len = 1;
    uint8_t param[1];
    param[0] = HCI_EXT_SUB_ONE_SHOT_ADV;

    if (gap_vendor_cmd_req(HCI_LE_VENDOR_EXTENSION_FEATURE2, len, param) == GAP_CAUSE_SUCCESS)
    {
        return GAP_CAUSE_SUCCESS;
    }
    return GAP_CAUSE_SEND_REQ_FAILED;
}
#endif

#if BT_VENDOR_CMD_ADV_TX_POWER_SUPPORT
T_GAP_CAUSE le_adv_set_tx_power(uint8_t option, uint8_t tx_gain)
{
    uint8_t param[3];
    param[0] = HCI_EXT_SUB_SET_ADV_TX_POWER;
    param[1] = option;
    param[2] = tx_gain;

    if (gap_vendor_cmd_req(HCI_LE_VENDOR_EXTENSION_FEATURE, 3, param) == GAP_CAUSE_SUCCESS)
    {
        return GAP_CAUSE_SUCCESS;
    }
    return GAP_CAUSE_SEND_REQ_FAILED;
}
#endif

#if BT_VENDOR_CMD_CONN_TX_POWER_SUPPORT
T_GAP_CAUSE le_set_conn_tx_power(uint8_t conn_id, bool reset, uint8_t tx_power)
{
    uint16_t conn_handle;
    if (le_get_conn_param(GAP_PARAM_CONN_HANDLE, &conn_handle, conn_id) == GAP_CAUSE_SUCCESS)
    {
        uint8_t param[5];
        param[0] = HCI_EXT_SUB_SET_LINK_TX_POW;
        param[1] = conn_handle & 0xFF;
        param[2] = (conn_handle >> 8) & 0xFF;
        param[3] = reset;
        param[4] = tx_power;

        if (gap_vendor_cmd_req(HCI_LE_VENDOR_EXTENSION_FEATURE, 5, param) == GAP_CAUSE_SUCCESS)
        {
            return GAP_CAUSE_SUCCESS;
        }
        return GAP_CAUSE_SEND_REQ_FAILED;
    }
    else
    {
        return GAP_CAUSE_NON_CONN;
    }
}
#endif

#if BT_VENDOR_CMD_SLAVE_LATENCY_SUPPORT
T_GAP_CAUSE le_disable_slave_latency(uint8_t conn_id, bool disable)
{
    uint16_t conn_handle;

    if (le_get_conn_param(GAP_PARAM_CONN_HANDLE, &conn_handle, conn_id) == GAP_CAUSE_SUCCESS) {
        uint8_t param[4];
        param[0] = HCI_EXT_SUB_DISABLE_LATENCY;
        param[1] = conn_handle & 0xFF;
        param[2] = (conn_handle >> 8) & 0xFF;

        if (disable) {
            param[3] = 0;
        } else {
            param[3] = 1;
        }

        if (gap_vendor_cmd_req(HCI_LE_VENDOR_EXTENSION_FEATURE, 4, param) == GAP_CAUSE_SUCCESS) {
            return GAP_CAUSE_SUCCESS;
        }
        return GAP_CAUSE_SEND_REQ_FAILED;
    } else {
        return GAP_CAUSE_NON_CONN;
    }
}
#endif

/**
 * @brief Callback for gap common module to notify app
 * @param[in] cb_type callback msy type @ref GAP_COMMON_MSG_TYPE.
 * @param[in] p_cb_data point to callback data @ref T_GAP_CB_DATA.
 * @retval void
 * example: 
 * uint8_t data[] ={0x33 , 0x01, 0x02,0x03, 0x04, 0x05, 0x06, 0x07};
 * mailbox_to_bt(data, sizeof(data));
 * mailbox_to_bt_set_profile_report(NULL, 0);
 */
void app_gap_vendor_callback(uint8_t cb_type, void *p_cb_data)
{
    T_GAP_VENDOR_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_GAP_VENDOR_CB_DATA));
    switch (cb_type)
    {
    case GAP_MSG_VENDOR_CMD_RSP:
		
        APP_PRINT_INFO4("GAP_MSG_VENDOR_CMD_RSP: command 0x%x, cause 0x%x, is_cmpl_evt %d, param_len %d",
                        cb_data.p_gap_vendor_cmd_rsp->command,
                        cb_data.p_gap_vendor_cmd_rsp->cause,
                        cb_data.p_gap_vendor_cmd_rsp->is_cmpl_evt,
                        cb_data.p_gap_vendor_cmd_rsp->param_len);
        switch(cb_data.p_gap_vendor_cmd_rsp->command)
        {
            case HCI_VENDOR_MAILBOX_CMD:
                bt_coex_handle_cmd_complete_evt(cb_data.p_gap_vendor_cmd_rsp->command,
                        cb_data.p_gap_vendor_cmd_rsp->cause, cb_data.p_gap_vendor_cmd_rsp->param_len,
                        cb_data.p_gap_vendor_cmd_rsp->param);
                break;

            default:
                break;
        }
        break;
    case GAP_MSG_VENDOR_EVT_INFO:
        {
		//format: subcode + status + payload(for wifi)
            uint16_t subcode;
            uint8_t *p = cb_data.p_gap_vendor_evt_info->param;
            LE_STREAM_TO_UINT16(subcode, p);
            APP_PRINT_INFO1("GAP_MSG_VENDOR_EVT_INFO: param_len %d",
                            cb_data.p_gap_vendor_evt_info->param_len);

            switch(subcode)
            {
                case HCI_VENDOR_PTA_AUTO_REPORT_EVENT:
                    bt_coex_handle_specific_evt(p,cb_data.p_gap_vendor_evt_info->param_len - 2);
                    break;
                default:
                    break;
            }
        }
		
        break;
    default:
        break;
    }
    if (ext_app_cb)
    {
        ext_app_cb(cb_type, p_cb_data);
    }
    return;
}

void vendor_cmd_init(P_FUN_GAP_APP_CB app_cb)
{
    if(app_cb != NULL)
    {
        ext_app_cb = app_cb;
    }
    gap_register_vendor_cb(app_gap_vendor_callback);
}

