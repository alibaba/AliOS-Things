/**
  * @file  hal_clock.h
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

#ifndef _DRIVER_CHIP_HAL_CLOCK_H_
#define _DRIVER_CHIP_HAL_CLOCK_H_

#include "driver/chip/hal_prcm.h"
#include "driver/chip/hal_ccm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get HF clock, which is the value of external high frequency oscillator
 * @return HF clock in Hz
 */
__STATIC_INLINE uint32_t HAL_GetHFClock(void)
{
	return HAL_PRCM_GetHFClock();
}

/**
 * @brief Get LF clock, which is the value of low frequence oscillator
 * @return LF clock in Hz
 */
__STATIC_INLINE uint32_t HAL_GetLFClock(void)
{
	return SYS_LFCLOCK;
}

/**
 * @brief Get CPU clock
 * @return CPU clock in Hz
 */
__STATIC_INLINE uint32_t HAL_GetCPUClock(void)
{
	return HAL_PRCM_GetCPUAClk();
}

/**
 * @brief Get Device clock
 * @return Device clock in Hz
 */
__STATIC_INLINE uint32_t HAL_GetDevClock(void)
{
	return HAL_PRCM_GetDevClock();
}

/**
 * @brief Get AHB1 clock
 * @return AHB1 clock in Hz
 */
__STATIC_INLINE uint32_t HAL_GetAHB1Clock(void)
{
	return HAL_CCM_BusGetAHB1Clock();
}

/**
 * @brief Get AHB2 clock
 * @return AHB2 clock in Hz
 */
__STATIC_INLINE uint32_t HAL_GetAHB2Clock(void)
{
	return HAL_CCM_BusGetAHB2Clock();
}

/**
 * @brief Get APB clock
 * @return APB clock in Hz
 */
__STATIC_INLINE uint32_t HAL_GetAPBClock(void)
{
	return HAL_CCM_BusGetAPBClock();
}

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_CLOCK_H_ */
