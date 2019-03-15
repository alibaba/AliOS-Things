/**
  * @file  hal_wakeup.h
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

#ifndef _DRIVER_CHIP_HAL_WAKEUP_H_
#define _DRIVER_CHIP_HAL_WAKEUP_H_

#ifdef __CONFIG_ARCH_APP_CORE
#include "sys/io.h"
#include "driver/chip/hal_clock.h"
#include "driver/chip/hal_gpio.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ignore the second set if the second set time is longer than the
 * first time.
 */
//#define WAKEUP_TIMER_CHECK_TIME 1

/** @brief the minimum time of wakeup timer support, based on ms */
#define WAKEUP_TIMER_MIN_TIME   (10)

/**
 * @brief wakeup events.
 * @note WKIO0~9 should define from BIT(0) to BIT(9).
 */
#ifdef __CONFIG_ARCH_APP_CORE
#define PM_WAKEUP_SRC_WKIO0     BIT(0 )                 /* 0x00000001 */
#define PM_WAKEUP_SRC_WKIO1     BIT(1 )                 /* 0x00000002 */
#define PM_WAKEUP_SRC_WKIO2     BIT(2 )                 /* 0x00000004 */
#define PM_WAKEUP_SRC_WKIO3     BIT(3 )                 /* 0x00000008 */
#define PM_WAKEUP_SRC_WKIO4     BIT(4 )                 /* 0x00000010 */
#define PM_WAKEUP_SRC_WKIO5     BIT(5 )                 /* 0x00000020 */
#define PM_WAKEUP_SRC_WKIO6     BIT(6 )                 /* 0x00000040 */
#define PM_WAKEUP_SRC_WKIO7     BIT(7 )                 /* 0x00000080 */
#define PM_WAKEUP_SRC_WKIO8     BIT(8 )                 /* 0x00000100 */
#define PM_WAKEUP_SRC_WKIO9     BIT(9 )                 /* 0x00000200 */
#endif
#define PM_WAKEUP_SRC_WKTIMER   BIT(10)                 /* 0x00000400 */
#define PM_WAKEUP_SRC_WKSEV     BIT(11)                 /* 0x00000800 */
#define PM_WAKEUP_SRC_NETCPU    (PM_WAKEUP_SRC_WKSEV)   /* 0x00000800 */
#define PM_WAKEUP_SRC_DEVICES   BIT(12)                 /* 0x00001000 */

/**
 * @brief Get last wakeup event.
 * retval  Events defined in PM_WAKEUP_SRC_XX.
 */
extern uint32_t HAL_Wakeup_GetEvent(void);

#ifdef __CONFIG_ARCH_APP_CORE

/**
 * @brief WakeIo to Gpio.
 * All wakeup io is GPIOA, so not return port info.
 * retval  GPIO Pin info.
 */
extern GPIO_Pin WakeIo_To_Gpio(uint32_t wkup_io);

/**
 * @brief Set IO hold.
 * @note Set all IO hold before poweroff to prevent IO output low level voltage.
 * @param hold_io:
 *        @arg hold_io-> IO hold mask.
 * retval  0 if success or other if failed.
 */
extern int32_t HAL_Wakeup_SetIOHold(uint32_t hold_io);

/**
 * @brief Set wakeup IO enable and mode.
 * @note This func will not change IO function until suspend. The IO will be
 *        setted to interupt and wakeup io mode in suspend and disabled after
 *        resume, so reinit IO if you want this IO used as other function.
 *        This IO will always used as wakeup IO until cleaned by HAL_Wakeup_ClrIO.
 *        This IO should set to EINT mode before suspend.
 * @param pn:
 *        @arg pn-> 0~9.
 * @param mode:
 *        @arg mode-> 0:negative edge, 1:positive edge.
 * @param pull:
 *	  @arg pull-> 0:no pull, 1:pull up, 2: pull down.
 * retval  None.
 */
extern void HAL_Wakeup_SetIO(uint32_t pn, uint32_t mode, uint32_t pull);

/**
 * @brief Clear wakeup IO enable.
 * @param pn:
 *        @arg pn-> 0~9.
 * retval  None.
 */
extern void HAL_Wakeup_ClrIO(uint32_t pn);
#endif

/**
 * @brief Set wakeup timer.
 * @note This will config wakeup timer counting immediately, and this will be
 *        used only once. The wakeup timer will be disabled when time out no
 *        matter it wakeup system or not. Wakeup timer should be setted
 *        everytime if you want wake up system from suspend.
 * @param count_32k:
 *        @arg count_32k-> counter to wakeup system based on 32k counter, from
 *             WAKEUP_TIMER_MIN_TIME*32(WAKEUP_TIMER_MIN_TIME mS) to
 *             2147483647(671088S, about 186.4h).
 * retval  0 if success or other if failed.
 */
extern int32_t HAL_Wakeup_SetTimer(uint32_t count_32k);

/**
 * @brief Set wakeup timer based on ms.
 * @param ms:
 *        @arg ms-> counter to wakeup system based on ms.
 * retval  0 if success or other if failed.
 */
#define HAL_Wakeup_SetTimer_mS(ms) \
	HAL_Wakeup_SetTimer((uint32_t)((uint64_t)(ms) * HAL_GetLFClock() / 1000))

/**
 * @brief Set wakeup timer based on second.
 * @param sec:
 *        @arg sec seconds to wakeup system.
 * retval  0 if success or other if failed.
 */
#define HAL_Wakeup_SetTimer_Sec(sec) \
	HAL_Wakeup_SetTimer((sec) * HAL_GetLFClock())

/**
 * @brief Config and enable wakeup io.
 * retval  0 if success or other if failed.
 */
extern int32_t HAL_Wakeup_SetSrc(uint32_t en_irq);

/** @brief Disable wakeup io. */
extern void HAL_Wakeup_ClrSrc(uint32_t en_irq);

/**
 * @brief Read wakeup io value.
 */
extern uint32_t HAL_Wakeup_ReadIO(void);

/**
 * @brief Read wakeup timer pending status.
 */
extern uint32_t HAL_Wakeup_ReadTimerPending(void);

/**
 * @brief Check wakeup io mode, EINT mode has expected before suspend.
 * retval  1 if success or 0 if failed.
 */
extern uint32_t HAL_Wakeup_CheckIOMode(void);

/** @brief Init wakeup IO and Timer as disable mode. */
extern void HAL_Wakeup_Init(void);

/** @brief Deinit wakeup IO and Timer. */
extern void HAL_Wakeup_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_WAKEUP_H_ */
