/**
 * File: pow_si.h
 * Brief: bsp APIs of Sanechips pow
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

#ifndef _POW_SI_H
#define _POW_SI_H

/*******************************************************************************
 *							 Include header files							   *
 ******************************************************************************/
#include <reg_define_si.h>

/*******************************************************************************
 *							   Macro definitions							   *
 ******************************************************************************/
#define EFLASH2_BASE        (0x10000000UL)
#define CP_NV_RW_BASE    	(EFLASH2_BASE)
#define CP_NV_RW_VLEN      	(3500UL)
#define CP_NV_RW_TLEN     	(4 * 1024UL)
#define AP_NV_RW_SYS_BASE 	(CP_NV_RW_BASE + CP_NV_RW_TLEN)
#define CHIP_SLEEP_ENTER_ADDR   (PMU_IRAM_AON_BASE + 0x7f8)
#define OSS_ICP_CHANNEL_PSM		(5)
#define AP_NVRW_BASE   (AP_NV_RW_SYS_BASE)

#define assert(__e) ((void)0)

#define VIC_IRQ_RTC_ALARM         (1 << PIC_IRQ_RTC_ALARM)
#define VIC_IRQ_ICP_DSP2CK        (1 << PIC_IRQ_ICP_DSP2CK)
#define PIC_IRQ_TIMER0_WAKE       (1 << PIC_IRQ_TIMER0)
#define VIC_IRQ_LPM0              (1 << PIC_IRQ_LPM0)
#define PIC_IRQ_CK_ITIMER_WAKE    (1 << PIC_IRQ_CK_ITIMER)
#define VIC_IRQ_LP_TIMER          (1 << PIC_IRQ_LP_TIMER)
#define VIC_IRQ_LP_UART           (1 << PIC_IRQ_LP_UART)
#define VIC_IRQ_UART1             (1 << PIC_IRQ_UART1)
#define VIC_IRQ_31                (1 << PIC_IRQ_31)

#define VIC_WAKEUP_SOURCE   ((PIC_IRQ_TIMER0_WAKE)|(VIC_IRQ_RTC_ALARM)|(VIC_IRQ_LPM0)|(VIC_IRQ_ICP_DSP2CK)|(VIC_IRQ_LP_UART)|(VIC_IRQ_LP_TIMER)|(VIC_IRQ_UART1))

#define ALL_CHIP_STANDBY  	(1*60*60*1000)
#define ALL_CHIP_ENTER      (1*1000)
#define ALL_CHIP_DORMANT    (2*1000)

#define LPM_SLEEP_MODE    	(7)

/*pcu AO reg map*/
#define CPSS_SLEEP_MODE_CONFIG_REG               (PCU_AO_BASEADDR)
#define APSS_CONFIG_REG                          (PCU_AO_BASEADDR+0x5*4)//0x20132014
#define	APSS_SW_CONTROL_CONFIG_REG				 (PCU_AO_BASEADDR+0x06*4)//0x20132018
#define	VCXO_STABLE_TIME_CONFIG_REG			     (PCU_AO_BASEADDR+0x0a*4)//0x20132028
#define	TOP_AO_SLEEP_MODE_CONFIG_REG             (PCU_AO_BASEADDR+0x0b*4)//0x2013202c
#define APSS_SLEEP_TIME_REG                      (PCU_AO_BASEADDR+0x17*4)//0x2013205c
#define TOP_AO_SW_CONTROL_CONFIG_REG			 (PCU_AO_BASEADDR+0x0f*4)//0x2013203c
#define AO_APSS_INT_WAKE_DIS_REG1                (PCU_AO_BASEADDR+0x11*4)//0x20132044
#define	AO_CPSS_INT_WAKEUP_DIS1_CONFIG_REG		 (PCU_AO_BASEADDR+0x12*4)//0x20132048
#define AO_INT_WAKEUP_TYPE_REG1                  (PCU_AO_BASEADDR+0x13*4)//0x2013204c
#define AO_INT_WAKEUP_POL_REG1                   (PCU_AO_BASEADDR+0x14*4)//0x20132050
#define AO_INT_WAKEUP_CLEAR_REG1                 (PCU_AO_BASEADDR+0x15*4)//0x20132054
#define CPSS_SLEEP_TIME_CONFIG_REG               (PCU_AO_BASEADDR+0x16*4)//0x20132058
//#define    APSS_SLEEP_TIME_CONFIG_REG                   (PCU_AO_BASE_ADDR+0x17*4)
#define TOP_SLEEP_TIME_ENABLE_CONFIG_REG         (PCU_AO_BASEADDR+0x18*4)//0x20132060
#define TOP_AO_SLEEPTIME_THRESHOLD_CONFIG_REG    (PCU_AO_BASEADDR+0x19*4)//0x20132064
#define EXTERNAL_INT2CORE_DISABLE_CONFIG_REG     (PCU_AO_BASEADDR+0x1e*4)//0x20132078
//#define READ_FSM_REG                                                    (PCU_AO_BASE_ADDR+0xf7*4)
#define PCU_SW_CONFIG_MASK_REG                   (PCU_AO_BASEADDR+0x1f*4)//0x2013207c
//#define    FSM_SEND_INT_CONFIG_REG                       (PCU_AO_BASE_ADDR+0x20*4)//0x20132080
#define CPSS_SLEEP2AP_CONFIG_REG                 (PCU_AO_BASEADDR+0x21*4)//0x20132084
#define IRAM_TCM_CTRL_CONNFIG_REG1               (PCU_AO_BASEADDR+0x22*4)//0x20132088
#define IRAM_TCM_CTRL_CONNFIG_REG2               (PCU_AO_BASEADDR+0x23*4)//0x2013208c
#define PCU_FSM_SEND_INT_CONFIG_REG              (PCU_AO_BASEADDR+0x20*4)//
#define AO_READ_FSM_REG                          (PCU_AO_BASEADDR+0xf7*4)//0x201323dc
#define AO_RD_SIGNAL_STA_REG1                    (PCU_AO_BASEADDR+0xf8*4)//0x201323e0
#define AO_RD_SIGNAL_STA_REG2                    (PCU_AO_BASEADDR+0xf9*4)//0x201323e4

/*pcu SBY reg map*/
#define TOP_SBY_SLEEP_MODE_CONFIG_REG            (PCU_SBY_BASEADDR)
#define TOP_SBY_CONFIG_REG                       (PCU_SBY_BASEADDR+0x1*4)//0x20141004
#define TOP_SBY_SW_CONTROL_CONFIG_REG            (PCU_SBY_BASEADDR+0x2*4)//0x20141008
#define SHARED_DEVICE1_REG                       (PCU_SBY_BASEADDR+0x4*4)//0x2014100c
#define SHARED_DEVICE2_REG                       (PCU_SBY_BASEADDR+0x5*4)//0x20141014
#define SBY_INT_WAKEUP_DIS_REG1                  (PCU_SBY_BASEADDR+0x6*4)//0x20141018
#define SBY_INT_WAKEUP_TYPE_REG1                 (PCU_SBY_BASEADDR+0x7*4)//0x2014101c
#define SBY_INT_WAKEUP_POL_REG1                  (PCU_SBY_BASEADDR+0x8*4)//0x20141020
#define SBY_INT_WAKEUP_CLEAR_REG1                (PCU_SBY_BASEADDR+0x9*4)//0x20141024
#define SBY_INT_WAKEUP_CLEAR_REG2                (PCU_SBY_BASEADDR+0xa*4)//0x20141028
#define SBY_RD_SIGNAL_STA_REG                    (PCU_SBY_BASEADDR+0xb*4)//0x2014102c
#define SBY_RESTORE_INT_DEAL_CONFIG_ADDR         (PCU_SBY_BASEADDR+0xd*4)

/*PMU_CTRL reg map*/
#define PMU_CTRL_BACKUP_REG1                     (PMU_CTRL_BASEADDR + 0x9*4)
#define PMU_CTRL_BACKUP_REG2                     (PMU_CTRL_BASEADDR + 0xa*4)
#define PMU_CTRL_BACKUP_REG3                     (PMU_CTRL_BASEADDR + 0xb*4)
#define PMU_CTRL_BACKUP_REG4                     (PMU_CTRL_BASEADDR + 0xc*4)


/*----------------------------------IRAM AON address start-------------------------------*/

/*iram_aon map*/
#define PMU_IRAM_AON_BASE           (0x20110000)
#define PSM_SCPU_CONTENT_ADDR_STORE (PMU_IRAM_AON_BASE + 0x168)
#define EFLASH_POWON_ADDR_STORE     (PMU_IRAM_AON_BASE + 0x16c)
#define EFLASH_RST_ADDR_STORE       (PMU_IRAM_AON_BASE + 0x170)

#define TICK_REG_SAVE_ADDR          (PMU_IRAM_AON_BASE + 0x338)
#define CORETIM_REG_SAVE_ADDR       (PMU_IRAM_AON_BASE + 0x354)

#define ICU_MASK_REG_SAVE_ADDR      (PMU_IRAM_AON_BASE + 0x360)
#define ICU_MODE_REG1_SAVE_ADDR     (PMU_IRAM_AON_BASE + 0x374)

#define CHIP_DOR_FLAG_ADDR          (PMU_IRAM_AON_BASE + 0x800)
#define CHIP_SLEEP_EXIT_ADDR        (PMU_IRAM_AON_BASE + 0x7fc)
#define CHIP_SLEEP_FLAG_ADDR        (PMU_IRAM_AON_BASE + 0x7ec)

#define WAKE_UP_CODE_ADDRESS        (PMU_IRAM_AON_BASE + 0x100)
#define PSM_NSCPU_CONTENT_ADDR      (PMU_IRAM_AON_BASE + 0x26c)

#define CP_SLEEP_TIME_REG           (PMU_IRAM_AON_BASE + 0x100c)
#define PMU_BOOST_EN_FLAG           (PMU_IRAM_AON_BASE + 0x1000)

#define WAKEUP_CODE_LENGTH          (0x64)

#define PMU_DOR_CONFIG_REG1         (PMU_CTRL_BASEADDR + 0x08)
#define BACKUP_REG_RESERVE1         (PMU_CTRL_BASEADDR + 0x24)
#define BACKUP_REG_RESERVE2         (PMU_CTRL_BASEADDR + 0x28)

#define RESET_STATE_HOT_BOOT        (PMU_CTRL_BASEADDR + 0x88)
#define PMU_BOOST_CONFIG            (PMU_CTRL_BASEADDR + 0x50)
#define PMU_REG_ENN_CONFIG          (PMU_CTRL_BASEADDR + 0x94)


#define CPU_CK_INTC_BASE  			(0xE000E000)
#define CPU_CK_CRU_BASE   			(0xE000F000)

#define PCU_RPMWFI_BYPASS           (1U << 0x3)
#define	PCU_SLEEP_MODE				((1U << 0)|(PCU_RPMWFI_BYPASS))
#define	PCU_POWEROFF_MODE			((1U << 1)|(PCU_RPMWFI_BYPASS))		/* power off &bypass m0 wfi */

/*----------------------------------sleep mod & set addr end---------------------------*/
#define AO_WAKE_SRC_EXT0                (0)
#define AO_WAKE_SRC_EXT1                (1)
#define AO_WAKE_SRC_EXT2                (2)
#define AO_WAKE_SRC_EXT3                (3)
#define AO_WAKE_SRC_EXT4                (4)
#define AO_WAKE_SRC_EXT5                (5)
#define AO_WAKE_SRC_EXT6                (6)
#define AO_WAKE_SRC_EXT7                (7)
#define AO_WAKE_SRC_LP_UART_CTS         (8)
#define AO_WAKE_SRC_LP_UART_RXD         (9)
#define AO_WAKE_SRC_LP_TIMER_INT        (0)
#define AO_WAKE_SRC_LP_UART_INT         (11)
#define AO_WAKE_SRC_UART0_INT     		(12)
#define AO_WAKE_SRC_UART2_INT     		(13)
#define AO_WAKE_SRC_ADC_INT     		(14)
#define AO_WAKE_SRC_DAC_INT     		(15)
#define AO_WAKE_SRC_HS_COMPA_INT     	(16)
#define AO_WAKE_SRC_AD_TIMER0_INT     	(17)
#define AO_WAKE_SRC_AD_TIMER1_INT     	(18)
#define AO_WAKE_SRC_ICP_DSP2M0_INT      (19)
#define AO_WAKE_SRC_ICP_DSP2CK_INT      (21)
#define AO_WAKE_SRC_TOP2APSS_INT        (23)
#define AO_WAKE_TYPE_TOP2APSS_INT       (22)

#define SBY_WAKE_SRC_EXT0               (4)
#define SBY_WAKE_SRC_EXT1               (5)
#define SBY_WAKE_SRC_EXT2               (6)
#define SBY_WAKE_SRC_EXT3               (7)
#define SBY_WAKE_SRC_EXT4               (8)
#define SBY_WAKE_SRC_EXT5               (9)
#define SBY_WAKE_SRC_EXT6               (10)
#define SBY_WAKE_SRC_EXT7               (11)
#define SBY_WAKE_SRC_RTC_ALARM_INT      (14)
#define SBY_WAKE_SRC_RTC_TIMER_INT      (15)
#define SBY_WAKE_SRC_LPM0_INT           (17)
#define SBY_WAKE_SRC_IWTD_INT           (20)

#define AO_WAKE_TYPE_LP_UART_INT        (11)

#define SBY_WAKE_TYPE_RTC_ALARM_INT     (13)
#define SBY_WAKE_TYPE_EXT0_INT          (2)
#define SBY_WAKE_TYPE_LPM0_INT          (16)

#define SBY_WAKE_POL_RTC_ALARM_INT      (13)
#define SBY_WAKE_CLEAR_RTC_ALARM_INT    (13)

/*------------------AO CRPM---------------------*/
#define	AO_CK802_BOOTLINK       (0x30)

#define SBY_CRM_RTC_CLKEN       (SBY_CRPM_BASEADDR + 0x1c)
#define CK_TIMER_CLKEN_BASEADDR (AP_CRPM_BASEADDR + 0x50)

#define CK802_ICU_INT_MASK0     (ICU_CK_BASEADDR + 0x290)
#define CK802_ICU_INT_MASK1     (ICU_CK_BASEADDR + 0x294)
#define CK802_ICU_INT_MASK2     (ICU_CK_BASEADDR + 0x298)
#define CK802_ICU_INT_MASK3     (ICU_CK_BASEADDR + 0x29C)
#define CK802_ICU_MOD_REG1_BASE (ICU_CK_BASEADDR + 0x80)
#define CK802_ICU_MOD_REG2_BASE (ICU_CK_BASEADDR + 0x180)

#define CK802_TIMER0_REG_BASE   (0x22000000)

#define CK802_TIMER_CONFIG_OFFSET_ADDR            (0x4)
#define CK802_TIMER_LOAD_OFFSET_ADDR              (0x8)
#define CK802_TIMER_EVT_OFFSET_ADDR               (0xC)
#define CK802_TIMER_SYNC_EN_OFFSET_ADDR           (0x10)
#define CK802_TIMER_EN_OFFSET_ADDR                (0x14)
#define CK802_TIMER_STA_OFFSET_ADDR               (0x18)
#define CK802_TIMER_INT_EN_OFFSET_ADDR            (0x1C)
#define CK802_TIMER_CNT_OFFSET_ADDR               (0x20)

/*-----------------TOP CRPM---------------------*/
#define TOP_CRPM_MPLL_CFG       (TOP_CRPM_BASEADDR + 0x010)
#define TOP_CRPM_APLL_CFG       (TOP_CRPM_BASEADDR + 0x018)
#define TOP_CRPM_XTAL_32K_CFG   (TOP_CRPM_BASEADDR + 0x024)
#define TOP_CRPM_PD_AHB_SEL     (TOP_CRPM_BASEADDR + 0x030)
#define TOP_CRPM_PD_AHB_CLKEN   	(TOP_CRPM_BASEADDR + 0x040)
#define TOP_CRPM_PD_TO_AP_CLKEN		(TOP_CRPM_BASEADDR + 0x04c)
#define TOP_CRPM_PD_TO_AON_CLKEN	(TOP_CRPM_BASEADDR + 0x050)
#define TOP_CRPM_PD_TO_OUT_CLKEN	(TOP_CRPM_BASEADDR + 0x058)
#define TOP_CRPM_PD_MATRIX_CLKEN	(TOP_CRPM_BASEADDR + 0x060)
#define TOP_CRPM_SPINOR				(TOP_CRPM_BASEADDR + 0x074)



/*-----------------AP CRPM---------------------*/
#define AP_CRPM_SEC_ENABLE		(AP_CRPM_BASEADDR)
#define AP_CRPM_AP_TO_CP_FLAG   (AP_CRPM_BASEADDR + 0x4)
#define AP_CRPM_CK_IDLE_EN      (AP_CRPM_BASEADDR + 0xc)
#define AP_CRPM_M0_CLKEN        (AP_CRPM_BASEADDR + 0x20)

/*-----------------AO CRPM---------------------*/
#define AO_CRPM_AON_CLKEN       (A0_CRPM_BASEADDR + 0x10)


/*=============read/write reg api========================================*/
#define MIN(a,b)   ((a) >= (b)?(b):(a))
#define pow_readl(addr) ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })
#define pow_writel(addr,val) (void)((*(volatile unsigned int *) (addr)) = (val))
#define pow_setregl(addr,val) (void)((*(volatile unsigned int *) (addr)) = (pow_readl(addr)|val))
#define pow_clrregl(addr,val) (void)((*(volatile unsigned int *) (addr)) = (pow_readl(addr)&(~val)))

#define LPM_RST_REG             ((LPM_BASEADDR + ((uint32_t)0x0 << 2)))
#define LPM_CNT_SYNC            ((LPM_BASEADDR + ((uint32_t)0x1 << 2)))
#define LPM_CNT                 ((LPM_BASEADDR + ((uint32_t)0x3 << 2)))
#define LPM_TIME_EN             ((LPM_BASEADDR + ((uint32_t)0x10 << 2)))
#define LPM_INT_CLEAR           ((LPM_BASEADDR + ((uint32_t)0x11 <<2)))
#define LPM_TIME1_CFG           ((LPM_BASEADDR + ((uint32_t)0x13 <<2)))

#endif

