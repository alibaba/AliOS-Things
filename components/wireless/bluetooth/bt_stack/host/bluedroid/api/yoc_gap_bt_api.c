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

#include "common/bt_target.h"
#include <string.h>
#include "yoc_bt_main.h"
#include "yoc_gap_bt_api.h"
#include "common/bt_trace.h"
#include "btc/btc_manage.h"
#include "btc_gap_bt.h"
#include "btc/btc_storage.h"

#if (BTC_GAP_BT_INCLUDED == TRUE)

bt_err_t yoc_bt_gap_register_callback(yoc_bt_gap_cb_t callback)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return BT_FAIL;
    }

    btc_profile_cb_set(BTC_PID_GAP_BT, callback);
    return BT_OK;
}

bt_err_t yoc_bt_gap_set_scan_mode(yoc_bt_connection_mode_t c_mode, yoc_bt_discovery_mode_t d_mode)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_SET_SCAN_MODE;
    arg.set_scan_mode.c_mode = c_mode;
    arg.set_scan_mode.d_mode = d_mode;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_start_discovery(yoc_bt_inq_mode_t mode, uint8_t inq_len, uint8_t num_rsps)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    if (mode != YOC_BT_INQ_MODE_GENERAL_INQUIRY &&
            mode != YOC_BT_INQ_MODE_LIMITED_INQUIRY) {
        return BT_ERR_INVALID_ARG;
    }

    if (inq_len < YOC_BT_GAP_MIN_INQ_LEN ||
            inq_len > YOC_BT_GAP_MAX_INQ_LEN) {
        return BT_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_START_DISCOVERY;

    arg.start_disc.mode = mode;
    arg.start_disc.inq_len = inq_len;
    arg.start_disc.num_rsps = num_rsps;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_cancel_discovery(void)
{
    btc_msg_t msg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_CANCEL_DISCOVERY;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_get_remote_services(yoc_bd_addr_t remote_bda)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_GET_REMOTE_SERVICES;

    memcpy(&arg.bda, remote_bda, sizeof(bt_bdaddr_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_get_remote_service_record(yoc_bd_addr_t remote_bda, yoc_bt_uuid_t *uuid)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_GET_REMOTE_SERVICE_RECORD;

    memcpy(&arg.get_rmt_srv_rcd.bda, remote_bda, sizeof(bt_bdaddr_t));
    memcpy(&arg.get_rmt_srv_rcd.uuid, uuid, sizeof(yoc_bt_uuid_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

uint8_t *yoc_bt_gap_resolve_eir_data(uint8_t *eir, yoc_bt_eir_type_t type, uint8_t *length)
{
    if (!eir) {
        return NULL;
    }

    return BTM_CheckEirData(eir, type, length);
}

bt_err_t yoc_bt_gap_config_eir_data(yoc_bt_eir_data_t *eir_data)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    if (eir_data == NULL) {
        return BT_ERR_INVALID_ARG;
    }

    if (eir_data->manufacturer_len > YOC_BT_EIR_MAX_LEN
        || eir_data->url_len > YOC_BT_EIR_MAX_LEN) {
        return BT_ERR_INVALID_ARG;
    }

    if ((eir_data->manufacturer_len > 0 && eir_data->p_manufacturer_data == NULL)
        || (eir_data->url_len > 0 && eir_data->p_url == NULL)) {
        return BT_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_CONFIG_EIR;

    memcpy(&arg.config_eir, eir_data, sizeof(yoc_bt_eir_data_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), btc_gap_bt_arg_deep_copy) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_set_cod(yoc_bt_cod_t cod, yoc_bt_cod_mode_t mode)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    switch (mode) {
    case YOC_BT_SET_COD_MAJOR_MINOR:
    case YOC_BT_SET_COD_SERVICE_CLASS:
    case YOC_BT_CLR_COD_SERVICE_CLASS:
    case YOC_BT_SET_COD_ALL:
    case YOC_BT_INIT_COD:
        break;
    default:
        return BT_ERR_INVALID_ARG;
        break;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_SET_COD;

    arg.set_cod.mode = mode;
    memcpy(&arg.set_cod.cod, &cod, sizeof(yoc_bt_cod_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}


bt_err_t yoc_bt_gap_get_cod(yoc_bt_cod_t *cod)
{
    return btc_gap_bt_get_cod(cod);
}


bt_err_t yoc_bt_gap_read_rssi_delta(yoc_bd_addr_t remote_addr)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_READ_RSSI_DELTA;
    memcpy(arg.read_rssi_delta.bda.address, remote_addr, sizeof(yoc_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_remove_bond_device(yoc_bd_addr_t bd_addr)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_REMOVE_BOND_DEVICE;

    memcpy(arg.rm_bond_device.bda.address, bd_addr, sizeof(yoc_bd_addr_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), NULL) == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

int yoc_bt_gap_get_bond_device_num(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_FAIL;
    }
    return btc_storage_get_num_bt_bond_devices();
}

bt_err_t yoc_bt_gap_get_bond_device_list(int *dev_num, yoc_bd_addr_t *dev_list)
{
    int ret;
    int dev_num_total;

    if (dev_num == NULL || dev_list == NULL) {
        return BT_ERR_INVALID_ARG;
    }

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    dev_num_total = btc_storage_get_num_bt_bond_devices();
    if (*dev_num > dev_num_total) {
        *dev_num = dev_num_total;
    }

    ret = btc_storage_get_bonded_bt_devices_list((bt_bdaddr_t *)dev_list, *dev_num);

    return (ret == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_get_bond_device_name(yoc_bd_addr_t *device, uint8_t *name, uint8_t length)
{
    int ret;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    ret = btc_storage_get_bonded_bt_devices_name((bt_bdaddr_t *)device, name, length);
    return (ret == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_set_pin(yoc_bt_pin_type_t pin_type, uint8_t pin_code_len, yoc_bt_pin_code_t pin_code)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_SET_PIN_TYPE;
    arg.set_pin_type.pin_type = pin_type;
    if (pin_type == YOC_BT_PIN_TYPE_FIXED){
        arg.set_pin_type.pin_code_len = pin_code_len;
        memcpy(arg.set_pin_type.pin_code, pin_code, pin_code_len);
    } else {
        arg.set_pin_type.pin_code_len = 0;
        memset(arg.set_pin_type.pin_code, 0, YOC_BT_PIN_CODE_LEN);
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), btc_gap_bt_arg_deep_copy)
            == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_pin_reply(yoc_bd_addr_t bd_addr, bool accept, uint8_t pin_code_len, yoc_bt_pin_code_t pin_code)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_PIN_REPLY;
    arg.pin_reply.accept = accept;
    arg.pin_reply.pin_code_len = pin_code_len;
    memcpy(arg.pin_reply.bda.address, bd_addr, sizeof(yoc_bd_addr_t));
    memcpy(arg.pin_reply.pin_code, pin_code, pin_code_len);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), btc_gap_bt_arg_deep_copy)
            == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

#if (BT_SSP_INCLUDED == TRUE)
bt_err_t yoc_bt_gap_set_security_param(yoc_bt_sp_param_t param_type,
        void *value, uint8_t len)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_SET_SECURITY_PARAM;
    arg.set_security_param.param_type = param_type;
    arg.set_security_param.len = len;
    arg.set_security_param.value = value;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), btc_gap_bt_arg_deep_copy)
            == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_ssp_passkey_reply(yoc_bd_addr_t bd_addr, bool accept, uint32_t passkey)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_PASSKEY_REPLY;
    arg.passkey_reply.accept = accept;
    arg.passkey_reply.passkey = passkey;
    memcpy(arg.passkey_reply.bda.address, bd_addr, sizeof(yoc_bd_addr_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), btc_gap_bt_arg_deep_copy)
            == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

bt_err_t yoc_bt_gap_ssp_confirm_reply(yoc_bd_addr_t bd_addr, bool accept)
{
    btc_msg_t msg;
    btc_gap_bt_args_t arg;

    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_ACT_CONFIRM_REPLY;
    arg.confirm_reply.accept = accept;
    memcpy(arg.confirm_reply.bda.address, bd_addr, sizeof(yoc_bd_addr_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_gap_bt_args_t), btc_gap_bt_arg_deep_copy)
            == BT_STATUS_SUCCESS ? BT_OK : BT_FAIL);
}

#endif /*(BT_SSP_INCLUDED == TRUE)*/

#endif /* #if BTC_GAP_BT_INCLUDED == TRUE */
