/*******************************************************************************
  SYS UART CONSOLE Support Layer
  
  File Name:
    sys_console_uart.c

  Summary:
    SYS UART CONSOLE Support Layer

  Description:
    This file contains the SYS UART CONSOLE support layer logic.
 *******************************************************************************/


// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "system/console/sys_console.h"
#include "sys_console_uart.h"
#include "system_config.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variable Definitions
// *****************************************************************************
// *****************************************************************************

SYS_CONSOLE_DEV_DESC consUsartDevDesc =
{
    .consoleDevice = SYS_CONSOLE_DEV_USART,
    .intent = DRV_IO_INTENT_READWRITE,
    .sysConsoleRead = Console_UART_Read,
    .sysConsoleReadC = Console_UART_ReadC,
    .sysConsoleWrite = Console_UART_Write,
    .sysConsoleRegisterCallback = Console_UART_RegisterCallback,
    .sysConsoleTasks = Console_UART_Tasks,
    .sysConsoleStatus = Console_UART_Status,
    .sysConsoleFlush = Console_UART_Flush
};

static struct QPacket wrQueueElements[SYS_CONSOLE_UART_WR_QUEUE_DEPTH];
static struct QPacket rdQueueElements[SYS_CONSOLE_UART_RD_QUEUE_DEPTH];

static struct QueueNode writeQueue = {0, 0, 0, wrQueueElements, SYS_CONSOLE_UART_WR_QUEUE_DEPTH};
static struct QueueNode readQueue = {0, 0, 0, rdQueueElements, SYS_CONSOLE_UART_RD_QUEUE_DEPTH};

static void pushQueue(struct QueueNode *q, struct QPacket pkt)
{
    q->qPkts[q->nextPos] = pkt;
    (q->nextPos < (q->elemArrSz - 1)) ? q->nextPos++ : (q->nextPos = 0);
    q->numElem++;
}

static void popQueue(struct QueueNode *q)
{
    (q->tailPos < (q->elemArrSz - 1)) ? q->tailPos++ : (q->tailPos = 0);
    q->numElem--;
}

CONS_UART_DATA consUartData =
{
    /* Device Layer Handle  */
    .deviceHandle = -1 ,

    /* console state */
    .state = CONSOLE_UART_STATE_INIT ,

    /* Cleint Status */
    .usartStatus = DRV_USART_CLIENT_STATUS_ERROR,

    /* Initial get line coding state */
    .deviceLineControl = DRV_USART_LINE_CONTROL_8NONE1,

    /* Read Transfer Handle */
    .readTransferHandle = DRV_USART_BUFFER_HANDLE_INVALID,

    /* Write Transfer Handle */
    .writeTransferHandle = DRV_USART_BUFFER_HANDLE_INVALID,

    /* Intialize the read complete flag */
    .isReadComplete = true,

    /* Initialize the write complete flag*/
    .isWriteComplete = true,

    .overflowFlag = false,

    .rdCallback = NULL,

    .wrCallback = NULL
};

SYS_CONSOLE_STATUS Console_UART_Status(void)
{
    SYS_CONSOLE_STATUS status = SYS_CONSOLE_STATUS_NOT_CONFIGURED;

    if (consUartData.state == CONSOLE_UART_STATE_INIT)
        return status;

    if (consUartData.state == CONSOLE_UART_STATE_CRITICAL_ERROR || consUartData.usartStatus == DRV_USART_CLIENT_STATUS_ERROR
            || consUartData.state == CONSOLE_UART_STATE_OPERATIONAL_ERROR || consUartData.overflowFlag ||           
            consUartData.usartStatus == DRV_USART_CLIENT_STATUS_BUSY )                                           
    {
        status = SYS_CONSOLE_STATUS_ERROR;
    }
    else
    {
        status = (writeQueue.numElem || readQueue.numElem) ? SYS_CONSOLE_STATUS_BUSY : SYS_CONSOLE_STATUS_CONFIGURED;
    }
    return status;
}

ssize_t Console_UART_Write(int fd, const void *buf, size_t count)
{
    unsigned char c;
    const char* str = (const char*)buf;

    while((c = *str++))
    {
        while(PLIB_USART_TransmitterBufferIsFull(DRV_USART_PERIPHERAL_ID_IDX0));
        PLIB_USART_TransmitterByteSend(DRV_USART_PERIPHERAL_ID_IDX0, c);
    }

#if 0
    struct QPacket pkt;
    DRV_USART_BUFFER_HANDLE bufferHandle;

    if(writeQueue.numElem == 0)
    {
        /* There is nothing in the console queue. Check if the data can be written
         * directly to the USART driver */
        DRV_USART_BufferAddWrite(consUartData.deviceHandle, &bufferHandle, (void *)buf, count);
    }

    if((bufferHandle == DRV_USART_BUFFER_HANDLE_INVALID) || (writeQueue.numElem > 0))
    {
        /* This means the driver queue is full. Push this message into the
         * console queue. */
        pkt.data.cbuf = buf;
        pkt.sz = count;

        //Pop the most recent if the queue is full
        if (writeQueue.numElem >= writeQueue.elemArrSz)
        {
            popQueue(&writeQueue);
        }

        pushQueue(&writeQueue, pkt);
    }

#endif
    return count;
}

ssize_t Console_UART_Read(int fd, void *buf, size_t count)
{
    struct QPacket pkt;
    size_t rdBytes = 0;

    pkt.data.buf = buf;
    pkt.sz = count;

    while ((readQueue.numElem < readQueue.elemArrSz) && (rdBytes < count))
    {
        pushQueue(&readQueue, pkt);
        pkt.data.buf = (char*)pkt.data.buf + 1;
        rdBytes++;
    }
    
    return rdBytes;
}

char Console_UART_ReadC(int fd)
{
    char readBuffer;
    ssize_t bytesRead = 0;

    do
    {
        bytesRead  = DRV_USART_Read(consUartData.deviceHandle, &readBuffer, 1);
        if(bytesRead == DRV_USART_READ_ERROR)
        {
            break;
        }
    } while (bytesRead == 0);

    return readBuffer;
}

void Console_UART_RegisterCallback(consoleCallbackFunction consCallback, SYS_CONSOLE_EVENT event)
{
    switch (event)
    {
        case SYS_CONSOLE_EVENT_READ_COMPLETE:
            consUartData.rdCallback = consCallback;
            break;
        case SYS_CONSOLE_EVENT_WRITE_COMPLETE:
            consUartData.wrCallback = consCallback;
            break;
        default:
            break;
    }
}

void Console_UART_Flush(void)
{
    if (consUartData.state != CONSOLE_UART_STATE_INIT)
    {
        consUartData.state = CONSOLE_UART_STATE_READY;
    }
    
    consUartData.overflowFlag = false;

    consUartData.writeTransferHandle = DRV_USART_BUFFER_HANDLE_INVALID;
    consUartData.isWriteComplete = true;
    writeQueue.nextPos = 0;
    writeQueue.tailPos = 0;
    writeQueue.numElem = 0;

    consUartData.readTransferHandle = DRV_USART_BUFFER_HANDLE_INVALID;
    consUartData.isReadComplete = true;
    readQueue.nextPos = 0;
    readQueue.tailPos = 0;
    readQueue.numElem = 0;
}

void UARTDeviceEventHandler
(
    DRV_USART_BUFFER_EVENT event,
    DRV_USART_BUFFER_HANDLE bufferHandle,
    uintptr_t context
)
{
    if (bufferHandle == DRV_USART_BUFFER_HANDLE_INVALID)
        return;
    
    switch ( event )
    {
        case DRV_USART_BUFFER_EVENT_COMPLETE:
            if(bufferHandle == consUartData.writeTransferHandle)
            {
                consUartData.isWriteComplete = true;
            }
            else if (bufferHandle == consUartData.readTransferHandle)
            {
                consUartData.isReadComplete = true;
            }
            break;
        case DRV_USART_BUFFER_EVENT_ERROR:
            break;
        default:
            break;

    }
}

void Console_UART_Tasks(SYS_MODULE_OBJ object)
{
    /* Update the application state machine based
     * on the current state */
    struct QPacket pkt;
    size_t *sizeRead;

    if (consUartData.state != CONSOLE_UART_STATE_INIT)
    {
        consUartData.usartStatus = DRV_USART_ClientStatus(consUartData.deviceHandle);
    }

    switch(consUartData.state)
    {
        case CONSOLE_UART_STATE_INIT:
        
            /* Open the device layer */
            consUartData.deviceHandle = DRV_USART_Open(SYS_CONSOLE_UART_IDX,
                    (DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_NONBLOCKING));

            if(consUartData.deviceHandle == DRV_HANDLE_INVALID)
            {
                /* If the UART driver could not be opened, this most likely
                 * could mean that the driver is being initialized. Continue to
                 * try opening the driver */

                consUartData.state = CONSOLE_UART_STATE_INIT;
                break;
            }

            /* Set the baud rate */
            DRV_USART_BaudSet(consUartData.deviceHandle, SYS_CONSOLE_UART_BAUD_RATE_IDX);

            /* Set line control */
            DRV_USART_LineControlSet(consUartData.deviceHandle, DRV_USART_LINE_CONTROL_8NONE1);

            /* Register a callback with device layer to get event notification (for end point 0) */
            DRV_USART_BufferEventHandlerSet(consUartData.deviceHandle, UARTDeviceEventHandler, 0);

            /* If the driver was opened, it is ready for operation */
            consUartData.state = CONSOLE_UART_STATE_READY;

            break;
            
        case CONSOLE_UART_STATE_SCHEDULE_READ:

            consUartData.state = CONSOLE_UART_STATE_WAIT_FOR_READ_COMPLETE;

            if (readQueue.numElem)
            {
                consUartData.isReadComplete = false;
                pkt = rdQueueElements[readQueue.tailPos];
                consUartData.readTransferHandle = DRV_USART_BUFFER_HANDLE_INVALID;

                DRV_USART_BufferAddRead (consUartData.deviceHandle, &consUartData.readTransferHandle,
                        pkt.data.buf, pkt.sz);

                if(consUartData.readTransferHandle == DRV_USART_BUFFER_HANDLE_INVALID)
                {
                    consUartData.state = CONSOLE_UART_STATE_READY;
                    break;
                }
            }
            break;

        case CONSOLE_UART_STATE_WAIT_FOR_READ_COMPLETE:

            if(consUartData.isReadComplete)
            {
                consUartData.readTransferHandle = DRV_USART_BUFFER_HANDLE_INVALID;
                sizeRead = &rdQueueElements[readQueue.tailPos].sz;
                popQueue(&readQueue);
                if (readQueue.numElem == 0)
                {
                    if (consUartData.rdCallback != NULL)
                    {
                        consUartData.rdCallback(sizeRead);
                    }
                    consUartData.state = CONSOLE_UART_STATE_READY;
                }
                else
                {
                    consUartData.state = CONSOLE_UART_STATE_SCHEDULE_READ;
                }
            }
            else
            {
                if (writeQueue.numElem)
                {
                    consUartData.state = CONSOLE_UART_STATE_SCHEDULE_WRITE;
                }
            }
            break;

        case CONSOLE_UART_STATE_READY:

            if (readQueue.numElem && consUartData.readTransferHandle != DRV_USART_BUFFER_HANDLE_INVALID)
            {
                consUartData.state = CONSOLE_UART_STATE_WAIT_FOR_READ_COMPLETE;
            }
            else if (readQueue.numElem && consUartData.readTransferHandle == DRV_USART_BUFFER_HANDLE_INVALID)
            {
                consUartData.state = CONSOLE_UART_STATE_SCHEDULE_READ;
            }
            else if (writeQueue.numElem)
            {
                /* If there is data to be written, then try writing it */
                consUartData.state = CONSOLE_UART_STATE_SCHEDULE_WRITE;
            }
            break;

        case CONSOLE_UART_STATE_SCHEDULE_WRITE:

            if (writeQueue.numElem)
            {
                /* This means there is data to sent out */
                pkt = wrQueueElements[writeQueue.tailPos];

                // Use write with callback version for buffer size greater than 1
                do
                {
                    /* This loop will NOT run forever. It will run till
                     * either there are elements in the write or till USART
                     * driver buffer queue is full */ 

                    consUartData.writeTransferHandle = DRV_USART_BUFFER_HANDLE_INVALID;
                    DRV_USART_BufferAddWrite(consUartData.deviceHandle, &consUartData.writeTransferHandle, pkt.data.buf, pkt.sz);
                    if(consUartData.writeTransferHandle != DRV_USART_BUFFER_HANDLE_INVALID)
                    {
                        /* This means this buffer was added successfully to
                         * the driver queue. Do a callback. */
                        if (consUartData.wrCallback != NULL)
                        {
                            consUartData.wrCallback((void *)wrQueueElements[writeQueue.tailPos].data.cbuf);
                        }

                        consUartData.usartStatus = DRV_USART_CLIENT_STATUS_READY;        

                        /* Remove this element from the write queue */
                        popQueue(&writeQueue);
                        pkt = wrQueueElements[writeQueue.tailPos];
                    }
                    else
                    {
                        /* Got an invalid handle. This most likely means
                         * that the write buffer queue is full */

                        consUartData.usartStatus = DRV_USART_CLIENT_STATUS_BUSY;         
                    }

                } while((writeQueue.numElem) && (consUartData.writeTransferHandle != DRV_USART_BUFFER_HANDLE_INVALID));

                /* If we are here, it either means there is nothing more to
                 * write of the write buffer queue is full. We cannot do much
                 * with respect to write at point. We move the state to
                 * ready */

                consUartData.state = CONSOLE_UART_STATE_READY;
            }
            break;

        case CONSOLE_UART_STATE_OPERATIONAL_ERROR:

            /* We arrive at this state if the UART driver reports an error on a read or write operation
               We will attempt to recover by flushing the local buffers */

            Console_UART_Flush();

            break;

        case CONSOLE_UART_STATE_CRITICAL_ERROR:
            break;
        default:
            break;
    }
}

/*******************************************************************************
 End of File
 */
