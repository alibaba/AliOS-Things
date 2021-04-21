
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "drv_mag_honeywell_qmc5883l.h"
#include "aos/hal/i2c.h"
#include "ulog/ulog.h"

#define QMC5883L_I2C_PORT 1

static int16_t x_max, x_min, z_min, y_max, y_min, z_max;
static uint8_t addr;
static uint8_t mode;
static uint8_t rate;
static uint8_t range;
static uint8_t oversampling;

int qmc5883l_i2c_master_send(const uint8_t *data, uint16_t size, uint32_t timeout) {
    return sensor_i2c_master_send(QMC5883L_I2C_PORT, QMC5883L_ADDR, data, size, timeout);
}

int32_t qmc5883l_i2c_master_recv(uint8_t *data, uint16_t size, uint32_t timeout) {
    return sensor_i2c_master_recv(QMC5883L_I2C_PORT, QMC5883L_ADDR, data, size, timeout);
}

static qmc5883l_write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t write_buffer[2] = {reg, data};
    qmc5883l_i2c_master_send(write_buffer, 2, 1000);
}

uint8_t qmc5883l_read_register(uint8_t addr, uint8_t reg)
{
    uint8_t data;
    qmc5883l_i2c_master_send(&reg, 1, 1000);
    aos_msleep(2);
    qmc5883l_i2c_master_recv(&data, 1, 1000);
    return data;
}

uint8_t qmc5883l_read_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    qmc5883l_i2c_master_send(&reg, 1, 1000);
    aos_msleep(20);
    qmc5883l_i2c_master_recv(buf, len, 1000);
    return 1;
}

static void qmc5883l_reconfig()
{
    qmc5883l_write_register(addr, QMC5883L_CONFIG,
                            oversampling | range | rate | mode);
    qmc5883l_write_register(addr, QMC5883L_CONFIG2, 0x1);
}

static void qmc5883l_reset()
{
    qmc5883l_write_register(addr, QMC5883L_RESET, 0x01);
    qmc5883l_reconfig();
    qmc5883l_resetCalibration();
}

void qmc5883l_setOversampling(int x)
{
    switch (x) {
        case 512:
            oversampling = QMC5883L_CONFIG_OS512;
            break;
        case 256:
            oversampling = QMC5883L_CONFIG_OS256;
            break;
        case 128:
            oversampling = QMC5883L_CONFIG_OS128;
            break;
        case 64:
            oversampling = QMC5883L_CONFIG_OS64;
            break;
    }
    qmc5883l_reconfig();
}

static void qmc5883l_setRange(int x)
{
    switch (x) {
        case 2:
            range = QMC5883L_CONFIG_2GAUSS;
            break;
        case 8:
            range = QMC5883L_CONFIG_8GAUSS;
            break;
    }
    qmc5883l_reconfig();
}

void qmc5883l_setSamplingRate(int x)
{
    switch (x) {
        case 10:
            rate = QMC5883L_CONFIG_10HZ;
            break;
        case 50:
            rate = QMC5883L_CONFIG_50HZ;
            break;
        case 100:
            rate = QMC5883L_CONFIG_100HZ;
            break;
        case 200:
            rate = QMC5883L_CONFIG_200HZ;
            break;
    }
    qmc5883l_reconfig();
}

static void _qmc5883l_init()
{
    /* This assumes the wire library has been initialized. */
    addr         = QMC5883L_ADDR;
    oversampling = QMC5883L_CONFIG_OS512;
    range        = QMC5883L_CONFIG_8GAUSS;
    rate         = QMC5883L_CONFIG_200HZ;
    mode         = QMC5883L_CONFIG_CONT;

    qmc5883l_reset();
}

int qmc5883l_ready()
{
    return qmc5883l_read_register(addr, QMC5883L_STATUS) & QMC5883L_STATUS_DRDY;
}

int qmc5883l_readRaw(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t  data[6];
    uint32_t timeout = 10000;

    while (!qmc5883l_ready() && (timeout--))
        ;

    if (!qmc5883l_read_len(addr, QMC5883L_X_LSB, data, 6))
        return 0;
    *x = data[0] | (data[1] << 8);
    *y = data[2] | (data[3] << 8);
    *z = data[4] | (data[5] << 8);

    return 1;
}

void qmc5883l_resetCalibration()
{
    x_max = y_max = z_max = INT16_MIN;
    x_min = y_min = z_min = INT16_MAX;
}

int qmc5883l_readHeading()
{
    int16_t x_org, y_org, z_org;
    float   x_offset, y_offset, z_offset;
    float   x_fit, y_fit, z_fit;

    if (!qmc5883l_readRaw(&x_org, &y_org, &z_org))
        return 0;

    // printf("org[%d,%d,%d],\n", x_org, y_org, z_org);

    /* Update the observed boundaries of the measurements */
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

    // LOGD("SENSOR", "fix[%f,%f,%f],\n", x_fit, y_fit, z_fit);

    int heading = 180.0 * atan2(x_fit, y_fit) / M_PI;
    heading     = (heading <= 0) ? (heading + 360) : heading;

    return heading;
}

void qmc5883l_init(void)
{

    int32_t ret = sensor_i2c_open(QMC5883L_I2C_PORT, QMC5883L_ADDR, I2C_BUS_BIT_RATES_100K, 0);
    if (ret) {
        LOGE("SENSOR", "sensor i2c open failed, ret:%d\n", ret);
        return;
    }

    _qmc5883l_init();
}

void qmc5883l_deinit(void) {
    int32_t ret = sensor_i2c_close(QMC5883L_I2C_PORT);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
    }
    return;
}