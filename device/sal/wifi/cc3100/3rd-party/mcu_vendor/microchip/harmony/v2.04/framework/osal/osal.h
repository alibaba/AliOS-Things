/*******************************************************************************
  Descriptive File Name: Operating System Abstraction Layer

  Company:
    Microchip Technology Inc.

  File Name:
    osal.h

  Summary:
    Common interface definitions for the Operating System Abstraction Layer (OSAL).

  Description:
    This file defines the common interface to the Operating System
    Abstraction Layer. It defines the common types used by the OSAL
    and defines the function prototypes. Depending upon the OSAL mode,
    a support level specific implementation file is included by this
    file to give the required level of compatibility. The available
    support levels include, OSAL_USE_NONE, OSAL_USE_BASIC, and
    OSAL_USE_RTOS.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _OSAL_H
#define _OSAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************




// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/* Each OSAL type (Full, Basic and none) will have a set of basic types
 * dependent upon the OS being used. This conditional block allows the
 * default set of types to be used or the RTOS specific type
*/
#include "system_config.h"

#include "osal/osal_definitions.h"


// *****************************************************************************
/* OSAL Semaphore Type

  Summary:
    Enumerated type representing the possible types of semaphore.

  Description:
    This enum represents possible semaphore types.

    OSAL_SEM_TYPE_BINARY -      Simple binary type that can be taken once

    OSAL_SEM_TYPE_COUNTING -    Complex type that can be taken set number of
                                times defined at creation time

  Remarks:
    Binary and counting semaphore type.
*/


// *****************************************************************************
/* OSAL Critical Type

  Summary:
    Enumerated type representing the possible types of critical section.

  Description:
    This enum represents possible critical section types.

    OSAL_CRIT_TYPE_LOW -        Low priority critical section, can be formed
                                by locking the scheduler (if supported by RTOS)
    OSAL_CRIT_TYPE_HIGH -       High priority critical section, will be formed
                                by disabling all interrupts.

 Remarks:
    Critical section types
*/


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* OSAL_SEM_Declare(semID)

  Summary:
    Declares an OSAL semaphore.

  Description:
    This function declares a data item of type OSAL_SEM_HANDLE_TYPE.

  Remarks:
    None.
  
  NOTE - put definition of OSAL_SEM_DECLARE macro inside each specific 
         implementation file.  Not all implementation files are using same 
         definition.  The type for each individual RTOS is different, to handle
         this properly in the code, each implementation had to define its own
         declaration.  Leave comment block here as the automated doc generator 
         reads this file to output Harmony OSAL help contents..
 */


// *****************************************************************************
/* Function: 
     OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE* semID, OSAL_SEM_TYPE type,
                                 uint8_t maxCount, uint8_t initialCount)
  Summary:
    Creates an OSAL Semaphore.

  Description:
    This function creates an OSAL binary or counting semaphore. If OSAL_SEM_TYPE_BINARY
    is specified, the maxcount and initialCount values are ignored; otherwise,
    these must contain valid values.

  Precondition:
    Semaphore must have been declared.

  Parameters:
    semID       - Pointer to the Semaphore ID

    type        - OSAL_SEM_TYPE_BINARY, create a binary semaphore

                  OSAL_SEM_TYPE_COUNTING, create a counting semaphore with the
                  specified count values

    maxCount    - Maximum value for a counting semaphore (ignored for a BINARY semaphore)

    initialCount - Starting count value for the semaphore (ignored for a BINARY semaphore)

  Returns:
    - OSAL_RESULT_TRUE    - Semaphore created
    - OSAL_RESULT_FALSE   - Semaphore creation failed
    - semID               - Updated with valid semaphore handle if call was successful

  Example:
    <code>
    OSAL_SEM_Create(&mySemID, OSAL_SEM_TYPE_COUNTING, 10, 5);
    </code>

  Remarks:
    None.
 */
//OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE* semID, OSAL_SEM_TYPE type, uint8_t maxCount, uint8_t initialCount);

// ****************************************************************************
/* Function: 
    OSAL_RESULT OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE* semID)

  Summary:
    Deletes an OSAL Semaphore.

  Description:
    This function deletes an OSAL semaphore.

  Precondition:
    Semaphore must have been created.

  Parameters:
    semID       - Pointer to the Semaphore ID

  Returns:
    - OSAL_RESULT_TRUE    - Semaphore deleted
    - OSAL_RESULT_FALSE   - Semaphore deletion failed

  Example:
    <code>
     OSAL_SEM_Delete(&mySemID);
   </code>

  Remarks:
    None.

 */
//OSAL_RESULT OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE* semID);

/*******************************************************************************
  Function:
     OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE* semID, uint16_t waitMS)

  Summary:
    Waits on a semaphore. Returns true if the semaphore was obtained within the
    time limit.

  Description:
    This function is a blocking function call that waits (i.e., pends) on a semaphore.
    The function will return true if the semaphore has been obtained, or false
    if it was not available or the time limit was exceeded.

  Conditions:
    Semaphore must have been created.

  Input:
    semID  -   Pointer to the Semaphore ID
    waitMS -   Time limit to wait in milliseconds.
                   0                 - do not wait
                   OSAL_WAIT_FOREVER - return only when semaphore is obtained
                   Other values      - timeout delay

  Returns:
    - OSAL_RESULT_TRUE  - Semaphore obtained
    - OSAL_RESULT_FALSE - Semaphore not obtained or timeout occurred

  Example:
    <code>
    if (OSAL_SEM_Pend(&semUARTRX, 50) == OSAL_RESULT_TRUE)
    {
        // character available
        c = DRV_USART_ReadByte(drvID);
        ...
    }
    else
    {
        // character not available, resend prompt
        ...
    }
    </code>

  Remarks:
    None.

    */
//OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE* semID, uint16_t waitMS);

// *****************************************************************************
/* Function: 
     OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE* semID)

  Summary:
     Posts a semaphore or increments a counting semaphore.

  Description:
     This function posts a binary semaphore or increments a counting semaphore.
     The highest priority task currently blocked on the semaphore will be released
     and made ready to run.

  Precondition:
     Semaphore must have been created.

  Parameters:
     semID       - The semID

  Returns:
    - OSAL_RESULT_TRUE    - Semaphore posted
    - OSAL_RESULT_FALSE   - Semaphore not posted

  Example:
    <code>
    OSAL_SEM_Post(&semSignal);
    </code>

  Remarks:
    None.

 */
//OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE* semID);

/**************************************************************************
  Function:
     OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE* semID)

  Summary:
    Posts a semaphore or increments a counting semaphore from within an
    Interrupt Service Routine (ISR).

  Description:
    This function posts a binary semaphore or increments a counting semaphore.
    The highest priority task currently blocked on the semaphore will be released
    and made ready to run. This form of the post function should be used inside
    an ISR.

  Conditions:
    Semaphore must have been created.

  Input:
    semID -  Pointer to the Semaphore ID

  Return:
    - OSAL_RESULT_TRUE - Semaphore posted
    - OSAL_RESULT_FALSE - Semaphore not posted

  Example:
    <code>
     void __ISR(UART_2_VECTOR) _UART2RXHandler()
     {
        char c;

        // read the character
        c = U2RXREG;
        // clear the interrupt flag
        IFS1bits.U2IF = 0;
        // post a semaphore indicating a character has been received
        OSAL_SEM_PostISR(&semSignal);

     }
    </code>

  Remarks:
    This version of the OSAL_SEM_Post function should be used if the
    program is, or may be, operating inside an ISR. The OSAL will take the
    necessary steps to ensure correct operation possibly disabling
    interrupts or entering a critical section. The exact requirements will
    depend upon the particular RTOS being used.

 */
//OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE* semID);

// *****************************************************************************
/* Function: 
    uint8_t OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE* semID)

  Summary:
    Returns the current value of a counting semaphore.

  Description:
    This function returns the current value of a counting semaphore. The value
    returned is assumed to be a single value ranging from 0-255.

  Precondition:
     Semaphore must have been created.

  Parameters:
     semID       - Pointer to the Semaphore ID

  Returns:
    - 0           - Semaphore is unavailable
    - 1-255       - Current value of the counting semaphore

  Example:
    <code>
     uint8_t semCount;

     semCount = OSAL_SEM_GetCount(semUART);

     if (semCount > 0)
     {
        // obtain the semaphore
         if (OSAL_SEM_Pend(&semUART) == OSAL_RESULT_TRUE)
         {
            // perform processing on the comm channel
            ...
         }
     }
     else
     {
        // no comm channels available
        ...
     }
    </code>

  Remarks:
    None.

 */
//uint8_t OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE* semID);


// Critical Section group
// *****************************************************************************
/* Function: 
    OSAL_CRITSECT_DATA_TYPE void OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity)

  Summary:
    Enters a critical section with the specified severity level.

  Description:
     This function enters a critical section of code. It is assumed that the
     sequence of operations bounded by the enter and leave critical section
     operations is treated as one atomic sequence that will not be disturbed.  
     This function should be paired with OSAL_CRIT_Leave().

  Precondition:
    None.

  Parameters:
    severity      - OSAL_CRIT_TYPE_LOW, The RTOS should disable all other running
                    tasks effectively locking the scheduling mechanism.

                    OSAL_CRIT_TYPE_HIGH, The RTOS should disable all possible
                    interrupts sources including the scheduler ensuring that the
                    sequence of code operates without interruption.  The state of 
                    interrupts are returned to the user before they are disabled.

  Returns:
    A data type of OSAL_CRITSECT_DATA_TYPE, this value represents the state of 
    interrupts before entering the critical section.

  Example:
    <code>
     OSAL_CRITSECT_DATA_TYPE IntState;
     // prevent other tasks pre-empting this sequence of code
     IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
     // modify the peripheral
     DRV_USART_Reinitialize( objUSART,  &initData);
     OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, IntState);
    </code>

  Remarks:
    The sequence of operations bounded by the OSAL_CRIT_Enter and OSAL_CRIT_Leave
    form a critical section. The severity level defines whether the RTOS should
    perform task locking or completely disable all interrupts.

 */
//__inline__ OSAL_CRITSECT_DATA_TYPE __attribute__((nomips16)) __attribute__((nomicromips)) OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity);

// *****************************************************************************
/* Function: 
    void OSAL_CRIT_Leave(OSAL_CRIT_TYPE severity, OSAL_CRITSECT_DATA_TYPE status)

  Summary:
     Leaves a critical section with the specified severity level.

  Description:
     This function leaves a critical section of code. It is assumed that the
     sequence of operations bounded by the enter and leave critical section
     operations is treated as one atomic sequence that will not be disturbed.
     The severity should match the severity level used in the corresponding
     OSAL_CRIT_Enter call to ensure that the RTOS carries out the correct action.

  Precondition:
    None.

  Parameters:
    severity      - OSAL_CRIT_TYPE_LOW, The scheduler will be unlocked, if no 
                    other nested calls to OSAL_CRIT_ENTER have been made.

                    OSAL_CRIT_TYPE_HIGH, Interrupts are returned to the state 
                    passed into this function.  The state should of been saved
                    by an earlier call to OSAL_CRIT_Enter.
   
    status        - The value which will be used to set the state of global 
                    interrupts, if OSAL_CRIT_TYPE_HIGH is passed in.
  Returns:
    None.

  Example:
    <code>
     OSAL_CRITSECT_DATA_TYPE IntState;
     // prevent other tasks pre-empting this sequence of code
     intState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
     // modify the peripheral
     DRV_USART_Reinitialize( objUSART,  &initData);
     OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, IntState);
    </code>

  Remarks:
    The sequence of operations bounded by the OSAL_CRIT_Enter and OSAL_CRIT_Leave
    form a critical section. The severity level defines whether the RTOS should
    perform task locking or completely disable all interrupts.

 */
//__inline__ void __attribute__((nomips16,nomicromips)) OSAL_CRIT_Leave(OSAL_CRIT_TYPE severity, OSAL_CRITSECT_DATA_TYPE status);


// *****************************************************************************
/* OSAL_MUTEX_Declare(mutexID)

  Summary:
    Declares an OSAL mutex.

  Description:
    This function declares a data item of type OSAL_MUTEX_HANDLE_TYPE

  Remarks:
    Place the definition of OSAL_MUTEX_Declare macro inside each specific 
    implementation file.  Not all implementation files are using same 
    definition.  The type for each individual RTOS is different, to handle
    this properly in the code, each implementation had to define its own
    declaration.
 */


// *****************************************************************************
/* Function: 
    OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE* mutexID)

  Summary:
    Creates a mutex.

  Description:
    This function creates a mutex, allocating storage if required and placing
    the mutex handle into the passed parameter.

  Precondition:
    Mutex must have been declared.

  Parameters:
    mutexID      - Pointer to the mutex handle

  Returns:
    - OSAL_RESULT_TRUE    - Mutex successfully created
    - OSAL_RESULT_FALSE   - Mutex failed to be created

  Example:
    <code>
    OSAL_MUTEX_HANDLE_TYPE mutexData;

    OSAL_MUTEX_Create(&mutexData);
    ...
     if (OSAL_MUTEX_Lock(&mutexData, 1000) == OSAL_RESULT_TRUE)
     {
        // manipulate the shared data
        ...
     }
    </code>

  Remarks:
    None.

 */
//OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE* mutexID);

// *****************************************************************************
/* Function: 
    OSAL_RESULT OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE* mutexID)

  Summary:
    Deletes a mutex.

  Description:
    This function deletes a mutex and frees associated storage if required.

  Precondition:
    None.

  Parameters:
    mutexID      - Pointer to the mutex handle

  Returns:
    - OSAL_RESULT_TRUE    - Mutex successfully deleted
    - OSAL_RESULT_FALSE   - Mutex failed to be deleted

  Example:
    <code>
    OSAL_MUTEX_Delete(&mutexData);
    </code>

  Remarks:
    None.

 */
//OSAL_RESULT OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE* mutexID);

// *****************************************************************************
/* Function: 
    OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE* mutexID, uint16_t waitMS)

  Summary:
    Locks a mutex.

  Description:
    This function locks a mutex, waiting for the specified timeout. If it cannot
    be obtained or the timeout period elapses 'false' is returned.

  Precondition:
    Mutex must have been created.

  Parameters:
    mutexID      - Pointer to the mutex handle

    waitMS       - Timeout value in milliseconds

                   0, do not wait return immediately

                   OSAL_WAIT_FOREVER, wait until mutex is obtained before returning

                   Other values, Timeout delay

  Returns:
    - OSAL_RESULT_TRUE    - Mutex successfully obtained
    - OSAL_RESULT_FALSE   - Mutex failed to be obtained or timeout occurred

  Example:
    <code>

    ...
     if (OSAL_MUTEX_Lock(&mutexData, 1000) == OSAL_RESULT_TRUE)
     {
        // manipulate the shared data
        ...

        // unlock the mutex
        OSAL_MUTEX_Unlock(&mutexData);
     }
    </code>

  Remarks:
    None.

 */
//OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE* mutexID, uint16_t waitMS);

// *****************************************************************************
/* Function: 
    OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE* mutexID)

  Summary:
    Unlocks a mutex.

  Description:
    This function unlocks a previously obtained mutex.

  Precondition:
    Mutex must have been created.

  Parameters:
    mutexID      - Pointer to the mutex handle

  Returns:
    - OSAL_RESULT_TRUE    - Mutex released
    - OSAL_RESULT_FALSE   - Mutex failed to be released or error occurred

  Example:
    <code>

    ...
    if (OSAL_MUTEX_Lock(&mutexData, 1000) == OSAL_RESULT_TRUE)
    {
        // manipulate the shared data
        ...

        // unlock the mutex
        OSAL_MUTEX_Unlock(&mutexData);
    }
    </code>

  Remarks:
    None.

 */
//OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE* mutexID);

// *****************************************************************************
/* Function: 
    void* OSAL_Malloc(size_t size)

  Summary:
    Allocates memory using the OSAL default allocator.

  Description:
     This function allocates a block of memory from the default allocator from
     the underlying RTOS. If no RTOS is present, it defaults to malloc.
     Many operating systems incorporate their own memory allocation scheme, using
     pools, blocks or by wrapping the standard C library functions in a critical
     section. Since an Harmony application may not know what target OS is being used
     (if any), this function ensures that the correct thread safe memory
     allocator will be used.

  Precondition:
    None.

  Parameters:
    size      - Size of the requested memory block in bytes

  Returns:
     Pointer to the block of allocated memory. NULL is returned if memory could
     not be allocated.

  Example:
    <code>
    // create a working array
    uint8_t* pData;

     pData = OSAL_Malloc(32);
     if (pData != NULL)
     {
        ...
     }
    </code>

  Remarks:
    None.

 */
//void* OSAL_Malloc(size_t size);

// *****************************************************************************
/* Function: 
    void OSAL_Free(void* pData)

  Summary:
    Deallocates a block of memory and return to the default pool.

  Description:
     This function deallocates memory and returns it to the default pool.
     In an RTOS-based application, the memory may have been allocated from
     multiple pools or simply from the heap.
     In non-RTOS applications, this function calls the C standard function
     free.

  Precondition:
    None.

  Parameters:
    pData      - Pointer to the memory block to be set free

  Returns:
    None.

  Example:
    <code>
    // create a working array
    uint8_t* pData;

     pData = OSAL_Malloc(32);
     if (pData != NULL)
     {
        ...

        // deallocate the memory
        OSAL_Free(pData);
        // and prevent it accidentally being used again
        pData = NULL;
     }
    </code>

  Remarks:
    None.

 */
//void OSAL_Free(void* pData);

// *****************************************************************************
/* Function: 
    OSAL_RESULT OSAL_Initialize()

  Summary:
    Performs OSAL initialization.

  Description:
     This function performs OSAL initialization .This function should be called
     near the start of main in an application that will use an underlying RTOS.
     This permits the RTOS to perform any one time initialization before the
     application attempts to create drivers or other items that may use the RTOS.
     Typical actions performed by OSAL_Initialize would be to allocate and
     prepare any memory pools for later use.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    OSAL_RESULT_TRUE  - Initialization completed successfully.

  Example:
    <code>
     int main()
     {
         OSAL_Initialize();

         App_Init();
         OSAL_Start();
     }
    </code>

  Remarks:
    None.

 */
//OSAL_RESULT OSAL_Initialize();

// *****************************************************************************
/* Function: 
    const char* OSAL_Name()

  Summary:
    Obtains the name of the underlying RTOS.

  Description:
    This function returns a const char* to the textual name of the RTOS.
    The name is a NULL terminated string.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    * const char* -   Name of the underlying RTOS or NULL

  Example:
    <code>
    // get the RTOS name
    const char* sName;

    sName = OSAL_Name();
    sprintf(buff, "RTOS: %s", sName);
    </code>

  Remarks:
    None.

 */
//__inline__ const char* OSAL_Name(void);

#endif // _OSAL_H

/*******************************************************************************
 End of File
 */