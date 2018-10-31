/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_FramexReceivedOK_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : FramexReceivedOK
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_FramesRxdOkCountClear
        PLIB_ETH_FramesRxdOkCountGet
        PLIB_ETH_ExistsFramexReceivedOK

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

#ifndef _ETH_FRAMEXRECEIVEDOK_UNSUPPORTED_H
#define _ETH_FRAMEXRECEIVEDOK_UNSUPPORTED_H


//******************************************************************************
/* Function :  ETH_FramesRxdOkCountClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FramesRxdOkCountClear 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FramesRxdOkCountClear function.
*/

PLIB_TEMPLATE void ETH_FramesRxdOkCountClear_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FramesRxdOkCountClear");
}


//******************************************************************************
/* Function :  ETH_FramesRxdOkCountGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FramesRxdOkCountGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FramesRxdOkCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_FramesRxdOkCountGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FramesRxdOkCountGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_ExistsFramexReceivedOK_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsFramexReceivedOK

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsFramexReceivedOK function.
*/

PLIB_TEMPLATE bool ETH_ExistsFramexReceivedOK_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_FRAMEXRECEIVEDOK_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

