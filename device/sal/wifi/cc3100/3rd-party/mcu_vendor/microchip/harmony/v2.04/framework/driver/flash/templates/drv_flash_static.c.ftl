<#--
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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
 -->

<#if CONFIG_USE_DRV_FLASH == true>
/*******************************************************************************
  FLASH Driver Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_flash_static.c

  Summary:
    FLASH Driver Interface Definition

  Description:
    The FLASH Driver provides an interface to access the FLASH on the PIC32
    microcontrollers. This file implements a minimally functional interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "driver/flash/drv_flash.h"


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// *****************************************************************************
/* FLASH Driver Program Unlock Key 1

  Summary:
    Specifies the FLASH Driver Program Unlock Key 1

  Description:
    This definition specifies the FLASH Driver Program Unlock Key 1 parameter is
    device specific and is obtained from the device specific processor
    header file.

  Remarks:
    None
*/
#define FLASH_PROGRAM_UNLOCK_KEY1                 (NVM_UNLOCK_KEY1)

// *****************************************************************************
/* FLASH Driver Program Unlock Key 2

  Summary:
    Specifies the FLASH Driver Program Unlock Key 2

  Description:
    This definition specifies the FLASH Driver Program Unlock Key 2 parameter is
    device specific and is obtained from the device specific processor header
    file.

  Remarks:
    None
*/
#define FLASH_PROGRAM_UNLOCK_KEY2                 (NVM_UNLOCK_KEY2)

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

void FLASH_StartOperationAtAddress( uint32_t flashAddr,  NVM_OPERATION_MODE operation) {
    // Base address of flash to be written to (destination)
   PLIB_NVM_FlashAddressToModify(NVM_ID_0, KVA_TO_PA(flashAddr));

   // Disable flash write/erase operations
   PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);

   // Select row write function & enable flash write/erase operations
   PLIB_NVM_MemoryOperationSelect(NVM_ID_0, operation);

   // Allow memory modifications
   PLIB_NVM_MemoryModifyEnable(NVM_ID_0);

   // Write the unlock key sequence
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0x0);
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, FLASH_PROGRAM_UNLOCK_KEY1);
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, FLASH_PROGRAM_UNLOCK_KEY2);

   // Start the operation
   PLIB_NVM_FlashWriteStart(NVM_ID_0);

   // It is assumed that when the flash is busy, no operations are possible.
   // That is, the cpu is blocked globally anyway.
   // The exception to this is if code can run in cache, or if code might be 
   // able to run in a second flash panel.
}   

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */
void DRV_FLASH0_WriteWord(uint32_t flashAddr,  uint32_t sourceData) {
   PLIB_NVM_FlashProvideData(NVM_ID_0, sourceData);
   FLASH_StartOperationAtAddress( flashAddr,  WORD_PROGRAM_OPERATION);
}

<#if CONFIG_PIC32MZ == true>
void DRV_FLASH0_WriteQuadWord(uint32_t flashAddr,  uint32_t *sourceData) {
   PLIB_NVM_FlashProvideQuadData(NVM_ID_0, sourceData);
   FLASH_StartOperationAtAddress( flashAddr,  QUAD_WORD_PROGRAM_OPERATION);
}
</#if>

void DRV_FLASH0_WriteRow(uint32_t flashAddr,  uint32_t *sourceData) {
   PLIB_NVM_DataBlockSourceAddress(NVM_ID_0, KVA_TO_PA(sourceData));
   FLASH_StartOperationAtAddress( flashAddr,  ROW_PROGRAM_OPERATION);
}

void DRV_FLASH0_ErasePage(uint32_t flashAddr) {
   FLASH_StartOperationAtAddress(flashAddr,  PAGE_ERASE_OPERATION);
}

bool DRV_FLASH0_IsBusy( ) {
   return !PLIB_NVM_FlashWriteCycleHasCompleted(NVM_ID_0);
}


/* *****************************************************************************
 End of File
 */

</#if>
<#--
/*******************************************************************************
 End of File
*/
-->



