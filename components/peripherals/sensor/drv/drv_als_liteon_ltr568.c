#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR568_I2C_SLAVE_ADDR                           0x23

#define LTR568_ALS_AVE_LIMIT                            0x7E /* ALS digital averaging limit */
#define LTR568_ALS_AVE_FAC                              0x7F /* ALS digital averaging factor */
#define LTR568_ALS_CONTR                                0x80 /* ALS operation mode */
#define LTR568_PS_CONTR                                 0x81 /* PS operation mode, SW reset */
#define LTR568_PS_LED                                   0x82 /* PS LED pulse freq, current duty, peak current */
#define LTR568_PS_N_PULSES                              0x83 /* PS number of pulses */
#define LTR568_PS_MEAS_RATE                             0x84 /* PS measurement rate*/
#define LTR568_ALS_INT_TIME                             0x85 /* ALS integ time, measurement rate*/
#define LTR568_PART_ID                                  0x86
#define LTR568_MANUFAC_ID                               0x87
#define LTR568_ALS_STATUS                               0x88
#define LTR568_IR_DATA_LSB                              0x89 /* ALS/RGB measurement IR data, LSB */
#define LTR568_IR_DATA_MSB                              0x8A /* ALS/RGB measurement IR data, MSB */
#define LTR568_GREEN_DATA_LSB                           0x8B /* ALS/RGB measurement Green data, LSB */
#define LTR568_GREEN_DATA_MSB                           0x8C /* ALS/RGB measurement Green data, MSB */
#define LTR568_PS_STATUS                                0x91
#define LTR568_PS_DATA_LSB                              0x92
#define LTR568_PS_DATA_MSB                              0x93
#define LTR568_PS_SAR                                   0x94
#define LTR568_ALS_SAR                                  0x95
#define LTR568_INTERRUPT                                0x98
#define LTR568_INTR_PRST                                0x99 /* ALS/PS interrupt persist setting */
#define LTR568_PS_THRES_HIGH_LSB                        0x9A /* PS interrupt upper threshold, lower byte */
#define LTR568_PS_THRES_HIGH_MSB                        0x9B /* PS interrupt upper threshold, upper byte */
#define LTR568_PS_THRES_LOW_LSB                         0x9C /* PS interrupt lower threshold, lower byte */
#define LTR568_PS_THRES_LOW_MSB                         0x9D /* PS interrupt lower threshold, upper byte */
#define LTR568_PXTALK_LSB                               0x9E /* Crosstalk correction on PS CH0 PD, lower byte */
#define LTR568_PXTALK_MSB                               0x9F /* Crosstalk correction on PS CH0 PD, upper byte */
#define LTR568_PS_VREHL                                 0xB6 /* External PS VREHL value, affect PS */

#define LTR568_ADDR_TRANS(n)                            ((n) << 1)
#define LTR568_I2C_ADDR                                 LTR568_ADDR_TRANS(LTR568_I2C_SLAVE_ADDR)
#define LTR568_PART_ID_VAL                              0x1C
#define LTR568_MANUFAC_ID_VAL                           0x05

#define LTR568_ALS_AVE_LIMIT_REG_ALS_AVE_LIMIT__POS     (0)
#define LTR568_ALS_AVE_LIMIT_REG_ALS_AVE_LIMIT__MSK     (0x03)
#define LTR568_ALS_AVE_LIMIT_REG_ALS_AVE_LIMIT__REG     (LTR568_ALS_AVE_LIMIT)

#define LTR568_ALS_CONTR_REG_ALS_MODE__POS              (0)
#define LTR568_ALS_CONTR_REG_ALS_MODE__MSK              (0x01)
#define LTR568_ALS_CONTR_REG_ALS_MODE__REG              (LTR568_ALS_CONTR)

#define LTR568_ALS_CONTR_REG_ALS_SAR__POS               (1)
#define LTR568_ALS_CONTR_REG_ALS_SAR__MSK               (0x02)
#define LTR568_ALS_CONTR_REG_ALS_SAR__REG               (LTR568_ALS_CONTR)

#define LTR568_ALS_CONTR_REG_ALS_GAIN__POS              (2)
#define LTR568_ALS_CONTR_REG_ALS_GAIN__MSK              (0x1C)
#define LTR568_ALS_CONTR_REG_ALS_GAIN__REG              (LTR568_ALS_CONTR)

#define LTR568_ALS_CONTR_REG_IR_EN__POS                 (5)
#define LTR568_ALS_CONTR_REG_IR_EN__MSK                 (0x20)
#define LTR568_ALS_CONTR_REG_IR_EN__REG                 (LTR568_ALS_CONTR)

#define LTR568_ALS_CONTR_REG_ALS_RES__POS               (6)
#define LTR568_ALS_CONTR_REG_ALS_RES__MSK               (0xC0)
#define LTR568_ALS_CONTR_REG_ALS_RES__REG               (LTR568_ALS_CONTR)

#define LTR568_PS_CONTR_REG_SW_RESET__POS               (0)
#define LTR568_PS_CONTR_REG_SW_RESET__MSK               (0x01)
#define LTR568_PS_CONTR_REG_SW_RESET__REG               (LTR568_PS_CONTR)

#define LTR568_PS_CONTR_REG_PS_MODE__POS                (1)
#define LTR568_PS_CONTR_REG_PS_MODE__MSK                (0x02)
#define LTR568_PS_CONTR_REG_PS_MODE__REG                (LTR568_PS_CONTR)

#define LTR568_PS_CONTR_REG_FTN_NTF_EN__POS             (2)
#define LTR568_PS_CONTR_REG_FTN_NTF_EN__MSK             (0x04)
#define LTR568_PS_CONTR_REG_FTN_NTF_EN__REG             (LTR568_PS_CONTR)

#define LTR568_PS_CONTR_REG_PS_OS__POS                  (3)
#define LTR568_PS_CONTR_REG_PS_OS__MSK                  (0x08)
#define LTR568_PS_CONTR_REG_PS_OS__REG                  (LTR568_PS_CONTR)

#define LTR568_PS_LED_REG_LED_CURR__POS                 (0)
#define LTR568_PS_LED_REG_LED_CURR__MSK                 (0x07)
#define LTR568_PS_LED_REG_LED_CURR__REG                 (LTR568_PS_LED)

#define LTR568_PS_LED_REG_PULSE_WIDTH__POS              (3)
#define LTR568_PS_LED_REG_PULSE_WIDTH__MSK              (0x18)
#define LTR568_PS_LED_REG_PULSE_WIDTH__REG              (LTR568_PS_LED)

#define LTR568_PS_LED_REG_PULSE_DUTY__POS               (5)
#define LTR568_PS_LED_REG_PULSE_DUTY__MSK               (0x60)
#define LTR568_PS_LED_REG_PULSE_DUTY__REG               (LTR568_PS_LED)

#define LTR568_PS_N_PULSES_REG_PULSE_COUNT__POS         (0)
#define LTR568_PS_N_PULSES_REG_PULSE_COUNT__MSK         (0x3F)
#define LTR568_PS_N_PULSES_REG_PULSE_COUNT__REG         (LTR568_PS_N_PULSES)

#define LTR568_PS_N_PULSES_REG_PS_AVE_FAC__POS          (6)
#define LTR568_PS_N_PULSES_REG_PS_AVE_FAC__MSK          (0xC0)
#define LTR568_PS_N_PULSES_REG_PS_AVE_FAC__REG          (LTR568_PS_N_PULSES)

#define LTR568_PS_MEAS_RATE_REG_MEAS_RATE__POS          (0)
#define LTR568_PS_MEAS_RATE_REG_MEAS_RATE__MSK          (0x07)
#define LTR568_PS_MEAS_RATE_REG_MEAS_RATE__REG          (LTR568_PS_MEAS_RATE)

#define LTR568_ALS_INT_TIME_REG_MEAS_RATE__POS          (0)
#define LTR568_ALS_INT_TIME_REG_MEAS_RATE__MSK          (0x03)
#define LTR568_ALS_INT_TIME_REG_MEAS_RATE__REG          (LTR568_ALS_INT_TIME)

#define LTR568_ALS_INT_TIME_REG_INTEG_TIME__POS         (2)
#define LTR568_ALS_INT_TIME_REG_INTEG_TIME__MSK         (0x0C)
#define LTR568_ALS_INT_TIME_REG_INTEG_TIME__REG         (LTR568_ALS_INT_TIME)

#define LTR568_ALS_STATUS_REG_ALS_DATA_STATUS__POS      (0)
#define LTR568_ALS_STATUS_REG_ALS_DATA_STATUS__MSK      (0x01)
#define LTR568_ALS_STATUS_REG_ALS_DATA_STATUS__REG      (LTR568_ALS_STATUS)

#define LTR568_ALS_STATUS_REG_ALS_INT_STATUS__POS       (1)
#define LTR568_ALS_STATUS_REG_ALS_INT_STATUS__MSK       (0x02)
#define LTR568_ALS_STATUS_REG_ALS_INT_STATUS__REG       (LTR568_ALS_STATUS)

#define LTR568_ALS_STATUS_REG_ALS_GAIN__POS             (3)
#define LTR568_ALS_STATUS_REG_ALS_GAIN__MSK             (0x78)
#define LTR568_ALS_STATUS_REG_ALS_GAIN__REG             (LTR568_ALS_STATUS)

#define LTR568_ALS_STATUS_REG_DATA_VALIDITY__POS        (7)
#define LTR568_ALS_STATUS_REG_DATA_VALIDITY__MSK        (0x80)
#define LTR568_ALS_STATUS_REG_DATA_VALIDITY__REG        (LTR568_ALS_STATUS)

#define LTR568_PS_STATUS_REG_PS_DATA_STATUS__POS        (0)
#define LTR568_PS_STATUS_REG_PS_DATA_STATUS__MSK        (0x01)
#define LTR568_PS_STATUS_REG_PS_DATA_STATUS__REG        (LTR568_PS_STATUS)

#define LTR568_PS_STATUS_REG_PS_INT_STATUS__POS         (1)
#define LTR568_PS_STATUS_REG_PS_INT_STATUS__MSK         (0x02)
#define LTR568_PS_STATUS_REG_PS_INT_STATUS__REG         (LTR568_PS_STATUS)

#define LTR568_PS_STATUS_REG_AMBIENT_SAT__POS           (2)
#define LTR568_PS_STATUS_REG_AMBIENT_SAT__MSK           (0x04)
#define LTR568_PS_STATUS_REG_AMBIENT_SAT__REG           (LTR568_PS_STATUS)

#define LTR568_PS_STATUS_REG_NTF__POS                   (4)
#define LTR568_PS_STATUS_REG_NTF__MSK                   (0x10)
#define LTR568_PS_STATUS_REG_NTF__REG                   (LTR568_PS_STATUS)

#define LTR568_PS_STATUS_REG_FTN__POS                   (5)
#define LTR568_PS_STATUS_REG_FTN__MSK                   (0x20)
#define LTR568_PS_STATUS_REG_FTN__REG                   (LTR568_PS_STATUS)

#define LTR568_INTERRUPT_REG_INT_MODE__POS              (0)
#define LTR568_INTERRUPT_REG_INT_MODE__MSK              (0x03)
#define LTR568_INTERRUPT_REG_INT_MODE__REG              (LTR568_INTERRUPT)

#define LTR568_INTERRUPT_REG_INT_POLARITY__POS          (2)
#define LTR568_INTERRUPT_REG_INT_POLARITY__MSK          (0x04)
#define LTR568_INTERRUPT_REG_INT_POLARITY__REG          (LTR568_INTERRUPT)

#define LTR568_INTR_PRST_REG_ALS_PERSIST__POS           (0)
#define LTR568_INTR_PRST_REG_ALS_PERSIST__MSK           (0x0F)
#define LTR568_INTR_PRST_REG_ALS_PERSIST__REG           (LTR568_INTR_PRST)

#define LTR568_INTR_PRST_REG_PS_PERSIST__POS            (4)
#define LTR568_INTR_PRST_REG_PS_PERSIST__MSK            (0xF0)
#define LTR568_INTR_PRST_REG_PS_PERSIST__REG            (LTR568_INTR_PRST)

#define LTR568_GET_BITSLICE(regvar, bitname)            ((regvar & LTR568_##bitname##__MSK) >> LTR568_##bitname##__POS)
#define LTR568_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR568_##bitname##__MSK) | ((val<<LTR568_##bitname##__POS)&LTR568_##bitname##__MSK))

#define LTR568_WAIT_TIME_PER_CHECK                      (10)
#define LTR568_WAIT_TIME_TOTAL                          (100)

typedef enum {
    LTR568_ALS_AVE_LIMIT_511 = 0x00,
    LTR568_ALS_AVE_LIMIT_255 = 0x01,
    LTR568_ALS_AVE_LIMIT_127 = 0x02,
} LTR568_CFG_ALS_AVE_LIMIT;

typedef enum {
    LTR568_ALS_STANDBY = 0x00,
    LTR568_ALS_ACTIVE = 0x01, 
} LTR568_CFG_ALS_MODE;

typedef enum {
    LTR568_ALS_SAR_ENABLE = 0x00,
    LTR568_ALS_SAR_DISABLE = 0x01, 
} LTR568_CFG_ALS_SAR_ENB;

typedef enum {
    LTR568_ALS_GAIN_1X = 0x00,
    LTR568_ALS_GAIN_4X = 0x01,
    LTR568_ALS_GAIN_16X = 0x02,
    LTR568_ALS_GAIN_64X = 0x03,
    LTR568_ALS_GAIN_128X = 0x04,
    LTR568_ALS_GAIN_512X = 0x05,
} LTR568_CFG_ALS_Gain;

typedef enum {
    LTR568_IR_DISABLE = 0x00,
    LTR568_IR_ENABLE = 0x01, 
} LTR568_CFG_IR_EN;

typedef enum {
    LTR568_ALS_RES_16BIT = 0x00,
    LTR568_ALS_RES_15BIT = 0x01,
    LTR568_ALS_RES_14BIT = 0x02,
    LTR568_ALS_RES_13BIT = 0x03,
} LTR568_CFG_ALS_RESOLUTION;

typedef enum {
    LTR568_SW_RESET_FALSE = 0x00,
    LTR568_SW_RESET_TRUE = 0x01, 
} LTR568_CFG_SW_RESET;

typedef enum {
    LTR568_PS_STANDBY = 0x00,
    LTR568_PS_ACTIVE = 0x01, 
} LTR568_CFG_PS_MODE;

typedef enum {
    LTR568_FTN_NTF_DISABLE = 0x00,
    LTR568_FTN_NTF_ENABLE = 0x01,
} LTR568_CFG_FTN_NTF_EN;

typedef enum {
    LTR568_PS_OS_DISABLE = 0x00,
    LTR568_PS_OS_ENABLE = 0x01,
} LTR568_CFG_PS_OS;

typedef enum {
    LTR568_PS_LED_CURRENT_0mA = 0x00,
    LTR568_PS_LED_CURRENT_50mA = 0x01,
    LTR568_PS_LED_CURRENT_100mA = 0x02,
    LTR568_PS_LED_CURRENT_120mA = 0x03,
    LTR568_PS_LED_CURRENT_140mA = 0x04,
    LTR568_PS_LED_CURRENT_170mA = 0x05,
    LTR568_PS_LED_CURRENT_190mA = 0x06,
    LTR568_PS_LED_CURRENT_240mA = 0x07,
} LTR568_CFG_PS_LED_CURRENT;

typedef enum {
    LTR568_PS_PULSE_WIDTH_4us = 0x00,
    LTR568_PS_PULSE_WIDTH_8us = 0x01,
    LTR568_PS_PULSE_WIDTH_16us = 0x02,
    LTR568_PS_PULSE_WIDTH_32us = 0x03,
} LTR568_CFG_PS_LED_PULSE_WIDTH;

typedef enum {
    LTR568_PS_PULSE_DUTY_12_5PCT = 0x00,
    LTR568_PS_PULSE_DUTY_25PCT = 0x01,
    LTR568_PS_PULSE_DUTY_50PCT = 0x02,
    LTR568_PS_PULSE_DUTY_100PCT = 0x03,
} LTR568_CFG_PS_LED_PULSE_DUTY;

typedef enum {
    LTR568_PS_AVE_FAC_0n = 0x00,
    LTR568_PS_AVE_FAC_2n = 0x01,
    LTR568_PS_AVE_FAC_4n = 0x02,
    LTR568_PS_AVE_FAC_5n = 0x03,
} LTR568_CFG_PS_AVE_FAC;

typedef enum {
    LTR568_PS_MEAS_RATE_6_125 = 0x00,                   /* PS Measurement Repeat Rate = 6.125ms */
    LTR568_PS_MEAS_RATE_12_5 = 0x01,                    /* PS Measurement Repeat Rate = 12.5ms */
    LTR568_PS_MEAS_RATE_25 = 0x02,                      /* PS Measurement Repeat Rate = 25ms */
    LTR568_PS_MEAS_RATE_50 = 0x03,                      /* PS Measurement Repeat Rate = 50ms */
    LTR568_PS_MEAS_RATE_100 = 0x04,                     /* PS Measurement Repeat Rate = 100ms (default) */
    LTR568_PS_MEAS_RATE_200 = 0x05,                     /* PS Measurement Repeat Rate = 200ms */
    LTR568_PS_MEAS_RATE_400 = 0x06,                     /* PS Measurement Repeat Rate = 400ms */
    LTR568_PS_MEAS_RATE_800 = 0x07,                     /* PS Measurement Repeat Rate = 800ms */
} LTR568_CFG_PS_MEAS_RATE;

typedef enum {
    LTR568_ALS_MEAS_RATE_100 = 0x00,                    /* ALS Measurement Repeat Rate = 100ms */
    LTR568_ALS_MEAS_RATE_200 = 0x01,                    /* ALS Measurement Repeat Rate = 200ms */
    LTR568_ALS_MEAS_RATE_400 = 0x02,                    /* ALS Measurement Repeat Rate = 400ms (default) */
    LTR568_ALS_MEAS_RATE_800 = 0x03,                    /* ALS Measurement Repeat Rate = 800ms */
} LTR568_CFG_ALS_MEAS_RATE;


typedef enum {
    LTR568_ALS_INT_TIME_50 = 0x00,                      /* ALS Measurement Repeat Rate = 50ms */
    LTR568_ALS_INT_TIME_100 = 0x01,                     /* ALS Measurement Repeat Rate = 100ms (default) */
    LTR568_ALS_INT_TIME_200 = 0x02,                     /* ALS Measurement Repeat Rate = 200ms */
    LTR568_ALS_INT_TIME_400 = 0x03,                     /* ALS Measurement Repeat Rate = 400ms */
} LTR568_CFG_ALS_INT_TIME;

typedef enum {
    LTR568_ALS_DATA_STATUS_OLD = 0x00,
    LTR568_ALS_DATA_STATUS_NEW = 0x01,
} LTR568_CFG_ALS_DATA_STATUS;

typedef enum {
    LTR568_ALS_INT_FALSE = 0x00,
    LTR568_ALS_INT_TRUE = 0x01,
} LTR568_CFG_ALS_INT_STATUS;

typedef enum {
    LTR568_ALS_DATA_VALID = 0x00,
    LTR568_ALS_DATA_INVALID = 0x01,
} LTR568_CFG_ALS_DATA_VALIDITY;

typedef enum {
    LTR568_PS_DATA_STATUS_OLD = 0x00,
    LTR568_PS_DATA_STATUS_NEW = 0x01,
} LTR568_CFG_PS_DATA_STATUS;

typedef enum {
    LTR568_PS_INT_FALSE = 0x00,
    LTR568_PS_INT_TRUE = 0x01,
} LTR568_CFG_PS_INT_STATUS;

typedef enum {
    LTR568_PS_AMBIENT_SAT_FALSE = 0x00,
    LTR568_PS_AMBIENT_SAT_TRUE = 0x01,
} LTR568_CFG_PS_AMBIENT_SAT_STATUS;

typedef enum {
    LTR568_PS_NTF_FALSE = 0x00,
    LTR568_PS_NTF_TRUE = 0x01,
} LTR568_CFG_PS_NTF_STATUS;

typedef enum {
    LTR568_PS_FTN_FALSE = 0x00,
    LTR568_PS_FTN_TRUE = 0x01,
} LTR568_CFG_PS_FTN_STATUS;

typedef enum {
    LTR568_INT_INACTIVE = 0x00,
    LTR568_INT_PS = 0x01,
    LTR568_INT_ALS = 0x02,
    LTR568_INT_ALS_PS = 0x02,
} LTR568_CFG_INT_MODE;

typedef enum {
    LTR568_INT_ACTIVE_LO = 0x00,
    LTR568_INT_ACTIVE_HI = 0x01,
} LTR568_CFG_INT_POLARITY;

typedef enum {
    LTR568_FLAG_INIT_ALS = 0,
    LTR568_FLAG_INIT_PS,
    LTR568_FLAG_INIT_RGB,
} LTR568_FLAG_INIT_BIT;

i2c_dev_t ltr568_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR568_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_als_ps_liteon_ltr568_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR568_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR568_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_als_liteon_ltr568_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR568_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR568_SET_BITSLICE(value, ALS_CONTR_REG_ALS_MODE, LTR568_ALS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR568_SET_BITSLICE(value, ALS_CONTR_REG_ALS_MODE, LTR568_ALS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR568_ALS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr568_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR568_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR568_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR568_PS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR568_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR568_PS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR568_PS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_als_liteon_ltr568_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR568_ALS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR568_GET_BITSLICE(value, ALS_STATUS_REG_ALS_DATA_STATUS) == LTR568_ALS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

UNUSED static int drv_ps_liteon_ltr568_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR568_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR568_GET_BITSLICE(value, PS_STATUS_REG_PS_DATA_STATUS) == LTR568_PS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_als_liteon_ltr568_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    value = LTR568_SET_BITSLICE(value, ALS_AVE_LIMIT_REG_ALS_AVE_LIMIT, LTR568_ALS_AVE_LIMIT_255);
    ret = sensor_i2c_write(drv, LTR568_ALS_AVE_LIMIT, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    ret = sensor_i2c_write(drv, LTR568_ALS_AVE_FAC, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR568_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR568_SET_BITSLICE(value, ALS_CONTR_REG_ALS_SAR, LTR568_ALS_SAR_DISABLE);
    value = LTR568_SET_BITSLICE(value, ALS_CONTR_REG_ALS_GAIN, LTR568_ALS_GAIN_1X);
    value = LTR568_SET_BITSLICE(value, ALS_CONTR_REG_IR_EN, LTR568_IR_ENABLE);
    value = LTR568_SET_BITSLICE(value, ALS_CONTR_REG_ALS_RES, LTR568_ALS_RES_16BIT);
    ret = sensor_i2c_write(drv, LTR568_ALS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR568_SET_BITSLICE(value, ALS_INT_TIME_REG_MEAS_RATE, LTR568_ALS_MEAS_RATE_400);
    value = LTR568_SET_BITSLICE(value, ALS_INT_TIME_REG_INTEG_TIME, LTR568_ALS_INT_TIME_100);
    ret = sensor_i2c_write(drv, LTR568_ALS_INT_TIME, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_ps_liteon_ltr568_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR568_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR568_SET_BITSLICE(value, PS_CONTR_REG_FTN_NTF_EN, LTR568_FTN_NTF_DISABLE);
    value = LTR568_SET_BITSLICE(value, PS_CONTR_REG_PS_OS, LTR568_PS_OS_DISABLE);
    ret = sensor_i2c_write(drv, LTR568_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR568_SET_BITSLICE(value, PS_LED_REG_LED_CURR, LTR568_PS_LED_CURRENT_100mA);
    value = LTR568_SET_BITSLICE(value, PS_LED_REG_PULSE_WIDTH, LTR568_PS_PULSE_WIDTH_32us);
    value = LTR568_SET_BITSLICE(value, PS_LED_REG_PULSE_DUTY, LTR568_PS_PULSE_DUTY_100PCT);
    ret = sensor_i2c_write(drv, LTR568_PS_LED, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR568_SET_BITSLICE(value, PS_N_PULSES_REG_PULSE_COUNT, 0);
    value = LTR568_SET_BITSLICE(value, PS_N_PULSES_REG_PS_AVE_FAC, LTR568_PS_AVE_FAC_0n);
    ret = sensor_i2c_write(drv, LTR568_PS_N_PULSES, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR568_SET_BITSLICE(value, PS_MEAS_RATE_REG_MEAS_RATE, LTR568_PS_MEAS_RATE_100);
    ret = sensor_i2c_write(drv, LTR568_PS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_als_liteon_ltr568_irq_handle(void)
{
    /* no handle so far */
}

static int drv_als_liteon_ltr568_open(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr568_set_power_mode(&ltr568_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_liteon_ltr568_close(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr568_set_power_mode(&ltr568_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static uint16_t drv_als_liteon_ltr568_get_gain_val(i2c_dev_t* drv)
{
    uint16_t als_gain = 0, als_gain_val = 0;
    bool    ret = false;

    ret = sensor_i2c_read(drv, LTR568_ALS_STATUS, (uint8_t*)&als_gain, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (!unlikely(ret))
    {
        als_gain = LTR568_GET_BITSLICE(als_gain, ALS_STATUS_REG_ALS_GAIN);
        switch (als_gain)
        {
            case LTR568_ALS_GAIN_1X:
                als_gain_val = 1;
                break;
            case LTR568_ALS_GAIN_4X:
                als_gain_val = 4;
                break;
            case LTR568_ALS_GAIN_16X:
                als_gain_val = 16;
                break;
            case LTR568_ALS_GAIN_64X:
                als_gain_val = 64;
                break;
            case LTR568_ALS_GAIN_128X:
                als_gain_val = 128;
                break;
            case LTR568_ALS_GAIN_512X:
                als_gain_val = 512;
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

static uint16_t drv_als_liteon_ltr568_get_integ_time_val(i2c_dev_t* drv)
{
    uint16_t als_integ = 0, als_integ_val = 0;
    bool     ret = false;

    ret = sensor_i2c_read(drv, LTR568_ALS_INT_TIME, (uint8_t*)&als_integ, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (!unlikely(ret))
    {
        als_integ = LTR568_GET_BITSLICE(als_integ, ALS_INT_TIME_REG_INTEG_TIME);
        switch (als_integ)
        {
            case LTR568_ALS_INT_TIME_50:
                als_integ_val = 5;
                break;
            case LTR568_ALS_INT_TIME_100:
                als_integ_val = 10;
                break;
            case LTR568_ALS_INT_TIME_200:
                als_integ_val = 20;
                break;
            case LTR568_ALS_INT_TIME_400:
                als_integ_val = 40;
                break;
            default:
                als_integ_val = 5;
                break;
        }
    }
    else
    {
        als_integ_val = 0;
    }

    return als_integ_val;
}

static int drv_als_liteon_ltr568_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t ir_data_reg[2] = { 0 };
    uint8_t als_data_reg[2] = { 0 };
    uint8_t als_sar = 0;
    uint16_t als_gain_val = 0, als_integ_time_val = 0;
    uint32_t als_data = 0;
    als_data_t * pdata = (als_data_t *) buf;

    if (buf == NULL){
        return -1;
    }

    size = sizeof(als_data_t);
    if (len < size){
        return -1;
    }

    ret = sensor_i2c_read(&ltr568_ctx, LTR568_IR_DATA_LSB, &ir_data_reg[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr568_ctx, LTR568_IR_DATA_MSB, &ir_data_reg[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr568_ctx, LTR568_GREEN_DATA_LSB, &als_data_reg[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr568_ctx, LTR568_GREEN_DATA_MSB, &als_data_reg[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr568_ctx, LTR568_ALS_SAR, &als_sar, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    als_data = ((uint32_t) als_data_reg[1] << 8 | als_data_reg[0]);
    als_gain_val = drv_als_liteon_ltr568_get_gain_val(&ltr568_ctx);
    als_integ_time_val = drv_als_liteon_ltr568_get_integ_time_val(&ltr568_ctx);
    if ((als_gain_val != 0) && (als_integ_time_val != 0))
    {
        pdata->lux = (als_data * 25) / als_gain_val / als_integ_time_val / 10;
    }
    else
    {
        pdata->lux = 0;
    }
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_als_liteon_ltr568_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_als_liteon_ltr568_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_als_liteon_ltr568_set_power_mode(&ltr568_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR568";
            info->unit = lux;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_ps_liteon_ltr568_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ps_liteon_ltr568_open(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr568_set_power_mode(&ltr568_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr568_close(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr568_set_power_mode(&ltr568_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_liteon_ltr568_read(void *buf, size_t len)
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

    ret = sensor_i2c_read(&ltr568_ctx, LTR568_PS_DATA_LSB, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr568_ctx, LTR568_PS_DATA_MSB, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->present = ((uint32_t) reg_data[1] << 8 | reg_data[0]);
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_ps_liteon_ltr568_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_ps_liteon_ltr568_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_ps_liteon_ltr568_set_power_mode(&ltr568_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR568";
            info->unit = cm;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_als_liteon_ltr568_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_als;
    memset(&sensor_als, 0, sizeof(sensor_als));

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr568_validate_id(&ltr568_ctx, LTR568_PART_ID_VAL, LTR568_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << LTR568_FLAG_INIT_ALS))) {
        /* fill the sensor_als obj parameters here */
        sensor_als.path = dev_als_path;
        sensor_als.tag = TAG_DEV_ALS;
        sensor_als.io_port = I2C_PORT;
        sensor_als.mode = DEV_POLLING;
        sensor_als.power = DEV_POWER_OFF;
        sensor_als.open = drv_als_liteon_ltr568_open;
        sensor_als.close = drv_als_liteon_ltr568_close;
        sensor_als.read = drv_als_liteon_ltr568_read;
        sensor_als.write = drv_als_liteon_ltr568_write;
        sensor_als.ioctl = drv_als_liteon_ltr568_ioctl;
        sensor_als.irq_handle = drv_als_liteon_ltr568_irq_handle;

        ret = sensor_create_obj(&sensor_als);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_als_liteon_ltr568_set_default_config(&ltr568_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << LTR568_FLAG_INIT_ALS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_ps_liteon_ltr568_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_ps;
    memset(&sensor_ps, 0, sizeof(sensor_ps));

    if (!g_init_bitwise) {
        ret = drv_als_ps_liteon_ltr568_validate_id(&ltr568_ctx, LTR568_PART_ID_VAL, LTR568_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << LTR568_FLAG_INIT_PS))) {
        /* fill the sensor_ps obj parameters here */
        sensor_ps.tag = TAG_DEV_PS;
        sensor_ps.path = dev_ps_path;
        sensor_ps.io_port = I2C_PORT;
        sensor_ps.mode = DEV_POLLING;
        sensor_ps.power = DEV_POWER_OFF;
        sensor_ps.open = drv_ps_liteon_ltr568_open;
        sensor_ps.close = drv_ps_liteon_ltr568_close;
        sensor_ps.read = drv_ps_liteon_ltr568_read;
        sensor_ps.write = drv_ps_liteon_ltr568_write;
        sensor_ps.ioctl = drv_ps_liteon_ltr568_ioctl;
        sensor_ps.irq_handle = drv_ps_liteon_ltr568_irq_handle;

        ret = sensor_create_obj(&sensor_ps);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr568_set_default_config(&ltr568_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << LTR568_FLAG_INIT_PS;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_als_liteon_ltr568_init);
SENSOR_DRV_ADD(drv_ps_liteon_ltr568_init);

