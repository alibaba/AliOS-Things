/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_UPLLFrequencyRange_PIC32MK.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : UPLLFrequencyRange
    and its Variant : PIC32MK
    For following APIs :
        PLIB_OSC_ExistsUPLLFrequencyRange
        PLIB_OSC_UPLLFrequencyRangeSet
        PLIB_OSC_UPLLFrequencyRangeGet

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

#ifndef _OSC_UPLLFREQUENCYRANGE_PIC32MK_H
#define _OSC_UPLLFREQUENCYRANGE_PIC32MK_H

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
/* Function :  OSC_ExistsUPLLFrequencyRange_PIC32MK

  Summary:
    Implements PIC32MK variant of PLIB_OSC_ExistsUPLLFrequencyRange

  Description:
    This template implements the PIC32MK variant of the PLIB_OSC_ExistsUPLLFrequencyRange function.
*/

#define PLIB_OSC_ExistsUPLLFrequencyRange PLIB_OSC_ExistsUPLLFrequencyRange
PLIB_TEMPLATE bool OSC_ExistsUPLLFrequencyRange_PIC32MK( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_UPLLFrequencyRangeSet_PIC32MK

  Summary:
    Implements PIC32MK variant of PLIB_OSC_UPLLFrequencyRangeSet 

  Description:
    This template implements the PIC32MK variant of the PLIB_OSC_UPLLFrequencyRangeSet function.
*/

PLIB_TEMPLATE void OSC_UPLLFrequencyRangeSet_PIC32MK( OSC_MODULE_ID index , OSC_UPLL_FREQ_RANGE PLLFrequencyRange )
{
    UPLLCONbits.PLLRANGE = PLLFrequencyRange;

    Nop();
    Nop();
}


//******************************************************************************
/* Function :  OSC_UPLLFrequencyRangeGet_PIC32MK

  Summary:
    Implements PIC32MK variant of PLIB_OSC_UPLLFrequencyRangeGet 

  Description:
    This template implements the PIC32MK variant of the PLIB_OSC_UPLLFrequencyRangeGet function.
*/

PLIB_TEMPLATE OSC_UPLL_FREQ_RANGE OSC_UPLLFrequencyRangeGet_PIC32MK( OSC_MODULE_ID index )
{
     return (OSC_UPLL_FREQ_RANGE)UPLLCONbits.PLLRANGE;
}


#endif /*_OSC_UPLLFREQUENCYRANGE_PIC32MK_H*/

/******************************************************************************
 End of File
*/

