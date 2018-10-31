/*******************************************************************************
  Console System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console.c

  Summary:
    Console System Service interface implementation.

  Description:
    The CONSOLE system service provides a simple interface to manage the CONSOLE
    module on Microchip microcontrollers. This file Implements the core
    interface routines for the CONSOLE system service. While building the system
    service from source, ALWAYS include this file in the build.
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
// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Macro: _PLIB_UNSUPPORTED

  Summary:
    Macro definition to avoid 'unsupported' warnings.

  Description:
    This macro should be defined to avoid unsupported warnings.

*/

#define _PLIB_UNSUPPORTED

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system/console/sys_console.h"

#if ( SYS_CONSOLE_DEVICE_MAX_INSTANCES == 0 )
    #error( " SYS_CONSOLE_DEVICE_MAX_INSTANCES must be > 0 ");
#endif

static SYS_CONSOLE_OBJECT_INSTANCE consoleDeviceInstance[SYS_CONSOLE_DEVICE_MAX_INSTANCES];


// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_CONSOLE_Initialize( const SYS_MODULE_INDEX index,
                                       const SYS_MODULE_INIT * const init )

  Summary:
    Initializes data for the instance of the Console module and opens the 
    specific module instance.

  Description:
    This function initializes the Console module, and selects the I/O device to
    be used. It also initializes any internal data structures.

  Precondition:
    None.

  Parameters:
    index           - Index for the instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the sys console. This pointer may be null if no
                      data is required because static overrides have been
                      provided.

 Returns:
    If successful, returns a valid handle to an object.  Otherwise, it
    returns SYS_MODULE_OBJ_INVALID. The returned object must be passed as
    argument to SYS_CONSOLE_Reinitialize, SYS_CONSOLE_Deinitialize, 
    SYS_CONSOLE_Tasks and SYS_CONSOLE_Status routines.

  Remarks:
    This routine should only be called once during system initialization
    unless SYS_Console_Deinitialize is first called to deinitialize the device
    instance before reinitializing it. If the system was already initialized
    it safely returns without causing any disturbance.
*/

SYS_MODULE_OBJ SYS_CONSOLE_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init )
{
    SYS_CONSOLE_INIT *initConfig = (SYS_CONSOLE_INIT*)init;

    /* Confirm valid arguments */
    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES || init == NULL)
        return SYS_MODULE_OBJ_INVALID;

    if ((consoleDeviceInstance[index].consoleDeviceInstanceStatus == SYS_STATUS_UNINITIALIZED) && initConfig)
    {
        consoleDeviceInstance[index].consoleDevHandle = CONSOLE_DEVICE_HANDLE_INVALID;

        consoleDeviceInstance[index].consoleInstanceDevDesc = initConfig->consDevDesc;
        consoleDeviceInstance[index].consoleDeviceInstanceStatus = SYS_STATUS_READY;

        return index;
    }

    return SYS_MODULE_OBJ_INVALID;
}


// *****************************************************************************
/* Function:
     void SYS_CONSOLE_Reinitialize ( SYS_MODULE_OBJ object,
                                 const SYS_MODULE_INIT * const init )

   Summary:
     Reinitializes and refreshes the data structure for the instance of the
     Console module.

   Description:
     This function reinitializes and refreshes the data structure for the
     instance of the Console module.

  PreCondition:
     The SYS_CONSOLE_Initialize function should have been called before calling
     this function.

  Parameters:
    object          - Identifies the SYS CONSOLE Object returned by the Initialize
                      interface
    init            - Pointer to the data structure containing any data
                      necessary to initialize the hardware

   Returns:
    - true	- if successful
    - false	- if unsuccessful
*/

bool SYS_CONSOLE_Reinitialize( SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init )
{
    SYS_MODULE_INDEX index = (SYS_MODULE_INDEX)object;

    /* Confirm valid arguments */
    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES || init == NULL)
        return false;
    
    if (!SYS_CONSOLE_Deinitialize(object))
        return false;

    if (SYS_CONSOLE_Initialize(index, init) == SYS_MODULE_OBJ_INVALID)
        return false;

    return true;
}


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specific module instance of the Console module.

  Description:
    Deinitializes the specific module instance disabling its operation (and
    any hardware for driver modules). Resets all the internal data
    structures and fields for the specified instance to the default
    settings.

  PreCondition:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

   Returns:
    - true	- if successful
    - false	- if unsuccessful
*/

bool SYS_CONSOLE_Deinitialize ( SYS_MODULE_OBJ object )
{
    SYS_MODULE_INDEX index = (SYS_MODULE_INDEX)object;

    /* Confirm valid argument*/
    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES)
        return false;

    consoleDeviceInstance[index].consoleDevHandle = CONSOLE_DEVICE_HANDLE_INVALID;
    consoleDeviceInstance[index].consoleDeviceInstanceStatus = SYS_STATUS_UNINITIALIZED;

    return true;
}


// *****************************************************************************
/* Function:
    SYS_STATUS SYS_CONSOLE_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific module instance of the Console module.

  Description:
    This function returns the status of the specific module instance disabling its
    operation (and any hardware for driver modules).

  PreCondition:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    object   - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

  Returns:
    SYS_STATUS_READY    Indicates that any previous module operation for the
                        specified module has completed

    SYS_STATUS_BUSY     Indicates that a previous module operation for the
                        specified module has not yet completed

    SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

SYS_STATUS SYS_CONSOLE_Status ( SYS_MODULE_OBJ object )
{
    SYS_STATUS ret = SYS_STATUS_ERROR;
    SYS_CONSOLE_STATUS status;
    SYS_MODULE_INDEX index = (SYS_MODULE_INDEX)object;

    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES)
        return SYS_STATUS_ERROR;

    if (consoleDeviceInstance[index].consoleInstanceDevDesc == NULL)
    {
        return SYS_STATUS_UNINITIALIZED;
    }

    status = consoleDeviceInstance[index].consoleInstanceDevDesc->sysConsoleStatus();

    switch (status)
    {
        case SYS_CONSOLE_STATUS_NOT_CONFIGURED:
            ret = SYS_STATUS_UNINITIALIZED;
            break;
        case SYS_CONSOLE_STATUS_CONFIGURED:
            ret = SYS_STATUS_READY;
            break;
        case SYS_CONSOLE_STATUS_BUSY:
            ret = SYS_STATUS_BUSY;
            break;
        case SYS_CONSOLE_STATUS_ERROR:
            ret = SYS_STATUS_ERROR;
            break;
        default:
            break;
    }

    return ret;
}


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the system console's state machine and implements its ISR.

  Description:
    This routine is used to maintain the system console's internal state machine 
    and implement its ISR for interrupt-driven implementations.

  Precondition:
    The SYS_CONSOLE_Initialize function must have been called for the specified 
    console instance.

  Parameters:
    object   - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

  Returns:
    None.
*/

void SYS_CONSOLE_Tasks ( SYS_MODULE_OBJ object )
{
    if (consoleDeviceInstance[(SYS_MODULE_INDEX)object].consoleInstanceDevDesc == NULL)
    {
        return;
    }

    consoleDeviceInstance[(SYS_MODULE_INDEX)object].consoleInstanceDevDesc->sysConsoleTasks(object);
}

// *****************************************************************************
/* Function:
    ssize_t SYS_CONSOLE_Read( const SYS_MODULE_INDEX index, int fd, void *buf, size_t count )

  Summary:
    Reads data from the console device.

  Description:
    This function reads the data from the console device.

  Preconditions:
    None.

  Parameters:
    index           - Console instance index
    fd              - I/O stream handle.
    buf             - Buffer to hold the read data.
    count           - Number of bytes to read.

  Returns:
    Number of bytes actually read.

  Remarks:
    None.
*/

ssize_t SYS_CONSOLE_Read(const SYS_MODULE_INDEX index, int fd, void *buf, size_t count )
{
    if (consoleDeviceInstance[index].consoleDeviceInstanceStatus == SYS_STATUS_UNINITIALIZED
            || consoleDeviceInstance[index].consoleInstanceDevDesc == NULL)
    {
        return 0;
    }

    return consoleDeviceInstance[index].consoleInstanceDevDesc->sysConsoleRead(fd, buf, count);
}


// *****************************************************************************
/* Function:
    ssize_t SYS_CONSOLE_Write( const SYS_MODULE_INDEX index, int fd, const void *buffer, size_t count )

  Summary:
    Writes data to the console device.

  Description:
    This function writes data to the console device.

  Preconditions:
    None.

  Parameters:
    index           - Console instance index
    fd              - I/O stream handle.
    buf             - Buffer holding the data to be written.
    count           - Number of bytes to write.

  Returns:
    Number of bytes written or -1 if error.

  Remarks:
    None.
*/

ssize_t SYS_CONSOLE_Write(const SYS_MODULE_INDEX index, int fd, const char *buf, size_t count )
{
    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES)
        return 0;

    if (consoleDeviceInstance[index].consoleDeviceInstanceStatus == SYS_STATUS_UNINITIALIZED
            || consoleDeviceInstance[index].consoleInstanceDevDesc == NULL)
    {
        return 0;
    }

    return consoleDeviceInstance[index].consoleInstanceDevDesc->sysConsoleWrite(fd, buf, count);
}


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_RegisterCallback(const SYS_MODULE_INDEX index, consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event)

  Summary:
    Registers a callback function with the console service that will be
    executed when the read or write queue is emptied.

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

  Remarks:
    None.
*/

void SYS_CONSOLE_RegisterCallback(const SYS_MODULE_INDEX index, consoleCallbackFunction cbFunc, SYS_CONSOLE_EVENT event)
{
    if (consoleDeviceInstance[index].consoleInstanceDevDesc == NULL)
    {
        return;
    }

    consoleDeviceInstance[index].consoleInstanceDevDesc->sysConsoleRegisterCallback(cbFunc, event);
}


// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Flush(const SYS_MODULE_INDEX index)

  Summary:
    Flushes the read and write queues and resets an overflow error for the
    console.

  Description:
    This function flushes the read and write queues and resets an overflow
    error for the console.

  Preconditions:
    None.

  Parameters:
    index               - Console instance index

  Returns:
    None.

  Remarks:
    None.
*/

void SYS_CONSOLE_Flush(const SYS_MODULE_INDEX index)
{
    if (consoleDeviceInstance[index].consoleInstanceDevDesc == NULL)
    {
        return;
    }

    consoleDeviceInstance[index].consoleInstanceDevDesc->sysConsoleFlush();
}

                      
/*******************************************************************************
 End of File
*/

