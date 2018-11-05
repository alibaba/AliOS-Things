/*******************************************************************************
  SYS APPIO CONSOLE Support Layer
  
  File Name:
    sys_console_appio.c

  Summary:
    SYS APPIO CONSOLE Support Layer

  Description:
    This file contains the SYS APPIO CONSOLE support layer logic.
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
#include "sys_console_appio.h"
#include "system_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/appio.h>

// *****************************************************************************
// *****************************************************************************
// Section: Global Variable Definitions
// *****************************************************************************
// *****************************************************************************

SYS_CONSOLE_DEV_DESC consAppIODevDesc =
{
    .consoleDevice = SYS_CONSOLE_DEV_APPIO,
    .intent = DRV_IO_INTENT_READWRITE,
    .sysConsoleRead = Console_APPIO_Read,
    .sysConsoleReadC = Console_APPIO_ReadC,
    .sysConsoleWrite = Console_APPIO_Write,
    .sysConsoleRegisterCallback = Console_APPIO_RegisterCallback,
    .sysConsoleTasks = Console_APPIO_Tasks,
    .sysConsoleStatus = Console_APPIO_Status,
    .sysConsoleFlush = Console_APPIO_Flush
};

static struct QPacket wrQueueElements[SYS_CONSOLE_APPIO_WR_QUEUE_DEPTH];
static struct QPacket rdQueueElements[SYS_CONSOLE_APPIO_RD_QUEUE_DEPTH];

static struct QueueNode writeQueue = {0, 0, 0, wrQueueElements, SYS_CONSOLE_APPIO_WR_QUEUE_DEPTH};
static struct QueueNode readQueue = {0, 0, 0, rdQueueElements, SYS_CONSOLE_APPIO_RD_QUEUE_DEPTH};

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

CONS_APPIO_DATA consAppIOData =
{
    /* console state */
    .state = CONSOLE_APPIO_STATE_INIT ,

    .rdCallback = NULL,

    .wrCallback = NULL,

    .writeCycleCount = 0
};

SYS_CONSOLE_STATUS Console_APPIO_Status(void)
{
    SYS_CONSOLE_STATUS status = SYS_CONSOLE_STATUS_NOT_CONFIGURED;

    if (consAppIOData.state == CONSOLE_APPIO_STATE_INIT)
        return status;

    status = SYS_CONSOLE_STATUS_CONFIGURED;

    return status;
}

ssize_t Console_APPIO_Write(int fd, const void *buf, size_t count)
{
    struct QPacket pkt;

    pkt.data.cbuf = buf;
    pkt.sz = count;

    //Pop the most recent if the queue is full
    if (writeQueue.numElem >= writeQueue.elemArrSz)
    {
        popQueue(&writeQueue);
    }

    pushQueue(&writeQueue, pkt);

    return count;
}

ssize_t Console_APPIO_Read(int fd, void *buf, size_t count)
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

char Console_APPIO_ReadC(int fd)
{
    char readBuffer = '\0';

#if defined(__APPIO_DEBUG)
    DBSCANF ("%s", &readBuffer);
#elif defined(PIC32_STARTER_KIT)
    DBGETS (pkt.data.buf,pkt.sz);
#endif

    return readBuffer;
}

void Console_APPIO_RegisterCallback(consoleCallbackFunction consCallback, SYS_CONSOLE_EVENT event)
{
    switch (event)
    {
        case SYS_CONSOLE_EVENT_READ_COMPLETE:
            consAppIOData.rdCallback = consCallback;
            break;
        case SYS_CONSOLE_EVENT_WRITE_COMPLETE:
            consAppIOData.wrCallback = consCallback;
            break;
        default:
            break;
    }
}

void Console_APPIO_Flush(void)
{
    if (consAppIOData.state != CONSOLE_APPIO_STATE_INIT)
    {
        consAppIOData.state = CONSOLE_APPIO_STATE_READY;
    }
    
    consAppIOData.writeCycleCount = 0;
    writeQueue.nextPos = 0;
    writeQueue.tailPos = 0;
    writeQueue.numElem = 0;
    readQueue.nextPos = 0;
    readQueue.tailPos = 0;
    readQueue.numElem = 0;
}

void Console_APPIO_Tasks (SYS_MODULE_OBJ object)
{

    /* Update the application state machine based
     * on the current state */
    struct QPacket pkt;
    size_t *sizeRead;

    switch(consAppIOData.state)
    {
        case CONSOLE_APPIO_STATE_INIT:

            /* Initialize APPIO */
            DBINIT();
 
            consAppIOData.state = CONSOLE_APPIO_STATE_READY;

            break;

        case CONSOLE_APPIO_STATE_START_READ:

            if (readQueue.numElem)
            {
                pkt = rdQueueElements[readQueue.tailPos];

                DBGETS (pkt.data.buf, 128);

                if (*(char*)pkt.data.buf != '\0')
                {
                    rdQueueElements[readQueue.tailPos].sz = strlen((char*)pkt.data.buf);                    
                    consAppIOData.state = CONSOLE_APPIO_STATE_READ_COMPLETE;

                }
            }
            break;

        case CONSOLE_APPIO_STATE_READ_COMPLETE:

            sizeRead = &rdQueueElements[readQueue.tailPos].sz;
            
            popQueue(&readQueue);
            
            if (readQueue.numElem == 0)
            {
                if (consAppIOData.rdCallback != NULL)
                {
                    consAppIOData.rdCallback(sizeRead);
                }
                consAppIOData.state = CONSOLE_APPIO_STATE_READY;
            }
            else
            {
                consAppIOData.state = CONSOLE_APPIO_STATE_START_READ;
            }
            break;

        case CONSOLE_APPIO_STATE_READY:

            if (writeQueue.numElem)
            {
                 consAppIOData.state = CONSOLE_APPIO_STATE_START_WRITE;
            }
            else if (readQueue.numElem)
            {
                 consAppIOData.state = CONSOLE_APPIO_STATE_START_READ;
            }
            break;

        case CONSOLE_APPIO_STATE_START_WRITE:

            if (writeQueue.numElem)
            {
                pkt = wrQueueElements[writeQueue.tailPos];

                DBPRINTF("%s",(char*)pkt.data.buf);

                consAppIOData.writeCycleCount = 0;
                consAppIOData.state = CONSOLE_APPIO_STATE_WAIT_FOR_WRITE_COMPLETE;
            }
            break;

        case CONSOLE_APPIO_STATE_WAIT_FOR_WRITE_COMPLETE:

            pkt = wrQueueElements[writeQueue.tailPos];
            
            /* Burn cycles to allow to printf to complete */
            if(consAppIOData.writeCycleCount++ > pkt.sz * 400)
            {
                if (consAppIOData.wrCallback != NULL)
                {
                    consAppIOData.wrCallback((void *)pkt.data.cbuf);
                }

                popQueue(&writeQueue);
                
                if (writeQueue.numElem == 0)
                {
                    consAppIOData.state = CONSOLE_APPIO_STATE_READY;
                }
                else
                {
                    consAppIOData.state = CONSOLE_APPIO_STATE_START_WRITE;
                }
            }
            break;

        case CONSOLE_APPIO_STATE_OPERATIONAL_ERROR:

            /* We arrive at this state if the APPIO driver reports an error on a read or write operation
             We will attempt to recover by flushing the local buffers */

            Console_APPIO_Flush();
            
            break;

        case CONSOLE_APPIO_STATE_CRITICAL_ERROR:
            break;
        default:
            break;
    }
}
/*******************************************************************************
 End of File
 */

