/*******************************************************************************
  INT Peripheral Library Template Implementation

  File Name:
    int_INTCPUPriority_Default.h

  Summary:
    INT PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : INTCPUPriority
    and its Variant : Default
    For following APIs :
        PLIB_INT_ExistsINTCPUPriority
        PLIB_INT_PriorityGet

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

#ifndef _INT_INTCPUPRIORITY_DEFAULT_H
#define _INT_INTCPUPRIORITY_DEFAULT_H

//******************************************************************************
/* Function :  INT_ExistsINTCPUPriority_Default

  Summary:
    Implements Default variant of PLIB_INT_ExistsINTCPUPriority

  Description:
    This template implements the Default variant of the PLIB_INT_ExistsINTCPUPriority function.
*/
#define PLIB_INT_ExistsINTCPUPriority PLIB_INT_ExistsINTCPUPriority
PLIB_TEMPLATE bool INT_ExistsINTCPUPriority_Default( INT_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  INT_PriorityGet_Default

  Summary:
    Implements Default variant of PLIB_INT_PriorityGet 

  Description:
    This template implements the Default variant of the PLIB_INT_PriorityGet function.
*/
PLIB_TEMPLATE INT_PRIORITY_LEVEL INT_PriorityGet_Default( INT_MODULE_ID index )
{
    return (INT_PRIORITY_LEVEL) INTSTATbits.SRIPL;
}

#endif /*_INT_INTCPUPRIORITY_DEFAULT_H*/

/******************************************************************************
 End of File
*/

