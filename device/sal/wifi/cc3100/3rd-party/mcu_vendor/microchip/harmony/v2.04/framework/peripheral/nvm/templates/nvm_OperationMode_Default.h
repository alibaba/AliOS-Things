/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_OperationMode_Default.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : OperationMode
    and its Variant : Default
    For following APIs :
        PLIB_NVM_ExistsOperationMode
        PLIB_NVM_MemoryOperationSelect

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _NVM_OPERATIONMODE_DEFAULT_H
#define _NVM_OPERATIONMODE_DEFAULT_H

//******************************************************************************
/* Function :  NVM_ExistsOperationMode_Default

  Summary:
    Implements Default variant of PLIB_NVM_ExistsOperationMode

  Description:
    This template implements the Default variant of the PLIB_NVM_ExistsOperationMode function.
*/

#define PLIB_NVM_ExistsOperationMode PLIB_NVM_ExistsOperationMode
PLIB_TEMPLATE bool NVM_ExistsOperationMode_Default( NVM_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  NVM_MemoryOperationSelect_Default

  Summary:
    Implements Default variant of PLIB_NVM_MemoryOperationSelect 

  Description:
    This template implements the Default variant of the PLIB_NVM_MemoryOperationSelect function.
*/

PLIB_TEMPLATE void NVM_MemoryOperationSelect_Default( NVM_MODULE_ID index , NVM_OPERATION_MODE	operationmode )
{
    /* Clear and Set, as NVMCON contains status bits and 
       hence need to be accessed atomically.
       Using bit field access may erroneously cause status bits to get cleared */
    NVMCONCLR = _NVMCON_NVMOP_MASK;
    NVMCONSET = ( _NVMCON_NVMOP_MASK & (((uint32_t)operationmode) << _NVMCON_NVMOP_POSITION) );
}


#endif /*_NVM_OPERATIONMODE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

