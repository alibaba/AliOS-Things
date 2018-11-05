/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH. 
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @{
 * @brief API to support Mutual Exclusion to protect a code segments in HAL and
 * BSPs.
 *
 * The original state of the mutex is unlocked.
 *
 * @note This mutex must not be used in the ISR context.
 *
 * By default, the mutex functions are empty because the mutex is only needed
 * if the RTOS is active. In this case, RTOS hooks should be set using the
 * function HAL_Lock_SetHooks() to provide a proper mutex implementation.
 * @file BCDS_HAL_Mutex.h
 */

#ifndef BCDS_HAL_MUTEX_H_
#define BCDS_HAL_MUTEX_H_


#include "BCDS_Basics.h"
#include "BCDS_Assert.h"

#define HAL_MUTEX_UNINITIALIZED		NULL

typedef void * HAL_Mutex_T;

/* Function data type for to handle different os Functions by setting a new Function*/
typedef Retcode_T (*HAL_Mutex_Hook_T)(HAL_Mutex_T *mutex);


/**
 * @brief a function to set RTOS hooks
 *
 * If an RTOS is used, then hooks should be set to provide proper mutex
 * functions based on RTOS mechanisms such as mutexSemaphores.
 *
 * @param[in] lockHook: the hook for the lock function
 *
 * @param[in] unlockHook: the hook for the unlock function
 *
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when any one of the param has been NULL
 *
 * @note a new wait Function Hook must implement a initialization or the used resource at first call
 */

Retcode_T HAL_Mutex_SetHooks(HAL_Mutex_Hook_T lockHook, HAL_Mutex_Hook_T unlockHook);


/**
 * @brief Lock the HAL mutex
 *
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when HAL_Mutex_SetHooks() is not initialized.
 */
Retcode_T HAL_Mutex_Lock(HAL_Mutex_T *mutex);


/**
 * @brief Unlock the HAL mutex
 *
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when HAL_Mutex_SetHooks() is not initialized.
 */
Retcode_T HAL_Mutex_Unlock(HAL_Mutex_T *mutex);

#endif /* BCDS_HAL_MUTEX_H_ */
/** @} */
