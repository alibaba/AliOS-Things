/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_AnPinsMode_Non_PPS.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : AnPinsMode
    and its Variant : Non_PPS
    For following APIs :
        PLIB_PORTS_AnPinsModeSelect
        PLIB_PORTS_ExistsAnPinsMode

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

#ifndef _PORTS_ANPINSMODE_NON_PPS_H
#define _PORTS_ANPINSMODE_NON_PPS_H


//******************************************************************************
/* Function :  PORTS_AnPinsModeSelect_Non_PPS

  Summary:
    Implements Non_PPS variant of PLIB_PORTS_AnPinsModeSelect 

  Description:
    This template implements the Non_PPS variant of the PLIB_PORTS_AnPinsModeSelect function.
    The operation is non-atomic
*/

PLIB_TEMPLATE void PORTS_AnPinsModeSelect_Non_PPS( PORTS_MODULE_ID  index , PORTS_AN_PIN   anPins , PORTS_PIN_MODE mode )
{
    if ( mode == PORTS_PIN_MODE_ANALOG )
	{
		AD1PCFGCLR = anPins;	
	}
	else
	{
		AD1PCFGSET = anPins;
	}
}


//******************************************************************************
/* Function :  PORTS_ExistsAnPinsMode_Non_PPS

  Summary:
    Implements Non_PPS variant of PLIB_PORTS_ExistsAnPinsMode

  Description:
    This template implements the Non_PPS variant of the PLIB_PORTS_ExistsAnPinsMode function.
*/

#define PLIB_PORTS_ExistsAnPinsMode PLIB_PORTS_ExistsAnPinsMode
PLIB_TEMPLATE bool PORTS_ExistsAnPinsMode_Non_PPS( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_ANPINSMODE_NON_PPS_H*/

/******************************************************************************
 End of File
*/

