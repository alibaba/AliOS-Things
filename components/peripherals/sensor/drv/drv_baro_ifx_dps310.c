/* The MIT License

Copyright(c) 2018 Infineon Technologies AG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define DPS310_I2C_SLAVE_ADDR_HIGH (0X77)
#define DPS310_I2C_ADDR_TRANS(n) ((n) << 1)
#define DPS310_I2C_ADDR DPS310_I2C_ADDR_TRANS(DPS310_I2C_SLAVE_ADDR_HIGH)


// general Constants
#define DPS310__PROD_ID 0U
#define DPS310__STD_SLAVE_ADDRESS 0x77U
#define DPS310__SPI_WRITE_CMD 0x00U
#define DPS310__SPI_READ_CMD 0x80U
#define DPS310__SPI_RW_MASK 0x80U
#define DPS310__SPI_MAX_FREQ 100000U

#define DPS310__LSB 0x01U

#define DPS310__TEMP_STD_MR 2U
#define DPS310__TEMP_STD_OSR 3U
#define DPS310__PRS_STD_MR 2U
#define DPS310__PRS_STD_OSR 3U
#define DPS310__OSR_SE 3U
// we use 0.1 mS units for time calculations, so 10 units are one millisecond
#define DPS310__BUSYTIME_SCALING 10U
// DPS310 has 10 milliseconds of spare time for each synchronous measurement /
// per second for asynchronous measurements this is for error prevention on
// friday-afternoon-products :D you can set it to 0 if you dare, but there is no
// warranty that it will still work
#define DPS310__BUSYTIME_FAILSAFE 10U
#define DPS310__MAX_BUSYTIME \
    ((1000U - DPS310__BUSYTIME_FAILSAFE) * DPS310__BUSYTIME_SCALING)
#define DPS310__NUM_OF_SCAL_FACTS 8

#define DPS310__SUCCEEDED 0
#define DPS310__FAIL_UNKNOWN -1
#define DPS310__FAIL_INIT_FAILED -2
#define DPS310__FAIL_TOOBUSY -3
#define DPS310__FAIL_UNFINISHED -4


// Constants for register manipulation
// SPI mode (3 or 4 wire)
#define DPS310__REG_ADR_SPI3W 0x09U
#define DPS310__REG_CONTENT_SPI3W 0x01U


// product id
#define DPS310__REG_INFO_PROD_ID \
    DPS310__REG_ADR_PROD_ID, DPS310__REG_MASK_PROD_ID, DPS310__REG_SHIFT_PROD_ID
#define DPS310__REG_ADR_PROD_ID 0x0DU
#define DPS310__REG_MASK_PROD_ID 0x0FU
#define DPS310__REG_SHIFT_PROD_ID 0U

// revision id
#define DPS310__REG_INFO_REV_ID \
    DPS310__REG_ADR_REV_ID, DPS310__REG_MASK_REV_ID, DPS310__REG_SHIFT_REV_ID
#define DPS310__REG_ADR_REV_ID 0x0DU
#define DPS310__REG_MASK_REV_ID 0xF0U
#define DPS310__REG_SHIFT_REV_ID 4U

// operating mode
#define DPS310__REG_INFO_OPMODE \
    DPS310__REG_ADR_OPMODE, DPS310__REG_MASK_OPMODE, DPS310__REG_SHIFT_OPMODE
#define DPS310__REG_ADR_OPMODE 0x08U
#define DPS310__REG_MASK_OPMODE 0x07U
#define DPS310__REG_SHIFT_OPMODE 0U


// temperature measure rate
#define DPS310__REG_INFO_TEMP_MR \
    DPS310__REG_ADR_TEMP_MR, DPS310__REG_MASK_TEMP_MR, DPS310__REG_SHIFT_TEMP_MR
#define DPS310__REG_ADR_TEMP_MR 0x07U
#define DPS310__REG_MASK_TEMP_MR 0x70U
#define DPS310__REG_SHIFT_TEMP_MR 4U

// temperature oversampling rate
#define DPS310__REG_INFO_TEMP_OSR                        \
    DPS310__REG_ADR_TEMP_OSR, DPS310__REG_MASK_TEMP_OSR, \
      DPS310__REG_SHIFT_TEMP_OSR
#define DPS310__REG_ADR_TEMP_OSR 0x07U
#define DPS310__REG_MASK_TEMP_OSR 0x07U
#define DPS310__REG_SHIFT_TEMP_OSR 0U

// temperature sensor
#define DPS310__REG_INFO_TEMP_SENSOR                           \
    DPS310__REG_ADR_TEMP_SENSOR, DPS310__REG_MASK_TEMP_SENSOR, \
      DPS310__REG_SHIFT_TEMP_SENSOR
#define DPS310__REG_ADR_TEMP_SENSOR 0x07U
#define DPS310__REG_MASK_TEMP_SENSOR 0x80U
#define DPS310__REG_SHIFT_TEMP_SENSOR 7U

// temperature sensor recommendation
#define DPS310__REG_INFO_TEMP_SENSORREC                              \
    DPS310__REG_ADR_TEMP_SENSORREC, DPS310__REG_MASK_TEMP_SENSORREC, \
      DPS310__REG_SHIFT_TEMP_SENSORREC
#define DPS310__REG_ADR_TEMP_SENSORREC 0x28U
#define DPS310__REG_MASK_TEMP_SENSORREC 0x80U
#define DPS310__REG_SHIFT_TEMP_SENSORREC 7U

// temperature shift enable (if temp_osr>3)
#define DPS310__REG_INFO_TEMP_SE \
    DPS310__REG_ADR_TEMP_SE, DPS310__REG_MASK_TEMP_SE, DPS310__REG_SHIFT_TEMP_SE
#define DPS310__REG_ADR_TEMP_SE 0x09U
#define DPS310__REG_MASK_TEMP_SE 0x08U
#define DPS310__REG_SHIFT_TEMP_SE 3U


// pressure measure rate
#define DPS310__REG_INFO_PRS_MR \
    DPS310__REG_ADR_PRS_MR, DPS310__REG_MASK_PRS_MR, DPS310__REG_SHIFT_PRS_MR
#define DPS310__REG_ADR_PRS_MR 0x06U
#define DPS310__REG_MASK_PRS_MR 0x70U
#define DPS310__REG_SHIFT_PRS_MR 4U

// pressure oversampling rate
#define DPS310__REG_INFO_PRS_OSR \
    DPS310__REG_ADR_PRS_OSR, DPS310__REG_MASK_PRS_OSR, DPS310__REG_SHIFT_PRS_OSR
#define DPS310__REG_ADR_PRS_OSR 0x06U
#define DPS310__REG_MASK_PRS_OSR 0x07U
#define DPS310__REG_SHIFT_PRS_OSR 0U

// pressure shift enable (if prs_osr>3)
#define DPS310__REG_INFO_PRS_SE \
    DPS310__REG_ADR_PRS_SE, DPS310__REG_MASK_PRS_SE, DPS310__REG_SHIFT_PRS_SE
#define DPS310__REG_ADR_PRS_SE 0x09U
#define DPS310__REG_MASK_PRS_SE 0x04U
#define DPS310__REG_SHIFT_PRS_SE 2U


// temperature ready flag
#define DPS310__REG_INFO_TEMP_RDY                        \
    DPS310__REG_ADR_TEMP_RDY, DPS310__REG_MASK_TEMP_RDY, \
      DPS310__REG_SHIFT_TEMP_RDY
#define DPS310__REG_ADR_TEMP_RDY 0x08U
#define DPS310__REG_MASK_TEMP_RDY 0x20U
#define DPS310__REG_SHIFT_TEMP_RDY 5U

// pressure ready flag
#define DPS310__REG_INFO_PRS_RDY \
    DPS310__REG_ADR_PRS_RDY, DPS310__REG_MASK_PRS_RDY, DPS310__REG_SHIFT_PRS_RDY
#define DPS310__REG_ADR_PRS_RDY 0x08U
#define DPS310__REG_MASK_PRS_RDY 0x10U
#define DPS310__REG_SHIFT_PRS_RDY 4U

// pressure value
#define DPS310__REG_ADR_PRS 0x00U
#define DPS310__REG_LEN_PRS 3U

// temperature value
#define DPS310__REG_ADR_TEMP 0x03U
#define DPS310__REG_LEN_TEMP 3U

// compensation coefficients
#define DPS310__REG_ADR_COEF 0x10U
#define DPS310__REG_LEN_COEF 18


// FIFO enable
#define DPS310__REG_INFO_FIFO_EN \
    DPS310__REG_ADR_FIFO_EN, DPS310__REG_MASK_FIFO_EN, DPS310__REG_SHIFT_FIFO_EN
#define DPS310__REG_ADR_FIFO_EN 0x09U
#define DPS310__REG_MASK_FIFO_EN 0x02U
#define DPS310__REG_SHIFT_FIFO_EN 1U

// FIFO flush
#define DPS310__REG_INFO_FIFO_FL \
    DPS310__REG_ADR_FIFO_EN, DPS310__REG_MASK_FIFO_EN, DPS310__REG_SHIFT_FIFO_EN
#define DPS310__REG_ADR_FIFO_FL 0x0CU
#define DPS310__REG_MASK_FIFO_FL 0x80U
#define DPS310__REG_SHIFT_FIFO_FL 7U

// FIFO empty
#define DPS310__REG_INFO_FIFO_EMPTY                          \
    DPS310__REG_ADR_FIFO_EMPTY, DPS310__REG_MASK_FIFO_EMPTY, \
      DPS310__REG_SHIFT_FIFO_EMPTY
#define DPS310__REG_ADR_FIFO_EMPTY 0x0BU
#define DPS310__REG_MASK_FIFO_EMPTY 0x01U
#define DPS310__REG_SHIFT_FIFO_EMPTY 0U

// FIFO full
#define DPS310__REG_INFO_FIFO_FULL                         \
    DPS310__REG_ADR_FIFO_FULL, DPS310__REG_MASK_FIFO_FULL, \
      DPS310__REG_SHIFT_FIFO_FULL
#define DPS310__REG_ADR_FIFO_FULL 0x0BU
#define DPS310__REG_MASK_FIFO_FULL 0x02U
#define DPS310__REG_SHIFT_FIFO_FULL 1U


// INT HL
#define DPS310__REG_INFO_INT_HL \
    DPS310__REG_ADR_INT_HL, DPS310__REG_MASK_INT_HL, DPS310__REG_SHIFT_INT_HL
#define DPS310__REG_ADR_INT_HL 0x09U
#define DPS310__REG_MASK_INT_HL 0x80U
#define DPS310__REG_SHIFT_INT_HL 7U

// INT FIFO enable
#define DPS310__REG_INFO_INT_EN_FIFO                           \
    DPS310__REG_ADR_INT_EN_FIFO, DPS310__REG_MASK_INT_EN_FIFO, \
      DPS310__REG_SHIFT_INT_EN_FIFO
#define DPS310__REG_ADR_INT_EN_FIFO 0x09U
#define DPS310__REG_MASK_INT_EN_FIFO 0x40U
#define DPS310__REG_SHIFT_INT_EN_FIFO 6U

// INT TEMP enable
#define DPS310__REG_INFO_INT_EN_TEMP                           \
    DPS310__REG_ADR_INT_EN_TEMP, DPS310__REG_MASK_INT_EN_TEMP, \
      DPS310__REG_SHIFT_INT_EN_TEMP
#define DPS310__REG_ADR_INT_EN_TEMP 0x09U
#define DPS310__REG_MASK_INT_EN_TEMP 0x20U
#define DPS310__REG_SHIFT_INT_EN_TEMP 5U

// INT PRS enable
#define DPS310__REG_INFO_INT_EN_PRS                          \
    DPS310__REG_ADR_INT_EN_PRS, DPS310__REG_MASK_INT_EN_PRS, \
      DPS310__REG_SHIFT_INT_EN_PRS
#define DPS310__REG_ADR_INT_EN_PRS 0x09U
#define DPS310__REG_MASK_INT_EN_PRS 0x10U
#define DPS310__REG_SHIFT_INT_EN_PRS 4U

// INT FIFO flag
#define DPS310__REG_INFO_INT_FLAG_FIFO                             \
    DPS310__REG_ADR_INT_FLAG_FIFO, DPS310__REG_MASK_INT_FLAG_FIFO, \
      DPS310__REG_SHIFT_INT_FLAG_FIFO
#define DPS310__REG_ADR_INT_FLAG_FIFO 0x0AU
#define DPS310__REG_MASK_INT_FLAG_FIFO 0x04U
#define DPS310__REG_SHIFT_INT_FLAG_FIFO 2U

// INT TMP flag
#define DPS310__REG_INFO_INT_FLAG_TEMP                             \
    DPS310__REG_ADR_INT_FLAG_TEMP, DPS310__REG_MASK_INT_FLAG_TEMP, \
      DPS310__REG_SHIFT_INT_FLAG_TEMP
#define DPS310__REG_ADR_INT_FLAG_TEMP 0x0AU
#define DPS310__REG_MASK_INT_FLAG_TEMP 0x02U
#define DPS310__REG_SHIFT_INT_FLAG_TEMP 1U

// INT PRS flag
#define DPS310__REG_INFO_INT_FLAG_PRS                            \
    DPS310__REG_ADR_INT_FLAG_PRS, DPS310__REG_MASK_INT_FLAG_PRS, \
      DPS310__REG_SHIFT_INT_FLAG_PRS
#define DPS310__REG_ADR_INT_FLAG_PRS 0x0AU
#define DPS310__REG_MASK_INT_FLAG_PRS 0x01U
#define DPS310__REG_SHIFT_INT_FLAG_PRS 0U


typedef enum _Mode
{
    IDLE               = 0x00,
    CMD_PRS            = 0x01,
    CMD_TEMP           = 0x02,
    INVAL_OP_CMD_BOTH  = 0x03, // invalid
    INVAL_OP_CONT_NONE = 0x04, // invalid
    CONT_PRS           = 0x05,
    CONT_TMP           = 0x06,
    CONT_BOTH          = 0x07
} Mode;


typedef struct
{
    i2c_dev_t i2c;

    Mode m_opMode;

    // flags
    uint8_t m_initFail;
    uint8_t m_productID;
    uint8_t m_revisionID;

    // settings
    uint8_t m_tempMr;
    uint8_t m_tempOsr;
    uint8_t m_prsMr;
    uint8_t m_prsOsr;
    uint8_t m_tempSensor;

    // compensation coefficients
    int32_t m_c0Half;
    int32_t m_c1;
    int32_t m_c00;
    int32_t m_c10;
    int32_t m_c01;
    int32_t m_c11;
    int32_t m_c20;
    int32_t m_c21;
    int32_t m_c30;
    // last measured scaled temperature
    //(necessary for pressure compensation)
    double m_lastTempScal;

} dps310_t;


const int32_t scaling_facts[DPS310__NUM_OF_SCAL_FACTS] = {
    524288, 1572864, 3670016, 7864320, 253952, 516096, 1040384, 2088960
};


dps310_t dps310_ctx = { .i2c.port                 = 3,
                        .i2c.config.address_width = 8,
                        .i2c.config.freq          = 200000,
                        .i2c.config.dev_addr      = DPS310_I2C_ADDR,
                        .m_prsMr                  = 0,
                        .m_prsOsr                 = 0 };

/**
 * reads a block from dps310
 *
 * regAdress: 	Address that has to be read
 * length: 		Length of data block
 * buffer: 	Buffer where data will be stored
 * returns: 	number of bytes that have been read successfully
 * 				NOTE! This is not always equal to length
 * 					  due to rx-Buffer overflow etc.
 */
int16_t readBlock(uint8_t regAddress, uint8_t length, uint8_t *buffer)
{
    // do not read if there is no buffer
    int ret;
    if (buffer == NULL) {
        return 0; // 0 bytes read successfully
    }

    ret = sensor_i2c_read(&(dps310_ctx.i2c), regAddress, buffer, length,
                          I2C_OP_RETRIES);
    if (ret != 0) {
        return 0;
    }

    return length;
}

/**
 * writes a given byte to a given register of dps310
 *
 * regAdress: 	Address of the register that has to be updated
 * data:		Byte that will be written to the register
 * check: 		If this is true, register content will be read after writing
 * 				to check if update was successful
 * return:		0 if byte was written successfully
 * 				or -1 on fail
 */
int16_t writeByte(uint8_t regAddress, uint8_t data)
{
    if (sensor_i2c_write(&(dps310_ctx.i2c), regAddress, &data, 1,
                         AOS_WAIT_FOREVER) == 0) {
        return DPS310__SUCCEEDED;
    } else {
        return DPS310__FAIL_UNKNOWN;
    }
}


/**
 * reads a byte from dps310
 *
 * regAdress: 	Address that has to be read
 * returns: 	register content or -1 on fail
 */
int16_t readByte(uint8_t regAddress)
{
    uint8_t data;
    int     ret =
      sensor_i2c_read(&(dps310_ctx.i2c), regAddress, &data, 1, I2C_OP_RETRIES);
    if (ret == 0)
        return data;
    else
        return -1;
}


/**
 * updates some given bits of a given register of dps310
 *
 * regAdress: 	Address of the register that has to be updated
 * data:		BitValues that will be written to the register
 * shift:		Amount of bits the data byte is shifted (left) before being
 * masked mask: 		Masks the bits of the register that have to be updated
 * 				Bits with value 1 are updated
 * 				Bits with value 0 are not changed
 * check: 		enables/disables check after writing
 * 				0 disables check
 * 				if check fails, -1 will be returned
 * return:		0 if byte was written successfully
 * 				or -1 on fail
 */
int16_t writeByteBitfield(uint8_t data, uint8_t regAddress, uint8_t mask,
                          uint8_t shift)
{
    int16_t old = readByte(regAddress);
    if (old < 0) {
        // fail while reading
        return old;
    }
    return writeByte(regAddress,
                     ((uint8_t)old & ~mask) | ((data << shift) & mask));
}


/**
 * Configures pressure measurement
 *
 * prsMr: 		the new measure rate for pressure
 * 				This can be a value from 0U to 7U.
 * 				Actual measure rate will be 2^prs_mr,
 * 				so this will be a value from 1 to 128.
 * prsOs: 	the new oversampling rate for temperature
 * 				This can be a value from 0U to 7U.
 * 				Actual measure rate will be 2^prsOsr,
 * 				so this will be a value from 1 to 128.
 * returns: 	0 normally or -1 on fail
 */
int16_t configPressure(uint8_t prsMr, uint8_t prsOsr)
{
    uint8_t toWrite;
    int16_t ret;
    // mask parameters
    prsMr &= DPS310__REG_MASK_PRS_MR >> DPS310__REG_SHIFT_PRS_MR;
    prsOsr &= DPS310__REG_MASK_PRS_OSR >> DPS310__REG_SHIFT_PRS_OSR;

    // set config register according to parameters
    toWrite = prsMr << DPS310__REG_SHIFT_PRS_MR;
    toWrite |= prsOsr << DPS310__REG_SHIFT_PRS_OSR;
    ret = writeByte(DPS310__REG_ADR_PRS_MR, toWrite);

    // abort immediately on fail
    if (ret != DPS310__SUCCEEDED) {
        return DPS310__FAIL_UNKNOWN;
    }

    // set PM SHIFT ENABLE if oversampling rate higher than eight(2^3)
    if (prsOsr > DPS310__OSR_SE) {
        ret = writeByteBitfield(1U, DPS310__REG_INFO_PRS_SE);
    } else {
        ret = writeByteBitfield(0U, DPS310__REG_INFO_PRS_SE);
    }

    if (ret == DPS310__SUCCEEDED) { // save new settings
        dps310_ctx.m_prsMr  = prsMr;
        dps310_ctx.m_prsOsr = prsOsr;
    } else { // try to rollback on fail avoiding endless recursion
        // this is to make sure that shift enable and oversampling rate
        // are always consistent
        if (prsMr != dps310_ctx.m_prsMr || prsOsr != dps310_ctx.m_prsOsr) {
            configPressure(dps310_ctx.m_prsMr, dps310_ctx.m_prsOsr);
        }
    }
    return ret;
}


/**
 * Calculates a scaled and compensated pressure value from raw data
 * raw: 	raw pressure value read from Dps310
 * returns: pressure value in Pa
 */
int32_t calcPressure(int32_t raw)
{
    double prs = raw;

    // scale pressure according to scaling table and oversampling
    prs /= scaling_facts[dps310_ctx.m_prsOsr];

    // Calculate compensated pressure
    prs =
      dps310_ctx.m_c00 +
      prs *
        (dps310_ctx.m_c10 + prs * (dps310_ctx.m_c20 + prs * dps310_ctx.m_c30)) +
      dps310_ctx.m_lastTempScal *
        (dps310_ctx.m_c01 + prs * (dps310_ctx.m_c11 + prs * dps310_ctx.m_c21));

    // return pressure
    return (int32_t)prs;
}

/**
 * Gets the next pressure measurement result in Pa
 *
 * result: 	address where the result will be written
 * returns: 0 on success
 * 			-1 on fail;
 */
int16_t getPressure(int32_t *result)
{
    uint8_t buffer[3] = { 0 };
    // read raw pressure data to buffer
    int16_t i = readBlock(DPS310__REG_ADR_PRS, DPS310__REG_LEN_PRS, buffer);
    if (i != DPS310__REG_LEN_PRS) {
        // something went wrong
        // negative pressure is not allowed
        return DPS310__FAIL_UNKNOWN;
    }

    // compose raw pressure value from buffer
    int32_t prs = (uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 |
                  (uint32_t)buffer[2];
    // recognize non-32-bit negative numbers
    // and convert them to 32-bit negative numbers using 2's complement
    if (prs & ((uint32_t)1 << 23)) {
        prs -= (uint32_t)1 << 24;
    }

    *result = calcPressure(prs);
    return DPS310__SUCCEEDED;
}


/**
 * Sets the Operation Mode of the Dps310
 *
 * opMode: 			the new OpMode that has to be set
 * return: 			0 on success, -1 on fail
 *
 * NOTE!
 * You cannot set background to 1 without setting temperature and pressure
 * You cannot set both temperature and pressure when background mode is disabled
 */
int16_t setOpMode(uint8_t opMode)
{
    // Filter irrelevant bits
    opMode &= DPS310__REG_MASK_OPMODE >> DPS310__REG_SHIFT_OPMODE;
    // Filter invalid OpModes
    if (opMode == INVAL_OP_CMD_BOTH || opMode == INVAL_OP_CONT_NONE) {
        return DPS310__FAIL_UNKNOWN;
    }
    // Set OpMode
    if (writeByte(DPS310__REG_ADR_OPMODE, opMode)) {
        return DPS310__FAIL_UNKNOWN;
    }
    dps310_ctx.m_opMode = (Mode)opMode;
    return DPS310__SUCCEEDED;
}

/**
 * Sets the Operation Mode of the Dps310
 *
 * background:		determines the general behavior of the Dps310
 *  				0 enables command mode (only measure on commands)
 * 					1 enables background mode (continuous work in background)
 * temperature: 	set 1 to measure temperature
 * pressure: 		set 1 to measure pressure
 * return:			0 on success, -1 on fail
 *
 * NOTE!
 * You cannot set background to 1 without setting temperature and pressure
 * You cannot set both temperature and pressure when background mode is disabled
 */
int16_t setOpMode_3param(uint8_t background, uint8_t temperature,
                         uint8_t pressure)
{
    uint8_t opMode = (background & DPS310__LSB) << 2U |
                     (temperature & DPS310__LSB) << 1U |
                     (pressure & DPS310__LSB);
    return setOpMode(opMode);
}

/**
 * Sets the Dps310 to standby mode
 *
 * returns:		0 on success
 * 				-2 if object initialization failed
 * 				-1 on other fail
 */
int16_t standby(void)
{
    // abort if initialization failed
    if (dps310_ctx.m_initFail) {
        return DPS310__FAIL_INIT_FAILED;
    }
    // set device to idling mode
    int16_t ret = setOpMode(IDLE);
    if (ret != DPS310__SUCCEEDED) {
        return ret;
    }
    // flush the FIFO
    ret = writeByteBitfield(1U, DPS310__REG_INFO_FIFO_FL);
    if (ret < 0) {
        return ret;
    }
    // disable the FIFO
    ret = writeByteBitfield(0U, DPS310__REG_INFO_FIFO_EN);
    return ret;
}


/**
 * reads some given bits of a given register of dps310
 *
 * regAdress: 	Address of the register that has to be updated
 * mask: 		Masks the bits of the register that have to be updated
 * 				Bits masked with value 1 are read
 * 				Bits masked with value 0 are set 0
 * shift:		Amount of bits the data byte is shifted (right) after being
 * masked return:		read and processed bits or -1 on fail
 */
int16_t readByteBitfield(uint8_t regAddress, uint8_t mask, uint8_t shift)
{
    int16_t ret = readByte(regAddress);
    if (ret < 0) {
        return ret;
    }
    return (((uint8_t)ret) & mask) >> shift;
}


/**
 * calculates the time that the DPS310 needs for 2^mr measurements
 * with an oversampling rate of 2^osr
 *
 * mr: 		Measure rate for temperature or pressure
 * osr: 	Oversampling rate for temperature or pressure
 * returns: time that the DPS310 needs for this measurement
 * 			a value of 10000 equals 1 second
 * 	NOTE! 	The measurement time for temperature and pressure
 * 			in sum must not be more than 1 second!
 * 			Timing behavior of pressure and temperature sensors
 * 			can be considered as equal.
 */
uint16_t calcBusyTime(uint16_t mr, uint16_t osr)
{
    // mask parameters first
    mr &= DPS310__REG_MASK_TEMP_MR >> DPS310__REG_SHIFT_TEMP_MR;
    osr &= DPS310__REG_MASK_TEMP_OSR >> DPS310__REG_SHIFT_TEMP_OSR;
    // formula from datasheet (optimized)
    return ((uint32_t)20U << mr) + ((uint32_t)16U << (osr + mr));
}


static void delay(int ms)
{
    aos_msleep(ms);
}


/**
 * Calculates a scaled and compensated pressure value from raw data
 * raw: 	raw temperature value read from Dps310
 * returns: temperature value in °C
 */
int32_t calcTemp(int32_t raw)
{
    double temp = raw;

    // scale temperature according to scaling table and oversampling
    temp /= scaling_facts[dps310_ctx.m_tempOsr];

    // update last measured temperature
    // it will be used for pressure compensation
    dps310_ctx.m_lastTempScal = temp;

    // Calculate compensated temperature
    temp = dps310_ctx.m_c0Half + dps310_ctx.m_c1 * temp;

    // return temperature
    return (int32_t)temp;
}


/**
 * starts a single pressure measurement
 * The desired precision can be set with oversamplingRate
 *
 * oversamplingRate: 	a value from 0 to 7 that decides about the precision
 * 						of the measurement
 * 						If this value equals n, the DPS310 will perform
 * 						2^n measurements and combine the results
 * returns: 			0 on success
 * 						-3 if the DPS310 is already busy
 * 						-2 if the object initialization failed
 * 						-1 on other fail
 */
int16_t startMeasurePressureOnce(uint8_t oversamplingRate)
{
    // abort if initialization failed
    if (dps310_ctx.m_initFail) {
        return DPS310__FAIL_INIT_FAILED;
    }
    // abort if device is not in idling mode
    if (dps310_ctx.m_opMode != IDLE) {
        return DPS310__FAIL_TOOBUSY;
    }
    // configuration of oversampling rate, lowest measure rate to avoid
    // conflicts
    if (oversamplingRate != dps310_ctx.m_prsOsr) {
        if (configPressure(0U, oversamplingRate)) {
            return DPS310__FAIL_UNKNOWN;
        }
    }
    // set device to pressure measuring mode
    return setOpMode_3param(0U, 0U, 1U);
}


/**
 * Gets the next temperature measurement result in degrees of Celsius
 *
 * result: 	address where the result will be written
 * returns:	0 on success
 * 			-1 on fail;
 */
int16_t getTemp(int32_t *result)
{
    uint8_t buffer[3] = { 0 };
    // read raw pressure data to buffer

    int16_t i = readBlock(DPS310__REG_ADR_TEMP, DPS310__REG_LEN_TEMP, buffer);
    if (i != DPS310__REG_LEN_TEMP) {
        // something went wrong
        return DPS310__FAIL_UNKNOWN;
    }

    // compose raw temperature value from buffer
    int32_t temp = (uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 |
                   (uint32_t)buffer[2];
    // recognize non-32-bit negative numbers
    // and convert them to 32-bit negative numbers using 2's complement
    if (temp & ((uint32_t)1 << 23)) {
        temp -= (uint32_t)1 << 24;
    }

    // return temperature
    *result = calcTemp(temp);
    return DPS310__SUCCEEDED;
}

/**
 * gets the result a single temperature or pressure measurement in °C or Pa
 *
 * &result:		reference to a 32-Bit signed Integer value where the result will
 * be written returns: 	0 on success -4 if the DPS310 is still busy -3 if the
 * DPS310 is not in command mode -2 if the object initialization failed -1 on
 * other fail
 */
int16_t getSingleResult(int32_t *result)
{
    // abort if initialization failed
    if (dps310_ctx.m_initFail) {
        return DPS310__FAIL_INIT_FAILED;
    }

    // read finished bit for current opMode
    int16_t rdy;
    switch (dps310_ctx.m_opMode) {
        case CMD_TEMP: // temperature
            rdy = readByteBitfield(DPS310__REG_INFO_TEMP_RDY);
            break;
        case CMD_PRS: // pressure
            rdy = readByteBitfield(DPS310__REG_INFO_PRS_RDY);
            break;
        default: // DPS310 not in command mode
            return DPS310__FAIL_TOOBUSY;
    }

    // read new measurement result
    switch (rdy) {
        case DPS310__FAIL_UNKNOWN: // could not read ready flag
            return DPS310__FAIL_UNKNOWN;
        case 0: // ready flag not set, measurement still in progress
            return DPS310__FAIL_UNFINISHED;
        case 1: // measurement ready, expected case
        {
            Mode oldMode = dps310_ctx.m_opMode;
            dps310_ctx.m_opMode =
              IDLE; // opcode was automatically reseted by DPS310
            switch (oldMode) {
                case CMD_TEMP: // temperature
                    return getTemp(
                      result); // get and calculate the temperature value
                case CMD_PRS:  // pressure
                    return getPressure(
                      result); // get and calculate the pressure value
                default:
                    return DPS310__FAIL_UNKNOWN; // should already be filtered
                                                 // above
            }
        }
    }
    return DPS310__FAIL_UNKNOWN;
}

/**
 * Configures temperature measurement
 *
 * tempMr: 	the new measure rate for temperature
 * 				This can be a value from 0U to 7U.
 * 				Actual measure rate will be 2^tempMr,
 * 				so this will be a value from 1 to 128.
 * tempOsr: 	the new oversampling rate for temperature
 * 				This can be a value from 0U to 7U.
 * 				Actual measure rate will be 2^tempOsr,
 * 				so this will be a value from 1 to 128.
 * returns: 	0 normally or -1 on fail
 */
int16_t configTemp(uint8_t tempMr, uint8_t tempOsr)
{
    // mask parameters
    tempMr &= DPS310__REG_MASK_TEMP_MR >> DPS310__REG_SHIFT_TEMP_MR;
    tempOsr &= DPS310__REG_MASK_TEMP_OSR >> DPS310__REG_SHIFT_TEMP_OSR;

    // set config register according to parameters
    uint8_t toWrite = tempMr << DPS310__REG_SHIFT_TEMP_MR;
    toWrite |= tempOsr << DPS310__REG_SHIFT_TEMP_OSR;
    // using recommended temperature sensor
    toWrite |= DPS310__REG_MASK_TEMP_SENSOR &
               (dps310_ctx.m_tempSensor << DPS310__REG_SHIFT_TEMP_SENSOR);
    int16_t ret = writeByte(DPS310__REG_ADR_TEMP_MR, toWrite);
    // abort immediately on fail
    if (ret != DPS310__SUCCEEDED) {
        return DPS310__FAIL_UNKNOWN;
    }

    // set TEMP SHIFT ENABLE if oversampling rate higher than eight(2^3)
    if (tempOsr > DPS310__OSR_SE) {
        ret = writeByteBitfield(1U, DPS310__REG_INFO_TEMP_SE);
    } else {
        ret = writeByteBitfield(0U, DPS310__REG_INFO_TEMP_SE);
    }

    if (ret == DPS310__SUCCEEDED) { // save new settings
        dps310_ctx.m_tempMr  = tempMr;
        dps310_ctx.m_tempOsr = tempOsr;
    } else {
        // try to rollback on fail avoiding endless recursion
        // this is to make sure that shift enable and oversampling rate
        // are always consistent
        if (tempMr != dps310_ctx.m_tempMr || tempOsr != dps310_ctx.m_tempOsr) {
            configTemp(dps310_ctx.m_tempMr, dps310_ctx.m_tempOsr);
        }
    }
    return ret;
}


/**
 * starts a single temperature measurement
 * The desired precision can be set with oversamplingRate
 *
 * oversamplingRate: 	a value from 0 to 7 that decides about the precision
 * 						of the measurement
 * 						If this value equals n, the DPS310 will perform
 * 						2^n measurements and combine the results
 * returns: 			0 on success
 * 						-3 if the DPS310 is already busy
 * 						-2 if the object initialization failed
 * 						-1 on other fail
 */
int16_t startMeasureTempOnce(uint8_t oversamplingRate)
{
    // abort if initialization failed
    if (dps310_ctx.m_initFail) {
        return DPS310__FAIL_INIT_FAILED;
    }
    // abort if device is not in idling mode
    if (dps310_ctx.m_opMode != IDLE) {
        return DPS310__FAIL_TOOBUSY;
    }

    if (oversamplingRate != dps310_ctx.m_tempOsr) {
        // configuration of oversampling rate
        if (configTemp(0U, oversamplingRate) != DPS310__SUCCEEDED) {
            return DPS310__FAIL_UNKNOWN;
        }
    }

    // set device to temperature measuring mode
    return setOpMode_3param(0U, 1U, 0U);
}


/**
 * performs one temperature measurement and writes result to the given address
 * the desired precision can be set with oversamplingRate
 *
 * &result:				reference to a 32-Bit signed Integer where the result
 * will be written It will not be written if result==NULL oversamplingRate: a
 * value from 0 to 7 that decides about the precision of the measurement If this
 * value equals n, the DPS310 will perform 2^n measurements and combine the
 * results returns: 			0 on success -4 if the DPS310 is could not
 * finish its measurement in time -3 if the DPS310 is already busy -2 if the
 * object initialization failed -1 on other fail
 */
int16_t measureTempOnce(int32_t *result, uint8_t oversamplingRate)
{
    // Start measurement
    int16_t ret = startMeasureTempOnce(oversamplingRate);

    if (ret != DPS310__SUCCEEDED) {
        return ret;
    }

    // wait until measurement is finished
    delay(calcBusyTime(0U, dps310_ctx.m_tempOsr) / DPS310__BUSYTIME_SCALING);
    delay(DPS310__BUSYTIME_FAILSAFE);

    ret = getSingleResult(result);
    if (ret != DPS310__SUCCEEDED) {
        standby();
    }
    return ret;
}

/**
 * performs one pressure measurement and writes result to the given address
 * the desired precision can be set with oversamplingRate
 *
 * &result:				reference to a 32-Bit signed Integer where the result
 * will be written It will not be written if result==NULL oversamplingRate: a
 * value from 0 to 7 that decides about the precision of the measurement If this
 * value equals n, the DPS310 will perform 2^n measurements and combine the
 * results returns: 			0 on success -4 if the DPS310 is could not
 * finish its measurement in time -3 if the DPS310 is already busy -2 if the
 * object initialization failed -1 on other fail
 */
int16_t measurePressureOnce(int32_t *result, uint8_t oversamplingRate)
{
    // start the measurement
    int16_t ret = startMeasurePressureOnce(oversamplingRate);
    if (ret != DPS310__SUCCEEDED) {
        return ret;
    }

    // wait until measurement is finished
    delay(calcBusyTime(0U, dps310_ctx.m_prsOsr) / DPS310__BUSYTIME_SCALING);
    delay(DPS310__BUSYTIME_FAILSAFE);

    ret = getSingleResult(result);
    if (ret != DPS310__SUCCEEDED) {
        standby();
    }
    return ret;
}

/**
 * performs one temperature measurement and writes result to the given address
 *
 * &result:		reference to a 32-Bit signed Integer value where the result will
 * be written It will not be written if result==NULL returns: 	0 on success -4
 * if the DPS310 is could not finish its measurement in time -3 if the DPS310 is
 * already busy -2 if the object initialization failed -1 on other fail
 */
int16_t measureTempOnce_1param(int32_t *result)
{
    return measureTempOnce(result, dps310_ctx.m_tempOsr);
}

/**
 * reads the compensation coefficients from the DPS310
 * this is called once from init(), which is called from begin()
 *
 * returns: 	0 on success, -1 on fail
 */
int16_t readcoeffs(void)
{
    uint8_t buffer[DPS310__REG_LEN_COEF];
    // read COEF registers to buffer
    int16_t ret = readBlock(DPS310__REG_ADR_COEF, DPS310__REG_LEN_COEF, buffer);
    // abort if less than REG_LEN_COEF bytes were read
    if (ret < DPS310__REG_LEN_COEF) {
        return DPS310__FAIL_UNKNOWN;
    }

    // compose coefficients from buffer content
    dps310_ctx.m_c0Half =
      ((uint32_t)buffer[0] << 4) | (((uint32_t)buffer[1] >> 4) & 0x0F);
    // this construction recognizes non-32-bit negative numbers
    // and converts them to 32-bit negative numbers with 2's complement
    if (dps310_ctx.m_c0Half & ((uint32_t)1 << 11)) {
        dps310_ctx.m_c0Half -= (uint32_t)1 << 12;
    }
    // c0 is only used as c0*0.5, so c0_half is calculated immediately
    dps310_ctx.m_c0Half = dps310_ctx.m_c0Half / 2U;

    // now do the same thing for all other coefficients
    dps310_ctx.m_c1 = (((uint32_t)buffer[1] & 0x0F) << 8) | (uint32_t)buffer[2];
    if (dps310_ctx.m_c1 & ((uint32_t)1 << 11)) {
        dps310_ctx.m_c1 -= (uint32_t)1 << 12;
    }

    dps310_ctx.m_c00 = ((uint32_t)buffer[3] << 12) |
                       ((uint32_t)buffer[4] << 4) |
                       (((uint32_t)buffer[5] >> 4) & 0x0F);
    if (dps310_ctx.m_c00 & ((uint32_t)1 << 19)) {
        dps310_ctx.m_c00 -= (uint32_t)1 << 20;
    }

    dps310_ctx.m_c10 = (((uint32_t)buffer[5] & 0x0F) << 16) |
                       ((uint32_t)buffer[6] << 8) | (uint32_t)buffer[7];
    if (dps310_ctx.m_c10 & ((uint32_t)1 << 19)) {
        dps310_ctx.m_c10 -= (uint32_t)1 << 20;
    }

    dps310_ctx.m_c01 = ((uint32_t)buffer[8] << 8) | (uint32_t)buffer[9];
    if (dps310_ctx.m_c01 & ((uint32_t)1 << 15)) {
        dps310_ctx.m_c01 -= (uint32_t)1 << 16;
    }

    dps310_ctx.m_c11 = ((uint32_t)buffer[10] << 8) | (uint32_t)buffer[11];
    if (dps310_ctx.m_c11 & ((uint32_t)1 << 15)) {
        dps310_ctx.m_c11 -= (uint32_t)1 << 16;
    }

    dps310_ctx.m_c20 = ((uint32_t)buffer[12] << 8) | (uint32_t)buffer[13];
    if (dps310_ctx.m_c20 & ((uint32_t)1 << 15)) {
        dps310_ctx.m_c20 -= (uint32_t)1 << 16;
    }

    dps310_ctx.m_c21 = ((uint32_t)buffer[14] << 8) | (uint32_t)buffer[15];
    if (dps310_ctx.m_c21 & ((uint32_t)1 << 15)) {
        dps310_ctx.m_c21 -= (uint32_t)1 << 16;
    }

    dps310_ctx.m_c30 = ((uint32_t)buffer[16] << 8) | (uint32_t)buffer[17];
    if (dps310_ctx.m_c30 & ((uint32_t)1 << 15)) {
        dps310_ctx.m_c30 -= (uint32_t)1 << 16;
    }

    return DPS310__SUCCEEDED;
}

/**
 * Function to fix a hardware problem on some devices
 * You have this problem if you measure a temperature which is too high (e.g.
 * 60°C when temperature is around 20°C) Call correctTemp() directly after
 * begin() to fix this issue
 */
int16_t correctTemp(void)
{
    if (dps310_ctx.m_initFail) {
        return DPS310__FAIL_INIT_FAILED;
    }
    writeByte(0x0E, 0xA5);
    writeByte(0x0F, 0x96);
    writeByte(0x62, 0x02);
    writeByte(0x0E, 0x00);
    writeByte(0x0F, 0x00);

    // perform a first temperature measurement (again)
    // the most recent temperature will be saved internally
    // and used for compensation when calculating pressure
    int32_t trash;
    measureTempOnce_1param(&trash);

    return DPS310__SUCCEEDED;
}


/**
 * Initializes the sensor.
 * This function has to be called from begin()
 * and requires a valid bus initialization.
 */
void init(void)
{
    int16_t prodId = readByteBitfield(DPS310__REG_INFO_PROD_ID);
    if (prodId != DPS310__PROD_ID) {
        // Connected device is not a Dps310
        dps310_ctx.m_initFail = 1U;
        return;
    }
    dps310_ctx.m_productID = prodId;
    int16_t revId          = readByteBitfield(DPS310__REG_INFO_REV_ID);
    if (revId < 0) {
        dps310_ctx.m_initFail = 1U;
        return;
    }
    dps310_ctx.m_revisionID = revId;
    // find out which temperature sensor is calibrated with coefficients...
    int16_t sensor = readByteBitfield(DPS310__REG_INFO_TEMP_SENSORREC);
    if (sensor < 0) {
        dps310_ctx.m_initFail = 1U;
        return;
    }

    //...and use this sensor for temperature measurement
    dps310_ctx.m_tempSensor = sensor;
    if (writeByteBitfield((uint8_t)sensor, DPS310__REG_INFO_TEMP_SENSOR) < 0) {
        dps310_ctx.m_initFail = 1U;
        return;
    }

    // read coefficients
    if (readcoeffs() < 0) {
        dps310_ctx.m_initFail = 1U;
        return;
    }

    // set to standby for further configuration
    standby();

    // set measurement precision and rate to standard values;
    configTemp(DPS310__TEMP_STD_MR, DPS310__TEMP_STD_OSR);
    configPressure(DPS310__PRS_STD_MR, DPS310__PRS_STD_OSR);

    // perform a first temperature measurement
    // the most recent temperature will be saved internally
    // and used for compensation when calculating pressure
    int32_t trash;
    measureTempOnce(&trash, dps310_ctx.m_tempOsr);

    // make sure the DPS310 is in standby after initialization
    standby();

    // Fix IC with a fuse bit problem, which lead to a wrong temperature
    // Should not affect ICs without this problem
    correctTemp();
}


static int drv_baro_ifx_dps310_open(void)
{
    init();
    return 0;
}

static int drv_baro_ifx_dps310_close(void)
{
    return 0;
}

static int drv_baro_ifx_dps310_read(void *buf, size_t len)
{

    int32_t pressure = 0;
    // int32_t temp = 0;
    size_t            size;

    barometer_data_t *pdata = (barometer_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(barometer_data_t);
    if (len < size) {
        return -1;
    }

    {
        // measureTempOnce(&temp, 7);
        // printf("temp: %d\r\n", temp);
        measurePressureOnce(&pressure, 7);
        //printf("pressure: %d\r\n", pressure);
    }

    pdata->p         = pressure;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_baro_ifx_dps310_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_baro_ifx_dps310_ioctl(int cmd, unsigned long arg)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_baro_ifx_dps310_irq_handle(void)
{
    /* no handle so far */
}

int drv_baro_ifx_dps310_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
	
    /* fill the sensor obj parameters here */
    sensor.tag        = TAG_DEV_BARO;
    sensor.path       = dev_baro_path;
    sensor.io_port    = I2C_PORT;
    sensor.open       = drv_baro_ifx_dps310_open;
    sensor.close      = drv_baro_ifx_dps310_close;
    sensor.read       = drv_baro_ifx_dps310_read;
    sensor.write      = drv_baro_ifx_dps310_write;
    sensor.ioctl      = drv_baro_ifx_dps310_ioctl;
    sensor.irq_handle = drv_baro_ifx_dps310_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_baro_ifx_dps310_init);