/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/
#ifndef BSPADC_H_
#define BSPADC_H_

/* Include all headers which are needed by this file. */
#include "BCDS_HAL.h"

#if BCDS_FEATURE_ADC

/* Put the type and macro definitions here */
#define AUTO_SELECT_CURRENT_ADC_HFPERCLK    0   /**< Macro to used for the emlib to auto select the current HFPERCLK which is currently used for configuring ADC time base  */
#define BSP_ADC_CLOCK_FREQ_ISR_MODE 700000      /**< Maximum ADC clock frequency supported/tested for ADC driver in ISR mode  */
#define BSP_ADC_CLOCK_FREQ_DMA_MODE 8000000     /**< Maximum ADC clock frequency supported/tested for ADC driver in DMA mode  */
/* Put the function declarations here */
/**
 * @brief
 * ADC interrupt service routine used to capture interrupts and to forward them to the ADC driver for processing.
 */
void BSP_Adc_HandlerIsr(void);
#endif
#endif /* BSPADC_H_ */

