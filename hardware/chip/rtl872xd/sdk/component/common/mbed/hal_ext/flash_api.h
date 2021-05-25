/** mbed Microcontroller Library
  ******************************************************************************
  * @file    flash_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed FLASH API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  ****************************************************************************** 
  */

#ifndef MBED_EXT_FLASH_API_EXT_H
#define MBED_EXT_FLASH_API_EXT_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup flash FLASH
 *  @ingroup    hal
 *  @brief      flash functions
 *  @{
 */

///@name Ameba Common
///@{

typedef struct flash_s flash_t;

/**
  * @brief  Erase flash sector
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to be erased.
  * @retval none
  */
void flash_erase_sector(flash_t *obj, uint32_t address);

/**
  * @brief  Erase flash block(64KB)
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to be erased.LSB 16bits will be masked.
  * @retval none
  */
void flash_erase_block(flash_t * obj, uint32_t address);

/**
  * @brief  Read a word from specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the address to read from.
  * @param  data: Specified the address to save the readback data.
  * @retval 1: Success
  * @note auto mode read is ok, because we have flash cache
  */
int flash_read_word(flash_t *obj, uint32_t address, uint32_t * data);

/**
  * @brief  Write a word to specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the address to be programmed to.
  * @param  data: Specified the data to be programmed.
  * @retval 1: Success
  * @note user mode write used
  */
int flash_write_word(flash_t *obj, uint32_t address, uint32_t data);

/**
  * @brief  Read a stream of data from specified address
  * @param obj: Flash object define in application software.
  * @param  address: Specifies the starting address to read from.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval 1: Success
  * @note auto mode is ok, because we have flash cache
  */
int flash_stream_read(flash_t *obj, uint32_t address, uint32_t len, uint8_t * data);

/**
  * @brief  Write a stream of data to specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @retval 1: Success
  */
int flash_stream_write(flash_t *obj, uint32_t address, uint32_t len, uint8_t * data);

/**
  * @brief  Control the flash chip write protect enable/disable.
  * @param  obj: Flash object define in application software.
  * @param  protect: This parameter can be 1 or 0.
  *             @arg 1: Protect the whole chip from being programmed/erased.
  *             @arg 0: Unprotect the whole chip from being programmed/erased.
  * @retval none
  */   
void flash_write_protect(flash_t *obj, uint32_t protect);

/**
  * @brief  Get the value of status register1
  * @param  obj: Flash object define in application software.
  * @retval : The value of status register1.
  */   
int flash_get_status(flash_t * obj);

/**
  * @brief  Set Status register to enable desired operation
  * @param  obj: Specifies the parameter of flash object.
  * @param  data: Specifies which bit users like to set.
  *        ex: if users want to set the third bit, data = 0x8. 
  * @retval 1: Success
  * @note  Please refer to the datatsheet of flash for more details of the content of status register.
  *        The block protected area and the corresponding control bits are provided in the flash datasheet.
  */  
int flash_set_status(flash_t * obj, uint32_t data);

/**
  * @brief This function aims to reset the status register, please make sure the operation is appropriate.
  * @param obj: Specifies the parameter of flash object.
  * @retval none 
  */
void flash_reset_status(flash_t * obj);

/**
  * @brief  It is the same with flash_stream_write function which is used to write a stream of data to specified address.
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @retval 1: Success
  */
int flash_burst_write(flash_t * obj, uint32_t address, uint32_t Length, uint8_t * data);

/**
  * @brief   It is the same with flash_stream_read function which is used to read a stream of data from specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to read from.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval 1: Success
  */
int flash_burst_read(flash_t * obj, uint32_t address, uint32_t Length, uint8_t * data);

/**
  * @brief   This function is only for Micron 128MB flash to access beyond 16MB by switching between eight 16MB-area(segment).
  *             Please refer to flash datasheet for more information about memory mapping.
  * @param  obj: Flash object define in application software.
  * @param  data: Specified which segment to choose.
  * @retval 1: Success
  */
int flash_set_extend_addr(flash_t * obj, uint32_t data);

/**
  * @brief   This function is only for Micron 128MB flash to read from Extended Address Register, which shows the current segment.
  *             Please refer to flash datasheet for more information about memory mapping.
  * @param  obj: Flash object define in application software.
  * @retval : The value of current Extended Address Register.
  */
int flash_get_extend_addr(flash_t * obj);

/**
  * @brief  Get flash ID (command: 0x9F). 
  * @param  obj: Flash object define in application software.
  * @param  buf: Pointer to a byte array to save the readback ID.
  * @param  len: Specifies the length of the buf. It should be 3.
  * @retval -1: Fail.
  */
int flash_read_id(flash_t *obj, uint8_t *buf, uint8_t len);

/**
  * @brief  This function is only for Winbond flash to get unique ID (command: 0x4B).
  * @param  obj: Flash object define in application software.
  * @param  buf: Pointer to a byte array to save the readback unique ID.
  * @param  len: Specifies the length of the buf. It should be 8.
  * @retval -1: Fail.
  */
int flash_read_unique_id(flash_t *obj, uint8_t *buf, uint8_t len);

/**
  * @brief  This function is only for Winbond flash to set lock mode.
  * @param  mode: This parameter can be 1 or 0.
  *             @arg 1: Enable individual sector / block protect feature.
  *             @arg 0: Set status register 1 to enble write protect feature.
  * @retval none
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
void flash_set_lock_mode(uint32_t mode);

/**
  * @brief  This function is only for Winbond flash to lock whole flash chip.
  * @param  none
  * @retval none
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
void flash_global_lock(void);

/**
  * @brief  This function is only for Winbond flash to unlock whole flash chip.
  * @param  none
  * @retval none
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
void flash_global_unlock(void);

/**
  * @brief  This function is only for Winbond flash to lock individual sector or block region, should refer to the datasheet for more details.
  * @param  address
  * @retval none
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
void flash_individual_lock(uint32_t address);

/**
  * @brief  This function is only for Winbond flash to unlock individual sector or block region, should refer to the datasheet for more details.
  * @param  address
  * @retval none
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
void flash_individual_unlock(uint32_t address);

/**
  * @brief  This function is only for Winbond flash to get the individual lock state on certain address.
  * @param  address
  * @retval 1: the target sector/block is locked. 
  *			0: the target sector/block is not locked.
  * @note But not all Winbond flash supports the function, plase refer to data sheets of the target flashes.
  */
int flash_read_individual_lock_state(uint32_t address);

///@}

#if ((defined(CONFIG_PLATFORM_8711B) && (CONFIG_PLATFORM_8711B == 1)) || (defined (CONFIG_PLATFORM_8721D) && (CONFIG_PLATFORM_8721D == 1)))
///@name AmebaZ and AmebaD 
///@{
/**
  * @brief  Erase the whole flash chip
  * @param obj: Flash object define in application software.
  * @retval   none
  */
void flash_erase_chip(flash_t *obj);
///@}
#endif //CONFIG_PLATFORM_8711B and CONFIG_PLATFORM_8721D

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif
