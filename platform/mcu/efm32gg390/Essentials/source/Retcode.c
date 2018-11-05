/*----------------------------------------------------------------------------*/
/*
 *  Copyright (C) Bosch Connected Devices and Solutions.
 *  All Rights Reserved. Confidential.
 *
 *  Distribution only to people who need to know this information in
 *  order to do their job.(Need-to-know principle).
 *  Distribution to persons outside the company, only if these persons
 *  signed a non-disclosure agreement.
 *  Electronic transmission, e.g. via electronic mail, must be made in
 *  encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 *  @file
 *
 * @brief The program retcode module implements support functionalities
 *  for the generic return code to be used across all the packages
 *  and components.
 *
 **/

#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_RETCODE

/* own header files */
#include "BCDS_Retcode.h"
#include "BCDS_Assert.h"

/* system header files */

/* additional headers */

/* constant definitions ***************************************************** */

/* function prototypes ****************************************************** */

static bool IsRetcodeModuleInitialized = false;

/* local variables ********************************************************** */

static Retcode_ErrorHandlingFunc_T errorHandlingFunc;

/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

/* global functions ********************************************************* */

/* The description of the function is available in header file */
Retcode_T Retcode_Initialize(Retcode_ErrorHandlingFunc_T func)
{
    Retcode_T Rc;

    if(IsRetcodeModuleInitialized)
    {
        /* Already initialized. */
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
    }

    if(NULL == func)
    {
        Rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        errorHandlingFunc = func;
        IsRetcodeModuleInitialized = true;
        Rc = RETCODE_OK;
    }
    return Rc;
}

/* The description of the function is available in header file */
void Retcode_RaiseError(Retcode_T error)
{
    /* If it asserts here, Retcode_initialize() is not done */
    //assert(errorHandlingFunc);

	//errorHandlingFunc(error, false);
}

/* The description of the function is available in header file */
void Retcode_RaiseErrorFromIsr(Retcode_T error)
{
    /* If it asserts here, Retcode_initialize() is not done */
    //assert(errorHandlingFunc);

    /* The flag which indicates the error is from ISR is set here. */
    //errorHandlingFunc(error, true);
}

/* Deprecated function. See header file for more information */
Retcode_T Retcode_initialize(Retcode_ErrorHandlingFunc_T func)
{
    return (Retcode_Initialize(func));
}

/* Deprecated function. See header file for more information */
void Retcode_raiseError(Retcode_T error)
{
    Retcode_RaiseError(error);
}

/* Deprecated function. See header file for more information */
void Retcode_raiseErrorFromIsr(Retcode_T error)
{
    Retcode_RaiseErrorFromIsr(error);
}

/** ************************************************************************* */
