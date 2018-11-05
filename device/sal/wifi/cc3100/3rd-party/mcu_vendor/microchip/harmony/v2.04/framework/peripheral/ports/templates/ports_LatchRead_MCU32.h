/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_LatchRead_MCU32.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : LatchRead
    and its Variant : MCU32
    For following APIs :
        PLIB_PORTS_PinGetLatched
        PLIB_PORTS_ReadLatched
        PLIB_PORTS_ExistsLatchRead

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

#ifndef _PORTS_LATCHREAD_MCU32_H
#define _PORTS_LATCHREAD_MCU32_H



//******************************************************************************
/* Function :  PORTS_PinGetLatched_MCU32

  Summary:
    Implements MCU32 variant of PLIB_PORTS_PinGetLatched 

  Description:
    This template implements the MCU32 variant of the PLIB_PORTS_PinGetLatched function.
*/

PLIB_TEMPLATE bool PORTS_PinGetLatched_MCU32( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    return (bool)((*(&LATB + ((channel - 1) * 0x10)) >> bitPos) & 1);
}


//******************************************************************************
/* Function :  PORTS_ReadLatched_MCU32

  Summary:
    Implements MCU32 variant of PLIB_PORTS_ReadLatched 

  Description:
    This template implements the MCU32 variant of the PLIB_PORTS_ReadLatched function.
*/

PLIB_TEMPLATE PORTS_DATA_TYPE PORTS_ReadLatched_MCU32( PORTS_MODULE_ID index , PORTS_CHANNEL channel )
{
    return (*(&LATB + ((channel - 1) * 0x10)));
}


//******************************************************************************
/* Function :  PORTS_ExistsLatchRead_MCU32

  Summary:
    Implements MCU32 variant of PLIB_PORTS_ExistsLatchRead

  Description:
    This template implements the MCU32 variant of the PLIB_PORTS_ExistsLatchRead function.
*/

#define PLIB_PORTS_ExistsLatchRead PLIB_PORTS_ExistsLatchRead
PLIB_TEMPLATE bool PORTS_ExistsLatchRead_MCU32( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_LATCHREAD_MCU32_H*/

/******************************************************************************
 End of File
*/

