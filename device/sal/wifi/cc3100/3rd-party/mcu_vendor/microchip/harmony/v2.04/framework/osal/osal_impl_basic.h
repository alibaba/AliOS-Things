/*******************************************************************************
  Operating System Abstraction Layer Basic Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    osal_impl_basic.h

  Summary:
    Header file for the OSAL Basic implementation.

  Description:
    This file defines the additions or variations to the OSAL base implementation.
 Where it is logical or possible to implement an OSAL function in a simple form
 without an RTOS being present then the function has been defined here and
 implemented either here as an inline or #define. Longer functions that are part
 of the basic implementation may also be found in the file osal.c
 The best way to consider this file is detailing any deviations from the osal.h
 definitions OR as the complete implementation of those functions when pretending
 to support BASIC operations.
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

#ifndef _OSAL_IMPL_BASIC_H
#define _OSAL_IMPL_BASIC_H

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
#include "system/int/sys_int.h"


typedef uint8_t                     OSAL_SEM_HANDLE_TYPE;
typedef uint8_t                     OSAL_MUTEX_HANDLE_TYPE;
typedef uint32_t                    OSAL_CRITSECT_DATA_TYPE;
#define OSAL_WAIT_FOREVER           (uint16_t) 0xFFFF

#define OSAL_SEM_DECLARE(semID)         uint8_t    semID
#define OSAL_MUTEX_DECLARE(mutexID)     uint8_t    mutexID

// *****************************************************************************
/* Macro: OSAL_ASSERT
 */

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
static __inline__ OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE* semID, OSAL_SEM_TYPE type, uint8_t maxCount, uint8_t initialCount);
static __inline__ OSAL_RESULT OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE* semID);
static __inline__ OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE* semID, uint16_t waitMS);
static __inline__ OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE* semID);
static __inline__ OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE* semID);
static __inline__ uint8_t OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE* semID);

static __inline__ OSAL_CRITSECT_DATA_TYPE OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity);
static __inline__ void OSAL_CRIT_Leave(OSAL_CRIT_TYPE severity, OSAL_CRITSECT_DATA_TYPE status);

static __inline__ OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE* mutexID);
static __inline__ OSAL_RESULT OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE* mutexID);
static __inline__ OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE* mutexID, uint16_t waitMS);
static __inline__ OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE* mutexID);

void* OSAL_Malloc(size_t size);
void OSAL_Free(void* pData);

OSAL_RESULT OSAL_Initialize();

static __inline__ const char* OSAL_Name(void);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines Group Defintions
// *****************************************************************************
// *****************************************************************************

/* Critical Section group */
// *****************************************************************************
/* Function: OSAL_CRITSECT_DATA_TYPE OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity)
 */
static OSAL_CRITSECT_DATA_TYPE OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity)
{
   if(severity == OSAL_CRIT_TYPE_LOW)
      return (0);
   /*if priority is set to HIGH the user wants interrupts disabled*/
   return (SYS_INT_StatusGetAndDisable());
}

// *****************************************************************************
/* Function: void OSAL_CRIT_Leave(OSAL_CRIT_TYPE severity, OSAL_CRITSECT_DATA_TYPE status)
 */
static void OSAL_CRIT_Leave(OSAL_CRIT_TYPE severity, OSAL_CRITSECT_DATA_TYPE status)
{
   if(severity == OSAL_CRIT_TYPE_LOW)
      return;
   /*if priority is set to HIGH the user wants interrupts re-enabled to the state 
   they were before disabling.*/
   SYS_INT_StatusRestore(status);   
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE semID, OSAL_SEM_TYPE type,
                                uint8_t maxCount, uint8_t initialCount)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE* semID, OSAL_SEM_TYPE type,
                                uint8_t maxCount, uint8_t initialCount)
{
   OSAL_CRITSECT_DATA_TYPE IntState;
   
   IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
 
   if (type == OSAL_SEM_TYPE_COUNTING)
      *semID = initialCount;
   else
      *semID = 1;
 
   OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH,IntState);
   
   return OSAL_RESULT_TRUE;
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE semID)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE* mutexID)
{
   return (OSAL_RESULT_TRUE);
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE semID, uint16_t waitMS)
 */
static  OSAL_RESULT __attribute__((always_inline)) OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE* semID, uint16_t waitMS)
{
   OSAL_CRITSECT_DATA_TYPE IntState;
   
   IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
       
   if (*semID > 0)
   {
      (*semID)--;
      OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH,IntState);
      
      return OSAL_RESULT_TRUE;
   }
   
   OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH,IntState);
      
   return OSAL_RESULT_FALSE;
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE semID)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE* semID)
{
   OSAL_CRITSECT_DATA_TYPE IntState;
   
   IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
   (*semID)++;
   OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH,IntState);
   
   return OSAL_RESULT_TRUE;
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE semID)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE* semID)
{
    (*semID)++;
    return OSAL_RESULT_TRUE;
}

// *****************************************************************************
/* Function: uint8_t OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE semID)
 */
static uint8_t __attribute__((always_inline)) OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE* semID)
{
    return *semID;
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE mutexID)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE* mutexID)
{
   *mutexID = 1;
   return OSAL_RESULT_TRUE;
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE mutexID)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE* mutexID)
{
   return (OSAL_RESULT_TRUE);
   
}
// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE mutexID, uint16_t waitMS)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE* mutexID, uint16_t waitMS)
{
    if (*mutexID == 1)
    {
        *mutexID = 0;
        return OSAL_RESULT_TRUE;
    }
    return OSAL_RESULT_FALSE;
}

// *****************************************************************************
/* Function: OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE mutexID)
 */
static OSAL_RESULT __attribute__((always_inline)) OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE* mutexID)
{
    *mutexID = 1;
    return OSAL_RESULT_TRUE;
}

// Miscellaneous functions
// *****************************************************************************
/* Function: void* OSAL_Malloc(size_t size)
 */
#define OSAL_Malloc(size)                               (malloc(size))

// *****************************************************************************
/* Function: void OSAL_Free(void* pData)
 */
#define OSAL_Free(pData)                                (free(pData))

// Initialization and Diagnostics
// *****************************************************************************
/* Function: OSAL_RESULT OSAL_Initialize()
 */
#define OSAL_Initialize()


// *****************************************************************************
/* Function: const char* OSAL_Name()
 */
static const char* __attribute__((always_inline)) OSAL_Name(void)
{
   return((const char*) "BASIC");
}


#ifdef __cplusplus
}
#endif

#endif // _OSAL_IMPL_BASIC_H

/*******************************************************************************
 End of File
 */




