/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_ClockDiagStatus_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ClockDiagStatus
    and its Variant : Default
    For following APIs :
        PLIB_OSC_ExistsClockDiagStatus
        PLIB_OSC_ClockStop
        PLIB_OSC_ClockStart
        PLIB_OSC_ClockStopStatus

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

#ifndef _OSC_CLOCKDIAGSTATUS_DEFAULT_H
#define _OSC_CLOCKDIAGSTATUS_DEFAULT_H

//******************************************************************************
/* Function :  OSC_ExistsClockDiagStatus_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsClockDiagStatus

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsClockDiagStatus function.
*/

#define PLIB_OSC_ExistsClockDiagStatus PLIB_OSC_ExistsClockDiagStatus
PLIB_TEMPLATE bool OSC_ExistsClockDiagStatus_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_ClockStop_Default

  Summary:
    Implements Default variant of PLIB_OSC_ClockStop 

  Description:
    This template implements the Default variant of the PLIB_OSC_ClockStop function.
*/

PLIB_TEMPLATE void OSC_ClockStop_Default( OSC_MODULE_ID index , OSC_CLOCK_DIAG clk )
{
    CLKDIAGSET = ( 1u << (_CLKDIAG_POSCSTOP_POSITION + clk) );
}


//******************************************************************************
/* Function :  OSC_ClockStart_Default

  Summary:
    Implements Default variant of PLIB_OSC_ClockStart 

  Description:
    This template implements the Default variant of the PLIB_OSC_ClockStart function.
*/

PLIB_TEMPLATE void OSC_ClockStart_Default( OSC_MODULE_ID index , OSC_CLOCK_DIAG clk )
{
    CLKDIAGCLR = ( 1u << (_CLKDIAG_POSCSTOP_POSITION + clk) );
}


//******************************************************************************
/* Function :  OSC_ClockStopStatus_Default

  Summary:
    Implements Default variant of PLIB_OSC_ClockStopStatus 

  Description:
    This template implements the Default variant of the PLIB_OSC_ClockStopStatus function.
*/

PLIB_TEMPLATE bool OSC_ClockStopStatus_Default( OSC_MODULE_ID index , OSC_CLOCK_DIAG clk )
{
    return ( ( ( *((SFR_TYPE *)(&CLKDIAG)) ) & ( 1u << (_CLKDIAG_POSCSTOP_POSITION + clk )) ) ? true : false );
}


#endif /*_OSC_CLOCKDIAGSTATUS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

