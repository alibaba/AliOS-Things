/*******************************************************************************
  DEVCON Peripheral Library Template Implementation

  File Name:
    devcon_TraceOutput_Default_1.h

  Summary:
    DEVCON PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : TraceOutput
    and its Variant : Default_1
    For following APIs :
        PLIB_DEVCON_TraceOutputEnable
        PLIB_DEVCON_TraceOutputDisable
        PLIB_DEVCON_ExistsTraceOutput

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

#ifndef _DEVCON_TRACEOUTPUT_DEFAULT_1_H
#define _DEVCON_TRACEOUTPUT_DEFAULT_1_H

//******************************************************************************
/* Function :  DEVCON_TraceOutputEnable_Default_1

  Summary:
    Implements Default variant of PLIB_DEVCON_TraceOutputEnable

  Description:
    This template implements the Default variant of the PLIB_DEVCON_TraceOutputEnable function.
*/

PLIB_TEMPLATE void DEVCON_TraceOutputEnable_Default_1( DEVCON_MODULE_ID index )
{
	DDPCONbits.TROEN = 1;
}


//******************************************************************************
/* Function :  DEVCON_TraceOutputDisable_Default_1

  Summary:
    Implements Default variant of PLIB_DEVCON_TraceOutputDisable

  Description:
    This template implements the Default variant of the PLIB_DEVCON_TraceOutputDisable function.
*/

PLIB_TEMPLATE void DEVCON_TraceOutputDisable_Default_1( DEVCON_MODULE_ID index )
{
	DDPCONbits.TROEN = 0;
}


//******************************************************************************
/* Function :  DEVCON_ExistsTraceOutput_Default_1

  Summary:
    Implements Default variant of PLIB_DEVCON_ExistsTraceOutput

  Description:
    This template implements the Default variant of the PLIB_DEVCON_ExistsTraceOutput function.
*/

#define PLIB_DEVCON_ExistsTraceOutput PLIB_DEVCON_ExistsTraceOutput
PLIB_TEMPLATE bool DEVCON_ExistsTraceOutput_Default_1( DEVCON_MODULE_ID index )
{
    return true;
}


#endif /*_DEVCON_TRACEOUTPUT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

