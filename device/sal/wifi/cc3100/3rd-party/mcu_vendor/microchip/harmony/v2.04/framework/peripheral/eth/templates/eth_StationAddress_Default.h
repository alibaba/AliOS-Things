/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_StationAddress_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : StationAddress
    and its Variant : Default
    For following APIs :
        PLIB_ETH_StationAddressGet
        PLIB_ETH_StationAddressSet
        PLIB_ETH_ExistsStationAddress

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

#ifndef _ETH_STATIONADDRESS_DEFAULT_H
#define _ETH_STATIONADDRESS_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_StationAddressGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_StationAddressGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_StationAddressGet function.
*/

PLIB_TEMPLATE uint8_t ETH_StationAddressGet_Default( ETH_MODULE_ID index , uint8_t which )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    volatile uint32_t * ethReg ;
	volatile uint32_t bitsMask =_EMACxSA0_STNADDR6_MASK;
	volatile uint32_t bitsPos = _EMACxSA0_STNADDR6_POSITION;
	volatile uint8_t addrRegIdx;
	
	addrRegIdx=2u-((which-1u)/2u); //Which EMACxSAx register to be accessed

	if(which%2u)
	{
		bitsMask = _EMACxSA0_STNADDR5_MASK;
		bitsPos = _EMACxSA0_STNADDR5_POSITION;
	}
	
	if(addrRegIdx == 0)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA0); 
	}
	else if(addrRegIdx == 1)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA1); 
	}
	else 
	{
		eth = (eth_registers_t * )(&eth->EMACxSA2); 
	}
	ethReg = (uint32_t *)eth;	
	
	return (uint8_t) ((*ethReg & bitsMask)>> bitsPos);
    
}


//******************************************************************************
/* Function :  ETH_StationAddressSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_StationAddressSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_StationAddressSet function.
*/

PLIB_TEMPLATE void ETH_StationAddressSet_Default( ETH_MODULE_ID index , uint8_t which , uint8_t stationAddress )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	volatile uint8_t * ethReg ;
	volatile uint32_t bitsMask = _EMACxSA0_STNADDR6_MASK;
	volatile uint32_t bitsPos = _EMACxSA0_STNADDR6_POSITION;
	volatile uint32_t *ethSetReg;
	volatile uint32_t *ethClrReg;
	volatile uint8_t addrRegIdx;
	
	if(which % 2u)
	{
		bitsMask = _EMACxSA0_STNADDR5_MASK;
		bitsPos = _EMACxSA0_STNADDR5_POSITION;
	}
	
	addrRegIdx = 2 - ((which - 1) / 2); //Which EMACxSAx register to be accessed
	if(addrRegIdx == 0)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA0); 
	}
	else if(addrRegIdx == 1)
	{
		eth = (eth_registers_t * )(&eth->EMACxSA1); 
	}
	else 
	{
		eth = (eth_registers_t * )(&eth->EMACxSA2); 
	}
	ethReg = (uint8_t *)eth;	
	
	ethClrReg = (uint32_t*)(ethReg + 4); // SET Reg Address for the corresponding EMACxSAx reg 
	ethSetReg = (uint32_t*)(ethReg + 8); // CLR Reg Address for the corresponding EMACxSAx reg
	
	*ethClrReg = bitsMask;
	*ethSetReg = bitsMask & ((uint32_t)stationAddress << bitsPos);	
      
}

//******************************************************************************
/* Function :  ETH_MACSetAddress_Default

  Summary:
    Implements Default variant of ETH_MACSetAddress

  Description:
    This template implements the Default variant of the ETH_MACSetAddress function.
*/
PLIB_TEMPLATE void ETH_MACSetAddress_Default(ETH_MODULE_ID index,uint8_t bAddress[6])
{
    ETH_StationAddressSet_Default(index, 1, bAddress[0]);
    ETH_StationAddressSet_Default(index, 2, bAddress[1]);
    ETH_StationAddressSet_Default(index, 3, bAddress[2]);
    ETH_StationAddressSet_Default(index, 4, bAddress[3]);
    ETH_StationAddressSet_Default(index, 5, bAddress[4]);
    ETH_StationAddressSet_Default(index, 6, bAddress[5]);    
}

//******************************************************************************
/* Function :  ETH_MACGetAddress_Default

  Summary:
    Implements Default variant of ETH_MACGetAddress

  Description:
    This template implements the Default variant of the ETH_MACGetAddress function.
*/
PLIB_TEMPLATE void ETH_MACGetAddress_Default(ETH_MODULE_ID index,uint8_t bAddress[6])
{
    *bAddress++ = ETH_StationAddressGet_Default(index,1);
    *bAddress++ = ETH_StationAddressGet_Default(index,2);
    *bAddress++ = ETH_StationAddressGet_Default(index,3);
    *bAddress++ = ETH_StationAddressGet_Default(index,4);
    *bAddress++ = ETH_StationAddressGet_Default(index,5);
    *bAddress   = ETH_StationAddressGet_Default(index,6);
}

//******************************************************************************
/* Function :  ETH_ExistsStationAddress_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsStationAddress

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsStationAddress function.
*/

#define PLIB_ETH_ExistsStationAddress PLIB_ETH_ExistsStationAddress
PLIB_TEMPLATE bool ETH_ExistsStationAddress_Default( ETH_MODULE_ID index )
{
    return true;
}
#endif /*_ETH_STATIONADDRESS_DEFAULT_H*/



/******************************************************************************
 End of File
*/

