/**************************************************************************
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_miim.h

  Summary:
    MIIM Device Driver Interface File

  Description:
    MIIM Device Driver Interface
    
    The MIIM device driver provides a simple interface to manage
    an MIIM peripheral using MIIM (SMI)interface.
    This file defines the interface definitions and prototypes for the MIIM driver.                                                         
  **************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c)2016 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_MIIM_H
#define _DRV_MIIM_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system/common/sys_common.h"
#include "system/common/sys_module.h"

#include "driver/driver_common.h"
#if defined (__PIC32C__) 
#include "arch/arm/devices_pic32c.h" /* PIC32C system header. */
#else
#include "peripheral/eth/plib_eth.h"
#endif



// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* MIIM Driver Module Index Numbers

  Summary:
    MIIM driver index definitions.

  Description:
    These constants provide the MIIM driver index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

    These values should be passed into the DRV_MIIM_Initialize and
    DRV_MIIM_Open routines to identify the driver instance in use.
*/

#define DRV_MIIM_INDEX_0         0

// *****************************************************************************
/* MIIM Driver Module Index Count

  Summary:
    Number of valid MIIM driver indices.

  Description:
    This constant identifies the number of valid MIIM driver indices.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is derived from part-specific header files defined as part of the
    peripheral libraries.
*/

#define DRV_MIIM_INDEX_COUNT     1


// *****************************************************************************
/* MIIM Driver Operation Result
 *
  Summary:
    Defines the possible results of operations that can succeed or
    fail

  Description:
    MIIM Driver Operation Result Codes
    
    This enumeration defines the possible results of any of the MIIM
    driver operations that have the possibility of failing. This result
    should be checked to ensure that the operation achieved the desired
    result.                                                                
*/

typedef enum
{
    /* Everything ok */
    DRV_MIIM_RES_OK                     /*DOM-IGNORE-BEGIN*/ =  0, /*DOM-IGNORE-END*/

    /* Operation pending, in progress */
    DRV_MIIM_RES_PENDING                /*DOM-IGNORE-BEGIN*/ =  1, /*DOM-IGNORE-END*/

    /* When Init Operation was performed the ETH module was not enabled */
    DRV_MIIM_RES_INIT_WARNING           /*DOM-IGNORE-BEGIN*/ =  2, /*DOM-IGNORE-END*/

    /* Error codes < 0 */

    /* Passed handle is invalid */
    DRV_MIIM_RES_HANDLE_ERR             /*DOM-IGNORE-BEGIN*/ =  -1, /*DOM-IGNORE-END*/

    /* Passed operation handle is invalid */
    DRV_MIIM_RES_OP_HANDLE_ERR          /*DOM-IGNORE-BEGIN*/ =  -2, /*DOM-IGNORE-END*/

    /* Passed callback handle is invalid */
    DRV_MIIM_RES_CALLBACK_HANDLE_ERR    /*DOM-IGNORE-BEGIN*/ =  -3, /*DOM-IGNORE-END*/

    /* The MIIM object hasn't been properly setup. Cannot start an operation */
    DRV_MIIM_RES_SETUP_ERR              /*DOM-IGNORE-BEGIN*/ =  -4, /*DOM-IGNORE-END*/
    
    /* Register value not supported */
    DRV_MIIM_RES_REGISTER_ERR           /*DOM-IGNORE-BEGIN*/ =  -5, /*DOM-IGNORE-END*/
    
    /* PHY address value not supported */
    DRV_MIIM_RES_ADDRESS_ERR            /*DOM-IGNORE-BEGIN*/ =  -6, /*DOM-IGNORE-END*/
    
    /* Wrong parameter value */
    DRV_MIIM_RES_PARAMETER_ERR          /*DOM-IGNORE-BEGIN*/ =  -7, /*DOM-IGNORE-END*/
    
    /* All MIIM operations are in use. A new one could not be started */
    DRV_MIIM_RES_OP_UNAVAILABLE_ERR     /*DOM-IGNORE-BEGIN*/ =  -8, /*DOM-IGNORE-END*/

    /* A MIIM scan operation is ongoing. A new operation could not be started */
    DRV_MIIM_RES_OP_SCAN_ERR            /*DOM-IGNORE-BEGIN*/ =  -9, /*DOM-IGNORE-END*/

    /* A MIIM operation has timed out and it's going to be discarded */
    DRV_MIIM_RES_OP_TIMEOUT_ERR         /*DOM-IGNORE-BEGIN*/ =  -10, /*DOM-IGNORE-END*/


    /* Internal MIIM driver error has occurred. Should not happen */
    DRV_MIIM_RES_OP_INTERNAL_ERR        /*DOM-IGNORE-BEGIN*/ =  -20, /*DOM-IGNORE-END*/

} DRV_MIIM_RESULT; 

// *****************************************************************************
/* MIIM Driver Client Status

  Summary:
    Identifies the client-specific status of the MIIM driver.

  Description:
    This enumeration identifies the client-specific status of the MIIM driver.

  Remarks:
    None.
*/

typedef enum
{
    /* Unspecified error condition. Client does not exist */
    DRV_MIIM_CLIENT_STATUS_ERROR   /*DOM-IGNORE-BEGIN*/ = DRV_CLIENT_STATUS_ERROR /*DOM-IGNORE-END*/,

    /* Up and running, can accept operations */
    DRV_MIIM_CLIENT_STATUS_READY   /*DOM-IGNORE-BEGIN*/ = DRV_CLIENT_STATUS_READY /*DOM-IGNORE-END*/

} DRV_MIIM_CLIENT_STATUS;

// *****************************************************************************
/*
  Type:
    DRV_MIIM_OPERATION_HANDLE

  Summary:
    MIIM operation handle.

  Description:
    A handle that identifies an operation started by a client.
    This handle can be used by the client to query the operation status, result, etc.
    It is also used when the operation complete notification occurs.

  Remarks:
    A valid operation handle is not NULL.
    An invalid operation handle == 0.

*/
typedef const void* DRV_MIIM_OPERATION_HANDLE;


// *****************************************************************************
/* MIIM Driver Operation flags

  Summary:
    List of flags that apply to a client operation.

  Description:
    This enumeration identifies the operation-specific flags supported by the MIIM driver.

  Remarks:
    Currently only 8 bit flags are supported.
    
    Multiple flags can be simultaneously set.
*/

typedef enum
{
    /* No flag specified */
    DRV_MIIM_OPERATION_FLAG_NONE   /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/,

    /* Upon completion discard the operation result. */
    /* The client will not poll to check the result nor will need notification */
    /* This allows dummy operations, discarded as they complete */
    DRV_MIIM_OPERATION_FLAG_DISCARD  /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/,

    /* Other flags could be eventually added */


} DRV_MIIM_OPERATION_FLAGS;

// *****************************************************************************
/*
  Type:
    MIIM Callback Registration handle

  Summary:
    Handle that identifies a client registration operation.


  Description:
    A handle that a client obtains when calling DRV_MIIM_RegisterCallback.
    It can be used to deregister the notification callback: DRV_MIIM_DeregisterCallback 

  Remarks:
    A valid registration handle is not NULL.
    An invalid registration handle == 0.
*/


typedef const void* DRV_MIIM_CALLBACK_HANDLE;


// *****************************************************************************
/* Type:
	MIIM Driver Operation Complete Callback

  Summary:
    Notification function that will be called when a MIIM operation is completed
    and the driver client needs to be notified.
   
  Description:
    The format of an operation callback notification function registered with the MIIM driver.
	
  Parameters:
    cliHandle   - the client handle.
                  This is the handle that identifies the client (obtained with DRV_MIIM_Open)
                  that initiated the operation.

    opHandle    - the operation handle.
                  This is the handle that identifies the operation (obtained with DRV_MIIM_Read, DRV_MIIM_Write, etc.)
                  
    opResult    - operation result
                  DRV_MIIM_RES_OK if operation completed successfully, otherwise an error code

    opData      - operation specific data, only if the result is DRV_MIIM_RES_OK
                  For read/scan operation this is the MIIM read data.
                  For write operation this is that data that was written with MIIM.
  Remarks:
    None.

*/
typedef void    (*DRV_MIIM_OPERATION_CALLBACK)(DRV_HANDLE cliHandle, DRV_MIIM_OPERATION_HANDLE opHandle, DRV_MIIM_RESULT opResult, uint16_t opData);


// *****************************************************************************
/* MIIM Device Driver Initialization Data

  Summary:
    Contains all the data necessary to initialize the MIIM device.

  Description:
    This data structure contains all the data necessary to initialize the MIIM
    device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the DRV_MIIM_Initialize routine.
*/

typedef struct DRV_MIIM_INIT 
{
    /* System module initialization */
    SYS_MODULE_INIT             moduleInit;

    /* Identifies peripheral (PLIB-level) ID */
    uintptr_t               ethphyId;

} DRV_MIIM_INIT;


// *****************************************************************************
/* MIIM Driver Set up flags

  Summary:
    List of flags that apply to a client setup operation.

  Description:
    This enumeration identifies the setup specific flags supported by the MIIM driver.

  Remarks:
    Multiple flags can be simultaneously set.
*/

typedef enum
{
    /* No flag specified. Default value */
    DRV_MIIM_SETUP_FLAG_NONE                    /*DOM-IGNORE-BEGIN*/ = 0x0000 /*DOM-IGNORE-END*/,

    /* Suppress the normal 32 bit MIIM preamble field. Some PHYs support suppressed preamble */
    DRV_MIIM_SETUP_FLAG_PREAMBLE_SUPPRESSED     /*DOM-IGNORE-BEGIN*/ = 0x0001 /*DOM-IGNORE-END*/,

    /* Include the 32 bit MIIM preamble field. Default operation. */
    DRV_MIIM_SETUP_FLAG_PREAMBLE_DEFAULT        /*DOM-IGNORE-BEGIN*/ = 0x0000 /*DOM-IGNORE-END*/,

    /* Scan operation will read across a range of PHY addresses */
    /* Scan will start with address 1 through the address set in the scan operation */
    DRV_MIIM_SETUP_FLAG_SCAN_ADDRESS_INCREMENT  /*DOM-IGNORE-BEGIN*/ = 0x0002 /*DOM-IGNORE-END*/,

    /* Scan operation will read just one PHY address. Default operation. */
    DRV_MIIM_SETUP_FLAG_SCAN_ADDRESS_DEFAULT    /*DOM-IGNORE-BEGIN*/ = 0x0000 /*DOM-IGNORE-END*/,


    /* Other flags could be eventually added */


} DRV_MIIM_SETUP_FLAGS;

// *****************************************************************************
/* MIIM Device Driver Set up Data

  Summary:
    Contains all the data necessary to set up the MIIM device.

  Description:
    This data structure contains all the data necessary to configure the MIIM
    device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the DRV_MIIM_Setup routine.
*/

typedef struct
{
    /* The clock frequency on which this MIIM module operates on, Hz */
    uint32_t    hostClockFreq;

    /* The MIIM bus maximum supported frequency, Hz */
    /* This is a maximum value. The actual generated value may differ. */
    uint32_t    maxBusFreq;

    /* Setup flags */
    DRV_MIIM_SETUP_FLAGS    setupFlags;
} DRV_MIIM_SETUP;



// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - System Level
// *****************************************************************************
// *****************************************************************************


/*****************************************************************************************
  Function:
       SYS_MODULE_OBJ DRV_MIIM_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init )
    
  Summary:
    Initializes the MIIM driver.

  Description:
    This function initializes the MIIM driver, making it ready for
    clients to open and use it.

  Precondition:
    None.

  Parameters:
    - drvIndex  - Index for the driver instance to be initialized
    - init      - Pointer to a data structure containing any data necessary to
                  initialize the driver. This pointer may be null if no data
                  is required because static overrides have been provided.

  Returns:
    - a valid handle to a driver object, if successful.
    - SYS_MODULE_OBJ_INVALID if initialization failed.
    
  Example:
    <code>
    </code>

  Remarks:
    - This function must be called before any other MIIM routine is
      called.
    - This function should only be called once during system initialization
      unless DRV_MIIM_Deinitialize is called to deinitialize the driver instance.                                                
    - The returned object must be passed as argument to DRV_MIIM_Reinitialize,
      DRV_MIIM_Deinitialize, DRV_MIIM_Tasks and DRV_MIIM_Status routines.

  *****************************************************************************************/

SYS_MODULE_OBJ DRV_MIIM_Initialize (const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void DRV_MIIM_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init )

  Summary:
    Reinitializes the driver and refreshes any associated hardware settings.

  Description:
    This function reinitializes the driver and refreshes any associated hardware
    settings using the initialization data given, but it will not interrupt any
    ongoing operations.

  Precondition:
    The DRV_MIIM_Initialize function must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    - object          - Driver object handle, returned from the DRV_MIIM_Initialize
                        routine
    - init            - Pointer to the initialization data structure

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    - This function can be called multiple times to reinitialize the module.
    - This operation can be used to refresh any supported hardware registers as
      specified by the initialization data or to change the power state of the
      module.

    - This function is currently NOT IMPLEMENTED.   
*/

void DRV_MIIM_Reinitialize (SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void DRV_MIIM_Deinitialize (SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specified instance of the MIIM driver module.

  Description:
    This function deinitializes the specified instance of the MIIM driver
    module, disabling its operation (and any hardware)and invalidates all of the
    internal data.

  Precondition:
    The DRV_MIIM_Initialize function must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    - object    - Driver object handle, returned from DRV_MIIM_Initialize

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    - Once the Initialize operation has been called, the Deinitialize operation
      must be called before the Initialize operation can be called again.
*/

void DRV_MIIM_Deinitialize (SYS_MODULE_OBJ object );


/**************************************************************************
  Function:
       SYS_STATUS DRV_MIIM_Status (SYS_MODULE_OBJ object )
    
  Summary:
    Provides the current status of the MIIM driver module.

  Description:
    This function provides the current status of the MIIM driver
    module.

  Precondition:
    The DRV_MIIM_Initialize function must have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from DRV_MIIM_Initialize

  Returns:
    - SYS_STATUS_READY  - Indicates that any previous module operation for the
                          specified module has completed
    - SYS_STATUS_BUSY   - Indicates that a previous module operation for the
                          specified module has not yet completed
    - SYS_STATUS_ERROR  - Indicates that the specified module is in an error state

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_MIIM_Initialize
    SYS_STATUS          status;
    
    status = DRV_MIIM_Status(object);
    if (SYS_STATUS_ERROR >= status)
    {
        // Handle error
    }
    </code>

  Remarks:
    - Any value greater than SYS_STATUS_READY is also a normal running state
      in which the driver is ready to accept new operations.
    - SYS_STATUS_BUSY - Indicates that the driver is busy with a previous
      system level operation and cannot start another
    - SYS_STATUS_ERROR - Indicates that the driver is in an error state
    - Any value less than SYS_STATUS_ERROR is also an error state.
    - SYS_MODULE_DEINITIALIZED - Indicates that the driver has been
      deinitialized
    - If the status operation returns SYS_STATUS_BUSY, the a previous system level
      operation has not yet completed. Once the status operation returns
      SYS_STATUS_READY, any previous operations have completed.
    - The value of SYS_STATUS_ERROR is negative (-1). Any value less than
      that is also an error state.
    - This function will NEVER block waiting for hardware.
    - If the Status operation returns an error value, the error may be
      cleared by calling the reinitialize operation. If that fails, the
      deinitialize operation will need to be called, followed by the
      initialize operation to return to normal operations.                   
  **************************************************************************/

SYS_STATUS DRV_MIIM_Status (SYS_MODULE_OBJ object );


/***************************************************************************
  Function:
       void DRV_MIIM_Tasks(SYS_MODULE_OBJ object )
    
  Summary:
    Maintains the driver's state machine.

  Description:
    This function is used to maintain the driver's internal state machine.

  Precondition:
    The DRV_MIIM_Initialize routine must have been called for the
    specified MIIM driver instance.

  Parameters:
    - object -  Object handle for the specified driver instance (returned from
                DRV_MIIM_Initialize)
  Returns:
    None
  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_MIIM_Initialize
    
    while (true)
    {
        DRV_MIIM_Tasks (object);
    
        // Do other tasks
    }
    </code>

  Remarks:
    - This function is normally not called directly by an application. It is
      called by the system's Tasks routine (SYS_Tasks)
    - This function will never block or access any resources that may cause
      it to block.                        
  ***************************************************************************/

void DRV_MIIM_Tasks(SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_MIIM_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent )

  Summary:
    Opens the specified MIIM driver instance and returns a handle to it.

  Description:
    This function opens the specified MIIM driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.

  Precondition:
    The DRV_MIIM_Initialize function must have been called before calling this
    function.

  Parameters:
    - drvIndex    - Identifier for the object instance to be opened
    - intent      - Zero or more of the values from the enumeration
                    DRV_IO_INTENT ORed together to indicate the intended use
                    of the driver

  Returns:
    - valid open-instance handle if successful (a number
      identifying both the caller and the module instance).
    - DRV_HANDLE_INVALID if an error occurs

  Example:
    <code>
    DRV_HANDLE  handle;

    handle = DRV_MIIM_Open(DRV_MIIM_INDEX_0, 0);
    if (DRV_HANDLE_INVALID == handle)
    {
        // Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_MIIM_Close routine is called.

    This function will NEVER block waiting for hardware.

*/

DRV_HANDLE DRV_MIIM_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent  );

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************

/**************************************************************************
  Function:
       DRV_MIIM_RESULT DRV_MIIM_Setup(DRV_HANDLE  handle, const DRV_MIIM_SETUP* pSetUp)
    
  Summary:
    Sets up a MIIM client.

  Description:
    This function performs the set up of a MIIM client.
    It programs the MIIM operation using the supplied frequencies.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid device handle.

  Parameters:
    - handle -     A valid open instance handle, returned from the driver's open routine

    - pSetUp -     A pointer to an DRV_MIIM_SETUP setup data structure 

  Returns:
    - DRV_MIIM_RES_OK if the setup operation has been performed successfully
    - an DRV_MIIM_RESULT error code if the set up procedure failed.

  Example:
    <code>
    </code>

  Remarks:
    None.

  **************************************************************************/

DRV_MIIM_RESULT DRV_MIIM_Setup(DRV_HANDLE  handle, const DRV_MIIM_SETUP* pSetUp);

// *****************************************************************************
/* Function:
    void DRV_MIIM_Close(DRV_HANDLE handle )

  Summary:
    Closes an opened instance of the MIIM driver.

  Description:
    This function closes an opened instance of the MIIM driver, invalidating
    the handle.

  Precondition:
    The DRV_MIIM_Initialize routine must have been called for the specified
    MIIM driver instance.

    DRV_MIIM_Open must have been called to obtain a valid opened device handle.

  Parameters:
    - handle     - A valid open instance handle, returned from the driver's open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_MIIM_Open

    DRV_MIIM_Close(handle);
    </code>

  Remarks:
    - After calling this routine, the handle passed in "handle" must not be used
      with any of the remaining driver routines.  A new handle must be obtained by
      calling DRV_MIIM_Open before the caller may use the driver again.
    - Usually there is no need for the driver client to verify that the Close
      operation has completed.
*/

void DRV_MIIM_Close(DRV_HANDLE handle );


/***************************************************************************
  Function:
       DRV_MIIM_CLIENT_STATUS DRV_MIIM_ClientStatus(DRV_HANDLE handle )
    
  Summary:
    Gets the current client-specific status the MIIM driver.

  Description:
    This function gets the client-specific status of the MIIM
    driver associated with the given handle.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device
      handle.

  Parameters:
    - handle -  A valid instance handle, returned from the driver's open routine

  Returns:
    - DRV_MIIM_CLIENT_STATUS_READY - if the client handle represents a valid MIIM client
    - DRV_MIIM_CLIENT_STATUS_ERROR - if the client handle is an invalid MIIM client

  Example:
    <code>
    </code>

  Remarks:
    This function can be used to check that a client handle points to 
    a valid MIIM client.
    The MIIM driver queues operations so it will always return 
    DRV_MIIM_CLIENT_STATUS_READY.

  ***************************************************************************/

DRV_MIIM_CLIENT_STATUS DRV_MIIM_ClientStatus(DRV_HANDLE handle );

/***************************************************************************
  Function:
       DRV_MIIM_CALLBACK_HANDLE DRV_MIIM_RegisterCallback(DRV_HANDLE handle, DRV_MIIM_OPERATION_CALLBACK cbFunction, DRV_MIIM_RESULT* pRegResult);
    
  Summary:
    Registers an notification callback function for the client operations.

  Description:
    This function registers a client callback function.
    The function will be called by the MIIM driver when a scheduled
    operation is completed.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device
      handle.

  Parameters:
    - handle        - A valid instance handle, returned from the driver's open routine
    - cbFunction    - notification function to be called upon the operation completion
    - pRegResult    - address to store the registration result.
                      Can be NULL, if not needed.

  Returns:
    - a valid DRV_MIIM_CALLBACK_HANDLE if the operation succeeded.
    - NULL otherwise


  Example:
    <code>
    </code>

  Remarks:
    There is only one notification callback function available per client.
    To register a new callback function use DRV_MIIM_DeregisterCallback first.

  ***************************************************************************/

DRV_MIIM_CALLBACK_HANDLE DRV_MIIM_RegisterCallback(DRV_HANDLE handle, DRV_MIIM_OPERATION_CALLBACK cbFunction, DRV_MIIM_RESULT* pRegResult );

/***************************************************************************
  Function:
       DRV_MIIM_RESULT DRV_MIIM_DeregisterCallback(DRV_HANDLE handle, DRV_MIIM_CALLBACK_HANDLE cbHandle);
    
  Summary:
    Deregisters an notification callback function for the client operations.

  Description:
    This function deregisters a previously registered client notification callback function.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device
      handle.

  Parameters:
    - handle    - a valid client handle, returned from the driver's open routine
    - cbHandle - a callback handle obtained by calling DRV_MIIM_RegisterCallback()

  Returns:
    - DRV_MIIM_RES_OK if the operation succeeded.
    - an error code otherwise


  Example:
    <code>
    </code>

  Remarks:
    There is only one notification callback function available per client.
    To register a new callback function use DRV_MIIM_DeregisterCallback first.

  ***************************************************************************/

DRV_MIIM_RESULT DRV_MIIM_DeregisterCallback(DRV_HANDLE handle, DRV_MIIM_CALLBACK_HANDLE cbHandle);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client SMI/MIIM operations
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Read(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);
  
  Summary:
    Initiates a SMI/MIIM read transaction.

  Description:
    This function initiates a SMI/MIIM read transaction for a given MIIM register.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device
      handle.

  Parameters:
    - handle    - client's driver handle (returned from DRV_MIIM_Open)
    - rIx       - MIIM register to be accessed
    - phyAdd    - MIIM PHY address to use
    - opFlags   - additional operation flags
                  See the DRV_MIIM_OPERATION_FLAGS definition.
    - pOpResult - address to store the operation result
                  Could be NULL if not needed.

  Returns:
    A not NULL DRV_MIIM_OPERATION_HANDLE if the operation was successfully scheduled.
    NULL if the operation failed. More details in pOpResult.


  Example:
    <code>
    </code>

  Remarks:

    If operation was scheduled successfully, the result will be DRV_MIIM_RES_PENDING.
    Otherwise an error code will be returned.

    Upon the operation completion:
        - If the operation is to be discarded (DRV_MIIM_OPERATION_FLAG_DISCARD is set) there will be no notification to the client.
          The operation associated resources will be released.

        - If the operation is not to be discarded, then:
            - if the client has registered an operation notification callback (DRV_MIIM_RegisterCallback)
              then the callback will be called.
              After that the operation associated resources will be released.
            - if there is no notification callback the MIIM driver will wait for the client to poll and 
              read the operation result using DRV_MIIM_OperationResult().
              Only then the operation will be released.

    A completed non-discardable operation will remain available for returning the result until the client is somehow
    notified of the operation result.
    When polling is used, DRV_MIIM_OperationResult() needs to be called to free the operation associated resources.
*/

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Read(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);


// *****************************************************************************
/* Function:
    DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Write(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, uint16_t wData, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);
  
  Summary:
    Initiates a SMI/MIIM write transaction.

  Description:
    This function initiates a SMI/MIIM write transaction for a given MIIM register.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device handle.

  Parameters:
    - handle    - client's driver handle (returned from DRV_MIIM_Open)
    - rIx       - MIIM register to be accessed
    - phyAdd    - MIIM PHY address to use
    - wData     - MIIM data to be written
    - opFlags   - additional operation flags
                  See the DRV_MIIM_OPERATION_FLAGS definition.
    - pOpResult - address to store the operation result
                  Could be NULL if not needed.

  Returns:
    A not NULL DRV_MIIM_OPERATION_HANDLE if the operation was successfully scheduled.
    NULL if the operation failed. More details in pOpResult.


  Example:
    <code>
    </code>

  Remarks:

    If operation was scheduled successfully, the result will be DRV_MIIM_RES_PENDING.
    Otherwise an error code will be returned.

    Upon the operation completion:
        - If the operation is to be discarded (DRV_MIIM_OPERATION_FLAG_DISCARD is set) there will be no notification to the client.
          The operation associated resources will be released.

        - If the operation is not to be discarded, then:
            - if the client has registered an operation notification callback (DRV_MIIM_RegisterCallback)
              then the notification callback will be called.
              After that the operation associated resources will be released.
           - if there is no notification callback the MIIM driver will wait for the client to poll and 
             read the operation result using DRV_MIIM_OperationResult().
             Only then the operation will be released.

    A completed non-discardable operation will remain available for returning the result until the client is somehow
    notified of the operation result.
    When polling is used, DRV_MIIM_OperationResult() needs to be called to free the operation associated resources.

    A write operation normally uses DRV_MIIM_OPERATION_FLAG_DISCARD if it is not interested when the operation has completed.

*/

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Write(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, uint16_t wData, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);


/* Function:
    DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Scan(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);
  
  Summary:
    Initiates a SMI/MIIM scan (periodic read)transaction.

  Description:
    This function initiates a SMI/MIIM scan transaction for a given MIIM register.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device handle.

  Parameters:
    - handle    - client's driver handle (returned from DRV_MIIM_Open)
    - rIx       - MIIM register to be accessed
    - phyAdd    - MIIM PHY address to use
    - opFlags   - additional operation flags
                  See the DRV_MIIM_OPERATION_FLAGS definition.
    - pOpResult - address to store the operation result
                  Could be NULL if not needed.

  Returns:
    A not NULL DRV_MIIM_OPERATION_HANDLE if the operation was successfully scheduled.
    NULL if the operation failed. More details in pOpResult.


  Example:
    <code>
    </code>

  Remarks:

    If operation was scheduled successfully, the result will be DRV_MIIM_RES_PENDING.
    Otherwise an error code will be returned.

    When a new scan result is available:
        - If the operation is to be discarded (DRV_MIIM_OPERATION_FLAG_DISCARD is set) there will be no notification to the client.

        - If the operation is not to be discarded, then:
            - if the client has registered an operation notification callback (DRV_MIIM_RegisterCallback)
              then the notification callback will be called.
           - if there is no notification callback the MIIM driver will wait for the client to poll and 
             read the operation result using DRV_MIIM_OperationResult().
             Only then the operation will be released.

   
    A scheduled scan operation will remain active in the background and will be available for returning the scan results.
    When polling is used, DRV_MIIM_OperationResult()will return the latest scan result.
    The operation associated resources will be released and scan stopped only when DRV_MIIM_OperationAbort() is called.

    While scan is active all other transactions (including from other clients) will be inhibited!
    Use carefully!

*/

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Scan(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);


/***************************************************************************
  Function:
       DRV_MIIM_RESULT DRV_MIIM_OperationResult(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle, uint16_t* pOpData)
    
  Summary:
    Gets the result of a client operation initiated by the MIIM driver.

  Description:
    Returns the result of a client operation initiated by the MIIM driver.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device handle.
    - A driver operation was started 

  Parameters:
    - handle    - A valid instance handle, returned from the driver's open routine
    - opHandle  - An operation handle obtained with a DRV_MIIM_Read/DRV_MIIM_Write/DRV_MIIM_Scan 
    - pOpData   - Address to store the data associated with the operation

  Returns:
    - DRV_MIIM_RESULT value describing the current operation result:
        DRV_MIIM_RES_OK for success; operation has been completed successfully and pOpData updated
        DRV_MIIM_RES_PENDING operation is in progress
        an DRV_MIIM_RESULT error code if the operation failed.

  Example:
    <code>
    </code>

  Remarks:
    This function will not block for hardware access and will immediately
    return the current status.                                             

    This function returns the result of the last driver operation.
    It will return DRV_MIIM_RES_PENDING if an operation is still in progress.
    Otherwise a DRV_MIIM_RESULT describing the operation outcome.
    
    Note that for a scan operation DRV_MIIM_RES_PENDING will be returned
    when there's no new scan data available. DRV_MIIM_RES_OK means the
    scan data is fresh.
  ***************************************************************************/

DRV_MIIM_RESULT DRV_MIIM_OperationResult(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle, uint16_t* pOpData);

/***************************************************************************
  Function:
       DRV_MIIM_RESULT DRV_MIIM_OperationAbort(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle)
    
  Summary:
    Aborts a current client operation initiated by the MIIM driver.

  Description:
    Aborts a current client operation initiated by the MIIM driver.

  Precondition:
    - The DRV_MIIM_Initialize routine must have been called.
    - DRV_MIIM_Open must have been called to obtain a valid opened device
      handle.
    - A driver operation was started

  Parameters:
    - handle    - A valid instance handle, returned from the driver's open routine
    - opHandle  - An operation handle obtained with a DRV_MIIM_Read/DRV_MIIM_Write/DRV_MIIM_Scan 

  Returns:
    DRV_MIIM_RES_OK for success; operation has been aborted
    
    < 0 - an error has occurred and the operation could not be completed

  Example:
    <code>
    </code>

  Remarks:
    This operation will stop/abort a scan operation started by DRV_MIIM_Scan.

  ***************************************************************************/

DRV_MIIM_RESULT DRV_MIIM_OperationAbort(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle);  


// *****************************************************************************
/* MIIM Driver Base Object

  Summary:
    Declaration of a MIIM base object.

  Description:
    This data structure identifies the required basic interface of the MIIM driver.
    Any dynamic MIIM driver has to export this interface.

  Remarks:
    This object provides the basic MIIM functionality.
    Any derived driver can override the basic functionality while maintaining the required interface.

*/


typedef struct DRV_MIIM_OBJECT_BASE 
{
    SYS_MODULE_OBJ              (*DRV_MIIM_Initialize)(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

    void                        (*DRV_MIIM_Reinitialize)(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);

    void                        (*DRV_MIIM_Deinitialize)(SYS_MODULE_OBJ object);

    SYS_STATUS                  (*DRV_MIIM_Status)(SYS_MODULE_OBJ object);

    void                        (*DRV_MIIM_Tasks)(SYS_MODULE_OBJ object);

    DRV_HANDLE                  (*DRV_MIIM_Open)(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);

    DRV_MIIM_RESULT             (*DRV_MIIM_Setup)(DRV_HANDLE  handle, const DRV_MIIM_SETUP* pSetUp);

    void                        (*DRV_MIIM_Close)(DRV_HANDLE handle);

    DRV_MIIM_CLIENT_STATUS      (*DRV_MIIM_ClientStatus)(DRV_HANDLE handle);

    DRV_MIIM_CALLBACK_HANDLE    (*DRV_MIIM_RegisterCallback)(DRV_HANDLE handle, DRV_MIIM_OPERATION_CALLBACK cbFunction, DRV_MIIM_RESULT* pRegResult);

    DRV_MIIM_RESULT             (*DRV_MIIM_DeregisterCallback)(DRV_HANDLE handle, DRV_MIIM_CALLBACK_HANDLE cbHandle);

    DRV_MIIM_OPERATION_HANDLE   (*DRV_MIIM_Read)(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);

    DRV_MIIM_OPERATION_HANDLE   (*DRV_MIIM_Write)(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, uint16_t wData, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);

    DRV_MIIM_OPERATION_HANDLE   (*DRV_MIIM_Scan)(DRV_HANDLE handle, unsigned int rIx, unsigned int phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult);

    DRV_MIIM_RESULT             (*DRV_MIIM_OperationResult)(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle, uint16_t* pOpData);  

    DRV_MIIM_RESULT             (*DRV_MIIM_OperationAbort)(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle);  

}DRV_MIIM_OBJECT_BASE;


// *****************************************************************************
/* The supported basic MIIM driver (DRV_MIIM_OBJECT_BASE).
   This object is implemented by default as using the standard MIIM interface.
   It can be overwritten dynamically when needed.

*/
extern const DRV_MIIM_OBJECT_BASE  DRV_MIIM_OBJECT_BASE_Default;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef _DRV_MIIM_H

/*******************************************************************************
 End of File
*/


