/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_SPINLOCK_H
#define YUNOS_BSP_SPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
	HWSPINLOCK_REG  = 0,
	HWSPINLOCK_DEV,
	HWSPINLOCK_NR	
}hwspinlock_id;

typedef enum 
{
	HWSPINLOCK_DEV_ADC = 0,/*adc*/
	HWSPINLOCK_DEV_PMU = 1,/*pmu*/
	HWSPINLOCK_DEV_EFC0 = 2,/*ftl*/
	HWSPINLOCK_DEV_EFC1 = 3,/*efc*/
	HWSPINLOCK_DEV_NR = 16
} hwspinlock_dev_id;

/**
* This function is used for driver try spinlock
* @param[in]  dev_id  soft spin lock id
* @return       the operation status, success is OK, others is error
*/
int hwspinlock_dev_trylock(hwspinlock_dev_id dev_id);

/**
* This function is used for set soft spin lock
* @param[in]  dev_id  soft spin lock id
* @return void
*/
void hwspinlock_dev_lock(hwspinlock_dev_id dev_id);

/**
* This function is used for release soft spin lock
* @param[in]  dev_id  soft spin lock id
* @return void
*/
 void hwspinlock_dev_unlock(hwspinlock_dev_id dev_id);

/**
* This function is used for set reg spin lock
* @param[in]   void
* @return 	  void
*/
void hwspinlock_reg_lock(void);

/**
* This function is used for release reg spin lock
* @param[in]   void
* @return 	  void
*/
void hwspinlock_reg_unlock(void);

#ifdef __cplusplus
}
#endif

#endif /* YUNOS_BSP_SPINLOCK_H */

