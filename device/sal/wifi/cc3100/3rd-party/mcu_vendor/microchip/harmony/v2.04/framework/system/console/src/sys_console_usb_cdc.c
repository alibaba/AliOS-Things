/*******************************************************************************
  USB DEVICE CDC Basic Demo Application
  
  File Name:
    app.c

  Summary:
    USB DEVICE CDC Basic Demo application

  Description:
    This file contains the USB DEVICE CDC Basic Demo application logic.
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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system_definitions.h"
#include "usb/usb_device_cdc.h"
#include "sys_console_usb_cdc_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variable Definitions
// *****************************************************************************
// *****************************************************************************

SYS_CONSOLE_DEV_DESC consUsbCdcDevDesc =
{
    .consoleDevice = SYS_CONSOLE_DEV_USB_CDC,
    .intent = DRV_IO_INTENT_READWRITE,
    .sysConsoleRead = Console_USB_Read,
    .sysConsoleWrite = Console_USB_Write,
    .sysConsoleRegisterCallback = Console_USB_RegisterCallback,
    .sysConsoleTasks = Console_USB_Tasks,
    .sysConsoleStatus = Console_USB_Status,
    .sysConsoleFlush = Console_USB_Flush
};

static char* pCdcReadBuff;
static int32_t tmpReadBuffer SYS_CONSOLE_BUFFER_DMA_READY; //4 byte aligned temporary read buffer
static struct QPkt wrQElements[SYS_CONSOLE_USB_CDC_WR_QUEUE_DEPTH];
static struct QPkt rdQElements[SYS_CONSOLE_USB_CDC_RD_QUEUE_DEPTH];

static struct Queue wrQueue = {0, 0, 0, wrQElements, SYS_CONSOLE_USB_CDC_WR_QUEUE_DEPTH};
static struct Queue rdQueue = {0, 0, 0, rdQElements, SYS_CONSOLE_USB_CDC_RD_QUEUE_DEPTH};

char* pReadBuf = NULL;

static void pushQ(struct Queue *q, struct QPkt pkt)
{
    q->qPkts[q->nextPos] = pkt;
    (q->nextPos < (q->elemArrSz - 1)) ? q->nextPos++ : (q->nextPos = 0);
    q->numElem++;
}

static void popQ(struct Queue *q)
{
    (q->tailPos < (q->elemArrSz - 1)) ? q->tailPos++ : (q->tailPos = 0);
    q->numElem--;
}

CONS_USB_CDC_DATA consUsbData =
{
    /* Device Layer Handle  */
    .deviceHandle = USB_DEVICE_HANDLE_INVALID ,

    /* CDC Instance index for this app object00*/
    .cdcInstance = SYS_CONSOLE_USB_CDC_INSTANCE,

    /* app state */
    .state = CONSOLE_USB_CDC_STATE_INIT ,

    /* device configured status */
    .isConfigured = false,

    /* Initial get line coding state */
    .getLineCodingData = {SYS_CONSOLE_USB_CDC_COMM_BAUD_RATE, 0, 0, 8},

    /* Read Transfer Handle */
    .readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID,

    /* Write Transfer Handle */
    .writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID,

    /* Intialize the read complete flag */
    .isReadComplete = true,

    /* Initialize the write complete flag*/
    .isWriteComplete = true,

    .rdCallback = NULL,

    .wrCallback = NULL
};

SYS_CONSOLE_STATUS Console_USB_Status(void)
{
    SYS_CONSOLE_STATUS status = SYS_CONSOLE_STATUS_NOT_CONFIGURED;

    if (consUsbData.isConfigured)
    {
        if (consUsbData.overflowFlag)
        {
            status = SYS_CONSOLE_STATUS_ERROR;
        }
        else
        {
            status = (wrQueue.numElem || rdQueue.numElem) ? SYS_CONSOLE_STATUS_BUSY : SYS_CONSOLE_STATUS_CONFIGURED;
        }
    }
    return status;
}

ssize_t Console_USB_Write(int fd, const void *buf, size_t count)
{
    struct QPkt pkt;

    pkt.data.cbuf = buf;
    pkt.sz = count;

    //Pop the most recent if the queue is full
    if (wrQueue.numElem >= wrQueue.elemArrSz)
    {
        popQ(&wrQueue);
    }

    pushQ(&wrQueue, pkt);

    return count;
}

ssize_t Console_USB_Read(int fd, void *buf, size_t count)
{
    struct QPkt pkt;
    size_t rdBytes = 0;

    pkt.data.buf = buf;
    pkt.sz = count;

    while ((rdQueue.numElem < rdQueue.elemArrSz) && (rdBytes < count))
    {
        pushQ(&rdQueue, pkt);
        pkt.data.buf = (char*)pkt.data.buf + 1;
        rdBytes++;
    }

    return rdBytes;
}

void Console_USB_RegisterCallback(consoleCallbackFunction consCallback, SYS_CONSOLE_EVENT event)
{
    switch (event)
    {
        case SYS_CONSOLE_EVENT_READ_COMPLETE:
            consUsbData.rdCallback = consCallback;
            break;
        case SYS_CONSOLE_EVENT_WRITE_COMPLETE:
            consUsbData.wrCallback = consCallback;
            break;
        default:
            break;
    }
}

void Console_USB_Flush(void)
{
    consUsbData.overflowFlag = false;

    consUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    consUsbData.isReadComplete = true;
    rdQueue.nextPos = 0;
    rdQueue.tailPos = 0;
    rdQueue.numElem = 0;

    consUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    consUsbData.isWriteComplete = true;
    wrQueue.nextPos = 0;
    wrQueue.tailPos = 0;
    wrQueue.numElem = 0;
}
/*****************************************************
 * This function is called in every step of the
 * application state machine.
 *****************************************************/

static bool consUSBReset(void)
{
    /* This function returns true if the device
     * was reset  */

    bool retVal;

    if(consUsbData.isConfigured == false)
    {
        consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_CONFIGURATION;
        consUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        consUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        consUsbData.isReadComplete = true;
        consUsbData.isWriteComplete = true;
        retVal = true;
    }
    else
    {
        retVal = false;
    }

    return(retVal);
}

void USBDeviceEventHandler ( USB_DEVICE_EVENT event,
        void * eventData, uintptr_t context)
{
    uint8_t usbActiveConfiguration;
    switch ( event )
    {
        case USB_DEVICE_EVENT_RESET:

            consUsbData.isConfigured = false;
            break;

        case USB_DEVICE_EVENT_CONFIGURED:

            usbActiveConfiguration = ((USB_DEVICE_EVENT_DATA_CONFIGURED *)eventData)->configurationValue;

            if (usbActiveConfiguration == 1)
            {
                /* Register the CDC Device application event handler here. */
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0,
                        USBDeviceCDCEventHandler, (uintptr_t)&consUsbData);

                consUsbData.isConfigured = true;
            }
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_POWER_DETECTED:
        case USB_DEVICE_EVENT_POWER_REMOVED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;

    }
}

/*******************************************************
 * USB CDC Device Events - Application Event Handler
 *******************************************************/

USB_DEVICE_CDC_EVENT_RESPONSE USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index ,
    USB_DEVICE_CDC_EVENT event ,
    void* pData,
    uintptr_t userData
)
{
    CONS_USB_CDC_DATA * consUsbDataObject;
    consUsbDataObject = (CONS_USB_CDC_DATA *)userData;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    uint16_t *breakData;

    switch ( event )
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:

            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */

              USB_DEVICE_ControlSend(consUsbDataObject->deviceHandle,
                    &consUsbDataObject->getLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:

            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */

            USB_DEVICE_ControlReceive(consUsbDataObject->deviceHandle,
                    &consUsbDataObject->setLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:

            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */

            controlLineStateData =
                    (USB_CDC_CONTROL_LINE_STATE *)pData;
            consUsbDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
            consUsbDataObject->controlLineStateData.carrier =
                    controlLineStateData->carrier;

            USB_DEVICE_ControlStatus(consUsbDataObject->deviceHandle,
                    USB_DEVICE_CONTROL_STATUS_OK);

            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:

            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */

            breakData = (uint16_t *)pData;
            consUsbDataObject->breakData = *breakData;
            break;

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:

            *pCdcReadBuff = tmpReadBuffer;

            /* This means that the host has sent some data*/
            consUsbDataObject->isReadComplete = true;
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:

            /* The data stage of the last control transfer is
             * complete. For now we accept all the data */

            USB_DEVICE_ControlStatus(consUsbDataObject->deviceHandle,
                    USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:

            /* This means the GET LINE CODING function data is valid. We dont
             * do much with this data in this demo. */
            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:

            /* This means that the data write got completed. We can schedule
             * the next read. */
            consUsbDataObject->isWriteComplete = true;
            break;

        default:
            break;
    }

    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

void Console_USB_Tasks(SYS_MODULE_OBJ object)
{
    /* Update the application state machine based
     * on the current state */

    USB_DEVICE_CDC_RESULT result;
    struct QPkt pkt;
    void *tmp;

    switch(consUsbData.state)
    {
        case CONSOLE_USB_CDC_STATE_INIT:

            /* Open the device layer */
            consUsbData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0,
                    DRV_IO_INTENT_READWRITE );

            if(consUsbData.deviceHandle == USB_DEVICE_HANDLE_INVALID)
            {
                //Keep trying, as lower layers may require more time to get started
                break;
            }

            /* Register a callback with device layer to get event notification (for end point 0) */
            USB_DEVICE_EventHandlerSet(consUsbData.deviceHandle, USBDeviceEventHandler, 0);


            /* Attach the device */
            USB_DEVICE_Attach (consUsbData.deviceHandle);

            consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_CONFIGURATION;

            break;

        case CONSOLE_USB_CDC_STATE_WAIT_FOR_CONFIGURATION:

            /* Check if the device was configured */
            if(consUsbData.isConfigured)
            {
                //Rig the console to hold from writing out until the user has made a key press
                pReadBuf = (char*)malloc(4);
                Console_USB_Read(0, (void*)pReadBuf, 1);

                consUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_READ;
            }
            break;

        case CONSOLE_USB_CDC_STATE_SCHEDULE_READ:

            if(consUSBReset())
            {
                break;
            }

            consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE;

            if (rdQueue.numElem)
            {
                consUsbData.isReadComplete = false;
                pkt = rdQElements[rdQueue.tailPos];
                consUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                pCdcReadBuff = pkt.data.buf;
                
                //Force this read to a multiple of the receive buffer size.  This is requirement at the USB CDC level
                USB_DEVICE_CDC_Read (consUsbData.cdcInstance, &consUsbData.readTransferHandle,
                        &tmpReadBuffer, SYS_CONSOLE_USB_CDC_READ_BUFFER_SIZE);

                if(consUsbData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                    consUsbData.state = CONSOLE_USB_CDC_STATE_READY;
                }
            }
            break;

        case CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE:

            if(consUSBReset())
            {
                break;
            }

            if(consUsbData.isReadComplete)
            {
                consUsbData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
                
                tmp = &rdQElements[rdQueue.tailPos].sz;
                popQ(&rdQueue);
                if (rdQueue.numElem == 0)
                {
                    //Free the initial test read buffer, as it is no longer needed
                    //bypass the upper layer callback also
                    if (pReadBuf != NULL)
                    {
                        free((void*)pReadBuf);
                        pReadBuf = NULL;
                    }
                    else if (consUsbData.rdCallback != NULL)
                    {
                        consUsbData.rdCallback(tmp);
                    }

                    consUsbData.state = CONSOLE_USB_CDC_STATE_READY;
                }
                else
                {
                    consUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_READ;
                }
            }
            else if (pReadBuf == NULL)
            {
                //Go to write if there is a write queued
                if (wrQueue.numElem && consUsbData.writeTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                     consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE;
                }
                else if (wrQueue.numElem && consUsbData.isWriteComplete && consUsbData.writeTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                     consUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE;
                }
            }
            break;

        case CONSOLE_USB_CDC_STATE_READY:

            if(consUSBReset())
            {
                break;
            }

            if (rdQueue.numElem && consUsbData.readTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
            {
                 consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE;
            }
            else if (wrQueue.numElem && consUsbData.writeTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
            {
                 consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE;
            }
            else if (rdQueue.numElem && consUsbData.isReadComplete && consUsbData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
            {
                 consUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_READ;
            }
            else if (wrQueue.numElem && consUsbData.isWriteComplete && consUsbData.writeTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
            {
                 consUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE;
            }
            break;

        case CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE:

            if(consUSBReset())
            {
                break;
            }

            if (wrQueue.numElem)
            {
                consUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                pkt = wrQElements[wrQueue.tailPos];

                consUsbData.isWriteComplete = false;
                
                result = USB_DEVICE_CDC_Write(consUsbData.cdcInstance, &consUsbData.writeTransferHandle,
                     pkt.data.cbuf, pkt.sz, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);

                if (result == USB_DEVICE_CDC_RESULT_OK)
                {
                    consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE;
                }
                else
                {
                    consUsbData.state = CONSOLE_USB_CDC_STATE_OPERATIONAL_ERROR;
                }
            }
            break;

        case CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE:

            if(consUSBReset())
            {
                break;
            }

            /* Check if a character was sent. The isWriteComplete
             * flag gets updated in the CDC event handler */
            if(consUsbData.isWriteComplete)
            {
                consUsbData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
                
                if (consUsbData.wrCallback != NULL)
                {
                    consUsbData.wrCallback((void *)wrQElements[wrQueue.tailPos].data.cbuf);
                }
                popQ(&wrQueue);
                if (wrQueue.numElem == 0)
                {
                    consUsbData.state = CONSOLE_USB_CDC_STATE_READY;
                }
                else
                {
                    consUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE;
                }
            }
            else
            {
                //Go to read if there is a read queued
                if (rdQueue.numElem && consUsbData.readTransferHandle != USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                     consUsbData.state = CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE;
                }
                else if (rdQueue.numElem && consUsbData.isReadComplete && consUsbData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                     consUsbData.state = CONSOLE_USB_CDC_STATE_SCHEDULE_READ;
                }
            }
            break;

        case CONSOLE_USB_CDC_STATE_OPERATIONAL_ERROR:

            /* Clear all flags and buffers and attempt to recover */
            Console_USB_Flush();

            consUsbData.state = CONSOLE_USB_CDC_STATE_READY;

            break;
        case CONSOLE_USB_CDC_STATE_CRITICAL_ERROR:
            /* Soft crash */
            break;
        default:
            break;
    }
}

/*******************************************************************************
 End of File
 */
