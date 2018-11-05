/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_Prescale_Unsupported.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Prescale
    and its Variant : Unsupported
    For following APIs :
        PLIB_TMR_PrescaleSelect
        PLIB_TMR_PrescaleGet
        PLIB_TMR_ExistsPrescale

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _TMR_PRESCALE_UNSUPPORTED_H
#define _TMR_PRESCALE_UNSUPPORTED_H

//******************************************************************************
/* Function :  TMR_PrescaleSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_PrescaleSelect

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_PrescaleSelect function.
*/

PLIB_TEMPLATE void TMR_PrescaleSelect_Unsupported( TMR_MODULE_ID index , TMR_PRESCALE prescale )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_PrescaleSelect");
}


//******************************************************************************
/* Function :  TMR_PrescaleGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_PrescaleGet

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_PrescaleGet function.
*/

PLIB_TEMPLATE TMR_PRESCALE TMR_PrescaleGet_Unsupported( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_PrescaleGet");

    return 0;
}


//******************************************************************************
/* Function :  TMR_PrescaleDivisorGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_PrescaleDivisorGet

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_PrescaleDivisorGet function.
*/

PLIB_TEMPLATE uint16_t TMR_PrescaleDivisorGet_Unsupported( TMR_MODULE_ID index , TMR_PRESCALE prescale )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_PrescaleDivisorGet");

    return 0;
}


//******************************************************************************
/* Function :  TMR_ExistsPrescale_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_ExistsPrescale

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_ExistsPrescale function.
*/

PLIB_TEMPLATE bool TMR_ExistsPrescale_Unsupported( TMR_MODULE_ID index )
{
    return false;
}


#endif /*_TMR_PRESCALE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

