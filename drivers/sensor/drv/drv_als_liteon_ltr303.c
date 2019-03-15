#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"


/*******************************************************************************
 *********************************   MACROS   **********************************
 ******************************************************************************/
#define LTR303_ADDR_TRANS(n)                                ((n) << 1)
#define LTR303_GET_BITSLICE(uint8Val, bitName)              (((uint8Val) & (LTR303_##bitName##__MSK)) >> (LTR303_##bitName##__POS))
#define LTR303_SET_BITSLICE(uint8Val, bitName, bitVal)      (((uint8Val) & ~(LTR303_##bitName##__MSK)) | ((bitVal << (LTR303_##bitName##__POS)) & (LTR303_##bitName##__MSK)))

/*******************************************************************************
 **************************   SENSOR SPECIFICATIONS   **************************
 ******************************************************************************/
/* I2C device address */
#define LTR303_SLAVE_ADDR                       (0x29)
#define LTR303_I2C_ADDR                         LTR303_ADDR_TRANS(LTR303_SLAVE_ADDR)

/* Device info */
#define LTR303_PART_ID_VAL                      0xA0
#define LTR303_MANUFAC_ID_VAL                   0x05

#define LTR303_WAIT_TIME_PER_CHECK              (10)
#define LTR303_WAIT_TIME_TOTAL                  (100)

/*******************************************************************************
 *************   Non-Configurable (Unless data sheet is updated)   *************
 ******************************************************************************/
/* Device register set address */
#define LTR303_ALS_CONTR_REG                    (0x80)
#define LTR303_ALS_MEAS_RATE_REG                (0x85)
#define LTR303_PART_ID_REG                      (0x86)
#define LTR303_MANUFAC_ID_REG                   (0x87)
#define LTR303_ALS_DATA_CH1_0_REG               (0x88)
#define LTR303_ALS_DATA_CH1_1_REG               (0x89)
#define LTR303_ALS_DATA_CH0_0_REG               (0x8A)
#define LTR303_ALS_DATA_CH0_1_REG               (0x8B)
#define LTR303_ALS_STATUS_REG                   (0x8C)
#define LTR303_INTERRUPT_REG                    (0x8F)
#define LTR303_ALS_THRES_UP_0_REG               (0x97)
#define LTR303_ALS_THRES_UP_1_REG               (0x98)
#define LTR303_ALS_THRES_LOW_0_REG              (0x99)
#define LTR303_ALS_THRES_LOW_1_REG              (0x9A)
#define LTR303_INTERRUPT_PERSIST_REG            (0x9E)

/* Register ALS_CONTR field */
#define LTR303_ALS_MODE__REG                    (LTR303_ALS_CONTR_REG)
#define LTR303_ALS_MODE__POS                    (0)
#define LTR303_ALS_MODE__MSK                    (0x01)
#define LTR303_SW_RESET__REG                    (LTR303_ALS_CONTR_REG)
#define LTR303_SW_RESET__POS                    (1)
#define LTR303_SW_RESET__MSK                    (0x02)
#define LTR303_ALS_GAIN__REG                    (LTR303_ALS_CONTR_REG)
#define LTR303_ALS_GAIN__POS                    (2)
#define LTR303_ALS_GAIN__MSK                    (0x1C)

/* Register ALS_MEAS_RATE field */
#define LTR303_ALS_MEAS_RPT_RATE__REG           (LTR303_ALS_MEAS_RATE_REG)
#define LTR303_ALS_MEAS_RPT_RATE__POS           (0)
#define LTR303_ALS_MEAS_RPT_RATE__MSK           (0x07)
#define LTR303_ALS_INTEG_TIME__REG              (LTR303_ALS_MEAS_RATE_REG)
#define LTR303_ALS_INTEG_TIME__POS              (3)
#define LTR303_ALS_INTEG_TIME__MSK              (0x38)

/* Register PART_ID field */
#define LTR303_REVISION_ID__REG                 (LTR303_PART_ID_REG)
#define LTR303_REVISION_ID__POS                 (0)
#define LTR303_REVISION_ID__MSK                 (0x0F)
#define LTR303_PART_NUMBER_ID__REG              (LTR303_PART_ID_REG)
#define LTR303_PART_NUMBER_ID__POS              (4)
#define LTR303_PART_NUMBER_ID__MSK              (0xF0)

/* Register MANUFAC_ID field */
#define LTR303_MANUFAC_ID__REG                  (LTR303_MANUFAC_ID_REG)
#define LTR303_MANUFAC_ID__POS                  (0)
#define LTR303_MANUFAC_ID__MSK                  (0xFF)

/* Register ALS_DATA field */
#define LTR303_ALS_DATA_CH1_0__REG              (LTR303_ALS_DATA_CH1_0_REG)
#define LTR303_ALS_DATA_CH1_0__POS              (0)
#define LTR303_ALS_DATA_CH1_0__MSK              (0xFF)
#define LTR303_ALS_DATA_CH1_1__REG              (LTR303_ALS_DATA_CH1_1_REG)
#define LTR303_ALS_DATA_CH1_1__POS              (0)
#define LTR303_ALS_DATA_CH1_1__MSK              (0xFF)
#define LTR303_ALS_DATA_CH0_0__REG              (LTR303_ALS_DATA_CH0_0_REG)
#define LTR303_ALS_DATA_CH0_0__POS              (0)
#define LTR303_ALS_DATA_CH0_0__MSK              (0xFF)
#define LTR303_ALS_DATA_CH0_1__REG              (LTR303_ALS_DATA_CH0_1_REG)
#define LTR303_ALS_DATA_CH0_1__POS              (0)
#define LTR303_ALS_DATA_CH0_1__MSK              (0xFF)

/* Register ALS_STATUS field */
#define LTR303_ALS_DATA_STATUS__REG             (LTR303_ALS_STATUS_REG)
#define LTR303_ALS_DATA_STATUS__POS             (2)
#define LTR303_ALS_DATA_STATUS__MSK             (0x04)
#define LTR303_ALS_INT_STATUS__REG              (LTR303_ALS_STATUS_REG)
#define LTR303_ALS_INT_STATUS__POS              (3)
#define LTR303_ALS_INT_STATUS__MSK              (0x08)
#define LTR303_ALS_GAIN_STATUS__REG             (LTR303_ALS_STATUS_REG)
#define LTR303_ALS_GAIN_STATUS__POS             (4)
#define LTR303_ALS_GAIN_STATUS__MSK             (0x70)
#define LTR303_ALS_DATA_VALIDITY__REG           (LTR303_ALS_STATUS_REG)
#define LTR303_ALS_DATA_VALIDITY__POS           (7)
#define LTR303_ALS_DATA_VALIDITY__MSK           (0x80)

/* Register INTERRUPT field */
#define LTR303_INT_MODE__REG                    (LTR303_INTERRUPT_REG)
#define LTR303_INT_MODE__POS                    (1)
#define LTR303_INT_MODE__MSK                    (0x02)
#define LTR303_INT_POLARITY__REG                (LTR303_INTERRUPT_REG)
#define LTR303_INT_POLARITY__POS                (2)
#define LTR303_INT_POLARITY__MSK                (0x04)

/* Register ALS_THRES field */
#define LTR303_ALS_THRES_UP_0__REG              (LTR303_ALS_THRES_UP_0_REG)
#define LTR303_ALS_THRES_UP_0__POS              (0)
#define LTR303_ALS_THRES_UP_0__MSK              (0xFF)
#define LTR303_ALS_THRES_UP_1__REG              (LTR303_ALS_THRES_UP_1_REG)
#define LTR303_ALS_THRES_UP_1__POS              (0)
#define LTR303_ALS_THRES_UP_1__MSK              (0xFF)
#define LTR303_ALS_THRES_LOW_0__REG             (LTR303_ALS_THRES_LOW_0_REG)
#define LTR303_ALS_THRES_LOW_0__POS             (0)
#define LTR303_ALS_THRES_LOW_0__MSK             (0xFF)
#define LTR303_ALS_THRES_LOW_1__REG             (LTR303_ALS_THRES_LOW_1_REG)
#define LTR303_ALS_THRES_LOW_1__POS             (0)
#define LTR303_ALS_THRES_LOW_1__MSK             (0xFF)

/* Register INTERRUPT_PERSIST field */
#define LTR303_ALS_PERSIST__REG                 (LTR303_INTERRUPT_PERSIST_REG)
#define LTR303_ALS_PERSIST__POS                 (0)
#define LTR303_ALS_PERSIST__MSK                 (0x0F)

/* Field value enumeration */
typedef enum {
    LTR303_ALS_STANDBY_MODE = 0x00,
    LTR303_ALS_ACTIVE_MODE = 0x01,
} LTR303_ALS_MODE_VAL;

typedef enum {
    LTR303_ALS_NO_RESET = 0x00,
    LTR303_ALS_RESET = 0x01,
} LTR_303_SW_RESET_VAL;

typedef enum {
    LTR303_ALS_GAIN_1x = 0x00,                  /* 1 lux to 64k lux (default) */
    LTR303_ALS_GAIN_2x = 0x01,                  /* 0.5 lux to 32k lux */
    LTR303_ALS_GAIN_4x = 0x02,                  /* 0.25 lux to 16k lux */
    LTR303_ALS_GAIN_8x = 0x03,                  /* 0.125 lux to 8k lux */
    LTR303_ALS_GAIN_48x = 0x06,                 /* 0.02 lux to 1.3k lux */
    LTR303_ALS_GAIN_96x = 0x07,                 /* 0.01 lux to 600 lux */
} LTR303_ALS_Gain_VAL;

typedef enum {
    LTR303_ALS_MEAS_RPT_RATE_50MS = 0x00,
    LTR303_ALS_MEAS_RPT_RATE_100MS = 0x01,
    LTR303_ALS_MEAS_RPT_RATE_200MS = 0x02,
    LTR303_ALS_MEAS_RPT_RATE_500MS = 0x03,
    LTR303_ALS_MEAS_RPT_RATE_1000MS = 0x04,
    LTR303_ALS_MEAS_RPT_RATE_2000MS = 0x05,
} LTR303_ALS_MEAS_RPT_RATE_VAL;

typedef enum {
    LTR303_ALS_INTEG_TIME_100MS = 0x00,
    LTR303_ALS_INTEG_TIME_50MS = 0x01,
    LTR303_ALS_INTEG_TIME_200MS = 0x02,
    LTR303_ALS_INTEG_TIME_400MS = 0x03,
    LTR303_ALS_INTEG_TIME_150MS = 0x04,
    LTR303_ALS_INTEG_TIME_250MS = 0x05,
    LTR303_ALS_INTEG_TIME_300MS = 0x06,
    LTR303_ALS_INTEG_TIME_350MS = 0x07,
} LTR303_ALS_INTEG_TIME_VAL;

typedef enum {
    LTR303_ALS_DATA_OLD = 0x00,
    LTR303_ALS_DATA_NEW = 0x01,
} LTR303_ALS_DATA_STATUS_VAL;

typedef enum {
    LTR303_ALS_INT_INACTIVE = 0x00,
    LTR303_ALS_INT_ACTIVE = 0x01,
} LTR303_ALS_INT_STATUS_VAL;

typedef enum {
    LTR303_ALS_DATA_VALID = 0x00,
    LTR303_ALS_DATA_INVALID = 0x01,
} LTR303_ALS_DATA_VALIDITY_VAL;

typedef enum {
    LTR303_INT_MODE_INACTIVE = 0x00,
    LTR303_INT_MODE_ACTIVE = 0x01,
} LTR303_INT_MODE_VAL;

typedef enum {
    LTR303_INT_POLARITY_ACTIVE_LO = 0x00,
    LTR303_INT_POLARITY_ACTIVE_HI = 0x01,
} LTR303_INT_POLARITY_VAL;

typedef enum {
    LTR303_ALS_PERSIST_EACH = 0x00,
    LTR303_ALS_PERSIST_2_CONT = 0x01,
    LTR303_ALS_PERSIST_16_CONT = 0x0F,
} LTR303_ALS_PERSIST_VAL;


i2c_dev_t ltr303_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR303_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;


static int drv_als_liteon_ltr303_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR303_PART_ID_REG, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR303_MANUFAC_ID_REG, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_als_liteon_ltr303_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR303_ALS_CONTR_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR303_SET_BITSLICE(value, ALS_MODE, LTR303_ALS_STANDBY_MODE);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR303_SET_BITSLICE(value, ALS_MODE, LTR303_ALS_ACTIVE_MODE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR303_ALS_CONTR_REG, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_als_liteon_ltr303_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR303_ALS_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR303_GET_BITSLICE(value, ALS_DATA_STATUS) == LTR303_ALS_DATA_NEW) ? 1 : 0;

    return ret;
}

/**************************************************************************//**
 * @brief
 *   Configure the settings of the sensor.
 *   This function assumes that the 100ms wait time requirement
 *   after sensor power up has been fulfilled.
 * @details
 *   1. The function first sets ALS_MODE field to Standby mode.
 *   2. Then it configures the corresponding registers to default configurations.
 * @param[in] drv
 *   The I2C peripheral to use.
 * @return
 *   0 if configuration is successful, -1 otherwise.
 *****************************************************************************/
static int drv_als_liteon_ltr303_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    /* Set ALS_MODE field to standby mode, no SW reset, and set ALS_GAIN field */
    value = LTR303_SET_BITSLICE(value, ALS_MODE, LTR303_ALS_STANDBY_MODE);
    value = LTR303_SET_BITSLICE(value, SW_RESET, LTR303_ALS_NO_RESET);
    value = LTR303_SET_BITSLICE(value, ALS_GAIN, LTR303_ALS_GAIN_1x);
    ret = sensor_i2c_write(drv, LTR303_ALS_CONTR_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* Set ALS_MEAS_RPT_RATE, ALS_INTEG_TIME fields */
    value = 0;
    value = LTR303_SET_BITSLICE(value, ALS_MEAS_RPT_RATE, LTR303_ALS_MEAS_RPT_RATE_500MS);
    value = LTR303_SET_BITSLICE(value, ALS_INTEG_TIME, LTR303_ALS_INTEG_TIME_100MS);
    ret = sensor_i2c_write(drv, LTR303_ALS_MEAS_RATE_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* Set INT_MODE, INT_POLARITY fields */
    value = 0;
    value = LTR303_SET_BITSLICE(value, INT_MODE, LTR303_INT_MODE_INACTIVE);
    value = LTR303_SET_BITSLICE(value, INT_POLARITY, LTR303_INT_POLARITY_ACTIVE_LO);
    ret = sensor_i2c_write(drv, LTR303_INTERRUPT_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* Set ALS_PERSIST fields */
    value = 0;
    value = LTR303_SET_BITSLICE(value, ALS_PERSIST, LTR303_ALS_PERSIST_EACH);
    ret = sensor_i2c_write(drv, LTR303_INTERRUPT_PERSIST_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static uint8_t drv_als_liteon_ltr303_get_gain_val(i2c_dev_t* drv)
{
    uint8_t als_gain = 0, als_gain_val = 0;
    bool    ret = false;

    ret = sensor_i2c_read(drv, LTR303_ALS_STATUS_REG, &als_gain, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (!unlikely(ret))
    {
        als_gain = LTR303_GET_BITSLICE(als_gain, ALS_GAIN_STATUS);
        switch (als_gain)
        {
            case LTR303_ALS_GAIN_1x:
                als_gain_val = 1;
                break;
            case LTR303_ALS_GAIN_2x:
                als_gain_val = 2;
                break;
            case LTR303_ALS_GAIN_4x:
                als_gain_val = 4;
                break;
            case LTR303_ALS_GAIN_8x:
                als_gain_val = 8;
                break;
            case LTR303_ALS_GAIN_48x:
                als_gain_val = 48;
                break;
            case LTR303_ALS_GAIN_96x:
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

static uint16_t drv_als_liteon_ltr303_get_integ_time_val(i2c_dev_t* drv)
{
    uint16_t als_integ = 0, als_integ_val = 0;
    bool     ret = false;

    ret = sensor_i2c_read(drv, LTR303_ALS_MEAS_RATE_REG, (uint8_t*)&als_integ, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (!unlikely(ret))
    {
        als_integ = LTR303_GET_BITSLICE(als_integ, ALS_INTEG_TIME);
        switch (als_integ)
        {
            case LTR303_ALS_INTEG_TIME_100MS:
                als_integ_val = 10;
                break;
            case LTR303_ALS_INTEG_TIME_50MS:
                als_integ_val = 5;
                break;
            case LTR303_ALS_INTEG_TIME_200MS:
                als_integ_val = 20;
                break;
            case LTR303_ALS_INTEG_TIME_400MS:
                als_integ_val = 40;
                break;
            case LTR303_ALS_INTEG_TIME_150MS:
                als_integ_val = 15;
                break;
            case LTR303_ALS_INTEG_TIME_250MS:
                als_integ_val = 25;
                break;
            case LTR303_ALS_INTEG_TIME_300MS:
                als_integ_val = 30;
                break;
            case LTR303_ALS_INTEG_TIME_350MS:
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

static void drv_als_liteon_ltr303_irq_handle(void)
{
    /* no handle so far */
}

static int drv_als_liteon_ltr303_open(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr303_set_power_mode(&ltr303_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_liteon_ltr303_close(void)
{
    int ret = 0;

    ret = drv_als_liteon_ltr303_set_power_mode(&ltr303_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_als_liteon_ltr303_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_ch1_data[2] = { 0 };
    uint8_t reg_ch0_data[2] = { 0 };
    uint32_t als_data_ch0 = 0, als_data_ch1 = 0, chRatio = 0, tmpCalc = 0;
    uint16_t als_gain_val = 0, als_integ_time_val = 0;
    als_data_t * pdata = (als_data_t *) buf;

    if (buf == NULL){
        return -1;
    }

    size = sizeof(als_data_t);
    if (len < size){
        return -1;
    }

    ret = sensor_i2c_read(&ltr303_ctx, LTR303_ALS_DATA_CH1_0_REG, &reg_ch1_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr303_ctx, LTR303_ALS_DATA_CH1_1_REG, &reg_ch1_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr303_ctx, LTR303_ALS_DATA_CH0_0_REG, &reg_ch0_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr303_ctx, LTR303_ALS_DATA_CH0_1_REG, &reg_ch0_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    als_data_ch0 = ((uint16_t) reg_ch0_data[1] << 8 | reg_ch0_data[0]);
    als_data_ch1 = ((uint16_t) reg_ch1_data[1] << 8 | reg_ch1_data[0]);

    chRatio = (als_data_ch1 * 100) / (als_data_ch0 + als_data_ch1);
    if (chRatio < 45)
    {
        tmpCalc = (1774 * als_data_ch0 + 1106 * als_data_ch1);
    }
    else if (chRatio >= 45 && chRatio < 64)
    {
        tmpCalc = (4279 * als_data_ch0 - 1955 * als_data_ch1);
    }
    else if (chRatio >= 64 && chRatio < 85)
    {
        tmpCalc = (593 * als_data_ch0 + 119 * als_data_ch1);
    }
    else
    {
        tmpCalc = 0;
    }

    als_gain_val = drv_als_liteon_ltr303_get_gain_val(&ltr303_ctx);
    als_integ_time_val = drv_als_liteon_ltr303_get_integ_time_val(&ltr303_ctx);
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

static int drv_als_liteon_ltr303_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_als_liteon_ltr303_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_als_liteon_ltr303_set_power_mode(&ltr303_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR303";
            info->unit = lux;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_als_liteon_ltr303_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_als;
    memset(&sensor_als, 0, sizeof(sensor_als));

    if (!g_init_bitwise) {
        ret = drv_als_liteon_ltr303_validate_id(&ltr303_ctx, LTR303_PART_ID_VAL, LTR303_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!g_init_bitwise) {
        /* fill the sensor_als obj parameters here */
        sensor_als.path = dev_als_path;
        sensor_als.tag = TAG_DEV_ALS;
        sensor_als.io_port = I2C_PORT;
        sensor_als.mode = DEV_POLLING;
        sensor_als.power = DEV_POWER_OFF;
        sensor_als.open = drv_als_liteon_ltr303_open;
        sensor_als.close = drv_als_liteon_ltr303_close;
        sensor_als.read = drv_als_liteon_ltr303_read;
        sensor_als.write = drv_als_liteon_ltr303_write;
        sensor_als.ioctl = drv_als_liteon_ltr303_ioctl;
        sensor_als.irq_handle = drv_als_liteon_ltr303_irq_handle;

        ret = sensor_create_obj(&sensor_als);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_als_liteon_ltr303_set_default_config(&ltr303_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_als_liteon_ltr303_init);

