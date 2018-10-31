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
 * @file Function.c
 *
 * @brief Diverse useful Functions e.g. Cmsis Functions for upper Modules
 *
 */

#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_HAL_MODULE_ID_CRITICALSECTION


/* additional interface header files */
#include "BCDS_HAL.h"
#include "Cmsis.h"

#ifdef __GNUC__
#ifdef ti_cc26xx
#ifndef __ARM_PROFILE_M__
#define __ARM_PROFILE_M__ 1
#endif /* #ifndef __ARM_PROFILE_M__ */
/* The __get_IPSR is defined in the below header which is supported for __ARM_PROFILE_M__ only.
 * If __ARM_PROFILE_M__ is not enabled, then TI will need a different implementation. */
#include "intrinsics.h"
#endif /* ti_cc26xx */

bool HAL_IsInISR(void){
	if( 0 != __get_IPSR() ){
		return true;
	}
	return false;
}

#endif
