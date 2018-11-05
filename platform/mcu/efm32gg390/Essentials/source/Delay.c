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
 * @file Delay.c
 *
 * @brief Implementation of a Delay functions for non Os
 *
 */

#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_DELAY

/* additional interface header files */
#include "BCDS_Basics.h"

/* own header files */
#include "BCDS_HAL_Delay.h"
#include "Cmsis.h"

#include "BCDS_BSP_Board.h"
#ifdef __GNUC__

static Retcode_T NoOs_Delay_Ms(uint32_t timeInMs);



static HAL_Delay_MsHook_T _Delay_MsHook = 	NoOs_Delay_Ms;




Retcode_T HAL_Delay_SetMsHook( HAL_Delay_MsHook_T msHook){
	if(NULL != msHook)
	{
		_Delay_MsHook = msHook;
		return RETCODE_OK;
	}
	else
	{
		return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER);
	}
}


Retcode_T HAL_Delay_WaitUs(uint32_t timeInUs)
{
	BSP_Board_DelayUs(timeInUs);
	return RETCODE_OK;
}


Retcode_T HAL_Delay_WaitMs(uint32_t timeInMs){
	if(NULL != _Delay_MsHook)
	{
		return(_Delay_MsHook(timeInMs));
	}
	return RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER);
}

static Retcode_T NoOs_Delay_Ms(uint32_t timeInMs){
	Retcode_T rc = RETCODE_OK;
	if(0 == timeInMs ){
		return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
	}

	do{
		rc = HAL_Delay_WaitUs(1000);
	}
	while(timeInMs--);
	return rc;
}


#endif
