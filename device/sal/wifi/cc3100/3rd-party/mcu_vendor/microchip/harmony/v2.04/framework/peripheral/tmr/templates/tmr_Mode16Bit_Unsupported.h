/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_Mode16Bit_Unsupported.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Mode16Bit
    and its Variant : Unsupported
    For following APIs :
        PLIB_TMR_Mode16BitEnable
        PLIB_TMR_ExistsMode16Bit

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

#ifndef _TMR_MODE16BIT_UNSUPPORTED_H
#define _TMR_MODE16BIT_UNSUPPORTED_H

//******************************************************************************
/* Function :  TMR_Mode16BitEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_Mode16BitEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_Mode16BitEnable function.
*/

PLIB_TEMPLATE void TMR_Mode16BitEnable_Unsupported( TMR_MODULE_ID index )
{
    /* This API will be called only for odd numbered timers which do not have
    T32 bit. for those timers, 16 bit is always enabled, so there is nothing
    to be done in this implementation */
    return;
}


//******************************************************************************
/* Function :  TMR_ExistsMode16Bit_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_ExistsMode16Bit

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_ExistsMode16Bit function.
*/

PLIB_TEMPLATE bool TMR_ExistsMode16Bit_Unsupported( TMR_MODULE_ID index )
{
    /* This API will be called only for odd numbered timers which do not have
    T32 bit. for those timers, 16 bit is always enabled, so it always returns
    true. 
    This workaround is needed as there was no need of this exist API at all,
    but now once we have it we can't remove it.  */
    return true;
}


#endif /*_TMR_MODE16BIT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

