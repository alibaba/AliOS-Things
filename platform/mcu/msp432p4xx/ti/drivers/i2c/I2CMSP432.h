/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       I2CMSP432.h
 *
 *  @brief      I2C driver implementation for the EUSCI controller on MSP432
 *
 *  This I2C driver implementation is designed to operate on a EUCSI controller
 *  in I2C mode.  The I2CMSP432 header file should be included in an application
 *  as follows:
 *  @code
 *  #include <ti/drivers/I2C.h>
 *  #include <ti/drivers/I2CMSP432.h>
 *  @endcode
 *
 *  Refer to @ref I2C.h for a complete description of APIs & example of use.
 *
 *  ## Supported Bit Rates ##
 *    - #I2C_100kHz
 *    - #I2C_400kHz
 *    - #I2C_1000kHz
 *
 ******************************************************************************
 */

#ifndef ti_drivers_i2c_I2CMSP432__include
#define ti_drivers_i2c_I2CMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/I2C.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>

/*
 *  I2C port/pin defines for pin configuration.  Ports P2, P3, and P7 are
 *  configurable through the port mapping controller.
 *  Value specifies the pin function and ranges from 0 to 31
 *  pin range: 0 - 7, port range: 0 - 15
 *
 *
 *    15 - 10   9  8  7 - 4  3 - 0
 *  -------------------------------
 *  |  VALUE | X | X | PORT | PIN |
 *  -------------------------------
 *
 *  value = pinConfig >> 10
 *  port = (pinConfig >> 4) & 0xf
 *  pin = pinConfig & 0x7
 *
 *  pmap = port * 0x8;  // 2 -> 0x10, 3 -> 0x18, 7 -> 0x38
 *  portMapReconfigure = PMAP_ENABLE_RECONFIGURATION;
 *
 *  Code from pmap.c:
 *   //Get write-access to port mapping registers:
 *   PMAP->KEYID = PMAP_KEYID_VAL;
 *
 *   //Enable/Disable reconfiguration during runtime
 *   PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | portMapReconfigure;
 *   HWREG8(PMAP_BASE + pin + pmap) = value;
 *
 *  For non-configurable ports (bits 20 - 12 will be 0).
 *  Bits 8 and 9 hold the module function (PRIMARY, SECONDARY, or
 *  TERTIALRY).
 *
 *        9         8      7 - 4  3 - 0
 *  -----------------------------------
 *  | PnSEL1.x | PnSEL0.x | PORT | PIN |
 *  -----------------------------------
 *
 *  moduleFunction = (pinConfig >> 8) & 0x3
 *  port = (pinConfig >> 4) & 0xf
 *  pin = 1 << (pinConfig & 0xf)
 *
 *  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port,
 *       pin, moduleFunction);
 *  or:
 *  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port,
 *       pin, moduleFunction);
 *
 */

#define I2CMSP432_P1_6_UCB0SDA   0x00000116
/*!< Port 1 Pin 6 is EUSCI B0 SDA */
#define I2CMSP432_P1_7_UCB0SCL   0x00000117
/*!< Port 1 Pin 7 is EUSCI B0 SCL */

#define I2CMSP432_P2_0_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x20)
/*!< Port 2 Pin 0 is EUSCI B0 SDA */
#define I2CMSP432_P2_0_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x20)
/*!< Port 2 Pin 0 is EUSCI B0 SCL */
#define I2CMSP432_P2_0_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x20)
/*!< Port 2 Pin 0 is EUSCI B2 SDA */
#define I2CMSP432_P2_0_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x20)
/*!< Port 2 Pin 0 is EUSCI B2 SCL */

#define I2CMSP432_P2_1_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x21)
/*!< Port 2 Pin 1 is EUSCI B0 SDA */
#define I2CMSP432_P2_1_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x21)
/*!< Port 2 Pin 1 is EUSCI B0 SCL */
#define I2CMSP432_P2_1_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x21)
/*!< Port 2 Pin 1 is EUSCI B2 SDA */
#define I2CMSP432_P2_1_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x21)
/*!< Port 2 Pin 1 is EUSCI B2 SCL */

#define I2CMSP432_P2_2_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x22)
/*!< Port 2 Pin 2 is EUSCI B0 SDA */
#define I2CMSP432_P2_2_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x22)
/*!< Port 2 Pin 2 is EUSCI B0 SCL */
#define I2CMSP432_P2_2_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x22)
/*!< Port 2 Pin 2 is EUSCI B2 SDA */
#define I2CMSP432_P2_2_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x22)
/*!< Port 2 Pin 2 is EUSCI B2 SCL */

#define I2CMSP432_P2_3_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x23)
/*!< Port 2 Pin 3 is EUSCI B0 SDA */
#define I2CMSP432_P2_3_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x23)
/*!< Port 2 Pin 3 is EUSCI B0 SCL */
#define I2CMSP432_P2_3_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x23)
/*!< Port 2 Pin 3 is EUSCI B2 SDA */
#define I2CMSP432_P2_3_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x23)
/*!< Port 2 Pin 3 is EUSCI B2 SCL */

#define I2CMSP432_P2_4_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x24)
/*!< Port 2 Pin 4 is EUSCI B0 SDA */
#define I2CMSP432_P2_4_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x24)
/*!< Port 2 Pin 4 is EUSCI B0 SCL */
#define I2CMSP432_P2_4_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x24)
/*!< Port 2 Pin 4 is EUSCI B2 SDA */
#define I2CMSP432_P2_4_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x24)
/*!< Port 2 Pin 4 is EUSCI B2 SCL */

#define I2CMSP432_P2_5_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x25)
/*!< Port 2 Pin 5 is EUSCI B0 SDA */
#define I2CMSP432_P2_5_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x25)
/*!< Port 2 Pin 5 is EUSCI B0 SCL */
#define I2CMSP432_P2_5_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x25)
/*!< Port 2 Pin 5 is EUSCI B2 SDA */
#define I2CMSP432_P2_5_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x25)
/*!< Port 2 Pin 5 is EUSCI B2 SCL */

#define I2CMSP432_P2_6_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x26)
/*!< Port 2 Pin 6 is EUSCI B0 SDA */
#define I2CMSP432_P2_6_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x26)
/*!< Port 2 Pin 6 is EUSCI B0 SCL */
#define I2CMSP432_P2_6_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x26)
/*!< Port 2 Pin 6 is EUSCI B2 SDA */
#define I2CMSP432_P2_6_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x26)
/*!< Port 2 Pin 6 is EUSCI B2 SCL */

#define I2CMSP432_P2_7_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x27)
/*!< Port 2 Pin 7 is EUSCI B0 SDA */
#define I2CMSP432_P2_7_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x27)
/*!< Port 2 Pin 7 is EUSCI B0 SCL */
#define I2CMSP432_P2_7_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x27)
/*!< Port 2 Pin 7 is EUSCI B2 SDA */
#define I2CMSP432_P2_7_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x27)
/*!< Port 2 Pin 7 is EUSCI B2 SCL */

#define I2CMSP432_P3_0_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x30)
/*!< Port 3 Pin 0 is EUSCI B0 SDA */
#define I2CMSP432_P3_0_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x30)
/*!< Port 3 Pin 0 is EUSCI B0 SCL */
#define I2CMSP432_P3_0_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x30)
/*!< Port 3 Pin 0 is EUSCI B2 SDA */
#define I2CMSP432_P3_0_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x30)
/*!< Port 3 Pin 0 is EUSCI B2 SCL */

#define I2CMSP432_P3_1_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x31)
/*!< Port 3 Pin 1 is EUSCI B0 SDA */
#define I2CMSP432_P3_1_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x31)
/*!< Port 3 Pin 1 is EUSCI B0 SCL */
#define I2CMSP432_P3_1_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x31)
/*!< Port 3 Pin 1 is EUSCI B2 SDA */
#define I2CMSP432_P3_1_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x31)
/*!< Port 3 Pin 1 is EUSCI B2 SCL */

#define I2CMSP432_P3_2_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x32)
/*!< Port 3 Pin 2 is EUSCI B0 SDA */
#define I2CMSP432_P3_2_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x32)
/*!< Port 3 Pin 2 is EUSCI B0 SCL */
#define I2CMSP432_P3_2_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x32)
/*!< Port 3 Pin 2 is EUSCI B2 SDA */
#define I2CMSP432_P3_2_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x32)
/*!< Port 3 Pin 2 is EUSCI B2 SCL */

#define I2CMSP432_P3_3_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x33)
/*!< Port 3 Pin 3 is EUSCI B0 SDA */
#define I2CMSP432_P3_3_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x33)
/*!< Port 3 Pin 3 is EUSCI B0 SCL */
#define I2CMSP432_P3_3_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x33)
/*!< Port 3 Pin 3 is EUSCI B2 SDA */
#define I2CMSP432_P3_3_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x33)
/*!< Port 3 Pin 3 is EUSCI B2 SCL */

#define I2CMSP432_P3_4_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x34)
/*!< Port 3 Pin 4 is EUSCI B0 SDA */
#define I2CMSP432_P3_4_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x34)
/*!< Port 3 Pin 4 is EUSCI B0 SCL */
#define I2CMSP432_P3_4_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x34)
/*!< Port 3 Pin 4 is EUSCI B2 SDA */
#define I2CMSP432_P3_4_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x34)
/*!< Port 3 Pin 4 is EUSCI B2 SCL */

#define I2CMSP432_P3_5_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x35)
/*!< Port 3 Pin 5 is EUSCI B0 SDA */
#define I2CMSP432_P3_5_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x35)
/*!< Port 3 Pin 5 is EUSCI B0 SCL */
#define I2CMSP432_P3_5_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x35)
/*!< Port 3 Pin 5 is EUSCI B2 SDA */
#define I2CMSP432_P3_5_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x35)
/*!< Port 3 Pin 5 is EUSCI B2 SCL */

#define I2CMSP432_P3_6_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x36)
/*!< Port 3 Pin 6 is EUSCI B0 SDA */
#define I2CMSP432_P3_6_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x36)
/*!< Port 3 Pin 6 is EUSCI B0 SCL */
#define I2CMSP432_P3_6_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x36)
/*!< Port 3 Pin 6 is EUSCI B2 SDA */
#define I2CMSP432_P3_6_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x36)
/*!< Port 3 Pin 6 is EUSCI B2 SCL */

#define I2CMSP432_P3_7_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x37)
/*!< Port 3 Pin 7 is EUSCI B0 SDA */
#define I2CMSP432_P3_7_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x37)
/*!< Port 3 Pin 7 is EUSCI B0 SCL */
#define I2CMSP432_P3_7_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x37)
/*!< Port 3 Pin 7 is EUSCI B2 SDA */
#define I2CMSP432_P3_7_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x37)
/*!< Port 3 Pin 7 is EUSCI B2 SCL */

#define I2CMSP432_P6_4_UCB1SDA   0x00000164
/*!< Port 6 Pin 4 is EUSCI B1 SDA */
#define I2CMSP432_P6_5_UCB1SCL   0x00000165
/*!< Port 6 Pin 5 is EUSCI B1 SCL */
#define I2CMSP432_P6_6_UCB3SDA   0x00000266
/*!< Port 6 Pin 6 is EUSCI B3 SDA */
#define I2CMSP432_P6_7_UCB3SCL   0x00000267
/*!< Port 6 Pin 7 is EUSCI B3 SCL */

#define I2CMSP432_P7_0_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x70)
/*!< Port 7 Pin 0 is EUSCI B0 SDA */
#define I2CMSP432_P7_0_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x70)
/*!< Port 7 Pin 0 is EUSCI B0 SCL */
#define I2CMSP432_P7_0_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x70)
/*!< Port 7 Pin 0 is EUSCI B2 SDA */
#define I2CMSP432_P7_0_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x70)
/*!< Port 7 Pin 0 is EUSCI B2 SCL */

#define I2CMSP432_P7_1_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x71)
/*!< Port 7 Pin 1 is EUSCI B0 SDA */
#define I2CMSP432_P7_1_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x71)
/*!< Port 7 Pin 1 is EUSCI B0 SCL */
#define I2CMSP432_P7_1_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x71)
/*!< Port 7 Pin 1 is EUSCI B2 SDA */
#define I2CMSP432_P7_1_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x71)
/*!< Port 7 Pin 1 is EUSCI B2 SCL */

#define I2CMSP432_P7_2_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x72)
/*!< Port 7 Pin 2 is EUSCI B0 SDA */
#define I2CMSP432_P7_2_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x72)
/*!< Port 7 Pin 2 is EUSCI B0 SCL */
#define I2CMSP432_P7_2_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x72)
/*!< Port 7 Pin 2 is EUSCI B2 SDA */
#define I2CMSP432_P7_2_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x72)
/*!< Port 7 Pin 2 is EUSCI B2 SCL */

#define I2CMSP432_P7_3_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x73)
/*!< Port 7 Pin 3 is EUSCI B0 SDA */
#define I2CMSP432_P7_3_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x73)
/*!< Port 7 Pin 3 is EUSCI B0 SCL */
#define I2CMSP432_P7_3_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x73)
/*!< Port 7 Pin 3 is EUSCI B2 SDA */
#define I2CMSP432_P7_3_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x73)
/*!< Port 7 Pin 3 is EUSCI B2 SCL */

#define I2CMSP432_P7_4_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x74)
/*!< Port 7 Pin 4 is EUSCI B0 SDA */
#define I2CMSP432_P7_4_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x74)
/*!< Port 7 Pin 4 is EUSCI B0 SCL */
#define I2CMSP432_P7_4_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x74)
/*!< Port 7 Pin 4 is EUSCI B2 SDA */
#define I2CMSP432_P7_4_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x74)
/*!< Port 7 Pin 4 is EUSCI B2 SCL */

#define I2CMSP432_P7_5_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x75)
/*!< Port 7 Pin 5 is EUSCI B0 SDA */
#define I2CMSP432_P7_5_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x75)
/*!< Port 7 Pin 5 is EUSCI B0 SCL */
#define I2CMSP432_P7_5_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x75)
/*!< Port 7 Pin 5 is EUSCI B2 SDA */
#define I2CMSP432_P7_5_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x75)
/*!< Port 7 Pin 5 is EUSCI B2 SCL */

#define I2CMSP432_P7_6_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x76)
/*!< Port 7 Pin 6 is EUSCI B0 SDA */
#define I2CMSP432_P7_6_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x76)
/*!< Port 7 Pin 6 is EUSCI B0 SCL */
#define I2CMSP432_P7_6_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x76)
/*!< Port 7 Pin 6 is EUSCI B2 SDA */
#define I2CMSP432_P7_6_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x76)
/*!< Port 7 Pin 6 is EUSCI B2 SCL */

#define I2CMSP432_P7_7_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x77)
/*!< Port 7 Pin 7 is EUSCI B0 SDA */
#define I2CMSP432_P7_7_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x77)
/*!< Port 7 Pin 7 is EUSCI B0 SCL */
#define I2CMSP432_P7_7_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x77)
/*!< Port 7 Pin 7 is EUSCI B2 SDA */
#define I2CMSP432_P7_7_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x77)
/*!< Port 7 Pin 7 is EUSCI B2 SCL */

#define I2CMSP432_P10_2_UCB3SDA   0x000001A2
/*!< Port 10 Pin 2 is EUSCI B3 SDA */
#define I2CMSP432_P10_3_UCB3SCL   0x000001A3
/*!< Port 10 Pin 3 is EUSCI B3 SCL */

/**
 *  @addtogroup I2C_STATUS
 *  I2CMSP432_STATUS_* macros are command codes only defined in the
 *  I2CMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/i2c/I2CMSP432.h>
 *  @endcode
 *  @{
 */

/* Add I2CMSP432_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup I2C_CMD
 *  I2CMSP432_CMD_* macros are command codes only defined in the
 *  I2CMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/i2c/I2CMSP432.h>
 *  @endcode
 *  @{
 */

/* Add I2CMSP432_CMD_* macros here */

/** @}*/

/* I2C function table pointer */
extern const I2C_FxnTable I2CMSP432_fxnTable;

/*!
 *  @cond NODOC
 *  I2CMSP432 mode
 *
 *  This enum defines the state of the I2C driver's state machine.
 */
typedef enum I2CMSP432_Mode {
    I2CMSP432_IDLE_MODE = 0,  /* I2C is not performing a transaction */
    I2CMSP432_WRITE_MODE,     /* I2C is currently performing write operations */
    I2CMSP432_READ_MODE,      /* I2C is currently performing read operations */
    I2CMSP432_ERROR = 0xFF    /* I2C error has occurred, exit gracefully */
} I2CMSP432_Mode;
/*! @endcond */

/*!
 *  @brief  I2CMSP432 Hardware attributes
 *
 *  The baseAddr and intNum fields define the base address and interrupt number
 *  of the EUSCI peripheral. And the clockSource field selects the clock to
 *  be used by the EUSCI.  These values are passed directly to driverlib APIs,
 *  and therefore must be populated by driverlib macro definitions, which are
 *  found in:
 *      - driverlib/i2c.h
 *
 *  intPriority is the I2C interrupt priority.
 *  This driver uses the ti.dpl interface instead of making OS calls directly,
 *  and the corresponding HwiP port handles the interrupt priority in an
 *  OS-specific way.  For example, in the case of the TI-RTOS kernel port,
 *  the intPriority is passed unmodified to Hwi_create() provided by the
 *  ti.sysbios.family.arm.m3.Hwi module; so the documentation for the
 *  ti.sysbios.family.arm.m3.Hwi module should be referenced for a description
 *  of usage of priority.
 *
 *  clkPin and dataPin define the pin mapping to be used for the SCL and
 *  SDA pins, respectively.  Macro values defined in this header file should
 *  be used for these fields.
 *
 *  An example structure is shown below:
 *  @code
 *  const I2CMSP432_HWAttrs i2cMSP432HWAttrsV1[] = {
 *      {
 *          .baseAddr = EUSCI_B0_BASE,
 *          .intNum = INT_EUSCIB0,
 *          .intPriority = (~0),
 *          .clockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
 *          .clkPin = I2CMSP432_P1_7_UCB0SCL
 *          .dataPin = I2CMSP432_P1_6_UCB0SDA,
 *      }
 *  };
 *  @endcode
 */
typedef struct I2CMSP432_HWAttrsV1 {
    /*! EUSCI_B_I2C peripheral's base address */
    uint32_t baseAddr;
    /*! EUSCI_B_I2C peripheral's interrupt number */
    uint32_t intNum;
    /*! EUSCI_B_I2C peripheral's interrupt priority */
    uint32_t intPriority;
    /*! EUSCI_B_I2C clock source */
    uint8_t  clockSource;
    /*! EUSCI_B_I2C clock pin configuration */
    uint16_t clkPin;
    /*! EUSCI_B_I2C data pin configuration */
    uint16_t dataPin;
} I2CMSP432_HWAttrsV1;

/*!
 *  @cond NODOC
 *  I2CMSP432 Object. The application must not access any member variables of
 *  this structure!
 */
typedef struct I2CMSP432_Object {
    SemaphoreP_Handle mutex;               /* Grants exclusive access to I2C */
    SemaphoreP_Handle transferComplete;    /* Notify finished I2C transfer */

    HwiP_Handle       hwiHandle;

    I2C_Transaction  *currentTransaction;  /* Ptr to current I2C transaction */
    I2C_CallbackFxn   transferCallbackFxn; /* Callback function pointer */

    /* I2C transaction pointers for I2C_MODE_CALLBACK */
    I2C_Transaction  *headPtr;            /* Head ptr for queued transactions */
    I2C_Transaction  *tailPtr;            /* Tail ptr for queued transactions */

    uint8_t          *writeBufIdx;        /* Internal inc. writeBuf index */
    size_t            writeCountIdx;      /* Internal dec. writeCounter */

    uint8_t          *readBufIdx;         /* Internal inc. readBuf index */
    size_t            readCountIdx;       /* Internal dec. readCounter */

    Power_NotifyObj   perfChangeNotify;
    uint32_t          perfConstraintMask;

    volatile I2CMSP432_Mode mode;         /* Stores the I2C state */
    I2C_TransferMode  transferMode;       /* Blocking or Callback mode */
    I2C_BitRate       bitRate;            /* SPI bit rate in Hz */
    bool              isOpen;             /* To determine if the SPI is open */
} I2CMSP432_Object;
/*! @endcond */

I2C_Handle I2CMSP432_open(I2C_Handle handle, I2C_Params *params);
bool I2CMSP432_transfer(I2C_Handle handle, I2C_Transaction *transaction);
void I2CMSP432_close(I2C_Handle handle);


#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_i2c_I2CMSP432__include */
