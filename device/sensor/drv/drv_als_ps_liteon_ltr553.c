/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "hal/sensor.h"

#define LTR553_ALS_CONTR                                0x80 /* ALS operation mode, SW reset */
#define LTR553_PS_CONTR                                 0x81 /* PS operation mode */
#define LTR553_PS_LED                                   0x82 /* LED pulse freq, current duty, peak current */
#define LTR553_PS_MEAS_RATE                             0x84 /* measurement rate*/
#define LTR553_ALS_MEAS_RATE                            0x85 /* ALS integ time, measurement rate*/
#define LTR553_PART_ID                                  0x86
#define LTR553_MANUFAC_ID                               0x87
#define LTR553_ALS_DATA1_L                              0x88
#define LTR553_ALS_DATA1_H                              0x89
#define LTR553_ALS_DATA0_L                              0x8a
#define LTR553_ALS_DATA0_H                              0x8b
#define LTR553_ALS_PS_STATUS                            0x8c
#define LTR553_PS_DATA_L                                0x8d
#define LTR553_PS_DATA_H                                0x8e
#define LTR553_INTR                                     0x8f /* output mode, polarity, mode */
#define LTR553_PS_THRESH_UP                             0x90 /* 11 bit, ps upper threshold */
#define LTR553_PS_THRESH_LOW                            0x92 /* 11 bit, ps lower threshold */
#define LTR553_ALS_THRESH_UP                            0x97 /* 16 bit, ALS upper threshold */
#define LTR553_ALS_THRESH_LOW                           0x99 /* 16 bit, ALS lower threshold */
#define LTR553_INTR_PRST                                0x9e /* ps thresh, als thresh */
#define LTR553_MAX_REG                                  0x9f

#define LTR553_I2C_SLAVE_ADDR                           0x23

#define LTR553_ADDR_TRANS(n)                            ((n) << 1)
#define LTR553_I2C_ADDR                                 LTR553_ADDR_TRANS(LTR553_I2C_SLAVE_ADDR)
#define LTR553_PART_ID_VAL                              0x92
#define LTR553_MANUFAC_ID_VAL                           0x05

#define LTR553_ALS_CONTR_REG_ALS_GAIN__POS              (2)
#define LTR553_ALS_CONTR_REG_ALS_GAIN__MSK              (0x1c)
#define LTR553_ALS_CONTR_REG_ALS_GAIN__REG              (LTR553_ALS_CONTR)

#define LTR553_ALS_CONTR_REG_ALS_MODE__POS              (0)
#define LTR553_ALS_CONTR_REG_ALS_MODE__MSK              (0x01)
#define LTR553_ALS_CONTR_REG_ALS_MODE__REG              (LTR553_ALS_CONTR)

#define LTR553_ALS_MEAS_RATE_REG_INTEG_TIME__POS        (3)
#define LTR553_ALS_MEAS_RATE_REG_INTEG_TIME__MSK        (0x38)
#define LTR553_ALS_MEAS_RATE_REG_INTEG_TIME__REG        (LTR553_ALS_MEAS_RATE)

#define LTR553_ALS_MEAS_RATE_REG_MEAS_RATE__POS         (0)
#define LTR553_ALS_MEAS_RATE_REG_MEAS_RATE__MSK         (0x07)
#define LTR553_ALS_MEAS_RATE_REG_MEAS_RATE__REG         (LTR553_ALS_MEAS_RATE)

#define LTR553_ALS_PS_STATUS_REG_ALS_STATUS__POS        (2)
#define LTR553_ALS_PS_STATUS_REG_ALS_STATUS__MSK        (0x04)
#define LTR553_ALS_PS_STATUS_REG_ALS_STATUS__REG        (LTR553_ALS_PS_STATUS)

#define LTR553_PS_CONTR_REG_PS_GAIN__POS                (2)
#define LTR553_PS_CONTR_REG_PS_GAIN__MSK                (0x0c)
#define LTR553_PS_CONTR_REG_PS_GAIN__REG                (LTR553_PS_CONTR)

#define LTR553_PS_CONTR_REG_PS_MODE__POS                (0)
#define LTR553_PS_CONTR_REG_PS_MODE__MSK                (0x03)
#define LTR553_PS_CONTR_REG_PS_MODE__REG                (LTR553_PS_CONTR)

#define LTR553_PS_LED_REG_PLUSE_FREQ__POS               (5)
#define LTR553_PS_LED_REG_PLUSE_FREQ__MSK               (0xe0)
#define LTR553_PS_LED_REG_PLUSE_FREQ__REG               (LTR553_PS_LED)

#define LTR553_PS_LED_REG_CURRENT_DUTY__POS             (3)
#define LTR553_PS_LED_REG_CURRENT_DUTY__MSK             (0x18)
#define LTR553_PS_LED_REG_CURRENT_DUTY__REG             (LTR553_PS_LED)

#define LTR553_PS_LED_REG_CURRENT__POS                  (0)
#define LTR553_PS_LED_REG_CURRENT__MSK                  (0x07)
#define LTR553_PS_LED_REG_CURRENT__REG                  (LTR553_PS_LED)

#define LTR553_PS_MEAS_RATE_REG_MEAS_RATE__POS          (0)
#define LTR553_PS_MEAS_RATE_REG_MEAS_RATE__MSK          (0x0F)
#define LTR553_PS_MEAS_RATE_REG_MEAS_RATE__REG          (LTR553_PS_MEAS_RATE)

#define LTR553_ALS_PS_STATUS_REG_PS_STATUS__POS         (0)
#define LTR553_ALS_PS_STATUS_REG_PS_STATUS__MSK         (0x01)
#define LTR553_ALS_PS_STATUS_REG_PS_STATUS__REG         (LTR553_ALS_PS_STATUS)

#define LTR553_GET_BITSLICE(regvar, bitname)            ((regvar & bitname##__MSK) >> bitname##__POS)
#define LTR553_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

#define LTR553_WAIT_TIME_PER_CHECK                      (10)
#define LTR553_WAIT_TIME_TOTAL                          (1000)

typedef enum {
    AG_GAIN_1X = 0x0,                                   /* 1 lux to 64k lux (default) */
    AG_GAIN_2X = 0x1,                                   /* 0.5 lux to 32k lux */
    AG_GAIN_4X = 0x2,                                   /* 0.25 lux to 16k lux */
    AG_GAIN_8X = 0x3,                                   /* 0.125 lux to 8k lux */
    AG_GAIN_48X = 0x6,                                  /* 0.02 lux to 1.3k lux */
    AG_GAIN_96X = 0x7,                                  /* 0.01 lux to 600 lux */
} CFG_ALS_Gain;

typedef enum {
    PG_GAIN_X16 = 0x0,                                  /* X16 (default) */
    PG_GAIN_X32 = 0x2,                                  /* X32 */
    PG_GAIN_X64 = 0x3,                                  /* X64 */
} CFG_PS_Gain;

typedef enum {
    LPMF_PERIOD_30K = 0x0,                              /* LED pulse period = 30kHz */
    LPMF_PERIOD_40K = 0x1,                              /* LED pulse period = 40kHz */
    LPMF_PERIOD_50K = 0x2,                              /* LED pulse period = 50kHz */
    LPMF_PERIOD_60K = 0x3,                              /* LED pulse period = 60kHz(default) */
    LPMF_PERIOD_70K = 0x4,                              /* LED pulse period = 70kHz */
    LPMF_PERIOD_80K = 0x5,                              /* LED pulse period = 80kHz */
    LPMF_PERIOD_90K = 0x6,                              /* LED pulse period = 90kHz */
    LPMF_PERIOD_100K = 0x7,                             /* LED pulse period = 100kHz */
} CFG_LED_pulse_Modulation_Frequency;

typedef enum {
    LCD_PER_25 = 0x0,                                   /* DUTY = 25% */
    LCD_PER_50 = 0x1,                                   /* DUTY = 50% */
    LCD_PER_75 = 0x2,                                   /* DUTY = 75% */
    LCD_PER_100 = 0x3,                                  /* DUTY = 100%(default) */
} CFG_LED_Current_DUTY;

typedef enum {
    LC_LEVEL_5 = 0x0,                                   /* LED pulse current level = 5mA */
    LC_LEVEL_10 = 0x1,                                  /* LED pulse current level = 10mA */
    LC_LEVEL_20 = 0x2,                                  /* LED pulse current level = 20mA */
    LC_LEVEL_50 = 0x3,                                  /* LED pulse current level = 50mA */
    LC_LEVEL_100 = 0x4,                                 /* LED pulse current level = 100mA(default) */
} CFG_LED_Current;

typedef enum {
    PMR_RATE_50 = 0x0,                                  /* PS Measurement Repeat Rate = 50ms */
    PMR_RATE_70 = 0x1,                                  /* PS Measurement Repeat Rate = 70ms */
    PMR_RATE_100 = 0x2,                                 /* PS Measurement Repeat Rate = 100ms(default) */
    PMR_RATE_200 = 0x3,                                 /* PS Measurement Repeat Rate = 200ms */
    PMR_RATE_500 = 0x4,                                 /* PS Measurement Repeat Rate = 500ms */
    PMR_RATE_1000 = 0x5,                                /* PS Measurement Repeat Rate = 1000ms */
    PMR_RATE_2000 = 0x6,                                /* PS Measurement Repeat Rate = 2000ms */
    PMR_RATE_10 = 0x8,                                  /* PS Measurement Repeat Rate = 10ms */
} CFG_PS_measurement_rate;

typedef enum {
    AIT_TIME_100 = 0x0,                                 /* ALS integration_time = 100ms(default) */
    AIT_TIME_50 = 0x1,                                  /* ALS integration_time = 50ms */
    AIT_TIME_200 = 0x2,                                 /* ALS integration_time = 200ms */
    AIT_TIME_400 = 0x3,                                 /* ALS integration_time = 400ms */
    AIT_TIME_150 = 0x4,                                 /* ALS integration_time = 150ms */
    AIT_TIME_250 = 0x5,                                 /* ALS integration_time = 250ms */
    AIT_TIME_300 = 0x6,                                 /* ALS integration_time = 300ms */
    AIT_TIME_350 = 0x7,                                 /* ALS integration_time = 350ms */
} CFG_ALS_integration_time;

typedef enum {
    AMR_RATE_50 = 0x0,                                  /* ALS Measurement Repeat Rate = 50ms */
    AMR_RATE_100 = 0x1,                                 /* ALS Measurement Repeat Rate = 100ms */
    AMR_RATE_200 = 0x2,                                 /* ALS Measurement Repeat Rate = 200ms */
    AMR_RATE_500 = 0x3,                                 /* ALS Measurement Repeat Rate = 500ms(default) */
    AMR_RATE_1000 = 0x4,                                /* ALS Measurement Repeat Rate = 1000ms */
    AMR_RATE_2000 = 0x5,                                /* ALS Measurement Repeat Rate = 2000ms */
} CFG_ALS_measurement_rate;

typedef enum {
    PM_MODE_STANDBY = 0,
    PM_MODE_ACTIVE = 3,
} CFG_PS_mode;

typedef enum {
    AM_MODE_STANDBY = 0,
    AM_MODE_ACTIVE = 1,
} CFG_ALS_mode;

typedef enum {
    ADS_STATUS_OLD = 0,
    ADS_STATUS_NEW = 1,
} CFG_ALS_data_status;

typedef enum {
    PDS_STATUS_OLD = 0,
    PDS_STATUS_NEW = 1,
} CFG_PS_data_status;

typedef enum {
    FLAG_INIT_ALS = 0,
    FLAG_INIT_PS,
} FLAG_INIT_BIT;

i2c_dev_t ltr553_ctx = {
    .port = 2,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR553_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_als_ps_liteon_ltr553_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if(drv == NULL){
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR553_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR553_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_als_liteon_ltr553_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;
    int     wait_time = 0;

    ret = sensor_i2c_read(drv, LTR553_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch(mode){
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR553_SET_BITSLICE(dev_mode, LTR553_ALS_CONTR_REG_ALS_MODE, AM_MODE_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR553_SET_BITSLICE(dev_mode, LTR553_ALS_CONTR_REG_ALS_MODE, AM_MODE_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR553_ALS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr553_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR553_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch(mode){
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR553_SET_BITSLICE(value, LTR553_PS_CONTR_REG_PS_MODE, PM_MODE_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR553_SET_BITSLICE(value, LTR553_PS_CONTR_REG_PS_MODE, PM_MODE_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR553_PS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_als_liteon_ltr553_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR553_ALS_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR553_GET_BITSLICE(value, LTR553_ALS_PS_STATUS_REG_ALS_STATUS) == ADS_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_ps_liteon_ltr553_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR553_ALS_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR553_GET_BITSLICE(value, LTR553_ALS_PS_STATUS_REG_PS_STATUS) == PDS_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_als_liteon_ltr553_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    value = LTR553_SET_BITSLICE(value, LTR553_ALS_MEAS_RATE_REG_INTEG_TIME, AIT_TIME_100);
    value = LTR553_SET_BITSLICE(value, LTR553_ALS_MEAS_RATE_REG_MEAS_RATE, AMR_RATE_500);
    ret = sensor_i2c_write(drv, LTR553_ALS_MEAS_RATE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR553_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR553_SET_BITSLICE(value, LTR553_ALS_CONTR_REG_ALS_GAIN, AG_GAIN_1X);
    ret = sensor_i2c_write(drv, LTR553_ALS_CONTR,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr553_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    value = LTR553_SET_BITSLICE(value, LTR553_PS_MEAS_RATE_REG_MEAS_RATE, PMR_RATE_100);
    ret = sensor_i2c_write(drv, LTR553_PS_MEAS_RATE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR553_SET_BITSLICE(value, LTR553_PS_LED_REG_PLUSE_FREQ, LPMF_PERIOD_60K);
    value = LTR553_SET_BITSLICE(value, LTR553_PS_LED_REG_CURRENT_DUTY, LCD_PER_100);
    value = LTR553_SET_BITSLICE(value, LTR553_PS_LED_REG_CURRENT, LC_LEVEL_100);
    ret = sensor_i2c_write(drv, LTR553_PS_MEAS_RATE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR553_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR553_SET_BITSLICE(value, LTR553_PS_CONTR_REG_PS_GAIN, PG_GAIN_X16);
    ret = sensor_i2c_write(drv, LTR553_PS_CONTR,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_als_liteon_ltr553_irq_handle(void)
{
    /* no handle so far */
}

static int drv_als_liteon_ltr553_open(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr553_set_power_mode(&ltr553_ctx, DEV_POWER_ON);
    if(unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_liteon_ltr553_close(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr553_set_power_mode(&ltr553_ctx, DEV_POWER_OFF);
    if(unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_liteon_ltr553_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint16_t als_raw[2] = {0};
    uint8_t reg_ch0[2] = {0};
    uint8_t reg_ch1[2] = {0};
    als_data_t* pdata = (als_data_t*)buf;
    int wait_time = 0;

    if(buf == NULL){
        return -1;
    }

    size = sizeof(als_data_t);
    if(len < size){
        return -1;
    }

    while (!drv_als_liteon_ltr553_is_ready(&ltr553_ctx)) {
        krhino_task_sleep(krhino_ms_to_ticks(LTR553_WAIT_TIME_PER_CHECK));
        wait_time += LTR553_WAIT_TIME_PER_CHECK;
        if (wait_time > LTR553_WAIT_TIME_TOTAL) {
            return -1;
        }
    }

    ret = sensor_i2c_read(&ltr553_ctx, LTR553_ALS_DATA1_L, &reg_ch1[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr553_ctx, LTR553_ALS_DATA1_H, &reg_ch1[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr553_ctx, LTR553_ALS_DATA0_L, &reg_ch0[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr553_ctx, LTR553_ALS_DATA0_H, &reg_ch0[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    als_raw[0] = (uint16_t)reg_ch0[1] << 8 | reg_ch0[0];
    als_raw[1] = (uint16_t)reg_ch1[1] << 8 | reg_ch1[0];
    pdata->lux = (uint32_t)((als_raw[0] + als_raw[1]) >> 1);

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_als_liteon_ltr553_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_als_liteon_ltr553_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_als_liteon_ltr553_set_power_mode(&ltr553_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LTR553";
            info->unit = lux;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_ps_liteon_ltr553_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ps_liteon_ltr553_open(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr553_set_power_mode(&ltr553_ctx, DEV_POWER_ON);
    if(unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr553_close(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr553_set_power_mode(&ltr553_ctx, DEV_POWER_OFF);
    if(unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_liteon_ltr553_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_data[2] = {0};
    proximity_data_t* pdata = (proximity_data_t*)buf;
    int wait_time = 0;

    if(buf == NULL){
        return -1;
    }

    size = sizeof(proximity_data_t);
    if(len < size){
        return -1;
    }

    while (!drv_ps_liteon_ltr553_is_ready(&ltr553_ctx)) {
        krhino_task_sleep(krhino_ms_to_ticks(LTR553_WAIT_TIME_PER_CHECK));
        wait_time += LTR553_WAIT_TIME_PER_CHECK;
        if (wait_time > LTR553_WAIT_TIME_TOTAL) {
            return -1;
        }
    }

    ret = sensor_i2c_read(&ltr553_ctx, LTR553_PS_DATA_L, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr553_ctx, LTR553_PS_DATA_H, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->present = (uint32_t)(reg_data[1] << 8 | reg_data[0]);

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_ps_liteon_ltr553_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_ps_liteon_ltr553_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_ps_liteon_ltr553_set_power_mode(&ltr553_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LTR553";
            info->unit = cm;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_als_liteon_ltr553_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_als;

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr553_validate_id(&ltr553_ctx, LTR553_PART_ID_VAL, LTR553_MANUFAC_ID_VAL);
        if(unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_ALS))) {
        /* fill the sensor_als obj parameters here */
        sensor_als.tag = TAG_DEV_ALS;
        sensor_als.path = dev_als_path;
        sensor_als.io_port = I2C_PORT;
        sensor_als.open = drv_als_liteon_ltr553_open;
        sensor_als.close = drv_als_liteon_ltr553_close;
        sensor_als.read = drv_als_liteon_ltr553_read;
        sensor_als.write = drv_als_liteon_ltr553_write;
        sensor_als.ioctl = drv_als_liteon_ltr553_ioctl;
        sensor_als.irq_handle = drv_als_liteon_ltr553_irq_handle;

        ret = sensor_create_obj(&sensor_als);
        if(unlikely(ret)) {
            return -1;
        }

        ret = drv_als_liteon_ltr553_set_default_config(&ltr553_ctx);
        if(unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_ALS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_ps_liteon_ltr553_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_ps;

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr553_validate_id(&ltr553_ctx, LTR553_PART_ID_VAL, LTR553_MANUFAC_ID_VAL);
        if(unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_PS))) {
        /* fill the sensor_ps obj parameters here */
        sensor_ps.tag = TAG_DEV_PS;
        sensor_ps.path = dev_ps_path;
        sensor_ps.io_port = I2C_PORT;
        sensor_ps.open = drv_ps_liteon_ltr553_open;
        sensor_ps.close = drv_ps_liteon_ltr553_close;
        sensor_ps.read = drv_ps_liteon_ltr553_read;
        sensor_ps.write = drv_ps_liteon_ltr553_write;
        sensor_ps.ioctl = drv_ps_liteon_ltr553_ioctl;
        sensor_ps.irq_handle = drv_ps_liteon_ltr553_irq_handle;

        ret = sensor_create_obj(&sensor_ps);
        if(unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr553_set_default_config(&ltr553_ctx);
        if(unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_PS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}
