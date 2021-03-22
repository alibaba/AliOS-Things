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
 * Name:    ADC_MCB1800.c
 * Purpose: A/D Converter interface for MCB1800 evaluation board
 * Rev.:    1.00
 *
 *----------------------------------------------------------------------------*/

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"
#include "Board_ADC.h"

#define ADC_RESOLUTION        10        /* Number of A/D converter bits       */

/* Clock Control Unit register bits */
#define CCU_CLK_CFG_RUN      (1 << 0)
#define CCU_CLK_CFG_AUTO     (1 << 1)
#define CCU_CLK_STAT_RUN     (1 << 0)

static volatile uint16_t AD_last;       /* Last converted value               */
static volatile uint8_t  AD_done;       /* AD conversion done flag            */


/**
  \fn          int32_t ADC_Initialize (void)
  \brief       Initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_Initialize (void) {

  /* Enable ADC0 clock */
  LPC_CCU1->CLK_APB3_ADC0_CFG = CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
  while (!(LPC_CCU1->CLK_APB3_ADC0_STAT & CCU_CLK_STAT_RUN));

  /* Configure ADC0_1 */
  LPC_ADC0->CR = (1 <<  1) |            /* Select ADC0_1 pin for conversion   */
                 (2 <<  8) |            /* 12MHz / (2+1) = 4MHz               */
                 (1 << 21) ;            /* ADC is operational                 */

  /* Enable ADC0 Channel 1 interrupt */
  LPC_ADC0->INTEN |=  (1 << 1);
  NVIC_EnableIRQ (ADC0_IRQn);

  return 0;
}

/**
  \fn          int32_t ADC_Uninitialize (void)
  \brief       De-initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_Uninitialize (void) {

  /* Disable ADC0 Channel 1 interrupt */
  NVIC_DisableIRQ (ADC0_IRQn);
  LPC_ADC0->INTEN &= ~(1 << 1);

  /* Disable ADC0 clock */
  LPC_CCU1->CLK_APB3_ADC0_CFG = 0;

  return 0;
}

/**
  \fn          int32_t ADC_StartConversion (void)
  \brief       Start conversion
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_StartConversion (void) {

  LPC_ADC0->CR |= (1 << 24);            /* Start conversion                   */

  return 0;
}

/**
  \fn          int32_t ADC_ConversionDone (void)
  \brief       Check if conversion finished
  \returns
   - \b  0: conversion finished
   - \b -1: conversion in progress
*/
int32_t ADC_ConversionDone (void) {
  return (AD_done ? 0 : -1);
}

/**
  \fn          int32_t ADC_GetValue (void)
  \brief       Get converted value
  \returns
   - <b> >=0</b>: converted value
   - \b -1: conversion in progress or failed
*/
int32_t ADC_GetValue (void) {

  if (AD_done) {
    AD_done = 0;
    return AD_last;
  }
  return -1;
}

/**
  \fn          uint32_t ADC_GetResolution (void)
  \brief       Get resolution of Analog-to-Digital Converter
  \returns     Resolution (in bits)
*/
uint32_t ADC_GetResolution (void) {
  return ADC_RESOLUTION;
}

/**
  \fn          void ADC0_IRQHandler (void)
  \brief       Analog-to-Digital Converter Interrupt Handler
*/
void ADC0_IRQHandler (void) {

  LPC_ADC0->GDR;                            /* Clear IRQ request flag         */
  AD_last = (LPC_ADC0->DR[1] >> 6) & 0x3FF; /* Read value and clear IRQ       */
  AD_done = 1;
}
