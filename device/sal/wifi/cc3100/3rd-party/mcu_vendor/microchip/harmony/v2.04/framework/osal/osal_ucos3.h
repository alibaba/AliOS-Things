/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                     (c) Copyright 2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   Microchip OSAL 
*
* File    : OSAL_uCOS3.H
* By      : JJL
* Version : V3.03.02
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#ifndef _OSAL_MICRIUM_H
#define _OSAL_MICRIUM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    INCLUDE FILES
------------------------------------------------------------------------------------------------------------------------
*/

#include <xc.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os.h"
/*
------------------------------------------------------------------------------------------------------------------------
*                                                     DATA TYPES
------------------------------------------------------------------------------------------------------------------------
*/

typedef CPU_SR                          OSAL_CRITSECT_DATA_TYPE;
typedef OS_SEM                          OSAL_SEM_HANDLE_TYPE;
typedef OS_MUTEX                        OSAL_MUTEX_HANDLE_TYPE;

#define OSAL_WAIT_FOREVER               (OS_TICK)0xFFFF
#define OSAL_SEM_DECLARE(semID)         OS_SEM     semID
#define OSAL_MUTEX_DECLARE(mutexID)     OS_MUTEX   mutexID

#define OSAL_ASSERT(test, message)      test
// *****************************************************************************
/* OSAL Result type

  Summary:
    Enumerated type representing the general return value from OSAL functions.

  Description:
    This enum represents possible return types from OSAL functions.

  Remarks:
    These enum values are the possible return values from OSAL functions
    where a standard success/fail type response is required. The majority
    of OSAL functions will return this type with a few exceptions.
*/

typedef enum OSAL_SEM_TYPE
{
    OSAL_SEM_TYPE_BINARY,
    OSAL_SEM_TYPE_COUNTING
} OSAL_SEM_TYPE;

typedef enum OSAL_CRIT_TYPE
{
    OSAL_CRIT_TYPE_LOW,
    OSAL_CRIT_TYPE_HIGH
} OSAL_CRIT_TYPE;

typedef enum OSAL_RESULT
{
    OSAL_RESULT_NOT_IMPLEMENTED = -1,
    OSAL_RESULT_FALSE = 0,
    OSAL_RESULT_TRUE = 1
} OSAL_RESULT;

// *****************************************************************************
// *****************************************************************************
// Section: Section: Interface Routines Group Declarations
// *****************************************************************************
// *****************************************************************************
OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE* semID, OSAL_SEM_TYPE type, uint8_t maxCount, uint8_t initialCount);
OSAL_RESULT OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE* semID);
OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE* semID, uint16_t waitMS);
OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE* semID);
OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE* semID);
uint8_t OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE* semID);

OSAL_CRITSECT_DATA_TYPE __attribute__((nomips16,nomicromips)) OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity);
void __attribute__((nomips16,nomicromips)) OSAL_CRIT_Leave(OSAL_CRIT_TYPE severity, OSAL_CRITSECT_DATA_TYPE status);

OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE* mutexID);
OSAL_RESULT OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE* mutexID);
OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE* mutexID, uint16_t waitMS);
OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE* mutexID);

void* OSAL_Malloc(size_t size);
void OSAL_Free(void* pData);

OSAL_RESULT OSAL_Initialize();

__inline__ const char* OSAL_Name(void);


#ifdef __cplusplus
}
#endif

#endif // _OSAL_MICRIUM_H
