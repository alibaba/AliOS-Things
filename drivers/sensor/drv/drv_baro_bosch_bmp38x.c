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
#define BMP380_BIT(x)                       ((uint8_t)(x))
#define BMP380_CHIP_ID_VAL                  BMP380_BIT(0X50)
#define BMP380_I2C_SLAVE_ADDR_LOW           (0X76)
#define BMP380_I2C_SLAVE_ADDR_HIGH          (0X77)

#define BMP380_I2C_ADDR_TRANS(n)            ((n)<<1)  
#define BMP380_I2C_ADDR                     BMP380_I2C_ADDR_TRANS(BMP380_I2C_SLAVE_ADDR_LOW)
#define BMP380_DEFAULT_ODR_1HZ              (1)
#define BMP380_TEMPERATURE_DATA_SIZE        (3)
#define BMP380_PRESSURE_DATA_SIZE           (3)

#define BMP380_TEMPERATURE_MSB_DATA         (2)
#define BMP380_TEMPERATURE_LSB_DATA         (1)
#define BMP380_TEMPERATURE_XLSB_DATA        (0)

#define BMP380_PRESSURE_MSB_DATA            (2)
#define BMP380_PRESSURE_LSB_DATA            (1)
#define BMP380_PRESSURE_XLSB_DATA           (0)

/* API error codes */
#define BMP380_E_NULL_PTR			        INT8_C(-1)
#define BMP380_E_DEV_NOT_FOUND			    INT8_C(-2)
#define BMP380_E_INVALID_ODR_OSR_SETTINGS   INT8_C(-3)
#define BMP380_E_CMD_EXEC_FAILED		    INT8_C(-4)
#define BMP380_E_CONFIGURATION_ERR		    INT8_C(-5)
#define BMP380_E_INVALID_LEN			    INT8_C(-6)
#define BMP380_E_COMM_FAIL			        INT8_C(-7)
#define BMP380_E_FIFO_WATERMARK_NOT_REACHED INT8_C(-8)

/* Register Address */
#define BMP380_CHIP_ID_ADDR		            UINT8_C(0x00)
#define BMP380_ERR_REG_ADDR		            UINT8_C(0x02)
#define BMP380_SENS_STATUS_REG_ADDR	        UINT8_C(0x03)
#define BMP380_DATA_ADDR		            UINT8_C(0x04)
#define BMP380_TDATA_ADDR		            UINT8_C(0x07)
#define BMP380_EVENT_ADDR		            UINT8_C(0x10)
#define BMP380_INT_STATUS_REG_ADDR	        UINT8_C(0x11)
#define BMP380_FIFO_LENGTH_ADDR		        UINT8_C(0x12)
#define BMP380_FIFO_DATA_ADDR		        UINT8_C(0x14)
#define BMP380_FIFO_WM_ADDR		            UINT8_C(0x15)
#define BMP380_FIFO_CONFIG_1_ADDR	        UINT8_C(0x17)
#define BMP380_FIFO_CONFIG_2_ADDR	        UINT8_C(0x18)
#define BMP380_INT_CTRL_ADDR		        UINT8_C(0x19)
#define BMP380_IF_CONF_ADDR		            UINT8_C(0x1A)
#define BMP380_PWR_CTRL_ADDR		        UINT8_C(0x1B)
#define BMP380_OSR_ADDR			            UINT8_C(0X1C)
#define BMP380_ODR_ADDR			            UINT8_C(0X1D)
#define BMP380_CALIB_DATA_ADDR		        UINT8_C(0x31)
#define BMP380_CMD_ADDR			            UINT8_C(0x7E)

#define BMP380_CALIB_DATA_ADDR		        UINT8_C(0x31)
#define BMP380_CALIB_DATA_LEN		        UINT8_C(21)
#define BMP380_CALIB_DATA_SIZE              UINT8_C(21)

#define BMP380_ULTRA_LOW_POWER_MODE         (0x00)
#define BMP380_LOW_POWER_MODE               (0x01)
#define BMP380_STANDARD_RESOLUTION_MODE     (0x02)
#define BMP380_HIGH_RESOLUTION_MODE         (0x03)
#define BMP380_ULTRA_HIGH_RESOLUTION_MODE   (0x04)
#define BMP380_HIGHEST_RESOLUTION_MODE      (0x05)

/* Over sampling macros */
#define BMP380_NO_OVERSAMPLING		        UINT8_C(0x00)
#define BMP380_OVERSAMPLING_2X		        UINT8_C(0x01)
#define BMP380_OVERSAMPLING_4X		        UINT8_C(0x02)
#define BMP380_OVERSAMPLING_8X		        UINT8_C(0x03)
#define BMP380_OVERSAMPLING_16X		        UINT8_C(0x04)
#define BMP380_OVERSAMPLING_32X		        UINT8_C(0x05)

/* Odr setting macros */
#define BMP380_ODR_200_HZ		            UINT8_C(0x00)
#define BMP380_ODR_100_HZ		            UINT8_C(0x01)
#define BMP380_ODR_50_HZ		            UINT8_C(0x02)
#define BMP380_ODR_25_HZ		            UINT8_C(0x03)
#define BMP380_ODR_12_5_HZ		            UINT8_C(0x04)
#define BMP380_ODR_6_25_HZ		            UINT8_C(0x05)
#define BMP380_ODR_3_1_HZ		            UINT8_C(0x06)
#define BMP380_ODR_1_5_HZ		            UINT8_C(0x07)
#define BMP380_ODR_0_78_HZ		            UINT8_C(0x08)
#define BMP380_ODR_0_39_HZ		            UINT8_C(0x09)
#define BMP380_ODR_0_2_HZ		            UINT8_C(0x0A)
#define BMP380_ODR_0_1_HZ		            UINT8_C(0x0B)
#define BMP380_ODR_0_05_HZ		            UINT8_C(0x0C)
#define BMP380_ODR_0_02_HZ		            UINT8_C(0x0D)
#define BMP380_ODR_0_01_HZ		            UINT8_C(0x0E)
#define BMP380_ODR_0_006_HZ		            UINT8_C(0x0F)
#define BMP380_ODR_0_003_HZ		            UINT8_C(0x10)
#define BMP380_ODR_0_001_HZ		            UINT8_C(0x11)

#define BMP380_PRESS_ENABLED                UINT8_C(0x01)
#define BMP380_TEMP_ENABLED                 UINT8_C(0x01)
/* CMD definition */
#define BMP380_SOFT_RST_CMD                 UINT8_C(0xB6)

/*  Status macros */
#define BMP380_CMD_RDY		                UINT8_C(0x10)
#define BMP380_DRDY_PRESS	                UINT8_C(0x20)
#define BMP380_DRDY_TEMP	                UINT8_C(0x40)

/* Error status macros */
#define BMP380_FATAL_ERR	                UINT8_C(0x01)
#define BMP380_CMD_ERR		                UINT8_C(0x02)
#define BMP380_CONF_ERR		                UINT8_C(0x04)

/* Power mode macros */
#define BMP380_SLEEP_MODE		            UINT8_C(0x00)
#define BMP380_FORCED_MODE		            UINT8_C(0x01)
#define BMP380_NORMAL_MODE		            UINT8_C(0x03)

#define BMP380_PRESS_OS_MSK		            UINT8_C(0x07)
#define BMP380_PRESS_OS_POS		            UINT8_C(0x00)

#define BMP380_TEMP_OS_MSK		            UINT8_C(0x38)
#define BMP380_TEMP_OS_POS		            UINT8_C(0x03)

#define BMP380_OP_MODE_MSK		            UINT8_C(0x30)
#define BMP380_OP_MODE_POS		            UINT8_C(0x04)

#define BMP380_ODR_MSK			            UINT8_C(0x1F)
#define BMP380_ODR_POS			            UINT8_C(0x00)

#define BMP380_PRESS_EN_MSK		            UINT8_C(0x01)
#define BMP380_PRESS_EN_POS		            UINT8_C(0x00)

#define BMP380_TEMP_EN_MSK		            UINT8_C(0x02)
#define BMP380_TEMP_EN_POS		            UINT8_C(0x01)

#define BMP380_SHIFT_BY_01_BIT              (1)
#define BMP380_SHIFT_BY_02_BITS             (2)
#define BMP380_SHIFT_BY_03_BITS             (3)
#define BMP380_SHIFT_BY_04_BITS             (4)
#define BMP380_SHIFT_BY_05_BITS             (5)
#define BMP380_SHIFT_BY_08_BITS             (8)
#define BMP380_SHIFT_BY_11_BITS             (11)
#define BMP380_SHIFT_BY_12_BITS             (12)
#define BMP380_SHIFT_BY_13_BITS             (13)
#define BMP380_SHIFT_BY_14_BITS             (14)
#define BMP380_SHIFT_BY_15_BITS             (15)
#define BMP380_SHIFT_BY_16_BITS             (16)
#define BMP380_SHIFT_BY_17_BITS             (17)
#define BMP380_SHIFT_BY_18_BITS             (18)
#define BMP380_SHIFT_BY_19_BITS             (19)
#define BMP380_SHIFT_BY_25_BITS             (25)
#define BMP380_SHIFT_BY_31_BITS             (31)
#define BMP380_SHIFT_BY_33_BITS             (33)
#define BMP380_SHIFT_BY_35_BITS             (35)
#define BMP380_SHIFT_BY_47_BITS             (47)

/* Macro to combine two 8 bit data's to form a 16 bit data */
#define BMP380_CONCAT_BYTES(msb, lsb)       (((uint16_t)msb << 8) | (uint16_t)lsb)

#define BMP380_SET_BITSLICE(reg_data, bitname, data) \
				((reg_data & ~(bitname##_MSK)) | \
				((data << bitname##_POS) & bitname##_MSK))

#define BMP380_GET_BITSLICE(reg_data, bitname) \
                ((reg_data & (bitname##_MSK)) >> \
				(bitname##_POS))

#define BMP380_GET_LSB(var)	               (uint8_t)(var & BMA4_SET_LOW_BYTE)
#define BMP380_GET_MSB(var)	               (uint8_t)((var & BMA4_SET_HIGH_BYTE) >> 8)

typedef struct bmp380_calib_param_t {
    uint16_t    dig_T1;
    uint16_t    dig_T2;
    int8_t      dig_T3;
    int16_t     dig_P1;
    int16_t     dig_P2;
    int8_t      dig_P3;
    int8_t      dig_P4;
    int16_t     dig_P5;
    int16_t     dig_P6;
    int8_t      dig_P7;
    int8_t      dig_P8;
    int16_t     dig_P9;
    int8_t      dig_P10;
    int8_t      dig_P11;
    int64_t         t_fine;
}bmp380_calib_param_t;

static bmp380_calib_param_t   g_bmp380_calib_table;

i2c_dev_t bmp380_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMP380_I2C_ADDR,
};

/**
 * This function gets the calibration param
 * 
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int  drv_baro_bosch_bmp380_get_calib_param(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t a_data_u8[BMP380_CALIB_DATA_SIZE] = {0};

    ret = sensor_i2c_read(drv,BMP380_CALIB_DATA_ADDR,
        a_data_u8,BMP380_CALIB_DATA_LEN,I2C_OP_RETRIES);
    
    if (unlikely(ret) != 0) {
        return ret;
    }
    
    g_bmp380_calib_table.dig_T1 = BMP380_CONCAT_BYTES(a_data_u8[1], a_data_u8[0]);
    g_bmp380_calib_table.dig_T2 = BMP380_CONCAT_BYTES(a_data_u8[3], a_data_u8[2]);
    g_bmp380_calib_table.dig_T3 = (int8_t)a_data_u8[4];
    g_bmp380_calib_table.dig_P1 = (int16_t)BMP380_CONCAT_BYTES(a_data_u8[6], a_data_u8[5]);
    g_bmp380_calib_table.dig_P2 = (int16_t)BMP380_CONCAT_BYTES(a_data_u8[8], a_data_u8[7]);
    g_bmp380_calib_table.dig_P3 = (int8_t)a_data_u8[9];
    g_bmp380_calib_table.dig_P4 = (int8_t)a_data_u8[10];
    g_bmp380_calib_table.dig_P5 = BMP380_CONCAT_BYTES(a_data_u8[12], a_data_u8[11]);
    g_bmp380_calib_table.dig_P6 = BMP380_CONCAT_BYTES(a_data_u8[14],  a_data_u8[13]);
    g_bmp380_calib_table.dig_P7 = (int8_t)a_data_u8[15];
    g_bmp380_calib_table.dig_P8 = (int8_t)a_data_u8[16];
    g_bmp380_calib_table.dig_P9 = (int16_t)BMP380_CONCAT_BYTES(a_data_u8[18], a_data_u8[17]);
    g_bmp380_calib_table.dig_P10 = (int8_t)a_data_u8[19];
    g_bmp380_calib_table.dig_P11 = (int8_t)a_data_u8[20];

    return 0;
}

/**
 * This function validates the chip ID of device
 * 
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  id_value  the expected CHIPID
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t value = 0;

    if (drv == NULL) {
        return -1;
    }
    
    ret = sensor_i2c_read(drv, BMP380_CHIP_ID_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret) != 0) {
        return ret;
    }
    
    if (id_value != value) {
        return -1;
    }
    return 0;
}

/**
 * This function sets the baro workmode
 * 
 * @param[in]  drv   pointer to the i2c dev
 * @param[in]  mode  the workmode to be setted
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_set_work_mode(i2c_dev_t* drv,uint8_t mode)
{
    uint8_t ret = 0;
    uint8_t value = 0;
    uint8_t temp = 0;
    uint8_t baro = 0;

    switch (mode) {
        case BMP380_ULTRA_LOW_POWER_MODE:
            temp = BMP380_NO_OVERSAMPLING;
            baro = BMP380_NO_OVERSAMPLING;
            break;

        case BMP380_LOW_POWER_MODE:
            temp = BMP380_OVERSAMPLING_2X;
            baro = BMP380_OVERSAMPLING_2X;
            break;

        case BMP380_STANDARD_RESOLUTION_MODE:
            temp = BMP380_OVERSAMPLING_4X;
            baro = BMP380_OVERSAMPLING_4X;
            break;

        case BMP380_HIGH_RESOLUTION_MODE:
            temp = BMP380_OVERSAMPLING_8X;
            baro = BMP380_OVERSAMPLING_8X;
            break;

        case BMP380_ULTRA_HIGH_RESOLUTION_MODE:
            temp = BMP380_OVERSAMPLING_16X;
            baro = BMP380_OVERSAMPLING_16X;
            break;

        case BMP380_HIGHEST_RESOLUTION_MODE:
            temp = BMP380_OVERSAMPLING_32X;
            baro = BMP380_OVERSAMPLING_32X;
            break;

        default:
            return -1;

    }

    ret = sensor_i2c_read(drv, BMP380_OSR_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret) != 0) {
        return ret;
    }
    value = BMP380_SET_BITSLICE(value,BMP380_PRESS_OS,baro);
    value = BMP380_SET_BITSLICE(value,BMP380_TEMP_OS,temp);
    
    ret = sensor_i2c_write(drv, BMP380_OSR_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    aos_msleep(2);
    if (unlikely(ret) != 0) {
        return ret;
    }

    return ret;
}

/**
 * This function sets the baro powermode
 * 
 * @param[in]  drv   pointer to the i2c dev
 * @param[in]  mode  the powermode to be setted
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t value = 0x00;
    uint8_t dev_mode;
    
    switch(mode){
        case DEV_POWER_OFF:
        case DEV_SLEEP:{
            dev_mode = (uint8_t)BMP380_SLEEP_MODE;

            break;
            }
        case DEV_POWER_ON:{
            dev_mode = (uint8_t)BMP380_NORMAL_MODE;
            break;
            }
        default:return -1;
    }

    ret = sensor_i2c_read(drv, BMP380_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret) != 0) {
        return ret;
    }
    value = BMP380_SET_BITSLICE(value,BMP380_OP_MODE,dev_mode);
    ret = sensor_i2c_write(drv, BMP380_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    aos_msleep(2);
    if (unlikely(ret) != 0) {
        return ret;
    }
    
    return 0;
}

/**
 * This function enables the pressure sensor and temperature sensor
 * 
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_enable_pressure_temp(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMP380_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret) != 0) {
        return ret;
    }

    value = BMP380_SET_BITSLICE(value,BMP380_PRESS_EN,BMP380_PRESS_ENABLED);
    value = BMP380_SET_BITSLICE(value,BMP380_TEMP_EN,BMP380_TEMP_ENABLED);

    ret = sensor_i2c_write(drv, BMP380_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    aos_msleep(2);
    if (unlikely(ret) != 0) {
        return ret;
    }
    
    return 0;
}

/**
 * This function gets the baro ODR according to HZ 
 * 
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the conrresponding baro ODR
 */
static uint8_t drv_baro_bosch_bmp380_hz2odr(int hz)
{
    if (hz > 100)
        return BMP380_ODR_200_HZ;
    else if (hz > 50)
        return BMP380_ODR_100_HZ;
    else if (hz > 25)
        return BMP380_ODR_50_HZ;
    else if (hz > 12)
        return BMP380_ODR_25_HZ;
    else if (hz > 6)
        return BMP380_ODR_12_5_HZ;
    else if (hz > 3)
        return BMP380_ODR_6_25_HZ;
    else if (hz > 1)
        return BMP380_ODR_3_1_HZ;
    else
        return BMP380_ODR_1_5_HZ;
}

/**
 * This function sets the baro ODR 
 * 
 * @param[in]  drv  pointer to the i2c dev
 * @param[in]  hz   the frequency required
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_set_odr(i2c_dev_t* drv, uint8_t odr)
{
    int     ret = 0;
    uint8_t v_data_u8 = 0;

    ret = sensor_i2c_read(drv,BMP380_ODR_ADDR,
                            &v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    if (unlikely(ret) != 0) {
        return ret;
    }

    v_data_u8 = BMP380_SET_BITSLICE(v_data_u8,BMP380_ODR,odr);
    ret = sensor_i2c_write(drv,BMP380_ODR_ADDR,
                            &v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    aos_msleep(2);
    if (unlikely(ret) != 0) {
        return ret;
    }
    
    return ret;
}

/**
 * This function does the soft reset
 * 
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int  drv_baro_bosch_bmp380_soft_reset(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t cmd_rdy_status;
	uint8_t cmd_err_status;
    uint8_t v_data_u8 = BMP380_SOFT_RST_CMD;

    ret = sensor_i2c_read(drv, BMP380_SENS_STATUS_REG_ADDR, &cmd_rdy_status, I2C_DATA_LEN, I2C_OP_RETRIES);
    
    if ((cmd_rdy_status & BMP380_CMD_RDY) && (!unlikely(ret))) {
    ret = sensor_i2c_write(drv,BMP380_CMD_ADDR,
                            &v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    aos_msleep(2);
    if (!unlikely(ret) != 0) {
        ret = sensor_i2c_read(drv, BMP380_ERR_REG_ADDR, &cmd_err_status, I2C_DATA_LEN, I2C_OP_RETRIES);
        if ((cmd_err_status & BMP380_CMD_ERR) || (unlikely(ret))) {
            ret = BMP380_E_CMD_EXEC_FAILED;
        }
    }
    } else {
        ret = BMP380_E_CMD_EXEC_FAILED;
    }

    if (unlikely(ret) != 0) {
        return ret;
    }
    
    return ret;
}

/**
 * This function sets the sensor to default
 * 
 * @param[in]  drv  pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    ret = drv_baro_bosch_bmp380_enable_pressure_temp(drv);
    if (unlikely(ret) != 0) {
        return ret;
    }

    ret = drv_baro_bosch_bmp380_set_power_mode(drv, DEV_SLEEP);
    if (unlikely(ret) != 0) {
        return ret;
    }

    ret = drv_baro_bosch_bmp380_set_odr(drv, BMP380_DEFAULT_ODR_1HZ);
    if (unlikely(ret) != 0) {
        return ret;
    }

    return 0;
}

/**
 * This function reads the uncompensated baro data
 * 
 * @param[in]      drv    pointer to the i2c dev
 * @param[in out]  pdata  pointer to the baro data
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_read_uncomp_baro(i2c_dev_t* drv, barometer_data_t* pdata)
{
    int     ret = 0;
    uint8_t data[BMP380_PRESSURE_DATA_SIZE] = {0};

    ret = sensor_i2c_read(drv, BMP380_DATA_ADDR, data, BMP380_PRESSURE_DATA_SIZE, I2C_OP_RETRIES);
    if (unlikely(ret) != 0) {
        return ret;
    }
    
    pdata->p = ((((uint32_t)(data[BMP380_PRESSURE_MSB_DATA]))<< BMP380_SHIFT_BY_16_BITS)
            | (((uint32_t)(data[BMP380_PRESSURE_LSB_DATA]))<< BMP380_SHIFT_BY_08_BITS)
            | ((uint32_t)data[BMP380_PRESSURE_XLSB_DATA]));
    return ret;
}

/**
 * This function compensates the uncompensated baro data
 * 
 * @param[in out]  pdata  pointer to the baro data
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_compensate_baro( barometer_data_t* pdata)
{
	int64_t  partial_data1;
	int64_t  partial_data2;
	int64_t  partial_data3;
	int64_t  partial_data4;
	int64_t  partial_data5;
	int64_t  partial_data6;
	int64_t  offset;
	int64_t  sensitivity;
	uint64_t comp_press;
    uint32_t comp_baro = 0;

	partial_data1 = g_bmp380_calib_table.t_fine * g_bmp380_calib_table.t_fine;
	partial_data2 = partial_data1 / 64;
	partial_data3 = (partial_data2 * g_bmp380_calib_table.t_fine) / 256;
	partial_data4 = (g_bmp380_calib_table.dig_P8 * partial_data3) / 32;
	partial_data5 = (g_bmp380_calib_table.dig_P7 * partial_data1) * 16;
	partial_data6 = (g_bmp380_calib_table.dig_P6 * g_bmp380_calib_table.t_fine) * 4194304;
	offset = (g_bmp380_calib_table.dig_P5 * 140737488355328) + partial_data4 + partial_data5 + partial_data6;

	partial_data2 = (g_bmp380_calib_table.dig_P4 * partial_data3) / 32;
	partial_data4 = (g_bmp380_calib_table.dig_P3 * partial_data1) * 4;
	partial_data5 = (g_bmp380_calib_table.dig_P2 - 16384) * g_bmp380_calib_table.t_fine * 2097152;
	sensitivity = ((g_bmp380_calib_table.dig_P1 - 16384) * 70368744177664) + partial_data2 + partial_data4 + partial_data5;

	partial_data1 = (sensitivity / 16777216) * pdata->p;
	partial_data2 = g_bmp380_calib_table.dig_P10 * g_bmp380_calib_table.t_fine;
	partial_data3 = partial_data2 + (65536 * g_bmp380_calib_table.dig_P9);
	partial_data4 = (partial_data3 * pdata->p) / 8192;
	partial_data5 = (partial_data4 * pdata->p) / 512;
	partial_data6 = (int64_t)((uint64_t)pdata->p * (uint64_t)pdata->p);
	partial_data2 = (g_bmp380_calib_table.dig_P11 * partial_data6) / 65536;
	partial_data3 = (partial_data2 * pdata->p) / 128;
	partial_data4 = (offset / 4) + partial_data1 + partial_data5 + partial_data3;
	comp_press = (((uint64_t)partial_data4 * 25) / (uint64_t)1099511627776);
    comp_baro = (uint32_t)(comp_press) / 100;
    pdata->p = comp_baro;
    return 0;
}

/**
 * This function reads the baro data and reports the data 
 * 
 * @param[in out]  buf   buffer for acc data
 * @param[in out]  len   length of data
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_read_baro(i2c_dev_t* drv, barometer_data_t* pdata)
{
    int ret = 0;
    
    ret = drv_baro_bosch_bmp380_read_uncomp_baro(drv, pdata);
    if (unlikely(ret) != 0) {
        return ret;
    }
    ret = drv_baro_bosch_bmp380_compensate_baro(pdata);
    if (unlikely(ret) != 0) {
        return ret;
    }

    return 0;
}

/**
 * This function compensates the uncompensated temp data
 * 
 * @param[in out]  pdata  pointer to the baro data
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_comp_temp(temperature_data_t* pdata)
{
    uint64_t partial_data1;
	uint64_t partial_data2;
	uint64_t partial_data3;
	int64_t  partial_data4;
	int64_t  partial_data5;
	int64_t  partial_data6;
	int64_t  comp_temp;

	partial_data1 = pdata->t - (256 * g_bmp380_calib_table.dig_T1);
	partial_data2 = g_bmp380_calib_table.dig_T2 * partial_data1;
	partial_data3 = partial_data1 * partial_data1;
	partial_data4 = (int64_t)partial_data3 * g_bmp380_calib_table.dig_T3;
	partial_data5 = ((int64_t)(partial_data2 * 262144) + partial_data4);
	partial_data6 = partial_data5 / 4294967296;
	/* Store t_lin in dev. structure for pressure calculation */
	g_bmp380_calib_table.t_fine = partial_data6;
	comp_temp = (int64_t)((partial_data6 * 25)  / 16384);
    pdata->t = (int32_t)(comp_temp);
    return 0;
}

/**
 * This function calibrate the uncompensated temp data
 * 
 * @param[in]      drv    pointer to the i2c dev
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_cali_temp(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t data[BMP380_TEMPERATURE_DATA_SIZE] = {0};
    temperature_data_t temp;

    ret  = sensor_i2c_read(drv, BMP380_TDATA_ADDR, data, BMP380_TEMPERATURE_DATA_SIZE, I2C_OP_RETRIES);
    if(unlikely(ret) != 0){
        return ret;
    }

    temp.t = ((((uint32_t)(data[BMP380_TEMPERATURE_MSB_DATA]))<< BMP380_SHIFT_BY_16_BITS)
            | (((uint32_t)(data[BMP380_TEMPERATURE_LSB_DATA]))<< BMP380_SHIFT_BY_08_BITS)
            | ((uint32_t)data[BMP380_TEMPERATURE_XLSB_DATA])); 

    ret = drv_baro_bosch_bmp380_comp_temp(&temp);
    if(unlikely(ret) != 0){
        return ret;
    }

    return 0;
}

/**
 * This function is the ISR 
 * 
 * @return
 */
static void drv_baro_bosch_bmp380_irq_handle(void)
{
    /* no handle so far */
}

/**
 * This function opens the baro 
 * 
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_open(void)
{
    int ret = 0;

    /* set the default config for the sensor here */
    ret = drv_baro_bosch_bmp380_set_work_mode(&bmp380_ctx,BMP380_ULTRA_LOW_POWER_MODE);
    if (unlikely(ret) != 0) {
        return -1;
    }
    ret = drv_baro_bosch_bmp380_enable_pressure_temp(&bmp380_ctx);
    if (unlikely(ret) != 0) {
        return ret;
    }
    ret  =  drv_baro_bosch_bmp380_set_power_mode(&bmp380_ctx, DEV_POWER_ON);
    if (unlikely(ret) != 0) {
        return -1;
    }
    
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

/**
 * This function closes the baro 
 * 
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_close(void)
{
    int ret = 0;
    ret  = drv_baro_bosch_bmp380_set_power_mode(&bmp380_ctx, DEV_POWER_OFF);
    if (unlikely(ret) != 0) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

/**
 * This function reads the baro data and reports the data 
 * 
 * @param[in out]  buf   buffer for baro data
 * @param[in out]  len   length of data
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_read(void *buf, size_t len)
{
    int    ret = 0;
    size_t size = 0;
    barometer_data_t* pdata = (barometer_data_t*)buf;
    
    if (buf == NULL) {
        return -1;
    }
    size = sizeof(barometer_data_t);
    if (len < size) {
        return -1;
    }
    
    ret = drv_baro_bosch_bmp380_cali_temp(&bmp380_ctx);
    if (unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_baro_bosch_bmp380_read_baro(&bmp380_ctx, pdata);
    if (unlikely(ret) != 0) {
        return -1;
    }

    pdata->timestamp = aos_now_ms();
    
    return (int)size;
}

/**
 * This function writess the baro
 * 
 * @param[in out]  buf   buffer for written data
 * @param[in out]  len   length of data
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

/**
 * This function is for the baro ioctl
 * 
 * @param[in]  cmd   the ioctl command
 * @param[in]  arg   the correspondding parameter
 * @return  the operation status, 0 is OK, others is error
 */
static int drv_baro_bosch_bmp380_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            uint8_t odr = drv_baro_bosch_bmp380_hz2odr(arg);
            ret = drv_baro_bosch_bmp380_set_odr(&bmp380_ctx, odr);
            if (unlikely(ret) != 0) {
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_baro_bosch_bmp380_set_power_mode(&bmp380_ctx, arg);
            if (unlikely(ret) != 0) {
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMP380";
            info->range_max = 1100;
            info->range_min = 300;
            info->unit = pa;

        }break;
       
       default:break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

/**
 * This function is for the baro initialization
 * 
 * @return  the operation status, 0 is OK, others is error
 */
int drv_baro_bosch_bmp380_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_BARO;
    sensor.path = dev_baro_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_baro_bosch_bmp380_open;
    sensor.close = drv_baro_bosch_bmp380_close;
    sensor.read = drv_baro_bosch_bmp380_read;
    sensor.write = drv_baro_bosch_bmp380_write;
    sensor.ioctl = drv_baro_bosch_bmp380_ioctl;
    sensor.irq_handle = drv_baro_bosch_bmp380_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_baro_bosch_bmp380_validate_id(&bmp380_ctx, BMP380_CHIP_ID_VAL);
    if (unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_baro_bosch_bmp380_soft_reset(&bmp380_ctx);
    if (unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_baro_bosch_bmp380_set_default_config(&bmp380_ctx);
    if (unlikely(ret) != 0) {
        return -1;
    }
    
    ret = drv_baro_bosch_bmp380_get_calib_param(&bmp380_ctx);
    if (unlikely(ret) != 0) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_baro_bosch_bmp380_init);

