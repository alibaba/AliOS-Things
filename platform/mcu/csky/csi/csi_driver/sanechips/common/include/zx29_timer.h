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
/******************************************************************************
 * @file     zx29_timer.h
 * @brief    header file for timer driver
 * @version  V1.0
 * @date     10. October 2017
 ******************************************************************************/
#ifndef __ZX29_TIMER_H
#define __ZX29_TIMER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <soc.h>
#include <addrspace.h>

/*******************************************************************************
 *                                                         Macro definitions                                                       *
 ******************************************************************************/
#define BIT(n)                  (1UL << (n))
#define BIT_MASK(n)             (BIT(n) - 1)

#define TIMER_CLKSEL_BASE     (AP_CRPM_BASEADDR + 0x48)
#define TIMER_DIVSEL_BASE     (AP_CRPM_BASEADDR + 0x4c)
#define IRQ_CLEAR_BASE        (AP_CRPM_BASEADDR + 0x88)
#define TIMER_CLKEN_BASE      (AP_CRPM_BASEADDR + 0x50)
#define AD_TIMER0_CLKEN_BASE   (LSP_CRPM_BASEADDR + 0x3c)
#define AD_TIMER1_CLKEN_BASE   (LSP_CRPM_BASEADDR + 0x40)
#define AD_TIMER0_CLKSEL_BASE     (LSP_CRPM_BASEADDR + 0x3c)
#define AD_TIMER1_CLKSEL_BASE     (LSP_CRPM_BASEADDR + 0x40)

#define TIMER0_CLKSEL         BIT(0)
#define TIMER1_CLKSEL         BIT(3)
#define TIMER_CLKDIV_MASK     BIT_MASK(4)
#define TIMER_WCLKDIV_MASK    BIT_MASK(16)
#define TIMER_STATUS_OVF      BIT(0)
#define TIMER_SYNC_MASK       BIT_MASK(4)
#define TIMER_CLKEN           BIT_MASK(3)
#define AD_TIMER_CLKEN        BIT_MASK(2)

#define TIMER0               (0)
#define TIMER1               (1)
#define TIMER2               (2)
#define TIMER3               (3)
#define TIMER4               (4)

/*
 *  define the bits for TxControl
 */
//#define DW_TIMER_TXCONTROL_ENABLE      (1UL << 0)
//#define DW_TIMER_TXCONTROL_MODE        (1UL << 1)
//#define DW_TIMER_TXCONTROL_INTMASK     (1UL << 2)

#define DW_TIMER_INIT_DEFAULT_VALUE     0x7ffffff

#define BSP_TCFLAGS_INACTIVE 0        ///< bit = 0: The timer is not running
#define BSP_TCFLAGS_ACTIVE   (1 << 0) ///< bit = 1: The timer is running
#define BSP_TCFLAGS_HANDLER  (1 << 1) ///< bit = 1: Call the user function when the timer expires

#define USEC_PER_SEC  1000000

/* User function prototype. Returns true to reload the timer, and the
 * function can modify the next interval if desired.
 */
typedef struct {
    uint32_t    version;        /*版本号寄存器 0x000*/
    uint32_t    config;         /*配置寄存器   0x004*/
    uint32_t    load;           /*重载寄存器   0x008*/
    uint32_t    evt_ctrl;       /*更新事件控制寄存器 0x00c*/
    uint32_t    sync_en;        /*同步使能寄存器 0x010*/
    uint32_t    en;             /*使能寄存器    0x014*/
    uint32_t    status;         /*状态寄存器    0x018*/
    uint32_t    irq_en;         /*中断使能寄存器 0x01c*/
    uint32_t    cnt;            /*计数值寄存器 0x020*/
} zx_timer_reg_t;

typedef struct {
    uint32_t    version;        /*版本号寄存器 0x000*/
    uint32_t    config;         /*配置寄存器   0x004*/
    uint32_t    load;           /*重载寄存器   0x008*/
    uint32_t    trg_config;           /*重载寄存器   0x008*/
    uint32_t    evt_ctrl;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch1_config;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch2_config;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch3_config;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch4_config;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch1_ccr;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch2_ccr;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch3_ccr;       /*更新事件控制寄存器 0x00c*/
    uint32_t    ch4_ccr;       /*更新事件控制寄存器 0x00c*/
    uint32_t    sync_en;        /*同步使能寄存器 0x010*/
    uint32_t    en;             /*使能寄存器    0x014*/
    uint32_t    status;         /*状态寄存器    0x018*/
    uint32_t    irq_en;         /*中断使能寄存器 0x01c*/
    uint32_t    cnt;            /*计数值寄存器 0x020*/
} ad_timer_reg_t;

#endif /* __ZX29_TIMER_H */
