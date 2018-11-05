/*******************************************************************************
  USART Driver Local Data Structures for static implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static_local_pic32c.h

  Summary:
    USART Driver Local Data Structures for static implementation

  Description:
    Driver Local Data Structures for static implementation
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute Software
only when embedded on a Microchip microcontroller or digital  signal  controller
that is integrated into your product or third party  product  (pursuant  to  the
sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_USART_STATIC_LOCAL_PIC32C_H
#define _DRV_USART_STATIC_LOCAL_PIC32C_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/usart/drv_usart.h"
<#if CONFIG_USE_3RDPARTY_RTOS>
#include "osal/osal.h"
</#if>
#include "system/debug/sys_debug.h"
#include "driver/usart/src/drv_usart_variant_mapping_pic32c.h"


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/* The receiver sampling divide of baudrate clock. */
#define _DRV_USART_HIGH_FRQ_SAMPLE_DIV           16
#define _DRV_USART_LOW_FRQ_SAMPLE_DIV            8
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true>

/* MSB(9th bit) as default high for addressed mode of USART */
#define _DRV_USART_ADDRESSED_MODE_MSB            0x0100
</#if>

<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING)>
// *****************************************************************************
/* USART Driver Buffer Handle Macros

  Summary:
    USART driver Buffer Handle Macros

  Description:
    Buffer handle related utility macros. USART driver buffer handle is a
    combination of buffer token and the buffer object index. The token
    is a 16 bit number that is incremented for every new read or write request
    and is used along with the buffer object index to generate a new buffer
    handle for every request.

  Remarks:
    None
*/

#define _DRV_USART_BUFFER_TOKEN_MAX         (0xFFFF)
#define _DRV_USART_MAKE_HANDLE(token, index) ((token) << 16 | (index))
#define _DRV_USART_UPDATE_BUFFER_TOKEN(token) \
{ \
    (token)++; \
    if ((token) >= _DRV_USART_BUFFER_TOKEN_MAX) \
        (token) = 0; \
    else \
        (token) = (token); \
}

/* USART Buffer Object Flags */
typedef enum
{
    /* Indicates this buffer was submitted by a read write function */
    DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE = 1 << 0,

    /* Indicates this buffer was submitted by a buffer add read write function
       */
    DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD = 1 << 1

} DRV_USART_BUFFER_OBJ_FLAGS;

// *****************************************************************************
/* USART Driver Buffer States

   Summary
    Identifies the possible state of the buffer that can result from a 
    buffer add/delete request.

   Description
    This enumeration identifies the possible state of the buffer that can 
    result from a buffer add/delete request by the client by calling,
      - DRV_USART_BufferAddRead : Updates state to DRV_USART_BUFFER_IS_IN_READ_QUEUE
      - DRV_USART_BufferAddWrite : Updates state to DRV_USART_BUFFER_IS_IN_WRITE_QUEUE
      - DRV_USART_BufferRemove : Updates state to DRV_USART_BUFFER_IS_FREE.
    
   Remarks:
    DRV_USART_BUFFER_IS_FREE is the state of the buffer which is in the 
    free buffer pool.

*/

typedef enum
{
    /* Buffer is not added to either write or read queue. In other words,
     * the buffer is in the free pool. */
    DRV_USART_BUFFER_IS_FREE,

    /* Buffer is added to the write queue. */
    DRV_USART_BUFFER_IS_IN_WRITE_QUEUE,

    /* Buffer is added to the read queue */
    DRV_USART_BUFFER_IS_IN_READ_QUEUE

} DRV_USART_BUFFER_STATE;

// *****************************************************************************
/* USART Driver Buffer Object

  Summary:
    Object used to keep track of a client's buffer.

  Description:
    This object is used to keep track of a client's buffer in the driver's
    queue.

  Remarks:
    None.
*/

typedef struct _DRV_USART_BUFFER_OBJ
{
    /* Driver instance to which the buffer object belongs to */
    uint8_t drvInstance;

    /* This flag tracks whether this object is in use */
    volatile bool inUse;

    /* Pointer to the application read or write buffer */
    uint8_t * buffer;

    /* Tracks how much data has been transferred */
    size_t nCurrentBytes;

    /* Number of bytes to be transferred */
    size_t size;

    /* Next buffer pointer */
    struct _DRV_USART_BUFFER_OBJ * next;

    /* Previous buffer pointer */
    struct _DRV_USART_BUFFER_OBJ * previous;

    /* Flags that indicate the type of buffer */
    DRV_USART_BUFFER_OBJ_FLAGS flags;

    /* Current state of the buffer */
    DRV_USART_BUFFER_STATE currentState;

    /* Buffer Handle that was assigned to this buffer when it was added to the
     * queue. */
    DRV_USART_BUFFER_HANDLE bufferHandle;

} DRV_USART_BUFFER_OBJ;

</#if>

// *****************************************************************************
/* USART Static Driver Instance Object

  Summary:
    Object used to keep any data required for the static USART driver.

  Description:
    This object is used to keep track of any data that must be maintained to
    manage the USART static driver.

  Remarks:
    None.
*/

typedef struct
{

<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==true)>
    /* Keeps track if the driver is in interrupt context
       and if so the nesting levels. */
    uint32_t interruptNestingCount;

    /* The buffer Q for the write operations */
    DRV_USART_BUFFER_OBJ  *queueWrite;

    /* The buffer Q for the read operations */
    DRV_USART_BUFFER_OBJ  *queueRead;

    /* Current read queue size */
    size_t queueSizeCurrentRead;

    /* Current write queue size */
    size_t queueSizeCurrentWrite;

    /* Application Context associated with the client */
    uintptr_t context;

    /* Event handler for this function */
    DRV_USART_BUFFER_EVENT_HANDLER eventHandler;

</#if>
    /* Client specific error */
    DRV_USART_ERROR error;

<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>
    /* Byte model transmit callback function */
    DRV_USART_BYTE_EVENT_HANDLER transmitCallback;
    
    /* Byte model receive callback function */
    DRV_USART_BYTE_EVENT_HANDLER receiveCallback;

    /* Byte model error callback function */
    DRV_USART_BYTE_EVENT_HANDLER errorCallback;
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Hardware instance mutex */
    OSAL_MUTEX_DECLARE(mutexDriverInstance);

<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
    /* Create a semaphore for read function*/
    OSAL_SEM_DECLARE(semReadDone);

    /* Create a semaphore for write function*/
    OSAL_SEM_DECLARE(semWriteDone);
</#if>
</#if>

} DRV_USART_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: Local functions.
// *****************************************************************************
// *****************************************************************************
<#macro DRV_USART_STATIC_LOCAL_API DRV_INSTANCE>
void _DRV_USART${DRV_INSTANCE}_HardwareSetup(void);
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==true)>
void _DRV_USART${DRV_INSTANCE}_BufferQueueTxTasks(void);
void _DRV_USART${DRV_INSTANCE}_BufferQueueRxTasks(void);
void _DRV_USART${DRV_INSTANCE}_BufferQueueErrorTasks(void);
</#if>
<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK == true>
void _DRV_USART${DRV_INSTANCE}_ByteTransmitTasks (void);
void _DRV_USART${DRV_INSTANCE}_ByteReceiveTasks (void);
void _DRV_USART${DRV_INSTANCE}_ByteErrorTasks (void);
</#if>
void _DRV_USART${DRV_INSTANCE}_ErrorConditionClear(void);
</#macro>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
<@DRV_USART_STATIC_LOCAL_API
DRV_INSTANCE="0"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
<@DRV_USART_STATIC_LOCAL_API
DRV_INSTANCE="1"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
<@DRV_USART_STATIC_LOCAL_API
DRV_INSTANCE="2"
/>
</#if>


#endif //#ifndef _DRV_USART_STATIC_LOCAL_PIC32C_H

/*******************************************************************************
 End of File
*/

