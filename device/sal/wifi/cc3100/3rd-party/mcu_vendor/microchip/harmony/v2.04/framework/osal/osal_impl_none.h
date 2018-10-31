/*******************************************************************************
    OSAL none implementation header file

  Company:
    Microchip Technology Inc.

  File Name:
    osal_impl_none.h

  Summary:
    OSAL none implementation when no OS or RTOS support is required

  Description:
    This file will be included by default if no OSAL or RTOS support is required
    by the application. It defaults all of the OSAL primitives to safe values
    usuable in a non-OS application.
    Do not include this file directly. It will be included by osal.h as required.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef _OSAL_IMPL_NONE_H    // Guards against multiple inclusion
#define _OSAL_IMPL_NONE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t                     OSAL_SEM_HANDLE_TYPE;
typedef uint8_t                     OSAL_MUTEX_HANDLE_TYPE;
typedef uint32_t                    OSAL_CRITSECT_DATA_TYPE;
#define OSAL_WAIT_FOREVER           (uint16_t) 0xFFFF

#define OSAL_SEM_DECLARE(semID)         
#define OSAL_MUTEX_DECLARE(mutexID)     

// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Macro: OSAL_ASSERT
 */
#define OSAL_ASSERT(test, message)      

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

static __inline__ const char* OSAL_Name(void);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines Group Defintions
// *****************************************************************************

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE semID, OSAL_SEM_TYPE type,
                                uint8_t maxCount, uint8_t initialCount)
 */
#define OSAL_SEM_Create(semID, type, maxCount,initialCount)     (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Delete (OSAL_SEM_HANDLE_TYPE semID)
 */
#define OSAL_SEM_Delete(semID)              (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE semID, uint16_t waitMS)
 */
#define OSAL_SEM_Pend(semID, waitMS)        (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE semID)
 */
#define OSAL_SEM_Post(semID)                (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE semID)
 */
#define OSAL_SEM_PostISR(semID)             (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: uint8_t OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE semID)
 */
#define OSAL_SEM_GetCount(semID)            (0)

// *****************************************************************************
/* Function: void OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity)
 */
#define OSAL_CRIT_Enter(severity)           (0)

// *****************************************************************************
/* Function: void OSAL_CRIT_Leave(OSAL_CRIT_TYPE severity)
 */
#define OSAL_CRIT_Leave(severity,status)        

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE mutexID)
 */
#define OSAL_MUTEX_Create(mutexID)          (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE mutexID)
 */
#define OSAL_MUTEX_Delete(mutexID)          (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE mutexID, uint16_t waitMS)
 */
#define OSAL_MUTEX_Lock(mutexID, waitMS)    (OSAL_RESULT_TRUE)

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE mutexID)
 */
#define OSAL_MUTEX_Unlock(mutexID)          (OSAL_RESULT_TRUE)

/* Function: void* OSAL_Malloc(size_t size)
 */
#define OSAL_Malloc(size)                           (malloc(size))

// *****************************************************************************
/* Function: void OSAL_Free(void* pData)
 */
#define OSAL_Free(pData)                            (free(pData))

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_Initialize()
 */
#define OSAL_Initialize()

// *****************************************************************************
/* Function: const char* OSAL_Name()
 */
static const char* __attribute__ ((always_inline)) OSAL_Name(void)
{
   return((const char*) "NONE");
}



#ifdef __cplusplus
}
#endif

#endif // _OSAL_IMPL_NONE_H

/*******************************************************************************
 End of File
 */









