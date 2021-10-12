/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "aos_system.h"
#include "py_defines.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_ntp_api.h"
#include "aiot_mqtt_api.h"
#include "module_aiot.h"

#define MOD_STR "AIOT_NTP"

/* ntp timestamp */
// extern int64_t pyamp_g_ntp_time;
// extern int64_t pyamp_g_up_time;

typedef struct {
    mp_obj_t cb;
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t msecond;
    uint64_t timestamp;
} hapy_aiot_ntp_notify_param_t;

static void aiot_device_ntp_notify(void *pdata)
{
    hapy_aiot_ntp_notify_param_t *param = (hapy_aiot_ntp_notify_param_t *)pdata;

    mp_obj_t dict;
    dict = mp_obj_new_dict(8);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("year", 4),
                              mp_obj_new_int(param->year));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("month", 5),
                              mp_obj_new_int(param->month));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("day", 3),
                              mp_obj_new_int(param->day));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("hour", 4),
                              mp_obj_new_int(param->hour));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("minute", 6),
                              mp_obj_new_int(param->minute));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("second", 6),
                              mp_obj_new_int(param->second));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("msecond", 7),
                              mp_obj_new_int(param->msecond));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("timestamp", 9),
                              mp_obj_new_int(param->timestamp));
    if (mp_obj_is_fun(param->cb)) {
        callback_to_python(param->cb, dict);
    } else {
        amp_error(MOD_STR,
                    "param->cb is not function");
    }
    aos_free(param);
}

static void aiot_app_ntp_recv_handler(void *handle, const aiot_ntp_recv_t *packet, void *userdata)
{
    int res = -1;

    hapy_aiot_ntp_notify_param_t *ntp_params;

    switch (packet->type) {
    case AIOT_NTPRECV_LOCAL_TIME:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "year: %d, month: %d, day: %d, hour: %d, min: %d, sec: %d, msec: %d, timestamp: %d",
                packet->data.local_time.year,
                packet->data.local_time.mon,
                packet->data.local_time.day,
                packet->data.local_time.hour,
                packet->data.local_time.min,
                packet->data.local_time.sec,
                packet->data.local_time.msec,
                packet->data.local_time.timestamp
        );

        // pyamp_g_ntp_time = packet->data.local_time.timestamp;
        // pyamp_g_up_time = aos_now_ms();

        ntp_params = (hapy_aiot_ntp_notify_param_t *)userdata;

        ntp_params->year        = packet->data.local_time.year;
        ntp_params->month       = packet->data.local_time.mon;
        ntp_params->day         = packet->data.local_time.day;
        ntp_params->hour        = packet->data.local_time.hour;
        ntp_params->minute      = packet->data.local_time.min;
        ntp_params->second      = packet->data.local_time.sec;
        ntp_params->msecond     = packet->data.local_time.msec;
        ntp_params->timestamp   = packet->data.local_time.timestamp;

        break;
    default:
        aos_free(ntp_params);
        return;
    }

    py_task_schedule_call(aiot_device_ntp_notify, ntp_params);

    res = aiot_ntp_deinit(&handle);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "ntp deinit failed");
    }
}

static void aiot_app_ntp_event_handler(void *handle, const aiot_ntp_event_t *event, void *userdata)
{
    switch (event->type) {
    case AIOT_NTPEVT_INVALID_RESPONSE:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "ntp receive data invalid");
        break;
    case AIOT_NTPEVT_INVALID_TIME_FORMAT:
        amp_debug(MOD_STR, "ntp receive data error");
        break;
    default:
        break;
    }
}

/* ntp service */
int32_t hapy_aiot_amp_ntp_service(void *mqtt_handle, mp_obj_t cb)
{
    int32_t res = STATE_SUCCESS;
    int32_t time_zone = 8;
    void *ntp_handle = NULL;
    hapy_aiot_ntp_notify_param_t *ntp_params;

    if (mqtt_handle == NULL) {
        amp_error(MOD_STR, "ntp service init failed");
        return -1;
    }

    ntp_handle = aiot_ntp_init();
    if (ntp_handle == NULL) {
        amp_error(MOD_STR, "ntp service init failed");
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_MQTT_HANDLE, (void *)mqtt_handle);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "ntp set mqtt handle failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_TIME_ZONE, (void *)&time_zone);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "ntp set time zone failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_RECV_HANDLER, (void *)aiot_app_ntp_recv_handler);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "ntp set receive handler failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_EVENT_HANDLER, (void *)aiot_app_ntp_event_handler);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "ntp set event handler failed");
        aiot_ntp_deinit(&ntp_handle);
        return -1;
    }

    ntp_params = aos_malloc(sizeof(hapy_aiot_ntp_notify_param_t));
    if (!ntp_params) {
        amp_error(MOD_STR, "alloc device_ntp_notify_param_t fail");
        return;
    }
    memset(ntp_params, 0, sizeof(hapy_aiot_ntp_notify_param_t));

    ntp_params->cb = cb ;

    res = aiot_ntp_setopt(ntp_handle, AIOT_NTPOPT_USERDATA, ntp_params);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "ntp set user data failed");
        aiot_ntp_deinit(&ntp_handle);
        aos_free(ntp_params);
        return -1;
    }

    res = aiot_ntp_send_request(ntp_handle);
    if (res < STATE_SUCCESS) {
        amp_error(MOD_STR, "ntp send request failed");
        aiot_ntp_deinit(&ntp_handle);
        aos_free(ntp_params);
        return -1;
    }

    return res;
}
