
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

#include <aos/kernel.h>
#include "hal/sensor.h"
#include "aos/log.h"


#define DO_FOREVER  

#define uDATA_STR  "uData: "   /* uData debug header */

#define TIMESTAMP_2_MS(t)      ((t) / 1000)   
#define MS_2_TIMESTAMP(ms)     ((ms) * 1000) 

#define INTERVAL_2_MS(i)       TIMESTAMP_2_MS(i)
#define MS_TO_INTERVAL(ms)     MS_2_TIMESTAMP(ms)

#define HZ_2_INTERVAL(hz)      ((hz) ? ((1000) / (hz)) : 0)
#define INTERVAL_2_HZ(i)       ((i) == 0 ? 0 : ((i) > MS_2_TIMESTAMP(1000) ? 1 : (MS_2_TIMESTAMP(1000) / (i)))) 

#define HZ_2_TIMESTAMP(hz)     HZ_2_INTERVAL(hz)
#define TIMESTAMP_2_HZ(t)      INTERVAL_2_HZ(t)

typedef bool b_subscribed;
typedef bool b_enbled;
typedef int (*fn_cb)(void* pData); /* callback for calibrated algo */

typedef enum 
{
 UDATA_SERVICE_ACC = 0,     /* Accelerometer */ 
 UDATA_SERVICE_MAG,         /* Magnetometer */
 UDATA_SERVICE_GYRO,        /* Gyroscope */
 UDATA_SERVICE_ALS,         /* Ambient light sensor */
 UDATA_SERVICE_PS,          /* Proximity */
 UDATA_SERVICE_BARO,        /* Barometer */
 UDATA_SERVICE_TEMP,        /* Temperature  */
 UDATA_SERVICE_UV,          /* Ultraviolet */
 UDATA_SERVICE_HUMI,        /* Humidity */
 UDATA_SERVICE_HALL,        /* HALL sensor */
 UDATA_SERVICE_HR,          /* Heart Rate sensor */
 UDATA_SERVICE_PEDOMETER,   
 UDATA_SERVICE_PDR,     
 UDATA_SERVICE_VDR,
 UDATA_SERVICE_GPS,
 
 UDATA_MAX_CNT, 
}udata_type_e;

/* the max size of the dat buf */
#define DATA_SIZE   64
#define ABS_DATA_MAX_CNT TAG_DEV_SENSOR_NUM_MAX

struct _abs_cali_cb_t {
    sensor_tag_e tag;
    int (*calibrated_algo_process_cb)(sensor_tag_e tag, void* pData); /* callback for calibrated algo */
};
typedef struct _abs_cali_cb_t abs_cali_cb_t;

struct _abs_data_pkg_t {
    sensor_tag_e              tag;
    uint8_t                   srv_cnt; /* count of the registed service base on this sensor */
    bool                      poweron; /* the power status of the registed service base on this sensor */
    uint32_t                  interval;/* the report data interval of the sensor*/
    uint64_t                  cur_timestamp; /* the current timestamp for every sensor, the unit is ms */
    int (*calibrated_algo_process_cb)(void* pData); /* callback for calibrated algo */
    dev_sensor_full_info_t    full_info;
};
typedef struct _abs_data_pkg_t  abs_data_pkg_t;

/* sensor service manager layer*/
struct _uData_service_t {
    udata_type_e               type; 
    sensor_tag_e               tag;
    b_subscribed               subscribe; /* subscribe only from aliyun side */
    b_enbled                   running;
    dev_sensor_config_t        config;
    uint8_t                    payload[DATA_SIZE];
    size_t(*service_process_cb)(sensor_tag_e tag, void* pdata);   /* process callback for udata service handle */
    int(*service_ioctl_cb)(udata_type_e type, sensor_tag_e tag); /* ioclt callback for udata service handle */
};
typedef struct _uData_service_t uData_service_t;

struct _sensor_msg_pkg_t {
    sensor_tag_e        tag;
    uint8_t             cmd;
    dev_sensor_config_t config;
};
typedef struct _sensor_msg_pkg_t  sensor_msg_pkg_t;

/* define the udata serivce struct here, please aline with aliyun side
   for the physical sensor, should be same as the dev sensor struct */
typedef struct _dev_barometer_data_t service_barometer_t;

typedef struct _service_pedometer_t{
    udata_type_e           type;
    uint32_t               step;
} service_pedometer_t;

typedef struct _udata_t{
    udata_type_e   type;
    uint16_t       value;
} udata_t;

typedef struct _udata_pkg_t{
    bool           valid;
    udata_type_e   type;
    __attribute__((aligned(4))) char payload[DATA_SIZE];
}udata_pkg_t; 

#endif /*UDATA_OBJ_DESC_H*/

