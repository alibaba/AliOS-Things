/**
  * @file  hal_wdg.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_WDG_H_
#define _DRIVER_CHIP_HAL_WDG_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Watchdog register block structure
 */
typedef struct {
    __IO uint32_t IRQ_EN;       /* offset: 0xA0, Watchdog IRQ Enable Register */
    __IO uint32_t IRQ_STATUS;   /* offset: 0xA4, Watchdog IRQ Status Register */
         uint32_t RESERVED0[2];
    __IO uint32_t CTRL;         /* offset: 0xB0, Watchdog Control Register */
    __IO uint32_t CFG;          /* offset: 0xB4, Watchdog Configuration Register */
    __IO uint32_t MODE;         /* offset: 0xB8, Watchdog Mode Register */
    __IO uint32_t RESET_CTRL;   /* offset: 0xBC, Watchdog Output Control Register */
} WDG_T;

#define WDG ((WDG_T *)(TIMER_BASE + 0xA0))  /* address: 0x400408A0 */

/* WDG->IRQ_EN */
#define WDG_IRQ_EN_BIT      HAL_BIT(0)

/* WDG->IRQ_STATUS */
#define WDG_IRQ_PENDING_BIT HAL_BIT(0)

/* WDG->CFG */
#define WDG_EVT_TYPE_SHIFT  0
#define WDG_EVT_TYPE_MASK   (0x3U << WDG_EVT_TYPE_SHIFT)
typedef enum {
    WDG_EVT_RESET           = (1U << WDG_EVT_TYPE_SHIFT),   /* reset system */
    WDG_EVT_INTERRUPT       = (2U << WDG_EVT_TYPE_SHIFT)    /* trigger interrupt */
} WDG_EventType;

/* WDG->MODE */
#define WDG_TIMEOUT_SHIFT   4
#define WDG_TIMEOUT_MASK    (0xF << WDG_TIMEOUT_SHIFT)
typedef enum {
    WDG_TIMEOUT_500MS       = (0U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_1SEC        = (1U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_2SEC        = (2U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_3SEC        = (3U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_4SEC        = (4U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_5SEC        = (5U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_6SEC        = (6U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_8SEC        = (7U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_10SEC       = (8U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_12SEC       = (9U  << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_14SEC       = (10U << WDG_TIMEOUT_SHIFT),
    WDG_TIMEOUT_16SEC       = (11U << WDG_TIMEOUT_SHIFT)
} WDG_Timeout;

#define WDG_EN_BIT          HAL_BIT(0)

/* WDG->RESET_CTRL */
#define WDG_RESET_CYCLE_SHIFT   0
#define WDG_RESET_CYCLE_MASK    (0x1F << WDG_RESET_CYCLE_SHIFT)
#define WDG_DEFAULT_RESET_CYCLE 0xA

/******************************************************************************/

/** @brief Type define of watchdog IRQ callback function */
typedef void (*WDG_IRQCallback) (void *arg);

/**
 * @brief Watchdog initialization parameters
 */
typedef struct {
    WDG_EventType       event;      /* Watchdog trigger event type */
    WDG_Timeout         timeout;    /* Interval to trigger event after last feed */
    uint8_t             resetCycle; /* Reset signal cycles, for WDG_EVT_RESET only.
                                       Set to WDG_DEFAULT_RESET_CYCLE generally, its range is [0, 31] */
    WDG_IRQCallback     callback;   /* Watchdog IRQ callback fucntion, for WDG_EVT_INTERRUPT only */
    void               *arg;        /* Argument of Watchdog IRQ callback fucntion, for WDG_EVT_INTERRUPT only */
} WDG_InitParam;

HAL_Status HAL_WDG_Init(const WDG_InitParam *param);
HAL_Status HAL_WDG_DeInit(void);

void HAL_WDG_Feed(void);
void HAL_WDG_Start(void);
void HAL_WDG_Stop(void);
void HAL_WDG_Reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_WDG_H_ */
