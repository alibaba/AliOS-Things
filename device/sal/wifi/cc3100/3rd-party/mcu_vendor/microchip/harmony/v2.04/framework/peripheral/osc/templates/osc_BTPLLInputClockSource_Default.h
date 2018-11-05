/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_BTPLLInputClockSource_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : BTPLLInputClockSource
    and its Variant : Default
    For following APIs :
        PLIB_OSC_ExistsBTPLLInputClockSource
        PLIB_OSC_BTPLLInputClockSourceSet
        PLIB_OSC_BTPLLInputClockSourceGet

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

#ifndef _OSC_BTPLLINPUTCLOCKSOURCE_DEFAULT_H
#define _OSC_BTPLLINPUTCLOCKSOURCE_DEFAULT_H

//******************************************************************************
/* Function :  OSC_ExistsBTPLLInputClockSource_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsBTPLLInputClockSource

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsBTPLLInputClockSource function.
*/

#define PLIB_OSC_ExistsBTPLLInputClockSource PLIB_OSC_ExistsBTPLLInputClockSource
PLIB_TEMPLATE bool OSC_ExistsBTPLLInputClockSource_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_BTPLLInputClockSourceSet_Default

  Summary:
    Implements Default variant of PLIB_OSC_BTPLLInputClockSourceSet 

  Description:
    This template implements the Default variant of the PLIB_OSC_BTPLLInputClockSourceSet function.
*/

PLIB_TEMPLATE void OSC_BTPLLInputClockSourceSet_Default( OSC_MODULE_ID index , OSC_BTPLL_IN_CLK_SOURCE PLLInClockSource )
{
    BTPLLCONbits.BTPLLICLK = PLLInClockSource;

    Nop();
    Nop();
}


//******************************************************************************
/* Function :  OSC_BTPLLInputClockSourceGet_Default

  Summary:
    Implements Default variant of PLIB_OSC_BTPLLInputClockSourceGet 

  Description:
    This template implements the Default variant of the PLIB_OSC_BTPLLInputClockSourceGet function.
*/

PLIB_TEMPLATE OSC_SYSPLL_IN_CLK_SOURCE OSC_BTPLLInputClockSourceGet_Default( OSC_MODULE_ID index )
{
    return (OSC_SYSPLL_IN_CLK_SOURCE)BTPLLCONbits.BTPLLICLK;
}


#endif /*_OSC_BTPLLINPUTCLOCKSOURCE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

