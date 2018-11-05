/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_RemapInput_WK.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : RemapInput
    and its Variant : WK
    For following APIs :
        PLIB_PORTS_RemapInput
        PLIB_PORTS_ExistsRemapInput

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

#ifndef _PORTS_REMAPINPUT_WK_H
#define _PORTS_REMAPINPUT_WK_H


//******************************************************************************
/* Function :  PORTS_RemapInput_WK

  Summary:
    Implements WK variant of PLIB_PORTS_RemapInput 

  Description:
    This template implements the WK variant of the PLIB_PORTS_RemapInput function.
*/

PLIB_TEMPLATE void PORTS_RemapInput_WK( PORTS_MODULE_ID      index , PORTS_REMAP_INPUT_FUNCTION inputFunction , PORTS_REMAP_INPUT_PIN      remapInputPin )
{
                            
    *(&INT0R + inputFunction) = ( ( *((SFR_TYPE *)(&INT0R + inputFunction)) ) & ~(_INT0R_INT0R_MASK) ) | ( (_INT0R_INT0R_MASK) & ((remapInputPin)<<(_INT0R_INT0R_POSITION)) );
    
}


//******************************************************************************
/* Function :  PORTS_ExistsRemapInput_WK

  Summary:
    Implements WK variant of PLIB_PORTS_ExistsRemapInput

  Description:
    This template implements the WK variant of the PLIB_PORTS_ExistsRemapInput function.
*/

#define PLIB_PORTS_ExistsRemapInput PLIB_PORTS_ExistsRemapInput
PLIB_TEMPLATE bool PORTS_ExistsRemapInput_WK( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_REMAPINPUT_WK_H*/

/******************************************************************************
 End of File
*/

