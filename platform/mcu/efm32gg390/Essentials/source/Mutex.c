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
 * @file Mutex.c
 *
 * @brief Implementation of a Mutex for non Os
 *
 */
#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_LOCK


/* own header files */
#include "BCDS_HAL_Mutex.h"

/* additional interface header files */
#include "BCDS_Basics.h"
#include "Cmsis.h"



static Retcode_T NoOs_Mutex_Lock(HAL_Mutex_T *mutex);
static Retcode_T NoOs_Mutex_Unlock(HAL_Mutex_T *mutex);


static HAL_Mutex_Hook_T _HAL_Mutex_LockFunction 	= 	NoOs_Mutex_Lock;
static HAL_Mutex_Hook_T _HAL_Mutex_UnlockFunction	= 	NoOs_Mutex_Unlock;



Retcode_T HAL_Mutex_SetHooks(HAL_Mutex_Hook_T lockHook, HAL_Mutex_Hook_T unlockHook)
{
	if( NULL != lockHook && NULL != unlockHook)
	{

		_HAL_Mutex_LockFunction 	= 	lockHook;
		_HAL_Mutex_UnlockFunction 	= 	unlockHook;

		return RETCODE_OK;
	}
	else
	{
		return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER);
	}

}


Retcode_T HAL_Mutex_Lock(HAL_Mutex_T *mutex)
{
	if(NULL != _HAL_Mutex_LockFunction)
	{
		return (_HAL_Mutex_LockFunction(mutex));
	}

	return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER);
}


Retcode_T HAL_Mutex_Unlock(HAL_Mutex_T *mutex)
{
	if(NULL != _HAL_Mutex_UnlockFunction)
	{
		return (_HAL_Mutex_UnlockFunction(mutex));
	}

	return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER);
}


static Retcode_T NoOs_Mutex_Lock(HAL_Mutex_T *mutex)
{
	BCDS_UNUSED(mutex);
	return RETCODE_OK;
}

static Retcode_T NoOs_Mutex_Unlock(HAL_Mutex_T *mutex)
{
	BCDS_UNUSED(mutex);
	return RETCODE_OK;
}

