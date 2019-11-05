#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR507_I2C_SLAVE_ADDR                           0x23

#define LTR507_ALS_CONTR                                0x80 /* ALS operation mode, SW reset */
#define LTR507_PS_CONTR                                 0x81 /* PS operation mode */
#define LTR507_PS_LED                                   0x82 /* LED pulse freq, current duty, peak current */
#define LTR507_PS_N_PULSES                              0x83 /* PS number of pulses */
#define LTR507_PS_MEAS_RATE                             0x84 /* measurement rate*/
#define LTR507_ALS_MEAS_RATE                            0x85 /* ALS integ time, measurement rate*/
#define LTR507_PART_ID                                  0x86
#define LTR507_MANUFAC_ID                               0x87
#define LTR507_ALS_DATA_0                               0x88
#define LTR507_ALS_DATA_1                               0x89
#define LTR507_ALS_PS_STATUS                            0x8A
#define LTR507_PS_DATA_0                                0x8B
#define LTR507_PS_DATA_1                                0x8C
#define LTR507_ALS_DATA_CH1_0                           0x8D
#define LTR507_ALS_DATA_CH1_1                           0x8E
#define LTR507_ALS_DATA_CH1_2                           0x8F
#define LTR507_ALS_DATA_CH2_0                           0x90
#define LTR507_ALS_DATA_CH2_1                           0x91
#define LTR507_ALS_DATA_CH2_2                           0x92
#define LTR507_ALS_COEFF1_DATA_0                        0x93
#define LTR507_ALS_COEFF1_DATA_1                        0x94
#define LTR507_ALS_COEFF2_DATA_0                        0x95
#define LTR507_ALS_COEFF2_DATA_1                        0x96
#define LTR507_ALS_IRF_CUT_OFF                          0x97
#define LTR507_INTERRUPT                                0x98
#define LTR507_PS_THRES_UP_0                            0x99 /* ps interrupt upper threshold, lower byte */
#define LTR507_PS_THRES_UP_1                            0x9A /* ps interrupt upper threshold, upper byte */
#define LTR507_PS_THRES_LOW_0                           0x9B /* ps interrupt lower threshold, lower byte */
#define LTR507_PS_THRES_LOW_1                           0x9C /* ps interrupt lower threshold, upper byte */
#define LTR507_ALS_THRES_UP_0                           0x9E /* ALS interrupt upper threshold, lower byte */
#define LTR507_ALS_THRES_UP_1                           0x9F /* ALS interrupt upper threshold, upper byte */
#define LTR507_ALS_THRES_LOW_0                          0xA0 /* ALS interrupt lower threshold, lower byte */
#define LTR507_ALS_THRES_LOW_1                          0xA1 /* ALS interrupt lower threshold, upper byte */
#define LTR507_INTR_PRST                                0xA4 /* ALS/PS interrupt persist setting */
#define LTR507_MAX_REG                                  0xA5

#define LTR507_ADDR_TRANS(n)                            ((n) << 1)
#define LTR507_I2C_ADDR                                 LTR507_ADDR_TRANS(LTR507_I2C_SLAVE_ADDR)
#define LTR507_PART_ID_VAL                              0x91
#define LTR507_MANUFAC_ID_VAL                           0x05

#define LTR507_ALS_CONTR_REG_ALS_GAIN__POS              (3)
#define LTR507_ALS_CONTR_REG_ALS_GAIN__MSK              (0x18)
#define LTR507_ALS_CONTR_REG_ALS_GAIN__REG              (LTR507_ALS_CONTR)

#define LTR507_ALS_CONTR_REG_ALS_SW_RESET__POS          (2)
#define LTR507_ALS_CONTR_REG_ALS_SW_RESET__MSK          (0x04)
#define LTR507_ALS_CONTR_REG_ALS_SW_RESET__REG          (LTR507_ALS_CONTR)

#define LTR507_ALS_CONTR_REG_ALS_MODE__POS              (1)
#define LTR507_ALS_CONTR_REG_ALS_MODE__MSK              (0x02)
#define LTR507_ALS_CONTR_REG_ALS_MODE__REG              (LTR507_ALS_CONTR)

#define LTR507_PS_CONTR_REG_PS_GAIN__POS                (2)
#define LTR507_PS_CONTR_REG_PS_GAIN__MSK                (0x0C)
#define LTR507_PS_CONTR_REG_PS_GAIN__REG                (LTR507_PS_CONTR)

#define LTR507_PS_CONTR_REG_PS_MODE__POS                (1)
#define LTR507_PS_CONTR_REG_PS_MODE__MSK                (0x02)
#define LTR507_PS_CONTR_REG_PS_MODE__REG                (LTR507_PS_CONTR)

#define LTR507_PS_LED_REG_PEAK_CURR__POS                (0)
#define LTR507_PS_LED_REG_PEAK_CURR__MSK                (0x07)
#define LTR507_PS_LED_REG_PEAK_CURR__REG                (LTR507_PS_LED)

#define LTR507_PS_LED_REG_DUTY_CYCLE__POS               (3)
#define LTR507_PS_LED_REG_DUTY_CYCLE__MSK               (0x18)
#define LTR507_PS_LED_REG_DUTY_CYCLE__REG               (LTR507_PS_LED)

#define LTR507_PS_LED_REG_PULSE_FREQ__POS               (5)
#define LTR507_PS_LED_REG_PULSE_FREQ__MSK               (0xE0)
#define LTR507_PS_LED_REG_PULSE_FREQ__REG               (LTR507_PS_LED)

#define LTR507_PS_MEAS_RATE_REG_MEAS_RATE__POS          (0)
#define LTR507_PS_MEAS_RATE_REG_MEAS_RATE__MSK          (0x07)
#define LTR507_PS_MEAS_RATE_REG_MEAS_RATE__REG          (LTR507_PS_MEAS_RATE)

#define LTR507_ALS_MEAS_RATE_REG_ADC_RES__POS           (5)
#define LTR507_ALS_MEAS_RATE_REG_ADC_RES__MSK           (0xE0)
#define LTR507_ALS_MEAS_RATE_REG_ADC_RES__REG           (LTR507_ALS_MEAS_RATE)

#define LTR507_ALS_MEAS_RATE_REG_MEAS_RATE__POS         (0)
#define LTR507_ALS_MEAS_RATE_REG_MEAS_RATE__MSK         (0x07)
#define LTR507_ALS_MEAS_RATE_REG_MEAS_RATE__REG         (LTR507_ALS_MEAS_RATE)

#define LTR507_ALS_PS_STATUS_REG_INT_SOURCE__POS        (4)
#define LTR507_ALS_PS_STATUS_REG_INT_SOURCE__MSK        (0x30)
#define LTR507_ALS_PS_STATUS_REG_INT_SOURCE__REG        (LTR507_ALS_PS_STATUS)

#define LTR507_ALS_PS_STATUS_REG_ALS_INT_STATUS__POS    (3)
#define LTR507_ALS_PS_STATUS_REG_ALS_INT_STATUS__MSK    (0x08)
#define LTR507_ALS_PS_STATUS_REG_ALS_INT_STATUS__REG    (LTR507_ALS_PS_STATUS)

#define LTR507_ALS_PS_STATUS_REG_ALS_DATA_STATUS__POS   (2)
#define LTR507_ALS_PS_STATUS_REG_ALS_DATA_STATUS__MSK   (0x04)
#define LTR507_ALS_PS_STATUS_REG_ALS_DATA_STATUS__REG   (LTR507_ALS_PS_STATUS)

#define LTR507_ALS_PS_STATUS_REG_PS_INT_STATUS__POS     (1)
#define LTR507_ALS_PS_STATUS_REG_PS_INT_STATUS__MSK     (0x02)
#define LTR507_ALS_PS_STATUS_REG_PS_INT_STATUS__REG     (LTR507_ALS_PS_STATUS)

#define LTR507_ALS_PS_STATUS_REG_PS_DATA_STATUS__POS    (0)
#define LTR507_ALS_PS_STATUS_REG_PS_DATA_STATUS__MSK    (0x01)
#define LTR507_ALS_PS_STATUS_REG_PS_DATA_STATUS__REG    (LTR507_ALS_PS_STATUS)

#define LTR507_GET_BITSLICE(regvar, bitname)            ((regvar & LTR507_##bitname##__MSK) >> LTR507_##bitname##__POS)
#define LTR507_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR507_##bitname##__MSK) | ((val<<LTR507_##bitname##__POS)&LTR507_##bitname##__MSK))

#define LTR507_WAIT_TIME_PER_CHECK                      (10)
#define LTR507_WAIT_TIME_TOTAL                          (100)

typedef enum {
    LTR507_ALS_GAIN_1 = 0x00,                           /* 1 lux to 64k lux (default) */
    LTR507_ALS_GAIN_2 = 0x01,                           /* 0.5 lux to 32k lux */
    LTR507_ALS_GAIN_3 = 0x02,                           /* 0.02 lux to 640 lux */
    LTR507_ALS_GAIN_4 = 0x03,                           /* 0.01 lux to 320 lux */
} LTR507_CFG_ALS_Gain;

typedef enum {
    LTR507_SW_RESET_NEG = 0x00,
    LTR507_SW_RESET = 0x01, 
} LTR507_CFG_SW_RESET;

typedef enum {
    LTR507_ALS_STANDBY = 0x00,
    LTR507_ALS_ACTIVE = 0x01, 
} LTR507_CFG_ALS_MODE;

typedef enum {
    LTR507_PS_GAIN = 0x03,
} LTR507_CFG_PS_GAIN;

typedef enum {
    LTR507_PS_STANDBY = 0x00,
    LTR507_PS_ACTIVE = 0x01, 
} LTR507_CFG_PS_MODE;

typedef enum {
    LTR507_PS_PULSE_FREQ_30K = 0x00,                    /* LED pulse period = 30kHz */
    LTR507_PS_PULSE_FREQ_40K = 0x01,                    /* LED pulse period = 40kHz */
    LTR507_PS_PULSE_FREQ_50K = 0x02,                    /* LED pulse period = 50kHz */
    LTR507_PS_PULSE_FREQ_60K = 0x03,                    /* LED pulse period = 60kHz (default) */
    LTR507_PS_PULSE_FREQ_70K = 0x04,                    /* LED pulse period = 70kHz */
    LTR507_PS_PULSE_FREQ_80K = 0x05,                    /* LED pulse period = 80kHz */
    LTR507_PS_PULSE_FREQ_90K = 0x06,                    /* LED pulse period = 90kHz */
    LTR507_PS_PULSE_FREQ_100K = 0x07,                   /* LED pulse period = 100kHz */
} LTR507_CFG_PS_LED_PULSE_FREQ;

typedef enum {
    LTR507_PS_DUTY_CYCLE = 0x01,
} LTR507_CFG_PS_LED_DUTY_CYCLE;

typedef enum {
    LTR507_PS_PEAK_CURRENT_5 = 0x00,                    /* LED pulse current level = 5mA */
    LTR507_PS_PEAK_CURRENT_10 = 0x01,                   /* LED pulse current level = 10mA */
    LTR507_PS_PEAK_CURRENT_20 = 0x02,                   /* LED pulse current level = 20mA */
    LTR507_PS_PEAK_CURRENT_50 = 0x03,                   /* LED pulse current level = 50mA (default) */
    LTR507_PS_PEAK_CURRENT_100 = 0x04,                  /* LED pulse current level = 100mA */
} LTR507_CFG_PS_LED_PEAK_CURRENT;

typedef enum {
    LTR507_PS_MEAS_RATE_12_5 = 0x00,                    /* PS Measurement Repeat Rate = 12.5ms */
    LTR507_PS_MEAS_RATE_50 = 0x01,                      /* PS Measurement Repeat Rate = 50ms */
    LTR507_PS_MEAS_RATE_70 = 0x02,                      /* PS Measurement Repeat Rate = 70ms */
    LTR507_PS_MEAS_RATE_100 = 0x03,                     /* PS Measurement Repeat Rate = 100ms (default) */
    LTR507_PS_MEAS_RATE_200 = 0x04,                     /* PS Measurement Repeat Rate = 200ms */
    LTR507_PS_MEAS_RATE_500 = 0x05,                     /* PS Measurement Repeat Rate = 500ms */
    LTR507_PS_MEAS_RATE_1000 = 0x06,                    /* PS Measurement Repeat Rate = 1000ms */
    LTR507_PS_MEAS_RATE_2000 = 0x07,                    /* PS Measurement Repeat Rate = 2000ms */
} LTR507_CFG_PS_MEAS_RATE;

typedef enum {
    LTR507_ALS_ADC_RES_20BIT = 0x00,                    /* 20 bit, integration time - 1200ms */
    LTR507_ALS_ADC_RES_19BIT = 0x01,                    /* 19 bit, integration time - 600ms */
    LTR507_ALS_ADC_RES_18BIT = 0x02,                    /* 18 bit, integration time - 300ms */
    LTR507_ALS_ADC_RES_17BIT = 0x03,                    /* 17 bit, integration time - 150ms */
    LTR507_ALS_ADC_RES_16BIT = 0x04,                    /* 16 bit, integration time - 75ms (default) */
    LTR507_ALS_ADC_RES_12BIT = 0x05,                    /* 12 bit, integration time - 4.685ms */
    LTR507_ALS_ADC_RES_8BIT = 0x06,                     /* 8 bit, integration time - 292us */
    LTR507_ALS_ADC_RES_4BIT = 0x07,                     /* 4 bit, integration time - 18us */
} LTR507_CFG_ALS_ADC_RESOLUTION;

typedef enum {
    LTR507_ALS_MEAS_RATE_100 = 0x00,                    /* ALS Measurement Repeat Rate = 100ms (default) */
    LTR507_ALS_MEAS_RATE_200 = 0x01,                    /* ALS Measurement Repeat Rate = 200ms */
    LTR507_ALS_MEAS_RATE_500 = 0x02,                    /* ALS Measurement Repeat Rate = 500ms */
    LTR507_ALS_MEAS_RATE_1000 = 0x03,                   /* ALS Measurement Repeat Rate = 1000ms */
    LTR507_ALS_MEAS_RATE_2000 = 0x04,                   /* ALS Measurement Repeat Rate = 2000ms */
} LTR507_CFG_ALS_MEAS_RATE;

typedef enum {
    LTR507_INT_NEG = 0x00,
    LTR507_INT_PS = 0x01,
    LTR507_INT_ALS = 0x02,
} LTR507_CFG_INT_SOURCE;

typedef enum {
    LTR507_ALS_INT_CLEAR = 0x00,
    LTR507_ALS_INT_TRIG = 0x01,
} LTR507_CFG_ALS_INT_STATUS;

typedef enum {
    LTR507_ALS_DATA_STATUS_OLD = 0x00,
    LTR507_ALS_DATA_STATUS_NEW = 0x01,
} LTR507_CFG_ALS_DATA_STATUS;

typedef enum {
    LTR507_PS_INT_CLEAR = 0x00,
    LTR507_PS_INT_TRIG = 0x01,
} LTR507_CFG_PS_INT_STATUS;

typedef enum {
    LTR507_PS_DATA_STATUS_OLD = 0x00,
    LTR507_PS_DATA_STATUS_NEW = 0x01,
} LTR507_CFG_PS_DATA_STATUS;

typedef enum {
    LTR507_FLAG_INIT_ALS = 0,
    LTR507_FLAG_INIT_PS,
} LTR507_FLAG_INIT_BIT;

i2c_dev_t ltr507_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR507_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_als_ps_liteon_ltr507_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR507_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR507_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_als_liteon_ltr507_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR507_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR507_SET_BITSLICE(value, ALS_CONTR_REG_ALS_MODE, LTR507_ALS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR507_SET_BITSLICE(value, ALS_CONTR_REG_ALS_MODE, LTR507_ALS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR507_ALS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr507_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR507_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR507_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR507_PS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR507_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR507_PS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR507_PS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_als_liteon_ltr507_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR507_ALS_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR507_GET_BITSLICE(value, ALS_PS_STATUS_REG_ALS_DATA_STATUS) == LTR507_ALS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

UNUSED static int drv_ps_liteon_ltr507_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR507_ALS_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR507_GET_BITSLICE(value, ALS_PS_STATUS_REG_PS_DATA_STATUS) == LTR507_PS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_als_liteon_ltr507_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR507_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR507_SET_BITSLICE(value, ALS_CONTR_REG_ALS_GAIN, LTR507_ALS_GAIN_1);
    ret = sensor_i2c_write(drv, LTR507_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR507_SET_BITSLICE(value, ALS_MEAS_RATE_REG_MEAS_RATE, LTR507_ALS_MEAS_RATE_500);
    value = LTR507_SET_BITSLICE(value, ALS_MEAS_RATE_REG_ADC_RES, LTR507_ALS_ADC_RES_16BIT);
    ret = sensor_i2c_write(drv, LTR507_ALS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr507_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR507_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR507_SET_BITSLICE(value, PS_CONTR_REG_PS_GAIN, LTR507_PS_GAIN);
    ret = sensor_i2c_write(drv, LTR507_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR507_SET_BITSLICE(value, PS_LED_REG_PEAK_CURR, LTR507_PS_PEAK_CURRENT_50);
    value = LTR507_SET_BITSLICE(value, PS_LED_REG_DUTY_CYCLE, LTR507_PS_DUTY_CYCLE);
    value = LTR507_SET_BITSLICE(value, PS_LED_REG_PULSE_FREQ, LTR507_PS_PULSE_FREQ_60K);
    ret = sensor_i2c_write(drv, LTR507_PS_LED, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 127;
    ret = sensor_i2c_write(drv, LTR507_PS_N_PULSES, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR507_SET_BITSLICE(value, PS_MEAS_RATE_REG_MEAS_RATE, LTR507_PS_MEAS_RATE_100);
    ret = sensor_i2c_write(drv, LTR507_PS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_als_liteon_ltr507_irq_handle(void)
{
    /* no handle so far */
}

static int drv_als_liteon_ltr507_open(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr507_set_power_mode(&ltr507_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_liteon_ltr507_close(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr507_set_power_mode(&ltr507_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_liteon_ltr507_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_data[2] = { 0 };
    als_data_t * pdata = (als_data_t *) buf;

    if (buf == NULL){
        return -1;
    }

    size = sizeof(als_data_t);
    if (len < size){
        return -1;
    }


    ret = sensor_i2c_read(&ltr507_ctx, LTR507_ALS_DATA_0, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr507_ctx, LTR507_ALS_DATA_1, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->lux = ((uint32_t) reg_data[1] << 8 | reg_data[0]);
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_als_liteon_ltr507_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_als_liteon_ltr507_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_als_liteon_ltr507_set_power_mode(&ltr507_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR507";
            info->unit = lux;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_ps_liteon_ltr507_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ps_liteon_ltr507_open(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr507_set_power_mode(&ltr507_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr507_close(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr507_set_power_mode(&ltr507_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_liteon_ltr507_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_data[2] = { 0 };
    proximity_data_t * pdata = (proximity_data_t *) buf;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(proximity_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr507_ctx, LTR507_PS_DATA_0, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr507_ctx, LTR507_PS_DATA_1, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    if (((reg_data[1] & 0x10) >> 4) == 0x00) {
        pdata->present = (((uint32_t) (reg_data[1] & 0x07) << 8) | reg_data[0]);
    }
    else {
        pdata->present = 0;
    }
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_ps_liteon_ltr507_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_ps_liteon_ltr507_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_ps_liteon_ltr507_set_power_mode(&ltr507_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR507";
            info->unit = cm;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_als_liteon_ltr507_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_als;

    memset(&sensor_als, 0, sizeof(sensor_als));

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr507_validate_id(&ltr507_ctx, LTR507_PART_ID_VAL, LTR507_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << LTR507_FLAG_INIT_ALS))) {
        /* fill the sensor_als obj parameters here */
        sensor_als.path = dev_als_path;
        sensor_als.tag = TAG_DEV_ALS;
        sensor_als.io_port = I2C_PORT;
        sensor_als.mode = DEV_POLLING;
        sensor_als.power = DEV_POWER_OFF;
        sensor_als.open = drv_als_liteon_ltr507_open;
        sensor_als.close = drv_als_liteon_ltr507_close;
        sensor_als.read = drv_als_liteon_ltr507_read;
        sensor_als.write = drv_als_liteon_ltr507_write;
        sensor_als.ioctl = drv_als_liteon_ltr507_ioctl;
        sensor_als.irq_handle = drv_als_liteon_ltr507_irq_handle;

        ret = sensor_create_obj(&sensor_als);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_als_liteon_ltr507_set_default_config(&ltr507_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << LTR507_FLAG_INIT_ALS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_ps_liteon_ltr507_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_ps;

    memset(&sensor_ps, 0, sizeof(sensor_ps));

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr507_validate_id(&ltr507_ctx, LTR507_PART_ID_VAL, LTR507_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << LTR507_FLAG_INIT_PS))) {
        /* fill the sensor_ps obj parameters here */
        sensor_ps.tag = TAG_DEV_PS;
        sensor_ps.path = dev_ps_path;
        sensor_ps.io_port = I2C_PORT;
        sensor_ps.mode = DEV_POLLING;
        sensor_ps.power = DEV_POWER_OFF;
        sensor_ps.open = drv_ps_liteon_ltr507_open;
        sensor_ps.close = drv_ps_liteon_ltr507_close;
        sensor_ps.read = drv_ps_liteon_ltr507_read;
        sensor_ps.write = drv_ps_liteon_ltr507_write;
        sensor_ps.ioctl = drv_ps_liteon_ltr507_ioctl;
        sensor_ps.irq_handle = drv_ps_liteon_ltr507_irq_handle;

        ret = sensor_create_obj(&sensor_ps);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr507_set_default_config(&ltr507_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << LTR507_FLAG_INIT_PS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_als_liteon_ltr507_init);
SENSOR_DRV_ADD(drv_ps_liteon_ltr507_init);

