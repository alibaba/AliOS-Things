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
 * @file CriticalSection.c
 *
 * @brief Implementation of a Critical Section for non Os
 *        who will dot interrupted from Any
 *
 * @note
 */
#include "BCDS_Essentials.h"

#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_CRITICALSECTION


/* additional interface header files */
#include "BCDS_Basics.h"



#ifdef __GNUC__

/* own header files */
#include "BCDS_HAL_CriticalSection.h"
#include "Cmsis.h"

#ifdef ti_cc26xx
/* The __enable_irq and __disable_irq are defined in the below header */
#include "intrinsics.h"
#endif /* ti_cc26xx */

static Retcode_T NoOs_CriticalSection_Enter(uint32_t * count);
static Retcode_T NoOs_CriticalSection_Leave(uint32_t * count);

static uint32_t _CriticalSection_Count=0;


static HAL_CriticalSection_Hook_T _CriticalSectionEnterHook = 	NoOs_CriticalSection_Enter;
static HAL_CriticalSection_Hook_T _CriticalSectionLeaveHook = 	NoOs_CriticalSection_Leave;



Retcode_T HAL_CriticalSection_SetHooks( HAL_CriticalSection_Hook_T enterHook,HAL_CriticalSection_Hook_T leaveHook)
{
	if(NULL != enterHook && NULL != leaveHook)
	{
		_CriticalSectionEnterHook = enterHook;
		_CriticalSectionLeaveHook = leaveHook;
		return RETCODE_OK;
	}
	else
	{
		return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER);
	}
}


Retcode_T HAL_CriticalSection_Enter(uint32_t * count ){

	if(NULL != _CriticalSectionEnterHook)
		{
			return( _CriticalSectionEnterHook(count));
		}


	return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER);
}


Retcode_T HAL_CriticalSection_Leave(uint32_t * count ){

	if(NULL != _CriticalSectionLeaveHook)
		{
			return( _CriticalSectionLeaveHook(count));
		}


	return RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_NULL_POINTER);
}



static Retcode_T NoOs_CriticalSection_Enter(uint32_t * count)
{
    __disable_irq();
    if (_CriticalSection_Count < UINT32_MAX)
    {
        _CriticalSection_Count++;
        *count = _CriticalSection_Count;
        return RETCODE_OK;

    }
    else
    {
    	*count = UINT32_MAX;
    	return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_OUT_OF_RESOURCES);
    }



}


static Retcode_T NoOs_CriticalSection_Leave(uint32_t * count)
{

    uint32_t lockLevel;

    if (_CriticalSection_Count > 0)
    {
        _CriticalSection_Count--;
        lockLevel = _CriticalSection_Count;
        if (lockLevel == 0)
        {
            __enable_irq();
        }

        *count = lockLevel;
        return RETCODE_OK;
    }
    else
    {
    	*count = UINT32_MAX;
    	 return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_OUT_OF_RESOURCES);
    }

}


#endif
