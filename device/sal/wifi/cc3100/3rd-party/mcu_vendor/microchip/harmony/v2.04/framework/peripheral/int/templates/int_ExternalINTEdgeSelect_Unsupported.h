/*******************************************************************************
  INT Peripheral Library Template Implementation

  File Name:
    int_ExternalINTEdgeSelect_Unsupported.h

  Summary:
    INT PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ExternalINTEdgeSelect
    and its Variant : Unsupported
    For following APIs :
        PLIB_INT_ExistsExternalINTEdgeSelect
        PLIB_INT_ExternalRisingEdgeSelect
        PLIB_INT_ExternalFallingEdgeSelect

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

#ifndef _INT_EXTERNALINTEDGESELECT_UNSUPPORTED_H
#define _INT_EXTERNALINTEDGESELECT_UNSUPPORTED_H

//******************************************************************************
/* Function :  INT_ExistsExternalINTEdgeSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_ExistsExternalINTEdgeSelect

  Description:
    This template implements the Unsupported variant of the PLIB_INT_ExistsExternalINTEdgeSelect function.
*/

PLIB_TEMPLATE bool INT_ExistsExternalINTEdgeSelect_Unsupported( INT_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  INT_ExternalRisingEdgeSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_ExternalRisingEdgeSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_INT_ExternalRisingEdgeSelect function.
*/

PLIB_TEMPLATE void INT_ExternalRisingEdgeSelect_Unsupported( INT_MODULE_ID index , INT_EXTERNAL_SOURCES source )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_INT_ExternalRisingEdgeSelect");
}


//******************************************************************************
/* Function :  INT_ExternalFallingEdgeSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_INT_ExternalFallingEdgeSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_INT_ExternalFallingEdgeSelect function.
*/

PLIB_TEMPLATE void INT_ExternalFallingEdgeSelect_Unsupported( INT_MODULE_ID index , INT_EXTERNAL_SOURCES source )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_INT_ExternalFallingEdgeSelect");
}


#endif /*_INT_EXTERNALINTEDGESELECT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

