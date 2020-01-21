/*
 * The Clear BSD License
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this
 * list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice,
 * this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_mini_bl.h"
#include "evkbimxrt1050_flexspi_nor_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FLASH_BASE 0x60000000U
#define FLASH_SECTOR_SIZE_BYTES 0x40000
#define FLASH_PAGE_SIZE_BYTES  512
#define FLASH_SIZE_KBYTES 0x10000

typedef struct
{
    uint32_t version;
    status_t (*init)(uint32_t instance, flexspi_nor_config_t *config);
    status_t (*program)(uint32_t instance, flexspi_nor_config_t *config, uint32_t dst_addr, const uint32_t *src);
    status_t (*erase_all)(uint32_t instance, flexspi_nor_config_t *config);
    status_t (*erase)(uint32_t instance, flexspi_nor_config_t *config, uint32_t start, uint32_t lengthInBytes);
    status_t (*read)(uint32_t instance, flexspi_nor_config_t *config, uint32_t *dst, uint32_t addr, uint32_t lengthInBytes);
    void (*clear_cache)(uint32_t instance);
    status_t (*xfer)(uint32_t instance, flexspi_xfer_t *xfer);
    status_t (*update_lut)(uint32_t instance, uint32_t seqIndex, const uint32_t *lutBase, uint32_t seqNumber);
}flexspi_nor_driver_interface_t;


/*******************************************************************************
 * Variables
 ******************************************************************************/
static flexspi_nor_driver_interface_t *hyperflashInterface = (flexspi_nor_driver_interface_t *)0x002012b8U;

static flexspi_nor_config_t hyperflash_config_ram;

extern const flexspi_nor_config_t hyperflash_config;

static bool isFlashInited = false;

/* Must be 4-byte aligned. */
SDK_ALIGN(static uint8_t intBuffer[FLASH_PAGE_SIZE_BYTES], 4);

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t hyperflash_init(void)
{
    status_t status;

    /* Copy the flash configuration to RAM. */
    hyperflash_config_ram = hyperflash_config;

    status = hyperflashInterface->init(0, &hyperflash_config_ram);

    if (kStatus_Success != status)
    {
        return status;
    }

    isFlashInited = true;

    return kStatus_Success;
}

static status_t hyperflash_read(uint32_t addr, void *data, uint32_t size)
{
    status_t status;

    if (!isFlashInited)
    {
        status = hyperflash_init();

        if(kStatus_Success != status)
        {
            return status;
        }
    }

    memcpy(data, (void*)addr, size);

    return kStatus_Success;
}

static status_t hyperflash_write(uint32_t addr, const void *data, uint32_t size)
{
    status_t status;
    uint32_t phy_address;
    uint32_t offset;
    uint32_t phyAddrAlign;
    uint32_t memcpylen;
    uint32_t sizeLeft = size;
    const uint8_t *buffer = (const uint8_t *)data;

    if (!isFlashInited)
    {
        status = hyperflash_init();

        if(kStatus_Success != status)
        {
            return status;
        }
    }

    phy_address = addr - FLASH_BASE;

    /* Parameter check */
    if (phy_address + size > FLASH_SIZE_KBYTES * 1024)
    {
        return kStatus_Fail;
    }

    /* Check if the startaddress is the page size aligned */
    if ((phy_address % FLASH_PAGE_SIZE_BYTES) != 0)
    {
        offset = phy_address % FLASH_PAGE_SIZE_BYTES;
        phyAddrAlign = (phy_address / FLASH_PAGE_SIZE_BYTES) * FLASH_PAGE_SIZE_BYTES;
        memcpylen = size;

        /* Check if the area across pages. */
        if (offset + size > FLASH_PAGE_SIZE_BYTES)
        {
            memcpylen = FLASH_PAGE_SIZE_BYTES - offset;
        }

        /* Need to read the page first*/
        memcpy(intBuffer, (void*)(phyAddrAlign + FLASH_BASE), FLASH_PAGE_SIZE_BYTES);

        /* Change the data required to be changed. */
        memcpy((void*)(((uint32_t)intBuffer) + offset), buffer, memcpylen);

        /* Write the first unaligned page then */
        status = hyperflashInterface->program(0, &hyperflash_config_ram, phyAddrAlign, intBuffer);
        if (status != kStatus_Success)
        {
            return status;
        }

        /* Update the address and size */
        phy_address = phyAddrAlign + FLASH_PAGE_SIZE_BYTES;
        sizeLeft -= memcpylen;
        buffer += memcpylen;
    }

    /* Now the startAddr is page size aligned. */
    while ( sizeLeft >= FLASH_PAGE_SIZE_BYTES)
    {
        /* Make sure 4 byte align. */
        if (0!= ((uint32_t)buffer & 0x03))
        {
            memcpy(intBuffer, buffer, FLASH_PAGE_SIZE_BYTES);
            status = hyperflashInterface->program(0, &hyperflash_config_ram, phy_address, intBuffer);
        }
        else
        {
            status = hyperflashInterface->program(0, &hyperflash_config_ram, phy_address, buffer);
        }

        if (status != kStatus_Success)
        {
            return status;
        }

        phy_address += FLASH_PAGE_SIZE_BYTES;
        sizeLeft -= FLASH_PAGE_SIZE_BYTES;
        buffer += FLASH_PAGE_SIZE_BYTES;
    }

    if (sizeLeft)
    {
        memcpy(intBuffer, buffer, sizeLeft);

        memcpy(intBuffer + sizeLeft, (void*)(phy_address + FLASH_BASE + sizeLeft), FLASH_PAGE_SIZE_BYTES - sizeLeft);

        status = hyperflashInterface->program(0, &hyperflash_config_ram, phy_address, intBuffer);
        if (status != kStatus_Success)
        {
            return status;
        }
    }

    return kStatus_Success;
}

static status_t hyperflash_erase(uint32_t addr, uint32_t size)
{
    status_t status;
    uint32_t phy_address = addr - FLASH_BASE;

    size = (((size - 1 ) / FLASH_SECTOR_SIZE_BYTES) + 1) * FLASH_SECTOR_SIZE_BYTES;

    if (!isFlashInited)
    {
        status = hyperflash_init();

        if(kStatus_Success != status)
        {
            return status;
        }
    }

    return hyperflashInterface->erase(0, &hyperflash_config_ram, phy_address, size);
}

const fsl_mini_bl_flash_ops_t flashOps =
{
    .read = hyperflash_read,
    .write = hyperflash_write,
    .erase = hyperflash_erase,
};
