/******************************************************************************
 * @file     ARMCA7.h
 * @brief    CMSIS Cortex-A7 Core Peripheral Access Layer Header File
 * @version  V1.1.0
 * @date     15. May 2019
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
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

#ifndef __BEST2001_DSP_H__
#define __BEST2001_DSP_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

/* -------------------------  Interrupt Number Definition  ------------------------ */

/** Device specific Interrupt IDs */
typedef enum IRQn
{
    /******  SGI Interrupts Numbers                 ****************************************/
    SGI0_IRQn                   =  0,       /*!< Software Generated Interrupt 0 */
    SGI1_IRQn                   =  1,       /*!< Software Generated Interrupt 1 */
    SGI2_IRQn                   =  2,       /*!< Software Generated Interrupt 2 */
    SGI3_IRQn                   =  3,       /*!< Software Generated Interrupt 3 */
    SGI4_IRQn                   =  4,       /*!< Software Generated Interrupt 4 */
    SGI5_IRQn                   =  5,       /*!< Software Generated Interrupt 5 */
    SGI6_IRQn                   =  6,       /*!< Software Generated Interrupt 6 */
    SGI7_IRQn                   =  7,       /*!< Software Generated Interrupt 7 */
    SGI8_IRQn                   =  8,       /*!< Software Generated Interrupt 8 */
    SGI9_IRQn                   =  9,       /*!< Software Generated Interrupt 9 */
    SGI10_IRQn                  = 10,       /*!< Software Generated Interrupt 10 */
    SGI11_IRQn                  = 11,       /*!< Software Generated Interrupt 11 */
    SGI12_IRQn                  = 12,       /*!< Software Generated Interrupt 12 */
    SGI13_IRQn                  = 13,       /*!< Software Generated Interrupt 13 */
    SGI14_IRQn                  = 14,       /*!< Software Generated Interrupt 14 */
    SGI15_IRQn                  = 15,       /*!< Software Generated Interrupt 15 */

    /******  Cortex-A7 Processor Exceptions Numbers ****************************************/
    SecurePhyTimer_IRQn         = 29,       /*!< Physical Timer Interrupt                      */

    /******  Platform Exceptions Numbers ***************************************************/
    WAKEUP_IRQn                 = 32,       /*!< Wakeup Interrupt                   */
    CODEC_IRQn                  = 33,       /*!< CODEC Interrupt                    */
    CODEC_TX_PEAK_IRQn          = 34,       /*!< CODEC TX PEAK Interrupt            */
    SDMMC_IRQn                  = 35,       /*!< SDMMC Interrupt                    */
    BES2001_AUDMA_IRQn          = 36,       /*!< Audio DMA Interrupt                */
    BES2001_GPDMA_IRQn          = 37,       /*!< General Purpose DMA Interrupt      */
    USB_IRQn                    = 38,       /*!< USB Interrupt                      */
    USB_PHY_IRQn                = 39,       /*!< USB PHY Interrupt                  */
    USB_CAL_IRQn                = 40,       /*!< USB Calibration Interrupt          */
    USB_PIN_IRQn                = 41,       /*!< USB Pin Interrupt                  */
    SEC_ENG_IRQn                = 42,       /*!< Security Engine Interrupt          */
    SEDMA_IRQn                  = 43,       /*!< SEDMA Interrupt                    */
    DUMP_IRQn                   = 44,       /*!< DUMP Interrupt                     */
    MCU_WDT_IRQn                = 45,       /*!< Watchdog Timer Interrupt           */
    MCU_TIMER00_IRQn            = 46,       /*!< Timer00 Interrupt                  */
    MCU_TIMER01_IRQn            = 47,       /*!< Timer01 Interrupt                  */
    MCU_TIMER10_IRQn            = 48,       /*!< Timer10 Interrupt                  */
    MCU_TIMER11_IRQn            = 49,       /*!< Timer11 Interrupt                  */
    MCU_TIMER20_IRQn            = 50,       /*!< Timer20 Interrupt                  */
    MCU_TIMER21_IRQn            = 51,       /*!< Timer21 Interrupt                  */
    I2C0_IRQn                   = 52,       /*!< I2C0 Interrupt                     */
    I2C1_IRQn                   = 53,       /*!< I2C1 Interrupt                     */
    SPI0_IRQn                   = 54,       /*!< SPI0 Interrupt                     */
    SPILCD_IRQn                 = 55,       /*!< SPILCD Interrupt                   */
    ITNSPI_IRQn                 = 56,       /*!< Internal SPI Interrupt             */
    SPIPHY_IRQn                 = 57,       /*!< SPIPHY Interrupt                   */
    UART0_IRQn                  = 58,       /*!< UART0 Interrupt                    */
    UART1_IRQn                  = 59,       /*!< UART1 Interrupt                    */
    UART2_IRQn                  = 60,       /*!< UART2 Interrupt                    */
    BTPCM_IRQn                  = 61,       /*!< BTPCM Interrupt                    */
    I2S0_IRQn                   = 62,       /*!< I2S0 Interrupt                     */
    SPDIF0_IRQn                 = 63,       /*!< SPDIF0 Interrupt                   */
    TRNG_IRQn                   = 64,       /*!< TRNG Interrupt                     */
    AON_GPIO_IRQn               = 65,       /*!< AON GPIO Interrupt                 */
    AON_GPIOAUX_IRQn            = 66,       /*!< AON GPIOAUX Interrupt              */
    AON_WDT_IRQn                = 67,       /*!< AON Watchdog Timer Interrupt       */
    AON_TIMER00_IRQn            = 68,       /*!< AON Timer00 Interrupt              */
    AON_TIMER01_IRQn            = 69,       /*!< AON Timer01 Interrupt              */
    TRANSQW_LCL_IRQn            = 70,       /*!< TRANSQ-WIFI Local Interrupt        */
    TRANSQW_RMT_IRQn            = 71,       /*!< TRANSQ-WIFI Peer Remote Interrupt  */
    WIFI_IRQn                   = 72,       /*!< DSP to MCU Interrupt               */
    ISDONE_IRQn                 = 73,       /*!< Intersys MCU2BT Data Done Interrupt */
    ISDONE1_IRQn                = 74,       /*!< Intersys MCU2BT Data1 Done Interrupt */
    ISDATA_IRQn                 = 75,       /*!< Intersys BT2MCU Data Indication Interrupt */
    ISDATA1_IRQn                = 76,       /*!< Intersys BT2MCU Data1 Indication Interrupt */
    BT_IRQn                     = 77,       /*!< BT to MCU Interrupt                */
    RESERVED58_IRQn             = 78,       /*!< Reserved Interrupt                 */
    RTC_IRQn                    = 79,       /*!< RTC Interrupt                      */
    GPADC_IRQn                  = 80,       /*!< GPADC Interrupt                    */
    CHARGER_IRQn                = 81,       /*!< Charger Interrupt                  */
    PWRKEY_IRQn                 = 82,       /*!< Power key Interrupt                */
    WIFIDUMP_IRQn               = 83,       /*!< WIFIDUMP Interrupt                 */
    CHKSUM_IRQn                 = 84,       /*!< Checksum Interrupt                 */
    CRC_IRQn                    = 85,       /*!< CRC Interrupt                      */
    AON_SPIDPD_IRQn             = 86,       /*!< AON SPIDPD Interrupt               */
    TRUSTZONE_IRQn              = 87,       /*!< TrustZone Interrupt                */
    TRANSQM_LCL_IRQn            = 88,       /*!< TRANSQ-MCU Local Interrupt         */
    TRANSQM_RMT_IRQn            = 89,       /*!< TRANSQ-MCU Peer Remote Interrupt   */
    MCU_IRQn                    = 90,       /*!< MCU to DSP Interrupt               */
    DSP_WDT_IRQn                = 91,       /*!< Watchdog Timer Interrupt           */
    DSP_TIMER00_IRQn            = 92,       /*!< Timer00 Interrupt                  */
    DSP_TIMER01_IRQn            = 93,       /*!< Timer01 Interrupt                  */
    DSP_TIMER10_IRQn            = 94,       /*!< Timer10 Interrupt                  */
    DSP_TIMER11_IRQn            = DSP_TIMER10_IRQn, /*!< Timer11 Interrupt          */
    XDMA_IRQn                   = 95,       /*!< DSP XDMA Interrupt                 */

    USER_IRQn_QTY,
    INVALID_IRQn                = USER_IRQn_QTY,
} IRQn_Type;

#define AUDMA_IRQn              BES2001_AUDMA_IRQn //A7 use AUDMA
#define GPDMA_IRQn              BES2001_GPDMA_IRQn

#define GPIO_IRQn               AON_GPIO_IRQn
#define GPIOAUX_IRQn            AON_GPIOAUX_IRQn
#define TIMER00_IRQn            DSP_TIMER00_IRQn
#define TIMER01_IRQn            DSP_TIMER01_IRQn
#define WDT_IRQn                DSP_WDT_IRQn

#define TRANSQ0_RMT_IRQn        TRANSQW_RMT_IRQn//use MCU's transq
#define TRANSQ0_LCL_IRQn        TRANSQW_LCL_IRQn
#define TRANSQ1_RMT_IRQn        TRANSQM_RMT_IRQn
#define TRANSQ1_LCL_IRQn        TRANSQM_LCL_IRQn

#endif

#if 0
/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

/* Peripheral and RAM base address */
#define VE_A7_MP_FLASH_BASE0                  (0x00000000UL)                                /*!< (FLASH0     ) Base Address */
#define VE_A7_MP_FLASH_BASE1                  (0x0C000000UL)                                /*!< (FLASH1     ) Base Address */
#define VE_A7_MP_SRAM_BASE                    (0x14000000UL)                                /*!< (SRAM       ) Base Address */
#define VE_A7_MP_PERIPH_BASE_CS2              (0x18000000UL)                                /*!< (Peripheral ) Base Address */
#define VE_A7_MP_VRAM_BASE                    (0x00000000UL + VE_A7_MP_PERIPH_BASE_CS2)     /*!< (VRAM       ) Base Address */
#define VE_A7_MP_ETHERNET_BASE                (0x02000000UL + VE_A7_MP_PERIPH_BASE_CS2)     /*!< (ETHERNET   ) Base Address */
#define VE_A7_MP_USB_BASE                     (0x03000000UL + VE_A7_MP_PERIPH_BASE_CS2)     /*!< (USB        ) Base Address */
#define VE_A7_MP_PERIPH_BASE_CS3              (0x1C000000UL)                                /*!< (Peripheral ) Base Address */
#define VE_A7_MP_DAP_BASE                     (0x00000000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (LOCAL DAP  ) Base Address */
#define VE_A7_MP_SYSTEM_REG_BASE              (0x00010000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (SYSTEM REG ) Base Address */
#define VE_A7_MP_SERIAL_BASE                  (0x00030000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (SERIAL     ) Base Address */
#define VE_A7_MP_AACI_BASE                    (0x00040000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (AACI       ) Base Address */
#define VE_A7_MP_MMCI_BASE                    (0x00050000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (MMCI       ) Base Address */
#define VE_A7_MP_KMI0_BASE                    (0x00060000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (KMI0       ) Base Address */
#define VE_A7_MP_UART_BASE                    (0x00090000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (UART       ) Base Address */
#define VE_A7_MP_WDT_BASE                     (0x000F0000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (WDT        ) Base Address */
#define VE_A7_MP_TIMER_BASE                   (0x00110000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (TIMER      ) Base Address */
#define VE_A7_MP_DVI_BASE                     (0x00160000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (DVI        ) Base Address */
#define VE_A7_MP_RTC_BASE                     (0x00170000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (RTC        ) Base Address */
#define VE_A7_MP_UART4_BASE                   (0x001B0000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (UART4      ) Base Address */
#define VE_A7_MP_CLCD_BASE                    (0x001F0000UL + VE_A7_MP_PERIPH_BASE_CS3)     /*!< (CLCD       ) Base Address */
#define VE_A7_MP_PRIVATE_PERIPH_BASE          (0x2C000000UL)                                /*!< (Peripheral ) Base Address */
#define VE_A7_MP_GIC_DISTRIBUTOR_BASE         (0x00001000UL + VE_A7_MP_PRIVATE_PERIPH_BASE) /*!< (GIC DIST   ) Base Address */
#define VE_A7_MP_GIC_INTERFACE_BASE           (0x00002000UL + VE_A7_MP_PRIVATE_PERIPH_BASE) /*!< (GIC CPU IF ) Base Address */
#define VE_A7_MP_PL310_BASE                   (0x000F0000UL + VE_A7_MP_PRIVATE_PERIPH_BASE) /*!< (L2C-310    ) Base Address */
#define VE_A7_MP_SSRAM_BASE                   (0x2E000000UL)                                /*!< (System SRAM) Base Address */
#define VE_A7_MP_DRAM_BASE                    (0x80000000UL)                                /*!< (DRAM       ) Base Address */
#define GIC_DISTRIBUTOR_BASE                  VE_A7_MP_GIC_DISTRIBUTOR_BASE
#define GIC_INTERFACE_BASE                    VE_A7_MP_GIC_INTERFACE_BASE

//The VE-A7 model implements L1 cache as architecturally defined, but does not implement L2 cache.
//Do not enable the L2 cache if you are running RTX on a VE-A7 model as it may cause a data abort.
#define L2C_310_BASE                          VE_A7_MP_PL310_BASE
#endif

/* --------  Configuration of the Cortex-A7 Processor and Core Peripherals  ------- */
#define __CA_REV        0x0000U    /* Core revision r0p0                            */
#define __CORTEX_A           7U    /* Cortex-A7 Core                                */
#define __FPU_PRESENT        1U    /* FPU present                                   */
#define __GIC_PRESENT        1U    /* GIC present                                   */
#define __TIM_PRESENT        1U    /* TIM present                                   */
#define __L2C_PRESENT        0U    /* L2C present                                   */
#define __GIC_PRIO_BITS      3U    /* Number of Bits used for Priority Levels       */

#include "ca/core_ca.h"

#ifndef __ASSEMBLER__
#include "ca/system_ARMCA.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
