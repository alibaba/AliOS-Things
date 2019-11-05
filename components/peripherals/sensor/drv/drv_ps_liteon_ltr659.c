#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR659_I2C_SLAVE_ADDR                           0x23

#define LTR659_SW_RESET                                 0x80
#define LTR659_PS_CONTR                                 0x81 /* PS operation mode */
#define LTR659_PS_LED                                   0x82 /* LED pulse freq, current duty, peak current */
#define LTR659_PS_N_PULSES                              0x83 /* PS number of pulses */
#define LTR659_PS_MEAS_RATE                             0x84 /* measurement rate*/
#define LTR659_PART_ID                                  0x86
#define LTR659_MANUFAC_ID                               0x87
#define LTR659_PS_STATUS                                0x8C
#define LTR659_PS_DATA_0                                0x8D
#define LTR659_PS_DATA_1                                0x8E
#define LTR659_INTERRUPT                                0x8F
#define LTR659_PS_THRES_UP_0                            0x90 /* PS interrupt upper threshold, lower byte */
#define LTR659_PS_THRES_UP_1                            0x91 /* PS interrupt upper threshold, upper byte */
#define LTR659_PS_THRES_LOW_0                           0x92 /* PS interrupt lower threshold, lower byte */
#define LTR659_PS_THRES_LOW_1                           0x93 /* PS interrupt lower threshold, upper byte */
#define LTR659_PS_OFFSET_1                              0x94 /* PS offset, upper byte */
#define LTR659_PS_OFFSET_0                              0x95 /* PS offset, lower byte */
#define LTR659_INTR_PRST                                0x9E /* PS interrupt persist setting */

#define LTR659_ADDR_TRANS(n)                            ((n) << 1)
#define LTR659_I2C_ADDR                                 LTR659_ADDR_TRANS(LTR659_I2C_SLAVE_ADDR)
#define LTR659_PART_ID_VAL                              0x92
#define LTR659_MANUFAC_ID_VAL                           0x05

#define LTR659_SW_RESET_REG_SW_RESET__POS               (1)
#define LTR659_SW_RESET_REG_SW_RESET__MSK               (0x02)
#define LTR659_SW_RESET_REG_SW_RESET__REG               (LTR659_SW_RESET)

#define LTR659_PS_CONTR_REG_PS_MODE__POS                (0)
#define LTR659_PS_CONTR_REG_PS_MODE__MSK                (0x03)
#define LTR659_PS_CONTR_REG_PS_MODE__REG                (LTR659_PS_CONTR)

#define LTR659_PS_CONTR_REG_PS_GAIN__POS                (2)
#define LTR659_PS_CONTR_REG_PS_GAIN__MSK                (0x0C)
#define LTR659_PS_CONTR_REG_PS_GAIN__REG                (LTR659_PS_CONTR)

#define LTR659_PS_CONTR_REG_PS_SAT_IND_EN__POS          (5)
#define LTR659_PS_CONTR_REG_PS_SAT_IND_EN__MSK          (0x20)
#define LTR659_PS_CONTR_REG_PS_SAT_IND_EN__REG          (LTR659_PS_CONTR)

#define LTR659_PS_LED_REG_LED_CURR__POS                 (0)
#define LTR659_PS_LED_REG_LED_CURR__MSK                 (0x07)
#define LTR659_PS_LED_REG_LED_CURR__REG                 (LTR659_PS_LED)

#define LTR659_PS_LED_REG_LED_DUTY__POS                 (3)
#define LTR659_PS_LED_REG_LED_DUTY__MSK                 (0x18)
#define LTR659_PS_LED_REG_LED_DUTY__REG                 (LTR659_PS_LED)

#define LTR659_PS_LED_REG_LED_PULSE__POS                (5)
#define LTR659_PS_LED_REG_LED_PULSE__MSK                (0xE0)
#define LTR659_PS_LED_REG_LED_PULSE__REG                (LTR659_PS_LED)

#define LTR659_PS_N_PULSES_REG_PULSES__POS              (0)
#define LTR659_PS_N_PULSES_REG_PULSES__MSK              (0x0F)
#define LTR659_PS_N_PULSES_REG_PULSES__REG              (LTR659_PS_N_PULSES)

#define LTR659_PS_MEAS_RATE_REG_MEAS_RPT_RATE__POS      (0)
#define LTR659_PS_MEAS_RATE_REG_MEAS_RPT_RATE__MSK      (0x0F)
#define LTR659_PS_MEAS_RATE_REG_MEAS_RPT_RATE__REG      (LTR659_PS_MEAS_RATE)

#define LTR659_PS_STATUS_REG_PS_DATA_STATUS__POS        (0)
#define LTR659_PS_STATUS_REG_PS_DATA_STATUS__MSK        (0x01)
#define LTR659_PS_STATUS_REG_PS_DATA_STATUS__REG        (LTR659_PS_STATUS)

#define LTR659_PS_STATUS_REG_PS_INT_STATUS__POS         (1)
#define LTR659_PS_STATUS_REG_PS_INT_STATUS__MSK         (0x02)
#define LTR659_PS_STATUS_REG_PS_INT_STATUS__REG         (LTR659_PS_STATUS)

#define LTR659_INTERRUPT_REG_INT_MODE__POS              (0)
#define LTR659_INTERRUPT_REG_INT_MODE__MSK              (0x01)
#define LTR659_INTERRUPT_REG_INT_MODE__REG              (LTR659_INTERRUPT)

#define LTR659_INTERRUPT_REG_INT_POLARITY__POS          (2)
#define LTR659_INTERRUPT_REG_INT_POLARITY__MSK          (0x04)
#define LTR659_INTERRUPT_REG_INT_POLARITY__REG          (LTR659_INTERRUPT)

#define LTR659_INTR_PRST_REG_PS_PERSIST__POS            (4)
#define LTR659_INTR_PRST_REG_PS_PERSIST__MSK            (0xF0)
#define LTR659_INTR_PRST_REG_PS_PERSIST__REG            (LTR659_INTR_PRST)

#define LTR659_GET_BITSLICE(regvar, bitname)            ((regvar & LTR659_##bitname##__MSK) >> LTR659_##bitname##__POS)
#define LTR659_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR659_##bitname##__MSK) | ((val<<LTR659_##bitname##__POS)&LTR659_##bitname##__MSK))

#define LTR659_WAIT_TIME_PER_CHECK                      (10)
#define LTR659_WAIT_TIME_TOTAL                          (100)

typedef enum {
    LTR659_SW_RESET_FALSE = 0x00,
    LTR659_SW_RESET_TRUE = 0x01, 
} LTR659_CFG_SW_RESET;

typedef enum {
    LTR659_PS_STANDBY = 0x00,
    LTR659_PS_ACTIVE = 0x02, 
} LTR659_CFG_PS_MODE;

typedef enum {
    LTR659_PS_GAIN_16X = 0x00,
    LTR659_PS_GAIN_32X = 0x02,
    LTR659_PS_GAIN_64X = 0x03,
} LTR659_CFG_PS_GAIN;

typedef enum {
    LTR659_PS_SAT_IND_FALSE = 0x00,
    LTR659_PS_SAT_IND_TRUE = 0x01, 
} LTR659_CFG_PS_SAT_IND_EN;

typedef enum {
    LTR659_LED_PEAK_CURRENT_5 = 0x00,                   /* LED pulse current level = 5mA */
    LTR659_LED_PEAK_CURRENT_10 = 0x01,                  /* LED pulse current level = 10mA */
    LTR659_LED_PEAK_CURRENT_20 = 0x02,                  /* LED pulse current level = 20mA */
    LTR659_LED_PEAK_CURRENT_50 = 0x03,                  /* LED pulse current level = 50mA */
    LTR659_LED_PEAK_CURRENT_100 = 0x04,                 /* LED pulse current level = 100mA (default) */
} LTR659_CFG_LED_PEAK_CURRENT;

typedef enum {
    LTR659_LED_DUTY_CYCLE_25PCT = 0x00,                 /* Duty = 25% */
    LTR659_LED_DUTY_CYCLE_50PCT = 0x01,                 /* Duty = 50% */
    LTR659_LED_DUTY_CYCLE_75PCT = 0x02,                 /* Duty = 75% */
    LTR659_LED_DUTY_CYCLE_100PCT = 0x03,                /* Duty = 100% (default) */
} LTR659_CFG_LED_DUTY_CYCLE;

typedef enum {
    LTR659_LED_PULSE_30kHZ = 0x00,                      /* LED pulse period = 30kHz */
    LTR659_LED_PULSE_40kHZ = 0x01,                      /* LED pulse period = 40kHz */
    LTR659_LED_PULSE_50kHZ = 0x02,                      /* LED pulse period = 50kHz */
    LTR659_LED_PULSE_60kHZ = 0x03,                      /* LED pulse period = 60kHz */
    LTR659_LED_PULSE_70kHZ = 0x04,                      /* LED pulse period = 70kHz */
    LTR659_LED_PULSE_80kHZ = 0x05,                      /* LED pulse period = 80kHz */
    LTR659_LED_PULSE_90kHZ = 0x06,                      /* LED pulse period = 90kHz */
    LTR659_LED_PULSE_100kHZ = 0x07,                     /* LED pulse period = 100kHz */
} LTR659_CFG_LED_PULSE_FREQUENCY;

typedef enum {
    LTR659_PS_MEAS_RATE_50 = 0x00,                      /* PS Measurement Repeat Rate = 50ms */
    LTR659_PS_MEAS_RATE_70 = 0x01,                      /* PS Measurement Repeat Rate = 70ms */
    LTR659_PS_MEAS_RATE_100 = 0x02,                     /* PS Measurement Repeat Rate = 100ms (default) */
    LTR659_PS_MEAS_RATE_200 = 0x03,                     /* PS Measurement Repeat Rate = 20ms */
    LTR659_PS_MEAS_RATE_500 = 0x04,                     /* PS Measurement Repeat Rate = 500ms */
    LTR659_PS_MEAS_RATE_1000 = 0x05,                    /* PS Measurement Repeat Rate = 1000ms */
    LTR659_PS_MEAS_RATE_2000 = 0x06,                    /* PS Measurement Repeat Rate = 2000ms */
    LTR659_PS_MEAS_RATE_10 = 0x08,                      /* PS Measurement Repeat Rate = 10ms */
} LTR659_CFG_PS_MEAS_RPT_RATE;

typedef enum {
    LTR659_PS_DATA_STATUS_OLD = 0x00,
    LTR659_PS_DATA_STATUS_NEW = 0x01,
} LTR659_CFG_PS_DATA_STATUS;

typedef enum {
    LTR659_PS_INT_STATUS_INACTIVE = 0x00,
    LTR659_PS_INT_STATUS_ACTIVE = 0x01,
} LTR659_CFG_PS_INT_STATUS;

typedef enum {
    LTR659_PS_INT_MODE_DISABLE = 0x00,
    LTR659_PS_INT_MODE_ENABLE = 0x01,
} LTR659_CFG_PS_INT_MODE;

typedef enum {
    LTR659_PS_INT_POLARITY_ACTIVE_LO = 0x00,
    LTR659_PS_INT_POLARITY_ACTIVE_HI = 0x01,
} LTR659_CFG_PS_INT_POLARITY;

i2c_dev_t ltr659_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR659_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_ps_liteon_ltr659_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR659_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR659_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_ps_liteon_ltr659_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR659_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR659_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR659_PS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR659_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR659_PS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR659_PS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_ps_liteon_ltr659_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR659_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR659_GET_BITSLICE(value, PS_STATUS_REG_PS_DATA_STATUS) == LTR659_PS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_ps_liteon_ltr659_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR659_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR659_SET_BITSLICE(value, PS_CONTR_REG_PS_GAIN, LTR659_PS_GAIN_16X);
    value = LTR659_SET_BITSLICE(value, PS_CONTR_REG_PS_SAT_IND_EN, LTR659_PS_SAT_IND_FALSE);
    ret = sensor_i2c_write(drv, LTR659_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR659_SET_BITSLICE(value, PS_LED_REG_LED_CURR, LTR659_LED_PEAK_CURRENT_100);
    value = LTR659_SET_BITSLICE(value, PS_LED_REG_LED_DUTY, LTR659_LED_DUTY_CYCLE_100PCT);
    value = LTR659_SET_BITSLICE(value, PS_LED_REG_LED_PULSE, LTR659_LED_PULSE_60kHZ);
    ret = sensor_i2c_write(drv, LTR659_PS_LED, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR659_SET_BITSLICE(value, PS_N_PULSES_REG_PULSES, 4);
    ret = sensor_i2c_write(drv, LTR659_PS_N_PULSES, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR659_SET_BITSLICE(value, PS_MEAS_RATE_REG_MEAS_RPT_RATE, LTR659_PS_MEAS_RATE_100);
    ret = sensor_i2c_write(drv, LTR659_PS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_ps_liteon_ltr659_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ps_liteon_ltr659_open(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr659_set_power_mode(&ltr659_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr659_close(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr659_set_power_mode(&ltr659_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_liteon_ltr659_read(void *buf, size_t len)
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

    ret = sensor_i2c_read(&ltr659_ctx, LTR659_PS_DATA_0, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr659_ctx, LTR659_PS_DATA_1, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->present = (((uint32_t) (reg_data[1] & 0x07) << 8) | reg_data[0]);
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_ps_liteon_ltr659_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_ps_liteon_ltr659_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_ps_liteon_ltr659_set_power_mode(&ltr659_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR659";
            info->unit = cm;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_ps_liteon_ltr659_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_ps;
    memset(&sensor_ps, 0, sizeof(sensor_ps));

    if (!g_init_bitwise) {
        ret = drv_ps_liteon_ltr659_validate_id(&ltr659_ctx, LTR659_PART_ID_VAL, LTR659_MANUFAC_ID_VAL);
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
        sensor_ps.open = drv_ps_liteon_ltr659_open;
        sensor_ps.close = drv_ps_liteon_ltr659_close;
        sensor_ps.read = drv_ps_liteon_ltr659_read;
        sensor_ps.write = drv_ps_liteon_ltr659_write;
        sensor_ps.ioctl = drv_ps_liteon_ltr659_ioctl;
        sensor_ps.irq_handle = drv_ps_liteon_ltr659_irq_handle;

        ret = sensor_create_obj(&sensor_ps);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr659_set_default_config(&ltr659_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise = 1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_ps_liteon_ltr659_init);

