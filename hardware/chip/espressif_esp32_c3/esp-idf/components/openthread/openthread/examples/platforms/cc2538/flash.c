/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "platform-cc2538.h"
#include "rom-utility.h"

#define FLASH_CTRL_FCTL_BUSY 0x00000080

#define FLASH_PAGE_SIZE 2048
#define FLASH_PAGE_NUM 2
#define FLASH_SWAP_SIZE (FLASH_PAGE_SIZE * (FLASH_PAGE_NUM / 2))

/* The linker script creates this external symbol */
extern uint8_t _FLASH_settings_pageA[];

/* Convert a settings offset to the physical address within the flash settings pages */
static uint32_t flashPhysAddr(uint8_t aSwapIndex, uint32_t aOffset)
{
    uint32_t address = (uint32_t)(&_FLASH_settings_pageA[0]) + aOffset;

    if (aSwapIndex)
    {
        address += FLASH_SWAP_SIZE;
    }

    return address;
}

void otPlatFlashInit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

uint32_t otPlatFlashGetSwapSize(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return FLASH_SWAP_SIZE;
}

void otPlatFlashErase(otInstance *aInstance, uint8_t aSwapIndex)
{
    OT_UNUSED_VARIABLE(aInstance);

    ROM_PageErase(flashPhysAddr(aSwapIndex, 0), FLASH_PAGE_SIZE);
    while (HWREG(FLASH_CTRL_FCTL) & FLASH_CTRL_FCTL_BUSY)
    {
    }
}

void otPlatFlashWrite(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, const void *aData, uint32_t aSize)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint32_t *data = (uint32_t *)(aData);

    for (uint32_t size = 0; size < aSize; size += sizeof(uint32_t), aOffset += sizeof(uint32_t), data++)
    {
        ROM_ProgramFlash(data, flashPhysAddr(aSwapIndex, aOffset), sizeof(uint32_t));

        while (HWREG(FLASH_CTRL_FCTL) & FLASH_CTRL_FCTL_BUSY)
        {
        }
    }
}

void otPlatFlashRead(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, uint8_t *aData, uint32_t aSize)
{
    OT_UNUSED_VARIABLE(aInstance);

    memcpy(aData, (void *)flashPhysAddr(aSwapIndex, aOffset), aSize);
}
