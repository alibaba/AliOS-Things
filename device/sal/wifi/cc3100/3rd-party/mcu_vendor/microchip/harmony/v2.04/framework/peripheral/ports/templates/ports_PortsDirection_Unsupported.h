/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_PortsDirection_Unsupported.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PortsDirection
    and its Variant : Unsupported
    For following APIs :
        PLIB_PORTS_PinDirectionInputSet
        PLIB_PORTS_PinDirectionOutputSet
        PLIB_PORTS_DirectionInputSet
        PLIB_PORTS_DirectionOutputSet
        PLIB_PORTS_DirectionGet
        PLIB_PORTS_ExistsPortsDirection

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

#ifndef _PORTS_PORTSDIRECTION_UNSUPPORTED_H
#define _PORTS_PORTSDIRECTION_UNSUPPORTED_H



//******************************************************************************
/* Function :  PORTS_PinDirectionInputSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinDirectionInputSet 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinDirectionInputSet function.
*/

PLIB_TEMPLATE void PORTS_PinDirectionInputSet_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinDirectionInputSet");
}


//******************************************************************************
/* Function :  PORTS_PinDirectionOutputSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinDirectionOutputSet 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinDirectionOutputSet function.
*/

PLIB_TEMPLATE void PORTS_PinDirectionOutputSet_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinDirectionOutputSet");
}


//******************************************************************************
/* Function :  PORTS_DirectionInputSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_DirectionInputSet 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_DirectionInputSet function.
*/

PLIB_TEMPLATE void PORTS_DirectionInputSet_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_MASK mask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_DirectionInputSet");
}


//******************************************************************************
/* Function :  PORTS_DirectionOutputSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_DirectionOutputSet 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_DirectionOutputSet function.
*/

PLIB_TEMPLATE void PORTS_DirectionOutputSet_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_MASK mask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_DirectionOutputSet");
}


//******************************************************************************
/* Function :  PORTS_DirectionGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_DirectionGet 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_DirectionGet function.
*/

PLIB_TEMPLATE PORTS_DATA_MASK PORTS_DirectionGet_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_DirectionGet");

    return (PORTS_DATA_MASK) 0;
}


//******************************************************************************
/* Function :  PORTS_ExistsPortsDirection_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ExistsPortsDirection

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ExistsPortsDirection function.
*/

PLIB_TEMPLATE bool PORTS_ExistsPortsDirection_Unsupported( PORTS_MODULE_ID index )
{
    return false;
}


#endif /*_PORTS_PORTSDIRECTION_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

