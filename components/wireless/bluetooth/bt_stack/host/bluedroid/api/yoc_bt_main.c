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


#include "yoc_bt_main.h"
#include "btc/btc_task.h"
#include "btc/btc_main.h"
#include "osi/future.h"
#include "osi/allocator.h"

static bool bd_already_enable = false;
static bool bd_already_init = false;

yoc_bluedroid_status_t yoc_bluedroid_get_status(void)
{
    if (bd_already_init) {
        if (1/*bd_already_enable*/) {
            return BT_BLUEDROID_STATUS_ENABLED;
        } else {
            return YOC_BLUEDROID_STATUS_INITIALIZED;
        }
    } else {
        return YOC_BLUEDROID_STATUS_UNINITIALIZED;
    }
}

bt_err_t yoc_bluedroid_enable(void)
{
    btc_msg_t msg;
    future_t **future_p;

    BT_LOG_DEBUG("%s start", __func__);
    if (!bd_already_init) {
        BT_LOG_ERROR("Bludroid not initialised\n");
        return BT_ERR_INVALID_STATE;
    }

    if (bd_already_enable) {
        BT_LOG_ERROR("Bluedroid already enabled\n");
        return BT_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        BT_LOG_ERROR("Bluedroid enable failed\n");
        return BT_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_ENABLE;

    if (btc_transfer_context(&msg, NULL, 0, NULL) != BT_STATUS_SUCCESS) {
        BT_LOG_ERROR("Bluedroid enable failed\n");
        return BT_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        BT_LOG_ERROR("Bluedroid enable failed\n");
        return BT_FAIL;
    }

    bd_already_enable = true;

    return BT_OK;
}

bt_err_t yoc_bluedroid_disable(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (!bd_already_enable) {
        BT_LOG_ERROR("Bluedroid already disabled\n");
        return BT_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        BT_LOG_ERROR("Bluedroid disable failed\n");
        return BT_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_DISABLE;

    if (btc_transfer_context(&msg, NULL, 0, NULL) != BT_STATUS_SUCCESS) {
        BT_LOG_ERROR("Bluedroid disable failed\n");
        return BT_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        BT_LOG_ERROR("Bluedroid disable failed\n");
        return BT_FAIL;
    }

    bd_already_enable = false;

    return BT_OK;
}

bt_err_t yoc_bluedroid_init(void)
{
    btc_msg_t msg;
    future_t **future_p;

    BT_LOG_DEBUG("%s start", __func__);
    // if (yoc_bt_controller_get_status() != YOC_BT_CONTROLLER_STATUS_ENABLED) {
    //     BT_LOG_ERROR("Controller not initialised\n");
    //     return BT_ERR_INVALID_STATE;
    // }

    if (bd_already_init) {
        BT_LOG_ERROR("Bluedroid already initialised\n");
        return BT_ERR_INVALID_STATE;
    }

#if HEAP_MEMORY_DEBUG
    osi_mem_dbg_init();
#endif

    btc_init();

    future_p = btc_main_get_future_p(BTC_MAIN_INIT_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        BT_LOG_ERROR("Bluedroid initialise failed\n");
        return BT_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_INIT;

    if (btc_transfer_context(&msg, NULL, 0, NULL) != BT_STATUS_SUCCESS) {
        BT_LOG_ERROR("Bluedroid initialise failed\n");
        return BT_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        BT_LOG_ERROR("Bluedroid initialise failed\n");
        return BT_FAIL;
    }

    bd_already_init = true;

    return BT_OK;
}


bt_err_t yoc_bluedroid_deinit(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (!bd_already_init) {
        BT_LOG_ERROR("Bluedroid already de-initialised\n");
        return BT_ERR_INVALID_STATE;
    }

    if (bd_already_enable) {
        BT_LOG_ERROR("Bludroid already enabled, do disable first\n");
        return BT_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_DEINIT_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        BT_LOG_ERROR("Bluedroid de-initialise failed\n");
        return BT_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_DEINIT;

    if (btc_transfer_context(&msg, NULL, 0, NULL) != BT_STATUS_SUCCESS) {
        BT_LOG_ERROR("Bluedroid de-initialise failed\n");
        return BT_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        BT_LOG_ERROR("Bluedroid de-initialise failed\n");
        return BT_FAIL;
    }

    btc_deinit();

    bd_already_init = false;

    return BT_OK;
}

