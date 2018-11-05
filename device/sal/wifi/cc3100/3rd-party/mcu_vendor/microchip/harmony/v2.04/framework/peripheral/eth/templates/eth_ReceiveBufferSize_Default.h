/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_ReceiveBufferSize_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReceiveBufferSize
    and its Variant : Default
    For following APIs :
        PLIB_ETH_ReceiveBufferSizeGet
        PLIB_ETH_ReceiveBufferSizeSet
        PLIB_ETH_ExistsReceiveBufferSize

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

#ifndef _ETH_RECEIVEBUFFERSIZE_DEFAULT_H
#define _ETH_RECEIVEBUFFERSIZE_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_ReceiveBufferSizeGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveBufferSizeGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveBufferSizeGet function.
*/

PLIB_TEMPLATE uint8_t ETH_ReceiveBufferSizeGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return	(uint8_t)eth->ETHCON2.RXBUF_SZ;
}


//******************************************************************************
/* Function :  ETH_ReceiveBufferSizeSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveBufferSizeSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveBufferSizeSet function.
*/

PLIB_TEMPLATE void ETH_ReceiveBufferSizeSet_Default( ETH_MODULE_ID index , uint8_t ReceiveBufferSize )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	PLIB_ASSERT(!(bool)eth->ETHCON1.RXEN,"Receive must be disabled!");
	eth->ETHCON2CLR = _ETHCON2_RXBUF_SZ_MASK;
	eth->ETHCON2SET = _ETHCON2_RXBUF_SZ_MASK & ((uint32_t)ReceiveBufferSize << _ETHCON2_RXBUF_SZ_POSITION);
}

//******************************************************************************
/* Function :  ETH_RxSetBufferSize_Defaultt

  Summary:
    Implements Default variant of ETH_RxSetBufferSize

  Description:
    This template implements the Default variant of the ETH_RxSetBufferSize function.
*/
PLIB_TEMPLATE int ETH_RxSetBufferSize_Default(ETH_MODULE_ID index,int rxBuffSize)
{
    rxBuffSize >>= 4;     // truncate
    if(!rxBuffSize)
    {
        return -1;
    }

    ETH_ReceiveBufferSizeSet_Default(index, rxBuffSize);

    return rxBuffSize << 4;
}


//******************************************************************************
/* Function :  ETH_ExistsReceiveBufferSize_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsReceiveBufferSize

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsReceiveBufferSize function.
*/

#define PLIB_ETH_ExistsReceiveBufferSize PLIB_ETH_ExistsReceiveBufferSize
PLIB_TEMPLATE bool ETH_ExistsReceiveBufferSize_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_RECEIVEBUFFERSIZE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

