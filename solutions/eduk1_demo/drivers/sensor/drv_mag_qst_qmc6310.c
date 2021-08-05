/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "drv_mag_qst_qmc6310.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include <math.h>
#include <stdlib.h>

#define QMC6310_I2C_PORT 1
uint8_t chipid = 0;
uint8_t mag_addr;
static int16_t x_max, x_min, z_min, y_max, y_min, z_max;
static uint8_t addr;
static uint8_t mode;
static uint8_t rate;
static uint8_t range;
static uint8_t oversampling;

static unsigned char sensor_mask[4] = {
    0x80,
    0xA0,
    0xB0,
    0xC0
};

static QMC6310_map c_map;

int qmc6310_i2c_master_send(const uint8_t *data, uint16_t size, uint32_t timeout)
{
    return sensor_i2c_master_send(1, mag_addr, data, size, timeout);
}

int32_t qmc6310_i2c_master_recv(uint8_t *data, uint16_t size, uint32_t timeout)
{
    return sensor_i2c_master_recv(1, mag_addr, data, size, timeout);
}

uint8_t qmc6310_read_register(uint8_t addr, uint8_t reg)
{
    uint8_t data;
    qmc6310_i2c_master_send(&reg, 1, 1000);
    aos_msleep(2);
    qmc6310_i2c_master_recv(&data, 1, 1000);
    return data;
}

int qmc6310_read_block(uint8_t addr, uint8_t *data, uint8_t len)
{
#if defined(QST_USE_SW_I2C)
    return qst_sw_readreg(mag_addr, addr, data, len);
#else
    sensor_i2c_mem_read(1, mag_addr, addr, 1, data, len, 100);
    return 1;
#endif
}

int qmc6310_write_reg(uint8_t addr, uint8_t data)
{
#if defined(QST_USE_SW_I2C)
    return qst_sw_writereg(mag_addr, addr, data);
#else
    uint8_t write_buffer[2] = {addr, data};
    qmc6310_i2c_master_send(write_buffer, 2, 1000);
#endif

}

static void qmc6310_set_layout(int layout)
{
    if (layout == 0) {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X] = AXIS_X;
        c_map.map[AXIS_Y] = AXIS_Y;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 1) {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X] = AXIS_Y;
        c_map.map[AXIS_Y] = AXIS_X;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 2) {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X] = AXIS_X;
        c_map.map[AXIS_Y] = AXIS_Y;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 3) {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X] = AXIS_Y;
        c_map.map[AXIS_Y] = AXIS_X;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 4) {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X] = AXIS_X;
        c_map.map[AXIS_Y] = AXIS_Y;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 5) {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X] = AXIS_Y;
        c_map.map[AXIS_Y] = AXIS_X;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 6) {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X] = AXIS_X;
        c_map.map[AXIS_Y] = AXIS_Y;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else if (layout == 7) {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X] = AXIS_Y;
        c_map.map[AXIS_Y] = AXIS_X;
        c_map.map[AXIS_Z] = AXIS_Z;
    } else {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X] = AXIS_X;
        c_map.map[AXIS_Y] = AXIS_Y;
        c_map.map[AXIS_Z] = AXIS_Z;
    }
}

static int qmc6310_get_chipid(void)
{
    int ret = 0;
    ret = qmc6310_read_block(QMC6310_CHIP_ID_REG, &chipid, 1);
    if (ret == 0) {
        LOGD("SENSOR", "change_iic_addr = 0x3c\n");
        mag_addr = QMC6310N_IIC_ADDR;
    } else {
        LOGE("SENSOR", "QMC6310_get_chipid chipid = 0x%x\n", chipid);
        return 1;
    }

    ret = qmc6310_read_block(QMC6310_CHIP_ID_REG, &chipid, 1);
    if (ret == 0) {
        LOGD("SENSOR", "change_iic_addr = 0x1c\n");
        mag_addr = QMC6310U_IIC_ADDR;
    } else {
        LOGD("SENSOR", "QMC6310N_get_chipid chipid = 0x%x,i2c_addr = 0x3c\n", chipid);
        return 1;
    }

    ret = qmc6310_read_block(QMC6310_CHIP_ID_REG, &chipid, 1);
    if (ret == 0) {
        LOGD("SENSOR", "Get_Chip_ID_Failed!\n");
        return 0;
    } else {
        LOGD("SENSOR", "QMC6310U_get_chipid chipid = 0x%x,i2c_addr = 0x1c\n", chipid);
        return 1;
    }
    return 1;
}

uint8_t qmc6310_read_mag_xyz(float *data)
{
    int res;
    unsigned char mag_data[6];
    short hw_d[3] = {0};
    short raw_c[3];
    int t1 = 0;
    unsigned char rdy = 0;

    /* Check status register for data availability */
    while (!(rdy & 0x01) && (t1 < 5)) {
        rdy = QMC6310_STATUS_REG;
        res = qmc6310_read_block(QMC6310_STATUS_REG, &rdy, 1);
        t1++;
    }

    mag_data[0] = QMC6310_DATA_OUT_X_LSB_REG;

    res = qmc6310_read_block(QMC6310_DATA_OUT_X_LSB_REG, mag_data, 6);
    if (res == 0) {
        return 0;
    }

    hw_d[0] = (short)(((mag_data[1]) << 8) | mag_data[0]);
    hw_d[1] = (short)(((mag_data[3]) << 8) | mag_data[2]);
    hw_d[2] = (short)(((mag_data[5]) << 8) | mag_data[4]);
#if 1
    // Unit:mG  1G = 100uT = 1000mG
    // printf("Hx=%d, Hy=%d, Hz=%d\n",hw_d[0],hw_d[1],hw_d[2]);
    raw_c[AXIS_X] = (int)(c_map.sign[AXIS_X] * hw_d[c_map.map[AXIS_X]]);
    raw_c[AXIS_Y] = (int)(c_map.sign[AXIS_Y] * hw_d[c_map.map[AXIS_Y]]);
    raw_c[AXIS_Z] = (int)(c_map.sign[AXIS_Z] * hw_d[c_map.map[AXIS_Z]]);

    data[0] = (float)raw_c[0] / 10.0f;
    data[1] = (float)raw_c[1] / 10.0f;
    data[2] = (float)raw_c[2] / 10.0f;
#endif
    return res;
}

void qmc6310_resetCalibration()
{
    x_max = y_max = z_max = INT16_MIN;
    x_min = y_min = z_min = INT16_MAX;
}

int qmc6310_readHeading()
{
    int16_t x_org, y_org, z_org;
    float   x_offset, y_offset, z_offset;
    float   x_fit, y_fit, z_fit;
    int res;
    unsigned char mag_data[6];
    short hw_d[3] = {0};
    short raw_c[3];
    float data[3];
    int t1 = 0;
    unsigned char rdy = 0;

    /* Check status register for data availability */
    while (!(rdy & 0x01) && (t1 < 5)) {
        rdy = QMC6310_STATUS_REG;
        res = qmc6310_read_block(QMC6310_STATUS_REG, &rdy, 1);
        t1++;
    }

    mag_data[0] = QMC6310_DATA_OUT_X_LSB_REG;

    res = qmc6310_read_block(QMC6310_DATA_OUT_X_LSB_REG, mag_data, 6);
    if (res == 0) {
        return 0;
    }

    x_org = (short)(((mag_data[1]) << 8) | mag_data[0]);
    y_org = (short)(((mag_data[3]) << 8) | mag_data[2]);
    z_org = (short)(((mag_data[5]) << 8) | mag_data[4]);
    LOGD("SENSOR", "org[%d,%d,%d],\n", x_org, y_org, z_org);

    x_min = x_org < x_min ? x_org : x_min;
    x_max = x_org > x_max ? x_org : x_max;
    y_min = y_org < y_min ? y_org : y_min;
    y_max = y_org > y_max ? y_org : y_max;
    z_min = z_org < z_min ? z_org : z_min;
    z_max = z_org > z_max ? z_org : z_max;

    /* Bail out if not enough data is available. */
    if (x_min == x_max || y_min == y_max || z_max == z_min)
        return 0;

    /* Recenter the measurement by subtracting the average */
    x_offset = (x_max + x_min) / 2.0;
    y_offset = (y_max + y_min) / 2.0;
    z_offset = (z_max + z_min) / 2.0;

    x_fit = (x_org - x_offset) * 1000.0 / (x_max - x_min);
    y_fit = (y_org - y_offset) * 1000.0 / (y_max - y_min);
    z_fit = (z_org - z_offset) * 1000.0 / (z_max - z_min);
    // printf("fix[%f,%f,%f],\n", x_fit, y_fit, z_fit);
    LOGD("SENSOR", "fix[%f,%f,%f],\n", x_fit, y_fit, z_fit);

    int heading = 180.0 * atan2(x_fit, y_fit) / M_PI;
    heading     = (heading <= 0) ? (heading + 360) : heading;

    return heading;

}

/* Set the sensor mode */
int qmc6310_set_mode(unsigned char mode)
{
    int err = 0;
    unsigned char ctrl1_value = 0;

    err = qmc6310_read_block(QMC6310_CTL_REG_ONE, &ctrl1_value, 1);
    ctrl1_value = (ctrl1_value & (~0x03)) | mode;
    LOGD("SENSOR", "QMC6310_set_mode, 0x0A = [%02x]->[%02x] \r\n", QMC6310_CTL_REG_ONE, ctrl1_value);
    err = qmc6310_write_reg(QMC6310_CTL_REG_ONE, ctrl1_value);

    return err;
}

int qmc6310_set_output_data_rate(unsigned char rate)
{

    int err = 0;
    unsigned char ctrl1_value = 0;

    err = qmc6310_read_block(QMC6310_CTL_REG_ONE, &ctrl1_value, 1);
    ctrl1_value = (ctrl1_value & (~0xE8)) | (rate << 5);
    LOGD("SENSOR", "QMC6310_set_output_data_rate, 0x0A = [%02x]->[%02x] \r\n", QMC6310_CTL_REG_ONE, ctrl1_value);
    err = qmc6310_write_reg(QMC6310_CTL_REG_ONE, ctrl1_value);

    return err;
}

static int qmc6308_enable(void)
{
    int ret = 0;
    if (chipid == 0x80) {
        qmc6310_write_reg(0x0a, 0xc3);
        qmc6310_write_reg(0x0b, 0x00);
        qmc6310_write_reg(0x0d, 0x40);
    } else {
        qmc6310_write_reg(0x0a, 0x63);
        qmc6310_write_reg(0x0b, 0x08);
        qmc6310_write_reg(0x0d, 0x40);

    }
    return ret;
}

static int qmc6310_enable(void)
{
    int ret = 0;

    qmc6310_write_reg(0x0d, 0x40);
    qmc6310_write_reg(0x29, 0x06);
    qmc6310_write_reg(0x0a, 0x0F); // 0XA9=ODR =100HZ 0XA5 = 50HZ
    qmc6310_write_reg(0x0b, 0x00); // 30 GS

    return 1;
}

int qmc6310_init(void)
{
    mag_addr = QMC6310U_IIC_ADDR;

    int32_t ret = sensor_i2c_open(1, mag_addr, I2C_BUS_BIT_RATES_100K, 0);
    if (ret) {
        LOGE("SENSOR", "sensor i2c open failed, ret:%d\n", ret);
        return;
    }

    ret = qmc6310_get_chipid();
    if (ret == 0) {
        return 0;
    }

    qmc6310_set_layout(0);
    qmc6310_enable();

    {
        unsigned char ctrl_value;
        qmc6310_read_block(QMC6310_CTL_REG_ONE, &ctrl_value, 1);
        LOGD("SENSOR", "QMC6310  0x%x=0x%x \r\n", QMC6310_CTL_REG_ONE, ctrl_value);
        qmc6310_read_block(QMC6310_CTL_REG_TWO, &ctrl_value, 1);
        LOGD("SENSOR", "QMC6310  0x%x=0x%x \r\n", QMC6310_CTL_REG_TWO, ctrl_value);
        qmc6310_read_block(0x0d, &ctrl_value, 1);
        LOGD("SENSOR", "QMC6310  0x%x=0x%x \r\n", 0x0d, ctrl_value);
    }
    return 1;
}

void qmc6310_deinit(void)
{
    int32_t ret = sensor_i2c_close(QMC6310_I2C_PORT);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
    }
    return;
}
