/*******************************************************************************
  Driver Common Header Definitions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_common.h

  Summary:
    This file defines the common macros and definitions used by the driver
    definition and implementation headers.

  Description:
    This file defines the common macros and definitions used by the driver
    definition and the implementation header.

  Remarks:
    The directory in which this file resides should be added to the compiler's
    search path for header files.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2010-2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS-IS WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _DRV_COMMON_H
#define _DRV_COMMON_H

#include <stdint.h>
#include <stddef.h>

// *****************************************************************************
/* Device Driver I/O Intent

  Summary:
    Identifies the intended usage of the device when it is opened.

  Description:
    This enumeration identifies the intended usage of the device when the
    caller opens the device. It identifies the desired behavior of the device
    driver for the following:

    * Blocking or non-blocking I/O behavior (do I/O calls such as read and write
      block until the operation is finished or do they return immediately and
      require the caller to call another routine to check the status of the
      operation)

    * Support reading and/or writing of data from/to the device

    * Identify the buffering behavior (sometimes called "double buffering" of
      the driver.  Indicates if the driver should maintain its own read/write
      buffers and copy data to/from these buffers to/from the caller's buffers.

    * Identify the DMA behavior of the peripheral

  Remarks:
    The buffer allocation method is not identified by this enumeration.  Buffers
    can be allocated statically at build time, dynamically at run-time, or
    even allocated by the caller and passed to the driver for its own usage if
    a driver-specific routine is provided for such.  This choice is left to
    the design of the individual driver and is considered part of its
    interface.

    These values can be considered "flags".  One selection from each of the
    groups below can be ORed together to create the complete value passed
    to the driver's open routine.
*/

typedef enum
{
    /* Read */
    DRV_IO_INTENT_READ               /*DOM-IGNORE-BEGIN*/ = 1 << 0 /* DOM-IGNORE-END*/,

    /* Write */
    DRV_IO_INTENT_WRITE              /*DOM-IGNORE-BEGIN*/ = 1 << 1 /* DOM-IGNORE-END*/,

    /* Read and Write*/
    DRV_IO_INTENT_READWRITE          /*DOM-IGNORE-BEGIN*/ \
            = DRV_IO_INTENT_READ|DRV_IO_INTENT_WRITE /* DOM-IGNORE-END*/,

    /* The driver will block and will return when the operation is complete */
    DRV_IO_INTENT_BLOCKING           /*DOM-IGNORE-BEGIN*/ = 0 << 2 /* DOM-IGNORE-END*/,

    /* The driver will return immediately */
    DRV_IO_INTENT_NONBLOCKING        /*DOM-IGNORE-BEGIN*/ = 1 << 2 /* DOM-IGNORE-END*/,

    /* The driver will support only one client at a time */
    DRV_IO_INTENT_EXCLUSIVE          /*DOM-IGNORE-BEGIN*/ = 1 << 3 /* DOM-IGNORE-END*/,

    /* The driver will support multiple clients at a time */
    DRV_IO_INTENT_SHARED             /*DOM-IGNORE-BEGIN*/ = 0 << 3 /* DOM-IGNORE-END*/

} DRV_IO_INTENT;


// *****************************************************************************
/* Driver Client Status

  Summary:
    Identifies the current status/state of a client's connection to a driver.

  Description:
    This enumeration identifies the current status/state of a client's link to
    a driver.

  Remarks:
    The enumeration used as the return type for the client-level status routines
    defined by each device driver or system module (for example,
    DRV_USART_ClientStatus) must be based on the values in this enumeration.
*/

typedef enum
{
    /* Indicates that a driver-specific error has occurred. */
    DRV_CLIENT_STATUS_ERROR_EXTENDED   = -10,

    /* An unspecified error has occurred.*/
    DRV_CLIENT_STATUS_ERROR            =  -1,

    /* The driver is closed, no operations for this client are ongoing,
    and/or the given handle is invalid. */
    DRV_CLIENT_STATUS_CLOSED           =   0,

    /* The driver is currently busy and cannot start additional operations. */
    DRV_CLIENT_STATUS_BUSY             =   1,

    /* The module is running and ready for additional operations */
    DRV_CLIENT_STATUS_READY            =   2,

    /* Indicates that the module is in a driver-specific ready/run state. */
    DRV_CLIENT_STATUS_READY_EXTENDED   =  10

} DRV_CLIENT_STATUS;


// *****************************************************************************
/* Device Driver Blocking Status Macro

  Summary:
    Returns if the I/O intent provided is blocking

  Description:
    This macro returns if the I/O intent provided is blocking.

  Remarks:
    None.
*/

#define DRV_IO_ISBLOCKING(intent)          (intent & DRV_IO_INTENT_BLOCKING)


// *****************************************************************************
/* Device Driver Non Blocking Status Macro

  Summary:
    Returns if the I/O intent provided is non-blocking.

  Description:
    This macro returns if the I/ intent provided is non-blocking.

  Remarks:
    None.
*/

#define DRV_IO_ISNONBLOCKING(intent)       (intent & DRV_IO_INTENT_NONBLOCKING )


// *****************************************************************************
/* Device Driver Exclusive Status Macro

  Summary:
    Returns if the I/O intent provided is non-blocking.

  Description:
    This macro returns if the I/O intent provided is non-blocking.

  Remarks:
    None.
*/

#define DRV_IO_ISEXCLUSIVE(intent)       (intent & DRV_IO_INTENT_EXCLUSIVE)


// *****************************************************************************
/* Device Driver IO Buffer Identifier

  Summary:
    Identifies to which buffer a device operation will apply.

  Description:
    This enumeration identifies to which buffer (read, write, both, or neither)
    a device operation will apply.  This is used for "flush" (or similar)
    operations.
*/

typedef enum
{
    // Operation does not apply to any buffer
    DRV_IO_BUFFER_TYPE_NONE      = 0x00,

    // Operation applies to read buffer
    DRV_IO_BUFFER_TYPE_READ      = 0x01,

    // Operation applies to write buffer
    DRV_IO_BUFFER_TYPE_WRITE     = 0x02,

    // Operation applies to both read and write buffers
    DRV_IO_BUFFER_TYPE_RW        = DRV_IO_BUFFER_TYPE_READ|DRV_IO_BUFFER_TYPE_WRITE

} DRV_IO_BUFFER_TYPES;


// *****************************************************************************
/* Device Handle

  Summary:
    Handle to an opened device driver.

  Description:
    This handle identifies the open instance of a device driver.  It must be
    passed to all other driver routines (except the initialization, deinitialization, 
    or power routines) to identify the caller.

  Remarks:
    Every application or module that wants to use a driver must first call
    the driver's open routine.  This is the only routine that is absolutely
    required for every driver.

    If a driver is unable to allow an additional module to use it, it must then
    return the special value DRV_HANDLE_INVALID.  Callers should check the
    handle returned for this value to ensure this value was not returned before
    attempting to call any other driver routines using the handle.
*/

typedef uintptr_t DRV_HANDLE;


// *****************************************************************************
/* Invalid Device Handle

 Summary:
    Invalid device handle.

 Description:
    If a driver is unable to allow an additional module to use it, it must then
    return the special value DRV_HANDLE_INVALID.  Callers should check the
    handle returned for this value to ensure this value was not returned before
    attempting to call any other driver routines using the handle.

 Remarks:
    None.
*/

#define DRV_HANDLE_INVALID  (((DRV_HANDLE) -1))


// *****************************************************************************
/* Not supported configuration

  Summary:
    Not supported configuration.

  Description:
    If the configuration option is not supported on an instance of the
    peripheral, use this macro to equate to that configuration. This option
    should be listed as a possible value in the description of that
    configuration option.
*/

#define DRV_CONFIG_NOT_SUPPORTED                    (((unsigned short) -1))


// *****************************************************************************
/* Unsupported Attribute Abstraction

  Summary:
    Abstracts the use of the unsupported attribute defined by the compiler.

  Description:
    This macro nulls the definition of the _PLIB_UNSUPPORTED macro, to support 
    compilation of the drivers for all different variants.

  Example:
    void _PLIB_UNSUPPORTED PLIB_USART_Enable(USART_MODULE_ID index);

    This function will not generate a compiler error if the interface is not 
    defined for the selected device.

 Remarks:
	None.
*/
    
#ifndef _PLIB_UNSUPPORTED

    #define _PLIB_UNSUPPORTED   

#endif

#endif //_DRV_COMMON_H

/*******************************************************************************
 End of File
*/
