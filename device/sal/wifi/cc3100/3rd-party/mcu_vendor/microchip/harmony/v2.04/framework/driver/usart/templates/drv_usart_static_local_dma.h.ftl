/*******************************************************************************
  USART Driver Local Data Structures for static implementation with DMA support

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static_local_dma.h

  Summary:
    USART Driver Local Data Structures for static implementation with DMA support

  Description:
    Driver Local Data Structures for static implementation with DMA support
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

#ifndef _DRV_USART_STATIC_LOCAL_DMA_H
#define _DRV_USART_STATIC_LOCAL_DMA_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/usart/drv_usart.h"
#include "driver/usart/src/drv_usart_variant_mapping.h"
<#if CONFIG_USE_3RDPARTY_RTOS>
#include "osal/osal.h"
</#if>
#include "system/clk/sys_clk.h"
#include "system/int/sys_int.h"
#include "system/debug/sys_debug.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/* USART FIFO+RX(8+1) size */
#define _DRV_USART_RX_DEPTH     9

// *****************************************************************************
/* USART DMA Driver Buffer Handle Macros

  Summary:
    USART DMA driver Buffer Handle Macros

  Description:
    Buffer handle related utility macros. USART driver buffer handle is a
    combination of buffer token and the buffer object index. The token
    is a 16 bit number that is incremented for every new read or write request
    and is used along with the buffer object index to generate a new buffer
    handle for every request.

  Remarks:
    None
*/

#define _DRV_USART_DMA_BUFFER_TOKEN_MAX         (0xFFFF)
#define _DRV_USART_DMA_MAKE_HANDLE(token, index) ((token) << 16 | (index))
#define _DRV_USART_DMA_UPDATE_BUFFER_TOKEN(token) \
{ \
    (token)++; \
    if ((token) >= _DRV_USART_DMA_BUFFER_TOKEN_MAX) \
        (token) = 0; \
    else \
        (token) = (token); \
}

// *****************************************************************************
/* USART DMA transfer abort

  Summary:
    USART DMA transfer abort

  Description:
    This constant indicates that the USART DMA transfer is aborted.

  Remarks:
    None.
*/
#define DRV_USART_DMA_TRANSFER_ABORT /*DOM-IGNORE-BEGIN*/((uint32_t)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
/* I2S DMA transfer error

  Summary:
    I2S DMA transfer error

  Description:
    This constant indicates that the I2S DMA transfer has an address error.

  Remarks:
    None.
*/
#define DRV_USART_DMA_TRANSFER_ERROR /*DOM-IGNORE-BEGIN*/((uint32_t)(-1))/*DOM-IGNORE-END*/


// *****************************************************************************
/* USART Buffer object Index

  Summary:
    USART Buffer object Index

  Description:
    This constant indicates the USART Buffer object Index.

  Remarks:
    None.
*/
#define DRV_USART_BUFFER_OBJECT_INDEX_INVALID /*DOM-IGNORE-BEGIN*/((uint32_t)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

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
    bool inUse;

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


/***************************************************
 * This is a data type for the buffer object
   index which will be combined with the unique index id
   to produce a unique buffer handle.
 ***************************************************/
typedef uint32_t DRV_USART_BUFFER_OBJECT_INDEX;

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

    /* Read DMA channel handle */
    SYS_DMA_CHANNEL_HANDLE dmaChannelHandleRead;

    /* Write DMA channel handle */
    SYS_DMA_CHANNEL_HANDLE dmaChannelHandleWrite;

    /* Application Context associated with the client */
    uintptr_t context;

    /* Event handler for this function */
    DRV_USART_BUFFER_EVENT_HANDLER eventHandler;

    /* The Event which has caused the calling of the eventHandler*/
    DRV_USART_BUFFER_EVENT    bufferEvent;

    /* Client specific error */
    DRV_USART_ERROR error;
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
void _DRV_USART${DRV_INSTANCE}_BufferQueueTxTasks(void);
void _DRV_USART${DRV_INSTANCE}_BufferQueueRxTasks(void);
void _DRV_USART${DRV_INSTANCE}_BufferQueueErrorTasks(void);
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
<#if CONFIG_DRV_USART_INST_IDX3 == true>
<@DRV_USART_STATIC_LOCAL_API
DRV_INSTANCE="3"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
<@DRV_USART_STATIC_LOCAL_API
DRV_INSTANCE="4"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
<@DRV_USART_STATIC_LOCAL_API
DRV_INSTANCE="5"
/>
</#if>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif //#ifndef _DRV_USART_STATIC_LOCAL_DMA_H

/*******************************************************************************
 End of File
*/

