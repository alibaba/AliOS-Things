/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osi/alarm.h"
#include "osi/allocator.h"
#include "osi/list.h"
#include "btc/btc_task.h"
#include "btc/btc_alarm.h"
#include "osi/mutex.h"
#include "bt_common.h"

enum {
    ALARM_STATE_IDLE,
    ALARM_STATE_OPEN,
};

static osi_mutex_t alarm_mutex;
static int alarm_state;

#if (BT_BLE_DYNAMIC_ENV_MEMORY == FALSE)
static struct alarm_t alarm_cbs[ALARM_CBS_NUM];
#else
static struct alarm_t *alarm_cbs;
#endif

static osi_alarm_err_t alarm_free(osi_alarm_t *alarm);
static osi_alarm_err_t alarm_set(osi_alarm_t *alarm, period_ms_t timeout, bool is_periodic);

int osi_alarm_create_mux(void)
{
    if (alarm_state != ALARM_STATE_IDLE) {
        OSI_TRACE_WARNING("%s, invalid state %d\n", __func__, alarm_state);
        return -1;
    }
    osi_mutex_new(&alarm_mutex);
    return 0;
}

int osi_alarm_delete_mux(void)
{
    if (alarm_state != ALARM_STATE_IDLE) {
        OSI_TRACE_WARNING("%s, invalid state %d\n", __func__, alarm_state);
        return -1;
    }
    osi_mutex_free(&alarm_mutex);
    return 0;
}

void osi_alarm_init(void)
{
    //assert(alarm_mutex != NULL);

    osi_mutex_lock(&alarm_mutex, OSI_MUTEX_MAX_TIMEOUT);
    if (alarm_state != ALARM_STATE_IDLE) {
        OSI_TRACE_WARNING("%s, invalid state %d\n", __func__, alarm_state);
        goto end;
    }
#if (BT_BLE_DYNAMIC_ENV_MEMORY == TRUE)
    if ((alarm_cbs = (osi_alarm_t *)osi_malloc(sizeof(osi_alarm_t) * ALARM_CBS_NUM)) == NULL) {
        OSI_TRACE_ERROR("%s, malloc failed\n", __func__);
        goto end;
    }
#endif

    memset(alarm_cbs, 0x00, sizeof(osi_alarm_t) * ALARM_CBS_NUM);
    alarm_state = ALARM_STATE_OPEN;

end:
    osi_mutex_unlock(&alarm_mutex);
}

void osi_alarm_deinit(void)
{
    //assert(alarm_mutex != NULL);

    osi_mutex_lock(&alarm_mutex, OSI_MUTEX_MAX_TIMEOUT);
    if (alarm_state != ALARM_STATE_OPEN) {
        OSI_TRACE_WARNING("%s, invalid state %d\n", __func__, alarm_state);
        goto end;
    }

    for (int i = 0; i < ALARM_CBS_NUM; i++) {
        if (!hal_os_timer_is_valid(&alarm_cbs[i].alarm_hdl)) {
            alarm_free(&alarm_cbs[i]);
        }
    }

#if (BT_BLE_DYNAMIC_ENV_MEMORY == TRUE)
    osi_free(alarm_cbs);
    alarm_cbs = NULL;
#endif

    alarm_state = ALARM_STATE_IDLE;

end:
    osi_mutex_unlock(&alarm_mutex);
}

static struct alarm_t *alarm_cbs_lookfor_available(void)
{
    int i;

    for (i = 0; i < ALARM_CBS_NUM; i++) {
        if (!hal_os_timer_is_valid(&alarm_cbs[i].alarm_hdl)) { //available
            OSI_TRACE_DEBUG("%s %d %p\n", __func__, i, &alarm_cbs[i]);
            return &alarm_cbs[i];
        }
    }

    return NULL;
}

static void alarm_cb_handler(void *timer, void *argv)
{
    struct alarm_t *alarm = (struct alarm_t *)argv;
    OSI_TRACE_DEBUG("TimerID %p\n", alarm);
    if (alarm_state != ALARM_STATE_OPEN) {
        OSI_TRACE_WARNING("%s, invalid state %d\n", __func__, alarm_state);
        return;
    }

    if (alarm->cb) {
        alarm->cb(alarm->cb_data);
    }

#if 0
    btc_msg_t msg;
    btc_alarm_args_t arg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ALARM;
    arg.cb = alarm->cb;
    arg.cb_data = alarm->cb_data;
    btc_transfer_context(&msg, &arg, sizeof(btc_alarm_args_t), NULL);
#endif
}

osi_alarm_t *osi_alarm_new(const char *alarm_name, osi_alarm_callback_t callback, void *data, period_ms_t timer_expire)
{
    //assert(alarm_mutex != NULL);

    struct alarm_t *timer_id = NULL;

    osi_mutex_lock(&alarm_mutex, OSI_MUTEX_MAX_TIMEOUT);
    if (alarm_state != ALARM_STATE_OPEN) {
        OSI_TRACE_ERROR("%s, invalid state %d\n", __func__, alarm_state);
        timer_id = NULL;
        goto end;
    }

    timer_id = alarm_cbs_lookfor_available();

    if (!timer_id) {
        OSI_TRACE_ERROR("%s alarm_cbs exhausted\n", __func__);
        timer_id = NULL;
        goto end;
    }

    timer_id->cb = callback;
    timer_id->cb_data = data;
    timer_id->deadline = 0;

    int  stat = hal_os_create_timer(&timer_id->alarm_hdl, alarm_cb_handler, timer_id, timer_expire);
    if (stat != 0) {
        OSI_TRACE_ERROR("%s failed to create timer, err 0x%x\n", __func__, stat);
        timer_id = NULL;
        goto end;
    }

end:
    osi_mutex_unlock(&alarm_mutex);
    return timer_id;
}

static osi_alarm_err_t alarm_free(osi_alarm_t *alarm)
{
    if (!alarm || !hal_os_timer_is_valid(&alarm->alarm_hdl)) {
        OSI_TRACE_ERROR("%s null\n", __func__);
        return OSI_ALARM_ERR_INVALID_ARG;
    }
    hal_os_timer_stop(&alarm->alarm_hdl);
    hal_os_free_timer(&alarm->alarm_hdl);

    memset(alarm, 0, sizeof(osi_alarm_t));
    return OSI_ALARM_ERR_PASS;
}

void osi_alarm_free(osi_alarm_t *alarm)
{
    //assert(alarm_mutex != NULL);

    osi_mutex_lock(&alarm_mutex, OSI_MUTEX_MAX_TIMEOUT);
    if (alarm_state != ALARM_STATE_OPEN) {
        OSI_TRACE_ERROR("%s, invalid state %d\n", __func__, alarm_state);
        goto end;
    }
    alarm_free(alarm);

end:
    osi_mutex_unlock(&alarm_mutex);
    return;
}

static osi_alarm_err_t alarm_set(osi_alarm_t *alarm, period_ms_t timeout, bool is_periodic)
{
    //assert(alarm_mutex != NULL);

    osi_alarm_err_t ret = OSI_ALARM_ERR_PASS;
    osi_mutex_lock(&alarm_mutex, OSI_MUTEX_MAX_TIMEOUT);
    if (alarm_state != ALARM_STATE_OPEN) {
        OSI_TRACE_ERROR("%s, invalid state %d\n", __func__, alarm_state);
        ret = OSI_ALARM_ERR_INVALID_STATE;
        goto end;
    }

    if (!alarm || !hal_os_timer_is_valid(&alarm->alarm_hdl)) {
        OSI_TRACE_ERROR("%s null\n", __func__);
        ret = OSI_ALARM_ERR_INVALID_ARG;
        goto end;
    }

    ret = hal_os_timer_stop(&alarm->alarm_hdl);

    if (ret != 0) {
       OSI_TRACE_ERROR("%s aos_timer_stop %d\n", __func__, ret); 
    //    return ret;
    }

    if (is_periodic) {
        ret = hal_os_timer_change(&alarm->alarm_hdl, timeout);
    } else {
        hal_os_timer_change_once(&alarm->alarm_hdl, timeout);
    }

    if (ret != 0) {
       OSI_TRACE_ERROR("%s aos_timer_change %d\n", __func__, ret); 
    //    return ret;
    }

    ret = hal_os_timer_start(&alarm->alarm_hdl);

    if (ret != 0) {
       OSI_TRACE_ERROR("%s aos_timer_start %d\n", __func__, ret); 
    //    return ret;
    }

    alarm->deadline = is_periodic ? 0 : (timeout + hal_now_ms());

end:
    osi_mutex_unlock(&alarm_mutex);
    return ret;
}

osi_alarm_err_t osi_alarm_set(osi_alarm_t *alarm, period_ms_t timeout)
{
    return alarm_set(alarm, timeout, false);
}

osi_alarm_err_t osi_alarm_set_periodic(osi_alarm_t *alarm, period_ms_t period)
{
    return alarm_set(alarm, period, true);
}

osi_alarm_err_t osi_alarm_cancel(osi_alarm_t *alarm)
{
    int ret = OSI_ALARM_ERR_PASS;
    osi_mutex_lock(&alarm_mutex, OSI_MUTEX_MAX_TIMEOUT);
    if (alarm_state != ALARM_STATE_OPEN) {
        OSI_TRACE_ERROR("%s, invalid state %d\n", __func__, alarm_state);
        ret = OSI_ALARM_ERR_INVALID_STATE;
        goto end;
    }

    if (!alarm || !hal_os_timer_is_valid(&alarm->alarm_hdl)) {
        OSI_TRACE_ERROR("%s null\n", __func__);
        ret = OSI_ALARM_ERR_INVALID_ARG;
        goto end;
    }

    ret = hal_os_timer_stop(&alarm->alarm_hdl);

end:
    osi_mutex_unlock(&alarm_mutex);
    return ret;
}

period_ms_t osi_alarm_get_remaining_ms(const osi_alarm_t *alarm)
{
    //assert(alarm_mutex != NULL);
    int64_t dt_us = 0;

    osi_mutex_lock(&alarm_mutex, OSI_MUTEX_MAX_TIMEOUT);
    dt_us = alarm->deadline - hal_now_ms();
    osi_mutex_unlock(&alarm_mutex);

    return (dt_us > 0) ? (period_ms_t)(dt_us / 1000) : 0;
}

uint32_t osi_time_get_os_boottime_ms(void)
{
    return (uint32_t)(hal_now_ms());
}
