/**
  * @file  hal_timer.c
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

#include "driver/chip/hal_timer.h"
#include "hal_base.h"

typedef enum {
	TIMER_STATE_INVALID = 0,
	TIMER_STATE_READY	= 1,
	TIMER_STATE_RUNNING	= 2
} TIMER_State;

typedef struct {
	TIMER_State         state;
	TIMER_IRQCallback	callback;
	void               *arg;
} TIMER_Private;

static TIMER_Private gTimerPrivate[TIMER_NUM];

#define TIMER_ASSERT_ID(timerID)	HAL_ASSERT_PARAM((timerID) < TIMER_NUM)

static void TIMER_EnableIRQ(TIMER_ID timerID)
{
	TIMER_ASSERT_ID(timerID);

	HAL_SET_BIT(TIMER->IRQ_EN, HAL_BIT(timerID));
}

static void TIMER_DisableIRQ(TIMER_ID timerID)
{
	TIMER_ASSERT_ID(timerID);

	HAL_CLR_BIT(TIMER->IRQ_EN, HAL_BIT(timerID));
}

static int TIMER_IsPendingIRQ(TIMER_ID timerID)
{
	TIMER_ASSERT_ID(timerID);

	return HAL_GET_BIT_VAL(TIMER->IRQ_STATUS, timerID, 1);
}

static void TIMER_ClearPendingIRQ(TIMER_ID timerID)
{
	TIMER_ASSERT_ID(timerID);

	HAL_SET_BIT(TIMER->IRQ_STATUS, HAL_BIT(timerID));
}

static void TIMER_IRQHandler(TIMER_ID timerID)
{
	if (TIMER_IsPendingIRQ(timerID)) {
		TIMER_ClearPendingIRQ(timerID);
		if (gTimerPrivate[timerID].callback) {
			gTimerPrivate[timerID].callback(gTimerPrivate[timerID].arg);
		}
	}
}

void TIMER0_IRQHandler(void)
{
	TIMER_IRQHandler(TIMER0_ID);
}

void TIMER1_IRQHandler(void)
{
	TIMER_IRQHandler(TIMER1_ID);
}

/**
 * @brief Initialize the timer according to the specified parameters
 * @param[in] timerID ID of the specified timer
 * @param[in] param Pointer to TIMER_InitParam structure
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_TIMER_Init(TIMER_ID timerID, const TIMER_InitParam *param)
{
	TIMER_Private *timerPriv;
	IRQn_Type IRQn;
	unsigned long flags;

	TIMER_ASSERT_ID(timerID);

	if (param->period == 0) {
		return HAL_INVALID;
	}

	flags = HAL_EnterCriticalSection();

	timerPriv = &gTimerPrivate[timerID];
	if (timerPriv->state == TIMER_STATE_INVALID)
		timerPriv->state = TIMER_STATE_READY;
	else
		timerPriv = NULL;

	HAL_ExitCriticalSection(flags);

	if (timerPriv == NULL) {
		HAL_WRN("timer %d already inited\n", timerID);
		return HAL_BUSY;
	}

	/* enable clock */

	/* release reset */

	/* init parameters */
#if 1
	/* NB: Clear other bits other than param->cfg, it seems no side effect */
	TIMER->TIMERx[timerID].CTRL = param->cfg;
#else
	HAL_MODIFY_REG(TIMER->TIMERx[timerID].CTRL,
	               TIMER_MODE_MASK | TIMER_CLK_SRC_MASK | TIMER_CLK_PRESCALER_MASK,
	               param->cfg);
#endif
	TIMER->TIMERx[timerID].LOAD_VAL = param->period;

	if (param->isEnableIRQ) {
		IRQn = (timerID == TIMER0_ID) ? TIMER0_IRQn : TIMER1_IRQn;
#if 0
		NVIC_IRQHandler IRQHandler = (timerID == TIMER0_ID ? TIMER0_IRQHandler : TIMER1_IRQHandler);
		HAL_NVIC_SetIRQHandler(IRQn, IRQHandler);
#endif
		timerPriv->callback = param->callback;
		timerPriv->arg = param->arg;
		if (TIMER_IsPendingIRQ(timerID)) {
			TIMER_ClearPendingIRQ(timerID);
		}
		TIMER_EnableIRQ(timerID);
		HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
		HAL_NVIC_EnableIRQ(IRQn);
	}

	return HAL_OK;
}

/**
 * @brief DeInitialize the specified timer
 * @param[in] timerID ID of the specified timer
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_TIMER_DeInit(TIMER_ID timerID)
{
	TIMER_Private *timerPriv;
	unsigned long flags;

	TIMER_ASSERT_ID(timerID);

	timerPriv = &gTimerPrivate[timerID];

	/* stop timer */
	HAL_TIMER_Stop(timerID);

	/* disable IRQ */
	HAL_NVIC_DisableIRQ(timerID == TIMER0_ID ? TIMER0_IRQn : TIMER1_IRQn);
	TIMER_DisableIRQ(timerID);
	if (TIMER_IsPendingIRQ(timerID)) {
		TIMER_ClearPendingIRQ(timerID);
	}
	timerPriv->callback = NULL;
	timerPriv->arg = NULL;

	/* force reset */

	/* disable clock */

	flags = HAL_EnterCriticalSection();
	timerPriv->state = TIMER_STATE_INVALID;
	HAL_ExitCriticalSection(flags);

	return HAL_OK;
}

/**
 * @brief Reload the timer's current value and start to count down
 * @param[in] timerID ID of the specified timer
 * @return None
 */
void HAL_TIMER_Start(TIMER_ID timerID)
{
	TIMER_ASSERT_ID(timerID);

	HAL_SET_BIT(TIMER->TIMERx[timerID].CTRL, TIMER_RELOAD_BIT | TIMER_START_BIT);
}

/**
 * @brief Stop the timer's counting, the current value of timer is kept
 * @param[in] timerID ID of the specified timer
 * @return None
 */
void HAL_TIMER_Stop(TIMER_ID timerID)
{
	TIMER_ASSERT_ID(timerID);

	HAL_CLR_BIT(TIMER->TIMERx[timerID].CTRL, TIMER_START_BIT);
}

/**
 * @brief Continue the timer's counting
 *
 * The timer's current value is not changed, and start to count down
 *
 * @param[in] timerID ID of the specified timer
 * @return None
 */
void HAL_TIMER_Continue(TIMER_ID timerID)
{
	TIMER_ASSERT_ID(timerID);

	HAL_SET_BIT(TIMER->TIMERx[timerID].CTRL, TIMER_START_BIT);
}
