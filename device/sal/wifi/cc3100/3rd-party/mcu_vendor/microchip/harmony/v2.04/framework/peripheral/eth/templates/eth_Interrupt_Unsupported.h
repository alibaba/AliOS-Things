/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_Interrupt_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Interrupt
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_InterruptSourceEnable
        PLIB_ETH_InterruptSourceDisable
        PLIB_ETH_InterruptSourceIsEnabled
        PLIB_ETH_InterruptSet
        PLIB_ETH_InterruptClear
        PLIB_ETH_InterruptStatusGet
        PLIB_ETH_ExistsInterrupt

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

#ifndef _ETH_INTERRUPT_UNSUPPORTED_H
#define _ETH_INTERRUPT_UNSUPPORTED_H

//******************************************************************************
/* Function :  ETH_InterruptSourceEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_InterruptSourceEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_InterruptSourceEnable function.
*/

PLIB_TEMPLATE void ETH_InterruptSourceEnable_Unsupported( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_InterruptSourceEnable");
}


//******************************************************************************
/* Function :  ETH_InterruptSourceDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_InterruptSourceDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_InterruptSourceDisable function.
*/

PLIB_TEMPLATE void ETH_InterruptSourceDisable_Unsupported( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_InterruptSourceDisable");
}


//******************************************************************************
/* Function :  ETH_InterruptSourceIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_InterruptSourceIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_InterruptSourceIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_InterruptSourceIsEnabled_Unsupported( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_InterruptSourceIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_InterruptSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_InterruptSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_InterruptSet function.
*/

PLIB_TEMPLATE void ETH_InterruptSet_Unsupported( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_InterruptSet");
}


//******************************************************************************
/* Function :  ETH_InterruptClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_InterruptClear 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_InterruptClear function.
*/

PLIB_TEMPLATE void ETH_InterruptClear_Unsupported( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_InterruptClear");
}


//******************************************************************************
/* Function :  ETH_InterruptStatusGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_InterruptStatusGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_InterruptStatusGet function.
*/

PLIB_TEMPLATE bool ETH_InterruptStatusGet_Unsupported( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_InterruptStatusGet");

    return false;
}


//******************************************************************************
/* Function :  ETH_ExistsInterrupt_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsInterrupt

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsInterrupt function.
*/

PLIB_TEMPLATE bool ETH_ExistsInterrupt_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_INTERRUPT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

