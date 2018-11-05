/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_ForceLock_PIC32WK.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ForceLock
    and its Variant : PIC32WK
    For following APIs :
        PLIB_OSC_ExistsForceLock
        PLIB_OSC_ForceSPLLLockEnable
        PLIB_OSC_ForceSPLLLockDisable
        PLIB_OSC_ForceSPLLLockStatus

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

#ifndef _OSC_FORCELOCK_PIC32WK_H
#define _OSC_FORCELOCK_PIC32WK_H

//******************************************************************************
/* Function :  OSC_ExistsForceLock_PIC32WK

  Summary:
    Implements PIC32WK variant of PLIB_OSC_ExistsForceLock

  Description:
    This template implements the PIC32WK variant of the PLIB_OSC_ExistsForceLock function.
*/

#define PLIB_OSC_ExistsForceLock PLIB_OSC_ExistsForceLock
PLIB_TEMPLATE bool OSC_ExistsForceLock_PIC32WK( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_ForceSPLLLockEnable_PIC32WK

  Summary:
    Implements PIC32WK variant of PLIB_OSC_ForceSPLLLockEnable 

  Description:
    This template implements the PIC32WK variant of the PLIB_OSC_ForceSPLLLockEnable function.
*/

PLIB_TEMPLATE void OSC_ForceSPLLLockEnable_PIC32WK( OSC_MODULE_ID index , OSC_PLL_SELECT pllSel )
{
    if(OSC_PLL_SYSTEM == pllSel)
    {
        SPLLCONSET = _SPLLCON_SPLLFLOCK_MASK;
    }
    if(OSC_PLL_USB == pllSel)
    {
        UPLLCONSET = _UPLLCON_UPLLFLOCK_MASK;
    }
    if(OSC_PLL_BT == pllSel)
    {
        BTPLLCONSET = _BTPLLCON_BTPLLFLOCK_MASK;
    }
}


//******************************************************************************
/* Function :  OSC_ForceSPLLLockDisable_PIC32WK

  Summary:
    Implements PIC32WK variant of PLIB_OSC_ForceSPLLLockDisable 

  Description:
    This template implements the PIC32WK variant of the PLIB_OSC_ForceSPLLLockDisable function.
*/

PLIB_TEMPLATE void OSC_ForceSPLLLockDisable_PIC32WK( OSC_MODULE_ID index , OSC_PLL_SELECT pllSel )
{
    if(OSC_PLL_SYSTEM == pllSel)
    {
        SPLLCONCLR = _SPLLCON_SPLLFLOCK_MASK;
    }
    if(OSC_PLL_USB == pllSel)
    {
        UPLLCONCLR = _UPLLCON_UPLLFLOCK_MASK;
    }
    if(OSC_PLL_BT == pllSel)
    {
        BTPLLCONCLR = _BTPLLCON_BTPLLFLOCK_MASK;
    }
}


//******************************************************************************
/* Function :  OSC_ForceSPLLLockStatus_PIC32WK

  Summary:
    Implements PIC32WK variant of PLIB_OSC_ForceSPLLLockStatus 

  Description:
    This template implements the PIC32WK variant of the PLIB_OSC_ForceSPLLLockStatus function.
*/

PLIB_TEMPLATE bool OSC_ForceSPLLLockStatus_PIC32WK( OSC_MODULE_ID index , OSC_PLL_SELECT pllSel )
{
    bool returnValue = false;

    if(OSC_PLL_SYSTEM == pllSel)
    {
        returnValue  = SPLLCONbits.SPLLFLOCK;
    }
    if(OSC_PLL_USB == pllSel)
    {
        returnValue  = UPLLCONbits.UPLLFLOCK;
    }
    if(OSC_PLL_BT == pllSel)
    {
        returnValue  = BTPLLCONbits.BTPLLFLOCK;
    }
    
    return returnValue;
}


#endif /*_OSC_FORCELOCK_PIC32WK_H*/

/******************************************************************************
 End of File
*/

