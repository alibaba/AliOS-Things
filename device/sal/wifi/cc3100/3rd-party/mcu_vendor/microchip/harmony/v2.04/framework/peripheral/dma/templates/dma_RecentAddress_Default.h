/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_RecentAddress_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : RecentAddress
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsRecentAddress
        PLIB_DMA_RecentAddressAccessed

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

#ifndef _DMA_RECENTADDRESS_DEFAULT_H
#define _DMA_RECENTADDRESS_DEFAULT_H

#include <sys/kmem.h>
#include "dma_Registers.h"
#define ConvertToVirtualAddress(a)  PA_TO_KVA1(a)


//******************************************************************************
/* Function :  DMA_ExistsRecentAddress_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsRecentAddress

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsRecentAddress function.
*/

#define PLIB_DMA_ExistsRecentAddress PLIB_DMA_ExistsRecentAddress
PLIB_TEMPLATE bool DMA_ExistsRecentAddress_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_RecentAddressAccessed_Default

  Summary:
    Implements Default variant of PLIB_DMA_RecentAddressAccessed

  Description:
    This template implements the Default variant of the PLIB_DMA_RecentAddressAccessed function.
    Operation is atomic.
*/

PLIB_TEMPLATE uint32_t DMA_RecentAddressAccessed_Default( DMA_MODULE_ID index )
{
    volatile dma_register_t *regs = (dma_register_t *)index;

    return (uint32_t) ConvertToVirtualAddress(regs->DMAADDR);
}


#endif /*_DMA_RECENTADDRESS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

