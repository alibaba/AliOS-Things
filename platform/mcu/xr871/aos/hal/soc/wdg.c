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

#include "hal/soc/soc.h"
#include "driver/chip/hal_wdg.h"

int32_t hal_wdg_init(wdg_dev_t *wdg)
{

        WDG_InitParam param;
	WDG_Timeout timeout_tbl[]={WDG_TIMEOUT_500MS,WDG_TIMEOUT_1SEC,WDG_TIMEOUT_2SEC,WDG_TIMEOUT_3SEC,
		                       WDG_TIMEOUT_4SEC,WDG_TIMEOUT_5SEC,WDG_TIMEOUT_6SEC,WDG_TIMEOUT_6SEC,
		                       WDG_TIMEOUT_8SEC,WDG_TIMEOUT_8SEC,WDG_TIMEOUT_10SEC,WDG_TIMEOUT_10SEC,
		                       WDG_TIMEOUT_12SEC,WDG_TIMEOUT_12SEC,WDG_TIMEOUT_14SEC,WDG_TIMEOUT_14SEC,
		                       WDG_TIMEOUT_16SEC};
	int32_t tbl_size;


	tbl_size = sizeof(timeout_tbl)/sizeof(timeout_tbl[0]);
        int pos = wdg->config.timeout > 500 ? (wdg->config.timeout/1000) : 0;
	if (pos < tbl_size) {
		param.timeout = timeout_tbl[pos];
	}
	else {
		param.timeout = timeout_tbl[tbl_size-1];
	}
	printf("*********hal_wdg_init timeout:%d",param.timeout);
	param.event = WDG_EVT_RESET;

	HAL_WDG_Init(&param);
	HAL_WDG_Start();

	return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
	HAL_WDG_Feed();
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
	HAL_WDG_Stop();

	HAL_WDG_DeInit();

	return 0;
}
