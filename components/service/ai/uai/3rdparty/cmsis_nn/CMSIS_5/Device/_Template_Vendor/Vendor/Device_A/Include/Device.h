/**************************************************************************//**
 * @file     <Device>.h
 * @brief    CMSIS Cortex-A Core Peripheral Access Layer Header File
 * @version  V1.00
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
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
 */

#ifndef <Device>_H      /* ToDo: replace '<Device>' with your device name */
#define <Device>_H

#ifdef __cplusplus
extern "C" {
#endif

/* ToDo: replace '<Vendor>' with vendor name; add your doxyGen comment   */
/** @addtogroup <Vendor>
  * @{
  */


/* ToDo: replace '<Device>' with device name; add your doxyGen comment   */
/** @addtogroup <Device>
  * @{
  */


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

typedef enum IRQn
{
/* =======================================  ARM Cortex-A Specific Interrupt Numbers  ========================================= */

  /* Software Generated Interrupts */
  SGI0_IRQn                          =  0,      /*!< Software Generated Interrupt  0                                           */
  SGI1_IRQn                          =  1,      /*!< Software Generated Interrupt  1                                           */
  SGI2_IRQn                          =  2,      /*!< Software Generated Interrupt  2                                           */
  SGI3_IRQn                          =  3,      /*!< Software Generated Interrupt  3                                           */
  SGI4_IRQn                          =  4,      /*!< Software Generated Interrupt  4                                           */
  SGI5_IRQn                          =  5,      /*!< Software Generated Interrupt  5                                           */
  SGI6_IRQn                          =  6,      /*!< Software Generated Interrupt  6                                           */
  SGI7_IRQn                          =  7,      /*!< Software Generated Interrupt  7                                           */
  SGI8_IRQn                          =  8,      /*!< Software Generated Interrupt  8                                           */
  SGI9_IRQn                          =  9,      /*!< Software Generated Interrupt  9                                           */
  SGI10_IRQn                         = 10,      /*!< Software Generated Interrupt 10                                           */
  SGI11_IRQn                         = 11,      /*!< Software Generated Interrupt 11                                           */
  SGI12_IRQn                         = 12,      /*!< Software Generated Interrupt 12                                           */
  SGI13_IRQn                         = 13,      /*!< Software Generated Interrupt 13                                           */
  SGI14_IRQn                         = 14,      /*!< Software Generated Interrupt 14                                           */
  SGI15_IRQn                         = 15,      /*!< Software Generated Interrupt 15                                           */
  
  /* Private Peripheral Interrupts */
  VirtualMaintenanceInterrupt_IRQn   = 25,      /*!< Virtual Maintenance Interrupt                                             */
  HypervisorTimer_IRQn               = 26,      /*!< Hypervisor Timer Interrupt                                                */
  VirtualTimer_IRQn                  = 27,      /*!< Virtual Timer Interrupt                                                   */ 
  Legacy_nFIQ_IRQn                   = 28,      /*!< Legacy nFIQ Interrupt                                                     */
  SecurePhysicalTimer_IRQn           = 29,      /*!< Secure Physical Timer Interrupt                                           */
  NonSecurePhysicalTimer_IRQn        = 30,      /*!< Non-Secure Physical Timer Interrupt                                       */
  Legacy_nIRQ_IRQn                   = 31,      /*!< Legacy nIRQ Interrupt                                                     */ 

 /* Shared Peripheral Interrupts */
 /* ToDo: add here your device specific external interrupt numbers */
  <DeviceInterrupt>_IRQn    = 0,                /*!< Device Interrupt                                                          */
  
} IRQn_Type;


/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ===========================  Configuration of the Arm Cortex-A Processor and Core Peripherals  ============================ */
/* ToDo: set the defines according your Device */
/* ToDo: define the correct core revision              
         5U if your device is a CORTEX-A5 device
         7U if your device is a CORTEX-A7 device
         9U if your device is a CORTEX-A9 device */
#define __CORTEX_A                    #U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0000U      /*!< Core revision r0p0                          */
/* ToDo: define the correct core features for the <Device> */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 1U      /*!< Set to 1 if L2C is present                  */

/** @} */ /* End of group Configuration_of_CMSIS */

/* ToDo: include the correct core_ca#.h file
         core_ca5.h if your device is a CORTEX-A5 device
         core_ca7.h if your device is a CORTEX-A7 device
         core_ca9.h if your device is a CORTEX-A9 device */
#include <core_ca#.h>                         /*!< Arm Cortex-A# processor and core peripherals */
/* ToDo: include your system_<Device>.h file
         replace '<Device>' with your device name */
#include "system_<Device>.h"                  /*!< <Device> System */


/* ========================================  Start of section using anonymous unions  ======================================== */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* =========================================================================================================================== */
/* ================                            Device Specific Peripheral Section                             ================ */
/* =========================================================================================================================== */


/** @addtogroup Device_Peripheral_peripherals
  * @{
  */

/* ToDo: add here your device specific peripheral access structure typedefs
         following is an example for a timer */

/* =========================================================================================================================== */
/* ================                                            TMR                                            ================ */
/* =========================================================================================================================== */


/**
  * @brief Timer (TMR)
  */

typedef struct
{                                               /*!< (@ 0x40000000) TIM Structure                                              */
  __IOM uint32_t   TimerLoad;                   /*!< (@ 0x00000004) Timer Load                                                 */
  __IM  uint32_t   TimerValue;                  /*!< (@ 0x00000008) Timer Counter Current Value                                */
  __IOM uint32_t   TimerControl;                /*!< (@ 0x0000000C) Timer Control                                              */
  __OM  uint32_t   TimerIntClr;                 /*!< (@ 0x00000010) Timer Interrupt Clear                                      */
  __IM  uint32_t   TimerRIS;                    /*!< (@ 0x00000014) Timer Raw Interrupt Status                                 */
  __IM  uint32_t   TimerMIS;                    /*!< (@ 0x00000018) Timer Masked Interrupt Status                              */
  __IM  uint32_t   RESERVED[1];
  __IOM uint32_t   TimerBGLoad;                 /*!< (@ 0x00000020) Background Load Register                                   */
} <DeviceAbbreviation>_TMR_TypeDef;

/*@}*/ /* end of group <Device>_Peripherals */


/* =========================================  End of section using anonymous unions  ========================================= */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* =========================================================================================================================== */
/* ================                          Device Specific Peripheral Address Map                           ================ */
/* =========================================================================================================================== */


/* ToDo: add here your device peripherals base addresses
         following is an example for timer */
/** @addtogroup Device_Peripheral_peripheralAddr
  * @{
  */

/* Peripheral and SRAM base address */
#define <DeviceAbbreviation>_FLASH_BASE       (0x00000000UL)                              /*!< (FLASH     ) Base Address */
#define <DeviceAbbreviation>_SRAM_BASE        (0x20000000UL)                              /*!< (SRAM      ) Base Address */
#define <DeviceAbbreviation>_PERIPH_BASE      (0x40000000UL)                              /*!< (Peripheral) Base Address */

/* Peripheral memory map */
#define <DeviceAbbreviation>TIM0_BASE         (<DeviceAbbreviation>_PERIPH_BASE)          /*!< (Timer0    ) Base Address */
#define <DeviceAbbreviation>TIM1_BASE         (<DeviceAbbreviation>_PERIPH_BASE + 0x0800) /*!< (Timer1    ) Base Address */
#define <DeviceAbbreviation>TIM2_BASE         (<DeviceAbbreviation>_PERIPH_BASE + 0x1000) /*!< (Timer2    ) Base Address */

/** @} */ /* End of group Device_Peripheral_peripheralAddr */


/* =========================================================================================================================== */
/* ================                                  Peripheral declaration                                   ================ */
/* =========================================================================================================================== */


/* ToDo: add here your device peripherals pointer definitions
         following is an example for timer */
/** @addtogroup Device_Peripheral_declaration
  * @{
  */

#define <DeviceAbbreviation>_TIM0        ((<DeviceAbbreviation>_TMR_TypeDef *) <DeviceAbbreviation>TIM0_BASE)
#define <DeviceAbbreviation>_TIM1        ((<DeviceAbbreviation>_TMR_TypeDef *) <DeviceAbbreviation>TIM0_BASE)
#define <DeviceAbbreviation>_TIM2        ((<DeviceAbbreviation>_TMR_TypeDef *) <DeviceAbbreviation>TIM0_BASE)


/** @} */ /* End of group <Device> */

/** @} */ /* End of group <Vendor> */

#ifdef __cplusplus
}
#endif

#endif  /* <Device>_H */
