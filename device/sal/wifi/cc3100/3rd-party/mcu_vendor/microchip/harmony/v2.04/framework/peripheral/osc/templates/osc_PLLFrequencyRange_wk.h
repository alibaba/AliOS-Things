/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_PLLFrequencyRange_wk.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PLLFrequencyRange
    and its Variant : wk
    For following APIs :
        PLIB_OSC_ExistsSysPLLFrequencyRange
        PLIB_OSC_SysPLLFrequencyRangeSet
        PLIB_OSC_SysPLLFrequencyRangeGet

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

#ifndef _OSC_PLLFREQUENCYRANGE_WK_H
#define _OSC_PLLFREQUENCYRANGE_WK_H

// *****************************************************************************
/* System PLL maximum input frequecy when range is set to maximum.

  Summary:
    Defines System PLL maximum input frequecy when range is set to maximum.

  Description:
    This macro defines maximum input frequecy when range is set to maximum.

  Remarks:
    None
*/

#define OSC_SYSPLL_RANGE_MAX						64000000

//******************************************************************************
/* Function :  OSC_ExistsSysPLLFrequencyRange_wk

  Summary:
    Implements wk variant of PLIB_OSC_ExistsSysPLLFrequencyRange

  Description:
    This template implements the wk variant of the PLIB_OSC_ExistsSysPLLFrequencyRange function.
*/

#define PLIB_OSC_ExistsSysPLLFrequencyRange PLIB_OSC_ExistsSysPLLFrequencyRange
PLIB_TEMPLATE bool OSC_ExistsSysPLLFrequencyRange_wk( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_SysPLLFrequencyRangeSet_wk

  Summary:
    Implements wk variant of PLIB_OSC_SysPLLFrequencyRangeSet 

  Description:
    This template implements the wk variant of the PLIB_OSC_SysPLLFrequencyRangeSet function.
*/

PLIB_TEMPLATE void OSC_SysPLLFrequencyRangeSet_wk( OSC_MODULE_ID index , OSC_SYSPLL_FREQ_RANGE PLLFrequencyRange )
{
    SPLLCONbits.SPLLRNG = PLLFrequencyRange;

    Nop();
    Nop();
}


//******************************************************************************
/* Function :  OSC_SysPLLFrequencyRangeGet_wk

  Summary:
    Implements wk variant of PLIB_OSC_SysPLLFrequencyRangeGet 

  Description:
    This template implements the wk variant of the PLIB_OSC_SysPLLFrequencyRangeGet function.
*/

PLIB_TEMPLATE OSC_SYSPLL_FREQ_RANGE OSC_SysPLLFrequencyRangeGet_wk( OSC_MODULE_ID index )
{
     return (OSC_SYSPLL_FREQ_RANGE)SPLLCONbits.SPLLRNG;
}


#endif /*_OSC_PLLFREQUENCYRANGE_WK_H*/

/******************************************************************************
 End of File
*/

