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
#define BMI260_I2C_ADDR_TRANS(n)      ((n)<<1)
#define BMI260_I2C_SLAVE_ADDR_LOW     0x68
#define BMI260_I2C_ADDR               BMI260_I2C_ADDR_TRANS(BMI260_I2C_SLAVE_ADDR_LOW)

/* Utility macros */
#define BMI2_SET_BITS(reg_data, bitname, data) \
				((reg_data & ~(bitname##_MASK)) | \
				((data << bitname##_POS) & bitname##_MASK))

#define BMI2_GET_BITS(reg_data, bitname) \
				((reg_data & (bitname##_MASK)) >> \
				(bitname##_POS))
#define BMI2_SET_BIT_POS0(reg_data, bitname, data) \
				((reg_data & ~(bitname##_MASK)) | \
				(data & bitname##_MASK))

#define BMI2_GET_BIT_POS0(reg_data, bitname) (reg_data & (bitname##_MASK))
#define BMI2_SET_BIT_VAL0(reg_data, bitname) (reg_data & ~(bitname##_MASK))

/* For getting LSB and MSB */
#define BMI2_GET_LSB(var)	(uint8_t)(var & BMI2_SET_LOW_BYTE)
#define BMI2_GET_MSB(var)	(uint8_t)((var & BMI2_SET_HIGH_BYTE) >> 8)

/* BMI2 register addresses */
#define BMI2_CHIP_ID_ADDR              UINT8_C(0x00)
#define BMI260_CHIP_ID                 UINT8_C(0x20)
#define BMI2_STATUS_ADDR               UINT8_C(0x03)
#define BMI2_AUX_X_LSB_ADDR            UINT8_C(0x04)
#define BMI2_ACC_X_LSB_ADDR            UINT8_C(0x0C)
#define BMI2_GYR_X_LSB_ADDR            UINT8_C(0x12)
#define BMI2_EVENT_ADDR                UINT8_C(0x1B)
#define BMI2_INT_STATUS_0_ADDR         UINT8_C(0x1C)
#define BMI2_INT_STATUS_1_ADDR         UINT8_C(0x1D)
#define BMI2_SYNC_COMMAND_ADDR         UINT8_C(0x1E)
#define BMI2_CONF_STATUS_ADDR          UINT8_C(0x21)
#define	BMI2_FIFO_LENGTH_0_ADDR        UINT8_C(0X24)
#define	BMI2_FIFO_DATA_ADDR            UINT8_C(0X26)
#define BMI2_PAGE_REG_ADDR             UINT8_C(0x2F)
#define BMI2_FEAT_START_ADDR           UINT8_C(0x30)
#define BMI2_ACC_CONF_ADDR             UINT8_C(0x40)
#define BMI2_ACC_CONF1_ADDR            UINT8_C(0x41)
#define BMI2_GYR_CONF_ADDR             UINT8_C(0x42)
#define BMI2_GYR_CONF1_ADDR            UINT8_C(0x43)
#define BMI2_AUX_CONF_ADDR             UINT8_C(0x44)
#define	BMI2_FIFO_DOWN_ADDR            UINT8_C(0X45)
#define	BMI2_FIFO_WTM_0_ADDR           UINT8_C(0X46)
#define	BMI2_FIFO_WTM_1_ADDR           UINT8_C(0X47)
#define	BMI2_FIFO_CONFIG_0_ADDR        UINT8_C(0X48)
#define	BMI2_FIFO_CONFIG_1_ADDR        UINT8_C(0X49)
#define BMI2_AUX_DEV_ID_ADDR           UINT8_C(0x4B)
#define BMI2_AUX_IF_CONF_ADDR          UINT8_C(0x4C)
#define BMI2_AUX_RD_ADDR               UINT8_C(0x4D)
#define BMI2_AUX_WR_ADDR               UINT8_C(0x4E)
#define BMI2_AUX_WR_DATA_ADDR          UINT8_C(0x4F)
#define	BMI2_INT1_IO_CTRL_ADDR         UINT8_C(0x53)
#define	BMI2_INT2_IO_CTRL_ADDR         UINT8_C(0x54)
#define	BMI2_INT1_MAP_ADDR             UINT8_C(0x56)
#define	BMI2_INT2_MAP_ADDR             UINT8_C(0x57)
#define	BMI2_INT_MAP_HW_ADDR           UINT8_C(0x58)
#define BMI2_CONF_CTRL_ADDR            UINT8_C(0x59)
#define BMI2_NEXT_LSB_ADDR             UINT8_C(0x5B)
#define BMI2_NEXT_MSB_ADDR             UINT8_C(0x5C)
#define	BMI2_CONF_LOAD_ADDR            UINT8_C(0x5E)
#define	BMI2_IF_CONF_ADDR              UINT8_C(0X6B)
#define	BMI2_ACC_SELF_TEST_ADDR        UINT8_C(0X6D)
#define	BMI2_GYR_OFF_COMP_3_ADDR       UINT8_C(0X74)
#define	BMI2_GYR_OFF_COMP_EN_ADDR      UINT8_C(0X77)
#define	BMI2_GYR_USR_GAIN_0_ADDR       UINT8_C(0X78)
#define	BMI2_PWR_CONF_ADDR             UINT8_C(0x7C)
#define BMI2_PWR_CTRL_ADDR             UINT8_C(0x7D)
#define BMI2_CMD_REG_ADDR              UINT8_C(0x7E)

/* BMI2 Commands */
#define BMI2_USER_DEF_CMD_0            UINT8_C(0x03)
#define BMI2_SOFT_RESET_CMD            UINT8_C(0xB6)
#define BMI2_FIFO_FLUSH_CMD            UINT8_C(0xB0)

/* Accelerometer Macro Definitions */
/* Accelerometer Bandwidth parameters */
#define BMI2_ACC_OSR4_AVG1             UINT8_C(0x00)
#define BMI2_ACC_OSR2_AVG2             UINT8_C(0x01)
#define	BMI2_ACC_NORMAL_AVG4           UINT8_C(0x02)
#define	BMI2_ACC_CIC_AVG8              UINT8_C(0x03)
#define	BMI2_ACC_RES_AVG16             UINT8_C(0x04)
#define	BMI2_ACC_RES_AVG32             UINT8_C(0x05)
#define	BMI2_ACC_RES_AVG64             UINT8_C(0x06)
#define	BMI2_ACC_RES_AVG128            UINT8_C(0x07)

/* Accelerometer Output Data Rate */
#define	BMI2_ACC_ODR_0_78HZ            UINT8_C(0x01)
#define	BMI2_ACC_ODR_1_56HZ            UINT8_C(0x02)
#define	BMI2_ACC_ODR_3_12HZ            UINT8_C(0x03)
#define	BMI2_ACC_ODR_6_25HZ            UINT8_C(0x04)
#define	BMI2_ACC_ODR_12_5HZ            UINT8_C(0x05)
#define	BMI2_ACC_ODR_25HZ              UINT8_C(0x06)
#define	BMI2_ACC_ODR_50HZ              UINT8_C(0x07)
#define	BMI2_ACC_ODR_100HZ             UINT8_C(0x08)
#define	BMI2_ACC_ODR_200HZ             UINT8_C(0x09)
#define	BMI2_ACC_ODR_400HZ             UINT8_C(0x0A)
#define	BMI2_ACC_ODR_800HZ             UINT8_C(0x0B)
#define	BMI2_ACC_ODR_1600HZ            UINT8_C(0x0C)

/* Accelerometer G Range */
#define	BMI2_ACC_RANGE_2G              UINT8_C(0x00)
#define	BMI2_ACC_RANGE_4G              UINT8_C(0x01)
#define	BMI2_ACC_RANGE_8G              UINT8_C(0x02)
#define	BMI2_ACC_RANGE_16G             UINT8_C(0x03)

/* Mask definitions for accelerometer configuration register */
#define BMI2_ACC_RANGE_MASK            UINT8_C(0x03)
#define BMI2_ACC_ODR_MASK              UINT8_C(0x0F)
#define BMI2_ACC_BW_PARAM_MASK         UINT8_C(0x70)
#define BMI2_ACC_PERF_MODE_MASK        UINT8_C(0x80)

/* Bit position definitions for accelerometer configuration register */
#define BMI2_ACC_BW_PARAM_POS          UINT8_C(0x04)
#define BMI2_ACC_PERF_MODE_POS         UINT8_C(0x07)

/* Self test macro to show resulting minimum difference signal in mg */
#define	BMI2_ST_ACC_X_AXIS_SIGNAL_DIFF UINT16_C(1800)
#define	BMI2_ST_ACC_Y_AXIS_SIGNAL_DIFF UINT16_C(1800)
#define	BMI2_ST_ACC_Z_AXIS_SIGNAL_DIFF UINT16_C(1800)

/* Mask definitions for accelerometer self-test */
#define	BMI2_ACC_SELF_TEST_EN_MASK     UINT8_C(0x01)
#define	BMI2_ACC_SELF_TEST_SIGN_MASK   UINT8_C(0x04)
#define	BMI2_ACC_SELF_TEST_AMP_MASK    UINT8_C(0x08)

/* Bit Positions for accelerometer self-test */
#define	BMI2_ACC_SELF_TEST_SIGN_POS    UINT8_C(0x02)
#define	BMI2_ACC_SELF_TEST_AMP_POS     UINT8_C(0x03)
/* Gyroscope Macro Definitions */
/* Gyroscope sense drive and low power mode */
/* Senses low power mode */
#define BMI2_GYR_LOW_POWER_MODE        UINT8_C(0x00)
/* Select drive */
#define BMI2_GYR_HIGH_PERF_MODE        UINT8_C(0x01)

/* Gyroscope Bandwidth parameters */
#define BMI2_GYR_OSR4_MODE             UINT8_C(0x00)
#define BMI2_GYR_OSR2_MODE             UINT8_C(0x01)
#define	BMI2_GYR_NORMAL_MODE           UINT8_C(0x02)
#define	BMI2_GYR_CIC_MODE              UINT8_C(0x03)

/* Gyroscope Output Data Rate */
#define	BMI2_GYR_ODR_25HZ              UINT8_C(0x06)
#define	BMI2_GYR_ODR_50HZ              UINT8_C(0x07)
#define	BMI2_GYR_ODR_100HZ             UINT8_C(0x08)
#define	BMI2_GYR_ODR_200HZ             UINT8_C(0x09)
#define	BMI2_GYR_ODR_400HZ             UINT8_C(0x0A)
#define	BMI2_GYR_ODR_800HZ             UINT8_C(0x0B)
#define	BMI2_GYR_ODR_1600HZ            UINT8_C(0x0C)
#define	BMI2_GYR_ODR_3200HZ            UINT8_C(0x0D)

/* Gyroscope OIS Range */
#define	BMI2_GYR_OIS_125               UINT8_C(0x00)
#define	BMI2_GYR_OIS_2000              UINT8_C(0x01)

/* Gyroscope Angular Rate Measurement Range */
#define	BMI2_GYR_RANGE_2000            UINT8_C(0x00)
#define	BMI2_GYR_RANGE_1000            UINT8_C(0x01)
#define	BMI2_GYR_RANGE_500             UINT8_C(0x02)
#define	BMI2_GYR_RANGE_250             UINT8_C(0x03)
#define	BMI2_GYR_RANGE_125             UINT8_C(0x04)

/* Mask definitions for gyroscope configuration register */
#define BMI2_GYR_RANGE_MASK            UINT8_C(0x07)
#define BMI2_GYR_OIS_RANGE_MASK        UINT8_C(0x08)
#define BMI2_GYR_ODR_MASK              UINT8_C(0x0F)
#define BMI2_GYR_BW_PARAM_MASK         UINT8_C(0x30)
#define BMI2_GYR_DSLP_MASK             UINT8_C(0x40)
#define BMI2_GYR_PERF_MODE_MASK        UINT8_C(0x80)

/* Bit position definitions for gyroscope configuration register */
#define BMI2_GYR_OIS_RANGE_POS         UINT8_C(0x03)
#define BMI2_GYR_BW_PARAM_POS          UINT8_C(0x04)
#define BMI2_GYR_DSLP_POS              UINT8_C(0x06)
#define BMI2_GYR_PERF_MODE_POS         UINT8_C(0x07)

/* default HZ */
#define BMI260_ACC_DEFAULT_ODR_100HZ   (100)
#define BMI260_ACC_DEFAULT_ODR_25HZ    (25)
#define BMI260_GYRO_DEFAULT_ODR_100HZ  (100)
#define BMI260_GYRO_DEFAULT_ODR_25HZ   (25)

/* Mask definitions for power control register */
#define BMI2_AUX_EN_MASK               UINT8_C(0x01)
#define BMI2_GYR_EN_MASK               UINT8_C(0x02)
#define BMI2_ACC_EN_MASK               UINT8_C(0x04)
#define BMI2_TEMP_EN_MASK              UINT8_C(0x08)

/* Bit position definitions for power control register */
#define BMI2_GYR_EN_POS                UINT8_C(0x01)
#define BMI2_ACC_EN_POS                UINT8_C(0x02)
#define BMI2_TEMP_EN_POS               UINT8_C(0x03)

/* Mask definitions for power configuration register */
#define BMI2_ADV_POW_EN_MASK           UINT8_C(0x01)

/* Mask definitions for sensor event flags */
#define BMI2_EVENT_FLAG_MASK           UINT8_C(0x1C)

/* Bit position definitions for sensor event flags */
#define BMI2_EVENT_FLAG_POS            UINT8_C(0x02)

/* BMI2 sensor data bytes */
#define BMI2_ACC_GYR_NUM_BYTES         UINT8_C(6)
#define	BMI2_AUX_NUM_BYTES             UINT8_C(8)

/* For enable and disable */
#define BMI2_ENABLE                    UINT8_C(1)
#define BMI2_DISABLE                   UINT8_C(0)

/* To define success code */
#define BMI2_OK                        INT8_C(0)

/* To define error codes */
#define BMI2_E_NULL_PTR                INT8_C(-1)
#define BMI2_E_COM_FAIL                INT8_C(-2)
#define BMI2_E_DEV_NOT_FOUND           INT8_C(-3)
#define BMI2_E_OUT_OF_RANGE            INT8_C(-4)
#define BMI2_E_ACC_INVALID_CFG         INT8_C(-5)
#define	BMI2_E_GYRO_INVALID_CFG        INT8_C(-6)
#define	BMI2_E_ACC_GYR_INVALID_CFG     INT8_C(-7)
#define	BMI2_E_INVALID_SENSOR          INT8_C(-8)
#define BMI2_E_CONFIG_LOAD             INT8_C(-9)
#define	BMI2_E_INVALID_PAGE            INT8_C(-11)
#define	BMI2_E_INVALID_FEAT_INT        INT8_C(-12)
#define	BMI2_E_INVALID_INT_PIN         INT8_C(-13)
#define	BMI2_E_SET_APS_FAIL            INT8_C(-14)
#define	BMI2_E_AUX_INVALID_CFG         INT8_C(-15)
#define	BMI2_E_AUX_BUSY                INT8_C(-16)
#define BMI2_E_SELF_TEST_FAIL          INT8_C(-17)
#define BMI2_E_REMAP_ERROR             INT8_C(-18)
#define BMI2_E_GYR_USER_GAIN_UPD_FAIL  INT8_C(-19)

enum bmi2_sensor_config_error_e {
    BMI2_NO_ERROR,
    BMI2_ACC_ERROR,
    BMI2_GYR_ERROR,
    BMI2_ACC_GYR_ERROR
};

i2c_dev_t bmi260_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMI260_I2C_ADDR,
};

static int     g_gyro_active_count = 0;
static int     g_acc_active_count = 0;

static int32_t g_bmi260_acc_factor[ACC_RANGE_MAX] = { 16384, 8192, 4096, 2048 };
static int32_t g_bmi260_gyro_factor[GYRO_RANGE_MAX] = {262, 131, 65, 33, 16 };

static int32_t g_cur_acc_factor = 0;
static int32_t g_cur_gyro_factor = 0;
static int32_t g_bmi260flag = 0;

/**
 * This function shows the error status when illegal sensor
 * configuration is set.
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int8_t cfg_error_status(i2c_dev_t* drv)
{
    int8_t  rslt = 0;  /* Variable to define error */
    int     ret;
    uint8_t reg_data; /* Variable to store data */

    /* Get error status of the set sensor configuration */
    ret = sensor_i2c_read(drv, BMI2_EVENT_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read CHIPID failed \n");
        rslt = BMI2_E_COM_FAIL;
        return rslt;
    }
    reg_data = BMI2_GET_BITS(reg_data, BMI2_EVENT_FLAG);

    switch(reg_data) {
        case BMI2_NO_ERROR:
            rslt = BMI2_OK;
            break;
        case BMI2_ACC_ERROR:
            rslt = BMI2_E_ACC_INVALID_CFG;
            break;
        case BMI2_GYR_ERROR:
            rslt = BMI2_E_GYRO_INVALID_CFG;
            break;
        case BMI2_ACC_GYR_ERROR:
            rslt = BMI2_E_ACC_GYR_INVALID_CFG;
            break;
        default:
            break;
    }

    return rslt;
}

/**
 * This function does the soft reset
 *
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_gyro_bosch_bmi260_soft_reset(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;
    value = BMI2_SOFT_RESET_CMD;

    ret = sensor_i2c_write(drv, BMI2_CMD_REG_ADDR, &value, I2C_DATA_LEN,
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
static int drv_acc_gyro_bosch_bmi260_validate_id(i2c_dev_t* drv,
                                                 uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret = 0;

    if(drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMI2_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read CHIPID failed \n");
        return ret;
    }
    printf("!!!!!!read CHIPID  %x\n", value);
    if(id_value != (value & 0x20)) {
        printf("!!!!!failed read CHIPID  %x\n", value);
        return -1;
    }

    return 0;
}

/**
 * This function enables the acc and gyro
 *
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_gyro_bosch_bmi260_enable(i2c_dev_t* drv)
{
    int     ret = 0;

    uint8_t reg_data = 0;   /* Variable to store register values */
    uint8_t aps_status = 0; /* Variable to store adv power status */
    int8_t  rslt = 0;        /* Variable to define error */

    if(drv == NULL) {
        return -1;
    }

    /* first enable ACC and GYRO */
    ret = sensor_i2c_read(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CTRL failed \n");
        return ret;
    }

    /* enable ACC */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_ACC_EN, BMI2_ENABLE);
    /* enable GYRO */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_EN, BMI2_ENABLE);

    ret = sensor_i2c_write(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write PWR_CTRL failed \n");
        return ret;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    /* get ACC_CONFIG */
    ret = sensor_i2c_read(drv, BMI2_ACC_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_ACC_CONF_ADDR failed \n");
        return ret;
    }
    /* enable accelerometer performance mode */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_ACC_PERF_MODE, BMI2_ENABLE);

    /* Write accelerometer configuration to ACC_CONF */
    ret = sensor_i2c_write(drv, BMI2_ACC_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_ACC_CONF_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting ACC failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    /* get GYRO_CONFIG */
    ret = sensor_i2c_read(drv, BMI2_GYR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }
    /* enable gyroscope performance mode */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_PERF_MODE, BMI2_ENABLE);

    /* disable gyroscope high performance/low-power mode */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_DSLP, BMI2_DISABLE);

    /* Write accelerometer configuration to GYR_CONF */
    ret = sensor_i2c_write(drv, BMI2_GYR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting GYRO failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    return 0;

}

/**
 * This function disables the acc and gyro
 *
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_gyro_bosch_bmi260_disable(i2c_dev_t* drv)
{
    int     ret = 0;

    uint8_t reg_data = 0;   /* Variable to store register values */

    uint8_t aps_status = 0; /* Variable to store adv power status */

    if(drv == NULL) {
        return -1;
    }

    /* first disable ACC and GYRO */
    ret = sensor_i2c_read(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CTRL failed \n");
        return ret;
    }

    /* disable ACC */
    reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_ACC_EN);
    /* disable GYRO */
    reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_GYR_EN);

    ret = sensor_i2c_write(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write PWR_CTRL failed \n");
        return ret;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_DISABLE)) {
        /* enable advance power save if disabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    return 0;

}

/**
 * This function enables the acc
 *
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_bosch_bmi260_enable(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t reg_data = 0;   /* Variable to store register values */
    uint8_t aps_status = 0; /* Variable to store adv power status */
    int8_t  rslt = 0;       /* Variable to define error */

    if(drv == NULL) {
        return -1;
    }

    /* first enable ACC and GYRO */
    ret = sensor_i2c_read(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CTRL failed \n");
        return ret;
    }

    /* enable ACC */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_ACC_EN, BMI2_ENABLE);
    /* enable GYRO */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_EN, BMI2_DISABLE);

    ret = sensor_i2c_write(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write PWR_CTRL failed \n");
        return ret;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    /* get ACC_CONFIG */
    ret = sensor_i2c_read(drv, BMI2_ACC_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_ACC_CONF_ADDR failed \n");
        return ret;
    }
    /* enable accelerometer performance mode */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_ACC_PERF_MODE, BMI2_ENABLE);

    /* Write accelerometer configuration to ACC_CONF */
    ret = sensor_i2c_write(drv, BMI2_ACC_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_ACC_CONF_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting ACC failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
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
static int drv_acc_bosch_bmi260_set_power_mode(i2c_dev_t* drv,
                                               dev_power_mode_e mode)
{
    int ret = 0;

    switch(mode) {
        case DEV_POWER_ON: {
            if(g_gyro_active_count > 0) {
                /* use IMU setting */
                ret = drv_acc_gyro_bosch_bmi260_enable(drv);
            } else if(g_gyro_active_count == 0) {
                /* use ACC only setting */
                ret = drv_acc_bosch_bmi260_enable(drv);
            } else  {
                printf("g_gyro_active_count error gyro_active_count  %d\n",
                       g_gyro_active_count);
                ret = -1;
            }

            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            } else {
                if(g_acc_active_count == 0)
                    g_acc_active_count++;
            }
        }
        break;

        case DEV_POWER_OFF:
        case DEV_SLEEP: {

            if(g_gyro_active_count >= 0) {
                ret = drv_acc_gyro_bosch_bmi260_disable(drv);
            } else  {
                printf("g_gyro_active_count error g_gyro_active_count  %d\n",
                       g_gyro_active_count);
                ret = -1;
            }
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            } else {
                if(g_acc_active_count > 0)
                    g_acc_active_count = 0;
            }
        }
        break;
        default:
            break;
    }
    return 0;
}

/**
 * This function gets the XYZ data of acc
 *
 * @param[in]      drv  pointer to the i2c dev
 * @param[in out]  x    pointer to the acc x data
 * @param[in out]  y    pointer to the acc y data
 * @param[in out]  z    pointer to the acc z data
 * @return  the operation status, 0 is OK, others is error
 */
static uint8_t drv_acc_bosch_bmi260_getXYZ(i2c_dev_t* drv, int16_t* x,
                                           int16_t* y, int16_t* z)
{
    int      ret = 0;
    uint8_t  msb;                                    /* Variables to store msb value */
    uint8_t  lsb;                                    /* Variables to store lsb value */
    uint16_t msb_lsb;                                /* Variables to store both msb and lsb value */
    uint8_t  index = 0;                              /* Variables to define index */
    uint8_t  reg_data[BMI2_ACC_GYR_NUM_BYTES] = {0}; /* Array to define data stored in register */

    ret = sensor_i2c_read(drv, BMI2_ACC_X_LSB_ADDR, &reg_data[0],
                          BMI2_ACC_GYR_NUM_BYTES, I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }

    /* Read x-axis data */
    lsb = reg_data[index++];
    msb = reg_data[index++];
    msb_lsb = ((uint16_t)msb << 8) | (uint16_t)lsb;
    *x = (int16_t)msb_lsb;

    /* Read y-axis data */
    lsb = reg_data[index++];
    msb = reg_data[index++];
    msb_lsb = ((uint16_t)msb << 8) | (uint16_t)lsb;
    *y = (int16_t)msb_lsb;

    /* Read z-axis data */
    lsb = reg_data[index++];
    msb = reg_data[index++];
    msb_lsb = ((uint16_t)msb << 8) | (uint16_t)lsb;
    *z = (int16_t)msb_lsb;

    return 0;
}

/**
 * This function gets the acc ODR according to HZ
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the conrresponding acc ODR
 */
static uint8_t drv_acc_bosch_bmi260_hz2odr(uint32_t hz)
{
    if(hz > 800)
        return BMI2_ACC_ODR_1600HZ;
    else if(hz > 400)
        return BMI2_ACC_ODR_800HZ;
    else if(hz > 200)
        return BMI2_ACC_ODR_400HZ;
    else if(hz > 100)
        return BMI2_ACC_ODR_200HZ;
    else if(hz > 50)
        return BMI2_ACC_ODR_100HZ;
    else if(hz > 25)
        return BMI2_ACC_ODR_50HZ;
    else if(hz > 12)
        return BMI2_ACC_ODR_25HZ;
    else if(hz > 6)
        return BMI2_ACC_ODR_12_5HZ;
    else if(hz > 3)
        return BMI2_ACC_ODR_6_25HZ;
    else if(hz >= 1)
        return BMI2_ACC_ODR_3_12HZ;
    else
        return BMI2_ACC_ODR_1_56HZ;

}

/**
 * This function sets the acc ODR
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_bosch_bmi260_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int     ret = 0;
    uint8_t odr = drv_acc_bosch_bmi260_hz2odr(hz);
    uint8_t reg_data = 0;   /* Variable to store register values */
    uint8_t aps_status = 0; /* Variable to store adv power status */
    int8_t  rslt = 0;       /* Variable to define error */

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }


    /* set odr nomatter what working mode */
    /* get ACC_CONFIG */
    ret = sensor_i2c_read(drv, BMI2_ACC_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_ACC_CONF_ADDR failed \n");
        return ret;
    }
    /* Set accelerometer ODR */
    reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ACC_ODR, odr);

    /* Write accelerometer configuration to ACC_CONF */
    ret = sensor_i2c_write(drv, BMI2_ACC_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_ACC_CONF_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting ACC failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
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
static int drv_acc_bosch_bmi260_set_range(i2c_dev_t* drv, uint32_t range)
{
    int     ret = 0;
    uint8_t tmp = 0;
    uint8_t reg_data = 0;   /* Variable to store register values */
    uint8_t aps_status = 0; /* Variable to store adv power status */
    int8_t  rslt = 0;       /* Variable to define error */

    if(unlikely(ret) != 0) {
        return ret;
    }

    switch(range) {
        case ACC_RANGE_2G: {
            tmp = BMI2_ACC_RANGE_2G;
        }
        break;

        case ACC_RANGE_4G: {
            tmp = BMI2_ACC_RANGE_4G;
        }
        break;

        case ACC_RANGE_8G: {
            tmp = BMI2_ACC_RANGE_8G;
        }
        break;

        case ACC_RANGE_16G: {
            tmp = BMI2_ACC_RANGE_16G;
        }
        break;

        default:
            break;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }
    /* set range nomatter what working mode */
    /* get ACC_CONFIG1 */
    ret = sensor_i2c_read(drv, BMI2_ACC_CONF1_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_ACC_CONF1_ADDR failed \n");
        return ret;
    }

    /* Set accelerometer range */
    reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ACC_RANGE, tmp);

    /* Write accelerometer configuration to ACC_CONF1 */
    ret = sensor_i2c_write(drv, BMI2_ACC_CONF1_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_ACC_CONF_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting ACC failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)) {
        g_cur_acc_factor = g_bmi260_acc_factor[range];
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    return 0;
}

/**
 * This function is the ISR
 *
 * @return
 */
static void drv_acc_bosch_bmi260_irq_handle(void)
{
    /* no handle so far */
}

/**
 * This function opens the acc
 *
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_acc_bosch_bmi260_open(void)
{
    int ret = 0;

    ret  = drv_acc_bosch_bmi260_set_power_mode(&bmi260_ctx, DEV_POWER_ON);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_bosch_bmi260_set_range(&bmi260_ctx, ACC_RANGE_8G);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_bosch_bmi260_set_odr(&bmi260_ctx, BMI260_ACC_DEFAULT_ODR_25HZ);
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
static int drv_acc_bosch_bmi260_close(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bmi260_set_power_mode(&bmi260_ctx, DEV_POWER_OFF);
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
static int drv_acc_bosch_bmi260_read(void *buf, size_t len)
{
    int          ret = 0;
    int          ret_getXYZ = 0;
    size_t       size;
    uint8_t      reg_data = 0;   /* Variable to store register values */
    uint8_t      aps_status = 0; /* Variable to store adv power status */
    int16_t     x = 0;
    int16_t     y = 0;
    int16_t     z = 0;
    accel_data_t *accel = (accel_data_t *)buf;
    if(buf == NULL) {
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size) {
        return -1;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    ret = drv_acc_bosch_bmi260_getXYZ(&bmi260_ctx, &x, &y, &z);
    if(unlikely(ret)) {
        printf("read ACC XYZ failed \n");
        ret_getXYZ = ret;
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    if(unlikely(ret_getXYZ)) {
        return -1;
    }

    if(g_cur_acc_factor != 0) {
        /* the unit of acc is mg, 1000 mg = 1 g */
        accel->data[DATA_AXIS_X] = x * ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)g_cur_acc_factor;
        accel->data[DATA_AXIS_Y] = y * ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)g_cur_acc_factor;
        accel->data[DATA_AXIS_Z] = z * ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)g_cur_acc_factor;
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
static int drv_acc_bosch_bmi260_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bmi260_set_odr(&bmi260_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bmi260_set_range(&bmi260_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bmi260_set_power_mode(&bmi260_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t*)arg;
            info->model = "BMI260";
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
int drv_acc_bosch_bmi260_init(void) {
    printf("drv_acc_bosch_bmi260_init started \n");
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_bosch_bmi260_open;
    sensor.close      = drv_acc_bosch_bmi260_close;
    sensor.read       = drv_acc_bosch_bmi260_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_bosch_bmi260_ioctl;
    sensor.irq_handle = drv_acc_bosch_bmi260_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret) != 0) {
        printf("sensor_create_obj failed \n");
        return -1;
    }
    ret = drv_acc_gyro_bosch_bmi260_validate_id(&bmi260_ctx, BMI260_CHIP_ID);
    if(unlikely(ret) != 0) {
        printf("drv_acc_gyro_bosch_bmi260_validate_id failed \n");
        return -1;
    }

    if(0 == g_bmi260flag) {
        ret = drv_acc_gyro_bosch_bmi260_soft_reset(&bmi260_ctx);
        if(unlikely(ret) != 0) {
            printf("drv_acc_gyro_bosch_bmi260_soft_reset failed \n");
            return -1;
        }
        ret = drv_acc_bosch_bmi260_set_power_mode(&bmi260_ctx, DEV_POWER_OFF);
        if(unlikely(ret) != 0) {
            printf("drv_acc_bosch_bmi260_set_power_mode failed \n");
            return -1;
        }
        g_bmi260flag = 1;
    } else {
        LOG("%s %s acc do not need reset\n", SENSOR_STR, __func__);
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

/**
 * This function enables the gyro
 *
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi260_enable(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t reg_data = 0;   /* Variable to store register values */
    uint8_t aps_status = 0; /* Variable to store adv power status */
    int8_t  rslt = 0;       /* Variable to define error */

    if(drv == NULL) {
        return -1;
    }

    /* first enable ACC and GYRO */
    ret = sensor_i2c_read(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CTRL failed \n");
        return ret;
    }

    /* enable ACC */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_ACC_EN, BMI2_DISABLE);
    /* enable GYRO */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_EN, BMI2_ENABLE);

    ret = sensor_i2c_write(drv, BMI2_PWR_CTRL_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write PWR_CTRL failed \n");
        return ret;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    /* get GYRO_CONFIG */
    ret = sensor_i2c_read(drv, BMI2_GYR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }
    /* enable gyroscope performance mode */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_PERF_MODE, BMI2_ENABLE);

    /* disable gyroscope high performance/low-power mode */
    reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_DSLP, BMI2_DISABLE);

    /* Write accelerometer configuration to GYR_CONF */
    ret = sensor_i2c_write(drv, BMI2_GYR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting GYRO failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    return 0;
}

/**
 * This function sets the gyro powermode
 *
 * @param[in]  drv   pointer to the i2c dev
 * @param[in]  mode  the powermode to be setted
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi260_set_power_mode(i2c_dev_t* drv,
                                                dev_power_mode_e mode)
{
    int     ret = 0;

    switch(mode) {
        case DEV_POWER_ON: {

            if(g_acc_active_count > 0) {
                /*use IMU setting*/
                ret = drv_acc_gyro_bosch_bmi260_enable(drv);
            } else if(g_acc_active_count == 0) {
                /*use gyro only setting*/
                ret = drv_gyro_bosch_bmi260_enable(drv);
            } else {
                printf("g_acc_active_count error g_acc_active_count  %d\n", g_acc_active_count);
                ret = -1;
            }

            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            } else {
                if(g_gyro_active_count == 0)
                    g_gyro_active_count++;
            }
        }
        break;

        case DEV_POWER_OFF:
        case DEV_SLEEP: {
            if(g_acc_active_count >= 0) {
                ret = drv_acc_gyro_bosch_bmi260_disable(drv);
            }  else  {
                printf("g_acc_active_count error g_acc_active_count  %d\n", g_acc_active_count);
                ret = -1;
            }
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            } else {
                if(g_gyro_active_count > 0)
                    g_gyro_active_count = 0;
            }
        }
        break;
        default:
            break;
    }
    return 0;
}

/**
 * This function gets the XYZ data of gyro
 *
 * @param[in]      drv  pointer to the i2c dev
 * @param[in out]  x    pointer to the gyro x data
 * @param[in out]  y    pointer to the gyro y data
 * @param[in out]  z    pointer to the gyro z data
 * @return  the operation status, 0 is OK, others is error
 */
static uint8_t drv_gyro_bosch_bmi260_getXYZ(i2c_dev_t* drv, int16_t* x,
                                            int16_t* y, int16_t* z)
{
    int     ret = 0;
    uint8_t  msb;                                    /* Variables to store msb value */
    uint8_t  lsb;                                    /* Variables to store lsb value */
    uint16_t msb_lsb;                                /* Variables to store both msb and lsb value */
    uint8_t  index = 0;                              /* Variables to define index */
    uint8_t  reg_data[BMI2_ACC_GYR_NUM_BYTES] = {0}; /* Array to define data stored in register */

    ret = sensor_i2c_read(drv, BMI2_GYR_X_LSB_ADDR, &reg_data[0],
                          BMI2_ACC_GYR_NUM_BYTES, I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }

    /* Read x-axis data */
    lsb = reg_data[index++];
    msb = reg_data[index++];
    msb_lsb = ((uint16_t)msb << 8) | (uint16_t)lsb;
    *x = (int16_t)msb_lsb;

    /* Read y-axis data */
    lsb = reg_data[index++];
    msb = reg_data[index++];
    msb_lsb = ((uint16_t)msb << 8) | (uint16_t)lsb;
    *y = (int16_t)msb_lsb;

    /* Read z-axis data */
    lsb = reg_data[index++];
    msb = reg_data[index++];
    msb_lsb = ((uint16_t)msb << 8) | (uint16_t)lsb;
    *z = (int16_t)msb_lsb;

    return 0;

}

/**
 * This function gets the gyro ODR according to HZ
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the conrresponding gyro ODR
 */
static uint8_t drv_gyro_bosch_bmi260_hz2odr(uint32_t hz)
{
    if(hz > 1600)
        return BMI2_GYR_ODR_3200HZ ;
    else if(hz > 800)
        return BMI2_GYR_ODR_1600HZ;
    else if(hz > 400)
        return BMI2_GYR_ODR_800HZ;
    else if(hz > 200)
        return BMI2_GYR_ODR_400HZ;
    else if(hz > 100)
        return BMI2_GYR_ODR_200HZ;
    else if(hz > 50)
        return BMI2_GYR_ODR_100HZ;
    else if(hz > 25)
        return BMI2_GYR_ODR_50HZ;
    else
        return BMI2_GYR_ODR_25HZ;

}

/**
 * This function sets the gyro ODR
 *
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi260_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int     ret = 0;
    uint8_t odr = drv_gyro_bosch_bmi260_hz2odr(hz);
    uint8_t reg_data = 0;   /* Variable to store register values */
    uint8_t aps_status = 0; /* Variable to store adv power status */
    int8_t  rslt = 0;       /* Variable to define error */

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    /* set gyro ODR setting */

    /*get GYRO_CONFIG*/
    ret = sensor_i2c_read(drv, BMI2_GYR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }

    /* Set gyroscope ODR */
    reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_GYR_ODR, odr);

    /* Write  configuration to GYR_CONF */
    ret = sensor_i2c_write(drv, BMI2_GYR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_GYR_CONF_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting GYRO failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
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
static int drv_gyro_bosch_bmi260_set_range(i2c_dev_t* drv, uint32_t range)
{
    int     ret = 0;
    uint8_t tmp = 0;
    uint8_t reg_data = 0;   /* Variable to store register values */
    uint8_t aps_status = 0; /* Variable to store adv power status */
    int8_t  rslt = 0;       /* Variable to define error */

    switch(range) {
        case GYRO_RANGE_125DPS: {
            tmp = BMI2_GYR_RANGE_125;
        }
        break;
        case GYRO_RANGE_250DPS: {
            tmp = BMI2_GYR_RANGE_250;
        }
        break;

        case GYRO_RANGE_500DPS: {
            tmp = BMI2_GYR_RANGE_500;
        }
        break;

        case GYRO_RANGE_1000DPS: {
            tmp = BMI2_GYR_RANGE_1000;
        }
        break;

        case GYRO_RANGE_2000DPS: {
            tmp = BMI2_GYR_RANGE_2000;
        }
        break;

        default:
            break;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    /* set gyro range setting */
    /* get GYRO_CONFIG1 */
    ret = sensor_i2c_read(drv, BMI2_GYR_CONF1_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read BMI2_GYR_CONF1_ADDR failed \n");
        return ret;
    }

    /* Set gyroscope range */
    reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_GYR_RANGE, tmp);

    /* Write  configuration to GYR_CONF1 */
    ret = sensor_i2c_write(drv, BMI2_GYR_CONF1_ADDR, &reg_data, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(3);
    if(unlikely(ret) != 0) {
        printf("write BMI2_GYR_CONF1_ADDR failed \n");
        return ret;
    }

    /* Get error status to check for invalid configurations */
    rslt = cfg_error_status(drv);
    if(rslt != BMI2_OK) {
        printf("setting GYRO failed, rslt = %d\n", rslt);
        ret = -1;
        return ret;
    }

    if((range >= GYRO_RANGE_250DPS)&&(range <= GYRO_RANGE_2000DPS)) {
        g_cur_gyro_factor = g_bmi260_gyro_factor[range];
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(drv, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    return 0;
}

/**
 * This function is ISR
 *
 * @return
 */
static void drv_gyro_bosch_bmi260_irq_handle(void)
{
    /* no handle so far */
}

/**
 * This function opens the acc
 *
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_gyro_bosch_bmi260_open(void)
{
    int ret = 0;
    ret  = drv_gyro_bosch_bmi260_set_power_mode(&bmi260_ctx, DEV_POWER_ON);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi260_set_range(&bmi260_ctx, GYRO_RANGE_1000DPS);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi260_set_odr(&bmi260_ctx, BMI260_GYRO_DEFAULT_ODR_25HZ);
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
static int drv_gyro_bosch_bmi260_close(void)
{
    int ret = 0;
    ret  = drv_gyro_bosch_bmi260_set_power_mode(&bmi260_ctx, DEV_POWER_OFF);
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
static int drv_gyro_bosch_bmi260_read(void *buf, size_t len)
{
    int         ret = 0;
    int         ret_getXYZ = 0;
    size_t      size;
    int16_t     x = 0;
    int16_t     y = 0;
    int16_t     z = 0;
    uint8_t     reg_data = 0;   /* Variable to store register values */
    uint8_t     aps_status = 0; /* Variable to store adv power status */
    gyro_data_t *gyro = (gyro_data_t *)buf;
    if(buf == NULL) {
        return -1;
    }

    size = sizeof(gyro_data_t);
    if(len < size) {
        return -1;
    }

    /* Get status of advance power save mode */
    ret = sensor_i2c_read(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        printf("read PWR_CONF_ADDR failed \n");
        return ret;
    }
    aps_status = BMI2_GET_BIT_POS0(reg_data, BMI2_ADV_POW_EN);
    if((aps_status == BMI2_ENABLE)) {
        /* Disable advance power save if enabled */
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_DISABLE);
        ret = sensor_i2c_write(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {
            printf("write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    ret = drv_gyro_bosch_bmi260_getXYZ(&bmi260_ctx, &x, &y, &z);
    if(unlikely(ret)) {
        printf("!!!!read gyro XYZ failed \n");
        ret_getXYZ = ret;
    }

    /* enable advance power save if needed */
    if(aps_status == BMI2_ENABLE) {
        /* Get status of advance power save mode */
        ret = sensor_i2c_read(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                              I2C_OP_RETRIES);
        if(unlikely(ret) != 0) {
            printf("!!!!read PWR_CONF_ADDR failed \n");
            return ret;
        }
        reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_ADV_POW_EN, BMI2_ENABLE);
        ret = sensor_i2c_write(&bmi260_ctx, BMI2_PWR_CONF_ADDR, &reg_data, I2C_DATA_LEN,
                               I2C_OP_RETRIES);
        aos_msleep(3);
        if(unlikely(ret) != 0) {

            printf("!!!!!write PWR_CONF_ADDR failed \n");
            return ret;
        }
    }

    if(unlikely(ret_getXYZ)) {
        return -1;
    }

    if(g_cur_gyro_factor != 0) {
        gyro->data[DATA_AXIS_X] = (int32_t)((int64_t)x * GYROSCOPE_UNIT_FACTOR / (int64_t)g_cur_gyro_factor);
        gyro->data[DATA_AXIS_Y] = (int32_t)((int64_t)y * GYROSCOPE_UNIT_FACTOR / (int64_t)g_cur_gyro_factor);
        gyro->data[DATA_AXIS_Z] = (int32_t)((int64_t)z * GYROSCOPE_UNIT_FACTOR / (int64_t)g_cur_gyro_factor);
        //printf("cur_gyro_factor = %d \n", g_cur_gyro_factor);
        //printf("x = %d, y = %d, z = %d \n", gyro->data[DATA_AXIS_X], gyro->data[DATA_AXIS_Y], gyro->data[DATA_AXIS_Z]);
    } else
        printf("g_cur_gyro_factor == 0 \n");
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
static int drv_gyro_bosch_bmi260_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_gyro_bosch_bmi260_set_odr(&bmi260_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_gyro_bosch_bmi260_set_range(&bmi260_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_gyro_bosch_bmi260_set_power_mode(&bmi260_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMI260";
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
int drv_gyro_bosch_bmi260_init(void) {
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_bosch_bmi260_open;
    sensor.close      = drv_gyro_bosch_bmi260_close;
    sensor.read       = drv_gyro_bosch_bmi260_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_bosch_bmi260_ioctl;
    sensor.irq_handle = drv_gyro_bosch_bmi260_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_gyro_bosch_bmi260_validate_id(&bmi260_ctx, BMI260_CHIP_ID);
    if(unlikely(ret) != 0) {
        return -1;
    }

    if(0 == g_bmi260flag) {
        ret = drv_acc_gyro_bosch_bmi260_soft_reset(&bmi260_ctx);
        if(unlikely(ret) != 0) {
            return -1;
        }

        ret = drv_gyro_bosch_bmi260_set_power_mode(&bmi260_ctx, DEV_POWER_OFF);
        if(unlikely(ret) != 0) {
            return -1;
        }
        g_bmi260flag = 1;
    }
    else {
        LOG("%s %s gyro do not need reset\n", SENSOR_STR, __func__);
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_bosch_bmi260_init);
SENSOR_DRV_ADD(drv_gyro_bosch_bmi260_init);