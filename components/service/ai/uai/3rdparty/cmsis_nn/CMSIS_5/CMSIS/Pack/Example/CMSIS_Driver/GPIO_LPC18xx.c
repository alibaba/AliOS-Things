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
 * Project:      GPIO Driver for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#include "LPC18xx.h"
#include "GPIO_LPC18xx.h"

/**
  \fn          void GPIO_PortClock (uint32_t clock)
  \brief       Port Clock Control
  \param[in]   clock  Enable or disable clock
*/
void GPIO_PortClock (uint32_t clock) {
  if (clock) {
    LPC_CCU1->CLK_M3_GPIO_CFG |= 3;
    while (!(LPC_CCU1->CLK_M3_GPIO_STAT & 1));
  }
  else {
    LPC_CCU1->CLK_M3_GPIO_CFG   &= ~(3);
    while (LPC_CCU1->CLK_M3_GPIO_STAT & 1);
  }
}

/**
  \fn          void GPIO_SetDir (uint32_t port_num,
                                 uint32_t pin_num,
                                 uint32_t dir)
  \brief       Configure GPIO pin direction
  \param[in]   port_num   GPIO number (0..7)
  \param[in]   pin_num    Port pin number
  \param[in]   dir        GPIO_DIR_INPUT, GPIO_DIR_OUTPUT
*/
void GPIO_SetDir (uint32_t port_num, uint32_t pin_num, uint32_t dir) {

  dir  ? (LPC_GPIO_PORT->DIR[port_num] |=  (1UL << pin_num)) : \
         (LPC_GPIO_PORT->DIR[port_num] &= ~(1UL << pin_num));
}

/**
  \fn          void GPIO_PinWrite (uint32_t port_num,
                                   uint32_t pin_num,
                                   uint32_t val)
  \brief       Write port pin
  \param[in]   port_num   GPIO number (0..7)
  \param[in]   pin_num    Port pin number
  \param[in]   val        Port pin value (0 or 1)
*/
void GPIO_PinWrite (uint32_t port_num, uint32_t pin_num, uint32_t val) {

  val ? (LPC_GPIO_PORT->SET[port_num] = (1UL << pin_num)) : \
        (LPC_GPIO_PORT->CLR[port_num] = (1UL << pin_num));
}

/**
  \fn          uint32_t GPIO_PinRead (uint32_t port_num, uint32_t pin_num)
  \brief       Read port pin
  \param[in]   port_num   GPIO number (0..7)
  \param[in]   pin_num    Port pin number
  \return      pin value (0 or 1)
*/
uint32_t GPIO_PinRead (uint32_t port_num, uint32_t pin_num) {
  return ((LPC_GPIO_PORT->PIN[port_num] & (1UL << pin_num)) ? (1) : (0));
}

/**
  \fn          void GPIO_PortWrite (uint32_t port_num,
                                    uint32_t mask,
                                    uint32_t val)
  \brief       Write port pins
  \param[in]   port_num   GPIO number (0..7)
  \param[in]   mask       Selected pins
  \param[in]   val        Pin values
*/
void GPIO_PortWrite (uint32_t port_num, uint32_t mask, uint32_t val) {
  LPC_GPIO_PORT->MASK[port_num] = ~mask;
  LPC_GPIO_PORT->MPIN[port_num] =  val;
}

/**
  \fn          uint32_t  GPIO_PortRead (uint32_t port_num)
  \brief       Read port pins
  \param[in]   port_num   GPIO number (0..7)
  \return      port pin inputs
*/
uint32_t GPIO_PortRead (uint32_t port_num) {
  return (LPC_GPIO_PORT->PIN[port_num]);
}
