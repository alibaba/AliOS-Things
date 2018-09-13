/**
  * @file  hal_spinlock.c
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

#include <stdbool.h>
#include "hal_base.h"
#include "driver/chip/hal_spinlock.h"
#include "kernel/os/os.h"

#define DBG_SPIN	0
#define SPIN_DBG(fmt, arg...)	\
	HAL_LOG(HAL_DBG_ON && DBG_SPIN, "[HAL SPIN] "fmt, ##arg)

static uint32_t SPIN_Num()
{
	__IO uint32_t *reg = SPINLOCK_SYSTATUS_REG;

	uint32_t num = HAL_GET_BIT(*reg, SPINLOCK_NUM);
	num = num >> 28;

	if (num > 1)
		num = (num - 1) * 2;

	return num * 32;
}

static uint32_t SPIN_Status()
{
	__IO uint32_t *reg = SPINLOCK_STATUS_REG;
	return *reg;
}

static uint32_t SPIN_ReadLock(int lock)
{
	__IO uint32_t *reg = SPINLOCKN_LOCK_REG(lock);
	return HAL_GET_BIT(*reg, SPINLOCK_TAKEN);
}

static void SPIN_ClearLock(int lock)
{
	__IO uint32_t *reg = SPINLOCKN_LOCK_REG(lock);
	HAL_CLR_BIT(*reg, SPINLOCK_TAKEN);
}

static int SPIN_AllocationLock()
{
	int i = 0;
	uint32_t spin_sta = SPIN_Status();
	SPIN_DBG("%s %d, spin_sta %d\n", __func__, __LINE__, spin_sta);
	uint32_t spin_num = SPIN_Num();
	SPIN_DBG("%s %d, SPIN_Num %u\n", __func__, __LINE__, spin_num);

	while (i < spin_num) {
		if (! (spin_sta & HAL_BIT(i)))
			return i;
		i++;
	}

	return -1;
}

static int SPIN_LockCheck(SPIN_Lock_t *lock)
{
	int ret = 0;

	if (!lock)
		ret = -1;

	if (lock->lock > SPIN_Num())
		ret = -1;

	return ret;
}

static int SPIN_UnLockCheck(SPIN_Lock_t *lock)
{
	int ret = 0;

	if (!lock)
		ret = -1;

	if (lock->lock > SPIN_Num())
		ret = -1;

	if (!(SPIN_Status()& HAL_BIT(lock->lock))) //it is unlock
		ret = -1;

	return ret;
}


/**
  * @brief Init the spinlock module.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPIN_Init()
{
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_SPINLOCK);
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_SPINLOCK);
	HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_SPINLOCK);
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_SPINLOCK);

	return HAL_OK;
}

/**
  * @brief Deinit the spinlock module.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPIN_Deinit()
{
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_SPINLOCK);

	return HAL_OK;
}

/**
  * @brief Request a spinlock.
  * @note Allocation a spinlock resources.
  * @param lock: used to save the spinlock number;
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPIN_RequestLock(SPIN_Lock_t *lock)
{
	if (!lock) {
		SPIN_DBG("%s %d invalid param\n", __func__, __LINE__);
		return HAL_ERROR;
	}

	int spin_lock = SPIN_AllocationLock();
	lock->lock = spin_lock;

	if (spin_lock == -1)
		return HAL_ERROR;

	return HAL_OK;
}

/**
  * @brief Taken lock.
  * @note If the lock alreadytaken, it will be block until lock is released.
  *           Else the lock will be taken.
  * @param lock: The spinlock.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPIN_Lock(SPIN_Lock_t *lock)
{
	if (SPIN_LockCheck(lock) != 0)
		return HAL_INVALID;

	while (SPIN_ReadLock(lock->lock));

	return HAL_OK;
}

/**
  * @brief Try to taken lock.
  * @note If the lock lock alreadytaken, it will be return busy.
  *           Else the lcok will be taken.
  * @param  lock: The spinlock.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPIN_TryLock(SPIN_Lock_t *lock)
{
	if (SPIN_LockCheck(lock) != 0)
		return HAL_INVALID;

	if (SPIN_ReadLock(lock->lock))
		return HAL_BUSY;

	return HAL_OK;
}

/**
  * @brief Unlock.
  * @param lock: The spinlock.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_SPIN_Unlock(SPIN_Lock_t *lock)
{
	if (SPIN_UnLockCheck(lock) != 0)
		return HAL_INVALID;

	SPIN_ClearLock(lock->lock);

	return HAL_OK;
}
