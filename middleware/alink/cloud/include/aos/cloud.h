/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_CLOUD_H
#define AOS_CLOUD_H

enum {
    CLOUD_CONNECTED,
    CLOUD_DISCONNECTED,
    GET_DEVICE_STATUS,
    SET_DEVICE_STATUS,
    GET_DEVICE_RAWDATA,
    SET_DEVICE_RAWDATA,
    UPGRADE_DEVICE,
    CANCEL_UPGRADE_DEVICE,
    GET_SUB_DEVICE_STATUS,
    SET_SUB_DEVICE_STATUS,
    MAX_EVENT_TYPE,
};

typedef void (*aos_cloud_cb_t)(int event, const char *json_buffer);

/**
 * Register cloud event callback.
 *
 * @param[in]  cb_type  event type interested.
 * @param[in]  cb       cloud event callback.
 *
 * @return  the operation status, 0 is OK, others is error.
 */
int aos_cloud_register_callback(int cb_type, aos_cloud_cb_t cb);

/**
 * Report event to cloud.
 *
 * @param[in]  method       remote method name.
 * @param[in]  json_buffer  method's payload.
 * @param[in]  done_cb      report done callback.
 * @param[in]  arg          private data passed to done_cb.
 *
 * @return  the operation status, 0 is OK, others is error.
 */
int aos_cloud_report(const char *method,
                     const char *json_buffer,
                     void (*done_cb)(void *),
                     void *arg);

/**
 * Trigger specific event, used by Cloud Backend.
 *
 * @param[in]  cb_type      event type.
 * @param[in]  json_buffer  payload.
 */
void aos_cloud_trigger(int cb_type, const char *json_buffer);

/**
 * Register Cloud Backend.
 *
 * @param[in]  report  called when user do aos_cloud_report.
 */
void aos_cloud_register_backend(int (*report)(const char *method, const char *json_buffer));

#endif /* AOS_CLOUD_H */

