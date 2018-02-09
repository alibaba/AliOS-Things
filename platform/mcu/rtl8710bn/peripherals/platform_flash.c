/**
******************************************************************************
* @file    platform_flash.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provides flash operation functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 

/* Includes ------------------------------------------------------------------*/
//#include "MICOPlatform.h"
//#include "MICORTOS.h"

#include "platform_logging.h"
//#include "MicoPlatform.h"
#include "board.h"
#include "platform_config.h"
#include "platform_peripheral.h"
//#include "PlatformLogging.h"
#include "stdio.h"

#include "device_lock.h"

#ifdef USE_MICO_SPI_FLASH
//#include "spi_flash.h"
//#include "hal_platform.h"
//#include "hal_spi_flash.h"
//#include "rtl8195a_spi_flash.h"
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USE_MICO_SPI_FLASH
//static sflash_handle_t sflash_handle = {0x0, 0x0, SFLASH_WRITE_NOT_ALLOWED};
#endif
/* Private function prototypes -----------------------------------------------*/

OSStatus platform_flash_init( const platform_flash_t *peripheral )
{
  	OSStatus err = kNoErr;

  	require_action_quiet( peripheral != NULL, exit, err = kParamErr);

  	if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    		err = kUnsupportedErr;
    		require_noerr(err, exit);
  	}
  	else{
    		err = kNoErr;
    		goto exit;
  	}
exit:
  	return err;
}

OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  	OSStatus err = kNoErr;
  	int section_start, section_num, section_end, section_ro_start, ro_section_num, start_sector;
  	int i =0;

  	require_action_quiet( end_address > start_address, exit, err = kParamErr);
  	require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  	require_action( start_address >= peripheral->flash_start_addr 
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  	section_ro_start = peripheral->flash_readonly_start/0x1000;
  	ro_section_num = peripheral->flash_readonly_len/0x1000;
  	if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    		err = kUnsupportedErr;  
    		require_noerr(err, exit);
  	}
#ifdef USE_MICO_SPI_FLASH
  	else if( peripheral->flash_type == FLASH_TYPE_SPI ){
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		start_sector = start_address/0x1000;
		section_start = start_sector*0x1000;
		section_end = (end_address+0x1000-1)/0x1000*0x1000;
		section_num = (section_end - section_start)/0x1000;
		section_ro_start = section_ro_start - start_sector;
		for(i =0; i < section_num; i++){
			if (i == section_ro_start) {// read only section don't write 
				i += ro_section_num;
				i--;
				continue;
			}
			flash_erase_sector((flash_t *)&peripheral->flash_obj, section_start + i*0x1000);	
		}
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
  	}
#endif
  	else{
    		err = kTypeErr;
    		goto exit;
  	}

exit:
  	return err;
}

OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
  	OSStatus err = kNoErr;
  	uint32_t ro_start, ro_end;
  	uint32_t start = *start_address, end=*start_address + length;
  	uint32_t write_len;
	u8 *sram_data = malloc(length);
	memcpy(sram_data, data, length);
  	ro_start = peripheral->flash_readonly_start;
  	ro_end = peripheral->flash_readonly_len + ro_start;
  	require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  	require_action( *start_address >= peripheral->flash_start_addr 
               && *start_address + length <= peripheral->flash_start_addr + peripheral->flash_length, exit, err = kParamErr);

  	if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    		err = kUnsupportedErr;    
    		require_noerr(err, exit);
  	}
#ifdef USE_MICO_SPI_FLASH
  	else if( peripheral->flash_type == FLASH_TYPE_SPI ){
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		if ((start > ro_end) || (end < ro_start)) {
			flash_stream_write((flash_t *)&peripheral->flash_obj, start, length, sram_data);
		} else {
			if (start < ro_start) {
				write_len = ro_start - start;
				flash_stream_write((flash_t *)&peripheral->flash_obj, start, write_len, sram_data);
			} 
			if (ro_end < end) {// start is in ro_start~ro_end
				write_len = ro_end - start;
				sram_data += write_len;
				flash_stream_write((flash_t *)&peripheral->flash_obj, ro_end, length - write_len, sram_data);
				
			}
		}
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
    		*start_address += length;
  	}
#endif
  	else{
    		err = kTypeErr;
    		goto exit;
  	}

	free(sram_data);

exit:
  	return err;
}

OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
  	OSStatus err = kNoErr;
	u8 *sram_data = malloc(length);

  	require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  	require_action( (*start_address >= peripheral->flash_start_addr) 
               && (*start_address + length) <= ( peripheral->flash_start_addr + peripheral->flash_length), exit, err = kParamErr);

  	if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    		err = kUnsupportedErr;
    		require_noerr(err, exit);    
  	}
#ifdef USE_MICO_SPI_FLASH
  	else if( peripheral->flash_type == FLASH_TYPE_SPI ){
		device_mutex_lock(RT_DEV_LOCK_FLASH);
    		flash_stream_read((flash_t*)&peripheral->flash_obj, (*start_address), length, sram_data);
		memcpy(data, sram_data, length);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
		*start_address += length;
  	}
#endif
  	else{
    		err = kTypeErr;
    		goto exit;
  	}

	free(sram_data);
exit:
  	return err;
}

OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  	OSStatus err = kNoErr;

  	require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  	require_action( start_address >= peripheral->flash_start_addr 
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  	if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    		err = kUnsupportedErr;
    		require_noerr(err, exit);
  	}
#ifdef USE_MICO_SPI_FLASH
  	else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    		err = kNoErr;
    		goto exit;
  	}
#endif
  	else{
    		err = kTypeErr;
    		goto exit;
  	}

exit:
  	return err;  
}

OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  	OSStatus err = kNoErr;

  	require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  	require_action( start_address >= peripheral->flash_start_addr 
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  	if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    		err = kUnsupportedErr;
    		require_noerr(err, exit);
  	}
#ifdef USE_MICO_SPI_FLASH
  	else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    		err = kNoErr;
    		goto exit;
  	}
#endif
  	else{
    		err = kTypeErr;
    		goto exit;
  	}

exit:
  	return err;  
}

