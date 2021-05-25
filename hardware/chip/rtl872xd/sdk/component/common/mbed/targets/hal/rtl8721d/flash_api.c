/** mbed Microcontroller Library
 ******************************************************************************
 * @file	flash_api.c
 * @author 
 * @version V1.0.0
 * @date	2016-08-01
 * @brief	This file provides mbed API for flash.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
 ****************************************************************************** 
 */



#include "objects.h"
#include "PinNames.h"


#include "pinmap.h"

#include "ameba_soc.h"
#include "flash_api.h"

extern u32 ConfigDebugInfo;

/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_FLASH
 *  @brief      MBED_FLASH driver modules.
 *  @{
 */

/** @defgroup MBED_FLASH_Exported_Functions MBED_FLASH Exported Functions
  * @{
  */

/**
  * @brief  Control the flash chip write protect enable/disable.
  * @param  obj: Flash object define in application software.
  * @param  protect: This parameter can be 1 or 0.
  *             @arg 1: Protect the whole chip from being programmed/erased.
  *             @arg 0: Unprotect the whole chip from being programmed/erased.
  * @retval none
  */   
void flash_write_protect(flash_t *obj, u32 protect)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	FLASH_Write_Lock();
	FLASH_SetStatusBits(0x1c, protect);
	FLASH_Write_Unlock();
}

/**
  * @brief  Set Status register to enable desired operation
  * @param  obj: Flash object define in application software.
  * @param  data: Specifies which bit users like to set.
  *        ex: if users want to set the third bit, data = 0x8. 
  * @retval 1: Success or Others: Failure.
  * @note  Please refer to the datatsheet of flash for more details of the content of status register.
  *        The block protected area and the corresponding control bits are provided in the flash datasheet.
  */  
int flash_set_status(flash_t *obj, u32 data)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	u8 status[2];
	u8 StatusLen = 1;

	status[0] = (u8)data;
	FLASH_Write_Lock();
	
	/* check if status2 */
	if (flash_init_para.FLASH_Status2_exist) {
		StatusLen = 2;
		FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status2, 1, &status[1]);
	}

	if(!flash_init_para.FLASH_cmd_wr_status2){
		FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, StatusLen, status);
	} else {
		FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, 1, &status[0]);
		FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status2, 1, &status[1]);
	}
	FLASH_Write_Unlock();

	return 1;
}

/**
  * @brief This function aims to reset the status register to 0, please make sure the operation is appropriate.
  * @param obj: Flash object define in application software.
  * @retval none 
  * @note Also reset the value of status register2 to 0 if flash chip has status register2
  */
void flash_reset_status(flash_t *obj)
{
	flash_set_status(obj, 0);
	return;
}


/**
  * @brief  Get the value of status register1
  * @param  obj: Flash object define in application software.
  * @retval : The value of status register1.
  */   
int flash_get_status(flash_t *obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	int data;
	
	FLASH_Write_Lock();
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status, 1, (u8*)&data);
	FLASH_Write_Unlock();
	
	return data;
}

/**
  * @brief  Erase flash sector(4KB)
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to be erased. LSB 12bits will be masked.
  * @retval none
  */
void flash_erase_sector(flash_t *obj, u32 address)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	FLASH_Write_Lock();
	FLASH_Erase(EraseSector, address);

	DCache_Invalidate(SPI_FLASH_BASE + address, 0x1000);
	FLASH_Write_Unlock();
}

/**
  * @brief  Erase flash block(64KB)
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to be erased. LSB 16bits will be masked.
  * @retval none
  */
void flash_erase_block(flash_t *obj, u32 address)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	FLASH_Write_Lock();
	FLASH_Erase(EraseBlock, address);

	DCache_Invalidate(SPI_FLASH_BASE + address, 0x10000);
	FLASH_Write_Unlock();
}

/**
  * @brief  Erase the whole flash chip
  * @param obj: Flash object define in application software.
  * @retval   none
  */
void flash_erase_chip(flash_t *obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	FLASH_Write_Lock();
	FLASH_Erase(EraseChip, 0);
	Cache_Flush();
	FLASH_Write_Unlock();
}

/**
  * @brief  Read a word from specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the address to read from.
  * @param  data: Specified the address to save the readback data.
  * @retval 1: Success or Others: Failure.
  * @note auto mode read is ok, because we have flash cache
  */
int  flash_read_word(flash_t *obj, u32 address, u32 * data)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
//	FLASH_RxData(0, address, 4, data);
	assert_param(data != NULL);
	
	u32 offset_to_align = address & 0x03;
	u32 read_data;
	u32 temp;
	u32 i = 4 - offset_to_align;

	if(offset_to_align){
		address -= offset_to_align;
		temp = HAL_READ32(SPI_FLASH_BASE, address);
		read_data= temp >> (offset_to_align * 8);

		address += 4;
		temp = HAL_READ32(SPI_FLASH_BASE, address);
		read_data |= (temp << (i * 8));

		*data = read_data;
	}else{
		* data = HAL_READ32(SPI_FLASH_BASE, address);
	}
	
	return 1;
}

/**
  * @brief  Write a word to specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the address to be programmed to.
  * @param  data: Specified the data to be programmed.
  * @retval 1: Success or Others: Failure.
  * @note user mode write used
  */
int  flash_write_word(flash_t *obj, u32 address, u32 data)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	// Disable write protection
	// flash_unlock();

	u32 write_word = data;
	u32 offset_to_align = address & 0x03;
	u32 temp;
	u32 i = 4 - offset_to_align;

	FLASH_Write_Lock();
	if(offset_to_align){
		address -= offset_to_align;
		temp = HAL_READ32(SPI_FLASH_BASE, address);
		temp = (temp << (i * 8))>>(8*i) | write_word << (8 * offset_to_align);
		FLASH_TxData12B(address, 4, (u8*)&temp);

		address += 4;
		temp = HAL_READ32(SPI_FLASH_BASE, address);
		temp = (temp >> (offset_to_align * 8)) << (offset_to_align * 8) | write_word >>(8*i);
		FLASH_TxData12B(address, 4, (u8*)&temp);
	}else{
		FLASH_TxData12B(address, 4, (u8*)&write_word);
	}

	DCache_Invalidate(SPI_FLASH_BASE + address, 4);
	FLASH_Write_Unlock();
	
	// Enable write protection
	// flash_lock();
	return 1;
}


/**
  * @brief  Read a stream of data from specified address
  * @param obj: Flash object define in application software.
  * @param  address: Specifies the starting address to read from.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval 1: Success or Others: Failure.
  * @note auto mode is ok, because we have flash cache
  */
int  flash_stream_read(flash_t *obj, u32 address, u32 len, u8 * data)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	assert_param(data != NULL);

	u32 offset_to_align;
	u32 i;
	u32 read_word;
	u8 *ptr;
	u8 *pbuf;

	offset_to_align = address & 0x03;
	pbuf = data;
	if (offset_to_align != 0) {
		/* the start address is not 4-bytes aligned */
		read_word = HAL_READ32(SPI_FLASH_BASE, (address - offset_to_align));
		ptr = (u8*)&read_word + offset_to_align;
		offset_to_align = 4 - offset_to_align;
		for (i=0;i<offset_to_align;i++) {
			*pbuf = *(ptr+i);
			pbuf++;
			len--;
			if (len == 0) {
				break;
			}
		}
	}

	/* address = next 4-bytes aligned */
	address = (((address-1) >> 2) + 1) << 2;

	ptr = (u8*)&read_word;
	if ((u32)pbuf & 0x03) {
		while (len >= 4) {
			read_word = HAL_READ32(SPI_FLASH_BASE, address);
			for (i=0;i<4;i++) {
				*pbuf = *(ptr+i);
				pbuf++;
			}
			address += 4;
			len -= 4;
		}
	} else {
		while (len >= 4) {
			*((u32 *)pbuf) = HAL_READ32(SPI_FLASH_BASE, address);
			pbuf += 4;
			address += 4;
			len -= 4;
		}
	}

	if (len > 0) {
		read_word = HAL_READ32(SPI_FLASH_BASE, address);
		for (i=0;i<len;i++) {
			*pbuf = *(ptr+i);
			pbuf++;
		}        
	}

	return 1;
}

/**
  * @brief  Write a stream of data to specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @retval 1: Success or Others: Failure.
  */
int  flash_stream_write(flash_t *obj, u32 address, u32 len, u8 * data)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	// Check address: 4byte aligned & page(256bytes) aligned
	u32 page_begin = address &  (~0xff);                     
	u32 page_end = (address + len) & (~0xff);
	u32 page_cnt = ((page_end - page_begin) >> 8) + 1;

	u32 addr_begin = address;
	u32 addr_end = (page_cnt == 1) ? (address + len) : (page_begin + 0x100);
	u32 size = addr_end - addr_begin;
	u8 *buffer = data;
	u8 write_data[12];
	
	u32 offset_to_align;
	u32 read_word;
	u32 i;

	FLASH_Write_Lock();
	while(page_cnt){	
		offset_to_align = addr_begin & 0x3;
		
		if(offset_to_align != 0){
			read_word = HAL_READ32(SPI_FLASH_BASE, addr_begin - offset_to_align);
			for(i = offset_to_align;i < 4;i++){
				read_word = (read_word &  (~(0xff << (8*i)))) |( (*buffer) <<(8*i));
				size--;
				buffer++;
				if(size == 0)
					break;
			}
			FLASH_TxData12B(addr_begin - offset_to_align, 4, (u8*)&read_word);
#ifdef MICRON_N25Q00AA
			FLASH_ReadFlagStatusReg();
#endif
		}

		addr_begin = (((addr_begin-1) >> 2) + 1) << 2;
		for(;size >= 12 ;size -= 12){
			_memcpy(write_data, buffer, 12);
			FLASH_TxData12B(addr_begin, 12, write_data);
#ifdef MICRON_N25Q00AA
			FLASH_ReadFlagStatusReg();
#endif
			buffer += 12;
			addr_begin += 12;
		}

		for(;size >= 4; size -=4){
			_memcpy(write_data, buffer, 4);			
			FLASH_TxData12B(addr_begin, 4, write_data);
#ifdef MICRON_N25Q00AA
			FLASH_ReadFlagStatusReg();
#endif
			buffer += 4;
			addr_begin += 4;
		}

		if(size > 0){
			read_word = HAL_READ32(SPI_FLASH_BASE, addr_begin);
			for( i = 0;i < size;i++){
				read_word = (read_word & (~(0xff << (8*i)))) | ((*buffer) <<(8*i));
				buffer++;
			}
			FLASH_TxData12B(addr_begin, 4, (u8*)&read_word); 
#ifdef MICRON_N25Q00AA
			FLASH_ReadFlagStatusReg();
#endif
		}

		page_cnt--;
		addr_begin = addr_end;
		addr_end = (page_cnt == 1) ? (address + len) : (((addr_begin>>8) + 1)<<8);
		size = addr_end - addr_begin;		
	}

	DCache_Invalidate(SPI_FLASH_BASE + address, len);
	FLASH_Write_Unlock();

	return 1;
}

/**
  * @brief  It is the same with flash_stream_write function which is used to write a stream of data to specified address.
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @retval 1: Success or Others: Failure.
  */
int flash_burst_write(flash_t *obj, u32 address ,u32 Length, u8 * data)
{
	flash_stream_write(obj, address, Length, data);
	return 1;
}

/**
  * @brief   It is the same with flash_stream_read function which is used to read a stream of data from specified address
  * @param  obj: Flash object define in application software.
  * @param  address: Specifies the starting address to read from.
  * @param  len: Specifies the length of the data to read.
  * @param  data: Specified the address to save the readback data.
  * @retval 1: Success or Others: Failure.
  */
int  flash_burst_read(flash_t *obj, u32 address, u32 Length, u8 * data)
{
	flash_stream_read(obj, address, Length, data);
	return 1;
}


/**
  * @brief   This function is only for Micron 128MB flash to access beyond 16MB by switching between eight 16MB-area(segment).
  *             Please refer to flash datasheet for more information about memory mapping.
  * @param  obj: Flash object define in application software.
  * @param  data: Specified which segment to choose.
  * @retval 1: Success or Others: Failure.
  */
int flash_set_extend_addr(flash_t *obj, u32 data)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	/*Write Extended Address Register to select operating segment*/
	u8 segnum = (u8)(data & 0x07);

	FLASH_Write_Lock();
	FLASH_SetStatus(0xC5, 1, &segnum);
	FLASH_Write_Unlock();
	return 1;
}

/**
  * @brief   This function is only for Micron 128MB flash to read from Extended Address Register, which shows the current segment.
  *             Please refer to flash datasheet for more information about memory mapping.
  * @param  obj: Flash object define in application software.
  * @retval : The value of current Extended Address Register.
  */
int flash_get_extend_addr(flash_t *obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	u8 temp = 0;

	FLASH_Write_Lock();
	FLASH_RxCmd(0xC8, 1, &temp);
	FLASH_Write_Unlock();
	return temp;
}

/**
  * @brief   This function is to read flash id.        
  * @param  obj: Flash object define in application software.
  * @param  buf: Specified the address to save the readback data.
  * @param  len: Specifies the length of the flash id to read.
  * @retval : The length of the flash id.
  */
int flash_read_id(flash_t *obj, uint8_t *buf, uint8_t len)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	assert_param(buf != NULL);
	assert_param(len >= 3);

	FLASH_Write_Lock();
	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, len, buf);
	FLASH_Write_Unlock();

	return len;
}
/** 
  * @}
  */

/** 
  * @}
  */

/** 
  * @}
  */