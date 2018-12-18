/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include <aos/kernel.h>
#include "sensor_drv_api.h"
#include "sensor_hal.h"


#define AK099XX_REG_WIA1                 0x00
#define AK099XX_REG_WIA2                 0x01
#define AK099XX_REG_INFO1                0x02
#define AK099XX_REG_INFO2                0x03
#define AK099XX_REG_ST1                  0x10
#define AK099XX_REG_HXL                  0x11
#define AK099XX_REG_HXH                  0x12
#define AK099XX_REG_HYL                  0x13
#define AK099XX_REG_HYH                  0x14
#define AK099XX_REG_HZL                  0x15
#define AK099XX_REG_HZH                  0x16
#define AK099XX_REG_TMPS                 0x17
#define AK099XX_REG_ST2                  0x18
#define AK099XX_REG_CNTL1                0x30
#define AK099XX_REG_CNTL2                0x31
#define AK099XX_REG_CNTL3                0x32

#define AK099XX_FUSE_ASAX                0x60
#define AK099XX_FUSE_ASAY                0x61
#define AK099XX_FUSE_ASAZ                0x62

#define AK099XX_BDATA_SIZE               9

#define AK099XX_MODE_SNG_MEASURE         0x01
#define AK099XX_MODE_CONT_MEASURE_MODE1  0x02
#define AK099XX_MODE_CONT_MEASURE_MODE2  0x04
#define AK099XX_MODE_CONT_MEASURE_MODE3  0x06
#define AK099XX_MODE_CONT_MEASURE_MODE4  0x08
#define AK099XX_MODE_CONT_MEASURE_MODE5  0x0A
#define AK099XX_MODE_CONT_MEASURE_MODE6  0x0C
#define AK099XX_MODE_SELF_TEST           0x10
#define AK099XX_MODE_FUSE_ACCESS         0x1F
#define AK099XX_MODE_POWER_DOWN          0x00

#define AK099XX_SOFT_RESET               0x01

#define AK099XX_SUPPORTED_ODR_POWER_DOWN  0
#define AK099XX_SUPPORTED_ODR_10HZ        10
#define AK099XX_SUPPORTED_ODR_20HZ        20
#define AK099XX_SUPPORTED_ODR_50HZ        50
#define AK099XX_SUPPORTED_ODR_100HZ       100

/* AKM Magnetic sensor overflow bit mask */
#define AK099XX_HOFL_BIT     0x08

#define AK0991X_DRDY_BIT     0x01

#define AK09911_WIA_VAL      0x548
#define AK09912_WIA_VAL      0x448
#define AK09913_WIA_VAL      0x848
#define AK09915_WIA_VAL      0x1048
#define AK09916C_WIA_VAL     0x948
#define AK09916D_WIA_VAL     0xB48
#define AK09918_WIA_VAL      0xC48

#define AK09918_DEVICE_NAME  "AK09918C"

/***************************************************************/
/* AK09918 self test releted */
#define TLIMIT_NO_RST        0x101
#define TLIMIT_NO_RST_READ   0x102

#define TLIMIT_NO_RST_WIA1   0x103
#define TLIMIT_LO_RST_WIA1   0x48
#define TLIMIT_HI_RST_WIA1   0x48
#define TLIMIT_NO_RST_WIA2   0x104
#define TLIMIT_LO_RST_WIA2   0x0c
#define TLIMIT_HI_RST_WIA2   0x0c

#define TLIMIT_NO_CNT_ITRT   3
#define TLIMIT_NO_CNT_CNTL2  0x201
#define TLIMIT_NO_CNT_WAIT   0x202
#define TLIMIT_NO_CNT_READ   0x203
#define TLIMIT_NO_CNT_1ST    0x204
#define TLIMIT_LO_CNT_1ST    1
#define TLIMIT_HI_CNT_1ST    1
#define TLIMIT_NO_CNT_2ND    0x205
#define TLIMIT_LO_CNT_2ND    0
#define TLIMIT_HI_CNT_2ND    0

#define TLIMIT_NO_SNG_CNTL2  0x301
#define TLIMIT_NO_SNG_WAIT   0x302

#define TLIMIT_NO_SNG_ST1    0x303
#define TLIMIT_LO_SNG_ST1    1
#define TLIMIT_HI_SNG_ST1    1

#define TLIMIT_NO_SNG_HX     0x304
#define TLIMIT_LO_SNG_HX     -32751
#define TLIMIT_HI_SNG_HX     32751

#define TLIMIT_NO_SNG_HY     0x306
#define TLIMIT_LO_SNG_HY     -32751
#define TLIMIT_HI_SNG_HY     32751

#define TLIMIT_NO_SNG_HZ     0x308
#define TLIMIT_LO_SNG_HZ     -32751
#define TLIMIT_HI_SNG_HZ     32751

/* Only MASKED bit of ST2 register is evaluated */
#define TLIMIT_NO_SNG_ST2    0x30A
#define TLIMIT_LO_SNG_ST2    0
#define TLIMIT_HI_SNG_ST2    0
#define TLIMIT_ST2_MASK      0x08

#define TLIMIT_NO_SLF_CNTL2  0x30B
#define TLIMIT_NO_SLF_WAIT   0x30C

#define TLIMIT_NO_SLF_ST1    0x30D
#define TLIMIT_LO_SLF_ST1    1
#define TLIMIT_HI_SLF_ST1    1

#define TLIMIT_NO_SLF_RVHX   0x30E
#define TLIMIT_LO_SLF_RVHX   -200
#define TLIMIT_HI_SLF_RVHX   200

#define TLIMIT_NO_SLF_RVHY   0x310
#define TLIMIT_LO_SLF_RVHY   -200
#define TLIMIT_HI_SLF_RVHY   200

#define TLIMIT_NO_SLF_RVHZ   0x312
#define TLIMIT_LO_SLF_RVHZ   -1000
#define TLIMIT_HI_SLF_RVHZ   -200

#define TLIMIT_NO_SLF_ST2    0x314
#define TLIMIT_LO_SLF_ST2    0
#define TLIMIT_HI_SLF_ST2    0

#define AKM_FST_ERRCODE(testno, data) \
    (int32_t)((((uint32_t)testno) << 16) | ((uint16_t)data))

#define AKM_FST(no, data, lo, hi, err) \
    if (drv_mag_akm_ak09918_test_data((no), (data), (lo), (hi), \
                                      (err)) != AKM_SUCCESS) \
    { goto SELFTEST_FAIL; }
/***************************************************************/

#define AKM_SUCCESS           (0)
#define AKM_ERROR             (-1)

#define MEASURE_WAIT_TIME     (20)

#define AK0991X_MAX_RANGE     49120000                        /* 4912uT */
#define AK0991X_MIN_RANGE     -49120000                       /* -4912uT */

#define AK099XX_DEFAULT_MODE  AK099XX_MODE_CONT_MEASURE_MODE1 /* Default 10Hz */

#define AK099XX_MAG_AXIS_ORDER_X  0
#define AK099XX_MAG_AXIS_ORDER_Y  1
#define AK099XX_MAG_AXIS_ORDER_Z  2
#define AK099XX_MAG_AXIS_SIGN_X   1                           /* only 1 or -1 */
#define AK099XX_MAG_AXIS_SIGN_Y   1                           /* only 1 or -1 */
#define AK099XX_MAG_AXIS_SIGN_Z   1                           /* only 1 or -1 */

#define CONVERT_TO_UGAUSS     1500                            /* 0.15uT = 1.5mGauss = 1500uGauss */

#define AKM_DEVICE_NAME       AK09918_DEVICE_NAME

#define AKM_LOG(format, ...)  LOG("%s %s %s " format, SENSOR_STR, \
                                  AKM_DEVICE_NAME, __func__, \
                                  ## __VA_ARGS__)

i2c_dev_t ak09918_dev = {
    .port = 3,
    .config.address_width = 8,
    .config.dev_addr = 0x0C << 1,
    .config.freq = 100,
};

static int16_t drv_mag_akm_ak09918_set_mode(
    i2c_dev_t *dev,
    const uint8_t   mode)
{
    int16_t ret = 0;
    uint8_t i2c_data = mode;

    ret = sensor_i2c_write(dev, AK099XX_REG_CNTL2, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

/* reserve for coordinate mapping */
static void drv_mag_akm_ak09918_convert_coordinate(int16_t data[3])
{
    const int16_t order[3] = {AK099XX_MAG_AXIS_ORDER_X, AK099XX_MAG_AXIS_ORDER_Y, AK099XX_MAG_AXIS_ORDER_Z};
    const int16_t sign[3] = {AK099XX_MAG_AXIS_SIGN_X, AK099XX_MAG_AXIS_SIGN_Y, AK099XX_MAG_AXIS_SIGN_Z}; /* only 1 or -1 */
    int16_t       cvt[3];
    int16_t       i = 0;

    for (i = 0; i < 3; i++) {
        cvt[i] = data[order[i]] * sign[i];
    }

    for (i = 0; i < 3; i++) {
        data[i] = cvt[i];
    }

    return;
}

static int drv_mag_akm_ak09918_soft_reset(i2c_dev_t *dev)
{
    int     ret = 0;
    uint8_t i2c_data = AK099XX_SOFT_RESET;

    ret = sensor_i2c_write(dev, AK099XX_REG_CNTL3, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int16_t drv_mag_akm_ak09918_valid_id(i2c_dev_t *dev)
{
    int      ret = 0;
    uint8_t  i2c_data[2];
    uint16_t id = 0;

    ret = sensor_i2c_read(dev, AK099XX_REG_WIA1, i2c_data, 2, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    id = (uint16_t)(((uint16_t)i2c_data[1] << 8) | i2c_data[0]);

    if (AK09918_WIA_VAL != id) {
        AKM_LOG("invalid device.\n");
        return AKM_ERROR;
    }

    return 0;
}

static int drv_mag_akm_ak09918_open(void)
{
    int16_t   ret = 0;
    i2c_dev_t *dev = &ak09918_dev;

    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_DEFAULT_MODE);
    if (unlikely(ret)) {
        return ret;
    }

    AKM_LOG("successfully \n");
    return ret;
}

static int drv_mag_akm_ak09918_close(void)
{
    int16_t   ret = 0;
    i2c_dev_t *dev = &ak09918_dev;

    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    AKM_LOG("successfully \n");
    return ret;
}

static int drv_mag_akm_ak09918_read(
    void   *buf,
    size_t len)
{
    mag_data_t *mag_data = (mag_data_t *)buf;
    i2c_dev_t  *dev = &ak09918_dev;
    size_t     size;
    uint8_t    i2c_data[AK099XX_BDATA_SIZE];
    int16_t    raw_lsb[3];
    int16_t    ret = 0;
    int16_t    i = 0;

    if (NULL == buf) {
        return AKM_ERROR;
    }

    size = sizeof(mag_data_t);

    if (len < size) {
        return AKM_ERROR;
    }

    /* Get measurement data from AK09918
     * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + TMPS + ST2 = 9bytes */
    ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (i2c_data[8] & AK099XX_HOFL_BIT) {
        AKM_LOG("sensor data overflow.\n");
    }

    for (i = 0; i < 3; i++) {
        raw_lsb[i] = (int16_t)(i2c_data[i*2 + 1] | (((uint16_t)i2c_data[i*2 + 2]) << 8));
    }

    drv_mag_akm_ak09918_convert_coordinate(raw_lsb);

    for (i = 0; i < 3; i++) {
        mag_data->data[i] = (raw_lsb[i] * CONVERT_TO_UGAUSS)/1000;
    }

    mag_data->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_mag_akm_ak09918_write(
    const void *buf,
    size_t     len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int16_t drv_mag_akm_ak09918_set_odr(
    i2c_dev_t *dev,
    const uint32_t  odr)
{
    uint8_t mode = 0;
    int16_t ret;

    if (odr == AK099XX_SUPPORTED_ODR_POWER_DOWN) {
        mode = AK099XX_MODE_POWER_DOWN;
    } else if (odr <= AK099XX_SUPPORTED_ODR_10HZ) {
        mode = AK099XX_MODE_CONT_MEASURE_MODE1;
    } else if (odr <= AK099XX_SUPPORTED_ODR_20HZ) {
        mode = AK099XX_MODE_CONT_MEASURE_MODE2;
    } else if (odr <= AK099XX_SUPPORTED_ODR_50HZ) {
        mode = AK099XX_MODE_CONT_MEASURE_MODE3;
    } else if (odr <= AK099XX_SUPPORTED_ODR_100HZ) {
        mode = AK099XX_MODE_CONT_MEASURE_MODE4;
    } else {
        AKM_LOG("unsupported ODR : %d, set to default mode\n", odr);
        mode = AK099XX_DEFAULT_MODE;
    }

    /* Before set a new mode, the device must be transition to power-down mode.
     * If the device is already powerdown mode, this command is ignored by
     * the set_operation function. */
    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    /* Transition to new mode */
    ret = drv_mag_akm_ak09918_set_mode(dev, mode);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_mag_akm_ak09918_get_info(dev_sensor_info_t *info)
{
    info->vendor = DEV_SENSOR_VENDOR_AKM;
    info->model = AKM_DEVICE_NAME;
    info->unit = mGauss;
    info->range_max = AK0991X_MAX_RANGE;
    info->range_min = AK0991X_MIN_RANGE;

    return;
}

static int16_t drv_mag_akm_ak09918_set_default_config(i2c_dev_t *dev)
{
    int16_t ret = 0;

    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_mag_akm_ak09918_set_odr(dev, AK099XX_SUPPORTED_ODR_10HZ);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}


static int16_t drv_mag_akm_ak09918_test_data(
    uint16_t testno,
    int16_t  testdata,
    int16_t  lolimit,
    int16_t  hilimit,
    int32_t  *err)
{
    //AKM_LOG("DBG: FST 0x%08X\n", AKM_FST_ERRCODE(testno, testdata));
    if ((lolimit <= testdata) && (testdata <= hilimit)) {
        return AKM_SUCCESS;
    } else {
        *err = AKM_FST_ERRCODE(testno, testdata);
        return AKM_ERROR;
    }
}

int16_t drv_mag_akm_ak09918_self_test(
    i2c_dev_t *dev,
    int32_t   *result)
{
    int16_t ret;
    uint8_t i2c_data[AK099XX_BDATA_SIZE];
    int16_t xval_i16, yval_i16, zval_i16;
    int16_t wait_time = 0;
    uint8_t val;
    int8_t  i = 0;

    /* initialize arg */
    *result = 0;

    /**********************************************************************
     * Step 1
     **********************************************************************/

    /* Soft Reset */
    ret = drv_mag_akm_ak09918_soft_reset(dev);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST, ret);
        goto SELFTEST_FAIL;
    }

    /* Wait over 1 ms */
    aos_msleep(2);

    /* Read values. */
    ret = sensor_i2c_read(dev, AK099XX_REG_WIA1, i2c_data, 2, I2C_OP_RETRIES);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST_READ, ret);
        goto SELFTEST_FAIL;
    }

    AKM_FST(TLIMIT_NO_RST_WIA1, i2c_data[0], TLIMIT_LO_RST_WIA1,
            TLIMIT_HI_RST_WIA1, result);
    AKM_FST(TLIMIT_NO_RST_WIA2, i2c_data[1], TLIMIT_LO_RST_WIA2,
            TLIMIT_HI_RST_WIA2, result);

    /**********************************************************************
     * Step 2
     **********************************************************************/

    /* Set to CNT measurement 50Hz pattern. */
    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_CONT_MEASURE_MODE3);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_CNT_CNTL2, ret);
        goto SELFTEST_FAIL;
    }
    aos_msleep(1);

    /* Current test program sets to MODE3, i.e. 50Hz=20msec interval
     * so, wait for double length of measurement time. */
    wait_time = (1000 / AK099XX_SUPPORTED_ODR_50HZ);

    for (i = 0; i < TLIMIT_NO_CNT_ITRT; i++) {
        aos_msleep(wait_time * 2);

        /* Get measurement data from AK09918
         * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + TMPS + ST2 = 9bytes */
        ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                              I2C_OP_RETRIES);
        if (AKM_SUCCESS != ret) {
            *result = AKM_FST_ERRCODE(TLIMIT_NO_CNT_READ, ret);
            goto SELFTEST_FAIL;
        }

        val = i2c_data[0] & AK0991X_DRDY_BIT;

        AKM_FST(TLIMIT_NO_CNT_1ST, val, TLIMIT_LO_CNT_1ST,
                TLIMIT_HI_CNT_1ST, result);

        /* Get measurement data from AK09918
         * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + TMPS + ST2 = 9bytes */
        ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                              I2C_OP_RETRIES);
        if (AKM_SUCCESS != ret) {
            *result = AKM_FST_ERRCODE(TLIMIT_NO_CNT_READ, ret);
            goto SELFTEST_FAIL;
        }

        val = i2c_data[0] & AK0991X_DRDY_BIT;

        AKM_FST(TLIMIT_NO_CNT_2ND, val, TLIMIT_LO_CNT_2ND,
                TLIMIT_HI_CNT_2ND, result);
    }

    /**********************************************************************
     * Step 3
     **********************************************************************/
    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_CNTL2, ret);
        goto SELFTEST_FAIL;
    }
    aos_msleep(1);

    /* Set to SNG measurement pattern. */
    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_SNG_MEASURE);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_CNTL2, ret);
        goto SELFTEST_FAIL;
    }

    /* Wait for single measurement. */
    aos_msleep(MEASURE_WAIT_TIME);

    /* Get measurement data from AK09918
     * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + TMPS + ST2 = 9bytes */
    ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                          I2C_OP_RETRIES);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_WAIT, ret);
        goto SELFTEST_FAIL;
    }

    /* Convert to 16-bit integer value. */
    xval_i16 = (int16_t)(((uint16_t)i2c_data[1]) | ((uint16_t)i2c_data[2] << 8));
    yval_i16 = (int16_t)(((uint16_t)i2c_data[3]) | ((uint16_t)i2c_data[4] << 8));
    zval_i16 = (int16_t)(((uint16_t)i2c_data[5]) | ((uint16_t)i2c_data[6] << 8));

    AKM_FST(TLIMIT_NO_SNG_ST1, i2c_data[0], TLIMIT_LO_SNG_ST1,
            TLIMIT_HI_SNG_ST1, result);
    AKM_FST(TLIMIT_NO_SNG_HX, xval_i16, TLIMIT_LO_SNG_HX,
            TLIMIT_HI_SNG_HX, result);
    AKM_FST(TLIMIT_NO_SNG_HY, yval_i16, TLIMIT_LO_SNG_HY,
            TLIMIT_HI_SNG_HY, result);
    AKM_FST(TLIMIT_NO_SNG_HZ, zval_i16, TLIMIT_LO_SNG_HZ,
            TLIMIT_HI_SNG_HZ, result);
    AKM_FST(TLIMIT_NO_SNG_ST2, (i2c_data[8] & TLIMIT_ST2_MASK),
            TLIMIT_LO_SNG_ST2, TLIMIT_HI_SNG_ST2, result);

    /* Set to self-test mode. */
    ret = drv_mag_akm_ak09918_set_mode(dev, AK099XX_MODE_SELF_TEST);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SLF_CNTL2, ret);
        goto SELFTEST_FAIL;
    }

    /* Wait for self test measurement. */
    aos_msleep(MEASURE_WAIT_TIME);

    /*
     * Get measurement data from AK09918
     * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + TMPS + ST2 = 9bytes */
    ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                          I2C_OP_RETRIES);
    if (AKM_SUCCESS != ret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_WAIT, ret);
        goto SELFTEST_FAIL;
    }

    /* Convert to 16-bit integer value. */
    xval_i16 = (int16_t)(((uint16_t)i2c_data[1]) | ((uint16_t)i2c_data[2] << 8));
    yval_i16 = (int16_t)(((uint16_t)i2c_data[3]) | ((uint16_t)i2c_data[4] << 8));
    zval_i16 = (int16_t)(((uint16_t)i2c_data[5]) | ((uint16_t)i2c_data[6] << 8));

    AKM_FST(TLIMIT_NO_SLF_ST1, i2c_data[0], TLIMIT_LO_SLF_ST1,
            TLIMIT_HI_SLF_ST1, result);
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX,
            TLIMIT_HI_SLF_RVHX, result);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY,
            TLIMIT_HI_SLF_RVHY, result);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ,
            TLIMIT_HI_SLF_RVHZ, result);
    AKM_FST(TLIMIT_NO_SLF_ST2, (i2c_data[8] & TLIMIT_ST2_MASK),
            TLIMIT_LO_SLF_ST2, TLIMIT_HI_SLF_ST2, result);

    AKM_LOG("self test success");
    return AKM_SUCCESS;

SELFTEST_FAIL:
    AKM_LOG("self test failed result = 0x%x", *result);
    return AKM_ERROR;
}

int drv_mag_akm_ak09918_ioctl(
    int           cmd,
    unsigned long arg)
{
    int           ret = 0;
    //int32_t           self_test_result = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *) arg;

    switch (cmd) {
    case SENSOR_IOCTL_ODR_SET:
        ret = drv_mag_akm_ak09918_set_odr(&ak09918_dev, (int)arg);
        if (unlikely(ret)) {
            return AKM_ERROR;
        }

        break;

    case SENSOR_IOCTL_RANGE_SET:
        break;

    case SENSOR_IOCTL_GET_INFO:
        drv_mag_akm_ak09918_get_info(info);
        break;

    case SENSOR_IOCTL_BIST_PROCESS:
        break;

    case SENSOR_IOCTL_WHO_AM_I:
        break;

    case SENSOR_IOCTL_SET_POWER:
        break;

    case SENSOR_IOCTL_GET_SENSOR_LIST:
        break;

    case SENSOR_IOCTL_DTC_CYCLE_SET:
        break;

    /*case SENSOR_IOCTL_SELF_TEST:
        ret = drv_mag_akm_ak09918_self_test(&ak09918_dev, &self_test_result);
        return ret;*/

    default:
        break;
    }

    return ret;
}

int drv_mag_akm_ak09918_init(void)
{
    int16_t      ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_MAG;
    sensor.path = dev_mag_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_mag_akm_ak09918_open;
    sensor.close = drv_mag_akm_ak09918_close;
    sensor.read = drv_mag_akm_ak09918_read;
    sensor.write = drv_mag_akm_ak09918_write;
    sensor.ioctl = drv_mag_akm_ak09918_ioctl;
    sensor.irq_handle = NULL;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_mag_akm_ak09918_valid_id(&ak09918_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_mag_akm_ak09918_soft_reset(&ak09918_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_mag_akm_ak09918_set_default_config(&ak09918_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    AKM_LOG("successfully.\n");
    return (int)ret;
}

SENSOR_DRV_ADD(drv_mag_akm_ak09918_init);

