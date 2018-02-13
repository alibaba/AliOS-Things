/**
  * @file  hal_spinlock.h
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

#ifndef _DRIVER_CHIP_HAL_SPINLOCK_H_
#define _DRIVER_CHIP_HAL_SPINLOCK_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPINLOCK_SYSTATUS_REG	(__IO uint32_t*) (SPINLOCK_BASE + 0X0)
#define SPINLOCK_STATUS_REG		(__IO uint32_t*) (SPINLOCK_BASE + 0X10)
#define SPINLOCKN_LOCK_REG(n)	(__IO uint32_t*) (SPINLOCK_BASE + 0X100 + (n) * 4)

#define SPINLOCK_NUM (0X03 << 28)
#define	IU0	HAL_BIT(8)
#define SPINLOCK_TAKEN HAL_BIT(0)


/**
  * @brief spinlock info.
  */
typedef struct SPIN_Lock_t {
	int lock;
} SPIN_Lock_t;

HAL_Status HAL_SPIN_Init();
HAL_Status HAL_SPIN_Deinit();
HAL_Status HAL_SPIN_RequestLock(SPIN_Lock_t *lock);
HAL_Status HAL_SPIN_Lock(SPIN_Lock_t *lock);
HAL_Status HAL_SPIN_TryLock(SPIN_Lock_t *lock);
HAL_Status HAL_SPIN_Unlock(SPIN_Lock_t *lock);

#ifdef __cplusplus
}
#endif

#endif //_DRIVER_CHIP_HAL_SPINLOCK_H_