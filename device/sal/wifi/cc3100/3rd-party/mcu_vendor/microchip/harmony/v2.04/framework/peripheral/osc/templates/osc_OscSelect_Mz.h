/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_OscSelect_Mz.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : OscSelect
    and its Variant : Mz
    For following APIs :
        PLIB_OSC_ExistsOscSelect
        PLIB_OSC_SysClockSelect

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

#ifndef _OSC_OSCSELECT_MZ_H
#define _OSC_OSCSELECT_MZ_H

// *****************************************************************************
/* Low power RC Oscillator clock frequency

  Summary:
    Sets up the low power RC oscillator clock frequency

  Description:
    This macro sets up the low power RC oscillator clock frequency.

  Remarks:
    None
*/

#define SYS_CLK_LPRC_CLOCK						32768


// *****************************************************************************
/* Fast RC clock frequency

  Summary:
    Sets up the Fast RC oscillator clock frequency

  Description:
    This macro sets up the Fast RC oscillator clock frequency.

  Remarks:
    None
*/

#define SYS_CLK_FRC_CLOCK						8000000L


// *****************************************************************************
/* USB PLL maximum input frequency.

  Summary:
    Defines the maximum limit for the USB PLL.

  Description:
    This macro defines the maximum limit for the USB PLL. This internally
    defines the maximum allowed multiplier based on the intput to the
    multiplier.

  Remarks:
    None
*/

#define OSC_USBPLL_UFIN_MAX						4000000


// *****************************************************************************
/* System PLL minimum input frequency.

  Summary:
    Defines the minimum limit for the System PLL.

  Description:
    This macro defines the minimum limit for the System PLL. This internally
    defines the multiplier based on the intput to the multiplier.

  Remarks:
    None
*/

#define OSC_SYSPLL_UFIN_MIN						4000000


// *****************************************************************************
/* System PLL maximum input frequency.

  Summary:
    Defines the maximum limit for the System PLL.

  Description:
    This macro defines the maximum limit for the System PLL. This internally
    defines the maximum allowed multiplier based on the intput to the
    multiplier.

  Remarks:
    None
*/

#define OSC_SYSPLL_UFIN_MAX						5000000


//******************************************************************************
/* Function :  OSC_ExistsOscSelect_Mz

  Summary:
    Implements Mz variant of PLIB_OSC_ExistsOscSelect

  Description:
    This template implements the Mz variant of the PLIB_OSC_ExistsOscSelect function.
*/

#define PLIB_OSC_ExistsOscSelect PLIB_OSC_ExistsOscSelect
PLIB_TEMPLATE bool OSC_ExistsOscSelect_Mz( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_SysClockSelect_Mz

  Summary:
    Implements Mz variant of PLIB_OSC_SysClockSelect 

  Description:
    This template implements the Mz variant of the PLIB_OSC_SysClockSelect function.
*/

PLIB_TEMPLATE void OSC_SysClockSelect_Mz( OSC_MODULE_ID index , OSC_SYS_TYPE newOsc )
{

    /* If the selection is either of the two, then the system PLL
       also need to be controlled */
    if ( newOsc == OSC_FRC_WITH_PLL )
    {
        SPLLCONSET = _SPLLCON_PLLICLK_MASK;
    }
    else if ( newOsc == OSC_PRIMARY_WITH_PLL )
    {
        SPLLCONCLR = _SPLLCON_PLLICLK_MASK;

        /* Load the value to select the System PLL */
        newOsc = OSC_FRC_WITH_PLL;
    }

    OSCCONbits.NOSC = newOsc;

    /* Trigger the clock change */
    OSCCONSET = _OSCCON_OSWEN_MASK;

    Nop();
    Nop();
}


#endif /*_OSC_OSCSELECT_MZ_H*/

/******************************************************************************
 End of File
*/

