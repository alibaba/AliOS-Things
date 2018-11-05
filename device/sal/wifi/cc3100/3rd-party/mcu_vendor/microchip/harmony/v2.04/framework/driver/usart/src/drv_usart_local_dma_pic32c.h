/*******************************************************************************
  USART Driver Local Data Structures with DMA support

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_local_dma_pic32c.h

  Summary:
    USART Driver Local Data Structures with DMA support

  Description:
    Driver Local Data Structures with DMA support
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_USART_LOCAL_DMA_PIC32C_H
#define _DRV_USART_LOCAL_DMA_PIC32C_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/usart/drv_usart.h"
#include "osal/osal.h"
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

/* MSB(9th bit) as default high for addressed mode of USART */
#define _DRV_USART_ADDRESSED_MODE_MSB            0x0100

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
/* USART DMA transfer error

  Summary:
    USART DMA transfer error

  Description:
    This constant indicates that the USART DMA transfer has an address error.

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
    /* This flag tracks whether this object is in use */
    volatile bool inUse;

    /* Pointer to the application read or write buffer */
    uint8_t * buffer;

    /* Address to communicate, Valid in DRV_USART_OPERATION_MODE_ADDRESSED mode */
    uint8_t address;    
    
    /* Tracking address sent. Valid in DRV_USART_OPERATION_MODE_ADDRESSED mode */
    bool isAddressSent;
    
    /* Tracks how much data has been transferred */
    size_t nCurrentBytes;

    /* Number of bytes to be transferred */
    size_t size;

    /* Client that owns this buffer */
    void * hClient;

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
/* USART Driver Global Instances Object

  Summary:
    Object used to keep track of data that is common to all instances of the
    USART driver.

  Description:
    This object is used to keep track of any data that is common to all
    instances of the USART driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Set to true if all members of this structure
       have been initialized once */
    bool membersAreInitialized;

    /* Mutex to protect client object pool */
    OSAL_MUTEX_DECLARE(mutexClientObjects);

    /* Mutex to protect buffer queue object pool */
    OSAL_MUTEX_DECLARE(mutexBufferQueueObjects);

} DRV_USART_COMMON_DATA_OBJ;

// *****************************************************************************
/* USART Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the USART driver.

  Description:
    This object is used to keep track of any data that must be maintained to
    manage a single instance of the USART driver.

  Remarks:
    None.
*/

typedef struct
{
    /* The module index associated with the object*/
    usart_registers_t *moduleId;

    /* The buffer Q for the write operations */
    DRV_USART_BUFFER_OBJ  *queueWrite;

    /* The buffer Q for the read operations */
    DRV_USART_BUFFER_OBJ  *queueRead;

    /* The status of the driver */
    SYS_STATUS status;

    /* Driver operation mode */
    DRV_USART_OPERATION_MODE operationMode;

    /* Flag to indicate this object is in use  */
    bool inUse;

    /* Flag to indicate that driver has been opened exclusively. */
    bool isExclusive;

    /* Keeps track of the number of clients
     * that have opened this driver */
    size_t nClients;

    /* Keep track of the system clock */
    uint32_t brgClock;

    /* Keeps track if the driver is in interrupt context
       and if so the nesting levels. */
    uint32_t interruptNestingCount;

    /* Interrupt Source for TX Interrupt */
    IRQn_Type interruptUSART;


    /* Hardware instance mutex */
    OSAL_MUTEX_DECLARE(mutexDriverInstance);

    /* Read queue size */
    size_t queueSizeRead;

    /* Write queue size */
    size_t queueSizeWrite;

    /* Current read queue size */
    size_t queueSizeCurrentRead;

    /* Current read queue size */
    size_t queueSizeCurrentWrite;

    /* Read DMA channel handle */
    SYS_DMA_CHANNEL_HANDLE dmaChannelHandleRead;

    /* Write DMA channel handle */
    SYS_DMA_CHANNEL_HANDLE dmaChannelHandleWrite;

    /* Transmit DMA Channel Interrupt Source */
    INT_SOURCE interruptDMA;

} DRV_USART_OBJ;


// *****************************************************************************
/* USART Driver Client Object

  Summary:
    Object used to track a single client.

  Description:
    This object is used to keep the data necesssary to keep track of a single
    client.

  Remarks:
    None.
*/

typedef struct
{
    /* The hardware instance object associated with the client */
    DRV_USART_OBJ * hDriver;

    /* The IO intent with which the client was opened */
    DRV_IO_INTENT   ioIntent;

    /* This flags indicates if the object is in use or is
     * available */
    bool inUse;

    /* Client specific buffer object. This is used by the
       DRV_USART_Read() and DRV_USART_Write() functions. */
    DRV_USART_BUFFER_OBJ bufferObject;

    /* Event handler for this function */
    DRV_USART_BUFFER_EVENT_HANDLER eventHandler;

	/* The Event which has caused the calling of the eventHandler*/
    DRV_USART_BUFFER_EVENT    bufferEvent;

    /* Client specific error */
    DRV_USART_ERROR error;

    /* Client status */
    DRV_USART_CLIENT_STATUS status;

    /* Application Context associated with this client */
    uintptr_t context;

    /* Create a semaphore for read function*/
    OSAL_SEM_DECLARE(semReadDone);

    /* Create a semaphore for write function*/
    OSAL_SEM_DECLARE(semWriteDone);

} DRV_USART_CLIENT_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: Local functions.
// *****************************************************************************
// *****************************************************************************

void _DRV_USART_HardwareSetup(usart_registers_t *moduleId, DRV_USART_INIT * init);
DRV_USART_CLIENT_OBJ * _DRV_USART_DriverHandleValidate(DRV_HANDLE handle);
void _DRV_USART_BufferQueueTxTasks(SYS_MODULE_OBJ index);
void _DRV_USART_BufferQueueRxTasks(SYS_MODULE_OBJ index);
void _DRV_USART_BufferQueueErrorTasks(SYS_MODULE_OBJ index);
void _DRV_USART_ErrorConditionClear(DRV_USART_OBJ *hDriver);
bool _DRV_USART_ClientBufferQueueObjectsRemove(DRV_USART_CLIENT_OBJ * clientObj);
void _DRV_USART_DMA_EventHandler(SYS_DMA_TRANSFER_EVENT event,
        SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
#endif //#ifndef _DRV_USART_LOCAL_PIC32C_H

/*******************************************************************************
 End of File
*/

