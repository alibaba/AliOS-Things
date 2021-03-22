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
 * $Revision:    V1.1
 *
 * Project:      SCU Driver for NXP LPC18xx
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.1
 *    - Corrected SCU_SFSCLKx(clk_pin) and SCU_ENAIOx(n) macros
 *  Version 1.0
 *    - Initial release
 */

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"

#define PORT_OFFSET          ( 0x80 )
#define PIN_OFFSET           ( 0x04 )
#define SCU_SFSPx(port, pin) (*((volatile uint32_t *) ((LPC_SCU_BASE + PORT_OFFSET * port + PIN_OFFSET * pin))))
#define SCU_SFSCLKx(clk_pin) (*((volatile uint32_t *) (&(LPC_SCU->SFSCLK_0) + clk_pin)))
#define SCU_ENAIOx(n)        (*((volatile uint32_t *) (&(LPC_SCU->ENAIO0) + n)))


/**
  \fn          int32_t SCU_PinConfiguare (uint8_t port, uint8_t pin, uint32_t pin_cfg)
  \brief       Set pin function and electrical characteristics
  \param[in]   port       Port number (0..15)
  \param[in]   pin        Pin number (0..31)
  \param[in]   pin_cfg    pin_cfg configuration bit mask
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_PinConfigure (uint8_t port, uint8_t pin, uint32_t pin_cfg) {

  if ((port > 15) || (pin > 31)) return -1;
  SCU_SFSPx(port, pin) = pin_cfg;
  return 0;
}

/**
  \fn          int32_t SCU_CLK_PinConfigure (uint8_t clk_pin, uint32_t pin_cfg)
  \brief       Set pin function and electrical characteristics for CLK pins
  \param[in]   clk_pin    Clock pin number should be 0..3
  \param[in]   pin_cfg    pin_cfg
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_CLK_PinConfigure (uint8_t pin_clk, uint32_t pin_cfg) {

  if (pin_clk > 3) return -1;
  SCU_SFSCLKx(pin_clk) = pin_cfg;
  return 0;
}

/**
  \fn          int32_t SCU_USB1_PinConfigure (uint32_t USB1_pin_cfg)
  \brief       Pin configuration for USB1 USB_DP/USBDM  pins
  \param[in]   USB1_pin_cfg   USB1_pin_cfg configuration bit mask
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_USB1_PinConfigure (uint32_t USB1_pin_cfg) {
  LPC_SCU->SFSUSB = USB1_pin_cfg;
  return 0;
}

/**
  \fn          int32_t SCU_I2C_PinConfigure (uint32_t I2C_mode)
  \brief       Set I2C pin configuration
  \param[in]   I2C_mode:  SCU_I2C_PIN_MODE_DISABLED
                          SCU_I2C_PIN_MODE_STANDARD_FAST
                          SCU_I2C_PIN_MODE_FAST_PLUS
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_I2C_PinConfigure (uint32_t I2C_mode) {

  switch (I2C_mode) {
    case SCU_I2C_PIN_MODE_DISABLED:      break;
    case SCU_I2C_PIN_MODE_STANDARD_FAST: break;
    case SCU_I2C_PIN_MODE_FAST_PLUS:     break;
    default: return -1;
  }
  LPC_SCU->SFSI2C0 = I2C_mode;
  return 0;
}

/**
  \fn          int32_t SCU_ADC_ChannelPinConfigure (uint8_t ADC_num, uint8_t channel, uint32_t cmd)
  \brief       ADC Channel configuration
  \param[in]   ADC_num:  0 = ADC0, 1 = ADC1
  \param[in]   channel:  channel number 0..7
  \param[in]   cmd:      1 - enabled, 0 - disabled
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_ADC_ChannelPinConfigure (uint8_t ADC_num, uint8_t channel, uint32_t cmd) {

  if ((ADC_num > 1) || (channel > 7) || (cmd > 1)) return -1;
  cmd ? (SCU_ENAIOx(ADC_num) |= (1 << channel)) : (SCU_ENAIOx(ADC_num) &= ~(1 << channel));
  return 0;
}

/**
  \fn          int32_t SCU_DAC_PinConfigure (uint32_t cmd)
  \brief       Analog function on P4_4
  \param[in]   cmd:      1 - enabled, 0 - disabled
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_DAC_PinConfigure (uint32_t cmd) {

  if (cmd > 1) return -1;
  cmd ? (LPC_SCU->ENAIO2 |= SCU_ENAIO2_DAC) : (LPC_SCU->ENAIO2 &= ~SCU_ENAIO2_DAC);
  return 0;
}

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
int32_t SCU_PinInterruptSourceSelect (uint8_t pin_int, uint8_t port, uint8_t pin) {

  if ((port > 7) || (pin > 31) || (pin_int > 7)) return -1;

  if (pin_int < 4) {
    LPC_SCU->PINTSEL0 &= ~(0xFF << (8 * pin_int));
    LPC_SCU->PINTSEL0 |=  ((pin | (port << 5)) << (8 * pin_int));
  } else {
    pin_int -= 4;
    LPC_SCU->PINTSEL1 &= ~(0xFF << (8 * pin_int));
    LPC_SCU->PINTSEL1 |=  ((pin | (port << 5)) << (8 * pin_int));
  }

  return 0;
}
