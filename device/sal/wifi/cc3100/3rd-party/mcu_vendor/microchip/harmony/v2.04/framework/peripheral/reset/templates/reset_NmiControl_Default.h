/*******************************************************************************
  RESET Peripheral Library Template Implementation

  File Name:
    reset_NmiControl_Default.h

  Summary:
    RESET PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : NmiControl
    and its Variant : Default
    For following APIs :
        PLIB_RESET_ExistsNmiControl
        PLIB_RESET_NmiReasonGet
        PLIB_RESET_NmiEventTrigger
        PLIB_RESET_NmiEventClear

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

#ifndef _RESET_NMICONTROL_DEFAULT_H
#define _RESET_NMICONTROL_DEFAULT_H


//******************************************************************************
/* Function :  RESET_ExistsNmiControl_Default

  Summary:
    Implements Default variant of PLIB_RESET_ExistsNmiControl

  Description:
    This template implements the Default variant of the PLIB_RESET_ExistsNmiControl function.
*/

#define PLIB_RESET_ExistsNmiControl PLIB_RESET_ExistsNmiControl
PLIB_TEMPLATE bool RESET_ExistsNmiControl_Default( RESET_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  RESET_NmiReasonGet_Default

  Summary:
    Implements Default variant of PLIB_RESET_NmiReasonGet 

  Description:
    This template implements the Default variant of the PLIB_RESET_NmiReasonGet function.
*/

PLIB_TEMPLATE RESET_NMI_REASON RESET_NmiReasonGet_Default( RESET_MODULE_ID index )
{
    return(RESET_NMI_REASON)(RNMICON & (~_RNMICON_NMICNT_MASK));
}


//******************************************************************************
/* Function :  RESET_NmiEventTrigger_Default

  Summary:
    Implements Default variant of PLIB_RESET_NmiEventTrigger 

  Description:
    This template implements the Default variant of the PLIB_RESET_NmiEventTrigger function.
*/

PLIB_TEMPLATE void RESET_NmiEventTrigger_Default( RESET_MODULE_ID index , RESET_NMI_REASON nmi_reason )
{
    RNMICONSET = nmi_reason;
}


//******************************************************************************
/* Function :  RESET_NmiEventClear_Default

  Summary:
    Implements Default variant of PLIB_RESET_NmiEventClear 

  Description:
    This template implements the Default variant of the PLIB_RESET_NmiEventClear function.
*/

PLIB_TEMPLATE void RESET_NmiEventClear_Default( RESET_MODULE_ID index , RESET_NMI_REASON nmi_reason )
{
    RNMICONCLR = nmi_reason;
}


#endif /*_RESET_NMICONTROL_DEFAULT_H*/

/******************************************************************************
 End of File
*/

