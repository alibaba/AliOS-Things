/**
 * @file    wm_flash_map.h
 *
 * @brief   flash zone map
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_FLASH_MAP_H__
#define __WM_FLASH_MAP_H__

/**FLASH MAP**/

#define FLASH_BASE_ADDR						(0x8000000)
#define SECBOOT_HEADER_ADDR 	(FLASH_BASE_ADDR  + 0x02000) 			/*256bytes*/
#define SECBOOT_AREA_ADDR		(FLASH_BASE_ADDR  + 0x02100)         /*(32K-256)bytes*/
#define IMAGE_HEADER_ADDR1		(FLASH_BASE_ADDR  + 0x0A000)
#define IMAGE_HEADER_ADDR2		(FLASH_BASE_ADDR  + 0x0B000)
#define TLS_FLASH_PARAM1_ADDR				(FLASH_BASE_ADDR  + 0x0C000)
#define TLS_FLASH_PARAM2_ADDR				(FLASH_BASE_ADDR  + 0x0D000)

#define CODE_START_ADDR			(FLASH_BASE_ADDR  + 0x0E000)
#define CODE_END_ADDR			(USER_ADDR_START  - 1)
#define USER_ADDR_START			(FLASH_BASE_ADDR  + 0xEF000)
#define TLS_FLASH_PARAM_DEFAULT  (USER_ADDR_START)


#define TLS_BOOT_IMAGE_SIGNATURE_WORD      0xa0ffff9f
#define SIGNATURE_WORD      				0xa0ffff9f

#endif /*__WM_CONFIG_H__*/

