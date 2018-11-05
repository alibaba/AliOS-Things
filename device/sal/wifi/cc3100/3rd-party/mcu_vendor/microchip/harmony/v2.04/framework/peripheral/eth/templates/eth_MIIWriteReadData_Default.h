/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MIIWriteReadData_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MIIWriteReadData
    and its Variant : Default
    For following APIs :
        PLIB_ETH_MIIMWriteDataSet
        PLIB_ETH_MIIMReadDataGet
        PLIB_ETH_ExistsMIIWriteReadData

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

#ifndef _ETH_MIIWRITEREADDATA_DEFAULT_H
#define _ETH_MIIWRITEREADDATA_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_MIIMWriteDataSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMWriteDataSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMWriteDataSet function.
*/

PLIB_TEMPLATE void ETH_MIIMWriteDataSet_Default( ETH_MODULE_ID index , uint16_t writeData )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		

	eth->EMACxMWTD.MWTD =writeData ;  				 
                     
#if defined(__PIC32MZ__)
    __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"); // there's 2 clock cycles till busy is set for a write op
#else                     
    __asm__ __volatile__ ("nop; nop;"); // there's 2 clock cycles till busy is set for a write op
#endif
}


//******************************************************************************
/* Function :  ETH_MIIMReadDataGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMReadDataGet

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMReadDataGet function.
*/

PLIB_TEMPLATE uint16_t ETH_MIIMReadDataGet_Default( ETH_MODULE_ID index )
{    
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint16_t)(eth->EMACxMRDD.MRDD);								 
}


//******************************************************************************
/* Function :  ETH_ExistsMIIWriteReadData_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIWriteReadData

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIWriteReadData function.
*/

#define PLIB_ETH_ExistsMIIWriteReadData PLIB_ETH_ExistsMIIWriteReadData
PLIB_TEMPLATE bool ETH_ExistsMIIWriteReadData_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_MIIWRITEREADDATA_DEFAULT_H*/

/******************************************************************************
 End of File
*/

