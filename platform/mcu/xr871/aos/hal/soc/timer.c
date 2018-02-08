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

#include <hal/soc/soc.h>

#include "driver/chip/hal_timer.h"

/**
 * @brief init a hardware timer
 * @param tmr timer struct
 * @param period micro seconds for repeat timer trigger
 * @param auto_reoad set to 0, if you just need oneshot timer
 * @param cb callback to be triggered after useconds
 * @ch    timer channel
 * @param arg passed to cb
 * @note  period   auto   auto   auto
 *         *-------|--------|--------|--------|
 */
int32_t hal_timer_init(timer_dev_t *tmr)
{
	HAL_Status status = HAL_ERROR;
	TIMER_InitParam param = {0};
	TIMER_ID timer_id;

	if (tmr->port == TIMER0_ID) {
		timer_id = TIMER0_ID;
	} else if (tmr->port == TIMER0_ID) {
		timer_id = TIMER1_ID;
	} else {
		printf("[timer]: init error, wrong port %d\n", tmr->port);
		return;
	}
	param.arg = tmr->config.arg;
	param.callback = tmr->config.cb;
	param.cfg = HAL_TIMER_MakeInitCfg(TIMER_MODE_REPEAT,
				TIMER_CLK_SRC_HFCLK, TIMER_CLK_PRESCALER_4);
	param.isEnableIRQ = 1;
	param.period = 6000 * tmr->config.period; //each cycle is 6us

	status = HAL_TIMER_Init(timer_id, &param);
	if (status != HAL_OK) {
		printf("timer int error %d\n", status);
	}
}

/**
 * @brief init a hardware timer
 * @param None
 * @retval 0 == success
 *         EIO == failure
 */
int32_t hal_timer_start(timer_dev_t *tmr)
{
	HAL_TIMER_Start(tmr->port);
	return 0;
}

/**
 * @brief stop a hardware timer
 * @param tmr timer struct
 * @param cb callback to be triggered after useconds
 * @param arg passed to cb
 */
void hal_timer_stop(timer_dev_t *tmr)
{
	HAL_TIMER_Stop(tmr->port);
}
