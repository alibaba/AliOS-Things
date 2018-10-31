/*
************************************************************************************************************************
*                                                      uC/OS-II
*                                                 The Real-Time Kernel
*
*                                     (c) Copyright 2014; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   Microchip OSAL
*
* File    : OSAL_UCOS2.C
* By      : JJL
* Version : V2.92.11
*
* LICENSING TERMS:
* ---------------
*           uC/OS-II is provided in source form for FREE short-term evaluation, for educational use or
*           for peaceful research.  If you plan or intend to use uC/OS-II in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-II for its use in your
*           application/product.   We provide ALL the source code for your convenience and to help you
*           experience uC/OS-II.  The fact that the source is provided does NOT mean that you can use
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

#define  MICRIUM_SOURCE
#include <xc.h>
//#include <os_cfg_app.h>
#include <ucos_ii.h>                                 /* uC/OS-II header file                                              */

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *osal_micrium__c = "$Id: $";
#endif


#include "osal/osal_ucos2.h"

/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_CRIT_Enter()
------------------------------------------------------------------------------------------------------------------------
  Function: void OSAL_CRIT_Enter(OSAL_CRIT_TYPE severity)

  Summary:
    Enter a critical section with the specified severity level.

  Description:
     The program is entering a critical section of code. It is assumed that the
     sequence of operations bounded by the enter and leave critical section operations
     is treated as one atomic sequence that will not be disturbed.

  Precondition:
    None

  Parameters:
    severity      - OSAL_CRIT_TYPE_LOW, The RTOS should disable all other running
                    tasks effectively locking the scheduling mechanism.
                  - OSAL_CRIT_TYPE_HIGH, The RTOS should disable all possible
                    interrupts sources including the scheduler ensuring that the
                    sequence of code operates without interruption.

  Returns:
    None

  Example:
    <code>
     // prevent other tasks pre-empting this sequence of code
     OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
     // modify the peripheral
     DRV_USART_Reinitialize( objUSART,  &initData);
     OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW);
    </code>

  Remarks:
    The sequence of operations bounded by the OSAL_CRIT_Enter() and OSAL_CRIT_Leave()
    form a critical section. The severity level defines whether the RTOS should
    perform task locking or completely disable all interrupts.
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_CRITSECT_DATA_TYPE __inline__ __attribute__((nomips16)) __attribute__((nomicromips)) __attribute__((always_inline)) OSAL_CRIT_Enter (OSAL_CRIT_TYPE  severity)
{

    OS_CPU_SR  cpu_sr;

    switch (severity) {
        case OSAL_CRIT_TYPE_LOW:
             OSSchedLock();
             break;

        case OSAL_CRIT_TYPE_HIGH:
             OS_ENTER_CRITICAL();
             break;
    }
    return (cpu_sr);
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_CRIT_Leave()
------------------------------------------------------------------------------------------------------------------------
  Function: void OSAL_CRIT_Leave (OSAL_CRIT_TYPE severity)

  Summary:
    Leave a critical section with the specified severity level.

  Description:
    The program is leaving a critical section of code. It is assumed that the
    sequence of operations bounded by the enter and leave critical section operations
    is treated as one atomic sequence that will not be disturbed.
    The severity should match the severity level used in the corresponding
    OSAL_CRIT_Enter call to ensure that the RTOS carries out the correct action.

  Precondition:
    None

  Parameters:
    severity      - OSAL_CRIT_TYPE_LOW, The RTOS should disable all other running
                    tasks effectively locking the scheduling mechanism.
                  - OSAL_CRIT_TYPE_HIGH, The RTOS should disable all possible
                    interrupts sources including the scheduler ensuring that the
                    sequence of code operates without interruption.

  Returns:
    None

  Example:
    <code>
     // prevent other tasks pre-empting this sequence of code
     OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
     // modify the peripheral
     DRV_USART_Reinitialize( objUSART,  &initData);
     OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW);
    </code>

  Remarks:
    The sequence of operations bounded by the OSAL_CRIT_Enter and OSAL_CRIT_Leave
    form a critical section. The severity level defines whether the RTOS should
    perform task locking or completely disable all interrupts.
------------------------------------------------------------------------------------------------------------------------
*/
void __inline__ __attribute__((nomips16)) __attribute__((nomicromips)) __attribute__((always_inline)) OSAL_CRIT_Leave (OSAL_CRIT_TYPE severity, OSAL_CRITSECT_DATA_TYPE status)
{

    OS_CPU_SR  cpu_sr;


    switch (severity) {
        case OSAL_CRIT_TYPE_LOW:
             OSSchedUnlock();
             break;

        case OSAL_CRIT_TYPE_HIGH:
             cpu_sr = status;
             OS_EXIT_CRITICAL();
             break;
    }
}

/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_Initialize()
------------------------------------------------------------------------------------------------------------------------

  Function: OSAL_RESULT  OSAL_Initialize (void)

  Summary:
    Initialize internal objects needed by the OSAL 

  Description:
    Initialize the mutex used to protect the heap.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Example:
    <code>
    OSAL_Initialize();
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_Initialize (void)
{
   return (OSAL_RESULT_TRUE);
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                       OSAL_Name()
------------------------------------------------------------------------------------------------------------------------

  Function: const  char *OSAL_Name (void)

  Summary:
    Get the name of the underlying kernel

  Description:
    This functions returns the name of the underlying RTOS kernel.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Example:
    <code>
    const  char *sName;

    sName = OSAL_Name();
    sprintf(buff, "RTOS Kernel: %s", sName);
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/

const  char __inline__ __attribute__((always_inline)) *OSAL_Name (void)
{
    return ("Micrium uC/OS-II");
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                       OSAL_Malloc()
------------------------------------------------------------------------------------------------------------------------

  Function: void *OSAL_Malloc (size_t size)

  Summary:
    Allocate dynamic storage

  Description:
    Get memory from heap

  Precondition:
    None

  Parameters:
    size         - The desired number of continuous bytes from the heap.

  Returns:
    a Pointer to the allocated storage or a NULL pointer if the request failed

  Example:
    <code>
    void *p_mem;


    p_mem = OSAL_Malloc(100);
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/

void* OSAL_Malloc (size_t  size)
{
    void                    *p_data;
    OSAL_CRITSECT_DATA_TYPE  IntState;


    IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    p_data   = malloc(size);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, IntState);

    return (p_data);
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                      OSAL_Free()
------------------------------------------------------------------------------------------------------------------------

  Function: void OSAL_Free (void *pData)

  Summary:
    Deallocate dynamic storage

  Description:
    Return memory back to heap

  Precondition:
    None

  Parameters:
    p_data        - A pointer to the block of storage to return back to the heap

  Returns:
    None

  Example:
    <code>
    OSAL_Free(p_mem);
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/
void  OSAL_Free (void *p_data)
{
    OSAL_CRITSECT_DATA_TYPE IntState;


    IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    free(p_data);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, IntState);    
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_SEM_Create()
------------------------------------------------------------------------------------------------------------------------

  Function: OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE  semID,
                                        OSAL_SEM_TYPE         type,
                                        uint8_t               maxCount,
                                        uint8_t               initialCount)
  Summary:
    Create an OSAL Semaphore

  Description:
    Create an OSAL binary or counting semaphore. If OSAL_SEM_TYPE_BINARY is specified then
    the maxcount and initialCount values are ignored otherwise these must contain valid
    values.

  Precondition:
    Semaphore must have been declared.

  Parameters:
    semID        - Semaphore ID

    type         - OSAL_SEM_TYPE_BINARY,   create a binary semaphore
                 - OSAL_SEM_TYPE_COUNTING, create a counting semaphore with the specified
                   count values.

    maxCount     - Maximum value for a counting semaphore. Ignored for a BINARY semaphore.

    initialCount - Starting count value for the semaphore. Ignored for a BINARY semaphore

  Returns:
    OSAL_RESULT_TRUE    - Semaphore created
    OSAL_RESULT_FALSE   - Semaphore creation failed

  Example:
    <code>
    OSAL_SEM_Create(&mySemID, OSAL_SEM_TYPE_COUNTING, 10, 5);
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/

OSAL_RESULT  OSAL_SEM_Create (OSAL_SEM_HANDLE_TYPE* semID, OSAL_SEM_TYPE type, uint8_t maxCount, uint8_t initialCount)
{
    (void)maxCount;
    switch (type) {
        case OSAL_SEM_TYPE_BINARY:
             semID = OSSemCreate(1);
             break;

        case OSAL_SEM_TYPE_COUNTING:
             semID = OSSemCreate(initialCount);
             break;

        default:
             return (OSAL_RESULT_NOT_IMPLEMENTED);
    }

    return (OSAL_RESULT_TRUE);
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_SEM_Delete()
------------------------------------------------------------------------------------------------------------------------

  Function: OSAL_RESULT OSAL_SEM_Delete (OSAL_SEM_HANDLE_TYPE semID)

  Summary:
    Delete an OSAL Semaphore

  Description:
    Delete an OSAL semaphore.

  Precondition:
    Semaphore must have been created.

  Parameters:
    semID       - Pointer to the semID

  Returns:
    OSAL_RESULT_TRUE    - Semaphore deleted
    OSAL_RESULT_FALSE   - Semaphore deletion failed

  Example:
    <code>
     OSAL_SEM_Delete(&mySemID);
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_SEM_Delete (OSAL_SEM_HANDLE_TYPE* semID)
{

    INT8U     err;


    OSSemDel(semID,
            OS_DEL_ALWAYS,
            &err);
    if (err == OS_ERR_NONE) {
        return (OSAL_RESULT_TRUE);
    } else {
        return (OSAL_RESULT_FALSE);
    }
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_SEM_Pend()
------------------------------------------------------------------------------------------------------------------------
  Function: OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE semID,
                                      uint16_t             waitMS)

  Summary:
     Pend on a semaphore. Returns true if semaphore obtained within time limit.

  Description:
     Blocking function call that pends (waits) on a semaphore. The function will
     return true is the semaphore has been obtained or false if it was not available
     or the time limit was exceeded.

  Precondition:
     Semaphore must have been created.

  Parameters:
     semID       - Pointer to the semID

    waitMS       - Time limit to wait in milliseconds.
                   0                 - do not wait
                   OSAL_WAIT_FOREVER - return only when semaphore is obtained
                   Other values      - timeout delay

  Returns:
    OSAL_RESULT_TRUE    - Semaphore obtained
    OSAL_RESULT_FALSE   - Semaphore not obtained or timeout occurred

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
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_SEM_Pend (OSAL_SEM_HANDLE_TYPE*  semID, uint16_t waitMS)
{
    INT8U    err;
    INT32U   wait;


    if (waitMS == 0) {                                      /* See if user selects NO WAIT                  */
        return (OSAL_RESULT_FALSE);                         /* ... yes, return immediately                  */
    }

    if (waitMS == OSAL_WAIT_FOREVER) {                      /* See if user selects 'wait for ever'          */
        wait = 0;                                           /* ... yes, for uC/OS-II, need to specify 0    */
    } else {
        wait = (INT32U)waitMS * (INT32U)1000 / (INT32U)OS_TICKS_PER_SEC;
    }

    OSSemPend(semID,
              wait,
              &err);

    if (err == OS_ERR_NONE) {
        return (OSAL_RESULT_TRUE);
    } else {
        return (OSAL_RESULT_FALSE);
    }
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_SEM_Post()
------------------------------------------------------------------------------------------------------------------------
  Function: OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE *semID)

  Summary:
     Post a semaphore or increment a counting semaphore.

  Description:
     Post a binary semaphore or increment a counting semaphore. The highest
     priority task currently blocked on the semaphore will be released and
     made ready to run.

  Precondition:
     Semaphore must have been created.

  Parameters:
     semID              - Pointer to the semID

  Returns:
    OSAL_RESULT_TRUE    - Semaphore posted
    OSAL_RESULT_FALSE   - Semaphore not posted

  Example:
    <code>
    OSAL_SEM_Post(&semSignal);
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_SEM_Post (OSAL_SEM_HANDLE_TYPE* semID)
{
    INT8U  err;


    err = OSSemPost(semID);

    if (err == OS_ERR_NONE) {
        return (OSAL_RESULT_TRUE);
    } else {
        return (OSAL_RESULT_FALSE);
    }
}

/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_SEM_PostISR()
------------------------------------------------------------------------------------------------------------------------
  Function: OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE semID)

  Summary:
     Post a semaphore or increment a counting semaphore from within an ISR

  Description:
     Post a binary semaphore or increment a counting semaphore. The highest
     priority task currently blocked on the semaphore will be released and
     made ready to run. This form of the post function should be used inside
    an ISR.

  Precondition:
     Semaphore must have been created.

  Parameters:
     semID       - Pointer to the semID

  Returns:
    OSAL_RESULT_TRUE    - Semaphore posted
    OSAL_RESULT_FALSE   - Semaphore not posted

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
     This version of the OSAL_SEM_Post function should be used if the program
     is, or may be, operating inside and ISR. The OSAL will take the necessary
     steps to ensure correct operation possibly disabling interrupts or entering
     a critical section. The exact requirements will depend upon the particular
     RTOS being used.
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_SEM_PostISR (OSAL_SEM_HANDLE_TYPE* semID)
{
    INT8U  err;


    err = OSSemPost(semID);

    if (err == OS_ERR_NONE) {
        return (OSAL_RESULT_TRUE);
    } else {
        return (OSAL_RESULT_FALSE);
    }
}

/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_SEM_GetCount()
------------------------------------------------------------------------------------------------------------------------
  Function: uint8_t OSAL_SEM_GetCount(OSAL_SEM_HANDLE_TYPE semID)

  Summary:
    Return the current value of a counting semaphore.

  Description:
    Return the current value of a counting semaphore. The value returned is
    assumed to be a single vlaue ranging from 0-255.

  Precondition:
     Semaphore must have been created.

  Parameters:
     semID       - Pointer to the semID

  Returns:
    0           - Semaphore is unavailable
    1-255       - Current value of the counting semaphore

  Example:
    <code>
     uint8_t semCount;

     semCount = OSAL_SEM_GetCount(&semUART);

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
     This version of the OSAL_SEM_Post() function should be used if the program
     is, or may be, operating inside and ISR. The OSAL will take the necessary
     steps to ensure correct operation possibly disabling interrupts or entering
     a critical section. The exact requirements will depend upon the particular
     RTOS being used.
------------------------------------------------------------------------------------------------------------------------
*/
uint8_t  OSAL_SEM_GetCount (OSAL_SEM_HANDLE_TYPE*  semID)
{
    INT16U                   ctr;
    OSAL_CRITSECT_DATA_TYPE  IntState;


    IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    ctr      = semID->OSEventCnt;
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, IntState);
    return ((uint8_t)ctr);
}

/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_MUTEX_Create()
------------------------------------------------------------------------------------------------------------------------
  Function: OSAL_RESULT OSAL_MUTEX_Create (OSAL_MUTEX_HANDLE_TYPE mutexID)

  Summary:
    Create a mutex.

  Description:
    This function creates a mutex, allocating storage if required and placing
    the mutex handle into the passed parameter.

  Precondition:
    None.

  Parameters:
    mutexID      - Pointer to the mutex handle

  Returns:
    OSAL_RESULT_TRUE    - Mutex successfully created

    OSAL_RESULT_FALSE   - Mutex failed to be created.

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
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_MUTEX_Create (OSAL_MUTEX_HANDLE_TYPE*  mutexID)
{
    mutexID = OSSemCreate(1);

    return (OSAL_RESULT_TRUE);
}


/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_MUTEX_Delete()
------------------------------------------------------------------------------------------------------------------------
  Function: OSAL_RESULT  OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE mutexID)

  Summary:
    Delete a mutex.

  Description:
    This function deletes a mutex and frees associated storage if required.

  Precondition:
    None.

  Parameters:
    mutexID             - Pointer to the mutex handle

  Returns:
    OSAL_RESULT_TRUE    - Mutex successfully deleted.

    OSAL_RESULT_FALSE   - Mutex failed to be deleted.

  Example:
    <code>
    OSAL_MUTEX_Delete(&mutexData);
    </code>

  Remarks:
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_MUTEX_Delete (OSAL_MUTEX_HANDLE_TYPE*  mutexID)
{
    INT8U     err;


    OSSemDel(mutexID,
            OS_DEL_ALWAYS,
            &err);
    (void)err;
    if (err == OS_ERR_NONE) {
        return (OSAL_RESULT_TRUE);
    } else {
        return (OSAL_RESULT_FALSE);
    }
}



/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_MUTEX_Lock()
------------------------------------------------------------------------------------------------------------------------
  Function: OSAL_RESULT  OSAL_MUTEX_Lock (OSAL_MUTEX_HANDLE_TYPE  mutexID,
                                          uint16_t                waitMS)

  Summary:
    Lock a mutex.

  Description:
    This function locks a mutex, waiting for the specified timeout. If it cannot
    be obtained or the timeout period elapses then false is returned;

  Precondition:
    None.

  Parameters:
    mutexID      - Pointer to the mutex handle

    waitMS       - Timeout value in milliseconds,
                   0                 - do not wait, return immediately
                   OSAL_WAIT_FOREVER - wait until mutex is obtained before returning
                   Other values      - Timeout delay

  Returns:
    OSAL_RESULT_TRUE    - Mutex successfully obtained.

    OSAL_RESULT_FALSE   - Mutex failed to be obtained or timeout occurred.

  Example:
    <code>
    OSAL_MUTEX_HANDLE_TYPE mutexData;

    OSAL_MUTEX_Create(&mutexData);
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
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT  OSAL_MUTEX_Lock (OSAL_MUTEX_HANDLE_TYPE*  mutexID, uint16_t  waitMS)
{
    INT8U   err;
    INT32U   wait;


    if (waitMS == 0) {                                      /* See if user selects NO WAIT                  */
        return (OSAL_RESULT_FALSE);                         /* ... yes, return immediately                  */
    }

    if (waitMS == OSAL_WAIT_FOREVER) {                      /* See if user selects 'wait for ever'          */
        wait = 0;                                           /* ... yes, for uC/OS-II, need to specify 0    */
    } else {
        wait = (INT32U)waitMS * (INT32U)1000 / (INT32U)OS_TICKS_PER_SEC;
    }

    OSSemPend(mutexID,
              wait,
              &err);

    if (err == OS_ERR_NONE) {
        return (OSAL_RESULT_TRUE);
    } else {
        return (OSAL_RESULT_FALSE);
    }
}

/*
------------------------------------------------------------------------------------------------------------------------
                                                 OSAL_MUTEX_Unlock()
------------------------------------------------------------------------------------------------------------------------
  Function: OSAL_RESULT  OSAL_MUTEX_Unlock (OSAL_MUTEX_HANDLE_TYPE mutexID)

  Summary:
    Unlock a mutex.

  Description:
    This function unlocks a previously obtained mutex.

  Precondition:
    None.

  Parameters:
    mutexID             - Pointer to the mutex handle

  Returns:
    OSAL_RESULT_TRUE    - Mutex released.

    OSAL_RESULT_FALSE   - Mutex failed to be released or error occurred.

  Example:
    <code>
    OSAL_MUTEX_HANDLE_TYPE mutexData;

    OSAL_MUTEX_Create(&mutexData);
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
------------------------------------------------------------------------------------------------------------------------
*/
OSAL_RESULT OSAL_MUTEX_Unlock (OSAL_MUTEX_HANDLE_TYPE*  mutexID)
{
    INT8U  err;


    err = OSSemPost(mutexID);

    if (err == OS_ERR_NONE) {
        return (OSAL_RESULT_TRUE);
    } else {
        return (OSAL_RESULT_FALSE);
    }
}



