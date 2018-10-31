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
 * @brief This is API to support signaling between concurrent execution contexts,
 * i.e. tasks and ISRs.
 *
 * HAL provides a basic implementation of the signaling between concurrent
 * execution contexts which is based on a conditional variable and a waiting
 * loop.
 *
 * If an RTOS is used, then hooks should be set to provide a proper signaling
 * functions based on RTOS mechanisms such as semaphores.
 *
 * @file BCDS_HAL_Signal.h
 *
 */

#ifndef BCDS_HAL_SIGNAL_H_
#define BCDS_HAL_SIGNAL_H_


#include "BCDS_Basics.h"
#include "BCDS_Assert.h"


enum HAL_Signal_Basic_Instance_E
{
		HAL_SIGNAL_NOTINIT 	= 0xFFFFFFFF,
		HAL_SIGNAL_FULL 	= 1,
		HAL_SIGNAL_EMPTY 	= 0
};

struct _HAL_SignalHandle_S
{
	volatile uint32_t _Basic;
	void *   _OsHandle;
};

typedef struct _HAL_SignalHandle_S HAL_SignalHandle_T;


/** A macro for the initialization of singal handles. */
#define HAL_SIGNAL_HANDLE_INIT	{HAL_SIGNAL_NOTINIT, NULL}


/* Function data type for to handle different os Functions by setting a new Function
 */
typedef Retcode_T (*HAL_Signal_WaitHook_T)( HAL_SignalHandle_T * handle, uint32_t timeoutInMs);


/** Function data type for to handle different os Functions by
 * setting a new Function
 */
typedef Retcode_T (*HAL_Signal_NotifyHook_T)(HAL_SignalHandle_T * handle);


/**
 * @brief a function to set RTOS hooks
 *
 * If an RTOS is used, then hooks should be set to provide a proper signaling
 * functions based on RTOS mechanisms such as semaphores.
 *
 * @param[in] waitHook: the hook for the wait function
 *
 * @param[in] notifyHook: the hook for the notify function
 *
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when any one of the param has been NULL
 *
 * @note a new wait Function Hook must implement a initialization or the used resource at first call
 */
Retcode_T HAL_Signal_SetHooks(HAL_Signal_WaitHook_T waitHook, HAL_Signal_NotifyHook_T notifyHook);


/**
 * @brief Wait for the signal in handle Context
 *
 * The call blocks until the signal is reached or the timeout is expired.
 *
 * @param[in] handle : Handle to a signal context
 *
 * @param[in] timeoutInMs : timeout in milliseconds, 0 -> No Timeout/wait for ever ,
 * Value > 0 timeout in ms
 *
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when HAL_Signal_SetHooks() is not initialized.
 *
 * @note   The Usage of wait in ISR Context is not Supported !
 */
Retcode_T HAL_Signal_Wait(HAL_SignalHandle_T * handle, uint32_t timeoutInMs);


/**
 * @brief Send a signal to handle context
 *
 * @param[in] handle : Handle to a Signal context
 *
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when HAL_Signal_SetHooks() is not initialized.
 */
Retcode_T HAL_Signal_Notify(HAL_SignalHandle_T * handle);

#endif /* BCDS_HAL_SIGNAL_H_ */
/** @} */
