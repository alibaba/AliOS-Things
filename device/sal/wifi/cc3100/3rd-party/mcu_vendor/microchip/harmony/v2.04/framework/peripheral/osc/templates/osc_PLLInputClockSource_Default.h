/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_PLLInputClockSource_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PLLInputClockSource
    and its Variant : Default
    For following APIs :
        PLIB_OSC_ExistsSysPLLInputClockSource
        PLIB_OSC_SysPLLInputClockSourceSet
        PLIB_OSC_SysPLLInputClockSourceGet

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

#ifndef _OSC_PLLINPUTCLOCKSOURCE_DEFAULT_H
#define _OSC_PLLINPUTCLOCKSOURCE_DEFAULT_H

//******************************************************************************
/* Function :  OSC_ExistsSysPLLInputClockSource_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsSysPLLInputClockSource

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsSysPLLInputClockSource function.
*/

#define PLIB_OSC_ExistsSysPLLInputClockSource PLIB_OSC_ExistsSysPLLInputClockSource
PLIB_TEMPLATE bool OSC_ExistsSysPLLInputClockSource_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_SysPLLInputClockSourceSet_Default

  Summary:
    Implements Default variant of PLIB_OSC_SysPLLInputClockSourceSet 

  Description:
    This template implements the Default variant of the PLIB_OSC_SysPLLInputClockSourceSet function.
*/

PLIB_TEMPLATE void OSC_SysPLLInputClockSourceSet_Default( OSC_MODULE_ID index , OSC_SYSPLL_IN_CLK_SOURCE PLLInClockSource )
{
    SPLLCONbits.PLLICLK = PLLInClockSource;

    Nop();
    Nop();
}


//******************************************************************************
/* Function :  OSC_SysPLLInputClockSourceGet_Default

  Summary:
    Implements Default variant of PLIB_OSC_SysPLLInputClockSourceGet 

  Description:
    This template implements the Default variant of the PLIB_OSC_SysPLLInputClockSourceGet function.
*/

PLIB_TEMPLATE OSC_SYSPLL_IN_CLK_SOURCE OSC_SysPLLInputClockSourceGet_Default( OSC_MODULE_ID index )
{
     return (OSC_SYSPLL_IN_CLK_SOURCE)SPLLCONbits.PLLICLK;
}


#endif /*_OSC_PLLINPUTCLOCKSOURCE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

