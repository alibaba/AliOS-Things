/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. March 2016
 * $Revision:    V1.0
 *
 * Project:      SCU Driver Definitions for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#ifndef __SCU_LPC18XX_H
#define __SCU_LPC18XX_H

#include <stdint.h>

// Pin identifier
typedef struct _PIN_ID {
  uint8_t       port;
  uint8_t       num;
  uint32_t      config_val;
} PIN_ID;

//------------------------------------------------------------------------------
// SCU REGISTER BIT DEFINITIONS
//------------------------------------------------------------------------------
// SCU Pin configuration register for normal-drive pins
#define SCU_SFS_MODE_POS              (         0 )
#define SCU_SFS_MODE_MSK              (7    <<  SCU_SFS_MODE_POS)
#define SCU_SFS_EPD                   (1    <<  3)
#define SCU_SFS_EPUN                  (1    <<  4)
#define SCU_SFS_EHS                   (1    <<  5)
#define SCU_SFS_EZI                   (1    <<  6)
#define SCU_SFS_ZIF                   (1    <<  7)

// SCU Pin configuration register for high-drive pins
//    P1_17
//    P2_3 to P2_5
//    P8_0 to P8_2
//    PA_1 to PA_3
#define SCU_SFS_EHD_POS               (         8 )
#define SCU_SFS_EHD_MSK               (3    <<  SCU_SFS_EHD_POS)

// SCU Pin configuration register for USB1 pins USB1_DP/USB1_DM
#define SCU_SFSUSB_AIM                (1    <<  0)
#define SCU_SFSUSB_ESEA               (1    <<  1)
#define SCU_SFSUSB_EPD                (1    <<  2)
#define SCU_SFSUSB_EPWR               (1    <<  4)
#define SCU_SFSUSB_VBUS               (1    <<  5)

// SCU Pin configuration for open-drain I2C pins
#define SCU_SFSI2C_SCL_EFP            (1    <<  0)
#define SCU_SFSI2C_SCL_EHD            (1    <<  2)
#define SCU_SFSI2C_SCL_EZI            (1    <<  3)
#define SCU_SFSI2C_SCL_ZIF            (1    <<  7)
#define SCU_SFSI2C_SDA_EFP            (1    <<  8)
#define SCU_SFSI2C_SDA_EHD            (1    << 10)
#define SCU_SFSI2C_SDA_EZI            (1    << 11)
#define SCU_SFSI2C_SDA_ZIF            (1    << 15)

// SCU Analog function select register ENAIO2
#define SCU_ENAIO2_DAC                (1    <<  0)
#define SCU_ENAIO2_BG                 (1    <<  4)



//------------------------------------------------------------------------------
// SCU FUNCTION PARAMETER DEFINITIONS
//------------------------------------------------------------------------------
// SCU_PIN_CFG_MODE(mode)
#define SCU_CFG_MODE_FUNC0                    (0)
#define SCU_CFG_MODE_FUNC1                    (1)
#define SCU_CFG_MODE_FUNC2                    (2)
#define SCU_CFG_MODE_FUNC3                    (3)
#define SCU_CFG_MODE_FUNC4                    (4)
#define SCU_CFG_MODE_FUNC5                    (5)
#define SCU_CFG_MODE_FUNC6                    (6)
#define SCU_CFG_MODE_FUNC7                    (7)

// SCU_PIN_CFG_DRIVER_STRENGTH(ehd) (only for high drive pins)
#define SCU_CFG_EHD_4mA                       (0)
#define SCU_CFG_EHD_8mA                       (1)
#define SCU_CFG_EHD_14mA                      (2)
#define SCU_CFG_EHD_20mA                      (3)

// pin_cfg (configuration bit mask)
#define SCU_PIN_CFG_MODE(mode)                (((mode) & SCU_SFS_MODE_MSK) << SCU_SFS_MODE_POS)
#define SCU_PIN_CFG_PULLDOWN_EN               ( SCU_SFS_EPD )
#define SCU_PIN_CFG_PULLUP_DIS                ( SCU_SFS_EPUN)
#define SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN   ( SCU_SFS_EHS )
#define SCU_PIN_CFG_INPUT_BUFFER_EN           ( SCU_SFS_EZI )
#define SCU_PIN_CFG_INPUT_FILTER_DIS          ( SCU_SFS_ZIF )
// additional pin_cfg for high-drive pins only
#define SCU_PIN_CFG_DRIVER_STRENGTH(ehd)      ((val & SCU_SFS_EHD_MSK) << SCU_SFS_EHD_POS)

// USB1_pin_cfg (configuration bit mask)
#define SCU_USB1_PIN_CFG_AIM                  (SCU_SFSUSB_AIM )
#define SCU_USB1_PIN_CFG_ESEA                 (SCU_SFSUSB_ESEA)
#define SCU_USB1_PIN_CFG_EPD                  (SCU_SFSUSB_EPD )
#define SCU_USB1_PIN_CFG_EPWR                 (SCU_SFSUSB_EPWR)
#define SCU_USB1_PIN_CFG_VBUS                 (SCU_SFSUSB_VBUS)

// I2C_mode
#define SCU_I2C_PIN_MODE_DISABLED             (0)
#define SCU_I2C_PIN_MODE_STANDARD_FAST        (SCU_SFSI2C_SCL_EZI | SCU_SFSI2C_SDA_EZI)
#define SCU_I2C_PIN_MODE_FAST_PLUS            (SCU_SFSI2C_SCL_EHD | SCU_SFSI2C_SCL_EZI | \
                                               SCU_SFSI2C_SCL_EZI | SCU_SFSI2C_SDA_EHD | \
                                               SCU_SFSI2C_SDA_EZI)


/**
  \fn          int32_t SCU_PinConfiguare (uint8_t port, uint8_t pin, uint32_t pin_cfg)
  \brief       Set pin function and electrical characteristics
  \param[in]   port       Port number (0..15)
  \param[in]   pin        Pin number (0..31)
  \param[in]   pin_cfg    pin_cfg configuration bit mask
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t SCU_PinConfigure (uint8_t port, uint8_t pin, uint32_t pin_cfg);

/**
  \fn          int32_t SCU_CLK_PinConfigure (uint8_t clk_pin, uint32_t pin_cfg)
  \brief       Set pin function and electrical characteristics for CLK pins
  \param[in]   clk_pin    Clock pin number should be 0..3
  \param[in]   pin_cfg    pin_cfg
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t SCU_CLK_PinConfigure (uint8_t pin_clk, uint32_t pin_cfg);

/**
  \fn          int32_t SCU_USB1_PinConfigure (uint32_t USB1_pin_cfg)
  \brief       Pin configuration for USB1 USB_DP/USBDM  pins
  \param[in]   USB1_pin_cfg   USB1_pin_cfg configuration bit mask
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t SCU_USB1_PinConfigure (uint32_t USB1_pin_cfg);

/**
  \fn          int32_t SCU_I2C_PinConfigure (uint32_t I2C_mode)
  \brief       Set I2C pin configuration
  \param[in]   I2C_mode:  SCU_I2C_PIN_MODE_DISABLED
                          SCU_I2C_PIN_MODE_STANDARD_FAST
                          SCU_I2C_PIN_MODE_FAST_PLUS
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t SCU_I2C_PinConfigure (uint32_t I2C_mode);

/**
  \fn          int32_t SCU_ADC_ChannelPinConfigure (uint8_t ADC_num, uint8_t channel, uint32_t cmd)
  \brief       ADC Channel configuration
  \param[in]   ADC_num:  0 = ADC0, 1 = ADC1
  \param[in]   channel:  channel number 0..7
  \param[in]   cmd:      1 - enabled, 0 - disabled
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t SCU_ADC_ChannelPinConfigure (uint8_t ADC_num, uint8_t channel, uint32_t cmd);

/**
  \fn          int32_t SCU_DAC_PinConfigure (uint32_t cmd)
  \brief       Analog function on P4_4
  \param[in]   cmd:      1 - enabled, 0 - disabled
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t SCU_DAC_PinConfigure (uint32_t cmd);

/**
  \fn          int32_t SCU_PinInterruptSourceSelect (uint8_t pin_int, uint8_t port, uint8_t pin)
  \brief       Select interrupt source pin
  \param[in]   pin_int:  pin interrupt 0..7
  \param[in]   port:     GPIO port number 0..7
  \param[in]   pin:      GPIO pin number 0..31
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t SCU_PinInterruptSourceSelect (uint8_t pin_int, uint8_t port, uint8_t pin);

#endif /* __SCU_LPC18XX_H */
