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

#ifndef __PM_PORT_H
#define __PM_PORT_H

#include "sys/interrupt.h"
#include "kernel/os/os_time.h"
#include "pm/pm.h"

#include "driver/chip/hal_rtc.h"

#ifdef CONFIG_PM

#define ct_assert(e) extern char (*ct_assert(void)) [sizeof(char[1 - 2*!(e)])]

#if defined(__CONFIG_CHIP_XR871)
#define ktime_t uint64_t
#define ktime_get() (HAL_RTC_GetFreeRunTime() / 1000)
#define ktime_to_msecs(t) (t)
#else
#define ktime_t uint32_t
#define ktime_get() OS_GetTicks()
#define ktime_to_msecs(t) OS_MSecsToJiffies(t)
#endif

#define arch_suspend_disable_irqs __disable_irq
#define arch_suspend_enable_irqs __enable_irq

extern unsigned int nvic_int_mask[];

extern void debug_jtag_init(void);
extern void debug_jtag_deinit(void);

int platform_prepare(enum suspend_state_t state);
extern void platform_wake(enum suspend_state_t state);

#endif

#endif /* __PM_PORT_H */
