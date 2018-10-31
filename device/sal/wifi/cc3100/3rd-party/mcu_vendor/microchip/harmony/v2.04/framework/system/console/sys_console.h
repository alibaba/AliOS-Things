/*******************************************************************************
  Console System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console.h

  Summary:
    Console System Service interface definitions.

  Description:
    This file contains the interface definition for the Console system
    service.  It provides a way to interact with the Console subsystem to
    manage the timing requests supported by the system.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SYS_CONSOLE_H
#define _SYS_CONSOLE_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "system_config.h"
#include "system/system.h"
#include "system/common/sys_module.h"
#include "system/common/sys_queue.h"
#include "system/console/src/sys_console_local.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  

    extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: SYS CONSOLE Data Types
// *****************************************************************************
// *****************************************************************************

/* SYS Console Module Index Numbers

  Summary:
    Console System Service index definitions.

  Description:
    These constants provide Console System Service index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

*/

#define SYS_CONSOLE_INDEX_0                 0
#define SYS_CONSOLE_INDEX_1                 1
#define SYS_CONSOLE_INDEX_2                 2
#define SYS_CONSOLE_INDEX_3                 3

/* These are in unistd.h */
#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#define CONSOLE_DEVICE_HANDLE_INVALID ((CONSOLE_DEVICE_HANDLE)(-1))

/* This is added here to prevent built errors, just in-case MHC was never run */
#ifndef SYS_CONSOLE_BUFFER_DMA_READY
#define SYS_CONSOLE_BUFFER_DMA_READY
#endif

//#ifdef SYS_CONSOLE_REMOVE_APIS 
#ifndef SYS_CONSOLE_INCLUDE_APIS

#ifndef SYS_CONSOLE_MESSAGE
    #define SYS_CONSOLE_MESSAGE(message)
#endif

#ifndef SYS_CONSOLE_PRINT
    #define SYS_CONSOLE_PRINT(fmt, ...)
#endif

#endif

// *****************************************************************************
/*  Console device enumeration

  Summary:
	Lists the available console devices.

  Description:
	This enumeration lists all of the available console devices. A console
    device is a physical peripheral used by the console service to send
    and receive data.

  Remarks:
    None.
*/
typedef enum
{
    SYS_CONSOLE_DEV_USART,

    SYS_CONSOLE_DEV_USB_CDC,

    SYS_CONSOLE_DEV_APPIO,

    SYS_CONSOLE_NR_DEVS,

} SYS_CONSOLE_DEVICE;



typedef struct
{
    SYS_CONSOLE_DEVICE consoleDevice;

    DRV_IO_INTENT intent;

    char (*sysConsoleReadC) (int fd);

    ssize_t (*sysConsoleRead) (int fd, void *buf, size_t count);

    ssize_t (*sysConsoleWrite) (int fd, const void *buf, size_t count);

    void (*sysConsoleRegisterCallback) (consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event);

    void (*sysConsoleTasks) (SYS_MODULE_OBJ object);

    SYS_CONSOLE_STATUS (*sysConsoleStatus) (void);

    void (*sysConsoleFlush) (void);

} SYS_CONSOLE_DEV_DESC;



// *****************************************************************************
/* SYS CONSOLE OBJECT INSTANCE structure

  Summary
    System Console object instance structure.

  Description:
    This data type defines the System Console object instance.

  Remarks:
    None.
*/

typedef struct
{
    /* State of this instance */
    SYS_STATUS consoleDeviceInstanceStatus;

    SYS_CONSOLE_DEV_DESC *consoleInstanceDevDesc;

    CONSOLE_DEVICE_HANDLE consoleDevHandle;

} SYS_CONSOLE_OBJECT_INSTANCE;


// *****************************************************************************
//
/* SYS Console Initialize structure

  Summary:
    Identifies the system console initialize structure.

  Description:
    This structure identifies the system console initialize structure.

  Remarks:
    None.
*/

typedef struct
{
    /* System module initialization */
    SYS_MODULE_INIT moduleInit;

    SYS_CONSOLE_DEV_DESC *consDevDesc;

} SYS_CONSOLE_INIT;


// *****************************************************************************
// *****************************************************************************
// Section: SYS CONSOLE CONTROL Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_CONSOLE_Initialize( const SYS_MODULE_INDEX index,
                                       const SYS_MODULE_INIT * const init )

  Summary:
    Initializes data for the instance of the Console module and opens the 
    specific module instance.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function initializes the Console module, and selects the I/O device to
    be used. It also initializes any internal data structures.

  Precondition:
    None.

  Parameters:
    index           - Index for the instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the Console System service. This pointer
                      may be null if no data is required because static
                      overrides have been provided.

  Returns:
    If successful, returns a valid handle to an object.  Otherwise, it
    returns SYS_MODULE_OBJ_INVALID. The returned object must be passed as
    argument to SYS_CONSOLE_Reinitialize, SYS_CONSOLE_Deinitialize, 
    SYS_CONSOLE_Tasks and SYS_CONSOLE_Status routines.

  Example:
    <code>
    SYS_MODULE_OBJ  objectHandle;

    // Populate the console initialization structure
    SYS_CONSOLE_INIT consInit =
    {
        .moduleInit = {0},
        .consDevDesc = &consUsbCdcDevDesc,
    };

    objectHandle = SYS_Console_Initialize (SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT*)&consInit);
    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine should only be called once during system initialization
    unless SYS_Console_Deinitialize is first called to deinitialize the device
    instance before reinitializing it. If the system was already initialized
    it safely returns without causing any disturbance.
*/

SYS_MODULE_OBJ SYS_CONSOLE_Initialize( const SYS_MODULE_INDEX index,
                                   const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Reinitialize( SYS_MODULE_OBJ object,
                               const SYS_MODULE_INIT * const init )

   Summary:
    Reinitializes and refreshes the data structure for the instance of the
    Console module.
	<p><b>Implementation:</b> Static/Dynamic</p>

   Description:
    This function reinitializes and refreshes the data structure for the instance
    of the Console module using the supplied data.

  Precondition:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    object          - Identifies the SYS CONSOLE Object returned by the Initialize
                      interface
    init            - Pointer to the data structure containing any data
                      necessary to initialize the hardware

   Returns:
    - true	- If successful
    - false	- If unsuccessful

   Example:
    <code>
    SYS_MODULE_OBJ  objectHandle;

    // Populate the console initialization structure
    SYS_CONSOLE_INIT consInit =
    {
        .moduleInit = {0},
        .consDevDesc = &consUsbCdcDevDesc,
    };

    SYS_CONSOLE_Reinitialize (objectHandle, (SYS_MODULE_INIT*)&consInit);
    </code>

   Remarks:
    This operation uses the same initialization data structure as the
    SYS_CONSOLE_Initialize operation. This function can be called multiple times
    to reinitialize the module.
*/

bool SYS_CONSOLE_Reinitialize( SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Deinitialize( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specific module instance of the Console module.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function deinitializes the specific module instance disabling its
    operation (and any hardware for driver modules). Resets all of the internal
    data structures and fields for the specified instance to the default settings.

  Precondition:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

   Returns:
    - true	- If successful
    - false	- If unsuccessful

  Example:
    <code>
    SYS_MODULE_OBJ      object;     //  Returned from SYS_CONSOLE_Initialize

    SYS_CONSOLE_Deinitialize (object);

    </code>

  Remarks:
    Once the Initialize operation has been called, the Deinitialize
    operation must be called before the Initialize operation can be called
    again.
*/

bool SYS_CONSOLE_Deinitialize( SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the console's state machine.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used to maintain the Console System Service internal state
    machine and implement its ISR for interrupt-driven implementations.

  Precondition:
    The SYS_CONSOLE_Initialize function must have been called for the specified
    CONSOLE driver instance.

  Parameters:
    object    - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

  Returns:
    None

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_CONSOLE_Initialize

    while (true)
    {
        SYS_CONSOLE_Tasks (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.
*/

void SYS_CONSOLE_Tasks ( SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
    SYS_STATUS SYS_CONSOLE_Status( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the Console module.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the status of the specific module instance disabling
    its operation (and any hardware for driver modules).

  Precondition:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

  Returns:
    * SYS_STATUS_READY          - Indicates that the driver is busy with a
                                  previous system level operation and cannot start
                                  another. Any value greater than SYS_STATUS_READY is
                                  also a normal running state in which the driver
                                  is ready to accept new operations.
    * SYS_STATUS_BUSY           - Indicates that the driver is busy with a
                                  previous system level operation and cannot start
                                  another.
    * SYS_STATUS_ERROR          - Indicates that the driver is in an error state.
                                  Any value less than SYS_STATUS_ERROR is
                                  also an error state.
    * SYS_MODULE_DEINITIALIZED  - Indicates that the driver has been deinitialized.
                                  This value is less than SYS_STATUS_ERROR.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_CONSOLE_Initialize
    SYS_STATUS          consStatus;

    consStatus = SYS_CONSOLE_Status (object);
    if (SYS_STATUS_ERROR >= consStatus)
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/

SYS_STATUS SYS_CONSOLE_Status( SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
    int SYS_CONSOLE_Read( int handle, char *buffer, int len )

  Summary:
    Reads data from the console device.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function reads the data from the console device.
	
  Preconditions:
    None.

  Parameters:
    index           - Console instance index
    fd              - I/O stream handle.
                      Maintained for backward compatibility.
                      NULL value can be passed as a parameter.
    buf             - Buffer to hold the read data.
    count           - Number of bytes to read.

  Returns:
    Number of bytes actually read.

  Example:
    <code>
    ssize_t nr;
    char myBuffer[MY_BUFFER_SIZE];
    nr = SYS_CONSOLE_Read( SYS_CONSOLE_INDEX_0, STDIN_FILENO, myBuffer, MY_BUFFER_SIZE );
    if (nr != MY_BUFFER_SIZE)
    {
        // handle error
    }
    </code>

  Remarks:
    None.
*/

ssize_t SYS_CONSOLE_Read(const SYS_MODULE_INDEX index, int fd, void *buf, size_t count );


// *****************************************************************************
/* Function:
    ssize_t SYS_CONSOLE_Write(const SYS_MODULE_INDEX index, int fd, const char *buf, size_t count )

  Summary:
    Writes data to the console device.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function writes data to the console device.

  Preconditions:
    None.

  Parameters:
    index           - Console instance index
    fd              - I/O stream handle.
                      Maintained for backward compatibility.
                      NULL value can be passed as a parameter.
    buf             - Buffer holding the data to be written.
    count           - Number of bytes to write.

  Returns:
    Number of bytes written or -1 if an error occurred.

  Example:
    <code>
    ssize_t nr;
    char myBuffer[] = "message";
    nr = SYS_CONSOLE_Write( SYS_CONSOLE_INDEX_0, STDOUT_FILENO, myBuffer, strlen(myBuffer) );
    if (nr != strlen(myBuffer))
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/

ssize_t SYS_CONSOLE_Write(const SYS_MODULE_INDEX index, int fd, const char *buf, size_t count);


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_RegisterCallback(const SYS_MODULE_INDEX index, 
	                     consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event)

  Summary:
    Registers a callback function with the console service that will be
    executed when the read or write queue is emptied.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used by an application to register a callback function
    with the console service. The callback function is called in response to
    an event. Separate callback functions are required for each event.

  Preconditions:
    None.

  Parameters:
    index               - Console instance index
    consCallbackFunc    - The name of the callback function
    event               - Enumerated list of events that can trigger a callback

  Returns:
    None.

  Example:
    <code>
    SYS_CONSOLE_RegisterCallback(SYS_CONSOLE_INDEX_0, APP_ReadComplete, \
                                 SYS_CONSOLE_EVENT_READ_COMPLETE);
    SYS_CONSOLE_RegisterCallback(SYS_CONSOLE_INDEX_0, APP_WriteComplete, \
                                 SYS_CONSOLE_EVENT_WRITE_COMPLETE);
    </code>

  Remarks:
    None.
*/

void SYS_CONSOLE_RegisterCallback(const SYS_MODULE_INDEX index, 
                      consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event);


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Flush(const SYS_MODULE_INDEX index)

  Summary:
    Flushes the read and write queues and resets an overflow error for the
    console.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function flushes the read and write queues and resets an overflow
    error for the console.

  Preconditions:
    None.

  Parameters:
    index               - Console instance index

  Returns:
    None.

  Example:
    <code>
    SYS_CONSOLE_Flush(SYS_CONSOLE_INDEX_0);
    </code>

  Remarks:
    None.
*/

void SYS_CONSOLE_Flush(const SYS_MODULE_INDEX index);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif //SYS_CONSOLE_H

/*******************************************************************************
 End of File
*/