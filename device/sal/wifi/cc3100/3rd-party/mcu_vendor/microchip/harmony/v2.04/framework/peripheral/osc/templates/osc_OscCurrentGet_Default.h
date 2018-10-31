/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_OscCurrentGet_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : OscCurrentGet
    and its Variant : Default
    For following APIs :
        PLIB_OSC_ExistsOscCurrentGet
        PLIB_OSC_CurrentSysClockGet

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

#ifndef _OSC_OSCCURRENTGET_DEFAULT_H
#define _OSC_OSCCURRENTGET_DEFAULT_H

//******************************************************************************
/* Function :  OSC_ExistsOscCurrentGet_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsOscCurrentGet

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsOscCurrentGet function.
*/

#define PLIB_OSC_ExistsOscCurrentGet PLIB_OSC_ExistsOscCurrentGet
PLIB_TEMPLATE bool OSC_ExistsOscCurrentGet_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_CurrentSysClockGet_Default

  Summary:
    Implements Default variant of PLIB_OSC_CurrentSysClockGet 

  Description:
    This template implements the Default variant of the PLIB_OSC_CurrentSysClockGet function.
*/

PLIB_TEMPLATE OSC_SYS_TYPE OSC_CurrentSysClockGet_Default( OSC_MODULE_ID index )
{
    return (OSC_SYS_TYPE)OSCCONbits.COSC;
}


#endif /*_OSC_OSCCURRENTGET_DEFAULT_H*/

/******************************************************************************
 End of File
*/

