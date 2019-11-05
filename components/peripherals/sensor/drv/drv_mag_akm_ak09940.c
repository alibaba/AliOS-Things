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
#define AK099XX_REG_RSV1                 0x02
#define AK099XX_REG_RSV2                 0x03
#define AK099XX_REG_ST1                  0x10
#define AK099XX_REG_HXL                  0x11
#define AK099XX_REG_HXM                  0x12
#define AK099XX_REG_HXH                  0x13
#define AK099XX_REG_HYL                  0x14
#define AK099XX_REG_HYM                  0x15
#define AK099XX_REG_HYH                  0x16
#define AK099XX_REG_HZL                  0x17
#define AK099XX_REG_HZM                  0x18
#define AK099XX_REG_HZH                  0x19
#define AK099XX_REG_TMPS                 0x1A
#define AK099XX_REG_ST2                  0x1B
#define AK099XX_REG_CNTL1                0x30
#define AK099XX_REG_CNTL2                0x31
#define AK099XX_REG_CNTL3                0x32
#define AK099XX_REG_CNTL4                0x33
#define AK099XX_REG_I2C_DIS              0x36
#define AK099XX_REG_TS                   0x37

#define AK099XX_BDATA_SIZE               12

#define AK099XX_TEM_ENABLE               0x40

#define AK099XX_MODE_POWER_DOWN          0x00
#define AK099XX_MODE_SNG_MEASURE         0x01
#define AK099XX_MODE_CONT_MEASURE_MODE1  0x02
#define AK099XX_MODE_CONT_MEASURE_MODE2  0x04
#define AK099XX_MODE_CONT_MEASURE_MODE3  0x06
#define AK099XX_MODE_CONT_MEASURE_MODE4  0x08
#define AK099XX_MODE_CONT_MEASURE_MODE5  0x0A
#define AK099XX_MODE_CONT_MEASURE_MODE6  0x0C
#define AK099XX_MODE_SELF_TEST           0x10
#define AK099XX_MODE_FIFO_ENABLE         0x80

#define AK099XX_LOW_POWER_DRIVE_1        0x00
#define AK099XX_LOW_POWER_DRIVE_2        0x20
#define AK099XX_LOW_NOISE_DRIVE_1        0x40
#define AK099XX_LOW_NOISE_DRIVE_2        0x60

#define AK099XX_SOFT_RESET               0x01

#define AK099XX_I2C_DIS                  0x1D

#define AK099XX_DRDY_BIT                 0x01

#define AK099XX_VAL_OVERFLOW             0x1FFFF

/* 
 * Refer to spec 11.3.4.
 * HXH, HYH, HZH only use 2 bits (include sign bit);
 * In order to set the sign bit with using 32 bits integer,
 * need to shift each byte 14 bits left to compose and shift back the composed value
 */
#define AK09940_SIGN_SHIFT               14
#define AK09940_H_SHIFT                  (16 + AK09940_SIGN_SHIFT)
#define AK09940_M_SHIFT                  (8 + AK09940_SIGN_SHIFT)
#define AK09940_L_SHIFT                  (AK09940_SIGN_SHIFT)

/* DOR bit in ST2 */
#define AK099XX_DOR_BIT                  0x01
/* Invalid fifo data bit in ST2 */
#define AK099XX_INV_FIFO_DATA            0x02

#define AK099XX_SUPPORTED_ODR_POWER_DOWN 0
#define AK099XX_SUPPORTED_ODR_10HZ       10
#define AK099XX_SUPPORTED_ODR_20HZ       20
#define AK099XX_SUPPORTED_ODR_50HZ       50
#define AK099XX_SUPPORTED_ODR_100HZ      100
#define AK099XX_SUPPORTED_ODR_200HZ      200
#define AK099XX_SUPPORTED_ODR_400HZ      400

#define AK09940_WIA_VAL                  0xA148

#define AK099XX_DEVICE_NAME              "AK09940"

/***************************************************************/
/* AK09940 self test releted */
#define TLIMIT_NO_RST        0x101
#define TLIMIT_NO_RST_READ   0x102

#define TLIMIT_NO_RST_WIA1   0x103
#define TLIMIT_LO_RST_WIA1   0x48
#define TLIMIT_HI_RST_WIA1   0x48
#define TLIMIT_NO_RST_WIA2   0x104
#define TLIMIT_LO_RST_WIA2   0xA1
#define TLIMIT_HI_RST_WIA2   0xA1

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
#define TLIMIT_CNT_PWD       0x206

#define TLIMIT_NO_SNG_CNTL2  0x301
#define TLIMIT_NO_SNG_WAIT   0x302

#define TLIMIT_NO_SNG_ST1    0x303
#define TLIMIT_LO_SNG_ST1    1
#define TLIMIT_HI_SNG_ST1    1

#define TLIMIT_NO_SNG_HX     0x304
#define TLIMIT_LO_SNG_HX     -131072
#define TLIMIT_HI_SNG_HX     131070

#define TLIMIT_NO_SNG_HY     0x306
#define TLIMIT_LO_SNG_HY     -131072
#define TLIMIT_HI_SNG_HY     131070

#define TLIMIT_NO_SNG_HZ     0x308
#define TLIMIT_LO_SNG_HZ     -131072
#define TLIMIT_HI_SNG_HZ     131070

/* Only MASKED bit of ST2 register is evaluated */
#define TLIMIT_NO_SNG_ST2    0x30A
#define TLIMIT_LO_SNG_ST2    0
#define TLIMIT_HI_SNG_ST2    0
#define TLIMIT_ST2_MASK      0x03

#define TLIMIT_NO_SLF_CNTL2  0x30B
#define TLIMIT_NO_SLF_WAIT   0x30C

#define TLIMIT_NO_SLF_ST1    0x30D
#define TLIMIT_LO_SLF_ST1    1
#define TLIMIT_HI_SLF_ST1    1

#define TLIMIT_NO_SLF_RVHX   0x30E
#define TLIMIT_LO_SLF_RVHX   -30000 //TBD
#define TLIMIT_HI_SLF_RVHX   30000  //TBD

#define TLIMIT_NO_SLF_RVHY   0x310
#define TLIMIT_LO_SLF_RVHY   -30000 //TBD
#define TLIMIT_HI_SLF_RVHY   30000  //TBD

#define TLIMIT_NO_SLF_RVHZ   0x312
#define TLIMIT_LO_SLF_RVHZ   -150000 //TBD
#define TLIMIT_HI_SLF_RVHZ   30000  //TBD

#define TLIMIT_NO_SLF_ST2    0x314
#define TLIMIT_LO_SLF_ST2    0
#define TLIMIT_HI_SLF_ST2    0

#define AKM_FST(no, data, lo, hi, err) \
    if (drv_mag_akm_ak09940_test_data((no), (data), (lo), (hi), \
                                      (err)) != AKM_SUCCESS) \
    { goto SELFTEST_FAIL; }
/***************************************************************/

#define AKM_SUCCESS           (0)
#define AKM_ERROR             (-1)

#define MEASURE_WAIT_TIME     (20)

#define AK09940_MAX_RANGE     12000000                        /* 12000000uG = 12000mG = 1200uT */
#define AK09940_MIN_RANGE     -12000000                       /* -12000000uG = -12000mG = -1200uT */

#define AK099XX_DEFAULT_MODE  (AK099XX_MODE_CONT_MEASURE_MODE1 | AK099XX_LOW_NOISE_DRIVE_2)     /* Default 10Hz and low noise drive 2 */

#define AK099XX_MAG_AXIS_ORDER_X  0
#define AK099XX_MAG_AXIS_ORDER_Y  1
#define AK099XX_MAG_AXIS_ORDER_Z  2
#define AK099XX_MAG_AXIS_SIGN_X   1                           /* only 1 or -1 */
#define AK099XX_MAG_AXIS_SIGN_Y   1                           /* only 1 or -1 */
#define AK099XX_MAG_AXIS_SIGN_Z   1                           /* only 1 or -1 */

#define CONVERT_TO_UGAUSS         100                        /* 10nT = 0.01uT = 0.1mGauss = 100uGauss */

#define AKM_DEVICE_NAME       AK099XX_DEVICE_NAME

#define AKM_LOG(format, ...)  LOG("%s %s %s " format, SENSOR_STR, \
                                  AKM_DEVICE_NAME, __func__, \
                                  ## __VA_ARGS__)

i2c_dev_t ak09940_dev = {
    .port = 3,
    .config.address_width = 8,
    .config.dev_addr = 0x0C << 1,
    .config.freq = 100,
};

static int16_t drv_mag_akm_ak09940_set_wm(
    i2c_dev_t *dev,
    uint8_t   wm)
{
    int16_t ret;

    // Set water mark
    ret = sensor_i2c_write(dev, AK099XX_REG_CNTL2, &wm, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int drv_mag_akm_ak09940_set_mode(
    i2c_dev_t *dev,
    const uint8_t   mode)
{
    int ret = 0;
    uint8_t wm = 0x00;
    uint8_t temp_enable = AK099XX_TEM_ENABLE;
    uint8_t i2c_data = mode;

    // FIFO function will be supported in the future. 
    drv_mag_akm_ak09940_set_wm(dev, wm);

    // Enable temperature measurement
    ret = sensor_i2c_write(dev, AK099XX_REG_CNTL2, &temp_enable, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_write(dev, AK099XX_REG_CNTL3, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

/* reserve for coordinate mapping */
static void drv_mag_akm_ak09940_convert_coordinate(int32_t data[3])
{
    const int32_t order[3] = {AK099XX_MAG_AXIS_ORDER_X, AK099XX_MAG_AXIS_ORDER_Y, AK099XX_MAG_AXIS_ORDER_Z};
    const int32_t sign[3] = {AK099XX_MAG_AXIS_SIGN_X, AK099XX_MAG_AXIS_SIGN_Y, AK099XX_MAG_AXIS_SIGN_Z}; /* only 1 or -1 */
    int32_t       cvt[3];
    int16_t       i = 0;

    for (i = 0; i < 3; i++) {
        cvt[i] = data[order[i]] * sign[i];
    }

    for (i = 0; i < 3; i++) {
        data[i] = cvt[i];
    }

    return;
}

static int drv_mag_akm_ak09940_soft_reset(i2c_dev_t *dev)
{
    int     ret = 0;
    uint8_t i2c_data = AK099XX_SOFT_RESET;

    ret = sensor_i2c_write(dev, AK099XX_REG_CNTL4, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int16_t drv_mag_akm_ak09940_valid_id(i2c_dev_t *dev)
{
    int      ret = 0;
    uint8_t  i2c_data[2];
    uint16_t id = 0;

    ret = sensor_i2c_read(dev, AK099XX_REG_WIA1, i2c_data, 2, I2C_OP_RETRIES);

    if (unlikely(ret)) {
        return ret;
    }

    id = (uint16_t)(((uint16_t)i2c_data[1] << 8) | i2c_data[0]);

    if (AK09940_WIA_VAL != id) {
        AKM_LOG("invalid device.\n");
        return AKM_ERROR;
    }

    return 0;
}

static int drv_mag_akm_ak09940_open(void)
{
    int16_t   ret = 0;
    i2c_dev_t *dev = &ak09940_dev;

    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_DEFAULT_MODE);
    if (unlikely(ret)) {
        return ret;
    }

    AKM_LOG("successfully \n");
    return ret;
}

static int drv_mag_akm_ak09940_close(void)
{
    int16_t   ret = 0;
    i2c_dev_t *dev = &ak09940_dev;

    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    AKM_LOG("successfully \n");
    return ret;
}

static int drv_mag_akm_ak09940_is_overflow(int32_t data[])
{
    int ret = 0;

    if (AK099XX_VAL_OVERFLOW == data[0] &&
        AK099XX_VAL_OVERFLOW == data[1] &&
        AK099XX_VAL_OVERFLOW == data[2]) {
        ret = 1;
    }

    return ret;
}

static int32_t drv_mag_akm_ak09940_byts_compose(uint8_t l, uint8_t m, uint8_t h)
{
    int32_t val = 0;
    int32_t temp_l, temp_m, temp_h;

    temp_l = ((uint32_t)l) << AK09940_L_SHIFT;
    temp_m = ((uint32_t)m) << AK09940_M_SHIFT;
    temp_h = ((uint32_t)h) << AK09940_H_SHIFT;
    val = ((int32_t)(temp_l | temp_m | temp_h)) >> AK09940_SIGN_SHIFT;

    return val;
}

static int drv_mag_akm_ak09940_read(
    void   *buf,
    size_t len)
{
    mag_data_t *mag_data = (mag_data_t *)buf;
    i2c_dev_t  *dev = &ak09940_dev;
    size_t     size;
    uint8_t    i2c_data[AK099XX_BDATA_SIZE];
    int32_t    raw_lsb[3];
    int        ret = 0;
    int16_t    i = 0;

    if (NULL == buf) {
        return AKM_ERROR;
    }

    size = sizeof(mag_data_t);

    if (len < size) {
        return AKM_ERROR;
    }

    /* Get measurement data from AK09940
     * ST1 + (HXL + HXM + HXH) + (HYL + HYM + HYH) + (HZL + HZM + HZH) + TMPS + ST2 = 12bytes */
    ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    for (i = 0; i < 3; i++) {
        raw_lsb[i] = drv_mag_akm_ak09940_byts_compose(i2c_data[i*3 + 1], i2c_data[i*3 + 2], i2c_data[i*3 + 3]);
    }

    if (drv_mag_akm_ak09940_is_overflow(raw_lsb)) {
        AKM_LOG("sensor data overflow.\n");
    }

    drv_mag_akm_ak09940_convert_coordinate(raw_lsb);

    for (i = 0; i < 3; i++) {
        mag_data->data[i] = (raw_lsb[i] * CONVERT_TO_UGAUSS)/1000;
    }

    mag_data->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_mag_akm_ak09940_write(
    const void *buf,
    size_t     len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_mag_akm_ak09940_set_odr(
    i2c_dev_t *dev,
    const uint32_t  odr)
{
    uint8_t mode = 0;
    int ret;

    if (odr == AK099XX_SUPPORTED_ODR_POWER_DOWN) {
        mode = AK099XX_MODE_POWER_DOWN;
    } else if (odr <= AK099XX_SUPPORTED_ODR_10HZ) {
        mode = (AK099XX_MODE_CONT_MEASURE_MODE1 | AK099XX_LOW_NOISE_DRIVE_2);
    }  else if (odr <= AK099XX_SUPPORTED_ODR_20HZ) {
        mode = (AK099XX_MODE_CONT_MEASURE_MODE2 | AK099XX_LOW_NOISE_DRIVE_2);
    } else if (odr <= AK099XX_SUPPORTED_ODR_50HZ) {
        mode = (AK099XX_MODE_CONT_MEASURE_MODE3 | AK099XX_LOW_NOISE_DRIVE_2);
    } else if (odr <= AK099XX_SUPPORTED_ODR_100HZ) {
        mode = (AK099XX_MODE_CONT_MEASURE_MODE4 | AK099XX_LOW_NOISE_DRIVE_2);
    } else if (odr <= AK099XX_SUPPORTED_ODR_200HZ) {
        mode = (AK099XX_MODE_CONT_MEASURE_MODE5 | AK099XX_LOW_NOISE_DRIVE_2);
    } else if (odr <= AK099XX_SUPPORTED_ODR_400HZ) {
        mode = (AK099XX_MODE_CONT_MEASURE_MODE5 | AK099XX_LOW_POWER_DRIVE_2);
    }  else {
        AKM_LOG("unsupported ODR : %d, set to default mode\n", odr);
        mode = AK099XX_DEFAULT_MODE;
    }

    /* Before set a new mode, the device must be transition to power-down mode.
     * If the device is already powerdown mode, this command is ignored by
     * the set_operation function. */
    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    /* Transition to new mode */
    ret = drv_mag_akm_ak09940_set_mode(dev, mode);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_mag_akm_ak09940_get_info(dev_sensor_info_t *info)
{
    info->vendor = DEV_SENSOR_VENDOR_AKM;
    info->model = AKM_DEVICE_NAME;
    info->unit = mGauss;
    info->range_max = AK09940_MAX_RANGE;
    info->range_min = AK09940_MIN_RANGE;

    return;
}

static int16_t drv_mag_akm_ak09940_set_default_config(i2c_dev_t *dev)
{
    int16_t ret = 0;

    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_mag_akm_ak09940_set_odr(dev, AK099XX_SUPPORTED_ODR_10HZ);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int16_t drv_mag_akm_ak09940_test_data(
    uint16_t testno,
    int32_t  testdata,
    int32_t  lolimit,
    int32_t  hilimit,
    int32_t  err[])
{
    //AKM_LOG("DBG: FST 0x%08X\n", AKM_FST_ERRCODE(testno, testdata));
    if ((lolimit <= testdata) && (testdata <= hilimit)) {
        return AKM_SUCCESS;
    } else {
        err[0] = testno;
        err[1] = testdata;
        return AKM_ERROR;
    }
}

int16_t drv_mag_akm_ak09940_self_test(
    i2c_dev_t *dev,
    int32_t   result[])
{
    int16_t ret;
    uint8_t i2c_data[AK099XX_BDATA_SIZE];
    int32_t xval_i32, yval_i32, zval_i32;
    int16_t wait_time = 0;
    uint8_t val;
    int8_t  i = 0;

    /* initialize arg */
    result[0] = 0;
    result[1] = 0;

    /**********************************************************************
     * Step 1
     **********************************************************************/

    /* Soft Reset */
    ret = drv_mag_akm_ak09940_soft_reset(dev);
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_NO_RST;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }

    /* Wait over 1 ms */
    aos_msleep(2);

    /* Read values. */
    ret = sensor_i2c_read(dev, AK099XX_REG_WIA1, i2c_data, 2, I2C_OP_RETRIES);
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_NO_RST_READ;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }

    AKM_FST(TLIMIT_NO_RST_WIA1, (int32_t)i2c_data[0], TLIMIT_LO_RST_WIA1,
            TLIMIT_HI_RST_WIA1, result);
    AKM_FST(TLIMIT_NO_RST_WIA2, (int32_t)i2c_data[1], TLIMIT_LO_RST_WIA2,
            TLIMIT_HI_RST_WIA2, result);

    /**********************************************************************
     * Step 2
     **********************************************************************/

    /* Set to CNT measurement 50Hz pattern. */
    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_MODE_CONT_MEASURE_MODE3);
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_NO_CNT_CNTL2;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }
    /* Wait 1ms for register setting write done. */
    aos_msleep(1);

    /* Current test program sets to MODE3, i.e. 50Hz=20msec interval
     * so, wait for double length of measurement time. */
    wait_time = (1000 / AK099XX_SUPPORTED_ODR_50HZ);

    for (i = 0; i < TLIMIT_NO_CNT_ITRT; i++) {
        aos_msleep(wait_time * 2);

        /* Get measurement data from AK09940
         * ST1 + (HXL + HXM + HXH) + (HYL + HYM + HYH) + (HZL + HZM + HZH) + TMPS + ST2 = 12bytes */
        ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                              I2C_OP_RETRIES);
        if (AKM_SUCCESS != ret) {
            result[0] = TLIMIT_NO_CNT_READ;
            result[1] = ret;
            goto SELFTEST_FAIL;
        }

        val = i2c_data[0] & AK099XX_DRDY_BIT;

        AKM_FST(TLIMIT_NO_CNT_1ST, (int32_t)val, TLIMIT_LO_CNT_1ST,
                TLIMIT_HI_CNT_1ST, result);

        /* Get measurement data from AK09940
         * ST1 + (HXL + HXM + HXH) + (HYL + HYM + HYH) + (HZL + HZM + HZH) + TMPS + ST2 = 12bytes */
        ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                              I2C_OP_RETRIES);
        if (AKM_SUCCESS != ret) {
            result[0] = TLIMIT_NO_CNT_READ;
            result[1] = ret;
            goto SELFTEST_FAIL;
        }

        val = i2c_data[0] & AK099XX_DRDY_BIT;

        AKM_FST(TLIMIT_NO_CNT_2ND, (int32_t)val, TLIMIT_LO_CNT_2ND,
                TLIMIT_HI_CNT_2ND, result);
    }

    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_MODE_POWER_DOWN);
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_CNT_PWD;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }
    /* Wait 1ms for register setting write done. */
    aos_msleep(1);

    /**********************************************************************
     * Step 3
     **********************************************************************/

    /* Set to SNG measurement pattern. */
    ret = drv_mag_akm_ak09940_set_mode(dev, AK099XX_MODE_SNG_MEASURE);
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_NO_SNG_CNTL2;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }

    /* Wait for single measurement. */
    aos_msleep(MEASURE_WAIT_TIME);

    /* Get measurement data from AK09940
     * ST1 + (HXL + HXM + HXH) + (HYL + HYM + HYH) + (HZL + HZM + HZH) + TMPS + ST2 = 12bytes */
    ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                          I2C_OP_RETRIES);
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_NO_SNG_WAIT;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }

    /* Convert to 32-bit integer value. */
    xval_i32 = drv_mag_akm_ak09940_byts_compose(i2c_data[1], i2c_data[2], i2c_data[3]);
    yval_i32 = drv_mag_akm_ak09940_byts_compose(i2c_data[4], i2c_data[5], i2c_data[6]);
    zval_i32 = drv_mag_akm_ak09940_byts_compose(i2c_data[7], i2c_data[8], i2c_data[9]);

    AKM_FST(TLIMIT_NO_SNG_ST1, (int32_t)i2c_data[0], TLIMIT_LO_SNG_ST1,
            TLIMIT_HI_SNG_ST1, result);
    AKM_FST(TLIMIT_NO_SNG_HX, xval_i32, TLIMIT_LO_SNG_HX,
            TLIMIT_HI_SNG_HX, result);
    AKM_FST(TLIMIT_NO_SNG_HY, yval_i32, TLIMIT_LO_SNG_HY,
            TLIMIT_HI_SNG_HY, result);
    AKM_FST(TLIMIT_NO_SNG_HZ, zval_i32, TLIMIT_LO_SNG_HZ,
            TLIMIT_HI_SNG_HZ, result);
    AKM_FST(TLIMIT_NO_SNG_ST2, (int32_t)(i2c_data[11] & TLIMIT_ST2_MASK),
            TLIMIT_LO_SNG_ST2, TLIMIT_HI_SNG_ST2, result);

    /* Set to self-test mode. */
    ret = drv_mag_akm_ak09940_set_mode(dev,  (AK099XX_LOW_NOISE_DRIVE_2 | AK099XX_MODE_SELF_TEST));
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_NO_SLF_CNTL2;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }

    /* Wait for self test measurement. */
    aos_msleep(MEASURE_WAIT_TIME);

    /* Get measurement data from AK09940
     * ST1 + (HXL + HXM + HXH) + (HYL + HYM + HYH) + (HZL + HZM + HZH) + TMPS + ST2 = 12bytes */
    ret = sensor_i2c_read(dev, AK099XX_REG_ST1, i2c_data, AK099XX_BDATA_SIZE,
                          I2C_OP_RETRIES);
    if (AKM_SUCCESS != ret) {
        result[0] = TLIMIT_NO_SNG_WAIT;
        result[1] = ret;
        goto SELFTEST_FAIL;
    }

    /* Convert to 32-bit integer value. */
    xval_i32 = drv_mag_akm_ak09940_byts_compose(i2c_data[1], i2c_data[2], i2c_data[3]);
    yval_i32 = drv_mag_akm_ak09940_byts_compose(i2c_data[4], i2c_data[5], i2c_data[6]);
    zval_i32 = drv_mag_akm_ak09940_byts_compose(i2c_data[7], i2c_data[8], i2c_data[9]);

    AKM_FST(TLIMIT_NO_SLF_ST1, (int32_t)i2c_data[0], TLIMIT_LO_SLF_ST1,
            TLIMIT_HI_SLF_ST1, result);
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i32, TLIMIT_LO_SLF_RVHX,
            TLIMIT_HI_SLF_RVHX, result);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i32, TLIMIT_LO_SLF_RVHY,
            TLIMIT_HI_SLF_RVHY, result);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i32, TLIMIT_LO_SLF_RVHZ,
            TLIMIT_HI_SLF_RVHZ, result);
    AKM_FST(TLIMIT_NO_SLF_ST2, (int32_t)(i2c_data[11] & TLIMIT_ST2_MASK),
            TLIMIT_LO_SLF_ST2, TLIMIT_HI_SLF_ST2, result);

    AKM_LOG("self test success");
    return AKM_SUCCESS;

SELFTEST_FAIL:
    AKM_LOG("self test failed result step:0x%x, val:%d", result[0], result[1]);
    return AKM_ERROR;
}

int drv_mag_akm_ak09940_ioctl(
    int           cmd,
    unsigned long arg)
{
    int           ret = 0;
    //int64_t           self_test_result;
    dev_sensor_info_t *info = (dev_sensor_info_t *) arg;

    switch (cmd) {
    case SENSOR_IOCTL_ODR_SET:
        ret = drv_mag_akm_ak09940_set_odr(&ak09940_dev, (int)arg);
        if (unlikely(ret)) {
            return AKM_ERROR;
        }
        break;

    case SENSOR_IOCTL_RANGE_SET:
        break;

    case SENSOR_IOCTL_GET_INFO:
        drv_mag_akm_ak09940_get_info(info);
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
        ret = drv_mag_akm_ak09940_self_test(&ak09940_dev, &self_test_result);
        return ret;*/

    default:
        break;
    }

    return ret;
}

int drv_mag_akm_ak09940_init(void)
{
    int16_t      ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_MAG;
    sensor.path = dev_mag_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_mag_akm_ak09940_open;
    sensor.close = drv_mag_akm_ak09940_close;
    sensor.read = drv_mag_akm_ak09940_read;
    sensor.write = drv_mag_akm_ak09940_write;
    sensor.ioctl = drv_mag_akm_ak09940_ioctl;
    sensor.irq_handle = NULL;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_mag_akm_ak09940_valid_id(&ak09940_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_mag_akm_ak09940_soft_reset(&ak09940_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_mag_akm_ak09940_set_default_config(&ak09940_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    AKM_LOG("successfully.\n");
    return (int)ret;
}

SENSOR_DRV_ADD(drv_mag_akm_ak09940_init);

