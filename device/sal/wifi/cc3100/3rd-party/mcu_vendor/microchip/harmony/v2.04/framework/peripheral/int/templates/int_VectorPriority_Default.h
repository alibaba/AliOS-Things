/*******************************************************************************
  INT Peripheral Library Template Implementation

  File Name:
    int_VectorPriority_Default.h

  Summary:
    INT PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : VectorPriority
    and its Variant : Default
    For following APIs :
        PLIB_INT_ExistsVectorPriority
        PLIB_INT_VectorPrioritySet
        PLIB_INT_VectorPriorityGet
        PLIB_INT_VectorSubPrioritySet
        PLIB_INT_VectorSubPriorityGet

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

#ifndef _INT_VECTORPRIORITY_DEFAULT_H
#define _INT_VECTORPRIORITY_DEFAULT_H

//******************************************************************************
/* Function :  INT_ExistsVectorPriority_Default

  Summary:
    Implements Default variant of PLIB_INT_ExistsVectorPriority

  Description:
    This template implements the Default variant of the PLIB_INT_ExistsVectorPriority function.
*/
#define PLIB_INT_ExistsVectorPriority PLIB_INT_ExistsVectorPriority
PLIB_TEMPLATE bool INT_ExistsVectorPriority_Default( INT_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  INT_VectorPrioritySet_Default

  Summary:
    Implements Default variant of PLIB_INT_VectorPrioritySet 

  Description:
    This template implements the Default variant of the PLIB_INT_VectorPrioritySet function.

  Note:
    The algorithm to calculate and write the field location has changed to use
    actual vector numbers. The enumeration for INT_VECTOR should now match the
    names to the actual vector numbers.
*/
PLIB_TEMPLATE void INT_VectorPrioritySet_Default( INT_MODULE_ID index , INT_VECTOR vector , INT_PRIORITY_LEVEL priority )
{
    volatile uint32_t *IPCx = (volatile uint32_t *)(&IPC0 + ((0x10 * (vector >> 2)) >> 2));
    uint32_t mask = 0x07 << ((vector & 0x03) * 8 + 2);
    uint32_t position = (vector & 0x03) * 8 + 2;

    /* read-modify-write */
    *IPCx &= ~mask;
    *IPCx |= (priority << position) & mask;
}

//******************************************************************************
/* Function :  INT_VectorPriorityGet_Default

  Summary:
    Implements Default variant of PLIB_INT_VectorPriorityGet 

  Description:
    This template implements the Default variant of the PLIB_INT_VectorPriorityGet function.

  Note:
    The algorithm to calculate and write the field location has changed to use
    actual vector numbers. The enumeration for INT_VECTOR should now match the
    names to the actual vector numbers.
*/
PLIB_TEMPLATE INT_PRIORITY_LEVEL INT_VectorPriorityGet_Default( INT_MODULE_ID index , INT_VECTOR vector )
{
    volatile uint32_t *IPCx = (volatile uint32_t *)(&IPC0 + ((0x10 * (vector >> 2)) >> 2));
    uint32_t mask = 0x07 << (((vector & 0x03) * 8) + 2);
    uint32_t position = (((vector & 0x03) * 8) + 2);

    return (INT_PRIORITY_LEVEL) ((*IPCx & mask) >> position);
}

//******************************************************************************
/* Function :  INT_VectorSubPrioritySet_Default

  Summary:
    Implements Default variant of PLIB_INT_VectorSubPrioritySet 

  Description:
    This template implements the Default variant of the PLIB_INT_VectorSubPrioritySet function.

  Note:
    The algorithm to calculate and write the field location has changed to use
    actual vector numbers. The enumeration for INT_VECTOR should now match the
    names to the actual vector numbers.
*/
PLIB_TEMPLATE void INT_VectorSubPrioritySet_Default( INT_MODULE_ID index , INT_VECTOR vector , INT_SUBPRIORITY_LEVEL subPriority )
{
    volatile uint32_t *IPCx = (volatile uint32_t *)(&IPC0 + ((0x10 * (vector >> 2)) >> 2));
    uint32_t mask = 0x03 << ((vector & 0x03) * 8);
    uint32_t position = (vector & 0x03) * 8;

    /* read-modify-write */
    *IPCx &= ~mask; /* zeroed mask */
    *IPCx |= (subPriority << position) & mask; /* set value */
}

//******************************************************************************
/* Function :  INT_VectorSubPriorityGet_Default

  Summary:
    Implements Default variant of PLIB_INT_VectorSubPriorityGet 

  Description:
    This template implements the Default variant of the PLIB_INT_VectorSubPriorityGet function.

  Note:
    The algorithm to calculate and write the field location has changed to use
    actual vector numbers. The enumeration for INT_VECTOR should now match the
    names to the actual vector numbers.
*/
PLIB_TEMPLATE INT_SUBPRIORITY_LEVEL INT_VectorSubPriorityGet_Default( INT_MODULE_ID index , INT_VECTOR vector )
{
    volatile uint32_t *IPCx = (volatile uint32_t *)(&IPC0 + ((0x10 * (vector >> 2)) >> 2));
    uint32_t mask = 0x03 << ((vector & 0x03) * 8);
    uint32_t position = (vector & 0x03) * 8;

    return (INT_SUBPRIORITY_LEVEL) ((*IPCx & mask) >> position);
}

#endif /*_INT_VECTORPRIORITY_DEFAULT_H*/

/******************************************************************************
 End of File
*/

