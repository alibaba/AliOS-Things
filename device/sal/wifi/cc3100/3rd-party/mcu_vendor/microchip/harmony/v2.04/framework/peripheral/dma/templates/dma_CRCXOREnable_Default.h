/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_CRCXOREnable_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : CRCXOREnable
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsCRCXOREnable
        PLIB_DMA_CRCXOREnableSet
        PLIB_DMA_CRCXOREnableGet

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

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _DMA_CRCXORENABLE_DEFAULT_H
#define _DMA_CRCXORENABLE_DEFAULT_H

#include "dma_Registers.h"

//******************************************************************************
/* Function :  DMA_ExistsCRCXOREnable_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsCRCXOREnable

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsCRCXOREnable function.
*/

#define PLIB_DMA_ExistsCRCXOREnable PLIB_DMA_ExistsCRCXOREnable
PLIB_TEMPLATE bool DMA_ExistsCRCXOREnable_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_CRCXOREnableSet_Default

  Summary:
    Implements Default variant of PLIB_DMA_CRCXOREnableSet

  Description:
    This template implements the Default variant of the PLIB_DMA_CRCXOREnableSet function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_CRCXOREnableSet_Default( DMA_MODULE_ID index , uint32_t DMACRCXOREnableMask )
{
    volatile dma_register_t *regs = (dma_register_t *)index;

    regs->DCRCXOR = DMACRCXOREnableMask;
}


//******************************************************************************
/* Function :  DMA_CRCXOREnableGet_Default

  Summary:
    Implements Default variant of PLIB_DMA_CRCXOREnableGet

  Description:
    This template implements the Default variant of the PLIB_DMA_CRCXOREnableGet function.
    Operation is atomic.
*/

PLIB_TEMPLATE uint32_t DMA_CRCXOREnableGet_Default( DMA_MODULE_ID index )
{
    volatile dma_register_t *regs = (dma_register_t *)index;

    return (uint32_t)regs->DCRCXOR;
}


#endif /*_DMA_CRCXORENABLE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

