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
#include "common/bt_trace.h"
#include <string.h>
#include "bt_err.h"
#include "yoc_a2dp_api.h"
#include "yoc_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_av.h"

#if BTC_AV_INCLUDED

#if BTC_AV_SINK_INCLUDED
bt_err_t yoc_a2d_sink_init(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_sink_deinit(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_sink_register_data_callback(yoc_a2d_sink_data_cb_t callback)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_REG_DATA_CB_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.data_cb = callback;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_sink_connect(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_CONNECT_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.connect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_sink_disconnect(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_DISCONNECT_EVT;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_sink_stop_player(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_STOP_PLAYER_CB_EVT;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

#endif /* BTC_AV_SINK_INCLUDED */

bt_err_t yoc_a2d_register_callback(yoc_a2d_cb_t callback)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return BT_FAIL;
    }

    btc_profile_cb_set(BTC_PID_A2DP, callback);
    return BT_OK;
}

bt_err_t yoc_a2d_media_ctrl(yoc_a2d_media_ctrl_t ctrl)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_API_MEDIA_CTRL_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));

    /* Switch to BTC context */
    arg.ctrl = ctrl;
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

#if BTC_AV_SRC_INCLUDED
bt_err_t yoc_a2d_source_init(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_source_deinit(void)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_source_connect(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_CONNECT_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.src_connect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_source_disconnect(yoc_bd_addr_t remote_bda)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_DISCONNECT_EVT;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

bt_err_t yoc_a2d_source_register_data_callback(yoc_a2d_source_data_cb_t callback)
{
    if (yoc_bluedroid_get_status() != BT_BLUEDROID_STATUS_ENABLED) {
        return BT_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_REG_DATA_CB_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.src_data_cb = callback;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? BT_OK : BT_FAIL;
}

#endif /* BTC_AV_SRC_INCLUDED */

#endif /* #if BTC_AV_INCLUDED */
