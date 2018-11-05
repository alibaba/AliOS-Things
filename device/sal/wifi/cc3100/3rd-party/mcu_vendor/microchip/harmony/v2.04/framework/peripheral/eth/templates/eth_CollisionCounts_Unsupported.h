/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_CollisionCounts_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : CollisionCounts
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_SingleCollisionCountClear
        PLIB_ETH_SingleCollisionCountGet
        PLIB_ETH_MultipleCollisionCountClear
        PLIB_ETH_MultipleCollisionCountGet
        PLIB_ETH_ExistsCollisionCounts

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

#ifndef _ETH_COLLISIONCOUNTS_UNSUPPORTED_H
#define _ETH_COLLISIONCOUNTS_UNSUPPORTED_H

//******************************************************************************
/* Function :  ETH_SingleCollisionCountClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_SingleCollisionCountClear 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_SingleCollisionCountClear function.
*/

PLIB_TEMPLATE void ETH_SingleCollisionCountClear_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_SingleCollisionCountClear");
}


//******************************************************************************
/* Function :  ETH_SingleCollisionCountGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_SingleCollisionCountGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_SingleCollisionCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_SingleCollisionCountGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_SingleCollisionCountGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_MultipleCollisionCountClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MultipleCollisionCountClear 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MultipleCollisionCountClear function.
*/

PLIB_TEMPLATE void ETH_MultipleCollisionCountClear_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MultipleCollisionCountClear");
}


//******************************************************************************
/* Function :  ETH_MultipleCollisionCountGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MultipleCollisionCountGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MultipleCollisionCountGet function.
*/

PLIB_TEMPLATE uint16_t ETH_MultipleCollisionCountGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MultipleCollisionCountGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_ExistsCollisionCounts_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsCollisionCounts

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsCollisionCounts function.
*/

PLIB_TEMPLATE bool ETH_ExistsCollisionCounts_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_COLLISIONCOUNTS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

