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
 * Name:    Blinky.c
 * Purpose: LED Flasher for MCB1800
 *
 *----------------------------------------------------------------------------*/

#include "LPC18xx.h"                    /* LPC18xx Definitions                */
#include "Board_LED.h"

uint32_t LEDOn, LEDOff; 

/*----------------------------------------------------------------------------
  SysTick IRQ Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) {
  static uint32_t ticks;
  
  switch (ticks++) {
    case  0: LEDOn  = 1; break;
    case  5: LEDOff = 1; break;
    case  9: ticks  = 0; break;
    
    default:
      if (ticks > 10) {
        ticks = 0;
      }
  }
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
  int32_t max_num = LED_GetCount() - 1;
  int32_t num = 0;
  int32_t dir = 1;

  SystemCoreClockUpdate ();                 /* Update system core clock       */  
  SysTick_Config(SystemCoreClock/100);      /* Generate interrupt each 10 ms  */
  LED_Initialize();                         /* LED Initialization             */

  while (1) {
    if (LEDOn) {
      LEDOn = 0;
      LED_On (num);                         /* Turn specified LED on          */
    }

    if (LEDOff) {
      LEDOff = 0;
      LED_Off (num);                        /* Turn specified LED off         */

      num += dir;                           /* Change LED number              */
      
      if (dir == 1 && num == max_num) {
        dir = -1;                           /* Change direction to down       */
      }
      else if (num == 0) {
        dir =  1;                           /* Change direction to up         */
      }
    }
  }
}
