/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
* 
*  sensor hal
*/

#ifndef HAL_SENSOR_H
#define HAL_SENSOR_H

#include <stdint.h>
#include <hal/base.h>
#include "hal/soc/soc.h"
#include "hal/soc/gpio.h"
#include "hal/soc/i2c.h"


#define I2C_REG_LEN       1
#define I2C_DATA_LEN      1
#define I2C_OP_RETRIES    AOS_WAIT_FOREVER

/* ioctl cmd list for sensor */
#define SENSOR_IOCTL_ODR_SET                 (0x01<<0)
#define SENSOR_IOCTL_RANGE_SET               (0x01<<1)
#define SENSOR_IOCTL_GET_INFO                (0x01<<2)
#define SENSOR_IOCTL_BIST_PROCESS            (0x01<<3)
#define SENSOR_IOCTL_WHO_AM_I                (0x01<<4)
#define SENSOR_IOCTL_SET_POWER               (0x01<<5)
#define SENSOR_IOCTL_GET_SENSOR_LIST         (0x01<<6)

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

/* Physical generic sensor data defs generic structs here*/
#define DATA_AXIS_X     0
#define DATA_AXIS_Y     1
#define DATA_AXIS_Z     2

#define dev_acc_path     "/dev/acc"
#define dev_mag_path     "/dev/mag"
#define dev_gyro_path    "/dev/gyro"
#define dev_als_path     "/dev/als"
#define dev_ps_path      "/dev/ps"
#define dev_baro_path    "/dev/baro"
#define dev_temp_path    "/dev/temp"
#define dev_uv_path      "/dev/uv"
#define dev_humi_path    "/dev/humi"
#define dev_hall_path    "/dev/hall"
#define dev_hr_path      "/dev/hr"
#define dev_gps_path     "/dev/gps"

#define sensor_node_path "/dev/sensor"
#define gps_node_path    "/dev/nodegps"

#define GPS_STR     "gps: "
#define SENSOR_STR  "sensor: "   /* sensor debug header */
#define ERROR_LINE  "error on line is "   

#define ACCELEROMETER_UNIT_FACTOR    1000    //mg
#define MAGNETOMETER_UNIT_FACTOR     1000    //mGauss
#define GYROSCOPE_UNIT_FACTOR        1000000 //uDPS

/* add the new sensor type into the last postion  */
typedef enum{
    TAG_DEV_ACC = 0,    /* Accelerometer */
    TAG_DEV_MAG,        /* Magnetometer */
    TAG_DEV_GYRO,       /* Gyroscope */
    TAG_DEV_ALS,        /* Ambient light sensor */
    TAG_DEV_PS,         /* Proximity */
    TAG_DEV_BARO,       /* Barometer */
    TAG_DEV_TEMP,       /* Temperature  */
    TAG_DEV_UV,         /* Ultraviolet */
    TAG_DEV_HUMI,       /* Humidity */
    TAG_DEV_HALL,       /* HALL */
    TAG_DEV_HR,         /* Heart Rate */
    TAG_DEV_GPS,
    TAG_DEV_SENSOR_NUM_MAX,
} sensor_tag_e;


typedef enum {
    DEV_SENSOR_VENDOR_STM               = 0,
    DEV_SENSOR_VENDOR_SEMTECH,
    DEV_SENSOR_VENDOR_AKM,
    DEV_SENSOR_VENDOR_CAPELLA,
    DEV_SENSOR_VENDOR_INVENSENSE,
    DEV_SENSOR_VENDOR_ROHM,
    DEV_SENSOR_VENDOR_BOSCH,
    DEV_SENSOR_VENDOR_KIONIX,
    DEV_SENSOR_VENDOR_LITEON,
    DEV_SENSOR_VENDOR_AMS,
    DEV_SENSOR_VENDOR_CNT_MAXIM,
} vendor_id_e;

typedef enum { 
    I2C_PORT,
    SPI_PORT,
    I2S_PORT,
    UART_PORT,     
} dev_io_port_e;

typedef enum {
    DEV_POWER_OFF = 0,
    DEV_POWER_ON,
    DEV_SLEEP,
    DEV_SUSPEND,
    DEV_DEEP_SUSPEND,
} dev_power_mode_e;

typedef enum {
    DEV_HEALTH_GOOD,
    DEV_HEALTH_SICK,
} dev_health_state_e;


typedef enum {
    DEV_POLLING = 0,
    DEV_INT,
    DEV_DATA_READY,
    DEV_FIFO,
    DEV_MODE_INV
} work_mode_e;

typedef enum {
    mg = 0,
    uGauss,
    udps,
    lux,
    cm,
    pa,
    permillage,
    bpm,
    dCelsius,
} value_unit_e;


typedef struct _dev_accel_data_t {
    uint64_t timestamp;
    int32_t  data[3];
#ifdef AOS_SENSOR_ACC_SUPPORT_STEP
    uint32_t step;
#endif
}accel_data_t;

typedef struct _dev_gyro_data_t {
    uint64_t timestamp;
    int32_t  data[3];
}gyro_data_t;

typedef struct _dev_mag_data_t {
    uint64_t timestamp;
    int32_t  data[3];
}mag_data_t;

typedef struct _dev_barometer_data_t{
    uint64_t timestamp;
    uint32_t p;
}barometer_data_t;

typedef struct _dev_temperature_data_t{
    uint64_t timestamp;
    int32_t  t;
}temperature_data_t;

typedef struct _dev_humidity_data_t{
    uint64_t timestamp;
    uint32_t h;
}humidity_data_t;

typedef struct _dev_als_data_t{
    uint64_t timestamp;
    uint32_t lux;
}als_data_t;

typedef struct _dev_uv_data_t{
    uint64_t timestamp;
    uint16_t uvi;
}uv_data_t;

typedef struct _dev_proximity_data_t{
    uint64_t timestamp;
    uint32_t  present;
}proximity_data_t;

typedef struct _dev_hall_data_t {
    uint64_t timestamp;
    uint8_t  hall_level;
}hall_data_t;

typedef struct _dev_rgb_data_t{
    uint64_t timestamp;
    uint32_t data[3];
}rgb_data_t;

typedef struct _dev_ir_data_t{
    uint64_t timestamp;
    uint32_t ir;
}ir_data_t;

typedef struct _dev_ecg_data_t{
    uint64_t  timestamp;
    int16_t   raw_data;
}ecg_data_t;

typedef struct _dev_heart_rate_data_t {
    uint64_t timestamp;
    uint8_t  hear_rate;
}heart_rate_data_t;

typedef struct _dev_blood_pressure_data_t {
    uint64_t timestamp;
    uint16_t systolic;
    uint16_t diastolic;
}blood_pressure_t;

typedef struct _dev_sensor_config_t {
    uint8_t     id;
    uint32_t    range;
    uint32_t    odr;
}dev_sensor_config_t;

typedef struct _sensor_list_t{
    uint32_t    cnt;
    uint8_t     list[TAG_DEV_SENSOR_NUM_MAX];
}sensor_list_t;

typedef struct _dev_sensor_info_t {
    vendor_id_e             vendor;
    char*                   model;
    value_unit_e            unit;
    uint32_t                range_max;
    uint32_t                range_min;
    dev_health_state_e      health;
    //sensor_list_t           list;
}dev_sensor_info_t;

typedef struct _dev_sensor_full_info_t {
    dev_sensor_info_t        info;
    dev_sensor_config_t      config;
}dev_sensor_full_info_t;

typedef struct _dev_sensor_data_t {
    uint64_t    timestamp;
    int32_t     data[3];
}dev_sensor_data_t;

typedef struct _dev_sensor_pkg_t {
    sensor_tag_e             tag;
    union{
        dev_sensor_info_t    info;
        dev_sensor_config_t  config;
        dev_sensor_data_t    data;
    }allocator;
}dev_sensor_pkg_t;

typedef struct _sensor_obj_t {
    char*                    path;
    sensor_tag_e             tag;
    dev_io_port_e            io_port;
    work_mode_e              mode;
    dev_power_mode_e         power;
    gpio_dev_t               gpio;
    dev_sensor_full_info_t   info;
    uint8_t                  ref;
    int (*open)(void);
    int (*close)(void);
    int (*read)(void *, size_t);
    int (*write)(const void *buf, size_t len);
    int (*ioctl)(int cmd, unsigned long arg);
    void(*irq_handle)(void);
}sensor_obj_t;

typedef struct _sensor_node_t{
    sensor_tag_e  tag;
    char         *path;
    int           fd;
} sensor_node_t;

typedef enum{
    ACC_RANGE_2G,
    ACC_RANGE_4G,
    ACC_RANGE_8G,
    ACC_RANGE_16G,
    ACC_RANGE_MAX
}acc_range_e;

typedef enum{
    GYRO_RANGE_125DPS,
    GYRO_RANGE_250DPS,
    GYRO_RANGE_500DPS,
    GYRO_RANGE_1000DPS,
    GYRO_RANGE_2000DPS,
    GYRO_RANGE_MAX
}gyro_range_e;


typedef struct _gps_time
{
    int     year;       
    int     mon;
    int     day;
    int     hour;
    int     min;
    int     sec;
    int     hsec;
} gps_time_t;


typedef struct _dev_gps_data_t {
    uint64_t    timestamp;
    gps_time_t  utc;
    float  lat;        
    float  lon;        
    float  elv;
}gps_data_t;

    
#endif /* HAL_SENSOR_H */

