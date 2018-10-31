/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_UsbClockSource_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : UsbClockSource
    and its Variant : Default
    For following APIs :
        PLIB_OSC_ExistsUsbClockSource
        PLIB_OSC_UsbClockSourceSelect
        PLIB_OSC_UsbClockSourceGet

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

#ifndef _OSC_USBCLOCKSOURCE_DEFAULT_H
#define _OSC_USBCLOCKSOURCE_DEFAULT_H


// *****************************************************************************
// *****************************************************************************
// Section: SYS Clock Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Clock USB clock fixed divisor

  Summary:
    Sets up Clock module USB clock fixed divisor.

  Description:
    Macro holds the value of the fixed USB clock divisor.

  Remarks:
	None.
*/

#define SYS_CLK_USB_FIXED_DIVISOR					2


// *****************************************************************************
/* Clock USB clock fixed multipler

  Summary:
    Sets up Clock module USB clock fixed multipler.

  Description:
    Macro holds the value of the fixed USB clock multipler.

  Remarks:
	None.
*/

#define SYS_CLK_USB_FIXED_PLL_MULTIPLIER			24


// *****************************************************************************
/* USB Clock Bus count

  Summary:
    The number of USB clock buses in available in the device.

  Description:
    This macro gives the number of USB clock buses in available in the device.

  Remarks:
    None
*/

#define USB_BUSES_MAX								1



//******************************************************************************
/* Function :  OSC_ExistsUsbClockSource_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsUsbClockSource

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsUsbClockSource function.
*/

#define PLIB_OSC_ExistsUsbClockSource PLIB_OSC_ExistsUsbClockSource
PLIB_TEMPLATE bool OSC_ExistsUsbClockSource_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_UsbClockSourceSelect_Default

  Summary:
    Implements Default variant of PLIB_OSC_UsbClockSourceSelect

  Description:
    This template implements the Default variant of the PLIB_OSC_UsbClockSourceSelect function.
*/

PLIB_TEMPLATE void OSC_UsbClockSourceSelect_Default( OSC_MODULE_ID index , OSC_USBCLOCK_SOURCE usbClock )
{
    OSCCONbits.UFRCEN = usbClock;
}


//******************************************************************************
/* Function :  OSC_UsbClockSourceGet_Default

  Summary:
    Implements Default variant of PLIB_OSC_UsbClockSourceGet

  Description:
    This template implements the Default variant of the PLIB_OSC_UsbClockSourceGet function.
*/

PLIB_TEMPLATE OSC_USBCLOCK_SOURCE OSC_UsbClockSourceGet_Default( OSC_MODULE_ID index )
{
    return (OSC_USBCLOCK_SOURCE)OSCCONbits.UFRCEN;
}


#endif /*_OSC_USBCLOCKSOURCE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

