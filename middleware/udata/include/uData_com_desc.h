
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
#include "sensor.h"
#include "aos/log.h"


#define DO_FOREVER 1

#define uDATA_STR "uData: " /* uData debug header */

#define TIMESTAMP_2_MS(t) ((t) / 1000)
#define MS_2_TIMESTAMP(ms) ((ms)*1000)

#define INTERVAL_2_MS(i) TIMESTAMP_2_MS(i)
#define MS_TO_INTERVAL(ms) MS_2_TIMESTAMP(ms)

#define HZ_2_INTERVAL(hz) ((hz) ? ((1000) / (hz)) : 0)
#define INTERVAL_2_HZ(i) \
    ((i) == 0            \
       ? 0               \
       : ((i) > MS_2_TIMESTAMP(1000) ? 1 : (MS_2_TIMESTAMP(1000) / (i))))

#define HZ_2_TIMESTAMP(hz) HZ_2_INTERVAL(hz)
#define TIMESTAMP_2_HZ(t) INTERVAL_2_HZ(t)

#ifndef UDATA_YLOOP
/** uData queue msg tag */
#define UDATA_MSG_DEV_READ 1
#define UDATA_MSG_DEV_IOCTL 2
#define UDATA_MSG_DEV_OPEN 3
#define UDATA_MSG_DEV_CLOSE 4
#define UDATA_MSG_DEV_ENABLE 5
#define UDATA_MSG_DEV_DISABLE 6
#define UDATA_MSG_SERVICE_SUBSRIBE 7   /* for external component */
#define UDATA_MSG_SERVICE_UNSUBSRIBE 8 /* for external component */
#define UDATA_MSG_SERVICE_PROCESS 9
#define UDATA_MSG_SERVICE_IOCTL 10
#define UDATA_MSG_REPORT_PUBLISH 11
#endif


typedef bool b_subscribed;
typedef bool b_enbled;
typedef int (*fn_cb)(void *pData); /* callback for calibrated algo */

typedef enum
{
    UDATA_SERVICE_ACC = 0, /* Accelerometer */
    UDATA_SERVICE_MAG,     /* Magnetometer */
    UDATA_SERVICE_GYRO,    /* Gyroscope */
    UDATA_SERVICE_ALS,     /* Ambient light sensor */
    UDATA_SERVICE_PS,      /* Proximity */
    UDATA_SERVICE_BARO,    /* Barometer */
    UDATA_SERVICE_TEMP,    /* Temperature  */
    UDATA_SERVICE_UV,      /* Ultraviolet */
    UDATA_SERVICE_HUMI,    /* Humidity */
    UDATA_SERVICE_NOISE,   /* Noise Loudness */
    UDATA_SERVICE_PM25,    /* PM2.5 */
    UDATA_SERVICE_PM1P0,   /* PM1.0 */
    UDATA_SERVICE_PM10,    /* PM10 */
    UDATA_SERVICE_CO2,     /* CO2Level */
    UDATA_SERVICE_HCHO,    /* HCHO Level */
    UDATA_SERVICE_TVOC,    /* TVOC Level */
    UDATA_SERVICE_PH,    /* PH value */
    UDATA_SERVICE_VWC,  /* volumetric water content value */
    UDATA_SERVICE_EC,   /* EC value */
    UDATA_SERVICE_SALINITY, /* SALINITY value */
    UDATA_SERVICE_TDS,      /* Total dissolved solids */
    UDATA_SERVICE_WINDSPD,
    UDATA_SERVICE_WINDDIR,
    UDATA_SERVICE_RAIN,
    UDATA_SERVICE_HALL,    /* HALL sensor */
    UDATA_SERVICE_HR,      /* Heart Rate sensor */
    UDATA_SERVICE_RGB,      /* RGB sensor */
    UDATA_SERVICE_GS,      /* Gesture sensor */
    UDATA_SERVICE_IR,      /* IR sensor */
    UDATA_SERVICE_PEDOMETER,
    UDATA_SERVICE_PDR,
    UDATA_SERVICE_VDR,
    UDATA_SERVICE_GPS,

    UDATA_MAX_CNT,
} udata_type_e;

typedef enum
{
    UDATA_QUEUE_CLOSE = 0,
    UDATA_QUEUE_OPEN,
} udata_queue_cb_status;

/* the max size of the dat buf */
#define DATA_SIZE 64
#define ABS_DATA_MAX_CNT TAG_DEV_SENSOR_NUM_MAX

struct _abs_cali_cb_t
{
    sensor_tag_e tag;
    int (*calibrated_algo_process_cb)(
      sensor_tag_e tag, void *pData); /* callback for calibrated algo */
};
typedef struct _abs_cali_cb_t abs_cali_cb_t;

struct _abs_data_pkg_t
{
    sensor_tag_e tag;
    uint8_t srv_cnt;   /* count of the registed service base on this sensor */
    bool    poweron;   /* the power status of the registed service base on this
                          sensor */
    uint32_t interval; /* the report data interval of the sensor*/
    uint64_t cur_timestamp; /* the current timestamp for every sensor, the unit
                               is ms */
    int (*calibrated_algo_process_cb)(
      void *pData); /* callback for calibrated algo */
    dev_sensor_full_info_t full_info;
};
typedef struct _abs_data_pkg_t abs_data_pkg_t;

/* sensor service manager layer*/
struct _uData_service_t
{
    udata_type_e        type;
    sensor_tag_e        tag;
    b_subscribed        subscribe; /* subscribe only from aliyun side */
    b_enbled            running;
    dev_sensor_config_t config;
    uint8_t             payload[DATA_SIZE];
    size_t (*service_process_cb)(
      sensor_tag_e tag, void *pdata,
      uint32_t len); /* process callback for udata service handle */

    int (*service_ioctl_cb)(
      udata_type_e type,
      sensor_tag_e tag); /* ioclt callback for udata service handle */
};
typedef struct _uData_service_t uData_service_t;

struct _sensor_msg_pkg_t
{
    unsigned int cmd;
    unsigned int index;
    unsigned int value;
};
typedef struct _sensor_msg_pkg_t sensor_msg_pkg_t;


typedef struct _uData_queue_cb_t
{
    int8_t status;
    void (*msg_cb)(sensor_msg_pkg_t *message);
} uData_queue_cb_t;

/* define the udata serivce struct here, please aline with aliyun side
   for the physical sensor, should be same as the dev sensor struct */
typedef struct _dev_barometer_data_t service_barometer_t;

typedef struct _service_pedometer_t
{
    udata_type_e type;
    uint32_t     step;
} service_pedometer_t;

typedef struct _udata_t
{
    udata_type_e type;
    uint16_t     value;
} udata_t;

typedef struct _udata_pkg_t
{
    bool                             valid;
    udata_type_e                     type;
    __attribute__((aligned(4))) char payload[DATA_SIZE];
} udata_pkg_t;

#endif /*UDATA_OBJ_DESC_H*/
