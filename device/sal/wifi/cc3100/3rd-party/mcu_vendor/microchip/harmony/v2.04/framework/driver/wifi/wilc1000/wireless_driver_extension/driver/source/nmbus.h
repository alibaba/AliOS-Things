/*******************************************************************************
  This module contains WILC1000 bus APIs implementation.
  
  File Name:
    nmbus.h

  Summary:
    This module contains WILC1000 bus APIs implementation.

  Description:
    This module contains WILC1000 bus APIs implementation.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _NMBUS_H_
#define _NMBUS_H_

#include "common/include/nm_common.h"

#ifdef __cplusplus
extern "C"{
#endif
/**
*	@fn		nm_bus_iface_init
*	@brief	Initialize bus interface
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_iface_init(void *);


/**
*	@fn		nm_bus_iface_deinit
*	@brief	Deinitialize bus interface
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_iface_deinit(void);

/**
*	@fn		nm_bus_reset
*	@brief	reset bus interface
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@version	1.0
*/
sint8 nm_bus_reset(void);

/**
*	@fn		nm_bus_iface_reconfigure
*	@brief	reconfigure bus interface
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_iface_reconfigure(void *ptr);

/**
*	@fn		nm_read_reg
*	@brief	Read register
*	@param [in]	u32Addr
*				Register address
*	@return	Register value
*/
uint32 nm_read_reg(uint32 u32Addr);

/**
*	@fn		nm_read_reg_with_ret
*	@brief	Read register with error code return
*	@param [in]	u32Addr
*				Register address
*	@param [out]	pu32RetVal
*				Pointer to u32 variable used to return the read value
*	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_read_reg_with_ret(uint32 u32Addr, uint32* pu32RetVal);

/**
*	@fn		nm_write_reg
*	@brief	write register
*	@param [in]	u32Addr
*				Register address
*	@param [in]	u32Val
*				Value to be written to the register
*	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_write_reg(uint32 u32Addr, uint32 u32Val);

/**
*	@fn		nm_read_block
*	@brief	Read block of data
*	@param [in]	u32Addr
*				Start address
*	@param [out]	puBuf
*				Pointer to a buffer used to return the read data
*	@param [in]	u32Sz
*				Number of bytes to read. The buffer size must be >= u32Sz
*	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/ 
sint8 nm_read_block(uint32 u32Addr, uint8 *puBuf, uint32 u32Sz);

/**
*	@fn		nm_write_block
*	@brief	Write block of data
*	@param [in]	u32Addr
*				Start address
*	@param [in]	puBuf
*				Pointer to the buffer holding the data to be written
*	@param [in]	u32Sz
*				Number of bytes to write. The buffer size must be >= u32Sz
*	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/ 
sint8 nm_write_block(uint32 u32Addr, uint8 *puBuf, uint32 u32Sz);




#ifdef __cplusplus
}
#endif

#endif /* _NMBUS_H_ */
