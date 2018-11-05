/*******************************************************************************
  Operating System Abstraction Layer for OpenRTOS
  
  Company:
    Microchip Technology Inc.

  File Name:
    osal_openrtos.h

  Summary:
    OSAL OpenRTOS implementation interface file

  Description:
    Interface file to allow OpenRTOS to be used with the OSAL
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

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _OSAL_OPENRTOS_H
#define _OSAL_OPENRTOS_H

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
  
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/* declare default data type handles. Any RTOS port must define it's own copy of these */
typedef SemaphoreHandle_t              OSAL_SEM_HANDLE_TYPE;
typedef SemaphoreHandle_t              OSAL_MUTEX_HANDLE_TYPE;
typedef BaseType_t                     OSAL_CRITSECT_DATA_TYPE;
#define OSAL_WAIT_FOREVER               (uint16_t)0xFFFF

#define OSAL_SEM_DECLARE(semID)         OSAL_SEM_HANDLE_TYPE   semID
#define OSAL_MUTEX_DECLARE(mutexID)     OSAL_MUTEX_HANDLE_TYPE mutexID

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


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines Definitions
// *****************************************************************************
// *****************************************************************************
/* These function definitions help map OSAL function calls into specific
   OpenRTOS calls or OSAL translation layer functions
   Each OSAL should fully implement the functions listed in osal.h so only
   deviations from that interface are required here.
 */

#define OSAL_Malloc(size)   pvPortMalloc(size)

#define OSAL_Free(pData)    vPortFree(pData)

#ifdef __cplusplus
}
#endif

#endif // _OSAL_OPENRTOS_H

/*******************************************************************************
 End of File
*/
