/**************************************************************************//**
 * @file     best1000.h
 * @brief    CMSIS Core Peripheral Access Layer Header File for
 *           ARMCM4 Device Series
 * @version  V2.02
 * @date     10. September 2014
 *
 * @note     configured for CM4 with FPU
 *
 ******************************************************************************/
/* Copyright (c) 2011 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#ifndef __FPGA1000_H__
#define __FPGA1000_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__
/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
  NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt          */
  HardFault_IRQn                = -13,      /*!<  3 HardFault Interrupt             */
  MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt     */
  BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt             */
  UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt           */
  SVCall_IRQn                   =  -5,      /*!< 11 SV Call Interrupt               */
  DebugMonitor_IRQn             =  -4,      /*!< 12 Debug Monitor Interrupt         */
  PendSV_IRQn                   =  -2,      /*!< 14 Pend SV Interrupt               */
  SysTick_IRQn                  =  -1,      /*!< 15 System Tick Interrupt           */

/* ----------------------  BEST1000 Specific Interrupt Numbers  --------------------- */
  FPU_IRQn                      =   0,      /*!< FPU Interrupt                      */
  SDIO_IRQn                     =   1,      /*!< SDIO Interrupt                     */
  SDMMC_IRQn                    =   2,      /*!< SDMMC Interrupt                    */
  AUDMA_IRQn                    =   3,      /*!< Audio DMA Interrupt                */
  GPDMA_IRQn                    =   4,      /*!< General Purpose DMA Interrupt      */
  DPDRX_IRQn                    =   5,      /*!< DPD RX Interrupt                   */
  DPDTX_IRQn                    =   6,      /*!< DPD TX Interrupt                   */
  USB_IRQn                      =   7,      /*!< USB Interrupt                      */
  WAKEUP_IRQn                   =   8,      /*!< Reserved Interrupt                 */
  GPIO_IRQn                     =   9,      /*!< GPIO Interrupt                     */
  WDT_IRQn                      =  10,      /*!< Watchdog Timer Interrupt           */
  RTC_IRQn                      =  11,      /*!< RTC Interrupt                      */
  TIMER00_IRQn                  =  12,      /*!< Timer00 Interrupt                  */
  TIMER01_IRQn                  =  13,      /*!< Timer01 Interrupt                  */
  I2C0_IRQn                     =  14,      /*!< I2C0 Interrupt                     */
  SPI0_IRQn                     =  15,      /*!< SPI0 Interrupt                     */
  SPILCD_IRQn                   =  16,      /*!< SPILCD Interrupt                   */
  UART0_IRQn                    =  17,      /*!< UART0 Interrupt                    */
  UART1_IRQn                    =  18,      /*!< UART1 Interrupt                    */
  CODEC_IRQn                    =  19,      /*!< CODEC Interrupt                    */
  BTPCM_IRQn                    =  20,      /*!< BTPCM Interrupt                    */
  I2S0_IRQn                     =  21,      /*!< I2S0 Interrupt                     */
  SPDIF_IRQn                    =  22,      /*!< SPDIF Interrupt                    */
  ITNSPI_IRQn                   =  23,      /*!< Reserved Interrupt                 */
  BT_IRQn                       =  24,      /*!< Reserved Interrupt                 */
  GPADC_IRQn                    =  25,      /*!< Reserved Interrupt                 */
  NONE4_IRQn                    =  26,      /*!< Reserved Interrupt                 */
  USB_PIN_IRQn                  =  27,      /*!< Reserved Interrupt                 */
  ISDONE_IRQn                   =  28,      /*!< Intersys MCU2BT Data Done Interrupt */
  ISDONE1_IRQn                  =  29,      /*!< Intersys MCU2BT Data1 Done Interrupt */
  ISDATA_IRQn                   =  30,      /*!< Intersys BT2MCU Data Indication Interrupt */
  ISDATA1_IRQn                  =  31,      /*!< Intersys BT2MCU Data1 Indication Interrupt */
  CHARGER_IRQn                  =  32,      /*!< Charger IRQ                        */
  PWRKEY_IRQn                   =  33,      /*!< Power key IRQ                      */

  USER_IRQn_QTY,
  INVALID_IRQn                  = USER_IRQn_QTY,
} IRQn_Type;

#endif

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the Cortex-M4 Processor and Core Peripherals  ------- */
#define __CM4_REV                 0x0001    /*!< Core revision r0p1                              */
#define __MPU_PRESENT             1         /*!< MPU present or not                              */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3         /*!< Number of Bits used for Priority Levels         */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used    */
#define __FPU_PRESENT             1         /*!< FPU present                                     */
#define __NUM_CODE_PATCH          32
#define __NUM_LIT_PATCH           32

#include "core_cm4.h"                       /* Processor and core peripherals                    */

#ifndef __ASSEMBLER__

#include "system_ARMCM.h"                  /* System Header                                     */

#endif

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

/* -------------------  Start of section using anonymous unions  ------------------ */
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

/* --------------------  End of section using anonymous unions  ------------------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
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

#ifdef __cplusplus
}
#endif

#endif
