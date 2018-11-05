/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_ChangeNotice_Default.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChangeNotice
    and its Variant : Default
    For following APIs :
        PLIB_PORTS_ChangeNoticeEnable
        PLIB_PORTS_ChangeNoticeDisable
        PLIB_PORTS_ExistsChangeNotice

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

#ifndef _PORTS_CHANGENOTICE_DEFAULT_H
#define _PORTS_CHANGENOTICE_DEFAULT_H


//******************************************************************************
/* Function :  PORTS_ChangeNoticeEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChangeNoticeEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChangeNoticeEnable function.
    This operation is atomic
*/

PLIB_TEMPLATE void PORTS_ChangeNoticeEnable_Default( PORTS_MODULE_ID index )
{
    CNCONSET = _CNCON_ON_MASK;
}


//******************************************************************************
/* Function :  PORTS_ChangeNoticeDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChangeNoticeDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChangeNoticeDisable function.
    This operation is atomic
*/

PLIB_TEMPLATE void PORTS_ChangeNoticeDisable_Default( PORTS_MODULE_ID index )
{
    CNCONCLR = _CNCON_ON_MASK;
}


//******************************************************************************
/* Function :  PORTS_ExistsChangeNotice_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ExistsChangeNotice

  Description:
    This template implements the Default variant of the PLIB_PORTS_ExistsChangeNotice function.
*/

#define PLIB_PORTS_ExistsChangeNotice PLIB_PORTS_ExistsChangeNotice
PLIB_TEMPLATE bool PORTS_ExistsChangeNotice_Default( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_CHANGENOTICE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

