/*
 * Copyright 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MFLASH_DRV_H__
#define __MFLASH_DRV_H__

#include <stdbool.h>
#include <stdint.h>

/* Flash constants */
#ifndef MFLASH_SECTOR_MASK
#define MFLASH_SECTOR_MASK  (0xFFF)
#endif

#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE  (4096)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE    (256)
#endif

#ifndef MFLASH_SPIFI
#define MFLASH_SPIFI        SPIFI0
#endif

#ifndef MFLASH_BAUDRATE
#define MFLASH_BAUDRATE     (96000000)
#endif

static inline uint32_t mflash_drv_is_sector_aligned(uint32_t addr)
{
    return ((addr) & (MFLASH_SECTOR_MASK)) == 0 ? true : false;
}

static inline uint32_t mflash_drv_addr_to_sector_addr(uint32_t addr)
{
    return (addr & (~((uint32_t)(MFLASH_SECTOR_MASK))));
}

static inline uint32_t mflash_drv_addr_to_sector_of(uint32_t addr)
{
    return ((addr) & ((uint32_t)(MFLASH_SECTOR_MASK)));
}

int32_t mflash_drv_init(void);
int32_t mflash_drv_write(void *any_addr, uint8_t *data, uint32_t data_len);

#endif
