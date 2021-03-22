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
 * Name:    Joystick_MCB1800.c
 * Purpose: Joystick interface for MCB1800 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

include "LPC18xx.h"
#include "GPIO_LPC18xx.h"
#include "SCU_LPC18xx.h"
#include "Board_Joystick.h"

#define JOYSTICK_COUNT                 (5)

/* Joystick pins:
   - center: PC_9  = GPIO6[8]
   - up:     PC_11 = GPIO6[10]
   - down:   PC_12 = GPIO6[11]
   - left:   PC_13 = GPIO6[12]
   - right:  PC_14 = GPIO6[13] */

/* Joystick GPIO definitions */
const GPIO_ID JOYSTICK_GPIO[] = {
  { 6,  8 },
  { 6, 10 },
  { 6, 11 },
  { 6, 12 },
  { 6, 13 }
};

/* Joystick pin definitions */
const PIN_ID JOYSTICK_PIN[] = {
  { 12,  9, (SCU_CFG_MODE_FUNC4 | SCU_SFS_EZI)},
  { 12, 11, (SCU_CFG_MODE_FUNC4 | SCU_SFS_EZI)},
  { 12, 12, (SCU_CFG_MODE_FUNC4 | SCU_SFS_EZI)},
  { 12, 13, (SCU_CFG_MODE_FUNC4 | SCU_SFS_EZI)},
  { 12, 14, (SCU_CFG_MODE_FUNC4 | SCU_SFS_EZI)}
};


/**
  \fn          int32_t Joystick_Initialize (void)
  \brief       Initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Initialize (void) {
  uint32_t n;

  /* Enable GPIO clock */
  GPIO_PortClock     (1);

  /* Configure pins */
  for (n = 0; n < JOYSTICK_COUNT; n++) {
    SCU_PinConfigure (JOYSTICK_PIN[n].port,  JOYSTICK_PIN[n].num,  JOYSTICK_PIN[n].config_val);
    GPIO_SetDir      (JOYSTICK_GPIO[n].port, JOYSTICK_GPIO[n].num, GPIO_DIR_INPUT);
  }
  return 0;
}

/**
  \fn          int32_t Joystick_Uninitialize (void)
  \brief       De-initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Uninitialize (void) {
  uint32_t n;

  /* Unconfigure pins */
  for (n = 0; n < JOYSTICK_COUNT; n++) {
    SCU_PinConfigure (JOYSTICK_PIN[n].port,  JOYSTICK_PIN[n].num,  0);
  }
  return 0;
}

/**
  \fn          uint32_t Joystick_GetState (void)
  \brief       Get joystick state
  \returns     Joystick state
*/
uint32_t Joystick_GetState (void) {
  uint32_t val;

  val  = 0;
  if (!(GPIO_PinRead (JOYSTICK_GPIO[0].port, JOYSTICK_GPIO[0].num))) val |= JOYSTICK_CENTER;
  if (!(GPIO_PinRead (JOYSTICK_GPIO[1].port, JOYSTICK_GPIO[1].num))) val |= JOYSTICK_UP;
  if (!(GPIO_PinRead (JOYSTICK_GPIO[2].port, JOYSTICK_GPIO[2].num))) val |= JOYSTICK_DOWN;
  if (!(GPIO_PinRead (JOYSTICK_GPIO[3].port, JOYSTICK_GPIO[3].num))) val |= JOYSTICK_LEFT;
  if (!(GPIO_PinRead (JOYSTICK_GPIO[4].port, JOYSTICK_GPIO[4].num))) val |= JOYSTICK_RIGHT;

  return val;
}
