/*******************************************************************************
  Console System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console_local.h

  Summary:
    Console System Service local declarations and definitions.

  Description:
    This file contains the Console System Service local declarations and definitions.
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


#ifndef _SYS_CONSOLE_LOCAL_H
#define _SYS_CONSOLE_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h> /* ssize_t */

#include "system_config.h"
#include "system/common/sys_module.h"
#include "driver/driver_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

typedef uintptr_t CONSOLE_DEVICE_HANDLE;

typedef void (*consoleCallbackFunction) (void *handle);

typedef enum
{
    SYS_CONSOLE_STATUS_NOT_CONFIGURED,

    SYS_CONSOLE_STATUS_CONFIGURED,

    SYS_CONSOLE_STATUS_BUSY,

    SYS_CONSOLE_STATUS_ERROR

} SYS_CONSOLE_STATUS;


typedef enum
{
    SYS_CONSOLE_EVENT_WRITE_COMPLETE,

    SYS_CONSOLE_EVENT_READ_COMPLETE,

} SYS_CONSOLE_EVENT;


// *****************************************************************************
// *****************************************************************************
// Section: Extern data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: APPIO Prototypes
// *****************************************************************************
// *****************************************************************************
CONSOLE_DEVICE_HANDLE Console_APPIO_Open(const SYS_MODULE_INDEX index, DRV_IO_INTENT intent);
void Console_APPIO_Close(CONSOLE_DEVICE_HANDLE consDevHandle);
ssize_t Console_APPIO_Write(int fd, const void *buf, size_t count);
ssize_t Console_APPIO_Read(int fd, void *buf, size_t count);
char Console_APPIO_ReadC(int fd);
void Console_APPIO_Tasks(SYS_MODULE_OBJ consObj);
SYS_CONSOLE_STATUS Console_APPIO_Status(void);
void Console_APPIO_RegisterCallback(consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event);
void Console_APPIO_Flush(void);

// *****************************************************************************
// *****************************************************************************
// Section: USB Prototypes
// *****************************************************************************
// *****************************************************************************
CONSOLE_DEVICE_HANDLE Console_USB_Open(const SYS_MODULE_INDEX index, DRV_IO_INTENT intent);
void Console_USB_Close(CONSOLE_DEVICE_HANDLE consDevHandle);
ssize_t Console_USB_Write(int fd, const void *buf, size_t count);
ssize_t Console_USB_Read(int fd, void *buf, size_t count);
char Console_USB_ReadC(int fd);
void Console_USB_Tasks (SYS_MODULE_OBJ consObj);
SYS_CONSOLE_STATUS Console_USB_Status (void);
void Console_USB_RegisterCallback(consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event);
void Console_USB_Flush(void);

// *****************************************************************************
// *****************************************************************************
// Section: UART Prototypes
// *****************************************************************************
// *****************************************************************************
CONSOLE_DEVICE_HANDLE Console_UART_Open(const SYS_MODULE_INDEX index, DRV_IO_INTENT intent);
void Console_UART_Close(CONSOLE_DEVICE_HANDLE consDevHandle);
ssize_t Console_UART_Write(int fd, const void *buf, size_t count);
ssize_t Console_UART_Read(int fd, void *buf, size_t count);
char Console_UART_ReadC(int fd);
void Console_UART_Tasks (SYS_MODULE_OBJ consObj);
SYS_CONSOLE_STATUS Console_UART_Status (void);
void Console_UART_RegisterCallback(consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event);
void Console_UART_Flush(void);

#endif //#ifndef _SYS_CONSOLE_LOCAL_H

/*******************************************************************************
 End of File
*/

