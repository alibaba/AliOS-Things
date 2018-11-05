/*******************************************************************************
  DEVCON Peripheral Library Template Implementation

  File Name:
    devcon_MPLL_Unsupported.h

  Summary:
    DEVCON PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MPLL
    and its Variant : Unsupported
    For following APIs :
        PLIB_DEVCON_MPLLIsReady
        PLIB_DEVCON_MPLLEnable
        PLIB_DEVCON_MPLLDisable
        PLIB_DEVCON_MPLLODiv1Set
        PLIB_DEVCON_MPLLODiv2Set
        PLIB_DEVCON_MPLLVregIsReady
        PLIB_DEVCON_MPLLVregEnable
        PLIB_DEVCON_MPLLVregDisable
        PLIB_DEVCON_MPLLMultiplierSet
        PLIB_DEVCON_MPLLVrefSet
        PLIB_DEVCON_MPLLInputDivSet
        PLIB_DEVCON_ExistsMPLL

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DEVCON_MPLL_UNSUPPORTED_H
#define _DEVCON_MPLL_UNSUPPORTED_H

//******************************************************************************
/* Function :  DEVCON_MPLLIsReady_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLIsReady 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLIsReady function.
*/

PLIB_TEMPLATE bool DEVCON_MPLLIsReady_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLIsReady");

    return false;
}


//******************************************************************************
/* Function :  DEVCON_MPLLEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLEnable function.
*/

PLIB_TEMPLATE void DEVCON_MPLLEnable_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLEnable");
}


//******************************************************************************
/* Function :  DEVCON_MPLLDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLDisable function.
*/

PLIB_TEMPLATE void DEVCON_MPLLDisable_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLDisable");
}


//******************************************************************************
/* Function :  DEVCON_MPLLODiv1Set_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLODiv1Set 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLODiv1Set function.
*/

PLIB_TEMPLATE void DEVCON_MPLLODiv1Set_Unsupported( DEVCON_MODULE_ID index , DEVCON_MPLL_OUTPUT_DIVIDER bits )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLODiv1Set");
}


//******************************************************************************
/* Function :  DEVCON_MPLLODiv2Set_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLODiv2Set 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLODiv2Set function.
*/

PLIB_TEMPLATE void DEVCON_MPLLODiv2Set_Unsupported( DEVCON_MODULE_ID index , DEVCON_MPLL_OUTPUT_DIVIDER bits )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLODiv2Set");
}


//******************************************************************************
/* Function :  DEVCON_MPLLVregIsReady_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLVregIsReady 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLVregIsReady function.
*/

PLIB_TEMPLATE bool DEVCON_MPLLVregIsReady_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLVregIsReady");

    return false;
}


//******************************************************************************
/* Function :  DEVCON_MPLLVregEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLVregEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLVregEnable function.
*/

PLIB_TEMPLATE void DEVCON_MPLLVregEnable_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLVregEnable");
}


//******************************************************************************
/* Function :  DEVCON_MPLLVregDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLVregDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLVregDisable function.
*/

PLIB_TEMPLATE void DEVCON_MPLLVregDisable_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLVregDisable");
}


//******************************************************************************
/* Function :  DEVCON_MPLLMultiplierSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLMultiplierSet 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLMultiplierSet function.
*/

PLIB_TEMPLATE void DEVCON_MPLLMultiplierSet_Unsupported( DEVCON_MODULE_ID index , uint8_t value )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLMultiplierSet");
}


//******************************************************************************
/* Function :  DEVCON_MPLLVrefSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLVrefSet 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLVrefSet function.
*/

PLIB_TEMPLATE void DEVCON_MPLLVrefSet_Unsupported( DEVCON_MODULE_ID index , DEVCON_MPLL_VREF_CONTROL vref )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLVrefSet");
}


//******************************************************************************
/* Function :  DEVCON_MPLLInputDivSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_MPLLInputDivSet 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_MPLLInputDivSet function.
*/

PLIB_TEMPLATE void DEVCON_MPLLInputDivSet_Unsupported( DEVCON_MODULE_ID index , uint8_t value )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_MPLLInputDivSet");
}


//******************************************************************************
/* Function :  DEVCON_ExistsMPLL_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_ExistsMPLL

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_ExistsMPLL function.
*/

PLIB_TEMPLATE bool DEVCON_ExistsMPLL_Unsupported( DEVCON_MODULE_ID index )
{
    return false;
}


#endif /*_DEVCON_MPLL_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

