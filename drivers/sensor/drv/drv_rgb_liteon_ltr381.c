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
#define LTR381_ADDR_TRANS(n)                                ((n) << 1)
#define LTR381_GET_BITSLICE(uint8Val, bitName)              (((uint8Val) & (LTR381_##bitName##__MSK)) >> (LTR381_##bitName##__POS))
#define LTR381_SET_BITSLICE(uint8Val, bitName, bitVal)      (((uint8Val) & ~(LTR381_##bitName##__MSK)) | ((bitVal << (LTR381_##bitName##__POS)) & (LTR381_##bitName##__MSK)))

/*******************************************************************************
 **************************   SENSOR SPECIFICATIONS   **************************
 ******************************************************************************/
/* I2C device address */
#define LTR381_SLAVE_ADDR                       (0x53)
#define LTR381_I2C_ADDR                         LTR381_ADDR_TRANS(LTR381_SLAVE_ADDR)

/* Device info */
#define LTR381_PART_ID_VAL                      0xC2
#define LTR381_MANUFAC_ID_VAL                   0x05

#define LTR381_WAIT_TIME_PER_CHECK              (10)
#define LTR381_WAIT_TIME_TOTAL                  (100)

/*******************************************************************************
 *************   Non-Configurable (Unless data sheet is updated)   *************
 ******************************************************************************/
/* Device register set address */
#define LTR381_MAIN_CTRL_REG                    (0x00)
#define LTR381_ALS_CS_MEAS_RATE_REG             (0x04)
#define LTR381_ALS_CS_GAIN_REG                  (0x05)
#define LTR381_PART_ID_REG                      (0x06)
#define LTR381_MAIN_STATUS_REG                  (0x07)
#define LTR381_CS_DATA_IR_0_REG                 (0x0A)
#define LTR381_CS_DATA_IR_1_REG                 (0x0B)
#define LTR381_CS_DATA_IR_2_REG                 (0x0C)
#define LTR381_CS_DATA_GREEN_0_REG              (0x0D)
#define LTR381_CS_DATA_GREEN_1_REG              (0x0E)
#define LTR381_CS_DATA_GREEN_2_REG              (0x0F)
#define LTR381_CS_DATA_RED_0_REG                (0x10)
#define LTR381_CS_DATA_RED_1_REG                (0x11)
#define LTR381_CS_DATA_RED_2_REG                (0x12)
#define LTR381_CS_DATA_BLUE_0_REG               (0x13)
#define LTR381_CS_DATA_BLUE_1_REG               (0x14)
#define LTR381_CS_DATA_BLUE_2_REG               (0x15)
#define LTR381_INT_CFG_REG                      (0x19)
#define LTR381_INT_PST_REG                      (0x1A)
#define LTR381_ALS_THRES_UP_0_REG               (0x21)
#define LTR381_ALS_THRES_UP_1_REG               (0x22)
#define LTR381_ALS_THRES_UP_2_REG               (0x23)
#define LTR381_ALS_THRES_LOW_0_REG              (0x24)
#define LTR381_ALS_THRES_LOW_1_REG              (0x25)
#define LTR381_ALS_THRES_LOW_2_REG              (0x26)

/* Register MAIN_CTRL field */
#define LTR381_ALS_CS_ENABLE__REG               (LTR381_MAIN_CTRL_REG)
#define LTR381_ALS_CS_ENABLE__POS               (1)
#define LTR381_ALS_CS_ENABLE__MSK               (0x02)
#define LTR381_CS_MODE__REG                     (LTR381_MAIN_CTRL_REG)
#define LTR381_CS_MODE__POS                     (2)
#define LTR381_CS_MODE__MSK                     (4)
#define LTR381_SW_RESET__REG                    (LTR381_MAIN_CTRL_REG)
#define LTR381_SW_RESET__POS                    (4)
#define LTR381_SW_RESET__MSK                    (0x10)

/* Register ALS_CS_MEAS_RATE field */
#define LTR381_ALS_CS_MEAS_RATE__REG            (LTR381_ALS_CS_MEAS_RATE_REG)
#define LTR381_ALS_CS_MEAS_RATE__POS            (0)
#define LTR381_ALS_CS_MEAS_RATE__MSK            (0x07)
#define LTR381_ALS_CS_RESOLUTION__REG           (LTR381_ALS_CS_MEAS_RATE_REG)
#define LTR381_ALS_CS_RESOLUTION__POS           (4)
#define LTR381_ALS_CS_RESOLUTION__MSK           (0x70)

/* Register ALS_CS_GAIN field */
#define LTR381_ALS_CS_GAIN_RANGE__REG           (LTR381_ALS_CS_GAIN_REG)
#define LTR381_ALS_CS_GAIN_RANGE__POS           (0)
#define LTR381_ALS_CS_GAIN_RANGE__MSK           (0x07)

/* Register PART_ID field */
#define LTR381_PART_NUMBER_ID__REG              (LTR381_PART_ID_REG)
#define LTR381_PART_NUMBER_ID__POS              (4)
#define LTR381_PART_NUMBER_ID__MSK              (0xF0)

/* Register MAIN_STATUS field */
#define LTR381_ALS_CS_DATA_STATUS__REG          (LTR381_MAIN_STATUS_REG)
#define LTR381_ALS_CS_DATA_STATUS__POS          (3)
#define LTR381_ALS_CS_DATA_STATUS__MSK          (0x08)
#define LTR381_ALS_CS_INT_STATUS__REG           (LTR381_MAIN_STATUS_REG)
#define LTR381_ALS_CS_INT_STATUS__POS           (4)
#define LTR381_ALS_CS_INT_STATUS__MSK           (0x10)
#define LTR381_POWER_ON_STATUS__REG             (LTR381_MAIN_STATUS_REG)
#define LTR381_POWER_ON_STATUS__POS             (5)
#define LTR381_POWER_ON_STATUS__MSK             (0x20)

/* Register INT_CFG field */
#define LTR381_ALS_INT_PIN_EN__REG              (LTR381_INT_CFG_REG)
#define LTR381_ALS_INT_PIN_EN__POS              (2)
#define LTR381_ALS_INT_PIN_EN__MSK              (0x04)
#define LTR381_ALS_INT_SEL__REG                 (LTR381_INT_CFG_REG)
#define LTR381_ALS_INT_SEL__POS                 (4)
#define LTR381_ALS_INT_SEL__MSK                 (0x30)

/* Register INT_PST field */
#define LTR381_ALS_CS_PERSIST__REG              (LTR381_INT_PST_REG)
#define LTR381_ALS_CS_PERSIST__POS              (4)
#define LTR381_ALS_CS_PERSIST__MSK              (0xF0)

/* Field value enumeration */
typedef enum {
    LTR381_ALS_STANDBY_MODE = 0x00,
    LTR381_ALS_ACTIVE_MODE = 0x01,
} LTR381_ALS_CS_ENABLE_VAL;

typedef enum {
    LTR381_ALS_MODE = 0x00,
    LTR381_CS_MODE = 0x01,
} LTR381_ALS_CS_MODE_VAL;

typedef enum {
    LTR381_ALS_NO_RESET = 0x00,
    LTR381_ALS_RESET = 0x01,
} LTR381_SW_RESET_VAL;

typedef enum {
    LTR381_ALS_CS_MEAS_RATE_25MS = 0x00,
    LTR381_ALS_CS_MEAS_RATE_50MS = 0x01,
    LTR381_ALS_CS_MEAS_RATE_100MS = 0x02,
    LTR381_ALS_CS_MEAS_RATE_200MS = 0x03,
    LTR381_ALS_CS_MEAS_RATE_500MS = 0x04,
    LTR381_ALS_CS_MEAS_RATE_1000MS = 0x05,
    LTR381_ALS_CS_MEAS_RATE_2000MS = 0x06,
} LTR381_ALS_CS_MEAS_RATE_VAL;

typedef enum {
    LTR381_ALS_CS_RESOLUTION_20BIT = 0x00,
    LTR381_ALS_CS_RESOLUTION_19BIT = 0x01,
    LTR381_ALS_CS_RESOLUTION_18BIT = 0x02,
    LTR381_ALS_CS_RESOLUTION_17BIT = 0x03,
    LTR381_ALS_CS_RESOLUTION_16BIT = 0x04,
} LTR381_ALS_CS_RESOLUTION_VAL;

typedef enum {
    LTR381_ALS_CS_GAIN_RANGE_1 = 0x00,
    LTR381_ALS_CS_GAIN_RANGE_3 = 0x01,
    LTR381_ALS_CS_GAIN_RANGE_6 = 0x02,
    LTR381_ALS_CS_GAIN_RANGE_9 = 0x03,
    LTR381_ALS_CS_GAIN_RANGE_18 = 0x04,
} LTR381_ALS_CS_GAIN_RANGE_VAL;

typedef enum {
    LTR381_CS_ALS_DATA_OLD = 0x00,
    LTR381_CS_ALS_DATA_NEW = 0x01,
} LTR381_CS_ALS_DATA_STATUS_VAL;

typedef enum {
    LTR381_CS_ALS_INT_FALSE = 0x00,
    LTR381_CS_ALS_INT_TRUE = 0x01,
} LTR381_CS_ALS_INT_STATUS_VAL;

typedef enum {
    LTR381_POWER_ON_NORMAL = 0x00,
    LTR381_POWER_ON_GOING = 0x01,
} LTR381_POWER_ON_STATUS_VAL;

typedef enum {
    LTR381_ALS_INT_INACTIVE = 0x00,
    LTR381_ALS_INT_ACTIVE = 0x01,
} LTR381_ALS_INT_PIN_EN_VAL;

typedef enum {
    LTR381_ALS_INT_IR_CHANNEL = 0x00,
    LTR381_ALS_INT_GREEN_CHANNEL = 0x01,
    LTR381_ALS_INT_BLUE_CHANNEL = 0x02,
    LTR381_ALS_INT_RED_CHANNEL = 0x03,
} LTR381_ALS_INT_SELECT_VAL;


i2c_dev_t ltr381_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR381_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;


static int drv_rgb_liteon_ltr381_validate_id(i2c_dev_t* drv, uint8_t part_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR381_PART_ID_REG, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id) {
        return -1;
    }

    return 0;
}

static int drv_rgb_liteon_ltr381_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR381_MAIN_CTRL_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR381_SET_BITSLICE(value, ALS_CS_ENABLE, LTR381_ALS_STANDBY_MODE);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR381_SET_BITSLICE(value, ALS_CS_ENABLE, LTR381_ALS_ACTIVE_MODE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR381_MAIN_CTRL_REG, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_rgb_liteon_ltr381_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR381_MAIN_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR381_GET_BITSLICE(value, ALS_CS_DATA_STATUS) == LTR381_CS_ALS_DATA_NEW) ? 1 : 0;

    return ret;
}

static int drv_rgb_liteon_ltr381_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    value = LTR381_SET_BITSLICE(value, ALS_CS_ENABLE, LTR381_ALS_STANDBY_MODE);
    value = LTR381_SET_BITSLICE(value, CS_MODE, LTR381_CS_MODE);
    value = LTR381_SET_BITSLICE(value, SW_RESET, LTR381_ALS_NO_RESET);
    ret = sensor_i2c_write(drv, LTR381_MAIN_CTRL_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR381_SET_BITSLICE(value, ALS_CS_MEAS_RATE, LTR381_ALS_CS_MEAS_RATE_100MS);
    value = LTR381_SET_BITSLICE(value, ALS_CS_RESOLUTION, LTR381_ALS_CS_RESOLUTION_18BIT);
    ret = sensor_i2c_write(drv, LTR381_ALS_CS_MEAS_RATE_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR381_SET_BITSLICE(value, ALS_CS_GAIN_RANGE, LTR381_ALS_CS_GAIN_RANGE_3);
    ret = sensor_i2c_write(drv, LTR381_ALS_CS_GAIN_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR381_SET_BITSLICE(value, ALS_INT_PIN_EN, LTR381_ALS_INT_INACTIVE);
    value = LTR381_SET_BITSLICE(value, ALS_INT_SEL, LTR381_ALS_INT_GREEN_CHANNEL);
    ret = sensor_i2c_write(drv, LTR381_INT_CFG_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    ret = sensor_i2c_write(drv, LTR381_INT_PST_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_rgb_liteon_ltr381_irq_handle(void)
{
    /* no handle so far */
}

static int drv_rgb_liteon_ltr381_open(void)
{
    int ret = 0;

    ret = drv_rgb_liteon_ltr381_set_power_mode(&ltr381_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_rgb_liteon_ltr381_close(void)
{
    int ret = 0;

    ret = drv_rgb_liteon_ltr381_set_power_mode(&ltr381_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_rgb_liteon_ltr381_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_ch_red_data[3] = { 0 };
    uint8_t reg_ch_green_data[3] = { 0 };
    uint8_t reg_ch_blue_data[3] = { 0 };
    uint32_t ch_red_data = 0;
    uint32_t ch_green_data = 0;
    uint32_t ch_blue_data = 0;
    rgb_data_t * pdata = (rgb_data_t *) buf;

    if (buf == NULL){
        return -1;
    }

    size = sizeof(rgb_data_t);
    if (len < size){
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_GREEN_0_REG, &reg_ch_green_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_GREEN_1_REG, &reg_ch_green_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_GREEN_2_REG, &reg_ch_green_data[2], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_RED_0_REG, &reg_ch_red_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_RED_1_REG, &reg_ch_red_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_RED_2_REG, &reg_ch_red_data[2], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_BLUE_0_REG, &reg_ch_blue_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_BLUE_1_REG, &reg_ch_blue_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr381_ctx, LTR381_CS_DATA_BLUE_2_REG, &reg_ch_blue_data[2], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    ch_red_data = (((uint32_t) reg_ch_red_data[2] << 16) | ((uint32_t) reg_ch_red_data[1] << 8) | reg_ch_red_data[0]);
    ch_green_data = (((uint32_t) reg_ch_green_data[2] << 16) | ((uint32_t) reg_ch_green_data[1] << 8) | reg_ch_green_data[0]);
    ch_blue_data = (((uint32_t) reg_ch_blue_data[2] << 16) | ((uint32_t) reg_ch_blue_data[1] << 8) | reg_ch_blue_data[0]);
    pdata->data[0] = ch_red_data;
    pdata->data[1] = ch_green_data;
    pdata->data[2] = ch_blue_data;
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_rgb_liteon_ltr381_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_rgb_liteon_ltr381_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_rgb_liteon_ltr381_set_power_mode(&ltr381_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR381";
            info->unit = lux;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_rgb_liteon_ltr381_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_rgb;
    memset(&sensor_rgb, 0, sizeof(sensor_rgb));

    if (!g_init_bitwise) {
        ret = drv_rgb_liteon_ltr381_validate_id(&ltr381_ctx, LTR381_PART_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!g_init_bitwise) {
        /* fill the sensor_rgb obj parameters here */
        sensor_rgb.path = dev_rgb_path;
        sensor_rgb.tag = TAG_DEV_RGB;
        sensor_rgb.io_port = I2C_PORT;
        sensor_rgb.mode = DEV_POLLING;
        sensor_rgb.power = DEV_POWER_OFF;
        sensor_rgb.open = drv_rgb_liteon_ltr381_open;
        sensor_rgb.close = drv_rgb_liteon_ltr381_close;
        sensor_rgb.read = drv_rgb_liteon_ltr381_read;
        sensor_rgb.write = drv_rgb_liteon_ltr381_write;
        sensor_rgb.ioctl = drv_rgb_liteon_ltr381_ioctl;
        sensor_rgb.irq_handle = drv_rgb_liteon_ltr381_irq_handle;

        ret = sensor_create_obj(&sensor_rgb);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_rgb_liteon_ltr381_set_default_config(&ltr381_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_rgb_liteon_ltr381_init);

