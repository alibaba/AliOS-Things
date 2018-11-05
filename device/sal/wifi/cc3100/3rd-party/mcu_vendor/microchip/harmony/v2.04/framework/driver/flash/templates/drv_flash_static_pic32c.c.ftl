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
    drv_flash_static_pic32c.c

  Summary:
    FLASH Driver Interface Definition

  Description:
    The FLASH Driver provides an interface to access the FLASH on the PIC32C
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
#include "driver/flash/drv_flash_pic32c.h"


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// *****************************************************************************

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */
void DRV_FLASH0_WriteRow(uint32_t flashAddr,  uint32_t *sourceData)
{
	volatile uint16_t page_number;
    SYS_INT_PROCESSOR_STATUS processor_status;

    processor_status = SYS_INT_StatusGetAndDisable();

	/*Calculate the Page number to be passed for FARG register*/
	page_number = ((uint32_t) flashAddr - IFLASH_ADDR) / DRV_FLASH_ROW_SIZE;

	for (uint32_t i = 0; i < DRV_FLASH_ROW_SIZE; i += 4)
	{
		*((uint32_t *)( IFLASH_ADDR + ( page_number * DRV_FLASH_ROW_SIZE ) + i )) = *sourceData++;
	}

	/* Issue the Flash write operation*/
	_EFC_REGS->EEFC_FCR.w = (EEFC_FCR_FCMD_WP | EEFC_FCR_FARG(page_number)| EEFC_FCR_FKEY_PASSWD);

    SYS_INT_StatusRestore(processor_status);
}

void DRV_FLASH0_ErasePage(uint32_t flashAddr)
{
    volatile uint16_t page_number;
    SYS_INT_PROCESSOR_STATUS processor_status;

    processor_status = SYS_INT_StatusGetAndDisable();

    /*Calculate the Page number to be passed for FARG register*/
    page_number = (flashAddr - IFLASH_ADDR) / DRV_FLASH_ROW_SIZE;

    /* Issue the Flash Erase operation. Passing 0x2 will erase 16 Pages. */
    _EFC_REGS->EEFC_FCR.w = (EEFC_FCR_FCMD_EPA | EEFC_FCR_FARG(page_number|0x2) | EEFC_FCR_FKEY_PASSWD);

    SYS_INT_StatusRestore(processor_status);
}

bool DRV_FLASH0_IsBusy( )
{
   return ((uint8_t)!_EFC_REGS->EEFC_FSR.w & EEFC_FSR_FRDY_Msk);
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



