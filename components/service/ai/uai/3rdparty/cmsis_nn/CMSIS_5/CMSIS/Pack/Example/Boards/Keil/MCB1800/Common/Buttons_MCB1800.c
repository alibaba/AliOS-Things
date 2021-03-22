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
 * Name:    Buttons_MCB1800.c
 * Purpose: Buttons interface for MCB1800 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"
#include "GPIO_LPC18xx.h"
#include "Board_Buttons.h"

#define BUTTONS_COUNT                  (4)
#define BUTTON_P4_0                    (1 << 0)
#define BUTTON_P4_3                    (1 << 1)
#define BUTTON_P4_4                    (1 << 2)
#define BUTTON_WAKEUP0                 (1 << 3)

/* Button pins:
   - BUTTON0: P4_0 = GPIO2[0]
   - BUTTON1: P4_3 = GPIO2[3]
   - BUTTON2: P4_4 = GPIO2[4]
   - BUTTON3: WAKEUP0         */

/* Button GPIO definitions */
const GPIO_ID BUTTON_GPIO[] = {
  { 2, 0 },
  { 2, 3 },
  { 2, 4 }
};

/* Button pin definitions */
const PIN_ID BUTTON_PIN[] = {
  { 4, 0, (SCU_CFG_MODE_FUNC0 | SCU_SFS_EZI)},
  { 4, 3, (SCU_CFG_MODE_FUNC0 | SCU_SFS_EZI)},
  { 4, 4, (SCU_CFG_MODE_FUNC0 | SCU_SFS_EZI)}
};


/**
  \fn          int32_t Buttons_Initialize (void)
  \brief       Initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Initialize (void) {

  /* Enable GPIO clock */
  GPIO_PortClock   (1);

  /* Configure Pins */
  SCU_PinConfigure (BUTTON_PIN[0].port, BUTTON_PIN[0].num, BUTTON_PIN[0].config_val);
  SCU_PinConfigure (BUTTON_PIN[1].port, BUTTON_PIN[1].num, BUTTON_PIN[1].config_val);
  SCU_PinConfigure (BUTTON_PIN[2].port, BUTTON_PIN[2].num, BUTTON_PIN[2].config_val);

  /* Configure WAKEUP0 input */
  LPC_CREG->CREG0          &= ~(3<<14); /* WAKEUP0 input to event router      */
  LPC_EVENTROUTER->HILO    &= ~ 1;      /* Detect low level or falling edge   */
  LPC_EVENTROUTER->EDGE    &= ~ 1;      /* Activate level detect              */
  LPC_EVENTROUTER->CLR_STAT =   1;      /* Clear status event bit 0           */

  return 0;
}

/**
  \fn          int32_t Buttons_Uninitialize (void)
  \brief       De-initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Uninitialize (void) {

  /* Unconfigure Pins */
  SCU_PinConfigure (BUTTON_PIN[0].port, BUTTON_PIN[0].num, 0);
  SCU_PinConfigure (BUTTON_PIN[1].port, BUTTON_PIN[1].num, 0);
  SCU_PinConfigure (BUTTON_PIN[2].port, BUTTON_PIN[2].num, 0);

  return 0;
}

/**
  \fn          uint32_t Buttons_GetState (void)
  \brief       Get buttons state
  \returns     Buttons state
*/
uint32_t Buttons_GetState (void) {
  uint32_t val;

  val = 0;
  if (!(GPIO_PinRead (BUTTON_GPIO[0].port, BUTTON_GPIO[0].num))) val |= BUTTON_P4_0;
  if (!(GPIO_PinRead (BUTTON_GPIO[1].port, BUTTON_GPIO[1].num))) val |= BUTTON_P4_3;
  if (!(GPIO_PinRead (BUTTON_GPIO[2].port, BUTTON_GPIO[2].num))) val |= BUTTON_P4_4;

  /* Check WAKEUP0 input */
  LPC_EVENTROUTER->CLR_STAT = 1;        /* Clear STATUS event bit 0           */
  if (LPC_EVENTROUTER->STATUS & 1)                               val |= BUTTON_WAKEUP0;

  return val;
}

/**
  \fn          uint32_t Buttons_GetCount (void)
  \brief       Get number of available buttons
  \return      Number of available buttons
*/
uint32_t Buttons_GetCount (void) {
  return BUTTONS_COUNT;
}
