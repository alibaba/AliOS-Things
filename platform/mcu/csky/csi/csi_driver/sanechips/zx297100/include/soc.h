/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**************************************************************************//**
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/


#ifndef SOC_H
#define SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SYSTEM_CLOCK
#define  SYSTEM_CLOCK    (19200000)
#endif
#ifndef LSP_DEFAULT_FREQ
#define  LSP_DEFAULT_FREQ    (19200000)
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */
#define ICU_INT_START                    32
typedef enum IRQn
{
     PIC_IRQ_WDT               =0,
     PIC_IRQ_LP_UART           =1,
     PIC_IRQ_TEMP_DET          =2,
     PIC_IRQ_UART1             =3,
     PIC_IRQ_SSP0              =4,
     PIC_IRQ_VBAT_DETN         =5,
     PIC_IRQ_I2C0              =6,
     PIC_IRQ_INNER_PWR_OC      =7,
     PIC_IRQ_RTC_ALARM         =8,
     PIC_IRQ_RTC_TIMER         =9,
     PIC_IRQ_EFC1              =10,
     PIC_IRQ_ICP_DSP2CK        =11,
     PIC_IRQ_ICP_M02CK         =12,
     PIC_IRQ_CK_ITIMER         =13,
     PIC_IRQ_TIMER0            =14,
     PIC_IRQ_AD_TIMER0         =15,
     PIC_IRQ_AD_TIMER1         =16,
     PIC_IRQ_LP_TIMER          =17,
     PIC_IRQ_CP_DMAINT1        =18,
     PIC_IRQ_AP_DMAINT1        =19,
     PIC_IRQ_LPM0              =20,
     PIC_IRQ_ADC               =21,
     PIC_IRQ_DAC               =22,
     PIC_IRQ_HS_COMPA          =23,
     PIC_IRQ_I2S               =25,
     PIC_IRQ_SECURE            =26,
     PIC_IRQ_SPI_NOR           =27,
     PIC_IRQ_RTC_TIMESTAMP     =28,
     PIC_IRQ_EXT0              =29,
     PIC_IRQ_EFC2              =30,
     PIC_IRQ_31                =31,
    ICU_IRQ_WAKE1_SP           = (ICU_INT_START+0),
    ICU_IRQ_WAKE1_LP           = (ICU_INT_START+1),
    ICU_IRQ_PCU_AO2AP_WAKE     = (ICU_INT_START+2),
    ICU_IRQ_CP_WTD_RST         = (ICU_INT_START+3),
    ICU_IRQ_LP_UART_CTS        = (ICU_INT_START+4),
    ICU_IRQ_EXT2               = (ICU_INT_START+5),
    ICU_IRQ_EXT3               = (ICU_INT_START+6),
    ICU_IRQ_EXT4               = (ICU_INT_START+7),
    ICU_IRQ_EXT5               = (ICU_INT_START+8),
    ICU_IRQ_EXT6               = (ICU_INT_START+9),
    ICU_IRQ_EXT7               = (ICU_INT_START+10),
    ICU_IRQ_LP_UART_RXD        = (ICU_INT_START+11),
    ICU_IRQ_UART0              = (ICU_INT_START+12),
    ICU_IRQ_SSP1               = (ICU_INT_START+13),
    ICU_IRQ_I2C1               = (ICU_INT_START+14),
    ICU_IRQ_UART2              = (ICU_INT_START+15),
    ICU_IRQ_SSP2               = (ICU_INT_START+16),
    ICU_IRQ_TIMER1             = (ICU_INT_START+17),
    ICU_IRQ_CEVA_GVI_R         = (ICU_INT_START+18),
    ICU_IRQ_EX0_DC2            = (ICU_INT_START+19),
    ICU_IRQ_EX1_DC2            = (ICU_INT_START+20),
    ICU_IRQ_EX2_DC2            = (ICU_INT_START+21),
    ICU_IRQ_EX3_DC2            = (ICU_INT_START+22),
    ICU_IRQ_EXT1               = (ICU_INT_START+23),
    ICU_IRQ_EX4_DC2            = (ICU_INT_START+24),
    ICU_IRQ_EX5_DC2            = (ICU_INT_START+25),
    ICU_IRQ_EX6_DC2            = (ICU_INT_START+26),
    ICU_IRQ_EX7_DC2            = (ICU_INT_START+27),
    ICU_IRQ_WAKEUP1_LLP        = (ICU_INT_START+28),
    ICU_IRQ_SDN_PVD_INTN_5V    = (ICU_INT_START+29),
    ICU_IRQ_IWDT_RST           = (ICU_INT_START+30),
    ICU_IRQ_PCU_CPSS2AP        = (ICU_INT_START+31),
    ICU_IRQ_CEVA_PSU_MAPV_R    = (ICU_INT_START+32),
    ICU_IRQ_CEVA_SEQ_PI_OUT_R  = (ICU_INT_START+33),
    ICU_IRQ_CEVA_UOP_R         = (ICU_INT_START+34),
    ICU_IRQ_CEVA_SN            = (ICU_INT_START+35),
} IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK801 Processor and Core Peripherals  ------- */
#define __CK802_REV               0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             0         /* MGU present or not */
#define __VIC_PRIO_BITS          2         /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0         /* Set to 1 if different SysTick Config is used */
#include "csi_core.h"                     /* Processor and core peripherals */
#include "stdint.h"

#define CONFIG_USART_NUM 4

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define ZX29_UART0_BASE    0x21401000    /*uart0*/
#define ZX29_UART1_BASE    0x21402000    /*uart1*/
#define ZX29_UART2_BASE    0x2140E000    /*uart2*/
#define ZX29_UART3_BASE    0x20134000    /*lp uart*/

#define ZX29_TIMER_0_BASE   0x22000000    /*ck timer0*/
#define ZX29_TIMER_1_BASE   0x22001000    /*ck timer1*/
#define ZX29_TIMER_2_BASE   0x20133000    /*lp timer*/
#define ZX29_TIMER_3_BASE   0x2140B000  /*ad timer0*/
#define ZX29_TIMER_4_BASE   0x2140C000  /*ad timer1*/

#define AD_TIMER_0_BASE     0x2140B000
#define AD_TIMER_1_BASE     0x2140C000

#define AP_WDT_BASEADDR  (0x22004000)  /*4K*/

#define SBY_CRPM_BASEADDR   (0x20140000)
#define AP_CRPM_BASEADDR    (0x22005000)

#define ZX29_I2C0_BASE      0x21405000
#define ZX29_I2C1_BASE      0x2140A000

#define ZX29_SPI0_BASE      0x21403000
#define ZX29_SPI1_BASE      0x21404000
#define ZX29_SPI2_BASE      0x2140D000

#define ZX29_AES_BASE       0x21421000

#define ZX29_PMU_BASE       0x20132000

#define ZX29_BMU_BASE       0x20146000

#define ZX29_SHA_BASE       0x21422000

#define ZX29_RSA_BASE       0x21423000

#define ZX29_RAND_BASE      0x21424000

#define ZX29_SECURE_BASE    0x21420000

#define ZX29_AP_CRPM_BASE   0x22005000

#define ZX29_RTC_BASE       (0x20144000)

#define CONFIG_GPIO_PIN_NUM 55
#define CONFIG_IIC_NUM      2
#define CONFIG_EFLASH_NUM   2
#define CONFIG_SPI_NUM      3
#define CONFIG_TIMER_NUM    5
#define CONFIG_ETH_NUM      2
#define CONFIG_WDT_NUM      1
#define CONFIG_TRNG_NUM     1
#define CONFIG_SHA_NUM      1
#define CONFIG_AES_NUM      1
#define CONFIG_BMU_NUM      1
#define CONFIG_PMU_NUM      1
#define CONFIG_RSA_NUM      1
#define CONFIG_RTC_NUM      1

#define SHA_CONTEXT_SIZE    12

#ifdef CONFIG_HAVE_VIC
#define  ATTRIBUTE_ISR __attribute__((isr))
#else
#define  ATTRIBUTE_ISR
#endif

#ifdef __cplusplus
}
#endif

#endif  /* SOC_H */
