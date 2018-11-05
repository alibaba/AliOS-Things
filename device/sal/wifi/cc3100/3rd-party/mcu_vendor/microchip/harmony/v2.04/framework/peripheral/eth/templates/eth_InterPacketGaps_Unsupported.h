/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_InterPacketGaps_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : InterPacketGaps
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_BackToBackIPGGet
        PLIB_ETH_BackToBackIPGSet
        PLIB_ETH_NonBackToBackIPG1Get
        PLIB_ETH_NonBackToBackIPG1Set
        PLIB_ETH_NonBackToBackIPG2Get
        PLIB_ETH_NonBackToBackIPG2Set
        PLIB_ETH_ExistsInterPacketGaps

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

#ifndef _ETH_INTERPACKETGAPS_UNSUPPORTED_H
#define _ETH_INTERPACKETGAPS_UNSUPPORTED_H

//******************************************************************************
/* Function :  ETH_BackToBackIPGGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_BackToBackIPGGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_BackToBackIPGGet function.
*/

PLIB_TEMPLATE uint8_t ETH_BackToBackIPGGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_BackToBackIPGGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_BackToBackIPGSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_BackToBackIPGSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_BackToBackIPGSet function.
*/

PLIB_TEMPLATE void ETH_BackToBackIPGSet_Unsupported( ETH_MODULE_ID index , uint8_t backToBackIPGValue )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_BackToBackIPGSet");
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG1Get_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_NonBackToBackIPG1Get 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_NonBackToBackIPG1Get function.
*/

PLIB_TEMPLATE uint8_t ETH_NonBackToBackIPG1Get_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_NonBackToBackIPG1Get");

    return 0;
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG1Set_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_NonBackToBackIPG1Set 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_NonBackToBackIPG1Set function.
*/

PLIB_TEMPLATE void ETH_NonBackToBackIPG1Set_Unsupported( ETH_MODULE_ID index , uint8_t nonBackToBackIPGValue )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_NonBackToBackIPG1Set");
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG2Get_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_NonBackToBackIPG2Get 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_NonBackToBackIPG2Get function.
*/

PLIB_TEMPLATE uint8_t ETH_NonBackToBackIPG2Get_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_NonBackToBackIPG2Get");

    return 0;
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG2Set_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_NonBackToBackIPG2Set 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_NonBackToBackIPG2Set function.
*/

PLIB_TEMPLATE void ETH_NonBackToBackIPG2Set_Unsupported( ETH_MODULE_ID index , uint8_t nonBackToBackIPGValue )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_NonBackToBackIPG2Set");
}


//******************************************************************************
/* Function :  ETH_ExistsInterPacketGaps_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsInterPacketGaps

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsInterPacketGaps function.
*/

PLIB_TEMPLATE bool ETH_ExistsInterPacketGaps_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_INTERPACKETGAPS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

