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


/* AK9754 Register definitions */
#define AK9754_WIA1_REG        0x00 /* Company code */
#define AK9754_WIA2_REG        0x01 /* Device ID */
#define AK9754_INFO1_REG       0x02 /* Unused informatiln */
#define AK9754_INFO2_REG       0x03 /* Unused informatiln */
#define AK9754_ST1_REG         0x04 /* Status 1 */
#define AK9754_IRDL_REG        0x05 /* IR sensor data (lower) */
#define AK9754_IRDH_REG        0x06 /* IR sensor data (upper) */
#define AK9754_TMPL_REG        0x07 /* Integrated temperature sensor data (lower) */
#define AK9754_TMPH_REG        0x08 /* Integrated temperature sensor data (upper) */
#define AK9754_ST2_REG         0x09 /* Status 2 */
#define AK9754_ST3_REG         0x0A /* Status 3 */
#define AK9754_SB0L_REG        0x0B /* latest IR sensor data (lower) */
#define AK9754_SB0H_REG        0x0C /* latest IR sensor data (upper) */
#define AK9754_SB1L_REG        0x0D /* previous IR sensor data (lower) */
#define AK9754_SB1H_REG        0x0E /* previous IR sensor data (upper) */
#define AK9754_SB2L_REG        0x0F /* 2 previous IR sensor data (lower) */
#define AK9754_SB2H_REG        0x10 /* 2 previous IR sensor data (upper) */
#define AK9754_SB3L_REG        0x11 /* 3 previous IR sensor data (lower) */
#define AK9754_SB3H_REG        0x12 /* 3 previous IR sensor data (upper) */
#define AK9754_SB4L_REG        0x13 /* 4 previous IR sensor data (lower) */
#define AK9754_SB4H_REG        0x14 /* 4 previous IR sensor data (upper) */
#define AK9754_SB5L_REG        0x15 /* 5 previous IR sensor data (lower) */
#define AK9754_SB5H_REG        0x16 /* 5 previous IR sensor data (upper) */
#define AK9754_SB6L_REG        0x17 /* 6 previous IR sensor data (lower) */
#define AK9754_SB6H_REG        0x18 /* 6 previous IR sensor data (upper) */
#define AK9754_SB7L_REG        0x19 /* 7 previous IR sensor data (lower) */
#define AK9754_SB7H_REG        0x1A /* 7 previous IR sensor data (upper) */
#define AK9754_SB8L_REG        0x1B /* 8 previous IR sensor data (lower) */
#define AK9754_SB8H_REG        0x1C /* 8 previous IR sensor data (upper) */
#define AK9754_SB9L_REG        0x1D /* oldest IR sensor data (lower) */
#define AK9754_SB9H_REG        0x1E /* oldest IR sensor data (lower) */
#define AK9754_ST4_REG         0x1F /* Status 4 */
#define AK9754_CTRL1_REG       0x20 /* Software reset */
#define AK9754_CTRL2_REG       0x21 /* Sync mode setting */
#define AK9754_CTRL3_REG       0x22 /* Lownoise mode / odr / tmp_fc / ird_fc settings */
#define AK9754_CTRL4_REG       0x23 /* Track mode settings */
#define AK9754_CTRL5_REG       0x24 /* TMP offset settings */
#define AK9754_CTRL6_REG       0x25 /* IRD gain settings */
#define AK9754_CTRL7_REG       0x26 /* Invert mode / wait time settings */
#define AK9754_CTRL8_REG       0x27 /* NSAMPLE settings */
#define AK9754_CTRL9_REG       0x28 /* Threshold level settings (lower) */
#define AK9754_CTRL10_REG      0x29 /* Threshold level settings (upper) */
#define AK9754_CTRL11_REG      0x2A /* Enable/Disable settings */
#define AK9754_CTRL12_REG      0x2B /* Mode */

/* Register helper values */
#define AK9754_COMPANY_ID      0x48 /* Company ID of AKM */
#define AK9754_DEVICE_ID       0x15 /* Device ID of AK9754 */
#define AK9754_WIA_VAL         0x1548

#define AK9754_ST1_DRDY        0x01 /* bit mask of DRDY for ST1 */
#define AK9754_ST1_HBD_ONCE    0x10 /* bit mask of HBD_ONCE for ST1 */

#define AK9754_CTRL1_SRST         1 /* bit mask of SRST for CTRL1 */

#define AK9754_CTRL3_IRD_FC_SHIFT       0
#define AK9754_CTRL3_TMP_FC_SHIFT       2
#define AK9754_CTRL3_ODR_SHIFT          4
#define AK9754_CTRL3_LOWNOISEMODE_SHIFT 6
/* helper function to make CTRL3 register's value */
#define AK9754_CTRL3(lnmode, odr, tmpfc, irdfc) \
    ((lnmode) << (AK9754_CTRL3_LOWNOISEMODE_SHIFT) | \
     (odr) << (AK9754_CTRL3_ODR_SHIFT) | \
     (tmpfc) << (AK9754_CTRL3_TMP_FC_SHIFT) | \
     (irdfc) << (AK9754_CTRL3_IRD_FC_SHIFT))

#define AK9754_CTRL7_HBD_INVERT_SHIFT   3
#define AK9754_CTRL7_HBD_WAITTIME_SHIFT 0
/* helper function to make CTRL7 register's value */
#define AK9754_CTRL7(invert, waittime) \
    ((invert) << (AK9754_CTRL7_HBD_INVERT_SHIFT) | \
     (waittime) << (AK9754_CTRL7_HBD_WAITTIME_SHIFT))

/* helper function to make CTRL9 / CTRL10 register's value */
#define AK9754_CTRL9(thld) ((thld) & 0xff)
#define AK9754_CTRL10(thld) (((thld) & 0xff00) >> 8)

#define AK9754_CNTL11_INT_ENABLE_DRDY 0x01
#define AK9754_CNTL11_INT_ENABLE_HBD  0x02
#define AK9754_CNTL11_SB_ENABLE       0x04
#define AK9754_CNTL11_SB_STOP_ON_HBD  0x08
#define AK9754_CNTL11_HBD_ENABLE      0x10

#define AK9754_CTRL12_MODE_STANDBY 0x00 /* Standby mode */
#define AK9754_CTRL12_MODE_MEASURE 0x01 /* Continuous measurement mode */

#define AK9754_SUPPORTED_ODR_POWER_DOWN 0
#define AK9754_SUPPORTED_ODR_1HZ        1
#define AK9754_SUPPORTED_ODR_2HZ        2
#define AK9754_SUPPORTED_ODR_10HZ       10
#define AK9754_SUPPORTED_ODR_50HZ       50

/*
 * The values below are generate automatically
 */
/* CNTL3 values */
/* LOWNOISEMODE bit: valid value is 1 or 0 */
#define AK9754_LOWNOISEMODE_VAL    1
/* ODR bit: valid rang is [0x00, 0x03] 00 1Hz 01 2Hz 02 10Hz 03 50Hz*/
#define AK9754_ODR_VAL             0x02
/* TMP_FC bit: valid rang is [0x00, 0x02] */
#define AK9754_TMP_FC_VAL          0x01
/* IRD_FC bit: valid rang is [0x00, 0x02] */
#define AK9754_IRD_FC_VAL          0x01

/* CNTL4 valus */
/* TRACKMODE bit: valid rang is [0x00, 0x07] */
#define AK9754_TRACKMODE_VAL       0x00

/* CNTL5 values */
/* TMP_OFFSET bit: valid rang is [0xc0, 0x3f] ([-64, 63]) */
#define AK9754_TMP_OFFSET_VAL      0x80

/* CNTL6 values */
/* IRD_GAIN bit: valid rang is [0xf0, 0x0f] ([-16, 15]) */
#define AK9754_IRD_GAIN_VAL        0xfa

/* CNTL7 values */
/* HBD_INVERT bit: valid value is 1 or 0  */
#define AK9754_INVERT_VAL          0
/* HBD_WAITTIME bit: valid range is [0x00, 0x07] */
#define AK9754_WAITTIME_VAL        0

/* CNTL8 values */
/* HBD_NSAMPLE bit: valid range is [0x00, 0x7f] */
#define AK9754_NSAMPLE_VAL         0x01

/* CNTL9, 10 values */
/* HBD_THLD  bit: valid range is [0x0000, 0x7fff] ([0, 32767]) */
#define AK9754_THLD_VAL            75

#define AK9754_DEVICE_NAME    "AK9754"

#define AKM_SUCCESS           (0)
#define AKM_ERROR             (-1)

#define MEASURE_WAIT_TIME     (20)

#define AK9754_DEFAULT_MODE   AK9754_CTRL3(AK9754_LOWNOISEMODE_VAL, AK9754_ODR_VAL, AK9754_TMP_FC_VAL, AK9754_IRD_FC_VAL) /* Default 10Hz */

#define AKM_DEVICE_NAME       AK9754_DEVICE_NAME

#define AKM_LOG(format, ...)  LOG("%s %s %s " format, SENSOR_STR, \
                                  AKM_DEVICE_NAME, __func__, \
                                  ## __VA_ARGS__)

/*************************************************
CAD1               CAD0            Slave Address
VSS                VSS                60H
VSS                non-connected      61H
VSS                VDD                62H
non-connected      VSS                64H
non-connected      non-connected      65H
non-connected      VDD                66H
VDD                VSS                68H
VDD                non-connected      69H
VDD                VDD              Do Not Use
************************************************/
i2c_dev_t ak9754_dev = {
    .port = 3,
    .config.address_width = 8,
    .config.dev_addr = 0x60 << 1,
    .config.freq = 100,
};

static int16_t drv_ir_akm_ak9754_set_mode(
    i2c_dev_t *dev,
    const uint8_t   mode)
{
    int16_t ret = 0;
    uint8_t i2c_data = mode;

    ret = sensor_i2c_write(dev, AK9754_CTRL12_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int drv_ir_akm_ak9754_soft_reset(i2c_dev_t *dev)
{
    int     ret = 0;
    uint8_t i2c_data = AK9754_CTRL1_SRST;

    ret = sensor_i2c_write(dev, AK9754_CTRL1_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int16_t drv_ir_akm_ak9754_valid_id(i2c_dev_t *dev)
{
    int      ret = 0;
    uint8_t  i2c_data[2];
    uint16_t id = 0;

    ret = sensor_i2c_read(dev, AK9754_WIA1_REG, i2c_data, 2, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    id = (uint16_t)(((uint16_t)i2c_data[1] << 8) | i2c_data[0]);

    if (AK9754_WIA_VAL != id) {
        AKM_LOG("invalid device.\n");
        return AKM_ERROR;
    }

    return 0;
}

static void drv_ir_akm_ak9754_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ir_akm_ak9754_open(void)
{
    int16_t   ret = 0;
    i2c_dev_t *dev = &ak9754_dev;

    ret = drv_ir_akm_ak9754_set_mode(dev, AK9754_CTRL12_MODE_MEASURE);
    if (unlikely(ret)) {
        return ret;
    }

    AKM_LOG("successfully \n");
    return ret;
}

static int drv_ir_akm_ak9754_close(void)
{
    int16_t   ret = 0;
    i2c_dev_t *dev = &ak9754_dev;

    ret = drv_ir_akm_ak9754_set_mode(dev, AK9754_CTRL12_MODE_STANDBY);
    if (unlikely(ret)) {
        return ret;
    }

    AKM_LOG("successfully \n");
    return ret;
}

static int drv_ir_akm_ak9754_read(
    void   *buf,
    size_t len)
{
    //ir_data_t *ir_data = (ir_data_t *)buf;
    ir_data_t *ir_data = (ir_data_t *)buf;
    i2c_dev_t  *dev = &ak9754_dev;
    size_t     size;
    uint8_t    i2c_data[6];
    int16_t    ret = 0;

    if (NULL == buf) {
        return AKM_ERROR;
    }

    size = sizeof(ir_data_t);

    if (len < size) {
        return AKM_ERROR;
    }

    /* Get measurement data from AK9754
     * ST1 + (IRDL + IRDH) + (TMPL+ TMPH) + ST2 = 6bytes */
    ret = sensor_i2c_read(dev, AK9754_ST1_REG, i2c_data, 6,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    //ir_data->ir = (uint32_t)((int16_t)(i2c_data[1] | (((uint16_t)i2c_data[2]) << 8) + 32767);
    ir_data->ir = (uint32_t)((int16_t)(i2c_data[1] | ((uint16_t)i2c_data[2] << 8)) + 32767);
    ir_data->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_ir_akm_ak9754_write(
    const void *buf,
    size_t     len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_ir_akm_ak9754_set_odr(
    i2c_dev_t *dev,
    const uint32_t  odr)
{
    uint8_t ODR_VAL = 0;
    int ret;

    if (odr == AK9754_SUPPORTED_ODR_POWER_DOWN) {
        ret = drv_ir_akm_ak9754_close();
        if (unlikely(ret)) {
            return ret;
        }
    } else if (odr <= AK9754_SUPPORTED_ODR_1HZ) {
        ODR_VAL = AK9754_CTRL3(AK9754_LOWNOISEMODE_VAL, 0x00, AK9754_TMP_FC_VAL,
                                 AK9754_IRD_FC_VAL);
    } else if (odr <= AK9754_SUPPORTED_ODR_2HZ) {
        ODR_VAL = AK9754_CTRL3(AK9754_LOWNOISEMODE_VAL, 0x01, AK9754_TMP_FC_VAL,
                                 AK9754_IRD_FC_VAL);
    } else if (odr <= AK9754_SUPPORTED_ODR_10HZ) {
        ODR_VAL = AK9754_CTRL3(AK9754_LOWNOISEMODE_VAL, AK9754_ODR_VAL, AK9754_TMP_FC_VAL,
                                 AK9754_IRD_FC_VAL);
    } else if (odr <= AK9754_SUPPORTED_ODR_50HZ) {
        ODR_VAL = AK9754_CTRL3(AK9754_LOWNOISEMODE_VAL, 0x03, AK9754_TMP_FC_VAL,
                                 AK9754_IRD_FC_VAL);
    } else {
        AKM_LOG("unsupported ODR : %d, set to default mode\n", odr);
        ODR_VAL = AK9754_DEFAULT_MODE;
    }

    ret = sensor_i2c_write(dev, AK9754_CTRL3_REG, &ODR_VAL, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_ir_akm_ak9754_open();
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_ir_akm_ak9754_get_info(dev_sensor_info_t *info)
{
    info->vendor = DEV_SENSOR_VENDOR_AKM;
    info->model = AKM_DEVICE_NAME;
    info->unit = cm;
    info->range_max =  15000;
    info->range_min = -15000;

    return;
}

static int16_t drv_ir_akm_ak9754_set_default_config(i2c_dev_t *dev)
{
    int16_t ret = 0;

    /*
     * setup measurement conditions (CTRL3 ~ CTRL6)
     */
    /* CTRL3 : Lownoise mode / odr / tmpfc / irdfc settings */
    uint8_t i2c_data = AK9754_CTRL3(AK9754_LOWNOISEMODE_VAL, AK9754_ODR_VAL, AK9754_TMP_FC_VAL,
                                 AK9754_IRD_FC_VAL);

    ret = sensor_i2c_write(dev, AK9754_CTRL3_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* CTRL4 : Track mode settings */
    i2c_data = AK9754_TRACKMODE_VAL;
    ret = sensor_i2c_write(dev, AK9754_CTRL4_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* CTRL5 : TMP offset settings */
    i2c_data = AK9754_TMP_OFFSET_VAL;
    ret = sensor_i2c_write(dev, AK9754_CTRL5_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* CTRL6 : IRD gain settings */
    i2c_data = AK9754_IRD_GAIN_VAL;
    ret = sensor_i2c_write(dev, AK9754_CTRL6_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /*
     * setup parameteres for human body detection (CTRL7 ~ CTRL10)
     */
    /* CTRL7 : Invert mode / waittime settings */
    i2c_data = AK9754_CTRL7(AK9754_INVERT_VAL, AK9754_WAITTIME_VAL);
    ret = sensor_i2c_write(dev, AK9754_CTRL7_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* CTRL8 : NSAMPLE settings */
    i2c_data = AK9754_NSAMPLE_VAL;
    ret = sensor_i2c_write(dev, AK9754_CTRL8_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
	
    /* CTRL9 : Threshold level settings (lower) */
    i2c_data = AK9754_CTRL9(AK9754_THLD_VAL);
    ret = sensor_i2c_write(dev, AK9754_CTRL9_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /* CTRL10 : Threshold level settings (upper) */
    i2c_data = AK9754_CTRL10(AK9754_THLD_VAL);
    ret = sensor_i2c_write(dev, AK9754_CTRL10_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    /*
     * device interrupt settings
     *   In this codes, AK9754 rise interrupts when human is detected
     */
    /* CTRL11 : function's Enable / Disable settings */
    i2c_data = AK9754_CNTL11_HBD_ENABLE | AK9754_CNTL11_INT_ENABLE_HBD;
    ret = sensor_i2c_write(dev, AK9754_CTRL11_REG, &i2c_data, 1, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}



int drv_ir_akm_ak9754_ioctl(
    int           cmd,
    unsigned long arg)
{
    int           ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *) arg;

    switch (cmd) {
    case SENSOR_IOCTL_ODR_SET:
        ret = drv_ir_akm_ak9754_set_odr(&ak9754_dev, (int)arg);
        if (unlikely(ret)) {
            return AKM_ERROR;
        }

        break;

    case SENSOR_IOCTL_RANGE_SET:
        break;

    case SENSOR_IOCTL_GET_INFO:
        drv_ir_akm_ak9754_get_info(info);
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

    default:
        break;
    }

    return ret;
}

int drv_ir_akm_ak9754_init(void)
{
    int16_t      ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_IR;
    sensor.path = dev_ir_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_ir_akm_ak9754_open;
    sensor.close = drv_ir_akm_ak9754_close;
    sensor.read = drv_ir_akm_ak9754_read;
    sensor.write = drv_ir_akm_ak9754_write;
    sensor.ioctl = drv_ir_akm_ak9754_ioctl;
    sensor.irq_handle = drv_ir_akm_ak9754_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_ir_akm_ak9754_valid_id(&ak9754_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_ir_akm_ak9754_soft_reset(&ak9754_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    ret = drv_ir_akm_ak9754_set_default_config(&ak9754_dev);
    if (unlikely(ret)) {
        return (int)ret;
    }

    AKM_LOG("successfully.\n");
    return (int)ret;
}

SENSOR_DRV_ADD(drv_ir_akm_ak9754_init);

