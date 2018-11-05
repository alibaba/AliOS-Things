/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_Counter16Bit_Unsupported.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Counter16Bit
    and its Variant : Unsupported
    For following APIs :
        PLIB_TMR_Counter16BitSet
        PLIB_TMR_Counter16BitGet
        PLIB_TMR_Counter16BitClear
        PLIB_TMR_ExistsCounter16Bit

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

#ifndef _TMR_COUNTER16BIT_UNSUPPORTED_H
#define _TMR_COUNTER16BIT_UNSUPPORTED_H

//******************************************************************************
/* Function :  TMR_Counter16BitSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_Counter16BitSet 

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_Counter16BitSet function.
*/

PLIB_TEMPLATE void TMR_Counter16BitSet_Unsupported( TMR_MODULE_ID index , uint16_t value )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_Counter16BitSet");
}


//******************************************************************************
/* Function :  TMR_Counter16BitGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_Counter16BitGet 

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_Counter16BitGet function.
*/

PLIB_TEMPLATE uint16_t TMR_Counter16BitGet_Unsupported( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_Counter16BitGet");

    return 0;
}


//******************************************************************************
/* Function :  TMR_Counter16BitClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_Counter16BitClear 

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_Counter16BitClear function.
*/

PLIB_TEMPLATE void TMR_Counter16BitClear_Unsupported( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_Counter16BitClear");
}


//******************************************************************************
/* Function :  TMR_ExistsCounter16Bit_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_ExistsCounter16Bit

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_ExistsCounter16Bit function.
*/

PLIB_TEMPLATE bool TMR_ExistsCounter16Bit_Unsupported( TMR_MODULE_ID index )
{
    return false;
}


#endif /*_TMR_COUNTER16BIT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

