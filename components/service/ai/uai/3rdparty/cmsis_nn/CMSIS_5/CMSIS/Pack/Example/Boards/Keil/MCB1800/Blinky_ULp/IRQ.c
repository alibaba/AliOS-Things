/*----------------------------------------------------------------------------
 * Name:    IRQ.c
 * Purpose: IRQ Handler
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2014 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC18xx.h"                    /* LPC18xx Definitions                */
#include "Board_LED.h"
#include "Board_ADC.h"

extern uint16_t AD_last;                /* Last converted value               */

uint8_t clock_1s;                       /* Flag activated each second         */

/*----------------------------------------------------------------------------
  SysTick IRQ Handler (occurs every 10 ms)
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) {
  static unsigned long ticks = 0;
  static unsigned long timetick;
  static unsigned int  leds = 0x01;

  if (ticks++ >= 99) {                  /* Set Clock1s to 1 every second      */
    ticks    = 0;
    clock_1s = 1;
  }

  /* Blink the LEDs depending on ADC_ConvertedValue                           */
  if (timetick++ >= (AD_last >> 8)) {
    timetick   = 0;
    leds     <<= 1;
    if (leds == (1 << LED_GetCount())) leds = 0x01;
    LED_SetOut (leds);
  }

  ADC_StartConversion();                /* Start ADC conversion               */
}
