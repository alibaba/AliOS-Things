
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "qmc5883l.h"
#include "aos/hal/i2c.h"

static i2c_dev_t i2c_dev;

static int16_t x_max, x_min, z_min, y_max, y_min, z_max;
static uint8_t addr;
static uint8_t mode;
static uint8_t rate;
static uint8_t range;
static uint8_t oversampling;

static qmc5883l_write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t write_buffer[2] = {reg, data};
    hal_i2c_master_send(&i2c_dev, addr, write_buffer, 2, 1000);
}

uint8_t qmc5883l_read_register(uint8_t addr, uint8_t reg)
{
    uint8_t data;
    hal_i2c_master_send(&i2c_dev, addr, &reg, 1, 1000);
    aos_msleep(2);
    hal_i2c_master_recv(&i2c_dev, addr, &data, 1, 1000);
    return data;
}

uint8_t qmc5883l_read_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    //    hal_i2c_mem_read(&i2c_dev, i2c_dev.config.dev_addr, &reg, 1, buf, len, 1000);
    //    return 0;
    hal_i2c_master_send(&i2c_dev, addr, &reg, 1, 1000);
    aos_msleep(20);
    hal_i2c_master_recv(&i2c_dev, addr, buf, len, 1000);
    return 1;
}

static void qmc5883l_reconfig()
{
    qmc5883l_write_register(addr, QMC5883L_CONFIG, oversampling | range | rate | mode);
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
    switch (x)
    {
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
    switch (x)
    {
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
    switch (x)
    {
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
    addr = QMC5883L_ADDR;
    oversampling = QMC5883L_CONFIG_OS512;
    range = QMC5883L_CONFIG_8GAUSS;
    rate = QMC5883L_CONFIG_200HZ;
    mode = QMC5883L_CONFIG_CONT;

    qmc5883l_reset();
}

int qmc5883l_ready()
{
    return qmc5883l_read_register(addr, QMC5883L_STATUS) & QMC5883L_STATUS_DRDY;
}

int qmc5883l_readRaw(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6];
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
    float x_offset, y_offset, z_offset;
    float x_fit, y_fit, z_fit;

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

    printf("fix[%f,%f,%f],\n", x_fit, y_fit, z_fit);

    int heading = 180.0 * atan2(x_fit, y_fit) / M_PI;
    heading = (heading <= 0) ? (heading + 360) : heading;

    return heading;
}

void qmc5883l_init(void)
{
    i2c_dev.port = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq = I2C_BUS_BIT_RATES_100K;
    i2c_dev.config.mode = I2C_MODE_MASTER;
    i2c_dev.config.dev_addr = QMC5883L_ADDR;

    hal_i2c_init(&i2c_dev);

    _qmc5883l_init();
}