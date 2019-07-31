
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * @uData
 * @brief uData object desc
 * @version since 0.0.1
 */

#ifndef UDATA_OBJ_DESC_H
#define UDATA_OBJ_DESC_H

#include <stdbool.h>
#include "aos/kernel.h"
#include "sensor/sensor.h"
#include "udata/udata.h"
#include "ulog/ulog.h"

#define UDATA_SENSOR_ALL        (0XFFFFFFFF)
#define UDATA_DTC_NAME_LEN      (32)
#define UDATA_CONV_DATA_LEN     (64)
#define UDATA_TYPE_NAME_LEN     (16)
#define UDATA_ABS_BITMAP_UNIT   (8)
#define ABS_DATA_BITMAP_SUM     SENSOR_MAX_NUM
#define UDATA_SENSOR_BITMAP_MAX ((SENSOR_MAX_NUM+(UDATA_ABS_BITMAP_UNIT-1))/(UDATA_ABS_BITMAP_UNIT))

#define ABS_DATA_MAX_CNT        TAG_DEV_SENSOR_NUM_MAX
#define DATA_SIZE               (UATA_PAYLOAD_SIZE * sizeof(uint64_t))

#define DO_FOREVER              1
#define uDATA_STR               "uData: " /* uData debug header */

#define TIMESTAMP_2_MS(t)  ((t) / 1000)
#define MS_2_TIMESTAMP(ms) ((ms)*1000)
#define INTERVAL_2_MS(i)   TIMESTAMP_2_MS(i)
#define MS_TO_INTERVAL(ms) MS_2_TIMESTAMP(ms)

#define HZ_2_INTERVAL(hz)  ((hz) ? ((1000) / (hz)) : 0)
#define INTERVAL_2_HZ(i) \
    ((i) == 0            \
       ? 0               \
       : ((i) > MS_2_TIMESTAMP(1000) ? 1 : (MS_2_TIMESTAMP(1000) / (i))))

#define HZ_2_TIMESTAMP(hz) HZ_2_INTERVAL(hz)
#define TIMESTAMP_2_HZ(t)  INTERVAL_2_HZ(t)

typedef bool b_subscribed;
typedef bool b_enbled;
typedef int (*fn_cb)(void *pData); /* callback for calibrated algo */

typedef enum {
    UDATA_QUEUE_CLOSE = 0,
    UDATA_QUEUE_OPEN,
} udata_queue_cb_status;

struct _abs_cali_cb_t {
    sensor_tag_e tag;
    int (*calibrated_algo_process_cb)(
      sensor_tag_e tag, void *pData); /* callback for calibrated algo */
};

typedef struct _abs_cali_cb_t abs_cali_cb_t;

struct _abs_data_pkg_t {
    sensor_tag_e tag;
    uint8_t      instance;
    uint8_t      srv_cnt;   /* count of the registed service base on this sensor */
    bool         poweron;   /* the power status of the registed service base on this
                          sensor */
    uint32_t     interval; /* the report data interval of the sensor*/
    uint64_t     cur_timestamp; /* the current timestamp for every sensor, the unit
                               is ms */
    int (*calibrated_algo_process_cb)(void *pData, size_t size); /* callback for calibrated algo */
    dev_sensor_full_info_t full_info;
};

typedef struct _abs_data_pkg_t abs_data_pkg_t;
typedef size_t (*SERVICE_PROCESS_CB)(udata_type_e type, uint32_t abs_index, void *pdata, uint32_t len);
typedef int (*SERVICE_IOCTL_CB)(udata_type_e type, uint32_t abs_index);

/* sensor service manager layer*/
struct _udata_service_t {
    udata_type_e        type;
    b_subscribed        subscribe; /* subscribe only from aliyun side */
    b_enbled            running;
    bool                task_flag;
    uint8_t             queue_id;
    uint8_t             abs_bitmap[UDATA_SENSOR_BITMAP_MAX];
    uint32_t            interval[SENSOR_MAX_NUM];
    uint64_t            time[SENSOR_MAX_NUM];
    dev_sensor_config_t config;
    uint8_t             payload[DATA_SIZE];
    SERVICE_PROCESS_CB  service_process_cb;
    SERVICE_IOCTL_CB    service_ioctl_cb; /* ioclt callback for udata service handle */
};

typedef struct _udata_service_t udata_service_t;

typedef struct _udata_queue_cb_t {
    int8_t status;
    void (*msg_cb)(sensor_msg_pkg_t *message);
} udata_queue_cb_t;

/* define the udata serivce struct here, please aline with aliyun side
   for the physical sensor, should be same as the dev sensor struct */
typedef struct _dev_barometer_data_t service_barometer_t;

typedef struct _service_pedometer_t {
    udata_type_e type;
    uint32_t     step;
} service_pedometer_t;

typedef struct _udata_t {
    udata_type_e type;
    uint16_t     value;
} udata_t;

#endif /*UDATA_OBJ_DESC_H*/
