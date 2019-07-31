/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef UDATA_H
#define UDATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define MAX_DATA_SIZE     64
#define UATA_PAYLOAD_SIZE ((MAX_DATA_SIZE + sizeof(uint64_t) - 1) / sizeof(uint64_t))

#define UDATA_MSG_DEV_READ           1
#define UDATA_MSG_DEV_IOCTL          2
#define UDATA_MSG_DEV_OPEN           3
#define UDATA_MSG_DEV_CLOSE          4
#define UDATA_MSG_DEV_ENABLE         5
#define UDATA_MSG_DEV_DISABLE        6
#define UDATA_MSG_SERVICE_SUBSRIBE   7
#define UDATA_MSG_SERVICE_UNSUBSRIBE 8
#define UDATA_MSG_SERVICE_PROCESS    9
#define UDATA_MSG_SERVICE_IOCTL      10
#define UDATA_MSG_REPORT_PUBLISH     11
#define UDATA_MSG_DATA_TO_CLOUD      12

typedef enum {
    UDATA_SERVICE_ACC = 0,  /* Accelerometer */
    UDATA_SERVICE_MAG,      /* Magnetometer */
    UDATA_SERVICE_GYRO,     /* Gyroscope */
    UDATA_SERVICE_ALS,      /* Ambient light sensor */
    UDATA_SERVICE_PS,       /* Proximity */
    UDATA_SERVICE_BARO,     /* Barometer */
    UDATA_SERVICE_TEMP,     /* Temperature  */
    UDATA_SERVICE_UV,       /* Ultraviolet */
    UDATA_SERVICE_HUMI,     /* Humidity */
    UDATA_SERVICE_NOISE,    /* Noise Loudness */
    UDATA_SERVICE_PM25,     /* PM2.5 */
    UDATA_SERVICE_PM1P0,    /* PM1.0 */
    UDATA_SERVICE_PM10,     /* PM10 */
    UDATA_SERVICE_CO2,      /* CO2Level */
    UDATA_SERVICE_HCHO,     /* HCHO Level */
    UDATA_SERVICE_TVOC,     /* TVOC Level */
    UDATA_SERVICE_PH,       /* PH value */
    UDATA_SERVICE_VWC,      /* volumetric water content value */
    UDATA_SERVICE_EC,       /* EC value */
    UDATA_SERVICE_SALINITY, /* SALINITY value */
    UDATA_SERVICE_TDS,      /* Total dissolved solids */
    UDATA_SERVICE_WINDSPD,
    UDATA_SERVICE_WINDDIR,
    UDATA_SERVICE_RAIN,
    UDATA_SERVICE_HALL,     /* HALL sensor */
    UDATA_SERVICE_HR,       /* Heart Rate sensor */
    UDATA_SERVICE_RGB,      /* RGB sensor */
    UDATA_SERVICE_GS,       /* Gesture sensor */
    UDATA_SERVICE_IR,       /* IR sensor */
    UDATA_SERVICE_PEDOMETER,
    UDATA_SERVICE_PDR,
    UDATA_SERVICE_VDR,
    UDATA_SERVICE_GPS,
    UDATA_SERVICE_RTC,

    UDATA_MAX_CNT,
} udata_type_e;

typedef struct _sensor_msg_pkg_t {
    unsigned int cmd;
    unsigned int index;
    unsigned int value;
} sensor_msg_pkg_t;

typedef struct _udata_pkg_t {
    bool         valid;
    udata_type_e type;
    uint64_t     payload[UATA_PAYLOAD_SIZE];
} udata_pkg_t;

int udata_register_msg_handler(void *func);

int udata_report_publish(udata_type_e type, void *pdata);

int udata_subscribe(udata_type_e type);

int udata_unsubscribe(udata_type_e type);

int udata_init(void);

#ifdef __cplusplus
}
#endif

#endif /* UDATA_H */

