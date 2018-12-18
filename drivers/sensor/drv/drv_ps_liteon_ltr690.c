#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR690_I2C_SLAVE_ADDR                           0x23

#define LTR690_PS_CONTR                                 0x81 /* PS operation mode */
#define LTR690_PS_LED                                   0x82 /* LED pulse freq, current duty, peak current */
#define LTR690_PS_N_PULSES                              0x83 /* PS number of pulses */
#define LTR690_PS_MEAS_RATE                             0x84 /* measurement rate*/
#define LTR690_PART_ID                                  0x86
#define LTR690_MANUFAC_ID                               0x87
#define LTR690_PS_STATUS                                0x91
#define LTR690_PS_DATA_0                                0x92
#define LTR690_PS_DATA_1                                0x93
#define LTR690_INTERRUPT                                0xA6
#define LTR690_INTR_PRST                                0xA7 /* PS interrupt persist setting */
#define LTR690_PS_THRES_UP_0                            0xA8 /* PS interrupt upper threshold, lower byte */
#define LTR690_PS_THRES_UP_1                            0xA9 /* PS interrupt upper threshold, upper byte */
#define LTR690_PS_THRES_LOW_0                           0xAA /* PS interrupt lower threshold, lower byte */
#define LTR690_PS_THRES_LOW_1                           0xAB /* PS interrupt lower threshold, upper byte */
#define LTR690_PS_CROSSTALK_DATA_LSB                    0xB0 /* PS crosstalk data, lower byte */
#define LTR690_PS_CROSSTALK_DATA_MSB                    0xB1 /* PS crosstalk data, upper byte */

#define LTR690_ADDR_TRANS(n)                            ((n) << 1)
#define LTR690_I2C_ADDR                                 LTR690_ADDR_TRANS(LTR690_I2C_SLAVE_ADDR)
#define LTR690_PART_ID_VAL                              0x1E
#define LTR690_MANUFAC_ID_VAL                           0x05

#define LTR690_PS_CONTR_REG_SW_RESET__POS               (0)
#define LTR690_PS_CONTR_REG_SW_RESET__MSK               (0x01)
#define LTR690_PS_CONTR_REG_SW_RESET__REG               (LTR690_PS_CONTR)

#define LTR690_PS_CONTR_REG_PS_MODE__POS                (1)
#define LTR690_PS_CONTR_REG_PS_MODE__MSK                (0x02)
#define LTR690_PS_CONTR_REG_PS_MODE__REG                (LTR690_PS_CONTR)

#define LTR690_PS_CONTR_REG_FTN_NTF_EN__POS             (2)
#define LTR690_PS_CONTR_REG_FTN_NTF_EN__MSK             (0x04)
#define LTR690_PS_CONTR_REG_FTN_NTF_EN__REG             (LTR690_PS_CONTR)

#define LTR690_PS_CONTR_REG_PS_OS__POS                  (3)
#define LTR690_PS_CONTR_REG_PS_OS__MSK                  (0x08)
#define LTR690_PS_CONTR_REG_PS_OS__REG                  (LTR690_PS_CONTR)

#define LTR690_PS_LED_REG_LED_CURR__POS                 (0)
#define LTR690_PS_LED_REG_LED_CURR__MSK                 (0x07)
#define LTR690_PS_LED_REG_LED_CURR__REG                 (LTR690_PS_LED)

#define LTR690_PS_LED_REG_PULSE_WIDTH__POS              (4)
#define LTR690_PS_LED_REG_PULSE_WIDTH__MSK              (0x10)
#define LTR690_PS_LED_REG_PULSE_WIDTH__REG              (LTR690_PS_LED)

#define LTR690_PS_N_PULSES_REG_PULSES__POS              (0)
#define LTR690_PS_N_PULSES_REG_PULSES__MSK              (0x0F)
#define LTR690_PS_N_PULSES_REG_PULSES__REG              (LTR690_PS_N_PULSES)

#define LTR690_PS_N_PULSES_REG_SEQUENCE__POS            (4)
#define LTR690_PS_N_PULSES_REG_SEQUENCE__MSK            (0xF0)
#define LTR690_PS_N_PULSES_REG_SEQUENCE__REG            (LTR690_PS_N_PULSES)

#define LTR690_PS_MEAS_RATE_REG_MEAS_RATE__POS          (0)
#define LTR690_PS_MEAS_RATE_REG_MEAS_RATE__MSK          (0x07)
#define LTR690_PS_MEAS_RATE_REG_MEAS_RATE__REG          (LTR690_PS_MEAS_RATE)

#define LTR690_PS_STATUS_REG_PS_DATA_STATUS__POS        (0)
#define LTR690_PS_STATUS_REG_PS_DATA_STATUS__MSK        (0x01)
#define LTR690_PS_STATUS_REG_PS_DATA_STATUS__REG        (LTR690_PS_STATUS)

#define LTR690_PS_STATUS_REG_PS_INT_STATUS__POS         (1)
#define LTR690_PS_STATUS_REG_PS_INT_STATUS__MSK         (0x02)
#define LTR690_PS_STATUS_REG_PS_INT_STATUS__REG         (LTR690_PS_STATUS)

#define LTR690_PS_STATUS_REG_NTF__POS                   (6)
#define LTR690_PS_STATUS_REG_NTF__MSK                   (0x40)
#define LTR690_PS_STATUS_REG_NTF__REG                   (LTR690_PS_STATUS)

#define LTR690_PS_STATUS_REG_FTN__POS                   (7)
#define LTR690_PS_STATUS_REG_FTN__MSK                   (0x80)
#define LTR690_PS_STATUS_REG_FTN__REG                   (LTR690_PS_STATUS)

#define LTR690_GET_BITSLICE(regvar, bitname)            ((regvar & LTR690_##bitname##__MSK) >> LTR690_##bitname##__POS)
#define LTR690_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR690_##bitname##__MSK) | ((val<<LTR690_##bitname##__POS)&LTR690_##bitname##__MSK))

#define LTR690_WAIT_TIME_PER_CHECK                      (10)
#define LTR690_WAIT_TIME_TOTAL                          (50)

typedef enum {
    LTR690_SW_RESET_NEG = 0x00,
    LTR690_SW_RESET = 0x01, 
} LTR690_CFG_SW_RESET;

typedef enum {
    LTR690_PS_STANDBY = 0x00,
    LTR690_PS_ACTIVE = 0x01, 
} LTR690_CFG_PS_MODE;

typedef enum {
    LTR690_FTN_NTF_EN_FALSE = 0x00,
    LTR690_FTN_NTF_EN_TRUE = 0x01,
} LTR690_CFG_FTN_NTF_EN;

typedef enum {
    LTR690_PS_OS_FALSE = 0x00,
    LTR690_PS_OS_TRUE = 0x01, 
} LTR690_CFG_PS_OS;

typedef enum {
    LTR690_PS_LED_CURRENT_0 = 0x00,                     /* LED pulse current level = 0mA */
    LTR690_PS_LED_CURRENT_20 = 0x01,                    /* LED pulse current level = 20mA */
    LTR690_PS_LED_CURRENT_50 = 0x02,                    /* LED pulse current level = 50mA */
    LTR690_PS_LED_CURRENT_100 = 0x03,                   /* LED pulse current level = 100mA (default) */
    LTR690_PS_LED_CURRENT_150 = 0x04,                   /* LED pulse current level = 150mA */
    LTR690_PS_LED_CURRENT_200 = 0x05,                   /* LED pulse current level = 200mA */
    LTR690_PS_LED_CURRENT_250 = 0x06,                   /* LED pulse current level = 250mA */
    LTR690_PS_LED_CURRENT_300 = 0x07,                   /* LED pulse current level = 300mA */
} LTR690_CFG_PS_LED_LED_CURRENT;

typedef enum {
    LTR690_PS_PULSE_WIDTH_64 = 0x00,                    /* LED pulse width = 64us */
    LTR690_PS_PULSE_WIDTH_16 = 0x02,                    /* LED pulse width = 16us */
    LTR690_PS_PULSE_WIDTH_32 = 0x03,                    /* LED pulse width = 32us (default) */
} LTR690_CFG_PS_LED_PULSE_WIDTH;

typedef enum {
    LTR690_PS_MEAS_RATE_6_125 = 0x00,                   /* PS Measurement Repeat Rate = 6.125ms */
    LTR690_PS_MEAS_RATE_12_5 = 0x01,                    /* PS Measurement Repeat Rate = 12.5ms */
    LTR690_PS_MEAS_RATE_25 = 0x02,                      /* PS Measurement Repeat Rate = 25ms */
    LTR690_PS_MEAS_RATE_50 = 0x03,                      /* PS Measurement Repeat Rate = 50ms (default) */
    LTR690_PS_MEAS_RATE_100 = 0x04,                     /* PS Measurement Repeat Rate = 100ms */
    LTR690_PS_MEAS_RATE_200 = 0x05,                     /* PS Measurement Repeat Rate = 200ms */
    LTR690_PS_MEAS_RATE_400 = 0x06,                     /* PS Measurement Repeat Rate = 400ms */
    LTR690_PS_MEAS_RATE_800 = 0x07,                     /* PS Measurement Repeat Rate = 800ms */
} LTR690_CFG_PS_MEAS_RATE;

typedef enum {
    LTR690_PS_DATA_STATUS_OLD = 0x00,
    LTR690_PS_DATA_STATUS_NEW = 0x01,
} LTR690_CFG_PS_DATA_STATUS;

i2c_dev_t ltr690_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR690_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_ps_liteon_ltr690_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR690_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR690_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_ps_liteon_ltr690_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR690_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR690_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR690_PS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR690_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR690_PS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR690_PS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_ps_liteon_ltr690_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR690_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR690_GET_BITSLICE(value, PS_STATUS_REG_PS_DATA_STATUS) == LTR690_PS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_ps_liteon_ltr690_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR690_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR690_SET_BITSLICE(value, PS_CONTR_REG_FTN_NTF_EN, LTR690_FTN_NTF_EN_FALSE);
    value = LTR690_SET_BITSLICE(value, PS_CONTR_REG_PS_OS, LTR690_PS_OS_FALSE);
    ret = sensor_i2c_write(drv, LTR690_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0xC0;
    value = LTR690_SET_BITSLICE(value, PS_LED_REG_LED_CURR, LTR690_PS_LED_CURRENT_100);
    value = LTR690_SET_BITSLICE(value, PS_LED_REG_PULSE_WIDTH, LTR690_PS_PULSE_WIDTH_32);
    ret = sensor_i2c_write(drv, LTR690_PS_LED, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0x88;
    ret = sensor_i2c_write(drv, LTR690_PS_N_PULSES, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0x20;
    value = LTR690_SET_BITSLICE(value, PS_MEAS_RATE_REG_MEAS_RATE, LTR690_PS_MEAS_RATE_100);
    ret = sensor_i2c_write(drv, LTR690_PS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_ps_liteon_ltr690_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ps_liteon_ltr690_open(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr690_set_power_mode(&ltr690_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr690_close(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr690_set_power_mode(&ltr690_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_liteon_ltr690_read(void *buf, size_t len)
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

    ret = sensor_i2c_read(&ltr690_ctx, LTR690_PS_DATA_0, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr690_ctx, LTR690_PS_DATA_1, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->present = (((uint32_t) (reg_data[1] & 0xFF) << 8) | reg_data[0]);
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_ps_liteon_ltr690_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_ps_liteon_ltr690_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_ps_liteon_ltr690_set_power_mode(&ltr690_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR690";
            info->unit = cm;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_ps_liteon_ltr690_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_ps;
    memset(&sensor_ps, 0, sizeof(sensor_ps));

    if (!g_init_bitwise) {
        ret = drv_ps_liteon_ltr690_validate_id(&ltr690_ctx, LTR690_PART_ID_VAL, LTR690_MANUFAC_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!g_init_bitwise) {
        /* fill the sensor_ps obj parameters here */
        sensor_ps.tag = TAG_DEV_PS;
        sensor_ps.path = dev_ps_path;
        sensor_ps.io_port = I2C_PORT;
        sensor_ps.mode = DEV_POLLING;
        sensor_ps.power = DEV_POWER_OFF;
        sensor_ps.open = drv_ps_liteon_ltr690_open;
        sensor_ps.close = drv_ps_liteon_ltr690_close;
        sensor_ps.read = drv_ps_liteon_ltr690_read;
        sensor_ps.write = drv_ps_liteon_ltr690_write;
        sensor_ps.ioctl = drv_ps_liteon_ltr690_ioctl;
        sensor_ps.irq_handle = drv_ps_liteon_ltr690_irq_handle;

        ret = sensor_create_obj(&sensor_ps);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr690_set_default_config(&ltr690_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise = 1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_ps_liteon_ltr690_init);

