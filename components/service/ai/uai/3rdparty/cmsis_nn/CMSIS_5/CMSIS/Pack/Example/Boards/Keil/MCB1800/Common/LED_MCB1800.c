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
 * Name:    LED_MCB1800.c
 * Purpose: LED interface for MCB1800 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

#include "SCU_LPC18xx.h"
#include "GPIO_LPC18xx.h"
#include "Board_LED.h"

#define LED_COUNT                      (8)

/* LED pins:
   - LED0: PD_10 = GPIO6[24]
   - LED1: PD_11 = GPIO6[25]
   - LED2: PD_12 = GPIO6[26]
   - LED3: PD_13 = GPIO6[27]
   - LED4: PD_14 = GPIO6[28]
   - LED5: P9_0  = GPIO4[12]
   - LED6: P9_1  = GPIO4[13]
   - LED7: P9_2  = GPIO4[14] */

/* LED GPIO definitions */
const GPIO_ID LED_GPIO[] = {
  { 6, 24 },
  { 6, 25 },
  { 6, 26 },
  { 6, 27 },
  { 6, 28 },
  { 4, 12 },
  { 4, 13 },
  { 4, 14 }
};

/* LED pin definitions */
const PIN_ID LED_PIN[] = {
  { 13, 10, (SCU_CFG_MODE_FUNC4 | SCU_PIN_CFG_PULLDOWN_EN)},
  { 13, 11, (SCU_CFG_MODE_FUNC4 | SCU_PIN_CFG_PULLDOWN_EN)},
  { 13, 12, (SCU_CFG_MODE_FUNC4 | SCU_PIN_CFG_PULLDOWN_EN)},
  { 13, 13, (SCU_CFG_MODE_FUNC4 | SCU_PIN_CFG_PULLDOWN_EN)},
  { 13, 14, (SCU_CFG_MODE_FUNC4 | SCU_PIN_CFG_PULLDOWN_EN)},
  {  9,  0, (SCU_CFG_MODE_FUNC0 | SCU_PIN_CFG_PULLDOWN_EN)},
  {  9,  1, (SCU_CFG_MODE_FUNC0 | SCU_PIN_CFG_PULLDOWN_EN)},
  {  9,  2, (SCU_CFG_MODE_FUNC0 | SCU_PIN_CFG_PULLDOWN_EN)}
};


/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize (void) {
  uint32_t n;

  /* Enable GPIO clock */
  GPIO_PortClock     (1);

  /* Configure pins: Output Mode with Pull-down resistors */
  for (n = 0; n < LED_COUNT; n++) {
    SCU_PinConfigure (LED_PIN[n].port,  LED_PIN[n].num,  LED_PIN[n].config_val);
    GPIO_SetDir      (LED_GPIO[n].port, LED_GPIO[n].num, GPIO_DIR_OUTPUT);
    GPIO_PinWrite    (LED_GPIO[n].port, LED_GPIO[n].num, 0);
  }
  return 0;
}

/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize (void) {
  uint32_t n;

  /* Unconfigure pins: turn off Pull-up/down resistors */
  for (n = 0; n < LED_COUNT; n++) {
    SCU_PinConfigure (LED_PIN[n].port,  LED_PIN[n].num,  0);
  }
  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {
  GPIO_PinWrite     (LED_GPIO[num].port, LED_GPIO[num].num, 1);
  return 0;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {
  GPIO_PinWrite     (LED_GPIO[num].port, LED_GPIO [num].num, 0);
  return 0;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut (uint32_t val) {
  uint32_t n;

  for (n = 0; n < LED_COUNT; n++) {
    if (val & (1 << n)) LED_On (n);
    else                LED_Off(n);
  }
  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount (void) {
  return LED_COUNT;
}
