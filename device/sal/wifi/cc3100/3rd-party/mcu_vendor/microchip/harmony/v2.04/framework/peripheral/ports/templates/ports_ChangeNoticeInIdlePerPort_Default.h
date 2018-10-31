/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_ChangeNoticeInIdlePerPort_Default.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChangeNoticeInIdlePerPort
    and its Variant : Default
    For following APIs :
        PLIB_PORTS_ChangeNoticeInIdlePerPortEnable
        PLIB_PORTS_ChangeNoticeInIdlePerPortDisable
        PLIB_PORTS_ExistsChangeNoticePerPortInIdle

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

#ifndef _PORTS_CHANGENOTICEINIDLEPERPORT_DEFAULT_H
#define _PORTS_CHANGENOTICEINIDLEPERPORT_DEFAULT_H



//******************************************************************************
/* Function :  PORTS_ChangeNoticeInIdlePerPortEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChangeNoticeInIdlePerPortEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChangeNoticeInIdlePerPortEnable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticeInIdlePerPortEnable_Default( PORTS_MODULE_ID  index , PORTS_CHANNEL channel )
{
    *(&CNCONBCLR + (channel - 1) * 0x40) = _CNCONB_SIDL_MASK;
}


//******************************************************************************
/* Function :  PORTS_ChangeNoticeInIdlePerPortDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChangeNoticeInIdlePerPortDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChangeNoticeInIdlePerPortDisable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticeInIdlePerPortDisable_Default( PORTS_MODULE_ID  index , PORTS_CHANNEL channel )
{
    *(&CNCONBSET + (channel - 1) * 0x40) = _CNCONB_SIDL_MASK;
}


//******************************************************************************
/* Function :  PORTS_ExistsChangeNoticePerPortInIdle_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ExistsChangeNoticePerPortInIdle

  Description:
    This template implements the Default variant of the PLIB_PORTS_ExistsChangeNoticePerPortInIdle function.
*/

#define PLIB_PORTS_ExistsChangeNoticePerPortInIdle PLIB_PORTS_ExistsChangeNoticePerPortInIdle
PLIB_TEMPLATE bool PORTS_ExistsChangeNoticePerPortInIdle_Default( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_CHANGENOTICEINIDLEPERPORT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

