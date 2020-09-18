// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "yoc_blufi_api.h"
#include "yoc_bt_defs.h"
#include "yoc_bt_main.h"
#include "btc/btc_task.h"
#include "btc_blufi_prf.h"
#include "btc/btc_manage.h"
#include "btc/btc_main.h"
#include "osi/future.h"
#include "btc_gatts.h"
#include "btc_gatt_util.h"

bt_err_t yoc_blufi_register_callbacks(yoc_blufi_callbacks_t *callbacks)
{
    if (yoc_bluedroid_get_status() == YOC_BLUEDROID_STATUS_UNINITIALIZED) {
        return BT_ERR_INVALID_STATE;
    }

    if (callbacks == NULL) {
        return BT_FAIL;
    }

    btc_blufi_set_callbacks(callbacks);
    return (btc_profile_cb_set(BTC_PID_BLUFI, callbacks->event_cb) == 0 ? BT_OK : BT_FAIL);
}

bt_err_t yoc_blufi_send_wifi_conn_report(wifi_mode_t opmode, yoc_blufi_sta_conn_state_t sta_conn_state, uint8_t softap_conn_num, yoc_blufi_extra_info_t *extra_info)
{
    btc_msg_t msg;
    btc_blufi_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_SEND_CFG_REPORT;
    arg.wifi_conn_report.opmode = opmode;
    arg.wifi_conn_report.sta_conn_state = sta_conn_state;
    arg.wifi_conn_report.softap_conn_num = softap_conn_num;
    arg.wifi_conn_report.extra_info = extra_info;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_blufi_args_t), btc_blufi_call_deep_copy) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_blufi_send_wifi_list(uint16_t apCount, yoc_blufi_ap_record_t *list)
{
    btc_msg_t msg;
    btc_blufi_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_SEND_WIFI_LIST;
    arg.wifi_list.apCount = apCount;
    arg.wifi_list.list = list;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_blufi_args_t), btc_blufi_call_deep_copy) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_blufi_profile_init(void)
{
    btc_msg_t msg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_INIT;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_blufi_profile_deinit(void)
{
    btc_msg_t msg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_DEINIT;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

uint16_t yoc_blufi_get_version(void)
{
    return btc_blufi_get_version();
}

bt_err_t yoc_blufi_close(yoc_gatt_if_t gatts_if, uint16_t conn_id)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_CLOSE;
    arg.close.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL)
            == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_blufi_send_error_info(yoc_blufi_error_state_t state)
{
    btc_msg_t msg;
    btc_blufi_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_SEND_ERR_INFO;
    arg.blufi_err_infor.state = state;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_blufi_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_blufi_send_custom_data(uint8_t *data, uint32_t data_len)
{
    btc_msg_t msg;
    btc_blufi_args_t arg;
    if(data == NULL || data_len == 0) {
        return BT_ERR_INVALID_ARG;
    }
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLUFI;
    msg.act = BTC_BLUFI_ACT_SEND_CUSTOM_DATA;
    arg.custom_data.data = data;
    arg.custom_data.data_len = data_len;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_blufi_args_t), btc_blufi_call_deep_copy) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}
