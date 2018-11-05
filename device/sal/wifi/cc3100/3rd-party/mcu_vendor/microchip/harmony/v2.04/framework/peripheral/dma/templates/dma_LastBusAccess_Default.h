/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_LastBusAccess_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : LastBusAccess
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsLastBusAccess
        PLIB_DMA_LastBusAccessIsRead
        PLIB_DMA_LastBusAccessIsWrite

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

#ifndef _DMA_LASTBUSACCESS_DEFAULT_H
#define _DMA_LASTBUSACCESS_DEFAULT_H

#include "dma_Registers.h"

//******************************************************************************
/* Function :  DMA_ExistsLastBusAccess_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsLastBusAccess

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsLastBusAccess function.
*/

#define PLIB_DMA_ExistsLastBusAccess PLIB_DMA_ExistsLastBusAccess
PLIB_TEMPLATE bool DMA_ExistsLastBusAccess_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_LastBusAccessIsRead_Default

  Summary:
    Implements Default variant of PLIB_DMA_LastBusAccessIsRead

  Description:
    This template implements the Default variant of the PLIB_DMA_LastBusAccessIsRead function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool DMA_LastBusAccessIsRead_Default( DMA_MODULE_ID index )
{
    volatile dma_register_t *regs = (dma_register_t *)index;

    return (bool)regs->DMASTAT.RDWR;
}


//******************************************************************************
/* Function :  DMA_LastBusAccessIsWrite_Default

  Summary:
    Implements Default variant of PLIB_DMA_LastBusAccessIsWrite

  Description:
    This template implements the Default variant of the PLIB_DMA_LastBusAccessIsWrite function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool DMA_LastBusAccessIsWrite_Default( DMA_MODULE_ID index )
{
    volatile dma_register_t *regs = (dma_register_t *)index;

    return (bool)!regs->DMASTAT.RDWR;
}


#endif /*_DMA_LASTBUSACCESS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

