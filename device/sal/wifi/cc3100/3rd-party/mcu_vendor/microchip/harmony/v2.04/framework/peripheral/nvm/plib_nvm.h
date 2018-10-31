/*******************************************************************************
  Non-Volatile Memory (NVM) Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.
    
  File Name:
    plib_nvm.h

  Summary:
    NVM PLIB Interface Header for NVM common definitions.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the NVM
    PLIB for all families of Microchip microcontrollers..The definitions in
    this file are common to NVM peripheral.
 *******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _PLIB_NVM_H
    #define _PLIB_NVM_H

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// ****************************************************************************
// ****************************************************************************
// Section: Included Files (continued at end of file)
// ****************************************************************************
// ****************************************************************************

/*  This section lists the other files that are included in this file.  However,
    please see the end of the file for additional implementation header files
    that are also included
 */

#include "processor/nvm_processor.h"

// ****************************************************************************
// ****************************************************************************
// Section: NVM Peripheral Library Interface Routines
// ****************************************************************************
// ****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_NVM_FlashEraseStart( NVM_MODULE_ID index)

  Summary:
    Performs erase operation on the selected Flash memory area.		  

  Description:
    This function Performs erase operation on the selected Flash memory region.
    This operation is atomic.

  Precondition:
    * The Address of the page to be erased must be provided using PLIB_NVM_FlashAddressToModify
	* Erase Operation should be selected using PLIB_NVM_MemoryOperationSelect
	* The module should be configured to access Flash memory using PLIB_NVM_MemoryModifyEnable
	* The unlock key sequence should be provided using PLIB_NVM_FlashWriteKeySequence

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_FlashEraseStart(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsWriteOperation in your application to determine whether
    this feature is available.
	 */

void PLIB_NVM_FlashEraseStart ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
	void PLIB_NVM_FlashWriteStart( NVM_MODULE_ID index)

  Summary:
    Performs a write operation on the Flash memory row selected.		  

  Description:
    This function performs a write operation on the Flash memory row selected.
    This operation is atomic.

  Precondition:
	* The Address of the page to be written must be provided using PLIB_NVM_FlashAddressToModify
	* Erase Operation should be selected using PLIB_NVM_MemoryOperationSelect
	* The module should be configured to access Flash memory using PLIB_NVM_MemoryModifyEnable
	* The unlock key sequence should be provided using PLIB_NVM_FlashWriteKeySequence

  Parameters:
    index   - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_FlashWriteStart(MY_NVM_INSTANCE);
    </code>

  Remarks:
   This feature may not be available on all devices. Please refer to the specific 
   device data sheet to determine availability or use PLIB_NVM_ExistsWriteOperation in your 
   application to determine whether this feature is available.
	 */

void PLIB_NVM_FlashWriteStart ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    bool PLIB_NVM_FlashWriteCycleHasCompleted( NVM_MODULE_ID index)

  Summary:
    Provides the status of the Flash write cycle.	  

  Description:
    This function provides the status of the Flash write cycle that was
    initiated by a write or erase operation.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * 0 - Write or erase cycle is incomplete
    * 1 - Write or erase cycle has completed

  Example:
    <code>
        bool status;
        status = PLIB_NVM_FlashWriteCycleHasCompleted(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsWriteOperation in your application to determine whether this
    feature is available.

 */

bool PLIB_NVM_FlashWriteCycleHasCompleted ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    bool PLIB_NVM_WriteOperationHasTerminated( NVM_MODULE_ID index)

  Summary:
    Provides the status of the Flash write operation or sequence.	  

  Description:
    This function provides the status of the Flash write operation or
    sequence that was initiated by a write or erase operation.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * 1 - Write operation prematurely terminated
    * 0 - Write operation completed

  Example:
    <code>
        bool status;
        status = PLIB_NVM_WriteOperationHasTerminated(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsWriteErrorStatus in your application to determine whether
    this feature is available.

 */

bool PLIB_NVM_WriteOperationHasTerminated ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    void PLIB_NVM_MemoryModifyEnable( NVM_MODULE_ID index)

  Summary:
    Allows write cycles to Flash memory.  

  Description:
    This function allows changing the write control (WR) bit and disables
    writing into the SWAP and NVMOP bits.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_MemoryModifyEnable(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsMemoryModificationControl in your application to determine
    whether this feature is available.

 */

 void PLIB_NVM_MemoryModifyEnable ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    void PLIB_NVM_MemoryModifyInhibit( NVM_MODULE_ID index)

  Summary:
    Inhibits write cycles to Flash memory.

  Description:
    This function disables the writing in the write control (WR) bit and enables 
	writing into the SWAP and NVMOP bits.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_MemoryModifyInhibit(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsMemoryModificationControl in your application to determine
    whether this feature is available.

 */

void PLIB_NVM_MemoryModifyInhibit ( NVM_MODULE_ID index ) ;


//******************************************************************************
/* Function:
    uint32_t PLIB_NVM_FlashRead( NVM_MODULE_ID index, uint32_t address)

  Summary:
    Read the specified address of Flash memory.

  Description:
    This function takes the address parameter in the argument and loads the read
    address to the appropriate register. The read operation provides data from
    the given address.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured	
    address			- The address in the memory from which to read

  Returns:
    Data value read at the memory address.

  Example:
    <code>
        uint32_t	DataToBeRead;
        DataToBeRead = PLIB_NVM_FlashRead(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsWriteOperation in your application to determine whether this
    feature is available.

 */

uint32_t PLIB_NVM_FlashRead ( NVM_MODULE_ID index , uint32_t address ) ;


//******************************************************************************
/* Function:
    void PLIB_NVM_FlashAddressToModify( NVM_MODULE_ID index, uint32_t address)

  Summary:
    Modifies the Flash memory address.

  Description:
    This function takes the address parameter in the argument and loads the
    address that will be modified by the actual write operation.
    The write or erase operation following this will write or erase the user data
    into or from the Flash program memory.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured	
    address			- The starting address in the memory from which data will
                      be written. This address should be a physical address.

  Returns:
    None.

  Example:
    <code>
        uint32_t	address = MY_FLASH_BASE_ADDRESS;
        PLIB_NVM_FlashAddressToModify(MY_NVM_INSTANCE, address);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsAddressModifyControl in your application to determine whether
    this feature is available.
 */

void PLIB_NVM_FlashAddressToModify ( NVM_MODULE_ID index , uint32_t address ) ;


//******************************************************************************
/* Function:
    void PLIB_NVM_FlashProvideData( NVM_MODULE_ID index, uint32_t data)

  Summary:
    Provides the data to be written into Flash memory.

  Description:
    This function provides the user data to intermediate registers before
    being written into Flash memory.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    data			- Data to be written

  Returns:
    None.

  Example:
    <code>
        uint32_t DataToWrite;
        PLIB_NVM_FlashProvideData(MY_NVM_INSTANCE, DataToWrite);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsProvideData in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_FlashProvideData ( NVM_MODULE_ID index , uint32_t data ) ;


//******************************************************************************
/* Function:
    void PLIB_NVM_MemoryOperationSelect( NVM_MODULE_ID index, 
                                         NVM_OPERATION_TYPE_SELECT operationmode)

  Summary:
    Selects the operation to be performed on Flash memory.

  Description:
    This function selects the operation to be performed on Flash memory.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_MemoryOperationSelect(MY_NVM_INSTANCE, NVM_MEMORY_ROW_PROGRAM_OPERATION);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsOperationMode in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_MemoryOperationSelect ( NVM_MODULE_ID index, NVM_OPERATION_MODE operationmode ) ;


//******************************************************************************
/* Function:
    void PLIB_NVM_FlashWriteKeySequence( NVM_MODULE_ID index, uint32_t keysequence)

  Summary:
    Copies the mandatory KEY sequence into the respective registers.

  Description:
    This function copies the mandatory KEY sequence into the respective registers.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    keysequence		- Mandatory KEY sequence depending on the controller type
	
  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_FlashWriteKeySequence(MY_NVM_INSTANCE, keysequence);
    </code>

  Remarks:
    Without the KEY sequence write or erase operation will not be executed.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsKeySequence in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_FlashWriteKeySequence ( NVM_MODULE_ID index , uint32_t keysequence ) ;


//******************************************************************************
/* Function:
    void PLIB_NVM_DataBlockSourceAddress( NVM_MODULE_ID index, uint32_t address)

  Summary:
    Takes the address parameter in the argument and loads the base address from 
    which data has to be copied into Flash memory.

  Description:
    This function takes the address parameter in the argument and loads the base
    address from which data has to be copied into Flash memory. This is to copy a row
    of data directly into Flash in one iteration without handling any
    intermediate holding registers.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index       - Identifier for the device instance to be configured
    address	    - The starting address in the user data memory from which
                  data will be written

  Returns:
    None.

  Example:
    <code>
        uint32_t	address = MY_RAM_BASE_ADDRESS;
        PLIB_NVM_DataBlockSourceAddress(MY_NVM_INSTANCE, address);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsSourceAddress in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_DataBlockSourceAddress ( NVM_MODULE_ID index , uint32_t address ) ;

//******************************************************************************
/* Function:
    bool PLIB_NVM_LowVoltageIsDetected( NVM_MODULE_ID index)

  Summary:
    Provides low voltage error detection status.

  Description:
    This function provides detection of low voltage error status.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * 1 - Low Voltage detection and possible data corruption
    * 0 - Voltage Level Acceptable for programming

  Example:
    <code>
        bool status;
        status = PLIB_NVM_LowVoltageIsDetected(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsLowVoltageStatus in your application to determine whether
    this feature is available.
 */

bool PLIB_NVM_LowVoltageIsDetected ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    bool PLIB_NVM_LowVoltageEventIsActive( NVM_MODULE_ID index)

  Summary:
    Provides low voltage detection status.

  Description:
    This function provides detection of low voltage event, if any.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * 1 - Low Voltage Event is active
    * 0 - Low Voltage Event is not active

  Example:
    <code>
    bool status;
    status = PLIB_NVM_LowVoltageEventIsActive(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsLowVoltageStatus in your application to determine whether
    this feature is available.
 */

bool PLIB_NVM_LowVoltageEventIsActive ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    void PLIB_NVM_ProgramFlashBank1LowerRegion(NVM_MODULE_ID index);

  Summary:
    Maps Flash Bank 1 to the lower mapped region.

  Description:
    This function maps Program Flash Bank 1 to the lower mapped region and
    programs Flash Bank 2 to the upper mapped region.
    This operation is atomic.

  Precondition:
    The WREN bit in the NVMCON register should be set to '0' using 
	PLIB_NVM_MemoryModifyInhibit before swapping the memory regions.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_ProgramFlashBank1LowerRegion(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsFlashBankRegionSelect in your application to determine
    whether this feature is available.
 */

void PLIB_NVM_ProgramFlashBank1LowerRegion(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_NVM_ProgramFlashBank2LowerRegion(NVM_MODULE_ID index);

  Summary:
    Maps the bank 2 to lower mapped region.

  Description:
    This function maps Program Flash Bank 2 to the lower mapped region and
    Program Flash Bank 1 to the upper mapped region.
    This operation is atomic.

  Precondition:
    The WREN bit in the NVMCON register should be set to '0' using 
	PLIB_NVM_MemoryModifyInhibit before swapping the memory regions.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_ProgramFlashBank2LowerRegion(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsFlashBankRegionSelect in your application to determine
    whether this feature is available.
 */

void PLIB_NVM_ProgramFlashBank2LowerRegion(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    bool PLIB_NVM_ProgramFlashBank2IsLowerRegion(NVM_MODULE_ID index);

  Summary:
   Gives the status of Program Flash Bank mapping.

  Description:
    This function tells which Program Flash Bank is mapped to which region.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * 1 - Program Flash Bank 2 is mapped to the lower mapped region and Program
          Flash Bank 1 is mapped to the upper mapped region
    * 0 - Program Flash Bank 1 is mapped to the lower mapped region and Program
          Flash Bank 2 is mapped to the upper mapped region

  Example:
    <code>
        if(!PLIB_NVM_ProgramFlashBank2IsLowerRegion(MY_NVM_INSTANCE))
        PLIB_NVM_ProgramFlashBank2LowerRegion(NVM_MODULE_ID index);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsFlashBankRegionSelect in your application to determine
    whether this feature is available.
 */

bool PLIB_NVM_ProgramFlashBank2IsLowerRegion(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_NVM_BootFlashBank1LowerRegion(NVM_MODULE_ID index);

  Summary:
    Maps Boot Flash Bank 1 to lower mapped region.

  Description:
    This function maps Boot Flash Bank 1 to the lower mapped region and
    Boot Flash Bank 2 to the upper mapped region.
    This operation is atomic.

  Precondition:
    The WREN bit in the NVMCON register should be set to '0' using 
	PLIB_NVM_MemoryModifyInhibit before swapping the memory regions.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_BootFlashBank1LowerRegion(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsBootFlashBankRegion in your application to determine
    whether this feature is available.
 */

void PLIB_NVM_BootFlashBank1LowerRegion(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_NVM_BootFlashBank2LowerRegion(NVM_MODULE_ID index);

  Summary:
    Maps Boot Flash Bank 2 to the lower mapped region.

  Description:
    This function maps Boot Flash Bank 2 to the lower mapped region and
    Boot Flash Bank 1 to the upper mapped region.
    This operation is atomic.

  Precondition:
    The WREN bit in the NVMCON register should be set to '0' using 
	PLIB_NVM_MemoryModifyInhibit before swapping the memory regions.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_BootFlashBank2LowerRegion(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsBootFlashBankRegion in your application to determine
    whether this feature is available.
 */

void PLIB_NVM_BootFlashBank2LowerRegion(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    bool PLIB_NVM_BootFlashBank2IsLowerRegion(NVM_MODULE_ID index);

  Summary:
    Gives the status of Boot Flash Bank mapping.

  Description:
    This function tells which Boot Flash Bank is mapped to which region.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * 1 - Boot Flash Bank 2 is mapped to the lower mapped region and Boot
          Flash Bank 1 is mapped to the upper mapped region
    * 0 - Boot Flash Bank 1 is mapped to the lower mapped region and Boot
          Flash Bank 2 is mapped to the upper mapped region

  Example:
    <code>
        if(!PLIB_NVM_BootFlashBank2IsLowerRegion(MY_NVM_INSTANCE))
        PLIB_NVM_BootFlashBank2LowerRegion(NVM_MODULE_ID index);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsBootFlashBankRegion in your application to determine
    whether this feature is available.
 */

bool PLIB_NVM_BootFlashBank2IsLowerRegion(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_NVM_FlashProvideQuadData(NVM_MODULE_ID index, uint32_t *data);

 Summary:
    Provides the quad data to be written into Flash memory.

  Description:
    This function provides the user quad data to intermediate registers before
    being written into Flash memory.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    data			- Address pointing to the data to be written.
    
  Returns:
    None.

  Example:
    <code>
        uint32_t  DataToWrite[4];
        PLIB_NVM_FlashProvideQuadData(MY_NVM_INSTANCE, &DataToWrite);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsProvideQuadData in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_FlashProvideQuadData(NVM_MODULE_ID index, uint32_t *data);

//******************************************************************************
/* Function:
    void PLIB_NVM_LockProgramFlashMemory(NVM_MODULE_ID index);

  Summary:
    Lock the Program Flash write-protect register. 

  Description:
    This function locks the Program Flash Write-Protect register (NVMPWP).

  Precondition:
    Unlock key sequence should be provided using API PLIB_NVM_FlashWriteKeySequence.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_LockProgramFlashMemory(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsLockPFMSelect in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_LockProgramFlashMemory(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    bool PLIB_NVM_IsProgramFlashMemoryLocked(NVM_MODULE_ID index);

  Summary:
    Provides lock status of Program Flash Write-Protect register.

  Description:
    This function provides lock status of Program Flash Write-Protect (NVMPWP)
    register.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * 1 - Program Flash write-protect register is locked
    * 0 - Program Flash write-protect register is not locked

  Example:
    <code>
    bool status;
        status = PLIB_NVM_IsProgramFlashMemoryLocked(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsLockPFMSelect in your application to determine whether this
    feature is available.
 */

bool PLIB_NVM_IsProgramFlashMemoryLocked(NVM_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_NVM_FlashWriteProtectMemoryAreaRange(NVM_MODULE_ID index, uint32_t address);

  Summary:
    Sets the address below which physical memory will be write protected.

  Description:
    This function sets the address below which physical memory will be protected.
    Physical memory below address 0x1Dxxxxxx is write protected, where 'xxxxxx'
    is specified by "address" parameter. When "address" has a value of '0',
    write protection is disabled for the entire program Flash. If the specified
    address falls within the page, the entire page and all pages below the
    current page will be protected.

  Precondition:
    The unlock key sequence should be provided using PLIB_NVM_FlashWriteKeySequence.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    address         - Flash program write-protect (Page) address

  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_FlashWriteProtectMemoryAreaRange(MY_NVM_INSTANCE, WRITE_PROTECT_PAGE_ADDRESS);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsFlashWPMemoryRangeProvide in your application to determine
    whether this feature is available.
 */

void PLIB_NVM_FlashWriteProtectMemoryAreaRange(NVM_MODULE_ID index, uint32_t address);

//******************************************************************************
/* Function:
    void PLIB_NVM_LockBootMemory(NVM_MODULE_ID index, NVM_BOOT_MEMORY_AREA memoryArea);

  Summary:
    Locks the boot write-protect bits.

  Description:
    This function locks the given (lower or upper alias) boot write-protect bits.
    This operation is atomic.

  Precondition:
    The unlock key sequence should be provided using PLIB_NVM_FlashWriteKeySequence.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    memoryArea      - Selects between Lower or Upper Boot Alias area
    
  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_LockBootMemory(MY_NVM_INSTANCE, LOWER_BOOT_ALIAS_AREA);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsLockBootSelect in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_LockBootMemory(NVM_MODULE_ID index, NVM_BOOT_MEMORY_AREA memoryArea);

//******************************************************************************
/* Function:
    bool PLIB_NVM_IsBootMemoryLocked(NVM_MODULE_ID index, NVM_BOOT_MEMORY_AREA memoryArea);

  Summary:
    Provides lock status of boot page write-protect bits. 

  Description:
    This function provides lock status of boot page write-protect bits.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    memoryArea      - Selects between the Lower or Upper Boot Alias area
    
  Returns:
    * 1 - Selected boot alias write-protect bits are locked
    * 0 - Selected boot alias write-protect bits are not locked

  Example:
    <code>
        bool status;
        status = PLIB_NVM_IsBootMemoryLocked(MY_NVM_INSTANCE, LOWER_BOOT_ALIAS_AREA);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsLockBootSelect in your application to determine whether this
    feature is available.
 */

bool PLIB_NVM_IsBootMemoryLocked(NVM_MODULE_ID index, NVM_BOOT_MEMORY_AREA memoryArea);

//******************************************************************************
/* Function:
    void PLIB_NVM_BootPageWriteProtectionEnable(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage);

  Summary:
    Locks the selected boot page.

  Description:
    This function locks the selected boot page.
    This operation is atomic.

  Precondition:
    Unlock key sequence should be provided using API PLIB_NVM_FlashWriteKeySequence.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    bootPage        - Selects the boot page which has to be locked
    
  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_BootPageWriteProtectionEnable(MY_NVM_INSTANCE, LOWER_BOOT_ALIAS_PAGE4);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsBootPageWriteProtect in your application to determine whether
    this feature is available.
 */

void PLIB_NVM_BootPageWriteProtectionEnable(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage);

//******************************************************************************
/* Function:
    void PLIB_NVM_BootPageWriteProtectionDisable(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage);

   Summary:
    Write protection for selected boot page is disabled.

  Description:
    This function disables Write protection for selected boot page.
    This operation is atomic.

  Precondition:
    Unlock key sequence should be provided using API PLIB_NVM_FlashWriteKeySequence.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    bootPage        - Selects the boot page for which write protection has to be disabled
    
  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_BootPageWriteProtectionDisable(MY_NVM_INSTANCE, LOWER_BOOT_ALIAS_PAGE4);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsBootPageWriteProtect in your application to determine whether
    this feature is available.
 */

void PLIB_NVM_BootPageWriteProtectionDisable(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage);

//******************************************************************************
/* Function:
    bool PLIB_NVM_IsBootPageWriteProtected(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage);

  Summary:
    Provides write protection status for boot memory page.

  Description:
    This function provides write protection status for selected boot memory page.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    bootPage        - Selects the boot page region
    
  Returns:
    * 1 - Selected boot region is write protected
    * 0 - Selected boot region is not write protected

  Example:
    <code>
    bool status;
    status = PLIB_NVM_IsBootPageWriteProtected(MY_NVM_INSTANCE, LOWER_BOOT_ALIAS_PAGE4);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsBootPageWriteProtect in your application to determine whether
    this feature is available.
 */

bool PLIB_NVM_IsBootPageWriteProtected(NVM_MODULE_ID index, NVM_BOOT_MEMORY_PAGE bootPage);

//******************************************************************************
/* Function:
    void PLIB_NVM_FlashSwapLockSelect
    ( 
        NVM_MODULE_ID index, 
        NVM_FLASH_SWAP_LOCK_TYPE lockType
    )

  Summary:
    Selects the kind of Flash swap lock required.		  

  Description:
    This function allows user to select which swap bits will be writable.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    lockType        - One of the value from NVM_FLASH_SWAP_LOCK_TYPE enum to
                      identify which swap bit will be locked
    
  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_FlashSwapLockSelect(MY_NVM_INSTANCE, NVM_FLASH_SWAP_UNLOCKED);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsSwapLockControl in your application to determine
    whether this feature is available.

*/

void PLIB_NVM_FlashSwapLockSelect
    ( 
        NVM_MODULE_ID index, 
        NVM_FLASH_SWAP_LOCK_TYPE lockType
    );

//******************************************************************************
/* Function:
    NVM_FLASH_SWAP_LOCK_TYPE PLIB_NVM_FlashSwapLockStatusGet
    ( 
        NVM_MODULE_ID index
    )

  Summary:
    Get the status of Swap lock bits.		  

  Description:
    This function allows user to get the status of swap lock bits.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured.
    
  Returns:
    - NVM_FLASH_SWAP_LOCK_TYPE  - The lock status of Flash swap bits.

  Example:
    <code>
    NVM_FLASH_SWAP_LOCK_TYPE lockStatus;
    lockStatus = PLIB_NVM_FlashSwapLockStatusGet(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsSwapLockControl in your application to determine
    whether this feature is available.

*/

NVM_FLASH_SWAP_LOCK_TYPE PLIB_NVM_FlashSwapLockStatusGet
    ( 
        NVM_MODULE_ID index
    );


//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMEnable( NVM_MODULE_ID index )

  Summary:
    Enables the EEPROM memory.		  

  Description:
    This function enables the EEPROM memory for access.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
	PLIB_NVM_EEPROMEnable(MY_NVM_INSTANCE);
	if(PLIB_NVM_EEPROMIsReady(MY_NVM_INSTANCE))
		{
			//Perform operation
		}
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to
    the specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMEnableControl in your application to determine whether
    this feature is available.
 */

void PLIB_NVM_EEPROMEnable( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMDisable( NVM_MODULE_ID index )

  Summary:
    Disables the EEPROM memory.		  

  Description:
    This function disables EEPROM memory access.
    This operation is atomic.

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.

  Parameters:
    index  - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
	if(PLIB_NVM_EEPROMOperationHasCompleted)
		{
			PLIB_NVM_EEPROMDisable(MY_NVM_INSTANCE);
		}
    </code>

  Remarks: 
    This feature may not be available on all devices. Please refer to
    the specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMEnableControl in your application to determine whether
    this feature is available.
 */

void PLIB_NVM_EEPROMDisable( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_NVM_EEPROMIsReady( NVM_MODULE_ID index )

  Summary:
    Provides the availability status of the EEPROM.	  

  Description:
    This function provides the ready status of the EEPROM which was
    initiated by PLIB_NVM_EEPROMEnable.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * true  - EEPROM is ready to use
    * false - EEPROM is not yet ready to use

  Example:
    <code>
	PLIB_NVM_EEPROMEnable(MY_NVM_INSTANCE);
	if(PLIB_NVM_EEPROMIsReady(MY_NVM_INSTANCE))
		{
			//Perform operation
		}
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMEnableControl in your application to determine whether this
    feature is available.

 */

bool PLIB_NVM_EEPROMIsReady( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMStopInIdleEnable( NVM_MODULE_ID index )

  Summary:
    Discontinues EEPROM operation when device enters Idle mode.

  Description:
    This function discontinues EEPROM operation when device enters Idle mode.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_EEPROMStopInIdleEnable(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMStopInIdleControl in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMStopInIdleEnable( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMStopInIdleDisable( NVM_MODULE_ID index )

  Summary:
    Continues EEPROM operation when device enters Idle mode.

  Description:
    This function continues EEPROM operation when device enters Idle mode.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_EEPROMStopInIdleDisable(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMStopInIdleControl in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMStopInIdleDisable( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_NVM_EEPROMStopInIdleIsEnabled( NVM_MODULE_ID index )

  Summary:
    Returns Stop in Idle mode status of the EEPROM operation.

  Description:
    This function returns whether the EEPROM continues or discontinues operation 
	when the device enters Idle mode.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * true  - EEPROM discontinues operation when the device enters Idle mode
    * false - EEPROM continues operation when the device enters Idle mode
   
  Example:
    <code>
	bool sidl_status;
    sidl_status = PLIB_NVM_EEPROMStopInIdleIsEnabled(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMStopInIdleControl in your application to determine whether this
    feature is available.
 */

bool PLIB_NVM_EEPROMStopInIdleIsEnabled( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMOperationSelect( NVM_MODULE_ID index , EEPROM_OPERATION_MODE mode)

  Summary:
    Selects the operation to be performed on the EEPROM.

  Description:
    This function selects the operation to be performed on the EEPROM from the set of 
	operations available from the enum EEPROM_OPERATION_MODE.
    This operation is atomic.

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_EEPROMOperationSelect(MY_NVM_INSTANCE, EEPROM_WORD_READ_OPERATION);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMOperationModeControl in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMOperationSelect( NVM_MODULE_ID index , EEPROM_OPERATION_MODE mode);

//******************************************************************************
/* Function:
    bool PLIB_NVM_EEPROMAddress( NVM_MODULE_ID index, uint32_t address)

  Summary:
    EEPROM address where operation has to be performed.

  Description:
    This function accepts the EEPROM address upon which to operate.
	Lower two bits of the address must always be '00'; otherwise, an error 
	will occur when operation is started on EEPROM.

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.
	
  Parameters:
    index           - Identifier for the device instance to be configured	
    address			- The address of the EEPROM from or to which data will
                      be read or written. This address should be a physical address.

  Returns:
    * true  - EEPROM address is valid
    * false - EEPROM address is invalid, please check if it is in the EEPROM 
			  address boundary and last two bits are '00'

  Example:
    <code>
        uint32_t  address = MY_EEPROM_ADDRESS;
		// Load address and check if it is valid
		if(true ==PLIB_NVM_EEPROMAddress( NVM_ID_0 , address )) 
		{
			//Perform operation
		}	

    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMAddressControl in your application to determine whether
    this feature is available.
 */

bool PLIB_NVM_EEPROMAddress ( NVM_MODULE_ID index , uint32_t address ) ;

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMDataToWrite ( NVM_MODULE_ID index , uint32_t data )

  Summary:
    Accepts the data to be written into the EEPROM.

  Description:
    This function accepts data to be written into the EEPROM.
    This operation is atomic.	

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    data			- Data to be written
	
  Returns:
    None.

  Example:
    <code>
        PLIB_NVM_EEPROMDataToWrite(MY_NVM_INSTANCE, DATA);
    </code>

  Remarks: 
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMDataControl in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMDataToWrite ( NVM_MODULE_ID index , uint32_t data ) ;


//******************************************************************************
/* Function:
    uint32_t PLIB_NVM_EEPROMRead( NVM_MODULE_ID index)

  Summary:
    Read the EEPROM data. 		  

  Description:
    This function returns the EERPOM data that is been fetched 
	by performing the EEPROM read operation sequence.
    This operation is atomic.
	
  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation,
	or may return junk data.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    32-bit EEPROM data.

  Example:
    <code>
	    uint32_t EEPROM_Data;
		// Start the read operation
		PLIB_NVM_EEPROMReadStart( NVM_MODULE_ID index );  
		// Wait until read is complete
		while (PLIB_NVM_EEPROMOperationHasCompleted( NVM_ID_0 ));
		// Now read the data
		EEPROM_Data = PLIB_NVM_EEPROMRead( NVM_ID_0 );
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMDataControl in your application to determine whether
    this feature is available.

 */

uint32_t PLIB_NVM_EEPROMRead( NVM_MODULE_ID index ) ;


//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMKeySequenceWrite ( NVM_MODULE_ID index , uint32_t keysequence )

  Summary:
    Write mandatory KEY sequence to unlock the EEPROM write or erase protection.

  Description:
    Without the KEY sequence write or erase operation will not be executed.
    Writing the KEY 0xEDB7 followed by writing the another KEY 0x1248 will unlock the EEPROM
    control register for write or erase operations. 
    Writing any other value will lock the EEPROM control register.
	The unlock sequence is not necessary for a read operation.
    This operation is atomic.	
	
  Precondition:
    Interrupts should be disabled when writing the unlock sequence to the EEKEY register to 
	prevent a disruption of the unlock sequence.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    keysequence		- Mandatory KEY sequence depending on the controller type
	
  Returns:
    None.

  Example:
    <code>
	    //EEPROM is locked.
	    uint32_t key1 = 0xEDB7;
		uint32_t key2 = 0x1248;
        PLIB_NVM_EEPROMKeySequenceWrite(MY_NVM_INSTANCE, key1);
        PLIB_NVM_EEPROMKeySequenceWrite(MY_NVM_INSTANCE, key2);
	    //EEPROM is now unlocked.		
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMKeySequence in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMKeySequenceWrite ( NVM_MODULE_ID index , uint32_t keysequence ) ;

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMWriteEnable( NVM_MODULE_ID index )

  Summary:
    Allows write or erase operation to the EEPROM.		  

  Description:
    This function enables write or erase operations on the EEPROM and inhibits read.
    This operation is atomic.

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_EEPROMWriteEnable(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMEnableOperationControl in your application to determine
    whether this feature is available.

 */

void PLIB_NVM_EEPROMWriteEnable( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMReadEnable( NVM_MODULE_ID index )

  Summary:
    Allows read operation to the EEPROM.		  

  Description:
    This function enables read operations on the EEPROM and inhibits write or erase.
    This operation is atomic.

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_EEPROMReadEnable(MY_NVM_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMEnableOperationControl in your application to determine
    whether this feature is available.

 */

void PLIB_NVM_EEPROMReadEnable( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_NVM_EEPROMWriteIsEnabled( NVM_MODULE_ID index )

  Summary:
    Returns EEPROM Write permission status.

  Description:
    This function returns whether or not the EEPROM write or erase.
    This operation is atomic.	

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * true  - EEPROM write or erase is enabled
    * false - EEPROM write or erase is not enabled

  Example:
    <code>
    if(PLIB_NVM_EEPROMWriteIsEnabled(MY_NVM_INSTANCE))
	{
		//perform write or erase
	}
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMEnableOperationControl in your application to determine whether this
    feature is available.
 */

bool PLIB_NVM_EEPROMWriteIsEnabled( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_NVM_EEPROMReadIsEnabled( NVM_MODULE_ID index )

  Summary:
    Returns EEPROM read permission status.

  Description:
    This function returns whether or not the EEPROM read is enabled.
    This operation is atomic.	

  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * true  - EEPROM read is enabled.
    * false - EEPROM read is not enabled.

  Example:
    <code>
    if(PLIB_NVM_EEPROMReadIsEnabled(MY_NVM_INSTANCE))
	{
		//perform read
	}
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMEnableOperationControl in your application to determine whether this
    feature is available.
 */

bool PLIB_NVM_EEPROMReadIsEnabled( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMReadStart( NVM_MODULE_ID index)

  Summary:
    Initiates a EEPROM read cycle.		  

  Description:
    This function initiates the EEPROM read cycle.
    This operation is atomic.

  Precondition:
    EEPROM read should be enabled through PLIB_NVM_EEPROMReadEnable.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
	// Enable read operation
	PLIB_NVM_EEPROMReadEnable( NVM_ID_0 );
	// Start the read cycle
	PLIB_NVM_EEPROMReadStart( NVM_MODULE_ID index );  
	// Wait until read is complete
	while (PLIB_NVM_EEPROMOperationHasCompleted( NVM_ID_0 ));
	// Read the data
	ee_data = PLIB_NVM_EEPROMRead( NVM_ID_0 ); 
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMStartOperationControl in your application to determine whether
    this feature is available.

 */

void PLIB_NVM_EEPROMReadStart ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMWriteStart( NVM_MODULE_ID index)

  Summary:
    Initiates a EEPROM write cycle.		  

  Description:
    This function initiates the EEPROM write cycle.
    This operation is atomic.

  Precondition:
    EEPROM write should be enabled through PLIB_NVM_EEPROMWriteEnable.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
	// Enable write operation
	PLIB_NVM_EEPROMWriteEnable( NVM_ID_0 );
    // Provide data    
	PLIB_NVM_EEPROMDataToWrite ( NVM_ID_0 , ee_data ) ;
	// unlock
	PLIB_NVM_EEPROMKeySequenceWrite(MY_NVM_INSTANCE, key1);  
	PLIB_NVM_EEPROMKeySequenceWrite(MY_NVM_INSTANCE, key2);
	// Start the write operation
	PLIB_NVM_EEPROMWriteStart( NVM_MODULE_ID index );
    // Wait until write is complete	
	while (PLIB_NVM_EEPROMOperationHasCompleted( NVM_ID_0 )); 
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMStartOperationControl in your application to determine whether
    this feature is available.
 */

void PLIB_NVM_EEPROMWriteStart ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMEraseStart( NVM_MODULE_ID index)

  Summary:
    Initiates EEPROM erase cycle.		  

  Description:
    This function initiates the EEPROM erase cycle.
    This operation is atomic.

  Precondition:
    PLIB_NVM_EEPROMWriteEnable should be called.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
	// Enable write or erase operation
	PLIB_NVM_EEPROMWriteEnable( NVM_ID_0 );
	// unlock
	PLIB_NVM_EEPROMKeySequenceWrite(MY_NVM_INSTANCE, key1);  
	PLIB_NVM_EEPROMKeySequenceWrite(MY_NVM_INSTANCE, key2);
	// Start the erase operation
	PLIB_NVM_EEPROMEraseStart( NVM_MODULE_ID index );
    // Wait until erase is complete	
	while (PLIB_NVM_EEPROMOperationHasCompleted( NVM_ID_0 )); 
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMStartOperationControl in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMEraseStart ( NVM_MODULE_ID index ) ;

//******************************************************************************
/* Function:
    bool PLIB_NVM_EEPROMOperationHasCompleted( NVM_MODULE_ID index )

  Summary:
    Provides the status of the EEPROM write or erase or read cycle.	  

  Description:
    This function provides the status of the EEPROM write or erase or read operation 
	status.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    * true  - Write or erase cycle has completed
    * false - Write or erase cycle has not completed

  Example:
    <code>
	// Start the erase operation
	PLIB_NVM_EEPROMEraseStart( NVM_MODULE_ID index );
    // Wait until erase is complete	
	while (PLIB_NVM_EEPROMOperationHasCompleted( NVM_ID_0 ));
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMStartOperationControl in your application to determine whether this
    feature is available.

 */

bool PLIB_NVM_EEPROMOperationHasCompleted( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_NVM_EEPROMNextWriteCycleIsLong( NVM_MODULE_ID index )

  Summary:
    Informs whether the next write or erase cycle of the EEPROM is long.	  

  Description:
    This function informs whether the next write or erase cycle of the EEPROM is long
	(i.e., the next write or erase to the EEPROM address (passed by PLIB_NVM_EEPROMAddress) 
	will require more time (~20 ms) than usual).
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    false - Next write/Erase cycle will not take more time to complete.
    true - Next write/Erase cycle will take more time to complete.

  Example:
    <code>
        if(PLIB_NVM_EEPROMNextWriteCycleIsLong( MY_NVM_INSTANCE ))
		{
			//stays here for ~20ms, better skip to other task for this time.
		}
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMLongWriteStatus in your application to determine whether this
    feature is available.

 */

bool PLIB_NVM_EEPROMNextWriteCycleIsLong( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMOperationAbort( NVM_MODULE_ID index )

  Summary:
    Aborts the current EEPROM operation.		  

  Description:
    This function aborts the on-going write or erase operation as soon as possible.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    PLIB_NVM_EEPROMOperationAbort(MY_NVM_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMOperationAbortControl in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMOperationAbort( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    EEPROM_ERROR PLIB_NVM_EEPROMErrorGet( NVM_MODULE_ID index )

  Summary:
    Returns the EEPROM operation error.		  

  Description:
    This function returns the EEPROM operation error.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    EEPROM_ERROR.

  Example:
    <code>
	EEPROM_ERROR error;
    error = PLIB_NVM_EEPROMErrorGet(MY_NVM_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMErrorStatus in your application to determine whether this
    feature is available.
 */

EEPROM_ERROR PLIB_NVM_EEPROMErrorGet( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMErrorClear( NVM_MODULE_ID index )

  Summary:
    Clears the EEPROM operation error.		  

  Description:
    This function clears the EEPROM operation error.
    This operation is atomic.

  Precondition:
    None.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
         PLIB_NVM_EEPROMErrorClear(MY_NVM_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMErrorStatus in your application to determine whether this
    feature is available.
 */

void PLIB_NVM_EEPROMErrorClear( NVM_MODULE_ID index );

    
//******************************************************************************
/* Function:
    uint8_t PLIB_NVM_EEPROMWaitStatesGet ( NVM_MODULE_ID index )

  Summary:
    Fetch the current EEPROM waitstates value.

  Description:
    This function fetches the current waitstates value for EEPROM. This
    operation is atomic.
	
  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation,
	or may return junk data.
	
  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    8-bit EEPROM wait states value.

  Example:
    <code>
	    uint8_t eews = 0;
		eews = PLIB_NVM_EEPROMWaitStatesGet (NVM_ID_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMWaitStates in your application to determine whether
    this feature is available.

 */

uint8_t PLIB_NVM_EEPROMWaitStatesGet  ( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_NVM_EEPROMWaitStatesSet ( NVM_MODULE_ID index, uint8_t eews )

  Summary:
    Configures the EEPROM waitstates.

  Description:
    This function configures the EEPROM waitstates. This operation is atomic.
	
  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation,
	or may return junk data.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    eews            - Waitstates value

  Returns:
    None

  Example:
    <code>
	    uint8_t eews = 2;
		PLIB_NVM_EEPROMWaitStatesSet (NVM_ID_0, eews);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMWaitStates in your application to determine whether
    this feature is available.

 */

void PLIB_NVM_EEPROMWaitStatesSet  ( NVM_MODULE_ID index, uint8_t eews );


//******************************************************************************
/* Function:
    uint32_t PLIB_NVM_EEPROMReadCalibrationData ( NVM_MODULE_ID index, EEPROM_CALIBRATION_REG regIndex )

  Summary:
    Read the EEPROM Calibration data. 		  

  Description:
    This function returns the Calibration data for the requested EEPROM
    calibration register. This operation is atomic.
	
  Precondition:
    This API will have no affect if there is any ongoing EEPROM operation,
	or may return junk data.
	
  Parameters:
    index           - Identifier for the device instance to be configured
    regIndex        - Index of the EEPROM calibration register.

  Returns:
    32-bit EEPROM Calibration data.

  Example:
    <code>
	    uint32_t calData = 0;
		calData = PLIB_NVM_EEPROMReadCalibrationData (NVM_ID_0, CALIBRATION_DATA_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_NVM_ExistsEEPROMCalibrationData in your application to determine whether
    this feature is available.

 */

uint32_t PLIB_NVM_EEPROMReadCalibrationData ( NVM_MODULE_ID index, EEPROM_CALIBRATION_REG regIndex );

// *****************************************************************************
// *****************************************************************************
// Section: NVM Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************

/* The following functions indicate the existence of the features on the device. 
*/

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsWriteErrorStatus( NVM_MODULE_ID index )

  Summary:
    Identifies whether the WriteErrorStatus feature exists on the NVM module. 

  Description:
    This function identifies whether the WriteErrorStatus feature is available 
	on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_WriteOperationHasTerminated
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The WriteErrorStatus feature is supported on the device
    - false  - The WriteErrorStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsWriteErrorStatus( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsMemoryModificationControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the MemoryModificationControl feature exists on the NVM module. 

  Description:
    This function identifies whether the MemoryModificationControl feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_MemoryModifyEnable
    - PLIB_NVM_MemoryModifyInhibit
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MemoryModificationControl feature is supported on the device
    - false  - The MemoryModificationControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsMemoryModificationControl( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsOperationMode( NVM_MODULE_ID index )

  Summary:
    Identifies whether the OperationMode feature exists on the NVM module. 

  Description:
    This function identifies whether the OperationMode feature is available on 
	the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_MemoryOperationSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The OperationMode feature is supported on the device
    - false  - The OperationMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsOperationMode( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsAddressModifyControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the AddressModifyControl feature exists on the NVM module.

  Description:
    This function identifies whether the AddressModifyControl feature is available 
	on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_FlashAddressToModify
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AddressModifyControl feature is supported on the device
    - false  - The AddressModifyControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsAddressModifyControl( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsProvideData( NVM_MODULE_ID index )

  Summary:
    Identifies whether the ProvideData feature exists on the NVM module. 

  Description:
    This function identifies whether the ProvideData feature is available on the 
	NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_FlashProvideData
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ProvideData feature is supported on the device
    - false  - The ProvideData feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsProvideData( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsWriteOperation( NVM_MODULE_ID index )

  Summary:
    Identifies whether the WriteOperation feature exists on the NVM module. 

  Description:
    This function identifies whether the WriteOperation feature is available on 
	the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_FlashRead
    - PLIB_NVM_FlashWriteStart
    - PLIB_NVM_FlashEraseStart
    - PLIB_NVM_FlashWriteCycleHasCompleted
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The WriteOperation feature is supported on the device
    - false  - The WriteOperation feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsWriteOperation( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsKeySequence( NVM_MODULE_ID index )

  Summary:
    Identifies whether the KeySequence feature exists on the NVM module. 

  Description:
    This function identifies whether the KeySequence feature is available on the 
	NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_FlashWriteKeySequence
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The KeySequence feature is supported on the device
    - false  - The KeySequence feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsKeySequence( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsSourceAddress( NVM_MODULE_ID index )

  Summary:
    Identifies whether the SourceAddress feature exists on the NVM module. 

  Description:
    This function identifies whether the SourceAddress feature is available on 
	the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_DataBlockSourceAddress
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SourceAddress feature is supported on the device
    - false  - The SourceAddress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsSourceAddress( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsLowVoltageStatus( NVM_MODULE_ID index )

  Summary:
    Identifies whether the LowVoltageStatus feature exists on the NVM module. 

  Description:
    This function identifies whether the LowVoltageStatus feature is available 
	on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_LowVoltageEventIsActive
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LowVoltageStatus feature is supported on the device
    - false  - The LowVoltageStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsLowVoltageStatus( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsLowVoltageError( NVM_MODULE_ID index )

  Summary:
    Identifies whether the LowVoltageError feature exists on the NVM module. 

  Description:
    This function identifies whether the LowVoltageStatus feature is available on 
	the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_LowVoltageIsDetected
    This operation is atomic.	

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LowVoltageStatus feature is supported on the device
    - false  - The LowVoltageStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsLowVoltageError( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsFlashBankRegionSelect( NVM_MODULE_ID index )

  Summary:
    Identifies whether the FlashBankRegionSelect feature exists on the NVM module. 

  Description:
    This function identifies whether the FlashBankRegionSelect feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_ProgramFlashBank1LowerRegion
    - PLIB_NVM_ProgramFlashBank2LowerRegion
    - PLIB_NVM_ProgramFlashBank2IsLowerRegion
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FlashBankRegionSelect feature is supported on the device
    - false  - The FlashBankRegionSelect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsFlashBankRegionSelect( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsBootFlashBankRegion( NVM_MODULE_ID index )

  Summary:
    Identifies whether the BootFlashBankRegion feature exists on the NVM module. 

  Description:
    This function identifies whether the BootFlashBankRegion feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_BootFlashBank1LowerRegion
    - PLIB_NVM_BootFlashBank2LowerRegion
    - PLIB_NVM_BootFlashBank2IsLowerRegion
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BootFlashBankRegion feature is supported on the device
    - false  - The BootFlashBankRegion feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsBootFlashBankRegion( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsProvideQuadData( NVM_MODULE_ID index )

  Summary:
    Identifies whether the ProvideQuadData feature exists on the NVM module. 

  Description:
    This function identifies whether the ProvideQuadData feature is available on 
	the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_FlashProvideQuadData
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ProvideQuadData feature is supported on the device
    - false  - The ProvideQuadData feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsProvideQuadData( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsLockPFMSelect( NVM_MODULE_ID index )

  Summary:
    Identifies whether the LockPFMSelect feature exists on the NVM module. 

  Description:
    This function identifies whether the LockPFMSelect feature is available on 
	the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_LockProgramFlashMemory
    - PLIB_NVM_IsProgramFlashMemoryLocked
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LockPFMSelect feature is supported on the device
    - false  - The LockPFMSelect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsLockPFMSelect( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsFlashWPMemoryRangeProvide( NVM_MODULE_ID index )

  Summary:
    Identifies whether the FlashWPMemoryRangeProvide feature exists on the NVM module. 

  Description:
    This function identifies whether the FlashWPMemoryRangeProvide feature is available 
	on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_FlashWriteProtectMemoryAreaRange
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FlashWPMemoryRangeProvide feature is supported on the device
    - false  - The FlashWPMemoryRangeProvide feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsFlashWPMemoryRangeProvide( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsLockBootSelect( NVM_MODULE_ID index )

  Summary:
    Identifies whether the LockBootSelect feature exists on the NVM module. 

  Description:
    This function identifies whether the LockBootSelect feature is available on 
	the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_LockBootMemory
    - PLIB_NVM_IsBootMemoryLocked
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LockBootSelect feature is supported on the device
    - false  - The LockBootSelect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsLockBootSelect( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsBootPageWriteProtect( NVM_MODULE_ID index )

  Summary:
    Identifies whether the BootPageWriteProtect feature exists on the NVM module. 

  Description:
    This function identifies whether the BootPageWriteProtect feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_BootPageWriteProtectionEnable
    - PLIB_NVM_BootPageWriteProtectionDisable
    - PLIB_NVM_IsBootPageWriteProtected
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BootPageWriteProtect feature is supported on the device
    - false  - The BootPageWriteProtect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsBootPageWriteProtect( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsSwapLockControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the SwapLockControl feature exists on the NVM module. 

  Description:
    This function identifies whether the SwapLockControl feature is available on 
	the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_FlashSwapLockSelect
    - PLIB_NVM_FlashSwapLockStatusGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SwapLockControl feature is supported on the device
    - false  - The SwapLockControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsSwapLockControl( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMEnableControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMEnableControl feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMEnableControl feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMEnable
    - PLIB_NVM_EEPROMDisable
    - PLIB_NVM_EEPROMIsReady
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMEnableControl feature is supported on the device
    - false  - The EEPROMEnableControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMEnableControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMStopInIdleControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMStopInIdleControl feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMStopInIdleControl feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMStopInIdleEnable
    - PLIB_NVM_EEPROMStopInIdleDisable
    - PLIB_NVM_EEPROMStopInIdleIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMStopInIdleControl feature is supported on the device
    - false  - The EEPROMStopInIdleControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMStopInIdleControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMOperationModeControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMOperationModeControl feature exists on the NVM 
	module. 

  Description:
    This function identifies whether the EEPROMOperationModeControl feature is 
	available on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_EEPROMOperationSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMOperationModeControl feature is supported on the device
    - false  - The EEPROMOperationModeControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMOperationModeControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMAddressControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMAddressControl feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMAddressControl feature is available 
	on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_EEPROMAddress
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMAddressControl feature is supported on the device
    - false  - The EEPROMAddressControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMAddressControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMDataControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMDataControl feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMDataControl feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMDataToWrite
	- PLIB_NVM_EEPROMRead
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMDataControl feature is supported on the device
    - false  - The EEPROMDataControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMDataControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMKeySequence( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMKeySequence feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMKeySequence feature is available 
	on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_EEPROMKeySequenceWrite
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMKeySequence feature is supported on the device
    - false  - The EEPROMKeySequence feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMKeySequence( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMEnableOperationControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMEnableOperationControl feature exists on the 
	NVM module. 

  Description:
    This function identifies whether the EEPROMEnableOperationControl feature is 
	available on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMWriteEnable
    - PLIB_NVM_EEPROMWriteIsEnabled
    - PLIB_NVM_EEPROMReadEnable
    - PLIB_NVM_EEPROMReadIsEnabled
    This operation is atomic.	

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMEnableOperationControl feature is supported on the device
    - false  - The EEPROMEnableOperationControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMEnableOperationControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMStartOperationControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMStartOperationControl feature exists on the 
	NVM module. 

  Description:
    This function identifies whether the EEPROMStartOperationControl feature is 
	available on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMReadStart	
    - PLIB_NVM_EEPROMWriteStart
    - PLIB_NVM_EEPROMEraseStart
    - PLIB_NVM_EEPROMOperationHasCompleted
    This operation is atomic.	

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMStartOperationControl feature is supported on the device
    - false  - The EEPROMStartOperationControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMStartOperationControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMLongWriteStatus( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMLongWriteStatus feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMLongWriteStatus feature is available 
	on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_EEPROMNextWriteCycleIsLong
    This operation is atomic.	

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMLongWriteStatus feature is supported on the device
    - false  - The EEPROMLongWriteStatus feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMLongWriteStatus( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMOperationAbortControl( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMOperationAbortControl feature exists on the NVM 
	module. 

  Description:
    This function identifies whether the EEPROMOperationAbortControl feature is 
	available on the NVM module.
    When this function returns true, this function is supported on the device: 
    - PLIB_NVM_EEPROMOperationAbort
    This operation is atomic.	

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMOperationAbortControl feature is supported on the device
    - false  - The EEPROMOperationAbortControl feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMOperationAbortControl( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMErrorStatus( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMErrorStatus feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMErrorStatus feature is available 
	on the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMErrorGet	
    - PLIB_NVM_EEPROMErrorClear
    This operation is atomic.	

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMErrorStatus feature is supported on the device
    - false  - The EEPROMErrorStatus feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMErrorStatus( NVM_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMCalibrationData( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMCalibrationData feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMCalibrationData feature is available on
    the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMReadCalibrationData
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMCalibrationData feature is supported on the device
    - false  - The EEPROMCalibrationData feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_NVM_ExistsEEPROMCalibrationData ( NVM_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_NVM_ExistsEEPROMWaitStates( NVM_MODULE_ID index )

  Summary:
    Identifies whether the EEPROMWaitStates feature exists on the NVM module. 

  Description:
    This function identifies whether the EEPROMWaitStates feature is available on
    the NVM module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_NVM_EEPROMWaitStatesGet
    - PLIB_NVM_EEPROMWaitStatesSet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EEPROMWaitStates feature is supported on the device
    - false  - The EEPROMWaitStates feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_NVM_ExistsEEPROMWaitStates ( NVM_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // #ifndef _PLIB_NVM_H
/******************************************************************************
 End of File
 */
