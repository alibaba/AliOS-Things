/*******************************************************************************
  INT Peripheral Library Template Implementation

  File Name:
    int_VariableOffset_Default.h

  Summary:
    INT PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : VariableOffset
    and its Variant : Default
    For following APIs :
        PLIB_INT_ExistsVariableOffset
        PLIB_INT_VariableOffsetSet
        PLIB_INT_VariableOffsetGet

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

#ifndef _INT_VARIABLEOFFSET_DEFAULT_H
#define _INT_VARIABLEOFFSET_DEFAULT_H

//******************************************************************************
/* Function :  INT_ExistsVariableOffset_Default

  Summary:
    Implements Default variant of PLIB_INT_ExistsVariableOffset

  Description:
    This template implements the Default variant of the PLIB_INT_ExistsVariableOffset function.
*/
#define PLIB_INT_ExistsVariableOffset PLIB_INT_ExistsVariableOffset
PLIB_TEMPLATE bool INT_ExistsVariableOffset_Default( INT_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  INT_VariableVectorOffsetSet_Default

  Summary:
    Implements Default variant of PLIB_INT_VariableVectorOffsetSet

  Description:
    This template implements the Default variant of the PLIB_INT_VariableVectorOffsetSet function.
*/
PLIB_TEMPLATE void INT_VariableVectorOffsetSet_Default( INT_MODULE_ID index , INT_VECTOR vector , uint32_t offset )
{
    volatile uint32_t *OFFx = (volatile uint32_t *)(&OFF000 + (vector));

    *OFFx = offset; /* backward compatibility */
}

//******************************************************************************
/* Function :  INT_VariableVectorOffsetGet_Default

  Summary:
    Implements Default variant of PLIB_INT_VariableVectorOffsetGet

  Description:
    This template implements the Default variant of the PLIB_INT_VariableVectorOffsetGet function.
*/
PLIB_TEMPLATE uint32_t INT_VariableVectorOffsetGet_Default( INT_MODULE_ID index , INT_VECTOR vector )
{
    volatile uint32_t *OFFx = (volatile uint32_t *)(&OFF000 + (vector));

    return (uint32_t)*OFFx; /* backward compatibility */
}

#endif /*_INT_VARIABLEOFFSET_DEFAULT_H*/

/******************************************************************************
 End of File
*/

