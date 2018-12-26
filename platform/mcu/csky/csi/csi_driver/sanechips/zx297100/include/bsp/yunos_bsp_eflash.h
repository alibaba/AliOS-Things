/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_EFLASH_H
#define YUNOS_BSP_EFLASH_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include "cffs_flash.h"

/**
 * Get the eflash information
 * @param[in]   finfo       pointer to the elash information
 * @return      0 on success; -1 on failure 
 */
int yunos_bsp_eflash_getinfo(flash_info_t *finfo);

/**
 * @brief Read eflash data.
 *
 * @param addroffset Begining read address.
 * @param buff       Pointer to the dest data.
 * @param bytesize   Size of read in bytes.
 *
 * @return Standard errno
 */
int yunos_bsp_eflash_read(uint32_t addroffset, uint8_t *buff, uint32_t bytesize);
								
/**
 * @brief Program eflash data in common mode.
 *
 * @param addroffset Begining write address.
 * @param buff       Pointer to the src data.
 * @param bytesize   Size of write in bytes.
 *
 * @return Standard errno
 */
int yunos_bsp_eflash_write(uint32_t addroffset, uint8_t *buff, uint32_t bytesize);
								
/**
 * @brief Program eflash data in poll mode.
 *
 * @param addroffset Begining write address.
 * @param buff   	 Pointer to the src data.
 * @param bytesize   Size of write in bytes.
 *
 * @return Standard errno
 */
int yunos_bsp_eflash_write_poll(uint32_t addroffset, uint8_t *buff, uint32_t bytesize);

/**
 * Read eflash by word
 * @param[in] addroffset  	address offset of eflash to write
 * @param[in] buff        	pointer to buffer to restore data
 * @param[in] bytesize    	words to write
 * @return      0 on success; -1 on failure 
 */
int yunos_bsp_eflash_wwrite(uint32_t addroffset, uint32_t *buff, int wordsize);

/**
 * @brief Erase eflash.
 *
 * @param eblk	   Begining erase address.
 * @param neblks   Size of erase in bytes.
 *
 * @return Standard errno
 */
int yunos_bsp_eflash_erase(uint32_t eblk, uint32_t neblks);

/**
 * @brief Erase eflash in poll mode.
 *
 * @param eblk		Begining erase address.
 * @param neblks    Size of erase in bytes.
 *
 * @return Standard errno
 */
int yunos_bsp_eflash_erase_poll(uint32_t eblk, uint32_t neblks);

/**
 * Read eflash by block
 * @param[in] ioblk        blocks offset of eflash to write
 * @param[in] nblks       blocks to write
 * @param[in] buffer      pointer to buffer to write
 * @return      0 on success; -1 on failure 
 */
int yunos_bsp_eflash_bwrite(uint32_t ioblk, uint32_t nblks, const void *buffer);

/**
 * Read eflash by block
 * @param[in] ioblk        blocks offset of eflash to read
 * @param[in] nblks       blocks to read
 * @param[in] buffer      pointer to buffer to read
 * @return      0 on success; -1 on failure 
 */
int yunos_bsp_eflash_bread(uint32_t ioblk, uint32_t nblks, void *buffer);


/**
 * @brief Set eflash 0/1 information page 0/1 as otp area.
 *
 * @param enable Bool:1/0
 *
 * @return Standard errno
 */
int yunos_bsp_eflash_write_protection(uint32_t enable);

#endif /* YUNOS_BSP_EFLASH_H */
