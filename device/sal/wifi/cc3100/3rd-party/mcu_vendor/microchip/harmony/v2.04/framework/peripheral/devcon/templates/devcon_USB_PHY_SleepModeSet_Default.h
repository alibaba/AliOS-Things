/*******************************************************************************
  DEVCON Peripheral Library Template Implementation

  File Name:
    devcon_USB_PHY_SleepModeSet_Default.h

  Summary:
    DEVCON PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : USB_PHY_SleepModeSet
    and its Variant : Default
    For following APIs :
        PLIB_DEVCON_USBPHYSleepModeSet
        PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet

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

#ifndef _DEVCON_USB_PHY_SLEEPMODESET_DEFAULT_H
#define _DEVCON_USB_PHY_SLEEPMODESET_DEFAULT_H

//******************************************************************************
/* Function :  DEVCON_USBPHYSleepModeSet_Default

  Summary:
    Implements Default variant of PLIB_DEVCON_USBPHYSleepModeSet

  Description:
    This template implements the Default variant of the PLIB_DEVCON_USBPHYSleepModeSet function.
*/

PLIB_TEMPLATE void DEVCON_USBPHYSleepModeSet_Default( DEVCON_MODULE_ID index , DEVCON_USB_SLEEP_MODE sleepOrRun )
{
	CFGCONbits.USBSSEN = sleepOrRun;
}


//******************************************************************************
/* Function :  DEVCON_ExistsUSB_PHY_SleepModeSet_Default

  Summary:
    Implements Default variant of PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet

  Description:
    This template implements the Default variant of the PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet function.
*/

#define PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet PLIB_DEVCON_ExistsUSB_PHY_SleepModeSet
PLIB_TEMPLATE bool DEVCON_ExistsUSB_PHY_SleepModeSet_Default( DEVCON_MODULE_ID index )
{
    return true;
}


#endif /*_DEVCON_USB_PHY_SLEEPMODESET_DEFAULT_H*/

/******************************************************************************
 End of File
*/

