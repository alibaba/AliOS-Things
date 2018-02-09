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

#ifndef YUNOS_BSP_NORFLASH_H
#define YUNOS_BSP_NORFLASH_H


/********************************************************************************
 * Included Files
 ********************************************************************************/
#include <stdint.h>
#include <sys/types.h>


/********************************************************************************
 * FLASH bsp error number
 ********************************************************************************/
#define YUNOS_BSP_API_NORFLASH_READID           0x0100
#define YUNOS_BSP_API_NORFLASH_GET_INFO         0x0200
#define YUNOS_BSP_API_NORFLASH_READ             0x0300
#define YUNOS_BSP_API_NORFLASH_WRITE            0x0400
#define YUNOS_BSP_API_NORFLASH_BWRITE           0x0500
#define YUNOS_BSP_API_NORFLASH_ERASE_CHIP       0x0600
#define YUNOS_BSP_API_NORFLASH_ERASE_SECTOR     0x0700

typedef struct norflash_info
{
    uint32_t sector_size;        /* Size of one erase sector. */
    uint8_t sector_num;          /* Number of erase sector. */
    uint32_t erased_value;       /* the value after erase. */
} norflash_info_t;

/**
 * read the eflash id
 * @param[in]   id      the pointer to the id buffer.
 * @return      OK or error code
 */
int yunos_bsp_norflash_readid(void *id);

/**
 * Get the eflash information
 * @param[in]   finfo      the pointer to the struct norflash_info.
 * @return      OK or error code
 */
int yunos_bsp_norflash_get_info(norflash_info_t *finfo);

/**
 * Read eflash by byte
 * @param[in]   srcaddr     the absolute address of eflash to read.
 * @param[in]   dstbuff     the pointer to the data buffer.
 * @param[in]   len         the read byte num.
 * @return      OK or error code
 */
int yunos_bsp_norflash_read(uint32_t srcaddr, void *dstbuf, uint32_t len);

/**
 * Erase the eflash by sector
 * @param[in]   dstaddr     the absolute of flash address to erase(aligned by sector).
 * @param[in]   len         the erase len.
 * @return      OK or error code
 */
int yunos_bsp_norflash_erase_sector(uint32_t dstaddr, uint32_t len);

/**
 * Erase the eflash by chip
 * @return      OK or error code
 */
int yunos_bsp_norflash_erase_chip(void);

/**
 * Write eflash
 * @param[in]   dstaddr     the absolute address offset of eflash to write
 * @param[in]   srcbuf      the pointer to the data buffer.
 * @param[in]   len         the write num.
 * @return      OK or error code
 */

int yunos_bsp_norflash_write(uint32_t dstaddr, const void *srcbuf, uint32_t len);

#endif /* YUNOS_BSP_NORFLASH_H */

