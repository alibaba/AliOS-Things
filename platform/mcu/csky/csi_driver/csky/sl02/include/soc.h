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
    /* ----------------------  CSKYCK803 Specific Interrupt Numbers  --------------------- */
    UART_IRQn                       =   0,      /* uart Interrupt */
    CORET_IRQn                      =   1,      /* core Timer Interrupt */
    TIM0_IRQn                       =   2,      /* timer0 Interrupt */
    TIM1_IRQn                       =   3,      /* timer1 Interrupt */
    TIM2_IRQn                       =   4,      /* timer1 Interrupt */
    TIM3_IRQn                       =   5,      /* timer1 Interrupt */
    GPIO0_IRQn                      =   7,      /* gpio0 Interrupt */
    GPIO1_IRQn                      =   8,      /* gpio1 Interrupt */
    GPIO2_IRQn                      =   9,      /* gpio2 Interrupt */
    GPIO3_IRQn                      =   10,     /* gpio3 Interrupt */
    GPIO4_IRQn                      =   11,     /* gpio4 Interrupt */
    GPIO5_IRQn                      =   12,     /* gpio5 Interrupt */
    GPIO6_IRQn                      =   13,     /* gpio6 Interrupt */
    GPIO7_IRQn                      =   14,     /* gpio7 Interrupt */
    STIM0_IRQn                      =   16,     /* stimer0 Interrupt */
    STIM1_IRQn                      =   17,     /* stimer0 Interrupt */
    STIM2_IRQn                      =   18,     /* stimer0 Interrupt */
    STIM3_IRQn                      =   19,     /* stimer0 Interrupt */
    PAD_IRQn                        =   20,     /* pad Interrupt */

}
IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK801 Processor and Core Peripherals  ------- */
#define __CK803_REV               0x0000U   /* Core revision r0p0 */
#define __MGU_PRESENT             0         /* MGU present or not */
#define __NVIC_PRIO_BITS          2         /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0         /* Set to 1 if different SysTick Config is used */

#include "core_ck802.h"                     /* Processor and core peripherals */
#include "stdint.h"

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */
#if 0

/* ================================================================================ */
/* ============== Universal Asyncronous Receiver / Transmitter (UART) ============= */
/* ================================================================================ */
typedef struct {
    union {
        __IM uint32_t RBR;           /* Offset: 0x000 (R/ )  Receive buffer register */
        __OM uint32_t THR;           /* Offset: 0x000 ( /W)  Transmission hold register */
        __IOM uint32_t DLL;          /* Offset: 0x000 (R/W)  Clock frequency division low section register */
    };
    union {
        __IOM uint32_t DLH;          /* Offset: 0x004 (R/W)  Clock frequency division high section register */
        __IOM uint32_t IER;          /* Offset: 0x004 (R/W)  Interrupt enable register */
    };
    __IM uint32_t IIR;             /* Offset: 0x008 (R/ )  Interrupt indicia register */
    __IOM uint32_t LCR;            /* Offset: 0x00C (R/W)  Transmission control register */
    __IOM uint32_t MCR;            /* Offset: 0x010 (R/W)  Modem control register */
    __IM uint32_t LSR;             /* Offset: 0x014 (R/ )  Transmission state register */
    __IM uint32_t MSR;             /* Offset: 0x018 (R/ )  Modem state register */
    uint32_t RESERVED1[24];
    __IM uint32_t USR;             /* Offset: 0x07c (R/ )  UART state register */
} CSKY_UART_TypeDef;


/* ================================================================================ */
/* ==============            Inter-Integrated Circuit (IIC)           ============= */
/* ================================================================================ */
typedef struct {
    __IOM uint32_t IC_CON;                    /* Offset: 0x000 (R/W)  Receive buffer register */
    __IOM uint32_t IC_TAR;                    /* Offset: 0x004 (R/W)  Transmission hold register */
    __IOM uint32_t IC_SAR;                    /* Offset: 0x008 (R/W)  Clock frequency division low section register */
    __IOM uint32_t IC_HS_MADDR;               /* Offset: 0x00c (R/W)  Clock frequency division high section register */
    __IOM uint32_t IC_DATA_CMD;               /* Offset: 0x010 (R/W)  Interrupt enable register */
    __IOM uint32_t IC_SS_SCL_HCNT;            /* Offset: 0x014 (R/W)  Interrupt indicia register */
    __IOM uint32_t IC_SS_SCL_LCNT;            /* Offset: 0x018 (R/W)  Transmission control register */
    __IOM uint32_t IC_FS_SCL_HCNT;            /* Offset: 0x01c (R/W)  Modem control register */
    __IOM uint32_t IC_FS_SCL_LCNT;            /* Offset: 0x020 (R/W)  Transmission state register */
    __IOM uint32_t IC_HS_SCL_HCNT;            /* Offset: 0x024 (R/W)  Transmission state register */
    __IOM uint32_t IC_HS_SCL_LCNT;            /* Offset: 0x028 (R/W)  Transmission state register */
    __IOM uint32_t IC_INTR_STAT;              /* Offset: 0x02c (R)  Transmission state register */
    __IOM uint32_t IC_INTR_MASK;              /* Offset: 0x030 (R/W)  Transmission state register */
    __IOM uint32_t IC_RAW_INTR_STAT;          /* Offset: 0x034 (R)  Transmission state register */
    __IOM uint32_t IC_RX_TL;                  /* Offset: 0x038 (R/W)  Transmission state register */
    __IOM uint32_t IC_TX_TL;                  /* Offset: 0x03c (R/W)  Transmission state register */
    __IOM uint32_t IC_CLR_INTR;               /* Offset: 0x040 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_RX_UNDER;           /* Offset: 0x044 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_RX_OVER;            /* Offset: 0x048 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_TX_OVER;            /* Offset: 0x04c (R)  Transmission state register */
    __IOM uint32_t IC_CLR_RD_REQ;             /* Offset: 0x050 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_TX_ABRT;            /* Offset: 0x054 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_RX_DONE;            /* Offset: 0x058 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_ACTIVITY;           /* Offset: 0x05c (R)  Transmission state register */
    __IOM uint32_t IC_CLR_STOP_DET;           /* Offset: 0x060 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_START_DET;          /* Offset: 0x064 (R)  Transmission state register */
    __IOM uint32_t IC_CLR_GEN_CALL;           /* Offset: 0x068 (R)  Transmission state register */
    __IOM uint32_t IC_ENABLE;                 /* Offset: 0x06c (R/W)  Transmission state register */
    __IOM uint32_t IC_STATUS;                 /* Offset: 0x070 (R)  Transmission state register */
    __IOM uint32_t IC_TXFLR;                  /* Offset: 0x074 (R)  Transmission state register */
    __IOM uint32_t IC_RXFLR;                  /* Offset: 0x078 (R)  Transmission state register */
    uint32_t RESERVED;                        /* Offset: 0x014 (R/ )  Transmission state register */
    __IOM uint32_t IC_TX_ABRT_SOURCE;         /* Offset: 0x080 (R/W)  Transmission state register */
    __IOM uint32_t IC_SAR1;                   /* Offset: 0x084 (R/W)  Transmission state register */
    __IOM uint32_t IC_DMA_CR;                 /* Offset: 0x088 (R/W)  Transmission state register */
    __IOM uint32_t IC_DMA_TDLR;               /* Offset: 0x08c (R/W)  Transmission state register */
    __IOM uint32_t IC_DMA_RDLR;               /* Offset: 0x090 (R/W)  Transmission state register */
    __IOM uint32_t IC_SAR2;                   /* Offset: 0x094 (R/W)  Transmission state register */
    __IOM uint32_t IC_SAR3;                   /* Offset: 0x098 (R/W)  Transmission state register */
    __IOM uint32_t IC_MULTI_SLAVE;            /* Offset: 0x09c (R/W)  Transmission state register */
    __IOM uint32_t IC_GEN_CALL_EN;            /* Offset: 0x0a0 (R/W)  Transmission state register */

} CSKY_IIC_TypeDef;


/* ================================================================================ */
/* ==============                          TIMER                      ============= */
/* ================================================================================ */
typedef struct {
    __IOM uint32_t TxLoadCount;               /* Offset: 0x000 (R/W)  Receive buffer register */
    __IOM uint32_t TxCurrentValue;            /* Offset: 0x004 (R)  Transmission hold register */
    __IOM uint32_t TxControl;                 /* Offset: 0x008 (R/W)  Clock frequency division low section register */
    __IOM uint32_t TxEOI;                     /* Offset: 0x00c (R)  Clock frequency division high section register */
    __IOM uint32_t TxIntStatus;               /* Offset: 0x010 (R)  Interrupt enable register */

} CSKY_TIMER_TypeDef;

/* ================================================================================ */
/* ==============                    TIMER  Control                   ============= */
/* ================================================================================ */
typedef struct {
    __IOM uint32_t TimersIntStatus;            /* Offset: 0x000 (R)  Interrupt indicia register */
    __IOM uint32_t TimerEOI;                   /* Offset: 0x004 (R)  Transmission control register */
    __IOM uint32_t TimerRawIntStatus;          /* Offset: 0x008 (R)  Modem control register */

} CSKY_TIMER_Control_TypeDef;


/* ================================================================================ */
/* ==============                        GPIO                         ============= */
/* ================================================================================ */
typedef struct {
    __IOM uint32_t SWPORT_DR;                 /* Offset: 0x000 (R/W)  Interrupt indicia register */
    __IOM uint32_t SWPORT_DDR;                /* Offset: 0x004 (R/W)  Interrupt indicia register */
    __IOM uint32_t PORT_CTL;                  /* Offset: 0x008 (R/W)  Interrupt indicia register */
} CKStruct_GPIO, *PCKStruct_GPIO;

typedef struct {
    __IOM uint32_t SHA_CON;                     /* Offset: 0x000 (R/W)  Control register */
    __IOM uint32_t SHA_INTSTATE;                /* Offset: 0x004 (R/W)  Instatus register */
    __IOM uint32_t SHA_H0L;                     /* Offset: 0x008 (R/W)  H0L register */
    __IOM uint32_t SHA_H1L;                     /* Offset: 0x00c (R/W)  H1L register */
    __IOM uint32_t SHA_H2L;                     /* Offset: 0x010 (R/W)  H2L register */
    __IOM uint32_t SHA_H3L;                     /* Offset: 0x014 (R/W)  H3L register */
    __IOM uint32_t SHA_H4L;                     /* Offset: 0x018 (R/W)  H4L register */
    __IOM uint32_t SHA_H5L;                     /* Offset: 0x01c (R/W)  H5L register */
    __IOM uint32_t SHA_H6L;                     /* Offset: 0x020 (R/W)  H6L register */
    __IOM uint32_t SHA_H7L;                     /* Offset: 0x024 (R/W)  H7L register */
    __IOM uint32_t SHA_H0H;                     /* Offset: 0x028 (R/W)  H0H register */
    __IOM uint32_t SHA_H1H;                     /* Offset: 0x02c (R/W)  H1H register */
    __IOM uint32_t SHA_H2H;                     /* Offset: 0x030 (R/W)  H2H register */
    __IOM uint32_t SHA_H3H;                     /* Offset: 0x034 (R/W)  H3H register */
    __IOM uint32_t SHA_H4H;                     /* Offset: 0x038 (R/W)  H4H register */
    __IOM uint32_t SHA_H5H;                     /* Offset: 0x03c (R/W)  H5H register */
    __IOM uint32_t SHA_H6H;                     /* Offset: 0x040 (R/W)  H6H register */
    __IOM uint32_t SHA_H7H;                     /* Offset: 0x044 (R/W)  H7H register */
    __IOM uint32_t SHA_DATA1;                   /* Offset: 0x048 (R/W)  DATA1 register */
    uint32_t REV[15];
    __IOM uint32_t SHA_DATA2;                   /* Offset: 0x088 (R/W)  DATA2 register */
} CSKY_SHA_TypeDef;


#endif

#define CONFIG_TIMER_NUM 4

#define CONFIG_USART_NUM 1

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
/* --------------------------  CPU FPGA memory map  ------------------------------- */
#define CSKY_SRAM_BASE              (0x20000000UL)

#define CSKY_UART_BASE              (0x40015000UL)
#define CSKY_PMU_BASE               (0x40016000UL)
#define CSKY_TIMER0_BASE            (0x40011000UL)
#define CSKY_TIMER1_BASE            (0x40011014UL)
#define CSKY_TIMER2_BASE            (0x40011028UL)
#define CSKY_TIMER3_BASE            (0x4001103cUL)
#define CSKY_TIMER_CONTROL_BASE     (0x400110a0UL)
#define CSKY_CLK_GEN_BASE           (0x40017000UL)
#define CSKY_STIMER0_BASE           (0x40018000UL)
#define CSKY_STIMER1_BASE           (0x40018014UL)
#define CSKY_STIMER2_BASE           (0x40018028UL)
#define CSKY_STIMER3_BASE           (0x4001803cUL)
#define CSKY_STIMER_CONTROL_BASE    (0x400110a0UL)

#define CONFIG_GPIO_NUM             8
#define CONFIG_GPIO_PIN_NUM         8
#define CSKY_GPIOA_BASE             (0x40019000UL)
#define CSKY_GPIOA_CONTROL_BASE     (0x40019030UL)
#define CSKY_SMPU_BASE              (0x4001a000UL)

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
#define CSKY_UART                  ((   CSKY_UART_TypeDef *)    CSKY_UART_BASE)

#ifdef CONFIG_HAVE_VIC
#define  ATTRIBUTE_ISR __attribute__((isr))
#else
#define  ATTRIBUTE_ISR
#endif

#ifdef __cplusplus
}
#endif

#endif  /* SOC_H */
