/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_PortsRead_MCU32_PPS.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PortsRead
    and its Variant : MCU32_PPS
    For following APIs :
        PLIB_PORTS_PinGet
        PLIB_PORTS_Read
        PLIB_PORTS_ExistsPortsRead

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

#ifndef _PORTS_PORTSREAD_MCU32_PPS_H
#define _PORTS_PORTSREAD_MCU32_PPS_H


//******************************************************************************
/* Function :  PORTS_PinGet_MCU32_PPS

  Summary:
    Implements MCU32_PPS variant of PLIB_PORTS_PinGet 

  Description:
    This template implements the MCU32_PPS variant of the PLIB_PORTS_PinGet function.
*/

PLIB_TEMPLATE bool PORTS_PinGet_MCU32_PPS( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    return (bool)((*(&PORTB + ((channel - 1) * 0x40)) >> bitPos) & 1);
}


//******************************************************************************
/* Function :  PORTS_Read_MCU32_PPS

  Summary:
    Implements MCU32_PPS variant of PLIB_PORTS_Read 

  Description:
    This template implements the MCU32_PPS variant of the PLIB_PORTS_Read function.
*/

PLIB_TEMPLATE PORTS_DATA_TYPE PORTS_Read_MCU32_PPS( PORTS_MODULE_ID index , PORTS_CHANNEL channel )
{
    return (*(&PORTB + ((channel - 1) * 0x40)));
}


//******************************************************************************
/* Function :  PORTS_ExistsPortsRead_MCU32_PPS

  Summary:
    Implements MCU32_PPS variant of PLIB_PORTS_ExistsPortsRead

  Description:
    This template implements the MCU32_PPS variant of the PLIB_PORTS_ExistsPortsRead function.
*/

#define PLIB_PORTS_ExistsPortsRead PLIB_PORTS_ExistsPortsRead
PLIB_TEMPLATE bool PORTS_ExistsPortsRead_MCU32_PPS( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_PORTSREAD_MCU32_PPS_H*/

/******************************************************************************
 End of File
*/

