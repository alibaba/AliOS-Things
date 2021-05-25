/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/**************************************************************************//**
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _SOC_H_
#define _SOC_H_

#include <stdint.h>
#include <csi_core.h>
#include <sys_freq.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IHS_VALUE
#define  IHS_VALUE    (20000000)
#endif

#ifndef EHS_VALUE
#define  EHS_VALUE    (20000000)
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn {
    /* ----------------------  SmartL Specific Interrupt Numbers  --------------------- */
    Supervisor_Software_IRQn        =   1,     
    Machine_Software_IRQn           =   3,     
    Supervisor_Timer_IRQn           =   5,     
    CORET_IRQn                      =   7,     
    Supervisor_External_IRQn        =   9,     
    Machine_External_IRQn           =   11,    
    UART_IRQn                       =   32+0,     /* uart Interrupt */
    TIM0_IRQn                       =   32+2,     /* timer0 Interrupt */
    TIM1_IRQn                       =   32+3,     /* timer1 Interrupt */
    TIM2_IRQn                       =   32+4,     /* timer1 Interrupt */
    TIM3_IRQn                       =   32+5,     /* timer1 Interrupt */
    GPIO0_IRQn                      =   32+6,     /* gpio0 Interrupt */
    GPIO1_IRQn                      =   32+7,     /* gpio1 Interrupt */
    GPIO2_IRQn                      =   32+8,     /* gpio2 Interrupt */
    GPIO3_IRQn                      =   32+9,     /* gpio3 Interrupt */
    GPIO4_IRQn                      =   32+10,     /* gpio4 Interrupt */
    GPIO5_IRQn                      =   32+11,     /* gpio5 Interrupt */
    GPIO6_IRQn                      =   32+12,     /* gpio6 Interrupt */
    GPIO7_IRQn                      =   32+13,     /* gpio7 Interrupt */
    STIM0_IRQn                      =   32+14,     /* stimer0 Interrupt */
    STIM1_IRQn                      =   32+15,     /* stimer0 Interrupt */
    STIM2_IRQn                      =   32+16,     /* stimer0 Interrupt */
    STIM3_IRQn                      =   32+17,     /* stimer0 Interrupt */
    PAD_IRQn                        =   32+18,     /* pad Interrupt */
}
IRQn_Type;

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

#define CONFIG_TIMER_NUM    4
#define CONFIG_USART_NUM    1
#define CONFIG_GPIO_NUM     8
#define CONFIG_GPIO_PIN_NUM 8

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
/* --------------------------  CPU FPGA memory map  ------------------------------- */
#define CSKY_SRAM_BASE              (0x20000000UL)

#define CSKY_UART_BASE              (0x10015000UL)
#define CSKY_PMU_BASE               (0x10016000UL)
#define CSKY_TIMER0_BASE            (0x10011000UL)
#define CSKY_TIMER1_BASE            (0x10011014UL)
#define CSKY_TIMER2_BASE            (0x10011028UL)
#define CSKY_TIMER3_BASE            (0x1001103cUL)
#define CSKY_TIMER_CONTROL_BASE     (0x100110a0UL)
#define CSKY_CLK_GEN_BASE           (0x10017000UL)
#define CSKY_STIMER0_BASE           (0x10018000UL)
#define CSKY_STIMER1_BASE           (0x10018014UL)
#define CSKY_STIMER2_BASE           (0x10018028UL)
#define CSKY_STIMER3_BASE           (0x1001803cUL)
#define CSKY_STIMER_CONTROL_BASE    (0x100110a0UL)

#define CSKY_GPIOA_BASE             (0x10019000UL)
#define CSKY_GPIOA_CONTROL_BASE     (0x10019030UL)
#define CSKY_SMPU_BASE              (0x1001a000UL)

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
#define CSKY_UART                  ((   CSKY_UART_TypeDef *)    CSKY_UART_BASE)

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_H_ */
