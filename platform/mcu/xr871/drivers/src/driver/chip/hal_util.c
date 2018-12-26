/**
  * @file  hal_util.c
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

#include <stdint.h>
#if defined(__CONFIG_CHIP_XR871)
#include "driver/chip/hal_rtc.h"
#else
#include "driver/chip/hal_prcm.h"
#endif

/**
 * @brief Provide accurate delay (in microsecond), and its accuracy is about
 *        32 microseconds.
 * @param[in] us Time (in microsecond) to delay
 * @return None
 *
 * @note Avoid using this function to delay for a long time (longer than 10ms),
 *       because its execution will occupy a lot of CPU resource.
 */
void HAL_UDelay(uint32_t us)
{
#if defined(__CONFIG_CHIP_XR871)
	uint64_t expire;

	expire = us + HAL_RTC_GetFreeRunTime();
	while (expire > HAL_RTC_GetFreeRunTime())
		;
#else /* __CONFIG_CHIP_XR871 */
#define CPUCLK_PER_LOOP 6
	uint32_t i;
	uint32_t loop = HAL_PRCM_GetCPUAClk() / 1000000 * us / CPUCLK_PER_LOOP;

	for (i = 0; i < loop; ++i) {
		__asm("nop");
	}
#undef CPUCLK_PER_LOOP
#endif /* __CONFIG_CHIP_XR871 */
}
