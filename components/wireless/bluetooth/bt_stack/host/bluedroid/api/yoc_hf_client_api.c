// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include "bt_err.h"
#include "yoc_hf_client_api.h"
#include "yoc_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_hf_client.h"
#include "bta/bta_api.h"
#include "bta/bta_hf_client_api.h"

#if BTC_HF_CLIENT_INCLUDED
bt_err_t yoc_hf_client_register_callback(yoc_hf_client_cb_t callback)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return BT_FAIL;
    }

    btc_profile_cb_set(BTC_PID_HF_CLIENT, callback);
    return BT_OK;
}

bt_err_t yoc_hf_client_init(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_deinit(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_connect(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_CONNECT_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.connect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_disconnect(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DISCONNECT_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.disconnect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_connect_audio(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_CONNECT_AUDIO_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.connect_audio), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_disconnect_audio(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DISCONNECT_AUDIO_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.disconnect_audio), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}


bt_err_t yoc_hf_client_start_voice_recognition(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_START_VOICE_RECOGNITION_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_stop_voice_recognition(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_STOP_VOICE_RECOGNITION_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_volume_update(yoc_hf_volume_control_target_t type, int volume)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_VOLUME_UPDATE_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.volume_update.type = type;
    arg.volume_update.volume = volume;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_dial(const char *number)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    if (number != NULL && strlen(number) > YOC_BT_HF_CLIENT_NUMBER_LEN) {
        return BT_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DIAL_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    if (number != NULL) {
        strcpy(arg.dial.number, number);
    } else {
        arg.dial.number[0] = '\0';
    }

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_dial_memory(int location)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DIAL_MEMORY_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.dial_memory.location = location;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_send_chld_cmd(yoc_hf_chld_type_t chld, int idx)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_CHLD_CMD_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.chld.type = chld;
    arg.chld.idx = idx;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_send_btrh_cmd(yoc_hf_btrh_cmd_t btrh)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_BTRH_CMD_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.btrh.cmd = btrh;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_answer_call(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_ANSWER_CALL_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_reject_call(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_REJECT_CALL_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_query_current_calls(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_QUERY_CURRENT_CALLS_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_query_current_operator_name(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_QUERY_CURRENT_OPERATOR_NAME_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_retrieve_subscriber_info(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_RETRIEVE_SUBSCRIBER_INFO_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_send_dtmf(char code)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_DTMF_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.send_dtmf.code = code;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_request_last_voice_tag_number(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_REQUEST_LAST_VOICE_TAG_NUMBER_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_hf_client_register_data_callback(yoc_hf_client_incoming_data_cb_t recv,
                                               yoc_hf_client_outgoing_data_cb_t send)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_REGISTER_DATA_CALLBACK_EVT;

    btc_hf_client_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.reg_data_cb.recv = recv;
    arg.reg_data_cb.send = send;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}


#if (BTM_SCO_HCI_INCLUDED == TRUE )
void yoc_hf_client_outgoing_data_ready(void)
{
    BTA_HfClientCiData();
}

void yoc_hf_client_pcm_resample_init(uint32_t src_sps, uint32_t bits, uint32_t channels)
{
    BTA_DmPcmInitSamples(src_sps, bits, channels);
}

void yoc_hf_client_pcm_resample_deinit(void)
{
    BTA_DmPcmDeinitSamples();
}

int32_t yoc_hf_client_pcm_resample(void *src, uint32_t in_bytes, void *dst)
{
    return BTA_DmPcmResample(src, in_bytes, dst);
}

#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE ) */

#endif /* BTC_HF_CLIENT_INCLUDED */
