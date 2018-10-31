/*******************************************************************************
  INT Peripheral Library Template Implementation

  File Name:
    int_SourceFlag_Default.h

  Summary:
    INT PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SourceFlag
    and its Variant : Default
    For following APIs :
        PLIB_INT_ExistsSourceFlag
        PLIB_INT_SourceFlagGet
        PLIB_INT_SourceFlagSet
        PLIB_INT_SourceFlagClear

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

#ifndef _INT_SOURCEFLAG_DEFAULT_H
#define _INT_SOURCEFLAG_DEFAULT_H

//******************************************************************************
/* Function :  INT_ExistsSourceFlag_Default

  Summary:
    Implements Default variant of PLIB_INT_ExistsSourceFlag

  Description:
    This template implements the Default variant of the PLIB_INT_ExistsSourceFlag function.
*/
#define PLIB_INT_ExistsSourceFlag PLIB_INT_ExistsSourceFlag
PLIB_TEMPLATE bool INT_ExistsSourceFlag_Default( INT_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  INT_SourceFlagGet_Default

  Summary:
    Implements Default variant of PLIB_INT_SourceFlagGet 

  Description:
    This template implements the Default variant of the PLIB_INT_SourceFlagGet function.

  Note:
    The source enum encoding is (x * 32) + y, where x is the register number 
    (IFSx) and y is the bit position. 0b0xxy_yyyy
*/
PLIB_TEMPLATE bool INT_SourceFlagGet_Default( INT_MODULE_ID index , INT_SOURCE source )
{
    volatile uint32_t *IFSx = (volatile uint32_t *)(&IFS0 + ((0x10 * (source / 32)) / 4));

    return (bool)((*IFSx >> (source & 0x1f)) & 0x1);
}

//******************************************************************************
/* Function :  INT_SourceFlagSet_Default

  Summary:
    Implements Default variant of PLIB_INT_SourceFlagSet 

  Description:
    This template implements the Default variant of the PLIB_INT_SourceFlagSet function.

  Note:
    The source enum encoding is (x * 32) + y, where x is the register number 
    (IFSx) and y is the bit position. 0b0xxy_yyyy
*/
PLIB_TEMPLATE void INT_SourceFlagSet_Default( INT_MODULE_ID index , INT_SOURCE source )
{
    volatile uint32_t *IFSx = (volatile uint32_t *) (&IFS0 + ((0x10 * (source / 32)) / 4));
    volatile uint32_t *IFSxSET = (volatile uint32_t *)(IFSx + 2);

    *IFSxSET = 1 << (source & 0x1f);
}

//******************************************************************************
/* Function :  INT_SourceFlagClear_Default

  Summary:
    Implements Default variant of PLIB_INT_SourceFlagClear 

  Description:
    This template implements the Default variant of the PLIB_INT_SourceFlagClear function.

  Note:
    The source enum encoding is (x * 32) + y, where x is the register number 
    (IFSx) and y is the bit position. 0b0xxy_yyyy
*/
PLIB_TEMPLATE void INT_SourceFlagClear_Default( INT_MODULE_ID index , INT_SOURCE source )
{
    volatile uint32_t *IFSx = (volatile uint32_t *) (&IFS0 + ((0x10 * (source / 32)) / 4));
    volatile uint32_t *IFSxCLR = (volatile uint32_t *)(IFSx + 1);

    *IFSxCLR = 1 << (source & 0x1f);
}

#endif /*_INT_SOURCEFLAG_DEFAULT_H*/

/******************************************************************************
 End of File
*/

