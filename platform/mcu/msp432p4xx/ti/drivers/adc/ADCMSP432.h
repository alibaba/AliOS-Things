/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
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
/** ============================================================================
 *  @file       ADCMSP432.h
 *
 *  @brief      ADC driver implementation for the ADC peripheral on MSP432
 *
 *  This ADC driver implementation is designed to operate on a ADC14 peripheral
 *  for MSP432.  The ADC MSP432 header file should be included in an application
 *  as follows:
 *  @code
 *  #include <ti/drivers/ADC.h>
 *  #include <ti/drivers/ADCMSP432.h>
 *  @endcode
 *
 *  Refer to @ref ADC.h for a complete description of APIs & example of use.
 *
 *  ============================================================================
 */
#ifndef ti_drivers_adc_ADCMSP432__include
#define ti_drivers_adc_ADCMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/ADC.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>

/*
 *  ADC port/pin defines for pin configuration.  Ports P2, P3, and P7 are
 *  configurable through the port mapping controller.  None of the port
 *  mappings support ADC.
 *  Channel specifies the ADC channel and ranges from 0 to 23.
 *  pin range: 0 - 7, port range: 0 - 15
 *
 *
 *    15 - 10    9   8  7 - 4  3 - 0
 *  ---------------------------------
 *  |  CHANNEL | X | X | PORT | PIN |
 *  ---------------------------------
 *
 *  channel = pinConfig >> 10
 *  port = (pinConfig >> 4) & 0xf
 *  pin = 1 << (pinConfig & 0xf)
 *  moduleFunction = (pinConfig >> 8) & 0x3
 *
 *  For non-configurable ports (bits 20 - 12 will be 0).
 *  Bits 8 and 9 hold the module function (PRIMARY, SECONDARY, or
 *  TERTIALRY).  All ADC modes are TERTIALRY, so bits 8 and 9
 *  are 1.
 *
 *  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port,
 *       pin, moduleFunction);
 *  or:
 *  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port,
 *       pin, moduleFunction);
 *
 */
/* Port 4 */
#define ADCMSP432_P4_0_A13 ((13 << 10) | 0x0340) /* ch 13, port 4, pin 0 */
#define ADCMSP432_P4_1_A12 ((12 << 10) | 0x0341) /* ch 12, port 4, pin 1 */
#define ADCMSP432_P4_2_A11 ((11 << 10) | 0x0342) /* ch 11, port 4, pin 2 */
#define ADCMSP432_P4_3_A10 ((10 << 10) | 0x0343) /* ch 10, port 4, pin 3 */
#define ADCMSP432_P4_4_A9  ((9 << 10) | 0x0344)  /* ch 9, port 4, pin 4 */
#define ADCMSP432_P4_5_A8  ((8 << 10) | 0x0345)  /* ch 8, port 4, pin 5 */
#define ADCMSP432_P4_6_A7  ((7 << 10) | 0x0346)  /* ch 7, port 4, pin 6 */
#define ADCMSP432_P4_7_A6  ((6 << 10) | 0x0347)  /* ch 6, port 4, pin 7 */

/* Port 5 */
#define ADCMSP432_P5_0_A5  ((5 << 10) | 0x0350)  /* ch 5, port 5, pin 0 */
#define ADCMSP432_P5_1_A4  ((4 << 10) | 0x0351)  /* ch 4, port 5, pin 1 */
#define ADCMSP432_P5_2_A3  ((3 << 10) | 0x0352)  /* ch 3, port 5, pin 2 */
#define ADCMSP432_P5_3_A2  ((2 << 10) | 0x0353)  /* ch 2, port 5, pin 3 */
#define ADCMSP432_P5_4_A1  ((1 << 10) | 0x0354)  /* ch 1, port 5, pin 4 */
#define ADCMSP432_P5_5_A0  ((0 << 10) | 0x0355)  /* ch 0, port 5, pin 5 */

/* Port 6 */
#define ADCMSP432_P6_0_A15 ((15 << 10) | 0x0360)  /* ch 15, port 6, pin 0 */
#define ADCMSP432_P6_1_A14 ((14 << 10) | 0x0361)  /* ch 14, port 6, pin 1 */

/* Port 8 */
#define ADCMSP432_P8_2_A23 ((23 << 10) | 0x0382)  /* ch 23, port 8, pin 2 */
#define ADCMSP432_P8_3_A22 ((22 << 10) | 0x0383)  /* ch 22, port 8, pin 3 */
#define ADCMSP432_P8_4_A21 ((21 << 10) | 0x0384)  /* ch 21, port 8, pin 4 */
#define ADCMSP432_P8_5_A20 ((20 << 10) | 0x0385)  /* ch 20, port 8, pin 5 */
#define ADCMSP432_P8_6_A19 ((19 << 10) | 0x0386)  /* ch 19, port 8, pin 6 */
#define ADCMSP432_P8_7_A18 ((18 << 10) | 0x0387)  /* ch 18, port 8, pin 7 */

/* Port 9 */
#define ADCMSP432_P9_0_A17 ((17 << 10) | 0x0390)  /* ch 17, port 9, pin 0 */
#define ADCMSP432_P9_1_A16 ((16 << 10) | 0x0391)  /* ch 16, port 9, pin 1 */

/*
 * ADCMSP432_HWAttrsV1.refVoltage values
 */
#define ADCMSP432_REF_VOLTAGE_VDD        ADC_VREFPOS_AVCC_VREFNEG_VSS | 0xFF  /* 0x00FF */
#define ADCMSP432_REF_VOLTAGE_INT_1_2V   REF_A_VREF1_2V  /* 0x0000 */
#define ADCMSP432_REF_VOLTAGE_INT_1_45V  REF_A_VREF1_45V /* 0x0010 */
#define ADCMSP432_REF_VOLTAGE_INT_2_5V   REF_A_VREF2_5V  /* 0x0030 */
#define ADCMSP432_REF_VOLTAGE_EXT        ADC_VREFPOS_EXTPOS_VREFNEG_EXTNEG    /* 0x0E00 */
#define ADCMSP432_REF_VOLTAGE_EXT_BUF    ADC_VREFPOS_EXTBUF_VREFNEG_EXTNEG    /* 0x0F00 */


/* ADC function table pointer */
extern const ADC_FxnTable ADCMSP432_fxnTable;

/*!
 *  @brief  ADCMSP432 Hardware attributes
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For MSPWare these definitions are found in:
 *      - adc14.h
 *      - gpio.h
 *      - ref_a.h
 *
 *  A sample structure is shown below:
 *  @code
 *  const ADCMSP432_HWAttrsV1 adcMSP432HWAttrs[Board_ADCCHANNELCOUNT] = {
 *      {
 *          .adcPin = ADCMSP432_P5_5_A0,
 *          .refVoltage = ADCMSP432_REF_VOLTAGE_INT_2_5V,
 *          .resolution = ADC_14BIT
 *      }
 *  };
 *  @endcode
 */
typedef struct ADCMSP432_HWAttrsV1 {
    uint_fast16_t  adcPin;     /*!< ADC pin, port channel */
    uint_fast16_t  refVoltage; /*!< Reference voltage for ADC channel */
    uint_fast32_t  resolution; /*!< ADC resolution for ADC channel  */
} ADCMSP432_HWAttrsV1;

/*!
 *  @brief  ADCMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct ADCMSP432_Object {
    bool isOpen;               /* To determine if the ADC is open */
    bool isProtected;          /* Flag to indicate if thread safety is ensured by the driver */
} ADCMSP432_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_adc_ADCMSP432__include */
