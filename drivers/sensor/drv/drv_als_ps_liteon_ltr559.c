#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR559_I2C_SLAVE_ADDR                           0x23

#define LTR559_ALS_CONTR                                0x80
#define LTR559_PS_CONTR                                 0x81 /* PS operation mode */
#define LTR559_PS_LED                                   0x82 /* LED pulse freq, current duty, peak current */
#define LTR559_PS_N_PULSES                              0x83 /* PS number of pulses */
#define LTR559_PS_MEAS_RATE                             0x84 /* measurement rate*/
#define LTR559_ALS_MEAS_RATE                            0x85
#define LTR559_PART_ID                                  0x86
#define LTR559_MANUFAC_ID                               0x87
#define LTR559_ALS_DATA_CH1_0                           0x88
#define LTR559_ALS_DATA_CH1_1                           0x89
#define LTR559_ALS_DATA_CH0_0                           0x8A
#define LTR559_ALS_DATA_CH0_1                           0x8B
#define LTR559_ALS_PS_STATUS                            0x8C
#define LTR559_PS_DATA_0                                0x8D
#define LTR559_PS_DATA_1                                0x8E
#define LTR559_INTERRUPT                                0x8F
#define LTR559_PS_THRES_UP_0                            0x90 /* PS interrupt upper threshold, lower byte */
#define LTR559_PS_THRES_UP_1                            0x91 /* PS interrupt upper threshold, upper byte */
#define LTR559_PS_THRES_LOW_0                           0x92 /* PS interrupt lower threshold, lower byte */
#define LTR559_PS_THRES_LOW_1                           0x93 /* PS interrupt lower threshold, upper byte */
#define LTR559_PS_OFFSET_1                              0x94 /* PS offset, upper byte */
#define LTR559_PS_OFFSET_0                              0x95 /* PS offset, lower byte */
#define LTR559_ALS_THRES_UP_0                           0x97
#define LTR559_ALS_THRES_UP_1                           0x98
#define LTR559_ALS_THRES_LOW_0                          0x99
#define LTR559_ALS_THRES_LOW_1                          0x9A
#define LTR559_INTR_PRST                                0x9E /* PS interrupt persist setting */

#define LTR559_ADDR_TRANS(n)                            ((n) << 1)
#define LTR559_I2C_ADDR                                 LTR559_ADDR_TRANS(LTR559_I2C_SLAVE_ADDR)
#define LTR559_PART_ID_VAL                              0x92
#define LTR559_MANUFAC_ID_VAL                           0x05

#define LTR559_ALS_CONTR_REG_ALS_MODE__POS              (0)
#define LTR559_ALS_CONTR_REG_ALS_MODE__MSK              (0x01)
#define LTR559_ALS_CONTR_REG_ALS_MODE__REG              (LTR559_ALS_CONTR)

#define LTR559_ALS_CONTR_REG_SW_RESET__POS              (1)
#define LTR559_ALS_CONTR_REG_SW_RESET__MSK              (0x02)
#define LTR559_ALS_CONTR_REG_SW_RESET__REG              (LTR559_ALS_CONTR)

#define LTR559_ALS_CONTR_REG_ALS_GAIN__POS              (2)
#define LTR559_ALS_CONTR_REG_ALS_GAIN__MSK              (0x1C)
#define LTR559_ALS_CONTR_REG_ALS_GAIN__REG              (LTR559_ALS_CONTR)

#define LTR559_PS_CONTR_REG_PS_MODE__POS                (0)
#define LTR559_PS_CONTR_REG_PS_MODE__MSK                (0x03)
#define LTR559_PS_CONTR_REG_PS_MODE__REG                (LTR559_PS_CONTR)

#define LTR559_PS_CONTR_REG_PS_SAT_IND_EN__POS          (5)
#define LTR559_PS_CONTR_REG_PS_SAT_IND_EN__MSK          (0x20)
#define LTR559_PS_CONTR_REG_PS_SAT_IND_EN__REG          (LTR559_PS_CONTR)

#define LTR559_PS_LED_REG_LED_CURR__POS                 (0)
#define LTR559_PS_LED_REG_LED_CURR__MSK                 (0x07)
#define LTR559_PS_LED_REG_LED_CURR__REG                 (LTR559_PS_LED)

#define LTR559_PS_LED_REG_LED_DUTY__POS                 (3)
#define LTR559_PS_LED_REG_LED_DUTY__MSK                 (0x18)
#define LTR559_PS_LED_REG_LED_DUTY__REG                 (LTR559_PS_LED)

#define LTR559_PS_LED_REG_LED_PULSE__POS                (5)
#define LTR559_PS_LED_REG_LED_PULSE__MSK                (0xE0)
#define LTR559_PS_LED_REG_LED_PULSE__REG                (LTR559_PS_LED)

#define LTR559_PS_N_PULSES_REG_PULSES__POS              (0)
#define LTR559_PS_N_PULSES_REG_PULSES__MSK              (0x0F)
#define LTR559_PS_N_PULSES_REG_PULSES__REG              (LTR559_PS_N_PULSES)

#define LTR559_PS_MEAS_RATE_REG_MEAS_RPT_RATE__POS      (0)
#define LTR559_PS_MEAS_RATE_REG_MEAS_RPT_RATE__MSK      (0x0F)
#define LTR559_PS_MEAS_RATE_REG_MEAS_RPT_RATE__REG      (LTR559_PS_MEAS_RATE)

#define LTR559_ALS_MEAS_RATE_REG_MEAS_RPT_RATE__POS     (0)
#define LTR559_ALS_MEAS_RATE_REG_MEAS_RPT_RATE__MSK     (0x07)
#define LTR559_ALS_MEAS_RATE_REG_MEAS_RPT_RATE__REG     (LTR559_ALS_MEAS_RATE)

#define LTR559_ALS_MEAS_RATE_REG_INTEG_TIME__POS        (3)
#define LTR559_ALS_MEAS_RATE_REG_INTEG_TIME__MSK        (0x38)
#define LTR559_ALS_MEAS_RATE_REG_INTEG_TIME__REG        (LTR559_ALS_MEAS_RATE)

#define LTR559_ALS_PS_STATUS_REG_PS_DATA_STATUS__POS    (0)
#define LTR559_ALS_PS_STATUS_REG_PS_DATA_STATUS__MSK    (0x01)
#define LTR559_ALS_PS_STATUS_REG_PS_DATA_STATUS__REG    (LTR559_ALS_PS_STATUS)

#define LTR559_ALS_PS_STATUS_REG_PS_INT_STATUS__POS     (1)
#define LTR559_ALS_PS_STATUS_REG_PS_INT_STATUS__MSK     (0x02)
#define LTR559_ALS_PS_STATUS_REG_PS_INT_STATUS__REG     (LTR559_ALS_PS_STATUS)

#define LTR559_ALS_PS_STATUS_REG_ALS_DATA_STATUS__POS   (2)
#define LTR559_ALS_PS_STATUS_REG_ALS_DATA_STATUS__MSK   (0x04)
#define LTR559_ALS_PS_STATUS_REG_ALS_DATA_STATUS__REG   (LTR559_ALS_PS_STATUS)

#define LTR559_ALS_PS_STATUS_REG_ALS_INT_STATUS__POS    (3)
#define LTR559_ALS_PS_STATUS_REG_ALS_INT_STATUS__MSK    (0x08)
#define LTR559_ALS_PS_STATUS_REG_ALS_INT_STATUS__REG    (LTR559_ALS_PS_STATUS)

#define LTR559_ALS_PS_STATUS_REG_ALS_GAIN__POS          (4)
#define LTR559_ALS_PS_STATUS_REG_ALS_GAIN__MSK          (0x70)
#define LTR559_ALS_PS_STATUS_REG_ALS_GAIN__REG          (LTR559_ALS_PS_STATUS)

#define LTR559_ALS_PS_STATUS_REG_ALS_DATA_VALID__POS    (7)
#define LTR559_ALS_PS_STATUS_REG_ALS_DATA_VALID__MSK    (0x80)
#define LTR559_ALS_PS_STATUS_REG_ALS_DATA_VALID__REG    (LTR559_ALS_PS_STATUS)

#define LTR559_INTERRUPT_REG_INT_MODE__POS              (0)
#define LTR559_INTERRUPT_REG_INT_MODE__MSK              (0x03)
#define LTR559_INTERRUPT_REG_INT_MODE__REG              (LTR559_INTERRUPT)

#define LTR559_INTERRUPT_REG_INT_POLARITY__POS          (2)
#define LTR559_INTERRUPT_REG_INT_POLARITY__MSK          (0x04)
#define LTR559_INTERRUPT_REG_INT_POLARITY__REG          (LTR559_INTERRUPT)

#define LTR559_INTR_PRST_REG_ALS_PERSIST__POS           (0)
#define LTR559_INTR_PRST_REG_ALS_PERSIST__MSK           (0x0F)
#define LTR559_INTR_PRST_REG_ALS_PERSIST__REG           (LTR559_INTR_PRST)

#define LTR559_INTR_PRST_REG_PS_PERSIST__POS            (4)
#define LTR559_INTR_PRST_REG_PS_PERSIST__MSK            (0xF0)
#define LTR559_INTR_PRST_REG_PS_PERSIST__REG            (LTR559_INTR_PRST)

#define LTR559_GET_BITSLICE(regvar, bitname)            ((regvar & LTR559_##bitname##__MSK) >> LTR559_##bitname##__POS)
#define LTR559_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR559_##bitname##__MSK) | ((val<<LTR559_##bitname##__POS)&LTR559_##bitname##__MSK))

#define LTR559_WAIT_TIME_PER_CHECK                      (10)
#define LTR559_WAIT_TIME_TOTAL                          (100)

typedef enum {
    LTR559_ALS_STANDBY = 0x00,
    LTR559_ALS_ACTIVE = 0x01, 
} LTR559_CFG_ALS_MODE;

typedef enum {
    LTR559_SW_RESET_FALSE = 0x00,
    LTR559_SW_RESET_TRUE = 0x01, 
} LTR559_CFG_SW_RESET;

typedef enum {
    LTR559_ALS_GAIN_1X = 0x00,                          /* 1 lux to 64k lux (default) */
    LTR559_ALS_GAIN_2X = 0x01,                          /* 0.5 lux to 32k lux */
    LTR559_ALS_GAIN_4X = 0x02,                          /* 0.25 lux to 16k lux */
    LTR559_ALS_GAIN_8X = 0x03,                          /* 0.125 lux to 8k lux */
    LTR559_ALS_GAIN_48X = 0x06,                         /* 0.02 lux to 1.3k lux */
    LTR559_ALS_GAIN_96X = 0x07,                         /* 0.01 lux to 600 lux */
} LTR559_CFG_ALS_Gain;

typedef enum {
    LTR559_PS_STANDBY = 0x00,
    LTR559_PS_ACTIVE = 0x02, 
} LTR559_CFG_PS_MODE;

typedef enum {
    LTR559_PS_SAT_IND_FALSE = 0x00,
    LTR559_PS_SAT_IND_TRUE = 0x01, 
} LTR559_CFG_PS_SAT_IND_EN;

typedef enum {
    LTR559_LED_PEAK_CURRENT_5 = 0x00,                   /* LED pulse current level = 5mA */
    LTR559_LED_PEAK_CURRENT_10 = 0x01,                  /* LED pulse current level = 10mA */
    LTR559_LED_PEAK_CURRENT_20 = 0x02,                  /* LED pulse current level = 20mA */
    LTR559_LED_PEAK_CURRENT_50 = 0x03,                  /* LED pulse current level = 50mA */
    LTR559_LED_PEAK_CURRENT_100 = 0x04,                 /* LED pulse current level = 100mA (default) */
} LTR559_CFG_LED_PEAK_CURRENT;

typedef enum {
    LTR559_LED_DUTY_CYCLE_25PCT = 0x00,                 /* Duty = 25% */
    LTR559_LED_DUTY_CYCLE_50PCT = 0x01,                 /* Duty = 50% */
    LTR559_LED_DUTY_CYCLE_75PCT = 0x02,                 /* Duty = 75% */
    LTR559_LED_DUTY_CYCLE_100PCT = 0x03,                /* Duty = 100% (default) */
} LTR559_CFG_LED_DUTY_CYCLE;

typedef enum {
    LTR559_LED_PULSE_30kHZ = 0x00,                      /* LED pulse period = 30kHz */
    LTR559_LED_PULSE_40kHZ = 0x01,                      /* LED pulse period = 40kHz */
    LTR559_LED_PULSE_50kHZ = 0x02,                      /* LED pulse period = 50kHz */
    LTR559_LED_PULSE_60kHZ = 0x03,                      /* LED pulse period = 60kHz */
    LTR559_LED_PULSE_70kHZ = 0x04,                      /* LED pulse period = 70kHz */
    LTR559_LED_PULSE_80kHZ = 0x05,                      /* LED pulse period = 80kHz */
    LTR559_LED_PULSE_90kHZ = 0x06,                      /* LED pulse period = 90kHz */
    LTR559_LED_PULSE_100kHZ = 0x07,                     /* LED pulse period = 100kHz */
} LTR559_CFG_LED_PULSE_FREQUENCY;

typedef enum {
    LTR559_PS_MEAS_RATE_50 = 0x00,                      /* PS Measurement Repeat Rate = 50ms */
    LTR559_PS_MEAS_RATE_70 = 0x01,                      /* PS Measurement Repeat Rate = 70ms */
    LTR559_PS_MEAS_RATE_100 = 0x02,                     /* PS Measurement Repeat Rate = 100ms (default) */
    LTR559_PS_MEAS_RATE_200 = 0x03,                     /* PS Measurement Repeat Rate = 20ms */
    LTR559_PS_MEAS_RATE_500 = 0x04,                     /* PS Measurement Repeat Rate = 500ms */
    LTR559_PS_MEAS_RATE_1000 = 0x05,                    /* PS Measurement Repeat Rate = 1000ms */
    LTR559_PS_MEAS_RATE_2000 = 0x06,                    /* PS Measurement Repeat Rate = 2000ms */
    LTR559_PS_MEAS_RATE_10 = 0x08,                      /* PS Measurement Repeat Rate = 10ms */
} LTR559_CFG_PS_MEAS_RPT_RATE;

typedef enum {
    LTR559_ALS_MEAS_RATE_50 = 0x00,                     /* ALS Measurement Repeat Rate = 50ms */
    LTR559_ALS_MEAS_RATE_100 = 0x01,                    /* ALS Measurement Repeat Rate = 100ms */
    LTR559_ALS_MEAS_RATE_200 = 0x02,                    /* ALS Measurement Repeat Rate = 200ms */
    LTR559_ALS_MEAS_RATE_500 = 0x03,                    /* ALS Measurement Repeat Rate = 500ms (default) */
    LTR559_ALS_MEAS_RATE_1000 = 0x04,                   /* ALS Measurement Repeat Rate = 1000ms */
    LTR559_ALS_MEAS_RATE_2000 = 0x05,                   /* ALS Measurement Repeat Rate = 2000ms */
} LTR559_CFG_ALS_MEAS_RATE;

typedef enum {
    LTR559_ALS_INTEG_TIME_100 = 0x00,                   /* ALS Integration Time = 100ms (default) */
    LTR559_ALS_INTEG_TIME_50 = 0x01,                    /* ALS Integration Time = 50ms */
    LTR559_ALS_INTEG_TIME_200 = 0x02,                   /* ALS Integration Time = 200ms */
    LTR559_ALS_INTEG_TIME_400 = 0x03,                   /* ALS Integration Time = 400ms */
    LTR559_ALS_INTEG_TIME_150 = 0x04,                   /* ALS Integration Time = 150ms */
    LTR559_ALS_INTEG_TIME_250 = 0x05,                   /* ALS Integration Time = 250ms */
    LTR559_ALS_INTEG_TIME_300 = 0x06,                   /* ALS Integration Time = 300ms */
    LTR559_ALS_INTEG_TIME_350 = 0x07,                   /* ALS Integration Time = 350ms */
} LTR559_CFG_ALS_INTEG_TIME;

typedef enum {
    LTR559_PS_DATA_STATUS_OLD = 0x00,
    LTR559_PS_DATA_STATUS_NEW = 0x01,
} LTR559_CFG_PS_DATA_STATUS;

typedef enum {
    LTR559_PS_INT_STATUS_INACTIVE = 0x00,
    LTR559_PS_INT_STATUS_ACTIVE = 0x01,
} LTR559_CFG_PS_INT_STATUS;

typedef enum {
    LTR559_ALS_DATA_STATUS_OLD = 0x00,
    LTR559_ALS_DATA_STATUS_NEW = 0x01,
} LTR559_CFG_ALS_DATA_STATUS;

typedef enum {
    LTR559_ALS_INT_STATUS_INACTIVE = 0x00,
    LTR559_ALS_INT_STATUS_ACTIVE = 0x01,
} LTR559_CFG_ALS_INT_STATUS;

typedef enum {
    LTR559_ALS_DATA_VALID = 0x00,
    LTR559_ALS_DATA_INVALID = 0x01,
} LTR559_CFG_ALS_DATA_VALID_STATUS;

typedef enum {
    LTR559_INT_MODE_DISABLE = 0x00,
    LTR559_INT_MODE_PS = 0x01,
    LTR559_INT_MODE_ALS = 0x02,
    LTR559_INT_MODE_PS_ALS = 0x03,
} LTR559_CFG_INT_MODE;

typedef enum {
    LTR559_INT_POLARITY_ACTIVE_LO = 0x00,
    LTR559_INT_POLARITY_ACTIVE_HI = 0x01,
} LTR559_CFG_INT_POLARITY;

typedef enum {
    LTR559_FLAG_INIT_ALS = 0,
    LTR559_FLAG_INIT_PS,
} LTR559_FLAG_INIT_BIT;

i2c_dev_t ltr559_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR559_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_als_ps_liteon_ltr559_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR559_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR559_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_als_liteon_ltr559_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR559_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR559_SET_BITSLICE(value, ALS_CONTR_REG_ALS_MODE, LTR559_ALS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR559_SET_BITSLICE(value, ALS_CONTR_REG_ALS_MODE, LTR559_ALS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR559_ALS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr559_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR559_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR559_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR559_PS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR559_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR559_PS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR559_PS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_als_liteon_ltr559_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR559_ALS_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR559_GET_BITSLICE(value, ALS_PS_STATUS_REG_ALS_DATA_STATUS) == LTR559_ALS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

UNUSED static int drv_ps_liteon_ltr559_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR559_ALS_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR559_GET_BITSLICE(value, ALS_PS_STATUS_REG_PS_DATA_STATUS) == LTR559_PS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

UNUSED static int drv_als_liteon_ltr559_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR559_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR559_SET_BITSLICE(value, ALS_CONTR_REG_ALS_GAIN, LTR559_ALS_GAIN_1X);
    ret = sensor_i2c_write(drv, LTR559_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR559_SET_BITSLICE(value, ALS_MEAS_RATE_REG_MEAS_RPT_RATE, LTR559_ALS_MEAS_RATE_500);
    value = LTR559_SET_BITSLICE(value, ALS_MEAS_RATE_REG_INTEG_TIME, LTR559_ALS_INTEG_TIME_100);
    ret = sensor_i2c_write(drv, LTR559_ALS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr559_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR559_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR559_SET_BITSLICE(value, PS_CONTR_REG_PS_SAT_IND_EN, LTR559_PS_SAT_IND_FALSE);
    ret = sensor_i2c_write(drv, LTR559_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR559_SET_BITSLICE(value, PS_LED_REG_LED_CURR, LTR559_LED_PEAK_CURRENT_100);
    value = LTR559_SET_BITSLICE(value, PS_LED_REG_LED_DUTY, LTR559_LED_DUTY_CYCLE_100PCT);
    value = LTR559_SET_BITSLICE(value, PS_LED_REG_LED_PULSE, LTR559_LED_PULSE_60kHZ);
    ret = sensor_i2c_write(drv, LTR559_PS_LED, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR559_SET_BITSLICE(value, PS_N_PULSES_REG_PULSES, 4);
    ret = sensor_i2c_write(drv, LTR559_PS_N_PULSES, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR559_SET_BITSLICE(value, PS_MEAS_RATE_REG_MEAS_RPT_RATE, LTR559_PS_MEAS_RATE_100);
    ret = sensor_i2c_write(drv, LTR559_PS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_als_liteon_ltr559_irq_handle(void)
{
    /* no handle so far */
}

static void drv_ps_liteon_ltr559_irq_handle(void)
{
    /* no handle so far */
}

static int drv_als_liteon_ltr559_open(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr559_set_power_mode(&ltr559_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr559_open(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr559_set_power_mode(&ltr559_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_als_liteon_ltr559_close(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr559_set_power_mode(&ltr559_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr559_close(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr559_set_power_mode(&ltr559_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static uint8_t drv_als_liteon_ltr559_get_gain_val(i2c_dev_t* drv)
{
    uint8_t als_gain = 0, als_gain_val = 0;
    bool    ret = false;

    ret = sensor_i2c_read(drv, LTR559_ALS_PS_STATUS, &als_gain, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (!unlikely(ret))
    {
        als_gain = LTR559_GET_BITSLICE(als_gain, ALS_PS_STATUS_REG_ALS_GAIN);
        switch (als_gain)
        {
            case LTR559_ALS_GAIN_1X:
                als_gain_val = 1;
                break;
            case LTR559_ALS_GAIN_2X:
                als_gain_val = 2;
                break;
            case LTR559_ALS_GAIN_4X:
                als_gain_val = 4;
                break;
            case LTR559_ALS_GAIN_8X:
                als_gain_val = 8;
                break;
            case LTR559_ALS_GAIN_48X:
                als_gain_val = 48;
                break;
            case LTR559_ALS_GAIN_96X:
                als_gain_val = 96;
                break;
            default:
                als_gain_val = 1;
                break;
        }
    }
    else
    {
        als_gain_val = 0;
    }

    return als_gain_val;
}

static uint16_t drv_als_liteon_ltr559_get_integ_time_val(i2c_dev_t* drv)
{
    uint16_t als_integ = 0, als_integ_val = 0;
    bool     ret = false;

    ret = sensor_i2c_read(drv, LTR559_ALS_MEAS_RATE, (uint8_t*)&als_integ, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (!unlikely(ret))
    {
        als_integ = LTR559_GET_BITSLICE(als_integ, ALS_MEAS_RATE_REG_INTEG_TIME);
        switch (als_integ)
        {
            case LTR559_ALS_INTEG_TIME_100:
                als_integ_val = 10;
                break;
            case LTR559_ALS_INTEG_TIME_50:
                als_integ_val = 5;
                break;
            case LTR559_ALS_INTEG_TIME_200:
                als_integ_val = 20;
                break;
            case LTR559_ALS_INTEG_TIME_400:
                als_integ_val = 40;
                break;
            case LTR559_ALS_INTEG_TIME_150:
                als_integ_val = 15;
                break;
            case LTR559_ALS_INTEG_TIME_250:
                als_integ_val = 25;
                break;
            case LTR559_ALS_INTEG_TIME_300:
                als_integ_val = 30;
                break;
            case LTR559_ALS_INTEG_TIME_350:
                als_integ_val = 35;
                break;
            default:
                als_integ_val = 10;
                break;
        }
    }
    else
    {
        als_integ_val = 0;
    }

    return als_integ_val;
}

static int drv_als_liteon_ltr559_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_ch1_data[2] = { 0 };
    uint8_t reg_ch0_data[2] = { 0 };
    uint32_t als_ch0_data = 0, als_ch1_data = 0, chRatio = 0, tmpCalc = 0;
    uint16_t als_gain_val = 0, als_integ_time_val = 0;
    als_data_t * pdata = (als_data_t *) buf;

    if (buf == NULL){
        return -1;
    }

    size = sizeof(als_data_t);
    if (len < size){
        return -1;
    }


    ret = sensor_i2c_read(&ltr559_ctx, LTR559_ALS_DATA_CH1_0, &reg_ch1_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr559_ctx, LTR559_ALS_DATA_CH1_1, &reg_ch1_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr559_ctx, LTR559_ALS_DATA_CH0_0, &reg_ch0_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr559_ctx, LTR559_ALS_DATA_CH0_1, &reg_ch0_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    als_ch0_data = ((uint16_t) reg_ch0_data[1] << 8 | reg_ch0_data[0]);
    als_ch1_data = ((uint16_t) reg_ch1_data[1] << 8 | reg_ch1_data[0]);

    chRatio = (als_ch1_data * 100) / (als_ch0_data + als_ch1_data);
    if (chRatio < 45)
    {
        tmpCalc = (1774 * als_ch0_data + 1106 * als_ch1_data);
    }
    else if (chRatio >= 45 && chRatio < 64)
    {
        tmpCalc = (4279 * als_ch0_data - 1955 * als_ch1_data);
    }
    else if (chRatio >= 64 && chRatio < 85)
    {
        tmpCalc = (593 * als_ch0_data + 119 * als_ch1_data);
    }
    else
    {
        tmpCalc = 0;
    }

    als_gain_val = drv_als_liteon_ltr559_get_gain_val(&ltr559_ctx);
    als_integ_time_val = drv_als_liteon_ltr559_get_integ_time_val(&ltr559_ctx);
    if ((als_gain_val != 0) && (als_integ_time_val != 0))
    {
        pdata->lux = tmpCalc / als_gain_val / als_integ_time_val / 100;
    }
    else
    {
        pdata->lux = 0;
    }
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_ps_liteon_ltr559_read(void *buf, size_t len)
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

    ret = sensor_i2c_read(&ltr559_ctx, LTR559_PS_DATA_0, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr559_ctx, LTR559_PS_DATA_1, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->present = (((uint32_t) (reg_data[1] & 0x07) << 8) | reg_data[0]);
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_als_liteon_ltr559_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_ps_liteon_ltr559_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_als_liteon_ltr559_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_als_liteon_ltr559_set_power_mode(&ltr559_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR559";
            info->unit = lux;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_liteon_ltr559_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_ps_liteon_ltr559_set_power_mode(&ltr559_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR559";
            info->unit = cm;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_als_liteon_ltr559_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_als;

    memset(&sensor_als, 0, sizeof(sensor_als));

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr559_validate_id(&ltr559_ctx, LTR559_PART_ID_VAL, LTR559_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << LTR559_FLAG_INIT_ALS))) {
        /* fill the sensor_ps obj parameters here */
        sensor_als.tag = TAG_DEV_ALS;
        sensor_als.path = dev_als_path;
        sensor_als.io_port = I2C_PORT;
        sensor_als.mode = DEV_POLLING;
        sensor_als.power = DEV_POWER_OFF;
        sensor_als.open = drv_als_liteon_ltr559_open;
        sensor_als.close = drv_als_liteon_ltr559_close;
        sensor_als.read = drv_als_liteon_ltr559_read;
        sensor_als.write = drv_als_liteon_ltr559_write;
        sensor_als.ioctl = drv_als_liteon_ltr559_ioctl;
        sensor_als.irq_handle = drv_als_liteon_ltr559_irq_handle;

        ret = sensor_create_obj(&sensor_als);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr559_set_default_config(&ltr559_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << LTR559_FLAG_INIT_ALS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_ps_liteon_ltr559_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_ps;

    memset(&sensor_ps, 0, sizeof(sensor_ps));

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr559_validate_id(&ltr559_ctx, LTR559_PART_ID_VAL, LTR559_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << LTR559_FLAG_INIT_PS))) {
        /* fill the sensor_ps obj parameters here */
        sensor_ps.tag = TAG_DEV_PS;
        sensor_ps.path = dev_ps_path;
        sensor_ps.io_port = I2C_PORT;
        sensor_ps.mode = DEV_POLLING;
        sensor_ps.power = DEV_POWER_OFF;
        sensor_ps.open = drv_ps_liteon_ltr559_open;
        sensor_ps.close = drv_ps_liteon_ltr559_close;
        sensor_ps.read = drv_ps_liteon_ltr559_read;
        sensor_ps.write = drv_ps_liteon_ltr559_write;
        sensor_ps.ioctl = drv_ps_liteon_ltr559_ioctl;
        sensor_ps.irq_handle = drv_ps_liteon_ltr559_irq_handle;

        ret = sensor_create_obj(&sensor_ps);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr559_set_default_config(&ltr559_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << LTR559_FLAG_INIT_PS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_als_liteon_ltr559_init);
SENSOR_DRV_ADD(drv_ps_liteon_ltr559_init);


