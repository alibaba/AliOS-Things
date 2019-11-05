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

#include "aos/kernel.h"
#include "network/hal/base.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define BMI160_I2C_ADDR_TRANS(n)                            ((n)<<1)
#define BMI160_I2C_SLAVE_ADDR_LOW                           0x68
#define BMI160_I2C_ADDR                                     BMI160_I2C_ADDR_TRANS(BMI160_I2C_SLAVE_ADDR_LOW)
/* COMMAND REGISTER */
#define BMI160_CMD_COMMANDS_ADDR				            (0X7E)
/* CHIP ID */
#define BMI160_USER_CHIP_ID_ADDR				            (0x00)
/* ACCEL CONFIG REGISTERS  FOR ODR, BANDWIDTH AND UNDERSAMPLING */
#define BMI160_USER_ACCEL_CONFIG_ADDR			            (0X40)
/* ACCEL RANGE */
#define BMI160_USER_ACCEL_RANGE_ADDR                        (0X41)
/* ACCEL DATA REGISTERS */
#define BMI160_USER_DATA_14_ADDR				            (0X12)
#define BMI160_USER_DATA_15_ADDR				            (0X13)
#define BMI160_USER_DATA_16_ADDR				            (0X14)
#define BMI160_USER_DATA_17_ADDR				            (0X15)
#define BMI160_USER_DATA_18_ADDR				            (0X16)
#define BMI160_USER_DATA_19_ADDR				            (0X17)
/* GYRO DATA REGISTERS */
#define BMI160_USER_DATA_8_ADDR					            (0X0C)
#define BMI160_USER_DATA_9_ADDR					            (0X0D)
#define BMI160_USER_DATA_10_ADDR				            (0X0E)
#define BMI160_USER_DATA_11_ADDR				            (0X0F)
#define BMI160_USER_DATA_12_ADDR				            (0X10)
#define BMI160_USER_DATA_13_ADDR				            (0X11)

#define BMI160_MODE_SWITCHING_DELAY		                    (30)

/* GYRO CONFIG REGISTERS  FOR ODR AND BANDWIDTH */
#define BMI160_USER_GYRO_CONFIG_ADDR                        (0X42)
/* GYRO RANGE */
#define BMI160_USER_GYRO_RANGE_ADDR                         (0X43)

/* BMI160 CHIPID */
#define BMI160_CHIP_ID_VALUE                                (0xD1)
/* CMD REGISTERS DEFINITION START */
/* COMMAND REGISTER LENGTH, POSITION AND MASK */
/* Command description address - Reg Addr --> 0x7E, Bit -->  0....7 */
#define BMI160_CMD_COMMANDS__POS                            (0)
#define BMI160_CMD_COMMANDS__LEN                            (8)
#define BMI160_CMD_COMMANDS__MSK                            (0xFF)
#define BMI160_CMD_COMMANDS__REG	                        (BMI160_CMD_COMMANDS_ADDR)

/* CMD */
#define BMI160_CMD_SOFTRESET                                (0xB6)
/* name	ACCEL POWER MODE */
#define ACCEL_MODE_NORMAL	                                (0x11)
#define	ACCEL_LOWPOWER		                                (0X12)
#define	ACCEL_SUSPEND		                                (0X10)
/* BMI160 Accel power modes */
#define BMI160_ACCEL_SUSPEND                                0
#define BMI160_ACCEL_NORMAL_MODE                            1
#define BMI160_ACCEL_LOW_POWER                              2
/* GYRO POWER MODE */
#define GYRO_MODE_SUSPEND		                            (0x14)
#define GYRO_MODE_NORMAL		                            (0x15)
#define GYRO_MODE_FASTSTARTUP	                            (0x17)
/* CHIP ID LENGTH, POSITION AND MASK */
/* Chip ID Description - Reg Addr --> (0x00), Bit --> 0...7 */
#define BMI160_USER_CHIP_ID__POS                            (0)
#define BMI160_USER_CHIP_ID__MSK                            (0xFF)
#define BMI160_USER_CHIP_ID__LEN                            (8)
#define BMI160_USER_CHIP_ID__REG                            (BMI160_USER_CHIP_ID_ADDR)
/* ACCEL CONFIGURATION LENGTH, POSITION AND MASK */
/* Acc_Conf Description - Reg Addr --> (0x40), Bit --> 0...3 */
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__POS      (0)
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__LEN      (4)
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__MSK      (0x0F)
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG		       \
(BMI160_USER_ACCEL_CONFIG_ADDR)

/* Acc_Conf Description - Reg Addr --> (0x40), Bit --> 4...6 */
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__POS              (4)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__LEN              (3)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__MSK              (0x70)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__REG	            (BMI160_USER_ACCEL_CONFIG_ADDR)

/* Acc_Conf Description - Reg Addr --> (0x40), Bit --> 7 */
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__POS  (7)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__LEN  (1)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__MSK  (0x80)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__REG	\
(BMI160_USER_ACCEL_CONFIG_ADDR)

/* Acc_Range Description - Reg Addr --> 0x41, Bit --> 0...3 */
#define BMI160_USER_ACCEL_RANGE__POS                        (0)
#define BMI160_USER_ACCEL_RANGE__LEN                        (4)
#define BMI160_USER_ACCEL_RANGE__MSK                        (0x0F)
#define BMI160_USER_ACCEL_RANGE__REG              \
(BMI160_USER_ACCEL_RANGE_ADDR)

/* GYRO CONFIGURATION LENGTH, POSITION AND MASK */
/* Gyro_Conf Description - Reg Addr --> (0x42), Bit --> 0...3 */
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__POS      (0)
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__LEN      (4)
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__MSK      (0x0F)
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG               \
(BMI160_USER_GYRO_CONFIG_ADDR)

/* Gyro_Conf Description - Reg Addr --> (0x42), Bit --> 4...5 */
#define BMI160_USER_GYRO_CONFIG_BW__POS                    (4)
#define BMI160_USER_GYRO_CONFIG_BW__LEN                    (2)
#define BMI160_USER_GYRO_CONFIG_BW__MSK                    (0x30)
#define BMI160_USER_GYRO_CONFIG_BW__REG \
(BMI160_USER_GYRO_CONFIG_ADDR)

/* Gyr_Range Description - Reg Addr --> 0x43, Bit --> 0...2 */
#define BMI160_USER_GYRO_RANGE__POS                       (0)
#define BMI160_USER_GYRO_RANGE__LEN                       (3)
#define BMI160_USER_GYRO_RANGE__MSK                       (0x07)
#define BMI160_USER_GYRO_RANGE__REG                       (BMI160_USER_GYRO_RANGE_ADDR)

/* ACCEL ODR */
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED            (0x00)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ              (0x01)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ              (0x02)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ              (0x03)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ              (0x04)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ              (0x05)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ                (0x06)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ                (0x07)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ               (0x08)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ               (0x09)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ               (0x0A)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ               (0x0B)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ              (0x0C)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED0           (0x0D)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED1           (0x0E)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED2           (0x0F)

/* GYRO ODR */
#define BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED             (0x00)
#define BMI160_GYRO_OUTPUT_DATA_RATE_25HZ	              (0x06)
#define BMI160_GYRO_OUTPUT_DATA_RATE_50HZ	              (0x07)
#define BMI160_GYRO_OUTPUT_DATA_RATE_100HZ	              (0x08)
#define BMI160_GYRO_OUTPUT_DATA_RATE_200HZ	              (0x09)
#define BMI160_GYRO_OUTPUT_DATA_RATE_400HZ	              (0x0A)
#define BMI160_GYRO_OUTPUT_DATA_RATE_800HZ	              (0x0B)
#define BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ	              (0x0C)
#define BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ	              (0x0D)
/* default HZ */
#define BMI160_ACC_DEFAULT_ODR_100HZ                      (100)
#define BMI160_ACC_DEFAULT_ODR_25HZ                       (25)
#define BMI160_GYRO_DEFAULT_ODR_100HZ                     (100)
#define BMI160_GYRO_DEFAULT_ODR_25HZ                      (25)
/* ACCEL RANGE */
#define BMI160_ACCEL_RANGE_2G                             (0X03)
#define BMI160_ACCEL_RANGE_4G                             (0X05)
#define BMI160_ACCEL_RANGE_8G                             (0X08)
#define BMI160_ACCEL_RANGE_16G                            (0X0C)

/* ACC sensitivity */
#define BMI160_ACC_SENSITIVITY_2G                         (16384)
#define BMI160_ACC_SENSITIVITY_4G                         (8192)
#define BMI160_ACC_SENSITIVITY_8G                         (4096)
#define BMI160_ACC_SENSITIVITY_16G                        (2048)

/* GYROSCOPE RANGE PARAMETER */
#define BMI160_GYRO_RANGE_2000_DEG_SEC	                  (0x00)
#define BMI160_GYRO_RANGE_1000_DEG_SEC	                  (0x01)
#define BMI160_GYRO_RANGE_500_DEG_SEC	                  (0x02)
#define BMI160_GYRO_RANGE_250_DEG_SEC	                  (0x03)
#define BMI160_GYRO_RANGE_125_DEG_SEC	                  (0x04)

/* GYRO sensitivity */
#define BMI160_GYRO_SENSITIVITY_125DPS                    (262)
#define BMI160_GYRO_SENSITIVITY_250DPS                    (131)
#define BMI160_GYRO_SENSITIVITY_500DPS                    (66)
#define BMI160_GYRO_SENSITIVITY_1000DPS                   (33)
#define BMI160_GYRO_SENSITIVITY_2000DPS                   (16)

/* ACCEL DATA XYZ LENGTH, POSITION AND MASK */
/* ACC_X (LSB) Description - Reg Addr --> (0x12), Bit --> 0...7 */
#define BMI160_USER_DATA_14_ACCEL_X_LSB__POS              (0)
#define BMI160_USER_DATA_14_ACCEL_X_LSB__LEN              (8)
#define BMI160_USER_DATA_14_ACCEL_X_LSB__MSK              (0xFF)
#define BMI160_USER_DATA_14_ACCEL_X_LSB__REG              (BMI160_USER_DATA_14_ADDR)

/* ACC_X (MSB) Description - Reg Addr --> 0x13, Bit --> 0...7 */
#define BMI160_USER_DATA_15_ACCEL_X_MSB__POS              (0)
#define BMI160_USER_DATA_15_ACCEL_X_MSB__LEN              (8)
#define BMI160_USER_DATA_15_ACCEL_X_MSB__MSK              (0xFF)
#define BMI160_USER_DATA_15_ACCEL_X_MSB__REG              (BMI160_USER_DATA_15_ADDR)

/* ACC_Y (LSB) Description - Reg Addr --> (0x14), Bit --> 0...7 */
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__POS              (0)
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__LEN              (8)
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__MSK              (0xFF)
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__REG              (BMI160_USER_DATA_16_ADDR)

/* ACC_Y (MSB) Description - Reg Addr --> (0x15), Bit --> 0...7 */
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__POS              (0)
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__LEN              (8)
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__MSK              (0xFF)
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__REG              (BMI160_USER_DATA_17_ADDR)

/* ACC_Z (LSB) Description - Reg Addr --> 0x16, Bit --> 0...7 */
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__POS              (0)
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__LEN              (8)
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__MSK              (0xFF)
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__REG              (BMI160_USER_DATA_18_ADDR)

/* ACC_Z (MSB) Description - Reg Addr --> (0x17), Bit --> 0...7 */
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__POS              (0)
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__LEN              (8)
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__MSK              (0xFF)
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__REG              (BMI160_USER_DATA_19_ADDR)

/* GYRO DATA XYZ LENGTH, POSITION AND MASK */
/* GYR_X (LSB) Description - Reg Addr --> (0x0C), Bit --> 0...7 */
#define BMI160_USER_DATA_8_GYRO_X_LSB__POS               (0)
#define BMI160_USER_DATA_8_GYRO_X_LSB__LEN               (8)
#define BMI160_USER_DATA_8_GYRO_X_LSB__MSK               (0xFF)
#define BMI160_USER_DATA_8_GYRO_X_LSB__REG               (BMI160_USER_DATA_8_ADDR)

/* GYR_X (MSB) Description - Reg Addr --> (0x0D), Bit --> 0...7 */
#define BMI160_USER_DATA_9_GYRO_X_MSB__POS               (0)
#define BMI160_USER_DATA_9_GYRO_X_MSB__LEN               (8)
#define BMI160_USER_DATA_9_GYRO_X_MSB__MSK               (0xFF)
#define BMI160_USER_DATA_9_GYRO_X_MSB__REG               (BMI160_USER_DATA_9_ADDR)

/* GYR_Y (LSB) Description - Reg Addr --> 0x0E, Bit --> 0...7 */
#define BMI160_USER_DATA_10_GYRO_Y_LSB__POS              (0)
#define BMI160_USER_DATA_10_GYRO_Y_LSB__LEN              (8)
#define BMI160_USER_DATA_10_GYRO_Y_LSB__MSK              (0xFF)
#define BMI160_USER_DATA_10_GYRO_Y_LSB__REG              (BMI160_USER_DATA_10_ADDR)

/* GYR_Y (MSB) Description - Reg Addr --> (0x0F), Bit --> 0...7 */
#define BMI160_USER_DATA_11_GYRO_Y_MSB__POS              (0)
#define BMI160_USER_DATA_11_GYRO_Y_MSB__LEN              (8)
#define BMI160_USER_DATA_11_GYRO_Y_MSB__MSK              (0xFF)
#define BMI160_USER_DATA_11_GYRO_Y_MSB__REG              (BMI160_USER_DATA_11_ADDR)

/* GYR_Z (LSB) Description - Reg Addr --> (0x10), Bit --> 0...7 */
#define BMI160_USER_DATA_12_GYRO_Z_LSB__POS              (0)
#define BMI160_USER_DATA_12_GYRO_Z_LSB__LEN              (8)
#define BMI160_USER_DATA_12_GYRO_Z_LSB__MSK              (0xFF)
#define BMI160_USER_DATA_12_GYRO_Z_LSB__REG              (BMI160_USER_DATA_12_ADDR)

/* GYR_Z (MSB) Description - Reg Addr --> (0x11), Bit --> 0...7 */
#define BMI160_USER_DATA_13_GYRO_Z_MSB__POS              (0)
#define BMI160_USER_DATA_13_GYRO_Z_MSB__LEN              (8)
#define BMI160_USER_DATA_13_GYRO_Z_MSB__MSK              (0xFF)
#define BMI160_USER_DATA_13_GYRO_Z_MSB__REG              (BMI160_USER_DATA_13_ADDR)

/* SHIFT VALUE DEFINITION */
#define BMI160_SHIFT_BIT_POSITION_BY_01_BIT              (1)
#define BMI160_SHIFT_BIT_POSITION_BY_02_BITS             (2)
#define BMI160_SHIFT_BIT_POSITION_BY_03_BITS             (3)
#define BMI160_SHIFT_BIT_POSITION_BY_04_BITS             (4)
#define BMI160_SHIFT_BIT_POSITION_BY_05_BITS             (5)
#define BMI160_SHIFT_BIT_POSITION_BY_06_BITS             (6)
#define BMI160_SHIFT_BIT_POSITION_BY_07_BITS             (7)
#define BMI160_SHIFT_BIT_POSITION_BY_08_BITS             (8)
#define BMI160_SHIFT_BIT_POSITION_BY_09_BITS             (9)
#define BMI160_SHIFT_BIT_POSITION_BY_12_BITS             (12)
#define BMI160_SHIFT_BIT_POSITION_BY_13_BITS             (13)
#define BMI160_SHIFT_BIT_POSITION_BY_14_BITS             (14)
#define BMI160_SHIFT_BIT_POSITION_BY_15_BITS             (15)
#define BMI160_SHIFT_BIT_POSITION_BY_16_BITS             (16)

/* BIT SLICE GET AND SET FUNCTIONS */
#define BMI160_GET_BITSLICE(regvar, bitname)\
		((regvar & bitname##__MSK) >> bitname##__POS)


#define BMI160_SET_BITSLICE(regvar, bitname, val)\
		((regvar & ~bitname##__MSK) | \
		((val<<bitname##__POS)&bitname##__MSK))

static int32_t g_bmi160_acc_factor[ACC_RANGE_MAX] = { BMI160_ACC_SENSITIVITY_2G, BMI160_ACC_SENSITIVITY_4G,
                                                    BMI160_ACC_SENSITIVITY_8G, BMI160_ACC_SENSITIVITY_16G
                                                  };
static int32_t g_bmi160_gyro_factor[GYRO_RANGE_MAX] = {BMI160_GYRO_SENSITIVITY_125DPS, BMI160_GYRO_SENSITIVITY_250DPS, BMI160_GYRO_SENSITIVITY_500DPS,
                                                     BMI160_GYRO_SENSITIVITY_1000DPS, BMI160_GYRO_SENSITIVITY_2000DPS
                                                    };

static int32_t g_cur_acc_factor = 0;
static int32_t g_cur_gyro_factor = 0;
static int32_t g_bmi160flag = 0;

i2c_dev_t bmi160_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMI160_I2C_ADDR,
};

/**
 * This function does the soft reset
 *
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_gyro_bosch_bmi160_soft_reset(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;
    value = BMI160_CMD_SOFTRESET;

    ret = sensor_i2c_write(drv, BMI160_CMD_COMMANDS__REG, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
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
static int drv_acc_gyro_bosch_bmi160_validate_id(i2c_dev_t* drv,
                                                 uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret = 0;

    if(drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMI160_USER_CHIP_ID__REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read CHIPID failed \n");
        return ret;
    }
    printf("read CHIPID  %x\n", value);
    if(id_value != value) {
        printf("failed read CHIPID  %x\n", value);
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
static int drv_acc_bosch_bmi160_set_power_mode(i2c_dev_t* drv,
                                               dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret = 0;

    switch(mode) {
        case DEV_POWER_ON: {

            value = ACCEL_MODE_NORMAL;
            ret = sensor_i2c_write(drv, BMI160_CMD_COMMANDS__REG, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }
        }
        break;

        case DEV_POWER_OFF:
        case DEV_SLEEP: {
            value = ACCEL_SUSPEND;
            ret = sensor_i2c_write(drv, BMI160_CMD_COMMANDS__REG, &value, I2C_DATA_LEN,
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
 * This function gets the acc ODR according to HZ
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the conrresponding acc ODR
 */
static uint8_t drv_acc_bosch_bmi160_hz2odr(uint32_t hz)
{
    if(hz > 800)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ;
    else if(hz > 400)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ;
    else if(hz > 200)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ;
    else if(hz > 100)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ;
    else if(hz > 50)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ;
    else if(hz > 25)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ;
    else if(hz > 12)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ;
    else if(hz > 6)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ;
    else if(hz > 3)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ;
    else if(hz >= 1)
        return BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ;
    else
        return BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ;

}

/**
 * This function sets the acc ODR
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_bosch_bmi160_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int     ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_bosch_bmi160_hz2odr(hz);

    ret = sensor_i2c_read(drv, BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG,
                          &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return ret;
    }

    value = BMI160_SET_BITSLICE(value, BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE,
                                odr);

    ret = sensor_i2c_write(drv, BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG,
                           &value, I2C_DATA_LEN, I2C_OP_RETRIES);
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
static int drv_acc_bosch_bmi160_set_range(i2c_dev_t* drv, uint32_t range)
{
    int     ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, BMI160_USER_ACCEL_RANGE__REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return ret;
    }

    switch(range) {
        case ACC_RANGE_2G: {
            tmp = BMI160_ACCEL_RANGE_2G;
        }
        break;

        case ACC_RANGE_4G: {
            tmp = BMI160_ACCEL_RANGE_4G;
        }
        break;

        case ACC_RANGE_8G: {
            tmp = BMI160_ACCEL_RANGE_8G;
        }
        break;

        case ACC_RANGE_16G: {
            tmp = BMI160_ACCEL_RANGE_16G;
        }
        break;

        default:
            break;
    }

    value  = BMI160_SET_BITSLICE(value, BMI160_USER_ACCEL_RANGE, tmp);
    ret = sensor_i2c_write(drv, BMI160_USER_ACCEL_RANGE__REG, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)) {
        g_cur_acc_factor = g_bmi160_acc_factor[range];
    }

    return 0;
}

/**
 * This function is the ISR
 *
 * @return
 */
static void drv_acc_bosch_bmi160_irq_handle(void)
{
    /* no handle so far */
}

/**
 * This function opens the acc
 *
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_bosch_bmi160_open(void)
{
    int ret = 0;

    ret  = drv_acc_bosch_bmi160_set_power_mode(&bmi160_ctx, DEV_POWER_ON);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_bosch_bmi160_set_range(&bmi160_ctx, ACC_RANGE_8G);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_bosch_bmi160_set_odr(&bmi160_ctx, BMI160_ACC_DEFAULT_ODR_25HZ);
    if(unlikely(ret) != 0) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

/**
 * This function closes the acc
 *
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_bosch_bmi160_close(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bmi160_set_power_mode(&bmi160_ctx, DEV_POWER_OFF);
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
static int drv_acc_bosch_bmi160_read(void *buf, size_t len)
{
    int          ret = 0;
    size_t       size;
    uint8_t      reg[6];
    accel_data_t *accel = (accel_data_t *)buf;
    if(buf == NULL) {
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size) {
        return -1;
    }

    ret  = sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_14_ACCEL_X_LSB__REG,
                           &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_15_ACCEL_X_MSB__REG,
                           &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_16_ACCEL_Y_LSB__REG,
                           &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_17_ACCEL_Y_MSB__REG,
                           &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_18_ACCEL_Z_LSB__REG,
                           &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_19_ACCEL_Z_MSB__REG,
                           &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return -1;
    }

    accel->data[DATA_AXIS_X] = (int16_t)((((int32_t)((int8_t)reg[1])) <<
                                          BMI160_SHIFT_BIT_POSITION_BY_08_BITS) | (reg[0]));
    accel->data[DATA_AXIS_Y] = (int16_t)((((int32_t)((int8_t)reg[3])) <<
                                          BMI160_SHIFT_BIT_POSITION_BY_08_BITS) | (reg[2]));
    accel->data[DATA_AXIS_Z] = (int16_t)((((int32_t)((int8_t)reg[5])) <<
                                          BMI160_SHIFT_BIT_POSITION_BY_08_BITS) | (reg[4]));

    if(g_cur_acc_factor != 0) {
        /* the unit of acc is mg, 1000 mg = 1 g */
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)g_cur_acc_factor;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)g_cur_acc_factor;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)g_cur_acc_factor;
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
static int drv_acc_bosch_bmi160_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bmi160_set_odr(&bmi160_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bmi160_set_range(&bmi160_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bmi160_set_power_mode(&bmi160_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t*)arg;
            info->model = "BMI160";
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
int drv_acc_bosch_bmi160_init(void) {
    printf("drv_acc_bosch_bmi160_init started \n");
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_bosch_bmi160_open;
    sensor.close      = drv_acc_bosch_bmi160_close;
    sensor.read       = drv_acc_bosch_bmi160_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_bosch_bmi160_ioctl;
    sensor.irq_handle = drv_acc_bosch_bmi160_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret) != 0) {
        printf("sensor_create_obj failed \n");
        return -1;
    }
    ret = drv_acc_gyro_bosch_bmi160_validate_id(&bmi160_ctx, BMI160_CHIP_ID_VALUE);
    if(unlikely(ret) != 0) {
        printf("drv_acc_gyro_bosch_bmi160_validate_id failed \n");
        return -1;
    }

    if(0 == g_bmi160flag) {
        ret = drv_acc_gyro_bosch_bmi160_soft_reset(&bmi160_ctx);
        if(unlikely(ret) != 0) {
            printf("drv_acc_gyro_bosch_bmi160_soft_reset failed \n");
            return -1;
        }
        ret = drv_acc_bosch_bmi160_set_power_mode(&bmi160_ctx, DEV_POWER_OFF);
        if(unlikely(ret) != 0) {
            printf("drv_acc_bosch_bmi160_set_power_mode failed \n");
            return -1;
        }
        g_bmi160flag = 1;
    } else {
        LOG("%s %s acc do not need reset\n", SENSOR_STR, __func__);
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    printf("drv_acc_bosch_bmi160_init  failed \n");
    return 0;
}

/**
 * This function sets the gyro powermode
 *
 * @param[in]  drv   pointer to the i2c dev
 * @param[in]  mode  the powermode to be setted
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi160_set_power_mode(i2c_dev_t* drv,
                                                dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret = 0;

    switch(mode) {
        case DEV_POWER_ON: {

            value = GYRO_MODE_NORMAL;
            ret = sensor_i2c_write(drv, BMI160_CMD_COMMANDS__REG, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }
        }
        break;

        case DEV_POWER_OFF:
        case DEV_SLEEP: {
            value = GYRO_MODE_SUSPEND;
            ret = sensor_i2c_write(drv, BMI160_CMD_COMMANDS__REG, &value, I2C_DATA_LEN,
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
 * This function gets the gyro ODR according to HZ
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the conrresponding gyro ODR
 */
static uint8_t drv_gyro_bosch_bmi160_hz2odr(uint32_t hz)
{
    if(hz > 1600)
        return BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ;
    else if(hz > 800)
        return BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ;
    else if(hz > 400)
        return BMI160_GYRO_OUTPUT_DATA_RATE_800HZ;
    else if(hz > 200)
        return BMI160_GYRO_OUTPUT_DATA_RATE_400HZ;
    else if(hz > 100)
        return BMI160_GYRO_OUTPUT_DATA_RATE_200HZ;
    else if(hz > 50)
        return BMI160_GYRO_OUTPUT_DATA_RATE_100HZ;
    else if(hz > 25)
        return BMI160_GYRO_OUTPUT_DATA_RATE_50HZ;
    else
        return BMI160_GYRO_OUTPUT_DATA_RATE_25HZ;

}

/**
 * This function sets the gyro ODR
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi160_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int     ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_gyro_bosch_bmi160_hz2odr(hz);

    ret = sensor_i2c_read(drv, BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG,
                          &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return ret;
    }

    value = BMI160_SET_BITSLICE(value, BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE,
                                odr);
    ret = sensor_i2c_write(drv, BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG,
                           &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        return ret;
    }
    return 0;
}

/**
 * This function sets the gyro range
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the range required
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi160_set_range(i2c_dev_t* drv, uint32_t range)
{
    int     ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, BMI160_USER_GYRO_RANGE__REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return ret;
    }

    switch(range) {
        case GYRO_RANGE_125DPS: {
            tmp = BMI160_GYRO_RANGE_125_DEG_SEC;
        }
        break;
        case GYRO_RANGE_250DPS: {
            tmp = BMI160_GYRO_RANGE_250_DEG_SEC;
        }
        break;

        case GYRO_RANGE_500DPS: {
            tmp = BMI160_GYRO_RANGE_500_DEG_SEC;
        }
        break;

        case GYRO_RANGE_1000DPS: {
            tmp = BMI160_GYRO_RANGE_1000_DEG_SEC;
        }
        break;

        case GYRO_RANGE_2000DPS: {
            tmp = BMI160_GYRO_RANGE_2000_DEG_SEC;
        }
        break;

        default:
            break;
    }

    value = BMI160_SET_BITSLICE(value, BMI160_USER_GYRO_RANGE, tmp);
    ret = sensor_i2c_write(drv, BMI160_USER_GYRO_RANGE__REG, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        return ret;
    }

    if((range >= GYRO_RANGE_125DPS)&&(range <= GYRO_RANGE_2000DPS)) {
        g_cur_gyro_factor = g_bmi160_gyro_factor[range];
    }

    return 0;
}

/**
 * This function is ISR
 *
 * @return
 */
static void drv_gyro_bosch_bmi160_irq_handle(void)
{
    /* no handle so far */
}

/**
 * This function opens the acc
 *
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi160_open(void)
{
    int ret = 0;
    ret  = drv_gyro_bosch_bmi160_set_power_mode(&bmi160_ctx, DEV_POWER_ON);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi160_set_range(&bmi160_ctx, GYRO_RANGE_125DPS);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi160_set_odr(&bmi160_ctx, BMI160_GYRO_DEFAULT_ODR_25HZ);
    if(unlikely(ret) != 0) {
        return -1;
    }

    return 0;

}

/**
 * This function closes the gyro
 *
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi160_close(void)
{
    int ret = 0;
    ret  = drv_gyro_bosch_bmi160_set_power_mode(&bmi160_ctx, DEV_POWER_OFF);
    if(unlikely(ret) != 0) {
        return -1;
    }
    return 0;
}

/**
 * This function reads the gyro data and reports the data
 *
 * @param[in out]  buf   buffer for gyro data
 * @param[in out]  len   length of data
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi160_read(void *buf, size_t len)
{
    int         ret = 0;
    size_t      size;
    uint8_t     reg[6];
    gyro_data_t *gyro = (gyro_data_t *)buf;
    if(buf == NULL) {
        return -1;
    }

    size = sizeof(gyro_data_t);
    if(len < size) {
        return -1;
    }

    ret  = sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_8_GYRO_X_LSB__REG,
                           &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_9_GYRO_X_MSB__REG,
                           &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_10_GYRO_Y_LSB__REG,
                           &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_11_GYRO_Y_MSB__REG,
                           &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_12_GYRO_Z_LSB__REG,
                           &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi160_ctx, BMI160_USER_DATA_13_GYRO_Z_MSB__REG,
                           &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return -1;
    }

    gyro->data[DATA_AXIS_X] = (int32_t)((((int16_t)((int8_t)reg[1])) <<
                                         BMI160_SHIFT_BIT_POSITION_BY_08_BITS) | (reg[0]));
    gyro->data[DATA_AXIS_Y] = (int32_t)((((int16_t)((int8_t)reg[3])) <<
                                         BMI160_SHIFT_BIT_POSITION_BY_08_BITS) | (reg[2]));
    gyro->data[DATA_AXIS_Z] = (int32_t)((((int16_t)((int8_t)reg[5])) <<
                                         BMI160_SHIFT_BIT_POSITION_BY_08_BITS) | (reg[4]));

    if(g_cur_gyro_factor != 0) {
        gyro->data[DATA_AXIS_X] = (int32_t)((int64_t)gyro->data[DATA_AXIS_X] * GYROSCOPE_UNIT_FACTOR /
                                  g_cur_gyro_factor);
        gyro->data[DATA_AXIS_Y] = (int32_t)((int64_t)gyro->data[DATA_AXIS_Y] * GYROSCOPE_UNIT_FACTOR /
                                  g_cur_gyro_factor);
        gyro->data[DATA_AXIS_Z] = (int32_t)((int64_t)gyro->data[DATA_AXIS_Z] * GYROSCOPE_UNIT_FACTOR /
                                  g_cur_gyro_factor);
    }
    gyro->timestamp = aos_now_ms();

    return (int)size;
}

/**
 * This function is for the gyro ioctl
 *
 * @param[in]  cmd   the ioctl command
 * @param[in]  arg   the correspondding parameter
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi160_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_gyro_bosch_bmi160_set_odr(&bmi160_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_gyro_bosch_bmi160_set_range(&bmi160_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_gyro_bosch_bmi160_set_power_mode(&bmi160_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMI160";
            info->range_max = 2000;
            info->range_min = 125;
            info->unit = udps;
        }
        break;

        default:
            break;
    }

    return 0;
}

/**
 * This function is for the gyro initialization
 *
 * @return  the operation status, 0 is OK, others is error
 */
int drv_gyro_bosch_bmi160_init(void) {
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_bosch_bmi160_open;
    sensor.close      = drv_gyro_bosch_bmi160_close;
    sensor.read       = drv_gyro_bosch_bmi160_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_bosch_bmi160_ioctl;
    sensor.irq_handle = drv_gyro_bosch_bmi160_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_gyro_bosch_bmi160_validate_id(&bmi160_ctx, BMI160_CHIP_ID_VALUE);
    if(unlikely(ret) != 0) {
        return -1;
    }

    if(0 == g_bmi160flag) {
        ret = drv_acc_gyro_bosch_bmi160_soft_reset(&bmi160_ctx);
        if(unlikely(ret) != 0) {
            return -1;
        }

        ret = drv_gyro_bosch_bmi160_set_power_mode(&bmi160_ctx, DEV_POWER_OFF);
        if(unlikely(ret) != 0) {
            return -1;
        }
        g_bmi160flag = 1;
    } else {
        LOG("%s %s gyro do not need reset\n", SENSOR_STR, __func__);
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_acc_bosch_bmi160_init);
SENSOR_DRV_ADD(drv_gyro_bosch_bmi160_init);