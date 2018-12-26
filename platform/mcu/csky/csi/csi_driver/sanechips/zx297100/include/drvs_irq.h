/**
 * File: Drvs_irq.h
 * Brief: Implementation of Sanechips irq
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef _DRVS_IRQ_H_
#define _DRVS_IRQ_H_

/*******************************************************************************
 *							 Include header files							   *
 ******************************************************************************/
//#include <yoc_auto_config.h>

/*******************************************************************************
 *							   Macro definitions							   *
 ******************************************************************************/
#define SI_VIC_BASE     0xE000E100
#define VIC_ISER (*(volatile unsigned int *)(SI_VIC_BASE + 0x0000))
#define VIC_IWER (*(volatile unsigned int *)(SI_VIC_BASE + 0x0040))
#define VIC_ICER (*(volatile unsigned int *)(SI_VIC_BASE + 0x0080))
#define VIC_IWDR (*(volatile unsigned int *)(SI_VIC_BASE + 0x00C0))
#define VIC_ISPR (*(volatile unsigned int *)(SI_VIC_BASE + 0x0100))
#define VIC_ICPR (*(volatile unsigned int *)(SI_VIC_BASE + 0x0180))
#define VIC_IABR (*(volatile unsigned int *)(SI_VIC_BASE + 0x0200))
#define VIC_IPR0 (*(volatile unsigned int *)(SI_VIC_BASE + 0x0300))
#define VIC_IPR1 (*(volatile unsigned int *)(SI_VIC_BASE + 0x0304))
#define VIC_IPR2 (*(volatile unsigned int *)(SI_VIC_BASE + 0x0308))
#define VIC_IPR3 (*(volatile unsigned int *)(SI_VIC_BASE + 0x030C))
#define VIC_IPR4 (*(volatile unsigned int *)(SI_VIC_BASE + 0x0310))
#define VIC_IPR5 (*(volatile unsigned int *)(SI_VIC_BASE + 0x0314))
#define VIC_IPR6 (*(volatile unsigned int *)(SI_VIC_BASE + 0x0318))
#define VIC_IPR7 (*(volatile unsigned int *)(SI_VIC_BASE + 0x031C))
#define VIC_ISR  (*(volatile unsigned int *)(SI_VIC_BASE + 0x0B00))
#define VIC_IPTR (*(volatile unsigned int *)(SI_VIC_BASE + 0x0B04))

/* define irq numbers */
#define PIC_IRQ_BASE                     0
#define PIC_IRQ_WDT                      (PIC_IRQ_BASE+0)
#define PIC_IRQ_LP_UART                  (PIC_IRQ_BASE+1)
#define PIC_IRQ_TEMP_DET                 (PIC_IRQ_BASE+2)
#define PIC_IRQ_UART1                    (PIC_IRQ_BASE+3)
#define PIC_IRQ_SSP0                     (PIC_IRQ_BASE+4)
#define PIC_IRQ_VBAT_DETN                (PIC_IRQ_BASE+5)
#define PIC_IRQ_I2C0                     (PIC_IRQ_BASE+6)
#define PIC_IRQ_INNER_PWR_OC             (PIC_IRQ_BASE+7)
#define PIC_IRQ_RTC_ALARM                (PIC_IRQ_BASE+8)
#define PIC_IRQ_RTC_TIMER                (PIC_IRQ_BASE+9)
#define PIC_IRQ_EFC1                     (PIC_IRQ_BASE+10)
#define PIC_IRQ_ICP_DSP2CK               (PIC_IRQ_BASE+11)
#define PIC_IRQ_ICP_M02CK                (PIC_IRQ_BASE+12)
#define PIC_IRQ_CK_ITIMER                (PIC_IRQ_BASE+13)
#define PIC_IRQ_TIMER0                   (PIC_IRQ_BASE+14)
#define PIC_IRQ_AD_TIMER0                (PIC_IRQ_BASE+15)
#define PIC_IRQ_AD_TIMER1                (PIC_IRQ_BASE+16)
#define PIC_IRQ_LP_TIMER                 (PIC_IRQ_BASE+17)
#define PIC_IRQ_CP_DMAINT1               (PIC_IRQ_BASE+18)
#define PIC_IRQ_AP_DMAINT1               (PIC_IRQ_BASE+19)
#define PIC_IRQ_LPM0                     (PIC_IRQ_BASE+20)
#define PIC_IRQ_ADC                      (PIC_IRQ_BASE+21)
#define PIC_IRQ_DAC                      (PIC_IRQ_BASE+22)
#define PIC_IRQ_HS_COMPA                 (PIC_IRQ_BASE+23)
#define PIC_IRQ_I2S                      (PIC_IRQ_BASE+25)
#define PIC_IRQ_SECURE                   (PIC_IRQ_BASE+26)
#define PIC_IRQ_SPI_NOR                  (PIC_IRQ_BASE+27)
#define PIC_IRQ_RTC_TIMESTAMP            (PIC_IRQ_BASE+28)
#define PIC_IRQ_EXT0                     (PIC_IRQ_BASE+29)
#define PIC_IRQ_EFC2                     (PIC_IRQ_BASE+30)
#define PIC_IRQ_31                       (PIC_IRQ_BASE+31)

#ifdef CONFIG_HAVE_ICU
#define ICU_IRQ_START                    32
#define ICU_IRQ_WAKE1_SP                 (ICU_IRQ_START+0)
#define ICU_IRQ_WAKE1_LP                 (ICU_IRQ_START+1)
#define ICU_IRQ_PCU_AO2AP_WAKE           (ICU_IRQ_START+2)
#define ICU_IRQ_CP_WTD_RST               (ICU_IRQ_START+3)
#define ICU_IRQ_LP_UART_CTS              (ICU_IRQ_START+4)
#define ICU_IRQ_EXT2                     (ICU_IRQ_START+5)
#define ICU_IRQ_EXT3                     (ICU_IRQ_START+6)
#define ICU_IRQ_EXT4                     (ICU_IRQ_START+7)
#define ICU_IRQ_EXT5                     (ICU_IRQ_START+8)
#define ICU_IRQ_EXT6                     (ICU_IRQ_START+9)
#define ICU_IRQ_EXT7                     (ICU_IRQ_START+10)
#define ICU_IRQ_LP_UART_RXD              (ICU_IRQ_START+11)
#define ICU_IRQ_UART0                    (ICU_IRQ_START+12)
#define ICU_IRQ_SSP1                     (ICU_IRQ_START+13)
#define ICU_IRQ_I2C1                     (ICU_IRQ_START+14)
#define ICU_IRQ_UART2                    (ICU_IRQ_START+15)
#define ICU_IRQ_SSP2                     (ICU_IRQ_START+16)
#define ICU_IRQ_TIMER1                   (ICU_IRQ_START+17)
#define ICU_IRQ_CEVA_GVI_R               (ICU_IRQ_START+18)
#define ICU_IRQ_EX0_DC2                  (ICU_IRQ_START+19)
#define ICU_IRQ_EX1_DC2                  (ICU_IRQ_START+20)
#define ICU_IRQ_EX2_DC2                  (ICU_IRQ_START+21)
#define ICU_IRQ_EX3_DC2                  (ICU_IRQ_START+22)
#define ICU_IRQ_EXT1                     (ICU_IRQ_START+23)
#define ICU_IRQ_EX4_DC2                  (ICU_IRQ_START+24)
#define ICU_IRQ_EX5_DC2                  (ICU_IRQ_START+25)
#define ICU_IRQ_EX6_DC2                  (ICU_IRQ_START+26)
#define ICU_IRQ_EX7_DC2                  (ICU_IRQ_START+27)
#define ICU_IRQ_WAKEUP1_LLP              (ICU_IRQ_START+28)
#define ICU_IRQ_SDN_PVD_INTN_5V          (ICU_IRQ_START+29)
#define ICU_IRQ_IWDT_RST                 (ICU_IRQ_START+30)
#define ICU_IRQ_PCU_CPSS2AP              (ICU_IRQ_START+31)
#define ICU_IRQ_CEVA_PSU_MAPV_R          (ICU_IRQ_START+32)
#define ICU_IRQ_CEVA_SEQ_PI_OUT_R        (ICU_IRQ_START+33)
#define ICU_IRQ_CEVA_UOP_R               (ICU_IRQ_START+34)
#define ICU_IRQ_CEVA_SN                  (ICU_IRQ_START+35)
#endif


/* define vic priority ,smaller priority number higher priority level */
#define PRIORITY_LEVEL1 0
#define PRIORITY_LEVEL2 1
#define PRIORITY_LEVEL3 2
#define PRIORITY_LEVEL4 3

/*******************************************************************************
 *							   Type definitions								   *
 ******************************************************************************/
#ifdef CONFIG_HAVE_ICU
typedef void (*icu_callback)(int irq, void *context);
#endif

#ifndef __ASSEMBLY__
#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif
#endif

#endif /* _DRVS_IRQ_H_ */

