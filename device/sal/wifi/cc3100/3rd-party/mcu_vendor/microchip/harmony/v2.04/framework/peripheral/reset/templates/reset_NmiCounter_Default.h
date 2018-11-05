/*******************************************************************************
  RESET Peripheral Library Template Implementation

  File Name:
    reset_NmiCounter_Default.h

  Summary:
    RESET PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : NmiCounter
    and its Variant : Default
    For following APIs :
        PLIB_RESET_ExistsNmiCounter
        PLIB_RESET_NmiCounterValueSet
        PLIB_RESET_NmiCounterValueGet

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

#ifndef _RESET_NMICOUNTER_DEFAULT_H
#define _RESET_NMICOUNTER_DEFAULT_H

//******************************************************************************
/* Function :  RESET_ExistsNmiCounter_Default

  Summary:
    Implements Default variant of PLIB_RESET_ExistsNmiCounter

  Description:
    This template implements the Default variant of the PLIB_RESET_ExistsNmiCounter function.
*/

#define PLIB_RESET_ExistsNmiCounter PLIB_RESET_ExistsNmiCounter
PLIB_TEMPLATE bool RESET_ExistsNmiCounter_Default( RESET_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  RESET_NmiCounterValueSet_Default

  Summary:
    Implements Default variant of PLIB_RESET_NmiCounterValueSet 

  Description:
    This template implements the Default variant of the PLIB_RESET_NmiCounterValueSet function.
*/

PLIB_TEMPLATE void RESET_NmiCounterValueSet_Default( RESET_MODULE_ID index , RESET_NMI_COUNT_TYPE nmi_count )
{
     RNMICONbits.NMICNT = nmi_count;
}


//******************************************************************************
/* Function :  RESET_NmiCounterValueGet_Default

  Summary:
    Implements Default variant of PLIB_RESET_NmiCounterValueGet 

  Description:
    This template implements the Default variant of the PLIB_RESET_NmiCounterValueGet function.
*/

PLIB_TEMPLATE RESET_NMI_COUNT_TYPE RESET_NmiCounterValueGet_Default( RESET_MODULE_ID index )
{
   return (RESET_NMI_COUNT_TYPE) RNMICONbits.NMICNT ;
}

#endif /*_RESET_NMICOUNTER_DEFAULT_H*/

/******************************************************************************
 End of File
*/

