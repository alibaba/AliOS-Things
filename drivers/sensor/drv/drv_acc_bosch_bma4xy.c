/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/
/*********************************************************************************************
*
*Copyright (C) 2016 - 2020 Bosch Sensortec GmbH

*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions are met:

*Redistributions of source code must retain the above copyright
*notice, this list of conditions and the following disclaimer.

*Redistributions in binary form must reproduce the above copyright
*notice, this list of conditions and the following disclaimer in the
*documentation and/or other materials provided with the distribution.

*Neither the name of the copyright holder nor the names of the
*contributors may be used to endorse or promote products derived from
*this software without specific prior written permission.

*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
*CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
*OR CONTRIBUTORS BE LIABLE FOR ANY
*DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
*OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
*HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
*WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*ANY WAY OUT OF THE USE OF THIS
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE

*The information provided is believed to be accurate and reliable.
*The copyright holder assumes no responsibility
*for the consequences of use
*of such information nor for any infringement of patents or
*other rights of third parties which may result from its use.
*No license is granted by implication or otherwise under any patent or
*patent rights of the copyright holder.
*
*
*******************************************************************************************/

#include <aos/aos.h>
#include <hal/base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include "common.h"
#include "sensor.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define BMA4xy_I2C_ADDR_LOW                    (0x18)
#define BMA4xy_I2C_ADDR_HIGH                   (0x19)
#define BMA4xy_I2C_ADDR_TRANS(n)               ((n)<<1)
#define BMA4xy_I2C_ADDR                        BMA4xy_I2C_ADDR_TRANS(BMA4xy_I2C_ADDR_LOW)

#define BMA4xy_CHIP_ID_ADDR                    UINT8_C(0X00)
#define BMA421_CHIP_ID_VALUE                   (0x11)
#define BMA422_CHIP_ID_VALUE                   (0x12)
#define BMA455_CHIP_ID_VALUE                   (0x15)
#define BMA456_CHIP_ID_VALUE                   (0x16)
#define BMI088_CHIP_ID_VALUE                   (0x1E)

/* POWER_CTRL REGISTER */
#define	BMA4_POWER_CONF_ADDR	               UINT8_C(0x7C)
#define	BMA4_POWER_CTRL_ADDR	               UINT8_C(0x7D)

/* name ACCEL CONFIG REGISTERS */
#define	BMA4_ACCEL_CONFIG_ADDR		           UINT8_C(0X40)
#define	BMA4_ACCEL_CONFIG1_ADDR		           UINT8_C(0X41)

#define BMA4xy_DEFAULT_ODR_100HZ               (100)
/* name COMMAND REGISTER */
#define	BMA4_CMD_ADDR		                   UINT8_C(0X7E)

/* ACCEL ODR */
#define	BMA4_OUTPUT_DATA_RATE_0_78HZ	       UINT8_C(0x01)
#define	BMA4_OUTPUT_DATA_RATE_1_56HZ	       UINT8_C(0x02)
#define	BMA4_OUTPUT_DATA_RATE_3_12HZ	       UINT8_C(0x03)
#define	BMA4_OUTPUT_DATA_RATE_6_25HZ	       UINT8_C(0x04)
#define	BMA4_OUTPUT_DATA_RATE_12_5HZ	       UINT8_C(0x05)
#define	BMA4_OUTPUT_DATA_RATE_25HZ		       UINT8_C(0x06)
#define	BMA4_OUTPUT_DATA_RATE_50HZ		       UINT8_C(0x07)
#define	BMA4_OUTPUT_DATA_RATE_100HZ		       UINT8_C(0x08)
#define	BMA4_OUTPUT_DATA_RATE_200HZ		       UINT8_C(0x09)
#define	BMA4_OUTPUT_DATA_RATE_400HZ		       UINT8_C(0x0A)
#define	BMA4_OUTPUT_DATA_RATE_800HZ		       UINT8_C(0x0B)
#define	BMA4_OUTPUT_DATA_RATE_1600HZ	       UINT8_C(0x0C)

/* ACCEL BANDWIDTH PARAMETER */
#define	BMA4_ACCEL_OSR4_AVG1		           UINT8_C(0)
#define	BMA4_ACCEL_OSR2_AVG2		           UINT8_C(1)
#define	BMA4_ACCEL_NORMAL_AVG4	               UINT8_C(2)
#define	BMA4_ACCEL_CIC_AVG8			           UINT8_C(3)
#define	BMA4_ACCEL_RES_AVG16		           UINT8_C(4)
#define	BMA4_ACCEL_RES_AVG32		           UINT8_C(5)
#define	BMA4_ACCEL_RES_AVG64		           UINT8_C(6)
#define	BMA4_ACCEL_RES_AVG128		           UINT8_C(7)

/* ACCEL RANGE CHECK */
#define	BMA4_ACCEL_RANGE_2G		               UINT8_C(0)
#define	BMA4_ACCEL_RANGE_4G		               UINT8_C(1)
#define	BMA4_ACCEL_RANGE_8G		               UINT8_C(2)
#define	BMA4_ACCEL_RANGE_16G	               UINT8_C(3)

/* AUX/ACCEL DATA BASE ADDRESS REGISTERS */
#define	BMA4_DATA_0_ADDR	                   UINT8_C(0X0A)
#define	BMA4_DATA_8_ADDR	                   UINT8_C(0X12)
#define	BMA4_ACCEL_DATA_LENGTH		           UINT8_C(6)

/* ENABLE/DISABLE BIT VALUES */
#define	BMA4_ENABLE			                   UINT8_C(0x01)
#define	BMA4_DISABLE		                   UINT8_C(0x00)

/* ADVANCE POWER SAVE POSITION AND MASK */
#define	BMA4_ADVANCE_POWER_SAVE_MSK	UINT8_C    (0x01)

/* ACCELEROMETER ENABLE POSITION AND MASK */
#define	BMA4_ACCEL_ENABLE_POS		           UINT8_C(2)
#define	BMA4_ACCEL_ENABLE_MSK		           UINT8_C(0x04)

/* ACCELEROMETER perform mode POSITION */
#define	BMA4_ACCEL_PERF_POS		               UINT8_C(7)

/* ACCELEROMETER ODR POSITION AND MASK */
#define	BMA4_ACCEL_ODR_POS		               UINT8_C(0)
#define	BMA4_ACCEL_ODR_MSK	                   UINT8_C(0x0F)

/* ACCELEROMETER RANGE POSITION AND MASK */
#define	BMA4_ACCEL_RANGE_POS	               UINT8_C(0)
#define	BMA4_ACCEL_RANGE_MSK	               UINT8_C(0x03)

/* BIT SLICE GET AND SET FUNCTIONS */
#define	BMA4_GET_BITSLICE(regvar, bitname)\
		((regvar & bitname##_MSK) >> bitname##_POS)
#define	BMA4_SET_BITSLICE(regvar, bitname, val)\
		((regvar & ~bitname##_MSK) | \
		((val<<bitname##_POS)&bitname##_MSK))
#define BMA4_SET_BITS_POS_0(reg_data, bitname, data) \
				((reg_data & ~(bitname##_MSK)) | \
				(data & bitname##_MSK))

#define BMA4_GET_BITS_POS_0(reg_data, bitname) (reg_data & (bitname##_MSK))

/* CMD values */
#define BMA4xy_ENABLE_SOFT_RESET_VALUE         UINT8_C(0XB6)

/* bma253 sensitivity factor table, the unit is LSB/g */
static uint32_t bma4xy_factor[4] = { 16384, 8192, 4096, 2048 };
static uint32_t current_factor = 0;
static uint32_t set_range_failed = 0;

i2c_dev_t bma4xy_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMA4xy_I2C_ADDR,
};

/**
 * This function does the soft reset
 *
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = BMA4xy_ENABLE_SOFT_RESET_VALUE;
    ret = sensor_i2c_write(drv, BMA4_CMD_ADDR, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        return -1;
    }
    return 0;
}

/**
 * This function validates the chip ID of device
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  id_value  the expected CHIPID
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_validate_id(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMA4xy_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("%s %s  sensor_i2c_read \n", SENSOR_STR, __func__);
        return ret;
    }

    if((BMA421_CHIP_ID_VALUE != value) && (BMA422_CHIP_ID_VALUE != value)
            && (BMA455_CHIP_ID_VALUE != value) && (BMA456_CHIP_ID_VALUE != value) && (BMI088_CHIP_ID_VALUE != value)) {
        printf("%s %s  value=%x \n", SENSOR_STR, __func__, value);
        return -1;
    }

    return 0;
}

/**
 * This function sets the acc powermode
 *
 * @param[in]  drv   pointer to the i2c dev
 * @param[in]  mode  the powermode to be setted
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_set_power_mode(i2c_dev_t* drv,
                                               dev_power_mode_e mode)
{

    uint8_t value, value1 = 0x00;
    int ret = 0;
    switch(mode) {
        case DEV_POWER_ON: {
            /* enable acc_pwerform_mode first */
            ret = sensor_i2c_read(drv, BMA4_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value |= (BMA4_ENABLE << BMA4_ACCEL_PERF_POS);
            ret = sensor_i2c_write(drv, BMA4_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }

            /* then enable PWR_CTRL acc_enable */
            ret = sensor_i2c_read(drv, BMA4_POWER_CTRL_ADDR, &value1, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value1 = BMA4_SET_BITSLICE(value1, BMA4_ACCEL_ENABLE, BMA4_ENABLE);
            ret = sensor_i2c_write(drv, BMA4_POWER_CTRL_ADDR, &value1, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }
        }
        break;
        case DEV_POWER_OFF:
        case DEV_SLEEP:
        case DEV_SUSPEND: {

            /* disable PWR_CTRL acc_enable first */
            ret = sensor_i2c_read(drv, BMA4_POWER_CTRL_ADDR, &value, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value = BMA4_SET_BITSLICE(value, BMA4_ACCEL_ENABLE, BMA4_DISABLE);
            ret = sensor_i2c_write(drv, BMA4_POWER_CTRL_ADDR, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }

            /* then powr_sav */
            ret = sensor_i2c_read(drv, BMA4_POWER_CONF_ADDR, &value1, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value1 = BMA4_SET_BITS_POS_0(value1, BMA4_ADVANCE_POWER_SAVE, BMA4_ENABLE);
            ret = sensor_i2c_write(drv, BMA4_POWER_CONF_ADDR, &value1, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }
        }
        break;

        default:
            break;
    }
    return 0;
}

/**
 * This function sets the acc ODR
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = 0x00;

    ret = sensor_i2c_read(drv, BMA4_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return ret;
    }

    if(hz >= 800) {
        odr = BMA4_OUTPUT_DATA_RATE_1600HZ;
    } else if(hz >= 400) {
        odr = BMA4_OUTPUT_DATA_RATE_800HZ;
    } else if(hz >= 200) {
        odr = BMA4_OUTPUT_DATA_RATE_400HZ;
    } else if(hz >= 100) {
        odr = BMA4_OUTPUT_DATA_RATE_200HZ;
    } else if(hz >= 50) {
        odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    } else if(hz >= 25) {
        odr = BMA4_OUTPUT_DATA_RATE_50HZ;
    } else if(hz >= 12) {
        odr = BMA4_OUTPUT_DATA_RATE_25HZ;
    } else if(hz >= 6) {
        odr = BMA4_OUTPUT_DATA_RATE_12_5HZ;
    } else if(hz >= 3) {
        odr = BMA4_OUTPUT_DATA_RATE_6_25HZ;
    } else {
        odr = BMA4_OUTPUT_DATA_RATE_3_12HZ;
    }

    value = BMA4_SET_BITSLICE(value, BMA4_ACCEL_ODR, odr);
    ret = sensor_i2c_write(drv, BMA4_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        return ret;
    }
    return 0;
}

/**
 * This function sets the acc range
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the range required
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t acc_range = 0x00;

    ret = sensor_i2c_read(drv, BMA4_ACCEL_CONFIG1_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        set_range_failed = 1;
        return ret;
    }

    switch(range) {
        case ACC_RANGE_2G: {
            acc_range = BMA4_ACCEL_RANGE_2G;
        }
        break;

        case ACC_RANGE_4G: {
            acc_range = BMA4_ACCEL_RANGE_4G;
        }
        break;

        case ACC_RANGE_8G: {
            acc_range = BMA4_ACCEL_RANGE_8G;
        }
        break;

        case ACC_RANGE_16G: {
            acc_range = BMA4_ACCEL_RANGE_16G;
        }
        break;

        default:
            break;
    }

    value = BMA4_SET_BITSLICE(value, BMA4_ACCEL_RANGE, acc_range);
    ret = sensor_i2c_write(drv, BMA4_ACCEL_CONFIG1_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        set_range_failed = 2;
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)) {
        current_factor = bma4xy_factor[range];
    }

    set_range_failed = 3;
    return 0;
}

/**
 * This function is the ISR
 *
 * @return
 */
void drv_acc_bosch_bma4xy_irq_handle(void)
{
    /* no handle so far */
}

/**
 * This function opens the acc
 *
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_open(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bma4xy_set_power_mode(&bma4xy_ctx, DEV_POWER_ON);
    if(unlikely(ret) != 0) {
        return -1;
    }
    return 0;

}

/**
 * This function closes the acc
 *
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_close(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bma4xy_set_power_mode(&bma4xy_ctx, DEV_POWER_OFF);
    if(unlikely(ret) != 0) {
        return -1;
    }
    return 0;
}

/**
 * This function reads the acc data and reports the data
 *
 * @param[in out]  buf   buffer for acc data
 * @param[in out]  len   length of data
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg[6];
    uint8_t range = 0;
    accel_data_t *accel = (accel_data_t *)buf;

    if(buf == NULL) {
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size) {
        return -1;
    }
    ret  = sensor_i2c_read(&bma4xy_ctx, 0x41,  &range, I2C_REG_LEN, I2C_OP_RETRIES);
    ret  = sensor_i2c_read(&bma4xy_ctx, BMA4_DATA_8_ADDR,  &reg[0], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma4xy_ctx, BMA4_DATA_8_ADDR+1,  &reg[1], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma4xy_ctx, BMA4_DATA_8_ADDR+2,  &reg[2], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma4xy_ctx, BMA4_DATA_8_ADDR+3,  &reg[3], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma4xy_ctx, BMA4_DATA_8_ADDR+4,  &reg[4], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma4xy_ctx, BMA4_DATA_8_ADDR+5,  &reg[5], I2C_REG_LEN,
                           I2C_OP_RETRIES);

    if(unlikely(ret) != 0) {
        return -1;
    }

    accel->data[DATA_AXIS_X] = ((int16_t)((reg[1] << 8) | reg[0]));
    accel->data[DATA_AXIS_Y] = ((int16_t)((reg[3] << 8) | reg[2]));
    accel->data[DATA_AXIS_Z] = ((int16_t)((reg[5] << 8) | reg[4]));
    if(current_factor != 0) {
        /* the unit of acc is mg, 1000 mg = 1 g */
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)current_factor;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)current_factor;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)current_factor;
    }
    accel->timestamp = aos_now_ms();

    return (int)size;
}

/**
 * This function is for the acc ioctl
 *
 * @param[in]  cmd   the ioctl command
 * @param[in]  arg   the correspondding parameter
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_bosch_bma4xy_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bma4xy_set_odr(&bma4xy_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bma4xy_set_range(&bma4xy_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bma4xy_set_power_mode(&bma4xy_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMA4xy";
            info->range_max = 16;
            info->range_min = 2;
            info->unit = mg;
        }
        break;

        default:
            break;
    }

    return 0;
}

/**
 * This function is for the acc initialization
 *
 * @return  the operation status, 0 is OK, others is error
 */
int drv_acc_bosch_bma4xy_init(void) {
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_bosch_bma4xy_open;
    sensor.close      = drv_acc_bosch_bma4xy_close;
    sensor.read       = drv_acc_bosch_bma4xy_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_bosch_bma4xy_ioctl;
    sensor.irq_handle = drv_acc_bosch_bma4xy_irq_handle;
    printf("%s %s successfully entered \n", SENSOR_STR, __func__);
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret) != 0) {
        printf("%s %s  sensor_create_obj \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_validate_id(&bma4xy_ctx);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_validate_id \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_soft_reset(&bma4xy_ctx);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_soft_reset \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_set_range(&bma4xy_ctx, ACC_RANGE_8G);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_range \n", SENSOR_STR, __func__);
        return -1;
    }

    /* set odr is 100hz, and will update */
    ret = drv_acc_bosch_bma4xy_set_odr(&bma4xy_ctx, BMA4xy_DEFAULT_ODR_100HZ);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_odr \n", SENSOR_STR, __func__);
        return -1;
    }
    ret = drv_acc_bosch_bma4xy_set_power_mode(&bma4xy_ctx, DEV_SLEEP);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_power_mode \n", SENSOR_STR, __func__);
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    printf("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}
