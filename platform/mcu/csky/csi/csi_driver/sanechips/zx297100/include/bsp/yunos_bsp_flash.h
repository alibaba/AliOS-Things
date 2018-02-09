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

#ifndef YUNOS_BSP_FLASH_H
#define YUNOS_BSP_FLASH_H

typedef struct flash_geometry_s
{
    uint8_t  sectorshift;   ///< erase sector size. example: 2^12=4k
    uint8_t  pageshift;     ///< write page size. example: 2^8=256 byte
    uint16_t nsectors;      ///< the number of sectors. example: 2048, 2048*4k=8192k=8m
    uint32_t npages;        ///< the number of pages. example: 32768, 32768*256byte=8388608byte=8m
} flash_geometry_t;

/**
 * Get the ID infomation of FLASH, and config flash driver
 * @return      return Success or fail
 */
 int yunos_bsp_flash_read_id(void);

/**
 * Enable  FLASH
 * @return      void
 */
void yunos_bsp_flash_write_enable(void);

/**
 * Disable FLASH
 * @return      0, success; -1 read failed
 */
void yunos_bsp_flash_write_disable(void);

/**
 * Write the memory of FLASH by page
 * @param[in]   addr        Start address of write page
 * @param[in]   num         Length of write data
 * @param[in]   data        write data buffer
 * @return      zero is on success, -1 is on failed.
 */
int yunos_bsp_flash_page_program(int addr, int num, int data);

/**
 * Init flash.
 * @return      0, success; -1 failed
 */
int yunos_bsp_flash_init(void);

/**
 * Get the flash information
 * @return      flash_geometry_t *
 */
flash_geometry_t *yunos_bsp_flash_get_info(void);

/**
 * Read data from FLASH by bytes
 * @param[in]   offset      The start reading address
 * @param[in]   nbytes      The length of reading data
 * @param[in]   buffer      The buffer of reading data
 * @return      return readed  size; -1 Read failed
 */
ssize_t yunos_bsp_flash_read(off_t offset, size_t nbytes, FAR uint8_t *buffer);

/**
 * Erase the hole falsh chip
 * @return      0, success; -1 fail.
 */
int yunos_bsp_flash_chip_erase(void);

/**
 * Erase the flash in special address by command
 * @param[in]   addr        Address of the block need to be erased
 * @return      void
 */
void yunos_bsp_flash_sector_erase(int addr);

/**
 * Check the  Status Register weather OK, if not, check it again
 * @return      void
 */
void yunos_bsp_flash_wait_readable(void);

#endif /* YUNOS_BSP_FLASH_H */
