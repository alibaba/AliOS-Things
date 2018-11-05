<#--
/*******************************************************************************
  FLASH Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_flash.h.ftl

  Summary:
    FLASH Driver Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->

/*** FLASH Driver Configuration ***/

<#if CONFIG_USE_DRV_FLASH == true>

/*******************************************************************************
  FLASH Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_flash_static.h

  Summary:
    FLASH driver interface declarations for the static single instance driver

  Description:
    The FLASH driver provides a simple interface to manage the on chip Non 
	Volatile Flash Memory on Microchip microcontrollers. This file defines the 
	interface definition for the FLASH driver.
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
#ifndef _DRV_FLASH_STATIC_H
#define _DRV_FLASH_STATIC_H

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
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************
    
    
    /***************************************************************************
     * With some excpetions for dioffereing parts, the basic flash operations 
     * that this driver supports are as follows:
     * NVM Operation bits
     * 0100 = Page erase operation: erases page selected by NVMADDR, 
     *        if it is not write-protected 
     * 0011 = Row program operation: programs row selected by NVMADDR, if it is 
     *        not write-protected 
     * 0010 = Quad Word (128-bit) program operation: programs the 128-bit 
     *        Flash word selected by NVMADDR, if it is not write-protected
     * 0001 = Word program operation: programs word selected by NVMADDR, if it 
     *        is not write-protected(2)
     * 
     **************************************************************************/

	#define DRV_FLASH_GetRowSize(handle) DRV_FLASH_ROW_SIZE
	#define DRV_FLASH_GetPageSize(handle) DRV_FLASH_PAGE_SIZE
    
	void DRV_FLASH0_WriteWord(uint32_t flashAddr,  uint32_t sourceData);
	#define DRV_FLASH_WriteWord(handle, flashAddr,  sourceData) DRV_FLASH0_WriteWord(flashAddr, sourceData)

	<#if CONFIG_PIC32MZ == true>
	void DRV_FLASH0_WriteQuadWord(uint32_t flashAddr,  uint32_t *sourceData);
	#define DRV_FLASH_WriteQuadWord(handle, flashAddr,  sourceData) DRV_FLASH0_WriteQuadWord(flashAddr, sourceData)
	</#if>

	void DRV_FLASH0_WriteRow(uint32_t flashAddr,  uint32_t *sourceData);
	#define DRV_FLASH_WriteRow(handle, flashAddr,  sourceData) DRV_FLASH0_WriteRow(flashAddr, sourceData)

    void DRV_FLASH0_ErasePage(uint32_t flashAddr);
    #define DRV_FLASH_ErasePage(handle, flashAddr) DRV_FLASH0_ErasePage(flashAddr)

    bool DRV_FLASH0_IsBusy( void );
    #define DRV_FLASH_IsBusy( handle ) DRV_FLASH0_IsBusy( )
    #define DRV_FLASH_Initialize(index, init ) SYS_MODULE_OBJ_STATIC
	#define DRV_FLASH_Open(index, intent) SYS_MODULE_OBJ_STATIC
	
	
#ifdef __cplusplus
}
#endif

#endif // #ifndef _DRV_FLASH_H
/*******************************************************************************
 End of File
*/

</#if> <#-- CONFIG_USE_DRV_FLASH == true>-->


<#--
/*******************************************************************************
 End of File
*/
-->

