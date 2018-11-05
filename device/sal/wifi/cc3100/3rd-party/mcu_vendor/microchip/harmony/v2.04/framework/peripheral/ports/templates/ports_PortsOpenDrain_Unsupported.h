/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_PortsOpenDrain_Unsupported.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PortsOpenDrain
    and its Variant : Unsupported
    For following APIs :
        PLIB_PORTS_PinOpenDrainEnable
        PLIB_PORTS_PinOpenDrainDisable
        PLIB_PORTS_OpenDrainEnable
        PLIB_PORTS_OpenDrainDisable
        PLIB_PORTS_ExistsPortsOpenDrain

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

#ifndef _PORTS_PORTSOPENDRAIN_UNSUPPORTED_H
#define _PORTS_PORTSOPENDRAIN_UNSUPPORTED_H


//******************************************************************************
/* Function :  PORTS_PinOpenDrainEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinOpenDrainEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinOpenDrainEnable function.
*/

PLIB_TEMPLATE void PORTS_PinOpenDrainEnable_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinOpenDrainEnable");
}


//******************************************************************************
/* Function :  PORTS_PinOpenDrainDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinOpenDrainDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinOpenDrainDisable function.
*/

PLIB_TEMPLATE void PORTS_PinOpenDrainDisable_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinOpenDrainDisable");
}


//******************************************************************************
/* Function :  PORTS_OpenDrainEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_OpenDrainEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_OpenDrainEnable function.
*/

PLIB_TEMPLATE void PORTS_OpenDrainEnable_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_MASK mask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_OpenDrainEnable");
}


//******************************************************************************
/* Function :  PORTS_OpenDrainDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_OpenDrainDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_OpenDrainDisable function.
*/

PLIB_TEMPLATE void PORTS_OpenDrainDisable_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_MASK mask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_OpenDrainDisable");
}


//******************************************************************************
/* Function :  PORTS_ExistsPortsOpenDrain_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ExistsPortsOpenDrain

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ExistsPortsOpenDrain function.
*/

PLIB_TEMPLATE bool PORTS_ExistsPortsOpenDrain_Unsupported( PORTS_MODULE_ID index )
{
    return false;
}


#endif /*_PORTS_PORTSOPENDRAIN_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

