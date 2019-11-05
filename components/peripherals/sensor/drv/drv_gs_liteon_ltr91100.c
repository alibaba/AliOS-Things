#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR91100_I2C_SLAVE_ADDR                           0x23

#define LTR91100_CTRL                                     0x80 /* SW reset control */
#define LTR91100_PS_CTRL                                  0x81 /* PS operation mode */
#define LTR91100_GS_CTRL                                  0x82 /* GS operation mode */
#define LTR91100_PS_LED                                   0x88 /* LED pulse freq, current duty, peak current */
#define LTR91100_PS_MEAS_RATE                             0x89 /* measurement rate*/
#define LTR91100_PS_XTALK_NE_LSB                          0x8B /* Crosstalk correction on PS north-east detector, lower byte */
#define LTR91100_PS_XTALK_NE_MSB                          0xCB /* Crosstalk correction on PS north-east detector, upper byte */
#define LTR91100_PS_XTALK_SW_LSB                          0x8C /* Crosstalk correction on PS south-west detector, lower byte */
#define LTR91100_PS_XTALK_SW_MSB                          0xCC /* Crosstalk correction on PS south-west detector, upper byte */
#define LTR91100_PS_THRES_UP_LSB                          0x8D /* PS interrupt upper threshold, lower byte */
#define LTR91100_PS_THRES_UP_MSB                          0xCD /* PS interrupt upper threshold, upper byte */
#define LTR91100_PS_THRES_LOW_LSB                         0x8E /* PS interrupt lower threshold, lower byte */
#define LTR91100_PS_THRES_LOW_MSB                         0xCE /* PS interrupt lower threshold, upper byte */
#define LTR91100_INTR_PRST                                0x8F /* PS interrupt persist setting */
#define LTR91100_GS_LED                                   0x90 /* GS LED setting */
#define LTR91100_GS_WAIT                                  0x91 /* GS wait time setting */
#define LTR91100_GS_PRST                                  0x92 /* GS interrupt persist setting */
#define LTR91100_GS_ENTRY_LSB                             0x93 /* GS entry threshold value (1st entry), lower byte */
#define LTR91100_GS_ENTRY_MSB                             0xC3 /* GS entry threshold value (1st entry), upper byte */
#define LTR91100_GS_EXIT                                  0x94 /* GS exit threshold value */
#define LTR91100_GS_GATE                                  0x95 /* GS gate after the 1st entry */
#define LTR91100_GS_XTALK_N                               0x96 /* GS crosstalk correction on north detector */
#define LTR91100_GS_XTALK_S                               0x97 /* GS crosstalk correction on south detector */
#define LTR91100_GS_XTALK_E                               0x98 /* GS crosstalk correction on east detector */
#define LTR91100_GS_XTALK_W                               0x99 /* GS crosstalk correction on west detector */
#define LTR91100_PART_ID                                  0x9A
#define LTR91100_MANUFAC_ID                               0x9B
#define LTR91100_PS_STATUS                                0x9D
#define LTR91100_GS_STATUS                                0x9E
#define LTR91100_PS_DATA_LSB                              0xB0
#define LTR91100_PS_DATA_MSB                              0xB1
#define LTR91100_GS_FIFO_ADDR                             0xB2 /* GS FIFO address pointer */
#define LTR91100_GS_FIFO_ACCESS_N                         0xB3 /* GS north data */
#define LTR91100_GS_FIFO_ACCESS_S                         0xB4 /* GS south data */
#define LTR91100_GS_FIFO_ACCESS_E                         0xB5 /* GS east data */
#define LTR91100_GS_FIFO_ACCESS_W                         0xB6 /* GS west data */

#define LTR91100_ADDR_TRANS(n)                            ((n) << 1)
#define LTR91100_I2C_ADDR                                 LTR91100_ADDR_TRANS(LTR91100_I2C_SLAVE_ADDR)
#define LTR91100_PART_ID_VAL                              0xC1
#define LTR91100_MANUFAC_ID_VAL                           0x05

#define LTR91100_CTRL_REG_SW_RESET__POS                   (1)
#define LTR91100_CTRL_REG_SW_RESET__MSK                   (0x02)
#define LTR91100_CTRL_REG_SW_RESET__REG                   (LTR91100_CTRL)

#define LTR91100_PS_CTRL_REG_PS_INT__POS                  (0)
#define LTR91100_PS_CTRL_REG_PS_INT__MSK                  (0x01)
#define LTR91100_PS_CTRL_REG_PS_INT__REG                  (LTR91100_PS_CTRL)

#define LTR91100_PS_CTRL_REG_PS_MODE__POS                 (1)
#define LTR91100_PS_CTRL_REG_PS_MODE__MSK                 (0x02)
#define LTR91100_PS_CTRL_REG_PS_MODE__REG                 (LTR91100_PS_CTRL)

#define LTR91100_PS_CTRL_REG_PS_GAIN__POS                 (2)
#define LTR91100_PS_CTRL_REG_PS_GAIN__MSK                 (0x0C)
#define LTR91100_PS_CTRL_REG_PS_GAIN__REG                 (LTR91100_PS_CTRL)

#define LTR91100_PS_CTRL_REG_PS_OFFSET_EN__POS            (4)
#define LTR91100_PS_CTRL_REG_PS_OFFSET_EN__MSK            (0x10)
#define LTR91100_PS_CTRL_REG_PS_OFFSET_EN__REG            (LTR91100_PS_CTRL)

#define LTR91100_PS_CTRL_REG_NEAR_FAR_STATUS_EN__POS      (7)
#define LTR91100_PS_CTRL_REG_NEAR_FAR_STATUS_EN__MSK      (0x80)
#define LTR91100_PS_CTRL_REG_NEAR_FAR_STATUS_EN__REG      (LTR91100_PS_CTRL)

#define LTR91100_GS_CTRL_REG_GS_INT__POS                  (0)
#define LTR91100_GS_CTRL_REG_GS_INT__MSK                  (0x01)
#define LTR91100_GS_CTRL_REG_GS_INT__REG                  (LTR91100_GS_CTRL)

#define LTR91100_GS_CTRL_REG_GS_MODE__POS                 (1)
#define LTR91100_GS_CTRL_REG_GS_MODE__MSK                 (0x02)
#define LTR91100_GS_CTRL_REG_GS_MODE__REG                 (LTR91100_GS_CTRL)

#define LTR91100_GS_CTRL_REG_GS_GAIN__POS                 (2)
#define LTR91100_GS_CTRL_REG_GS_GAIN__MSK                 (0x0C)
#define LTR91100_GS_CTRL_REG_GS_GAIN__REG                 (LTR91100_GS_CTRL)

#define LTR91100_GS_CTRL_REG_GS_OFFSET_EN__POS            (4)
#define LTR91100_GS_CTRL_REG_GS_OFFSET_EN__MSK            (0x10)
#define LTR91100_GS_CTRL_REG_GS_OFFSET_EN__REG            (LTR91100_GS_CTRL)

#define LTR91100_GS_CTRL_REG_GS_FIFO_RESET__POS           (5)
#define LTR91100_GS_CTRL_REG_GS_FIFO_RESET__MSK           (0x20)
#define LTR91100_GS_CTRL_REG_GS_FIFO_RESET__REG           (LTR91100_GS_CTRL)

#define LTR91100_GS_CTRL_REG_GS_FORCE_START__POS          (6)
#define LTR91100_GS_CTRL_REG_GS_FORCE_START__MSK          (0x40)
#define LTR91100_GS_CTRL_REG_GS_FORCE_START__REG          (LTR91100_GS_CTRL)

#define LTR91100_PS_LED_REG_PLED_DRIVE__POS               (0)
#define LTR91100_PS_LED_REG_PLED_DRIVE__MSK               (0x03)
#define LTR91100_PS_LED_REG_PLED_DRIVE__REG               (LTR91100_PS_LED)

#define LTR91100_PS_LED_REG_PLED_BOOST__POS               (2)
#define LTR91100_PS_LED_REG_PLED_BOOST__MSK               (0x0C)
#define LTR91100_PS_LED_REG_PLED_BOOST__REG               (LTR91100_PS_LED)

#define LTR91100_PS_MEAS_RATE_REG_PULSES__POS             (0)
#define LTR91100_PS_MEAS_RATE_REG_PULSES__MSK             (0x0F)
#define LTR91100_PS_MEAS_RATE_REG_PULSES__REG             (LTR91100_PS_MEAS_RATE)

#define LTR91100_PS_MEAS_RATE_REG_MEAS_RATE__POS          (5)
#define LTR91100_PS_MEAS_RATE_REG_MEAS_RATE__MSK          (0x0E)
#define LTR91100_PS_MEAS_RATE_REG_MEAS_RATE__REG          (LTR91100_PS_MEAS_RATE)

#define LTR91100_INTR_PRST_REG_PS_PERSIST__POS            (4)
#define LTR91100_INTR_PRST_REG_PS_PERSIST__MSK            (0xF0)
#define LTR91100_INTR_PRST_REG_PS_PERSIST__REG            (LTR91100_INTR_PRST)

#define LTR91100_GS_LED_REG_GLED_DRIVE__POS               (0)
#define LTR91100_GS_LED_REG_GLED_DRIVE__MSK               (0x03)
#define LTR91100_GS_LED_REG_GLED_DRIVE__REG               (LTR91100_GS_LED)

#define LTR91100_GS_LED_REG_GLED_BOOST__POS               (2)
#define LTR91100_GS_LED_REG_GLED_BOOST__MSK               (0x0C)
#define LTR91100_GS_LED_REG_GLED_BOOST__REG               (LTR91100_GS_LED)

#define LTR91100_GS_WAIT_REG_PULSES__POS                  (0)
#define LTR91100_GS_WAIT_REG_PULSES__MSK                  (0x0F)
#define LTR91100_GS_WAIT_REG_PULSES__REG                  (LTR91100_GS_WAIT)

#define LTR91100_GS_WAIT_REG_WAIT_TIME__POS               (4)
#define LTR91100_GS_WAIT_REG_WAIT_TIME__MSK               (0x70)
#define LTR91100_GS_WAIT_REG_WAIT_TIME__REG               (LTR91100_GS_WAIT)

#define LTR91100_GS_PRST_REG_GS_PERSIST__POS              (2)
#define LTR91100_GS_PRST_REG_GS_PERSIST__MSK              (0x0C)
#define LTR91100_GS_PRST_REG_GS_PERSIST__REG              (LTR91100_GS_PRST)

#define LTR91100_PS_STATUS_REG_PS_DATA_STATUS__POS        (0)
#define LTR91100_PS_STATUS_REG_PS_DATA_STATUS__MSK        (0x01)
#define LTR91100_PS_STATUS_REG_PS_DATA_STATUS__REG        (LTR91100_PS_STATUS)

#define LTR91100_PS_STATUS_REG_PS_INT_STATUS__POS         (1)
#define LTR91100_PS_STATUS_REG_PS_INT_STATUS__MSK         (0x02)
#define LTR91100_PS_STATUS_REG_PS_INT_STATUS__REG         (LTR91100_PS_STATUS)

#define LTR91100_PS_STATUS_REG_PS_DATA_VALIDITY__POS      (2)
#define LTR91100_PS_STATUS_REG_PS_DATA_VALIDITY__MSK      (0x04)
#define LTR91100_PS_STATUS_REG_PS_DATA_VALIDITY__REG      (LTR91100_PS_STATUS)

#define LTR91100_PS_STATUS_REG_FAR_STATUS__POS            (3)
#define LTR91100_PS_STATUS_REG_FAR_STATUS__MSK            (0x08)
#define LTR91100_PS_STATUS_REG_FAR_STATUS__REG            (LTR91100_PS_STATUS)

#define LTR91100_PS_STATUS_REG_NEAR_STATUS__POS           (4)
#define LTR91100_PS_STATUS_REG_NEAR_STATUS__MSK           (0x10)
#define LTR91100_PS_STATUS_REG_NEAR_STATUS__REG           (LTR91100_PS_STATUS)

#define LTR91100_GS_STATUS_REG_GS_DATA_STATUS__POS        (3)
#define LTR91100_GS_STATUS_REG_GS_DATA_STATUS__MSK        (0x08)
#define LTR91100_GS_STATUS_REG_GS_DATA_STATUS__REG        (LTR91100_GS_STATUS)

#define LTR91100_GS_STATUS_REG_GS_INT_STATUS__POS         (4)
#define LTR91100_GS_STATUS_REG_GS_INT_STATUS__MSK         (0x10)
#define LTR91100_GS_STATUS_REG_GS_INT_STATUS__REG         (LTR91100_GS_STATUS)

#define LTR91100_GS_STATUS_REG_GS_DATA_VALIDITY__POS      (5)
#define LTR91100_GS_STATUS_REG_GS_DATA_VALIDITY__MSK      (0x20)
#define LTR91100_GS_STATUS_REG_GS_DATA_VALIDITY__REG      (LTR91100_GS_STATUS)

#define LTR91100_GS_STATUS_REG_GS_FIFO_FULL__POS          (6)
#define LTR91100_GS_STATUS_REG_GS_FIFO_FULL__MSK          (0x40)
#define LTR91100_GS_STATUS_REG_GS_FIFO_FULL__REG          (LTR91100_GS_STATUS)

#define LTR91100_GS_STATUS_REG_GS_FIFO_EMPTY__POS         (7)
#define LTR91100_GS_STATUS_REG_GS_FIFO_EMPTY__MSK         (0x80)
#define LTR91100_GS_STATUS_REG_GS_FIFO_EMPTY__REG         (LTR91100_GS_STATUS)

#define LTR91100_GET_BITSLICE(regvar, bitname)            ((regvar & LTR91100_##bitname##__MSK) >> LTR91100_##bitname##__POS)
#define LTR91100_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR91100_##bitname##__MSK) | ((val<<LTR91100_##bitname##__POS)&LTR91100_##bitname##__MSK))

#define LTR91100_WAIT_TIME_PER_CHECK                      (10)
#define LTR91100_WAIT_TIME_TOTAL                          (100)

typedef enum {
    LTR91100_SW_RESET_FALSE = 0x00,
    LTR91100_SW_RESET_TRUE = 0x01, 
} LTR91100_CFG_SW_RESET;

typedef enum {
    LTR91100_PS_INT_DISABLE = 0x00,
    LTR91100_PS_INT_ENABLE = 0x01, 
} LTR91100_CFG_PS_INT;

typedef enum {
    LTR91100_PS_STANDBY = 0x00,
    LTR91100_PS_ACTIVE = 0x01, 
} LTR91100_CFG_PS_MODE;

typedef enum {
    LTR91100_PS_GAIN_1X = 0x00,
} LTR91100_CFG_PS_GAIN;

typedef enum {
    LTR91100_PS_OFFSET_DISABLE = 0x00,
    LTR91100_PS_OFFSET_ENABLE = 0x01, 
} LTR91100_CFG_PS_OFFSET_EN;

typedef enum {
    LTR91100_NEAR_FAR_STATUS_DISABLE = 0x00,
    LTR91100_NEAR_FAR_STATUS_ENABLE = 0x01,
} LTR91100_CFG_NEAR_FAR_STATUS_EN;

typedef enum {
    LTR91100_GS_INT_DISABLE = 0x00,
    LTR91100_GS_INT_ENABLE = 0x01, 
} LTR91100_CFG_GS_INT;

typedef enum {
    LTR91100_GS_STANDBY = 0x00,
    LTR91100_GS_ACTIVE = 0x01, 
} LTR91100_CFG_GS_MODE;

typedef enum {
    LTR91100_GS_GAIN_1X = 0x00,
} LTR91100_CFG_GS_GAIN;

typedef enum {
    LTR91100_GS_OFFSET_DISABLE = 0x00,
    LTR91100_GS_OFFSET_ENABLE = 0x01, 
} LTR91100_CFG_GS_OFFSET_EN;

typedef enum {
    LTR91100_GS_FIFO_RESET_FALSE = 0x00,
    LTR91100_GS_FIFO_RESET_TRUE = 0x01, 
} LTR91100_CFG_GS_FIFO_RESET;

typedef enum {
    LTR91100_GS_FORCE_START_DISABLE = 0x00,
    LTR91100_GS_FORCE_START_ENABLE = 0x01, 
} LTR91100_CFG_GS_FORCE_START;

typedef enum {
    LTR91100_PLED_DRIVE_100mA = 0x00,
    LTR91100_PLED_DRIVE_50mA = 0x01,
    LTR91100_PLED_DRIVE_25mA = 0x02,
    LTR91100_PLED_DRIVE_12_5mA = 0x03,
} LTR91100_CFG_PLED_DRIVE;

typedef enum {
    LTR91100_PLED_BOOST_1X = 0x00,
    LTR91100_PLED_BOOST_1_5X = 0x01,
    LTR91100_PLED_BOOST_2X = 0x02,
    LTR91100_PLED_BOOST_3X = 0x03,
} LTR91100_CFG_PLED_BOOST;

typedef enum {
    LTR91100_PS_MEAS_RATE_6_125 = 0x00,                   /* GS Measurement Repeat Rate = 6.125ms */
    LTR91100_PS_MEAS_RATE_50 = 0x01,                      /* GS Measurement Repeat Rate = 50ms */
    LTR91100_PS_MEAS_RATE_100 = 0x02,                     /* GS Measurement Repeat Rate = 100ms (default) */
    LTR91100_PS_MEAS_RATE_200 = 0x03,                     /* GS Measurement Repeat Rate = 200ms */
    LTR91100_PS_MEAS_RATE_400 = 0x04,                     /* GS Measurement Repeat Rate = 400ms */
    LTR91100_PS_MEAS_RATE_800 = 0x05,                     /* GS Measurement Repeat Rate = 800ms */
    LTR91100_PS_MEAS_RATE_12_5 = 0x06,                    /* GS Measurement Repeat Rate = 12.5ms */
    LTR91100_PS_MEAS_RATE_25 = 0x07,                      /* GS Measurement Repeat Rate = 25ms */
} LTR91100_CFG_PS_MEAS_RATE;

typedef enum {
    LTR91100_GLED_DRIVE_100mA = 0x00,
    LTR91100_GLED_DRIVE_50mA = 0x01,
    LTR91100_GLED_DRIVE_25mA = 0x02,
    LTR91100_GLED_DRIVE_12_5mA = 0x03,
} LTR91100_CFG_GLED_DRIVE;

typedef enum {
    LTR91100_GLED_BOOST_1X = 0x00,
    LTR91100_GLED_BOOST_1_5X = 0x01,
    LTR91100_GLED_BOOST_2X = 0x02,
    LTR91100_GLED_BOOST_3X = 0x03,
} LTR91100_CFG_GLED_BOOST;

typedef enum {
    LTR91100_GS_WAIT_TIME_0ms = 0x00,
    LTR91100_GS_WAIT_TIME_2ms = 0x01,
    LTR91100_GS_WAIT_TIME_4ms = 0x02,
    LTR91100_GS_WAIT_TIME_6ms = 0x03,
    LTR91100_GS_WAIT_TIME_10ms = 0x04,
    LTR91100_GS_WAIT_TIME_14ms = 0x05,
    LTR91100_GS_WAIT_TIME_18ms = 0x06,
    LTR91100_GS_WAIT_TIME_22ms = 0x07,
} LTR91100_CFG_GS_WAIT_TIME;

typedef enum {
    LTR91100_GS_PERSIST_1st = 0x00,                       /* 1st gesture exit occurrence will exit gesture detections */
    LTR91100_GS_PERSIST_2nd = 0x01,                       /* 2nd gesture exit occurrence will exit gesture detections */
    LTR91100_GS_PERSIST_3rd = 0x02,                       /* 3rd gesture exit occurrence will exit gesture detections */
    LTR91100_GS_PERSIST_4th = 0x03,                       /* 4th gesture exit occurrence will exit gesture detections */
} LTR91100_CFG_GS_PERSIST;

typedef enum {
    LTR91100_PS_DATA_STATUS_OLD = 0x00,
    LTR91100_PS_DATA_STATUS_NEW = 0x01,
} LTR91100_CFG_PS_DATA_STATUS;

typedef enum {
    LTR91100_PS_INT_STATUS_INACTIVE = 0x00,
    LTR91100_PS_INT_STATUS_ACTIVE = 0x01,
} LTR91100_CFG_PS_INT_STATUS;

typedef enum {
    LTR91100_PS_DATA_VALID = 0x00,
    LTR91100_PS_DATA_INVALID = 0x01,
} LTR91100_CFG_PS_DATA_VALIDITY;

typedef enum {
    LTR91100_PS_FAR_STATUS_FALSE = 0x00,
    LTR91100_PS_FAR_STATUS_TRUE = 0x01,
} LTR91100_CFG_PS_FAR_STATUS;

typedef enum {
    LTR91100_PS_NEAR_STATUS_FALSE = 0x00,
    LTR91100_PS_NEAR_STATUS_TRUE = 0x01,
} LTR91100_CFG_PS_NEAR_STATUS;

typedef enum {
    LTR91100_GS_DATA_STATUS_OLD = 0x00,
    LTR91100_GS_DATA_STATUS_NEW = 0x01,
} LTR91100_CFG_GS_DATA_STATUS;

typedef enum {
    LTR91100_GS_INT_STATUS_INACTIVE = 0x00,
    LTR91100_GS_INT_STATUS_ACTIVE = 0x01,
} LTR91100_CFG_GS_INT_STATUS;

typedef enum {
    LTR91100_GS_DATA_VALID = 0x00,
    LTR91100_GS_DATA_INVALID = 0x01,
} LTR91100_CFG_GS_DATA_VALIDITY;

typedef enum {
    LTR91100_GS_FIFO_FULL_FALSE = 0x00,
    LTR91100_GS_FIFO_FULL_TRUE = 0x01,
} LTR91100_CFG_GS_FIFO_FULL;

typedef enum {
    LTR91100_GS_FIFO_EMPTY_TRUE = 0x00,
    LTR91100_GS_FIFO_EMPTY_FALSE = 0x01,
} LTR91100_CFG_GS_FIFO_EMPTY;

i2c_dev_t ltr91100_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR91100_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_gs_liteon_ltr91100_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR91100_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR91100_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_gs_liteon_ltr91100_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t ps_dev_mode = 0, gs_dev_mode = 0;
    uint8_t ps_value = 0, gs_value = 0;

    ret = sensor_i2c_read(drv, LTR91100_PS_CTRL, &ps_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    ret = sensor_i2c_read(drv, LTR91100_GS_CTRL, &gs_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            ps_dev_mode = LTR91100_SET_BITSLICE(ps_value, PS_CTRL_REG_PS_MODE, LTR91100_PS_STANDBY);
            gs_dev_mode = LTR91100_SET_BITSLICE(gs_value, GS_CTRL_REG_GS_MODE, LTR91100_GS_STANDBY);
            break;
        case DEV_POWER_ON:
            ps_dev_mode = LTR91100_SET_BITSLICE(ps_value, PS_CTRL_REG_PS_MODE, LTR91100_PS_ACTIVE);
            gs_dev_mode = LTR91100_SET_BITSLICE(gs_value, GS_CTRL_REG_GS_MODE, LTR91100_GS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR91100_PS_CTRL, &ps_dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    ret = sensor_i2c_write(drv, LTR91100_GS_CTRL, &gs_dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_gs_liteon_ltr91100_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR91100_GS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR91100_GET_BITSLICE(value, GS_STATUS_REG_GS_DATA_STATUS) == LTR91100_GS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_gs_liteon_ltr91100_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR91100_PS_CTRL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR91100_SET_BITSLICE(value, PS_CTRL_REG_PS_INT, LTR91100_PS_INT_DISABLE);
    value = LTR91100_SET_BITSLICE(value, PS_CTRL_REG_PS_GAIN, LTR91100_PS_GAIN_1X);
    value = LTR91100_SET_BITSLICE(value, PS_CTRL_REG_PS_OFFSET_EN, LTR91100_PS_OFFSET_DISABLE);
    value = LTR91100_SET_BITSLICE(value, PS_CTRL_REG_NEAR_FAR_STATUS_EN, LTR91100_NEAR_FAR_STATUS_DISABLE);
    ret = sensor_i2c_write(drv, LTR91100_PS_CTRL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR91100_GS_CTRL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR91100_SET_BITSLICE(value, GS_CTRL_REG_GS_INT, LTR91100_GS_INT_DISABLE);
    value = LTR91100_SET_BITSLICE(value, GS_CTRL_REG_GS_GAIN, LTR91100_GS_GAIN_1X);
    value = LTR91100_SET_BITSLICE(value, GS_CTRL_REG_GS_OFFSET_EN, LTR91100_GS_OFFSET_DISABLE);
    value = LTR91100_SET_BITSLICE(value, GS_CTRL_REG_GS_FIFO_RESET, LTR91100_GS_FIFO_RESET_FALSE);
    value = LTR91100_SET_BITSLICE(value, GS_CTRL_REG_GS_FORCE_START, LTR91100_GS_FORCE_START_ENABLE);
    ret = sensor_i2c_write(drv, LTR91100_GS_CTRL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR91100_SET_BITSLICE(value, PS_LED_REG_PLED_DRIVE, LTR91100_PLED_DRIVE_100mA);
    value = LTR91100_SET_BITSLICE(value, PS_LED_REG_PLED_BOOST, LTR91100_PLED_BOOST_1X);
    ret = sensor_i2c_write(drv, LTR91100_PS_LED, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR91100_SET_BITSLICE(value, PS_MEAS_RATE_REG_PULSES, 10);
    value = LTR91100_SET_BITSLICE(value, PS_MEAS_RATE_REG_MEAS_RATE, LTR91100_PS_MEAS_RATE_100);
    ret = sensor_i2c_write(drv, LTR91100_PS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR91100_SET_BITSLICE(value, GS_LED_REG_GLED_DRIVE, LTR91100_GLED_DRIVE_100mA);
    value = LTR91100_SET_BITSLICE(value, GS_LED_REG_GLED_BOOST, LTR91100_GLED_BOOST_1X);
    ret = sensor_i2c_write(drv, LTR91100_GS_LED, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR91100_SET_BITSLICE(value, GS_WAIT_REG_PULSES, 10);
    value = LTR91100_SET_BITSLICE(value, GS_WAIT_REG_WAIT_TIME, LTR91100_GS_WAIT_TIME_0ms);
    ret = sensor_i2c_write(drv, LTR91100_GS_WAIT, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    
    return 0;
}

static void drv_gs_liteon_ltr91100_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gs_liteon_ltr91100_open(void)
{
    int ret = 0;

    ret = drv_gs_liteon_ltr91100_set_power_mode(&ltr91100_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_gs_liteon_ltr91100_close(void)
{
    int ret = 0;

    ret = drv_gs_liteon_ltr91100_set_power_mode(&ltr91100_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gs_liteon_ltr91100_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t gs_ndata = 0, gs_sdata = 0, gs_edata = 0, gs_wdata = 0;
    uint8_t ps_data[2] = { 0 };
    proximity_data_t * pdata = (proximity_data_t *) buf;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(proximity_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr91100_ctx, LTR91100_PS_DATA_LSB, &ps_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr91100_ctx, LTR91100_PS_DATA_MSB, &ps_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr91100_ctx, LTR91100_GS_FIFO_ACCESS_N, &gs_ndata, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr91100_ctx, LTR91100_GS_FIFO_ACCESS_S, &gs_sdata, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr91100_ctx, LTR91100_GS_FIFO_ACCESS_E, &gs_edata, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr91100_ctx, LTR91100_GS_FIFO_ACCESS_W, &gs_wdata, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->present = gs_ndata;
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_gs_liteon_ltr91100_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_gs_liteon_ltr91100_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_gs_liteon_ltr91100_set_power_mode(&ltr91100_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR91100";
            info->unit = cm;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_gs_liteon_ltr91100_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_gs;
    memset(&sensor_gs, 0, sizeof(sensor_gs));

    if (!g_init_bitwise) {
        ret = drv_gs_liteon_ltr91100_validate_id(&ltr91100_ctx, LTR91100_PART_ID_VAL, LTR91100_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!g_init_bitwise) {
        /* fill the sensor_gs obj parameters here */
        sensor_gs.tag = TAG_DEV_GS;
        sensor_gs.path = dev_gs_path;
        sensor_gs.io_port = I2C_PORT;
        sensor_gs.mode = DEV_POLLING;
        sensor_gs.power = DEV_POWER_OFF;
        sensor_gs.open = drv_gs_liteon_ltr91100_open;
        sensor_gs.close = drv_gs_liteon_ltr91100_close;
        sensor_gs.read = drv_gs_liteon_ltr91100_read;
        sensor_gs.write = drv_gs_liteon_ltr91100_write;
        sensor_gs.ioctl = drv_gs_liteon_ltr91100_ioctl;
        sensor_gs.irq_handle = drv_gs_liteon_ltr91100_irq_handle;

        ret = sensor_create_obj(&sensor_gs);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_gs_liteon_ltr91100_set_default_config(&ltr91100_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise = 1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_gs_liteon_ltr91100_init);

