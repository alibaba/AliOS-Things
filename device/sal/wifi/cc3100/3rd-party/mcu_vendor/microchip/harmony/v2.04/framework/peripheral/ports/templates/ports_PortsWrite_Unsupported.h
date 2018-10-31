/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_PortsWrite_Unsupported.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PortsWrite
    and its Variant : Unsupported
    For following APIs :
        PLIB_PORTS_PinWrite
        PLIB_PORTS_PinSet
        PLIB_PORTS_PinClear
        PLIB_PORTS_PinToggle
        PLIB_PORTS_Write
        PLIB_PORTS_Set
        PLIB_PORTS_Toggle
        PLIB_PORTS_Clear
        PLIB_PORTS_ExistsPortsWrite

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

#ifndef _PORTS_PORTSWRITE_UNSUPPORTED_H
#define _PORTS_PORTSWRITE_UNSUPPORTED_H


//******************************************************************************
/* Function :  PORTS_PinWrite_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinWrite 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinWrite function.
*/

PLIB_TEMPLATE void PORTS_PinWrite_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos , bool            value )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinWrite");
}


//******************************************************************************
/* Function :  PORTS_PinSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinSet 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinSet function.
*/

PLIB_TEMPLATE void PORTS_PinSet_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinSet");
}


//******************************************************************************
/* Function :  PORTS_PinClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinClear 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinClear function.
*/

PLIB_TEMPLATE void PORTS_PinClear_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinClear");
}


//******************************************************************************
/* Function :  PORTS_PinToggle_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinToggle 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinToggle function.
*/

PLIB_TEMPLATE void PORTS_PinToggle_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_BIT_POS       bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinToggle");
}


//******************************************************************************
/* Function :  PORTS_Write_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_Write 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_Write function.
*/

PLIB_TEMPLATE void PORTS_Write_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_TYPE value )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_Write");
}


//******************************************************************************
/* Function :  PORTS_Set_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_Set 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_Set function.
*/

PLIB_TEMPLATE void PORTS_Set_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_TYPE value , PORTS_DATA_MASK mask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_Set");
}


//******************************************************************************
/* Function :  PORTS_Toggle_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_Toggle 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_Toggle function.
*/

PLIB_TEMPLATE void PORTS_Toggle_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_MASK toggleMask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_Toggle");
}


//******************************************************************************
/* Function :  PORTS_Clear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_Clear 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_Clear function.
*/

PLIB_TEMPLATE void PORTS_Clear_Unsupported( PORTS_MODULE_ID index , PORTS_CHANNEL channel , PORTS_DATA_MASK clearMask )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_Clear");
}


//******************************************************************************
/* Function :  PORTS_ExistsPortsWrite_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ExistsPortsWrite

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ExistsPortsWrite function.
*/

PLIB_TEMPLATE bool PORTS_ExistsPortsWrite_Unsupported( PORTS_MODULE_ID index )
{
    return false;
}


#endif /*_PORTS_PORTSWRITE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

