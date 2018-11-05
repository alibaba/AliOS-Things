/*******************************************************************************
  INT Peripheral Library Template Implementation

  File Name:
    int_VectorPriority_Unsupported.h

  Summary:
    INT PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : VectorPriority
    and its Variant : Unsupported
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

#ifndef _INT_VECTORPRIORITY_UNSUPPORTED_H
#define _INT_VECTORPRIORITY_UNSUPPORTED_H

//******************************************************************************
/* Function :  INT_ExistsVectorPriority_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_ExistsVectorPriority

  Description:
    This template implements the Unsupported variant of the PLIB_INT_ExistsVectorPriority function.
*/

PLIB_TEMPLATE bool INT_ExistsVectorPriority_Unsupported( INT_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  INT_VectorPrioritySet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_VectorPrioritySet 

  Description:
    This template implements the Unsupported variant of the PLIB_INT_VectorPrioritySet function.
*/

PLIB_TEMPLATE void INT_VectorPrioritySet_Unsupported( INT_MODULE_ID index , INT_VECTOR vector , INT_PRIORITY_LEVEL priority )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_INT_VectorPrioritySet");
}


//******************************************************************************
/* Function :  INT_VectorPriorityGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_VectorPriorityGet 

  Description:
    This template implements the Unsupported variant of the PLIB_INT_VectorPriorityGet function.
*/

PLIB_TEMPLATE INT_PRIORITY_LEVEL INT_VectorPriorityGet_Unsupported( INT_MODULE_ID index , INT_VECTOR vector )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_INT_VectorPriorityGet");

    return (INT_PRIORITY_LEVEL) 0;
}


//******************************************************************************
/* Function :  INT_VectorSubPrioritySet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_VectorSubPrioritySet 

  Description:
    This template implements the Unsupported variant of the PLIB_INT_VectorSubPrioritySet function.
*/

PLIB_TEMPLATE void INT_VectorSubPrioritySet_Unsupported( INT_MODULE_ID index , INT_VECTOR vector , INT_SUBPRIORITY_LEVEL subPriority )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_INT_VectorSubPrioritySet");
}


//******************************************************************************
/* Function :  INT_VectorSubPriorityGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_VectorSubPriorityGet 

  Description:
    This template implements the Unsupported variant of the PLIB_INT_VectorSubPriorityGet function.
*/

PLIB_TEMPLATE INT_SUBPRIORITY_LEVEL INT_VectorSubPriorityGet_Unsupported( INT_MODULE_ID index , INT_VECTOR vector )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_INT_VectorSubPriorityGet");

    return (INT_SUBPRIORITY_LEVEL) 0;
}


#endif /*_INT_VECTORPRIORITY_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

