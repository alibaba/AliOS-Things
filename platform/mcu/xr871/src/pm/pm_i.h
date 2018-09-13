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

#ifndef __PM_H
#define __PM_H

#include "pm/pm.h"

#ifdef CONFIG_PM

#define CONFIG_PM_DEBUG 1

#ifdef CONFIG_PM_DEBUG
#define PM_DBG(format, args...)  do {printf(format, ##args);} while (0)
#define PM_LOGD(format, args...) do {printf("PM: "format, ##args);} while (0)
#define PM_LOGN(format, args...) do {printf("PM: "format, ##args);} while (0)
#define PM_LOGW(format, args...) do {printf("PM: WAR "format, ##args);} while (0)
#define PM_LOGE(format, args...) do {printf("PM: ERR "format, ##args);} while (0)
#define PM_LOGA(format, args...) do {printf("PM: "format, ##args);} while (0)
#else
#define PM_DBG(x...)
#define PM_LOGD(x...)
#define PM_LOGN(x...)
#define PM_LOGW(x...)
#define PM_LOGE(x...)
#define PM_LOGA(x...)
#endif
#define PM_BUG_ON(v) do {if(v) {printf("PM: BUG at %s:%d!\n", __func__, __LINE__); while (1);}} while (0)
#define PM_WARN_ON(v) do {if(v) {printf("PM: WARN at %s:%d!\n", __func__, __LINE__);}} while (0)

#ifdef CONFIG_PM_DEBUG
#define MAX_DEV_NAME 40
struct suspend_stats {
	int     success;
	int     fail;
	int     failed_suspend_noirq;
	int     failed_suspend;
	int     failed_resume;
	int     failed_resume_noirq;
	char    failed_devs[MAX_DEV_NAME];
	int     last_failed_step;
};
#endif

#define PM_EARLY_SUSPEND           (1<<16)
#define PM_SUSPEND_BEGIN           (2<<16)
#define PM_SUSPEND_DEVICES         (3<<16)
#define PM_SUSPEND_ENTER           (4<<16)
#define PM_RESUME_SYSTEM           (6<<16)
#define PM_RESUME_DEVICES          (7<<16)
#define PM_RESUME_END              (8<<16)
#define PM_RESUME_COMPLETE         (9<<16)
#define PM_RESUME_ERROR            (0x0e<<16)
#define PM_SUSPEND_FAIL_FLAG       (0xFFFF)
#define PM_FIRST_BOOT_FLAG         (0x0000)

#define PM_POWEROFF                (0x0a<<16)

#define PM_SYNC_MAGIC              (0x7FF2DCCD)

/**
 * @brief Data constructs for implementation in assembly.
 * @note systick saved by timer subsys.
 */
struct arm_CMX_core_regs {
	unsigned int msp;
	unsigned int psp;
	unsigned int psr;
	unsigned int primask;
	unsigned int faultmask;
	unsigned int basepri;
	unsigned int control;
	unsigned int reg12[16]; /* used only for debug */
};

/**
 * @brief Callbacks for managing platform dependent system sleep states.
 *
 * @begin: Initialise a transition to given system sleep state.
 *      @begin() is executed right prior to suspending devices. The information
 *      conveyed to the platform code by @begin() should be disregarded by it as
 *	soon as @end() is executed. If @begin() fails (ie. returns nonzero),
 *	@prepare(), @enter() and @finish() will not be called.
 *
 * @prepare: Prepare the platform for entering the system sleep state indicated.
 *      @prepare() is called right after devices have been suspended (ie. the
 *      appropriate .suspend() method has been executed for each device) and
 *      before device drivers' late suspend callbacks are executed. It returns
 *      0 on success or a negative error code otherwise, in which case the
 *      system cannot enter the desired sleep state.
 *
 * @enter: Enter the system sleep state indicated.
 *      It returns 0 on success or a negative error code otherwise, in which
 *      case the system cannot enter the desired sleep state.
 *
 * @wake: Called when the system has just left a sleep state, right after
 *      the CPU have been enabled and before device drivers' early resume
 *      callbacks are executed.
 *      It is always called after @enter().
 *
 * @end: Called after resuming devices, to indicate to the platform that the
 *      system has returned to the working state or the transition to the sleep
 *      state has been aborted.
 *	Platforms implementing @begin() should also provide a @end() which
 *	cleans up transitions aborted before @enter().
 */
struct platform_suspend_ops {
	int (*begin)(enum suspend_state_t state);
	int (*prepare)(enum suspend_state_t state);
	void (*enter)(enum suspend_state_t state);
	void (*wake)(enum suspend_state_t state);
	void (*end)(enum suspend_state_t state);
};

#define to_device(ptr_module, idx)			\
	__containerof(ptr_module, struct soc_device, node[idx])
#define get_device(d) d->ref++
#define put_device(d) d->ref--

extern void __cpu_sleep(enum suspend_state_t state);
extern void __cpu_suspend(enum suspend_state_t state);
#endif

#endif
