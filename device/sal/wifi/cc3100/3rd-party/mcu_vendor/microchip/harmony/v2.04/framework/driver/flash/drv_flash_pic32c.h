/*******************************************************************************
  Flash Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_flash.h

  Summary:
    Flash Driver interface declarations for the static single instance driver.

  Description:
    The Flash device driver provides a simple interface to manage the Flash Controller on 
    Microchip microcontrollers. This file defines the interface Declarations for the 
    Flash driver.
    
  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.
    
    Static single-open interfaces also eliminate the need for the open handle.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.

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
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#ifndef _DRV_FLASH_PIC32C_H
#define _DRV_FLASH_PIC32C_H

#include <stdint.h>
#include "system_config.h"
#include "driver/driver_common.h"
#include "system/common/sys_module.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// *****************************************************************************
/* Driver FLASH Module Index

  Summary:
    FLASH driver index definitions

  Description:
    These constants provide FLASH driver index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.
    These values should be passed into the DRV_FLASH_Initialize and
    DRV_FLASH_Open routines to identify the driver instance in use.
*/

#define DRV_FLASH_INDEX_0             0
// *****************************************************************************
/* FLASH Driver Program Row Size.

  Summary:
    Specifies the FLASH Driver Program Row Size in bytes.

  Description:
    This definition specifies the FLASH Driver Program Row Size in bytes. This
    parameter is device specific and is obtained from the device specific
    processor header file. The Program Row Size is the maximum block size
    that can be programmed in one program operation.

  Remarks:
    None
*/
#define DRV_FLASH_ROW_SIZE                            (IFLASH_PAGE_SIZE)

// *****************************************************************************
/* FLASH Driver Program Page Size.

  Summary:
    Specifies the FLASH Driver Program Page Size in bytes.

  Description:
    This definition specifies the FLASH Driver Program Page Size in bytes. This
    parameter is device specific and is obtained from the device specific
    processor header file.

  Remarks:
    None
*/
#define DRV_FLASH_PAGE_SIZE                           (IFLASH_PAGE_SIZE * 16)




// *****************************************************************************
// *****************************************************************************
// Section: Interface Headers for the Static Driver
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_FLASH_Initialize(
	  const SYS_MODULE_INDEX index, 
	  const SYS_MODULE_INIT * const init
		)

  Summary:
    Initializes the Flash instance for the specified driver index.
    <p><b>Implementation:</b> Static</p>	

  Description:
    This function initializes the Flash Driver instance for the specified driver
    instance, making it ready for clients to use it. The initialization
    routine is specified by the MHC parameters.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the instance to be initialized

    init   - Pointer to a data structure containing any data necessary to
             initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Remarks:
    This function must be called before any other Flash function is called.
    This function should only be called once during system initialization. 
*/
SYS_MODULE_OBJ DRV_FLASH_Initialize(
	  const SYS_MODULE_INDEX index, 
	  const SYS_MODULE_INIT * const init
		);

// *****************************************************************************
/*	Function:
	DRV_HANDLE DRV_FLASH_Open(
		const SYS_MODULE_INDEX index, 
		const DRV_IO_INTENT ioIntent
	);

    Summary
        Initializes a channel to the appropriate flash device.

    Precondition
        None

    Parameters
		None.

    Returns
		Handle for future calls to the driver's operations.

*/
	DRV_HANDLE DRV_FLASH_Open(
		const SYS_MODULE_INDEX index, 
		const DRV_IO_INTENT ioIntent
	);
		
               
// *****************************************************************************
/* Function
    void DRV_FLASH_WriteRow( const DRV_HANDLE handle, uint32_t flashAddr,  uint32_t sourceData)

   Summary
    Writes an DRV_FLASH_ROW_SIZE bytes to the Flash at the (word-aligned) flashAddr.
    <p><b>Implementation:</b> Static</p>

   Description:
    This function starts the process of programming a buffer into the Flash.
    It does not wait for the write operation to be done, which is left to the user.
    It does not verify that the write was successful, which is left to the user.

   Precondition:
    The flashAddr is taken as a valid Flash address and should be page aligned. No range checking occurs.
    The memory pointed to by sourceData must be valid memory for at least 
    DRV_FLASH_ROW_SIZE bytes.
    Any previous Flash operations (write or erase) must be completed or this will
    fail silently.
    The Flash must be correctly erased at flashAddr.

   Parameters:
    param1 const DRV_HANDLE handle	Drive handle returned from DRV_FLASH_Open
    param2 uint32_t flashAddr      This contains the address of the location where
                                  the data will be written.
    param3 uint32_t *sourceData    This points to the data which will be written to 
                                  the Flash.

   Returns:
    None.

   Remarks:
    Most devices will be running for code stored in the Flash.  This means that any writes
    to the Flash will necessarily be writes to program space.  As such, they will prevent
    the CPU from reading further instructions until the write is done.  However, some devices
    may have more than one Flash such that it can run from one while writing to another.  
    Additionally, if the application is small enough, it may run out of a cache.  In any 
    case, it is up to the user to wait for an operation to complete and or to decide that 
    such a wait is unnecessary.

   Example:
    <code>
    flashAddr = 0x00500200;
    uint32_t dataStore[DRV_FLASH_ROW_SIZE] = {0,1,2,3,4,5};
    DRV_FLASH_Write_Row( const DRV_HANDLE handle, flashAddr,  dataStore);
    </code>
*/
    void DRV_FLASH_WriteRow( const DRV_HANDLE handle, uint32_t flashAddr,  uint32_t sourceData);
               
// *****************************************************************************
/* Function
    uint32_t DRV_FLASH_GetPageSize( const DRV_HANDLE handle )

   Summary
    Returns the size in bytes of a single "Page" which can be erased in the flash.
    <p><b>Implementation:</b> Static</p>

   Description:
    This function allows the user to get the size of a flash Page.

   Precondition:
	None
	
   Parameters:
    param1 const DRV_HANDLE handle	Drive handle returned from DRV_FLASH_Open

   Returns:
    None.

   Remarks:
    None.

*/
    uint32_t DRV_FLASH_GetPageSize( const DRV_HANDLE handle);
               
// *****************************************************************************
/* Function
    uint32_t DRV_FLASH_GetRowSize( const DRV_HANDLE handle )

   Summary
    Returns the size in bytes of a single "Row" which can be written to the flash.
    <p><b>Implementation:</b> Static</p>

   Description:
    This function allows the user to get the size of a flash Row.

   Precondition:
	None
	
   Parameters:
    param1 const DRV_HANDLE handle	Drive handle returned from DRV_FLASH_Open

   Returns:
    None.

   Remarks:
    None.

*/
    uint32_t DRV_FLASH_GetRowSize( const DRV_HANDLE handle);

// *****************************************************************************
/* Function
    void DRV_FLASH_ErasePage(uint32_t flashAddr);

   Summary
    Erases a page of Flash.
    <p><b>Implementation:</b> Static</p>

   Description:
    This function starts the process of erasing a page of Flash.  It does not 
    wait for the erase operation to be done.  That is left to the user.
    It does not verify that the erase was successful.  That is left to the user.
    It always erases 16 pages.  The size of a page in bytes will vary by
    device.  It will be available in the DRV_FLASH_PAGE_SIZE parameter.

   Precondition:
    The flashAddr is taken as a valid Flash address and should be page aligned. No range checking occurs.
    Any previous Flash operations (write or erase) must be completed or this will
    fail silently.
    The Flash must be correctly erased at flashAddr.

   Parameters:
    param uint32_t flashAddr   This contains the address of the location where 
                               the data will be written.
    param uint32_t sourceData  This is the data which will be written to the Flash.

   Returns:
    None.

   Remarks:
    Most devices will be running for code stored in the Flash. This means that any erases
    of the Flash will necessarily be writes to program space. As such, they will prevent
    the CPU from reading further instructions until the write is done.  However, some devices
    may have more than one Flash such that it can run from one while writing to another.  
    Additionally, if the application is small enough, it may run out of a cache. In any 
    case, it is up to the user to wait for an operation to complete and or to decide that 
    such a wait is unnecessary.

   Example:
    <code>
    flashAddr = 0x00500200;
    DRV_FLASH_Erase_Page(handle, flashAddr);
    </code>
*/
   void DRV_FLASH_ErasePage( const DRV_HANDLE handle, uint32_t flashAddr);

// *****************************************************************************
/* Function
    bool DRV_FLASH_IsBusy( void )

   Summary
    Returns true if the Flash device is still busy writing or is erasing.
    <p><b>Implementation:</b> Static</p>

   Description:
    This function checks whether the process of programming a Row into the Flash is
    still operating.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    - true  - Indicates the Flash is busy
    - false - Indicates the Flash is not busy

   Remarks:
    Most devices will be running for code stored in the Flash.  This means that any writes
    to the Flash will necessarily be writes to program space.  As such, they will prevent
    the CPU from reading further instructions until the write is done.  However, some devices
    may have more than one Flash such that it can run from one while writing to another.  
    Additionally, if the application is small enough, it may run out of a cache.  In any 
    case, it is up to the user to wait for an operation to complete and or to decide that 
    such a wait is unnecessary.

   Example:
    <code>
    flashAddr = 0x00500200;
    DRV_FLASH_WriteRow(flashAddr,  sourceData);
    DRV_FLASH_IsBusy( void );
    </code>
*/
	bool DRV_FLASH_IsBusy( const DRV_HANDLE handle );

	
#ifdef DRV_FLASH_DRIVER_MODE_STATIC
#include "framework/driver/flash/drv_flash_static_pic32c.h"
#endif
	
	
#endif /* _DRV_FLASH_PIC32C-H */
	
	
/*******************************************************************************
 End of File
*/
