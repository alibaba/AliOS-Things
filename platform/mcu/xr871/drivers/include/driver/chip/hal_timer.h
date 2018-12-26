/**
  * @file  hal_timer.h
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

#ifndef _DRIVER_CHIP_HAL_TIMER_H_
#define _DRIVER_CHIP_HAL_TIMER_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer ID definition
 */
typedef enum {
    TIMER0_ID = 0,
    TIMER1_ID = 1,
    TIMER_NUM = 2
} TIMER_ID;

/**
 * @brief TIMERx register block structure
 */
typedef struct {
    __IO uint32_t CTRL;         /* offset: 0x00, Timer control register */
    __IO uint32_t LOAD_VAL;     /* offset: 0x04, Timer load/interval value register */
    __I  uint32_t CUR_VAL;      /* offset: 0x08, Timer current value register */
         uint32_t RESERVED[1];  /* offset: 0x0C */
} TIMERx_T;

/**
 * @brief TIMER register block structure
 */
typedef struct {
    __IO uint32_t IRQ_EN;            /* offset: 0x00, Timer IRQ enable register */
    __IO uint32_t IRQ_STATUS;        /* offset: 0x04, Timer IRQ status register */
         uint32_t RESERVED[2];       /* offset: 0x08 */
    TIMERx_T      TIMERx[TIMER_NUM]; /* offset: 0x10, TIMERx register block */
} TIMER_T;

#define TIMER ((TIMER_T *)TIMER_BASE) /* address: 0x40040800 */

/*
 * Bit field definition of TIMER->TIMERx[x].CTRL
 */

/* timer mode */
#define TIMER_MODE_SHIFT    7   /* R/W */
#define TIMER_MODE_MASK     (0x1U << TIMER_MODE_SHIFT)
typedef enum {
    TIMER_MODE_REPEAT       = (0U << TIMER_MODE_SHIFT),
    TIMER_MODE_ONCE         = (1U << TIMER_MODE_SHIFT)
} TIMER_Mode;

/* Timer clock prescaler */
#define TIMER_CLK_PRESCALER_SHIFT   4   /* R/W */
#define TIMER_CLK_PRESCALER_MASK    (0x7U << TIMER_CLK_PRESCALER_SHIFT)
typedef enum {
    TIMER_CLK_PRESCALER_1           = (0U << TIMER_CLK_PRESCALER_SHIFT),
    TIMER_CLK_PRESCALER_2           = (1U << TIMER_CLK_PRESCALER_SHIFT),
    TIMER_CLK_PRESCALER_4           = (2U << TIMER_CLK_PRESCALER_SHIFT),
    TIMER_CLK_PRESCALER_8           = (3U << TIMER_CLK_PRESCALER_SHIFT),
    TIMER_CLK_PRESCALER_16          = (4U << TIMER_CLK_PRESCALER_SHIFT),
    TIMER_CLK_PRESCALER_32          = (5U << TIMER_CLK_PRESCALER_SHIFT),
    TIMER_CLK_PRESCALER_64          = (6U << TIMER_CLK_PRESCALER_SHIFT),
    TIMER_CLK_PRESCALER_128         = (7U << TIMER_CLK_PRESCALER_SHIFT)
} TIMER_ClkPrescaler;

/* Timer clock source */
#define TIMER_CLK_SRC_SHIFT 2   /* R/W */
#define TIMER_CLK_SRC_MASK  (0x3U << TIMER_CLK_SRC_SHIFT)
typedef enum {
    TIMER_CLK_SRC_LFCLK     = (0U << TIMER_CLK_SRC_SHIFT),
    TIMER_CLK_SRC_HFCLK     = (1U << TIMER_CLK_SRC_SHIFT)
} TIMER_ClkSrc;

#define TIMER_RELOAD_BIT    HAL_BIT(1)

#define TIMER_START_BIT     HAL_BIT(0)

/*
 * TIMER->TIMERx[x].LOAD_VAL
 */

/*
 * TIMER->TIMERx[x].CUR_VAL
 */

/******************************************************************************/

/** @brief Type define of Timer IRQ callback function */
typedef void (*TIMER_IRQCallback)(void *arg);

/**
 * @brief Timer initialization parameters
 *
 * @note Timer counts down from period to zero
 *     - counting interval = period / (clkSrc / clkPrescaler)
 */
typedef struct {
    uint32_t            cfg;            /* Created by HAL_TIMER_MakeInitCfg() */
    uint32_t            period;         /* Hardware reload value */

    int8_t              isEnableIRQ;    /* Enable IRQ or not when TIMER counts down to zero */
    TIMER_IRQCallback   callback;       /* TIMER IRQ callback function */
    void               *arg;            /* Argument of TIMER IRQ callback function */
} TIMER_InitParam;

/**
 * @brief Make configuration value for TIMER_InitParam::cfg
 * @param[in] mode Timer working mode
 * @param[in] clkSrc Timer clock source
 * @param[in] clkPrescaler Timer clock prescaler
 * @return configuration value for TIMER_InitParam::cfg
 */
__STATIC_INLINE uint32_t HAL_TIMER_MakeInitCfg(TIMER_Mode mode,
                                               TIMER_ClkSrc clkSrc,
                                               TIMER_ClkPrescaler clkPrescaler)
{
	return (mode | clkSrc | clkPrescaler);
}

/**
 * @brief Get the current counting value of the specified timer
 * @param[in] timerID ID of the specified Timer
 * @return Current counting value of the specified timer
 */
__STATIC_INLINE uint32_t HAL_TIMER_GetCurrentValue(TIMER_ID timerID)
{
	return TIMER->TIMERx[timerID].CUR_VAL;
}

HAL_Status HAL_TIMER_Init(TIMER_ID timerID, const TIMER_InitParam *param);
HAL_Status HAL_TIMER_DeInit(TIMER_ID timerID);

void HAL_TIMER_Start(TIMER_ID timerID);
void HAL_TIMER_Stop(TIMER_ID timerID);

/**
 * @brief Pause the timer's counting, the current value of timer is kept
 * @param[in] timerID ID of the specified timer
 * @return None
 */
__STATIC_INLINE void HAL_TIMER_Pause(TIMER_ID timerID)
{
	HAL_TIMER_Stop(timerID);
}

void HAL_TIMER_Continue(TIMER_ID timerID);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_TIMER_H_ */
