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
#define  SYSTEM_CLOCK    (20000000)
#endif
/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
    /* ----------------------  CSKYCK801 Specific Interrupt Numbers  --------------------- */
    GPIOA_IRQn                      =   0,
    CORET_IRQn                      =   1,      /* core Timer Interrupt */
    TIMA0_IRQn                      =   2,      /* timerA0 Interrupt */
    TIMA1_IRQn                      =   3,      /* timerA1 Interrupt */
    WDT_IRQn                        =   5,      /* wdt Interrupt */
    UART0_IRQn                      =   6,      /* uart0 Interrupt */
    UART1_IRQn                      =   7,      /* uart1 Interrupt */
    UART2_IRQn                      =   8,      /* uart2 Interrupt */
    I2C0_IRQn                       =   9,      /* i2c0 Interrupt */
    I2C1_IRQn                       =   10,     /* i2c1 Interrupt */
    SPI1_IRQn                       =   11,     /* spi0 Interrupt */
    SPI0_IRQn                       =   12,     /* spi1 Interrupt */
    RTC_IRQn                        =   13,     /* rtc Interrupt */
    EXTWAK_IRQn                     =   14,     /* extwakeup Interrupt */
    DMAC_IRQn                       =   17,     /* dmac Interrupt */
    PMU_IRQn                        =   18,     /* pmu Interrupt */
    PWM_IRQn                        =   19,     /* pwm Interrupt */
    UART3_IRQn                      =   21,     /* uart3 Interrupt */
    SPU_IRQn                        =   22,     /* spu Interrupt */
    TIMB0_IRQn                      =   23,     /* timerB0 Interrupt */
    TIMB1_IRQn                      =   24,     /* timerB1 Interrupt */
    GPIOB_IRQn                      =   27,     /* GPIOB Interrupt */
    AES_IRQn                        =   26,     /* aes Interrupt */
    RSA_IRQn                        =   28,     /* rsa Interrupt */
    SHA_IRQn                        =   29,     /* sha Interrupt */
} IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK801 Processor and Core Peripherals  ------- */
#define __CK803_REV               0x0000U   /* Core revision r0p0 */
#define __MGU_PRESENT             0         /* MGU present or not */
#define __NVIC_PRIO_BITS          2         /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0         /* Set to 1 if different SysTick Config is used */

#include "core_ck803S.h"                     /* Processor and core peripherals */
#include "stdint.h"

typedef enum {
    CKENUM_DMA_UART0_RX,
    CKENUM_DMA_UART0_TX,
    CKENUM_DMA_UART1_RX,
    CKENUM_DMA_UART1_TX,
    CKENUM_DMA_ADC_RX,
    CKENUM_DMA_ADC_TX,
    CKENUM_DMA_SPI1_RX,
    CKENUM_DMA_SPI1_TX,
    CKENUM_DMA_SPI0_RX,
    CKENUM_DMA_SPI0_TX,
    CKENUM_DMA_IIC_RX,
    CKENUM_DMA_IIC_TX,
    CKENUM_DMA_IIC1_RX,
    CKENUM_DMA_IIC1_TX,
    CKENUM_DMA_IIS_RX,
    CKENUM_DMA_IIS_TX,
    CKENUM_DMA_MEMORY
} ckenum_dma_device_e;

#define CONFIG_CRC_NUM  1
#define CONFIG_IIC_NUM  2
#define CONFIG_TRNG_NUM 1
#define CONFIG_AES_NUM  1
#define CONFIG_RSA_NUM  1
#define CONFIG_SHA_NUM  1
#define CONFIG_SPI_NUM  2
#define CONFIG_PWM_NUM  1
#define CONFIG_TIMER_NUM 8
#define CONFIG_RTC_NUM 1
#define CONFIG_WDT_NUM 1
#define CONFIG_DMAC_NUM 1
#define CONFIG_GPIO_NUM            2
#define CONFIG_GPIO_PIN_NUM        43
#define CONFIG_USART_NUM 4
#define CONFIG_SPU_NUM 1

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
/* --------------------------  CPU FPGA memory map  ------------------------------- */
#define CSKY_QSPIMEM_BASE          (0x10000000UL)
#define CSKY_ISRAM_BASE            (0x18000000UL)
#define CSKY_SRAM_BASE             (0x20000000UL)
#define CSKY_PMU_BASE              (0x40000000UL)
#define CSKY_DMA0_BASE             (0x40001000UL)
#define CSKY_OTP_BASE              (0x40006000UL)
#define CSKY_SASC_BASE             (0x40009000UL)
#define CSKY_SPU_BASE              (0x40020000UL)
#define CSKY_SQSPIC_BASE           (0x40030000UL)
#define CSKY_SASC_BASE             (0x40009000UL)

/* SUB0*/
#define CSKY_AES_BASE              (0x4000D000UL)
#define CSKY_SHA_BASE              (0x4000E000UL)
#define CSKY_RSA_BASE              (0x4000F000UL)
#define CSKY_CRC_BASE              (0x40010000UL)
#define CSKY_TRNG_BASE             (0x40015000UL)
/* APB0 */
#define CSKY_TIM0_BASE             (0x50000000UL)
#define CSKY_TIM1_BASE             (0x50000400UL)
#define CSKY_TIM2_BASE             (0x50000800UL)
#define CSKY_TIM3_BASE             (0x50000C00UL)
#define CSKY_RTC0_BASE             (0x50004000UL)
#define CSKY_WDT_BASE              (0x50008000UL)
#define CSKY_SPI0_BASE             (0x5000C000UL)
#define CSKY_UART0_BASE            (0x50010000UL)
#define CSKY_UART1_BASE            (0x50010400UL)
#define CSKY_I2C0_BASE             (0x50014000UL)
#define CSKY_GPIO0_BASE            (0x50018000UL)
#define CSKY_PWM_BASE              (0x5001C000UL)
#define CSKY_ADC_BASE              (0x50020000UL)
#define CSKY_USI0_BASE             (0x50028000UL)
/* APB1*/
#define CSKY_TIM4_BASE             (0x60000000UL)
#define CSKY_TIM5_BASE             (0x60000400UL)
#define CSKY_TIM6_BASE             (0x60000800UL)
#define CSKY_TIM7_BASE             (0x60000C00UL)
#define CSKY_LPWDT_BASE            (0x60008000UL)
#define CSKY_SPI1_BASE             (0x6000C000UL)
#define CSKY_UART2_BASE            (0x60010000UL)
#define CSKY_UART3_BASE            (0x60010400UL)
#define CSKY_I2C1_BASE             (0x60014000UL)
#define CSKY_GPIO1_BASE            (0x60018000UL)
#define CSKY_TIPC_BASE             (0x6001c000UL)
#define CSKY_ETB_BASE              (0x60024000UL)
#define CSKY_USI1_BASE             (0x60028000UL)
#define CSKY_DAC_BASE              (0x6002C000UL)
#define CSKY_IOC_BASE              (0x60030000UL)


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
#define CSKY_UART1                 ((   CSKY_UART_TypeDef *)    CSKY_UART1_BASE)
#define CSKY_SHA                   ((   CSKY_SHA_TypeDef *)     CSKY_SHA_BASE)

#ifdef CONFIG_HAVE_VIC
#define  ATTRIBUTE_ISR __attribute__((isr))
#else
#define  ATTRIBUTE_ISR
#endif

#ifdef __cplusplus
}
#endif

#endif  /* SOC_H */
