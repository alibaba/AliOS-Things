#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR706_I2C_SLAVE_ADDR                           0x23

#define LTR706_CLK_ON                                   0x80
#define LTR706_PS_CONTR                                 0x81 /* PS operation mode */
#define LTR706_PS_VCSEL                                 0x82 /* LED pulse freq, current duty, peak current */
#define LTR706_PS_N_PULSES                              0x83 /* PS number of pulses */
#define LTR706_PS_MEAS_RATE                             0x84 /* measurement rate*/
#define LTR706_PART_ID                                  0x86
#define LTR706_MANUFAC_ID                               0x87
#define LTR706_PS_STATUS                                0x90
#define LTR706_PS_DATA_0                                0x91
#define LTR706_PS_DATA_1                                0x92
#define LTR706_INTERRUPT                                0x93
#define LTR706_INTR_PRST                                0x94 /* PS interrupt persist setting */
#define LTR706_PS_THRES_UP_0                            0x95 /* PS interrupt upper threshold, lower byte */
#define LTR706_PS_THRES_UP_1                            0x96 /* PS interrupt upper threshold, upper byte */
#define LTR706_PS_THRES_LOW_0                           0x97 /* PS interrupt lower threshold, lower byte */
#define LTR706_PS_THRES_LOW_1                           0x98 /* PS interrupt lower threshold, upper byte */
#define LTR706_FAULT_DETECTION                          0x9F /* Fault detectino control and status register */
#define LTR706_FD_STATUS                                0xA0 /* Fault detection status */

#define LTR706_ADDR_TRANS(n)                            ((n) << 1)
#define LTR706_I2C_ADDR                                 LTR706_ADDR_TRANS(LTR706_I2C_SLAVE_ADDR)
#define LTR706_PART_ID_VAL                              0x05
#define LTR706_MANUFAC_ID_VAL                           0x05

#define LTR706_CLK_ON_REG_SW_RESET__POS                 (1)
#define LTR706_CLK_ON_REG_SW_RESET__MSK                 (0x02)
#define LTR706_CLK_ON_REG_SW_RESET__REG                 (LTR706_CLK_ON)

#define LTR706_CLK_ON_REG_ON_CLOCK__POS                 (5)
#define LTR706_CLK_ON_REG_ON_CLOCK__MSK                 (0x20)
#define LTR706_CLK_ON_REG_ON_CLOCK__REG                 (LTR706_CLK_ON)

#define LTR706_PS_CONTR_REG_PS_MODE__POS                (1)
#define LTR706_PS_CONTR_REG_PS_MODE__MSK                (0x02)
#define LTR706_PS_CONTR_REG_PS_MODE__REG                (LTR706_PS_CONTR)

#define LTR706_PS_CONTR_REG_FTN_NTF_EN__POS             (5)
#define LTR706_PS_CONTR_REG_FTN_NTF_EN__MSK             (0x20)
#define LTR706_PS_CONTR_REG_FTN_NTF_EN__REG             (LTR706_PS_CONTR)

#define LTR706_PS_CONTR_REG_PS_OFFSET_EN__POS           (7)
#define LTR706_PS_CONTR_REG_PS_OFFSET_EN__MSK           (0x80)
#define LTR706_PS_CONTR_REG_PS_OFFSET_EN__REG           (LTR706_PS_CONTR)

#define LTR706_PS_VCSEL_REG_VCSEL_PEAK_CURR__POS        (0)
#define LTR706_PS_VCSEL_REG_VCSEL_PEAK_CURR__MSK        (0x0F)
#define LTR706_PS_VCSEL_REG_VCSEL_PEAK_CURR__REG        (LTR706_PS_VCSEL)

#define LTR706_PS_N_PULSES_REG_PULSES__POS              (0)
#define LTR706_PS_N_PULSES_REG_PULSES__MSK              (0x3F)
#define LTR706_PS_N_PULSES_REG_PULSES__REG              (LTR706_PS_N_PULSES)

#define LTR706_PS_MEAS_RATE_REG_MEAS_RATE__POS          (0)
#define LTR706_PS_MEAS_RATE_REG_MEAS_RATE__MSK          (0x07)
#define LTR706_PS_MEAS_RATE_REG_MEAS_RATE__REG          (LTR706_PS_MEAS_RATE)

#define LTR706_PS_STATUS_REG_PS_DATA_STATUS__POS        (0)
#define LTR706_PS_STATUS_REG_PS_DATA_STATUS__MSK        (0x01)
#define LTR706_PS_STATUS_REG_PS_DATA_STATUS__REG        (LTR706_PS_STATUS)

#define LTR706_PS_STATUS_REG_PS_INT_STATUS__POS         (1)
#define LTR706_PS_STATUS_REG_PS_INT_STATUS__MSK         (0x02)
#define LTR706_PS_STATUS_REG_PS_INT_STATUS__REG         (LTR706_PS_STATUS)

#define LTR706_INTERRUPT_REG_INT_MODE__POS              (0)
#define LTR706_INTERRUPT_REG_INT_MODE__MSK              (0x03)
#define LTR706_INTERRUPT_REG_INT_MODE__REG              (LTR706_INTERRUPT)

#define LTR706_INTERRUPT_REG_INT_POLARITY__POS          (2)
#define LTR706_INTERRUPT_REG_INT_POLARITY__MSK          (0x04)
#define LTR706_INTERRUPT_REG_INT_POLARITY__REG          (LTR706_INTERRUPT)

#define LTR706_INTERRUPT_REG_NTF__POS                   (6)
#define LTR706_INTERRUPT_REG_NTF__MSK                   (0x40)
#define LTR706_INTERRUPT_REG_NTF__REG                   (LTR706_INTERRUPT)

#define LTR706_INTERRUPT_REG_FTN__POS                   (7)
#define LTR706_INTERRUPT_REG_FTN__MSK                   (0x80)
#define LTR706_INTERRUPT_REG_FTN__REG                   (LTR706_INTERRUPT)

#define LTR706_INTR_PRST_REG_PS_PERSIST__POS            (4)
#define LTR706_INTR_PRST_REG_PS_PERSIST__MSK            (0xF0)
#define LTR706_INTR_PRST_REG_PS_PERSIST__REG            (LTR706_INTR_PRST)

#define LTR706_FAULT_DETECTION_REG_FD_LEVEL__POS        (0)
#define LTR706_FAULT_DETECTION_REG_FD_LEVEL__MSK        (0x01)
#define LTR706_FAULT_DETECTION_REG_FD_LEVEL__REG        (LTR706_FAULT_DETECTION)

#define LTR706_FAULT_DETECTION_REG_FD_SELF_CHECK__POS   (4)
#define LTR706_FAULT_DETECTION_REG_FD_SELF_CHECK__MSK   (0x10)
#define LTR706_FAULT_DETECTION_REG_FD_SELF_CHECK__REG   (LTR706_FAULT_DETECTION)

#define LTR706_FD_STATUS_REG_VCSEL_FAULT__POS           (0)
#define LTR706_FD_STATUS_REG_VCSEL_FAULT__MSK           (0x01)
#define LTR706_FD_STATUS_REG_VCSEL_FAULT__REG           (LTR706_FD_STATUS)

#define LTR706_FD_STATUS_REG_ENB_FAULT__POS             (1)
#define LTR706_FD_STATUS_REG_ENB_FAULT__MSK             (0x02)
#define LTR706_FD_STATUS_REG_ENB_FAULT__REG             (LTR706_FD_STATUS)

#define LTR706_FD_STATUS_REG_ENB_GATE_STUCK_HIGH__POS   (2)
#define LTR706_FD_STATUS_REG_ENB_GATE_STUCK_HIGH__MSK   (0x04)
#define LTR706_FD_STATUS_REG_ENB_GATE_STUCK_HIGH__REG   (LTR706_FD_STATUS)

#define LTR706_FD_STATUS_REG_ENB_GATE_STUCK_LOW__POS    (3)
#define LTR706_FD_STATUS_REG_ENB_GATE_STUCK_LOW__MSK    (0x08)
#define LTR706_FD_STATUS_REG_ENB_GATE_STUCK_LOW__REG    (LTR706_FD_STATUS)

#define LTR706_FD_STATUS_REG_VCSEL_LEAKAGE__POS         (4)
#define LTR706_FD_STATUS_REG_VCSEL_LEAKAGE__MSK         (0x10)
#define LTR706_FD_STATUS_REG_VCSEL_LEAKAGE__REG         (LTR706_FD_STATUS)

#define LTR706_GET_BITSLICE(regvar, bitname)            ((regvar & LTR706_##bitname##__MSK) >> LTR706_##bitname##__POS)
#define LTR706_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR706_##bitname##__MSK) | ((val<<LTR706_##bitname##__POS)&LTR706_##bitname##__MSK))

#define LTR706_WAIT_TIME_PER_CHECK                      (10)
#define LTR706_WAIT_TIME_TOTAL                          (50)

typedef enum {
    LTR706_SW_RESET_FALSE = 0x00,
    LTR706_SW_RESET_TRUE = 0x01, 
} LTR706_CFG_SW_RESET;

typedef enum {
    LTR706_CLOCK_OFF = 0x00,
    LTR706_CLOCK_ON = 0x01, 
} LTR706_CFG_ON_CLOCK;

typedef enum {
    LTR706_PS_STANDBY = 0x00,
    LTR706_PS_ACTIVE = 0x01, 
} LTR706_CFG_PS_MODE;

typedef enum {
    LTR706_FTN_NTF_DISABLE = 0x00,
    LTR706_FTN_NTF_ENABLE = 0x01,
} LTR706_CFG_FTN_NTF_EN;

typedef enum {
    LTR706_PS_OFFSET_DISABLE = 0x00,
    LTR706_PS_OFFSET_ENABLE = 0x01, 
} LTR706_CFG_PS_OFFSET_EN;

typedef enum {
    LTR706_VCSEL_CURRENT_3mA = 0x00,
    LTR706_VCSEL_CURRENT_4mA = 0x01,
    LTR706_VCSEL_CURRENT_5mA = 0x02,
    LTR706_VCSEL_CURRENT_6mA = 0x03,
    LTR706_VCSEL_CURRENT_7mA = 0x04,
    LTR706_VCSEL_CURRENT_8mA = 0x05,
    LTR706_VCSEL_CURRENT_9mA = 0x06,
    LTR706_VCSEL_CURRENT_10mA = 0x07,
    LTR706_VCSEL_CURRENT_11mA = 0x08,
    LTR706_VCSEL_CURRENT_12mA = 0x09,
    LTR706_VCSEL_CURRENT_13mA = 0x0A,
    LTR706_VCSEL_CURRENT_14mA = 0x0B,
    LTR706_VCSEL_CURRENT_15mA = 0x0C,
} LTR706_CFG_VCSEL_PEAK_CURRENT;

typedef enum {
    LTR706_PS_MEAS_RATE_6_125 = 0x00,                   /* PS Measurement Repeat Rate = 6.125ms */
    LTR706_PS_MEAS_RATE_12_5 = 0x01,                    /* PS Measurement Repeat Rate = 12.5ms */
    LTR706_PS_MEAS_RATE_25 = 0x02,                      /* PS Measurement Repeat Rate = 25ms */
    LTR706_PS_MEAS_RATE_50 = 0x03,                      /* PS Measurement Repeat Rate = 50ms */
    LTR706_PS_MEAS_RATE_100 = 0x04,                     /* PS Measurement Repeat Rate = 100ms (default) */
    LTR706_PS_MEAS_RATE_200 = 0x05,                     /* PS Measurement Repeat Rate = 200ms */
    LTR706_PS_MEAS_RATE_400 = 0x06,                     /* PS Measurement Repeat Rate = 400ms */
    LTR706_PS_MEAS_RATE_800 = 0x07,                     /* PS Measurement Repeat Rate = 800ms */
} LTR706_CFG_PS_MEAS_RATE;

typedef enum {
    LTR706_PS_DATA_STATUS_OLD = 0x00,
    LTR706_PS_DATA_STATUS_NEW = 0x01,
} LTR706_CFG_PS_DATA_STATUS;

typedef enum {
    LTR706_PS_INT_STATUS_INACTIVE = 0x00,
    LTR706_PS_INT_STATUS_ACTIVE = 0x01,
} LTR706_CFG_PS_INT_STATUS;

typedef enum {
    LTR706_PS_INT_MODE_DISABLE = 0x00,
    LTR706_PS_INT_MODE_ENABLE = 0x01,
} LTR706_CFG_PS_INT_MODE;

typedef enum {
    LTR706_PS_INT_POLARITY_ACTIVE_LO = 0x00,
    LTR706_PS_INT_POLARITY_ACTIVE_HI = 0x01,
} LTR706_CFG_PS_INT_POLARITY;

typedef enum {
    LTR706_NTF_FALSE = 0x00,
    LTR706_NTF_TRUE = 0x01,
} LTR706_CFG_NTF;

typedef enum {
    LTR706_FTN_FALSE = 0x00,
    LTR706_FTN_TRUE = 0x01,
} LTR706_CFG_FTN;

typedef enum {
    LTR706_FD_LEVEL_2 = 0x00,
    LTR706_FD_LEVEL_1 = 0x01,
} LTR706_CFG_FD_LEVEL;

typedef enum {
    LTR706_FD_SELF_CHECK_DISABLE = 0x00,
    LTR706_FD_SELF_CHECK_ENABLE = 0x01,
} LTR706_CFG_FD_SELF_CHECK;

typedef enum {
    LTR706_VCSEL_FAULT_FALSE = 0x00,
    LTR706_VCSEL_FAULT_TRUE = 0x01,
} LTR706_CFG_VCSEL_FAULT;

typedef enum {
    LTR706_ENB_FAULT_FALSE = 0x00,
    LTR706_ENB_FAULT_TRUE = 0x01,
} LTR706_CFG_ENB_FAULT;

typedef enum {
    LTR706_ENB_GATE_STUCK_HIGH_FALSE = 0x00,
    LTR706_ENB_GATE_STUCK_HIGH_TRUE = 0x01,
} LTR706_CFG_ENB_GATE_STUCK_HIGH;

typedef enum {
    LTR706_ENB_GATE_STUCK_LOW_FALSE = 0x00,
    LTR706_ENB_GATE_STUCK_LOW_TRUE = 0x01,
} LTR706_CFG_ENB_GATE_STUCK_LOW;

typedef enum {
    LTR706_VCSEL_LEAKAGE_FALSE = 0x00,
    LTR706_VCSEL_LEAKAGE_TRUE = 0x01,
} LTR706_CFG_VCSEL_LEAKAGE;

i2c_dev_t ltr706_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR706_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_ps_liteon_ltr706_validate_id(i2c_dev_t* drv, uint8_t part_id, uint8_t manufac_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;
    uint8_t manufac_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR706_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR706_MANUFAC_ID, &manufac_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id || manufac_id_value != manufac_id) {
        return -1;
    }

    return 0;
}

static int drv_ps_liteon_ltr706_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR706_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR706_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR706_PS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR706_SET_BITSLICE(value, PS_CONTR_REG_PS_MODE, LTR706_PS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR706_PS_CONTR, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_ps_liteon_ltr706_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR706_PS_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR706_GET_BITSLICE(value, PS_STATUS_REG_PS_DATA_STATUS) == LTR706_PS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_ps_liteon_ltr706_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR706_CLK_ON, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR706_SET_BITSLICE(value, CLK_ON_REG_ON_CLOCK, LTR706_CLOCK_OFF);
    ret = sensor_i2c_write(drv, LTR706_CLK_ON, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR706_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR706_SET_BITSLICE(value, PS_CONTR_REG_FTN_NTF_EN, LTR706_FTN_NTF_DISABLE);
    value = LTR706_SET_BITSLICE(value, PS_CONTR_REG_PS_OFFSET_EN, LTR706_PS_OFFSET_DISABLE);
    ret = sensor_i2c_write(drv, LTR706_PS_CONTR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, LTR706_PS_VCSEL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR706_SET_BITSLICE(value, PS_VCSEL_REG_VCSEL_PEAK_CURR, LTR706_VCSEL_CURRENT_5mA);
    ret = sensor_i2c_write(drv, LTR706_PS_VCSEL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR706_SET_BITSLICE(value, PS_N_PULSES_REG_PULSES, 0x20);
    ret = sensor_i2c_write(drv, LTR706_PS_N_PULSES, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR706_SET_BITSLICE(value, PS_MEAS_RATE_REG_MEAS_RATE, LTR706_PS_MEAS_RATE_100);
    ret = sensor_i2c_write(drv, LTR706_PS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR706_SET_BITSLICE(value, INTERRUPT_REG_INT_MODE, LTR706_PS_INT_MODE_DISABLE);
    value = LTR706_SET_BITSLICE(value, INTERRUPT_REG_INT_POLARITY, LTR706_PS_INT_POLARITY_ACTIVE_LO);
    value = LTR706_SET_BITSLICE(value, INTERRUPT_REG_NTF, LTR706_NTF_FALSE);
    value = LTR706_SET_BITSLICE(value, INTERRUPT_REG_FTN, LTR706_FTN_FALSE);
    ret = sensor_i2c_write(drv, LTR706_INTERRUPT, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR706_SET_BITSLICE(value, FAULT_DETECTION_REG_FD_LEVEL, LTR706_FD_LEVEL_1);
    value = LTR706_SET_BITSLICE(value, FAULT_DETECTION_REG_FD_SELF_CHECK, LTR706_FD_SELF_CHECK_DISABLE);
    ret = sensor_i2c_write(drv, LTR706_FAULT_DETECTION, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_ps_liteon_ltr706_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ps_liteon_ltr706_open(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr706_set_power_mode(&ltr706_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_ps_liteon_ltr706_close(void)
{
    int ret = 0;

    ret = drv_ps_liteon_ltr706_set_power_mode(&ltr706_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_ps_liteon_ltr706_read(void *buf, size_t len)
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

    ret = sensor_i2c_read(&ltr706_ctx, LTR706_PS_DATA_0, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr706_ctx, LTR706_PS_DATA_1, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->present = ((((uint32_t) (reg_data[1] & 0x07)) << 8) | reg_data[0]);
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_ps_liteon_ltr706_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_ps_liteon_ltr706_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_ps_liteon_ltr706_set_power_mode(&ltr706_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR706";
            info->unit = cm;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_ps_liteon_ltr706_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_ps;
    memset(&sensor_ps, 0, sizeof(sensor_ps));

    if (!g_init_bitwise) {
        ret = drv_ps_liteon_ltr706_validate_id(&ltr706_ctx, LTR706_PART_ID_VAL, LTR706_MANUFAC_ID_VAL);
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
        sensor_ps.open = drv_ps_liteon_ltr706_open;
        sensor_ps.close = drv_ps_liteon_ltr706_close;
        sensor_ps.read = drv_ps_liteon_ltr706_read;
        sensor_ps.write = drv_ps_liteon_ltr706_write;
        sensor_ps.ioctl = drv_ps_liteon_ltr706_ioctl;
        sensor_ps.irq_handle = drv_ps_liteon_ltr706_irq_handle;

        ret = sensor_create_obj(&sensor_ps);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_ps_liteon_ltr706_set_default_config(&ltr706_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise = 1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_ps_liteon_ltr706_init);

