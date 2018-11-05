/*******************************************************************************
  Console System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console_usb_cdc_local.h

  Summary:
    Console System Service local declarations and definitions for usb cdc I/O 
    device.

  Description:
    This file contains the Console System Service local declarations and 
    definitions for usb cdc I/O device.
*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END


#ifndef _SYS_CONSOLE_USB_CDC_LOCAL_H
#define _SYS_CONSOLE_USB_CDC_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "sys_console_local.h"
#include "usb/usb_device.h"
#include "usb/usb_device_cdc.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

#define USB_DEVICE_CDC_INDEX_0 0

// *****************************************************************************
/* SYS TMR State Machine States

   Summary
    Defines the various states that can be achieved by the driver operation.

   Description
    This enumeration defines the various states that can be achieved by the 
    driver operation.

   Remarks:
    None.
*/

typedef enum
{
    /* Application opens and attaches the device here */
    CONSOLE_USB_CDC_STATE_INIT = 0,

    /* Application waits for device configuration*/
    CONSOLE_USB_CDC_STATE_WAIT_FOR_CONFIGURATION,

    /* The application checks if a switch was pressed */
    //CONSOLE_USB_CDC_STATE_CHECK_SWITCH_PRESSED,
    CONSOLE_USB_CDC_STATE_READY,

    /* Wait for a character receive */
    CONSOLE_USB_CDC_STATE_SCHEDULE_READ,

    /* A character is received from host */
    CONSOLE_USB_CDC_STATE_WAIT_FOR_READ_COMPLETE,

    /* Wait for the TX to get completed */
    CONSOLE_USB_CDC_STATE_SCHEDULE_WRITE,

    /* Wait for the write to complete */
    CONSOLE_USB_CDC_STATE_WAIT_FOR_WRITE_COMPLETE,

    /* Application Non-Recoverable Error state*/
    CONSOLE_USB_CDC_STATE_CRITICAL_ERROR,

    /* Application Recoverable Error state*/
    CONSOLE_USB_CDC_STATE_OPERATIONAL_ERROR

} CONSOLE_USB_CDC_STATE;


typedef struct
{
    /* Device layer handle returned by device layer open function */
    USB_DEVICE_HANDLE deviceHandle;

    USB_DEVICE_CDC_INDEX cdcInstance;

    /* Application's current state*/
    CONSOLE_USB_CDC_STATE state;

    /* Device configured state */
    bool isConfigured;

    /* Read Data Buffer */
    uint8_t readBuffer[64];

    /* Set Line Coding Data */
    USB_CDC_LINE_CODING setLineCodingData;

    /* Get Line Coding Data */
    USB_CDC_LINE_CODING getLineCodingData;

    /* Control Line State */
    USB_CDC_CONTROL_LINE_STATE controlLineStateData;

    /* Break data */
    uint16_t breakData;

    /* Read transfer handle */
    USB_DEVICE_CDC_TRANSFER_HANDLE readTransferHandle;

    /* Write transfer handle */
    USB_DEVICE_CDC_TRANSFER_HANDLE writeTransferHandle;

    /* True if a character was read */
    bool isReadComplete;

    /* True if a character was written*/
    bool isWriteComplete;

    /* Set when an attempt is made to push to a full write queue */
    bool overflowFlag;

    /* Called when read queue is emptied */
    void (*rdCallback)(void *handle);

    /* Called when write is completed */
    void (*wrCallback)(void *handle);

} CONS_USB_CDC_DATA;

struct QPkt
{
    union
    {
        const void *cbuf;
        void *buf;
    } data;
    size_t sz;
};


struct Queue
{
    uint32_t tailPos;
    uint32_t nextPos;
    uint32_t numElem;
    struct QPkt *qPkts;
    uint32_t elemArrSz;
};

// *****************************************************************************
// *****************************************************************************
// Section: Extern data Definitions
// *****************************************************************************
// *****************************************************************************

void APP_USBDeviceEventCallBack(USB_DEVICE_EVENT events,
        void * eventData, uintptr_t context);

void USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index ,
    USB_DEVICE_CDC_EVENT event ,
    void * pData,
    uintptr_t userData
);


#endif //#ifndef _SYS_CONSOLE_USB_CDC_LOCAL_H

/*******************************************************************************
 End of File
*/

