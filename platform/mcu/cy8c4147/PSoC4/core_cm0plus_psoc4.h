/***************************************************************************//**
* \file core_cm0plus_psoc4.h
* \version 5.70
*
* \brief Provides important type information for the  PSOC4 device family.
* This includes types necessary for core_cm0.h.
*
* \note Documentation of the API's in this file is located in the
*   System Reference Guide provided with PSoC Creator.
*
********************************************************************************
* \copyright
* Copyright 2008-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BOOT_CORE_CM0_PSOC4_H)
#define CY_BOOT_CORE_CM0_PSOC4_H

/** Interrupt Number Definition */
typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn   = -14,              /*!< 2 Non Maskable Interrupt                         */
  HardFault_IRQn        = -13,              /*!< 3 Cortex-M0 Hard Fault Interrupt                 */
  SVCall_IRQn           = -5,               /*!< 11 Cortex-M0 SV Call Interrupt                   */
  PendSV_IRQn           = -2,               /*!< 14 Cortex-M0 Pend SV Interrupt                   */
  SysTick_IRQn          = -1                /*!< 15 Cortex-M0 System Tick Interrupt               */
/******  PSOC4 Peripheral Interrupt Numbers *******************************************************/
  /* Not relevant.  All peripheral interrupts are defined by the user */
} IRQn_Type;

#define __CHECK_DEVICE_DEFINES

#define __CM0PLUS_REV             0x0000
#define __VTOR_PRESENT            0U
#define __MPU_PRESENT             1U
#define __NVIC_PRIO_BITS          2
#define __Vendor_SysTickConfig    0

#include <core_cm0plus.h>

#endif /* CY_BOOT_CORE_CM0_PSOC4_H */
