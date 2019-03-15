/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <k_api.h>

#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define MMC3680KJ_REG_DATA 0x00
#define MMC3680KJ_REG_XL 0x00
#define MMC3680KJ_REG_XH 0x01
#define MMC3680KJ_REG_YL 0x02
#define MMC3680KJ_REG_YH 0x03
#define MMC3680KJ_REG_ZL 0x04
#define MMC3680KJ_REG_ZH 0x05
#define MMC3680KJ_REG_TEMP 0x06
#define MMC3680KJ_REG_STATUS 0x07
#define MMC3680KJ_REG_CTRL0 0x08
#define MMC3680KJ_REG_CTRL1 0x09
#define MMC3680KJ_REG_CTRL2 0x0a
#define MMC3680KJ_REG_X_THD 0x0b
#define MMC3680KJ_REG_Y_THD 0x0c
#define MMC3680KJ_REG_Z_THD 0x0d
#define MMC3680KJ_REG_SELFTEST 0x0e
#define MMC3680KJ_REG_PASSWORD 0x0f
#define MMC3680KJ_REG_OTPMODE 0x12
#define MMC3680KJ_REG_TESTMODE 0x13
#define MMC3680KJ_REG_SR_PWIDTH 0x20
#define MMC3680KJ_REG_OTP 0x2a
#define MMC3680KJ_REG_PRODUCTID 0x2f

#define MMC3680KJ_CMD_REFILL 0x20
#define MMC3680KJ_CMD_RESET 0x10
#define MMC3680KJ_CMD_SET 0x08
#define MMC3680KJ_CMD_TM_M 0x01
#define MMC3680KJ_CMD_TM_T 0x02
#define MMC3680KJ_CMD_START_MDT 0x04
#define MMC3680KJ_CMD_100HZ 0x00
#define MMC3680KJ_CMD_200HZ 0x01
#define MMC3680KJ_CMD_400HZ 0x02
#define MMC3680KJ_CMD_600HZ 0x03
#define MMC3680KJ_CMD_CM_14HZ 0x01
#define MMC3680KJ_CMD_CM_5HZ 0x02
#define MMC3680KJ_CMD_CM_1HZ 0x04
#define MMC3680KJ_CMD_SW_RST 0x80
#define MMC3680KJ_CMD_PASSWORD 0xe1
#define MMC3680KJ_CMD_OTP_OPER 0x11
#define MMC3680KJ_CMD_OTP_MR 0x80
#define MMC3680KJ_CMD_OTP_ACT 0x80
#define MMC3680KJ_CMD_OTP_NACT 0x00
#define MMC3680KJ_CMD_STSET_OPEN 0x02
#define MMC3680KJ_CMD_STRST_OPEN 0x04
#define MMC3680KJ_CMD_ST_CLOSE 0x00
#define MMC3680KJ_CMD_INT_MD_EN 0x40
#define MMC3680KJ_CMD_INT_MDT_EN 0x20

#define MMC3680KJ_PRODUCT_ID 0x0a
#define MMC3680KJ_OTP_READ_DONE_BIT 0x10
#define MMC3680KJ_PUMP_ON_BIT 0x08
#define MMC3680KJ_MDT_BIT 0x04
#define MMC3680KJ_MEAS_T_DONE_BIT 0x02
#define MMC3680KJ_MEAS_M_DONE_BIT 0x01

#define MMC3680KJ_I2C_SLAVE_ADDR 0x30
#define MMC3680KJ_ADDR_TRANS(n) ((n) << 1)
#define MMC3680KJ_I2C_ADDR MMC3680KJ_ADDR_TRANS(MMC3680KJ_I2C_SLAVE_ADDR)

#define MMC3680KJ_OFFSET 32768
#define MMC3680KJ_SENSITIVITY 1024000
#define MMC3680KJ_T_ZERO -75
#define MMC3680KJ_T_SENSITIVITY 80

#define MMC3680KJ_MAG_DATA_SIZE 6
#define OTP_CONVERT(REG) (((REG) >= 32 ? (32 - (REG)) : (REG)) * 6)

typedef enum
{
    FLAG_INIT_MAG = 0,
    FLAG_INIT_TEMP,
} FLAG_INIT_BIT;

i2c_dev_t mmc3680kj_ctx = {
    .port                 = 4,
    .config.address_width = 7,
    .config.freq          = 100000,
    .config.dev_addr      = MMC3680KJ_I2C_ADDR,
};

static uint8_t g_init_bitwise  = 0;
static int32_t g_otp_matrix[3] = { 1000000, 1000000, 1350000 };

static int mmc3680kj_validate_id(i2c_dev_t *drv, uint8_t product_id)
{
    uint8_t value = 0;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, MMC3680KJ_REG_PRODUCTID, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (value != product_id) {
        return -1;
    }

    return 0;
}

static int mmc3680kj_check_otp(i2c_dev_t *drv)
{
    uint8_t value = 0;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, MMC3680KJ_REG_STATUS, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if ((value & MMC3680KJ_OTP_READ_DONE_BIT) != MMC3680KJ_OTP_READ_DONE_BIT) {
        return -1;
    }

    return 0;
}

static int mmc3680kj_get_comp_matrix(i2c_dev_t *drv)
{
    uint8_t value       = 0;
    uint8_t reg_data[2] = { 0 };
    int     ret         = 0;

    if (drv == NULL) {
        return -1;
    }

    value = MMC3680KJ_CMD_PASSWORD;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_PASSWORD, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = MMC3680KJ_CMD_OTP_OPER;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_OTPMODE, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = MMC3680KJ_CMD_OTP_MR;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_TESTMODE, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = MMC3680KJ_CMD_OTP_ACT;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_CTRL2, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, MMC3680KJ_REG_OTP, reg_data, 2, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = MMC3680KJ_CMD_OTP_NACT;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_CTRL2, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    g_otp_matrix[0] = 1000000;
    g_otp_matrix[1] = OTP_CONVERT(reg_data[0] & 0x3f) * 1000 + 1000000;
    g_otp_matrix[2] =
      (OTP_CONVERT((reg_data[1] & 0x0f) << 2 | (reg_data[0] & 0xc0) >> 6) +
       1000) *
      1350;

    return 0;
}

static int mmc3680kj_set_pulse_width(i2c_dev_t *drv)
{
    uint8_t value = 0;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    value = MMC3680KJ_CMD_OTP_NACT;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_CTRL2, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = MMC3680KJ_CMD_PASSWORD;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_PASSWORD, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, MMC3680KJ_REG_SR_PWIDTH, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value &= 0xe7;

    ret = sensor_i2c_write(drv, MMC3680KJ_REG_SR_PWIDTH, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int mmc3680kj_set(i2c_dev_t *drv)
{
    uint8_t value = 0;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    value = MMC3680KJ_CMD_SET;
    ret   = sensor_i2c_write(drv, MMC3680KJ_REG_CTRL0, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    krhino_task_sleep(krhino_ms_to_ticks(1));

    return 0;
}

static int mmc3680kj_set_output_resolution(i2c_dev_t *drv, uint8_t res)
{
    int ret = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_write(drv, MMC3680KJ_REG_CTRL1, &res, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int mmc3680kj_enable(i2c_dev_t *drv, FLAG_INIT_BIT flag)
{
    uint8_t value = 0;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    if (flag == FLAG_INIT_MAG) {
        value = MMC3680KJ_CMD_TM_M;
        ret   = sensor_i2c_write(drv, MMC3680KJ_REG_CTRL0, &value, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        if (unlikely(ret)) {
            return ret;
        }

        do {
            krhino_task_sleep(krhino_ms_to_ticks(10));
            ret = sensor_i2c_read(drv, MMC3680KJ_REG_STATUS, &value,
                                  I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } while ((value & 0x01) != 0x01);
    } else if (flag == FLAG_INIT_TEMP) {
        value = MMC3680KJ_CMD_TM_T;
        ret   = sensor_i2c_write(drv, MMC3680KJ_REG_CTRL0, &value, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        if (unlikely(ret)) {
            return ret;
        }

        do {
            krhino_task_sleep(krhino_ms_to_ticks(10));
            ret = sensor_i2c_read(drv, MMC3680KJ_REG_STATUS, &value,
                                  I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } while ((value & 0x02) != 0x02);
    } else {
        return -1;
    }

    return 0;
}

static int mmc3680kj_need_set()
{
    static sys_time_t last_time = 0;
    sys_time_t        now_time  = 0;

    now_time = krhino_sys_time_get();
    if (now_time - last_time > 5000) {
        last_time = now_time;
        return 1;
    }

    return 0;
}

static void drv_mag_memsic_mmc3680kj_irq_handle(void)
{
    /* no handle so far */
}

static int drv_mag_memsic_mmc3680kj_open(void)
{
    int ret = 0;

    ret = mmc3680kj_set(&mmc3680kj_ctx);
    if (unlikely(ret)) {
        return ret;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_mag_memsic_mmc3680kj_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_mag_memsic_mmc3680kj_read(void *buf, size_t len)
{
    int         ret = 0;
    size_t      size;
    mag_data_t *pdata                            = (mag_data_t *)buf;
    uint8_t     reg_raw[MMC3680KJ_MAG_DATA_SIZE] = { 0 };
    uint16_t    data_raw[3]                      = { 0 };
    uint32_t    mag_raw[3]                       = { 0 };

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(mag_data_t);
    if (len < size) {
        return -1;
    }

    if (mmc3680kj_need_set()) {
        ret = mmc3680kj_set(&mmc3680kj_ctx);
        if (unlikely(ret)) {
            return ret;
        }
    }

    ret = mmc3680kj_enable(&mmc3680kj_ctx, FLAG_INIT_MAG);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(&mmc3680kj_ctx, MMC3680KJ_REG_DATA, reg_raw,
                          MMC3680KJ_MAG_DATA_SIZE, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    data_raw[0] = (uint16_t)(reg_raw[1] << 8 | reg_raw[0]);
    data_raw[1] = (uint16_t)(reg_raw[3] << 8 | reg_raw[2]);
    data_raw[2] = (uint16_t)(reg_raw[5] << 8 | reg_raw[4]);
    mag_raw[0]  = (uint32_t)(data_raw[0]);
    mag_raw[1]  = (uint32_t)(data_raw[1] - data_raw[2] + MMC3680KJ_OFFSET);
    mag_raw[2]  = (uint32_t)(data_raw[1] + data_raw[2] - MMC3680KJ_OFFSET);

    pdata->data[0] = (int32_t)(mag_raw[0] - MMC3680KJ_OFFSET) *
                     g_otp_matrix[0] / MMC3680KJ_SENSITIVITY;
    pdata->data[1] = (int32_t)(mag_raw[1] - MMC3680KJ_OFFSET) *
                     g_otp_matrix[1] / MMC3680KJ_SENSITIVITY;
    pdata->data[2] = (int32_t)(mag_raw[2] - MMC3680KJ_OFFSET) *
                     g_otp_matrix[2] / MMC3680KJ_SENSITIVITY;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_mag_memsic_mmc3680kj_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_mag_memsic_mmc3680kj_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "MMC3680KJ";
            info->unit              = mGauss;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_mag_memsic_mmc3680kj_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor_mag;

    memset(&sensor_mag, 0, sizeof(sensor_mag));

    if (!g_init_bitwise) {
        ret = mmc3680kj_check_otp(&mmc3680kj_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        ret = mmc3680kj_validate_id(&mmc3680kj_ctx, MMC3680KJ_PRODUCT_ID);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_MAG))) {
        /* fill the sensor_mag obj parameters here */
        sensor_mag.tag        = TAG_DEV_MAG;
        sensor_mag.path       = dev_mag_path;
        sensor_mag.io_port    = I2C_PORT;
        sensor_mag.open       = drv_mag_memsic_mmc3680kj_open;
        sensor_mag.close      = drv_mag_memsic_mmc3680kj_close;
        sensor_mag.read       = drv_mag_memsic_mmc3680kj_read;
        sensor_mag.write      = drv_mag_memsic_mmc3680kj_write;
        sensor_mag.ioctl      = drv_mag_memsic_mmc3680kj_ioctl;
        sensor_mag.irq_handle = drv_mag_memsic_mmc3680kj_irq_handle;

        ret = sensor_create_obj(&sensor_mag);
        if (unlikely(ret)) {
            return -1;
        }

        ret = mmc3680kj_get_comp_matrix(&mmc3680kj_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        ret = mmc3680kj_set_pulse_width(&mmc3680kj_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        ret =
          mmc3680kj_set_output_resolution(&mmc3680kj_ctx, MMC3680KJ_CMD_100HZ);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_MAG;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_temp_memsic_mmc3680kj_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_memsic_mmc3680kj_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_memsic_mmc3680kj_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_memsic_mmc3680kj_read(void *buf, size_t len)
{
    int                 ret = 0;
    size_t              size;
    temperature_data_t *pdata   = (temperature_data_t *)buf;
    uint8_t             reg_raw = 0;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(temperature_data_t);
    if (len < size) {
        return -1;
    }

    ret = mmc3680kj_enable(&mmc3680kj_ctx, FLAG_INIT_TEMP);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(&mmc3680kj_ctx, MMC3680KJ_REG_TEMP, &reg_raw,
                          I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    pdata->t = reg_raw * MMC3680KJ_T_SENSITIVITY / 100 + MMC3680KJ_T_ZERO;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_temp_memsic_mmc3680kj_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_temp_memsic_mmc3680kj_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "MMC3680KJ";
            info->unit              = dCelsius;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_memsic_mmc3680kj_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor_temp;

    memset(&sensor_temp, 0, sizeof(sensor_temp));

    if (!g_init_bitwise) {
        ret = mmc3680kj_check_otp(&mmc3680kj_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        ret = mmc3680kj_validate_id(&mmc3680kj_ctx, MMC3680KJ_PRODUCT_ID);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_TEMP))) {
        /* fill the sensor_temp obj parameters here */
        sensor_temp.tag        = TAG_DEV_TEMP;
        sensor_temp.path       = dev_temp_path;
        sensor_temp.io_port    = I2C_PORT;
        sensor_temp.open       = drv_temp_memsic_mmc3680kj_open;
        sensor_temp.close      = drv_temp_memsic_mmc3680kj_close;
        sensor_temp.read       = drv_temp_memsic_mmc3680kj_read;
        sensor_temp.write      = drv_temp_memsic_mmc3680kj_write;
        sensor_temp.ioctl      = drv_temp_memsic_mmc3680kj_ioctl;
        sensor_temp.irq_handle = drv_temp_memsic_mmc3680kj_irq_handle;

        ret = sensor_create_obj(&sensor_temp);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_TEMP;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_mag_memsic_mmc3680kj_init);

