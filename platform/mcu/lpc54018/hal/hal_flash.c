/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
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
 *
 * POSIX getopt for Windows
 * Code given out at the 1985 UNIFORUM conference in Dallas.
 *
 * From std-unix@ut-sally.UUCP (Moderator, John Quarterman) Sun Nov  3 14:34:15 1985
 * Relay-Version: version B 2.10.3 4.3bsd-beta 6/6/85; site gatech.CSNET
 * Posting-Version: version B 2.10.2 9/18/84; site ut-sally.UUCP
 * Path: gatech!akgua!mhuxv!mhuxt!mhuxr!ulysses!allegra!mit-eddie!genrad!panda!talcott!harvard!seismo!ut-sally!std-unix
 * From: std-unix@ut-sally.UUCP (Moderator, John Quarterman)
 * Newsgroups: mod.std.unix
 * Subject: public domain AT&T getopt source
 * Message-ID: <3352@ut-sally.UUCP>
 * Date: 3 Nov 85 19:34:15 GMT
 * Date-Received: 4 Nov 85 12:25:09 GMT
 * Organization: IEEE/P1003 Portable Operating System Environment Committee
 * Lines: 91
 * Approved: jsq@ut-sally.UUC
 * Here's something you've all been waiting for:  the AT&T public domain
 * source for getopt(3).  It is the code which was given out at the 1985
 * UNIFORUM conference in Dallas.  I obtained it by electronic mail
 * directly from AT&T.  The people there assure me that it is indeed
 * in the public domain
 * There is no manual page.  That is because the one they gave out at
 * UNIFORUM was slightly different from the current System V Release 2
 * manual page.  The difference apparently involved a note about the
 * famous rules 5 and 6, recommending using white space between an option
 * and its first argument, and not grouping options that have arguments.
 * Getopt itself is currently lenient about both of these things White
 * space is allowed, but not mandatory, and the last option in a group can
 * have an argument.  That particular version of the man page evidently
 * has no official existence, and my source at AT&T did not send a copy.
 * The current SVR2 man page reflects the actual behavor of this getopt.
 * However, I am not about to post a copy of anything licensed by AT&T.
 */
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "aos/hal/flash.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FLASH_BASE 0x10000000U
#define FLASH_PAGE_SIZE_BYTES (256)
#define FLASH_SECTOR_SIZE_BYTES (4096)
#define FLASH_SIZE_KBYTES (0x20000U)
#define QUAD_MODE_VAL 0x02


#include "fsl_spifi.h"

#define COMMAND_NUM (6)
#define READ (0)
#define PROGRAM_PAGE (1)
#define GET_STATUS (2)
#define ERASE_SECTOR (3)
#define WRITE_ENABLE (4)
#define WRITE_REGISTER (5)

#define EIO 5

/*******************************************************************************
 * Variables
 ******************************************************************************/
static bool isFlashInited = false;

spifi_command_t command[COMMAND_NUM] = {
    {FLASH_PAGE_SIZE_BYTES, false, kSPIFI_DataInput, 1, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x6B},
    {FLASH_PAGE_SIZE_BYTES, false, kSPIFI_DataOutput, 0, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x32},
    {1, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x05},
    {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
    {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x06},
    {1, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x31}};


/* Must be 4-byte aligned. */
SDK_ALIGN(static uint8_t intBuffer[FLASH_PAGE_SIZE_BYTES], 4);

typedef int32_t (*flash_device_erase)(uint32_t phy_address, uint32_t size);
typedef int32_t (*flash_device_write)(uint32_t phy_address, const void *data, uint32_t size);
typedef int32_t (*flash_device_read)(uint32_t phy_address, void *data, uint32_t size);

typedef struct 
{
    uint32_t start_address;
    uint32_t disk_size;
    uint32_t page_size;
    uint32_t sector_size;
    flash_device_erase erase;
    flash_device_write write;
    flash_device_read  read;
} flash_device_t;


static void check_if_finish()
{
    uint32_t val = 0;
    /* Check WIP bit */
    do
    {
        SPIFI_SetCommand(BOARD_SPIFI_QSPI, &command[GET_STATUS]);
        while ((BOARD_SPIFI_QSPI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U)
        {
        }
        val = SPIFI_ReadData(BOARD_SPIFI_QSPI);
    } while (val & 0x1);
}

static void enable_quad_mode()
{
    /* Write enable */
    SPIFI_SetCommand(BOARD_SPIFI_QSPI, &command[WRITE_ENABLE]);

    /* Set write register command */
    SPIFI_SetCommand(BOARD_SPIFI_QSPI, &command[WRITE_REGISTER]);

    SPIFI_WriteData(BOARD_SPIFI_QSPI, QUAD_MODE_VAL);

    check_if_finish();
}

static void program_page(uint32_t address, const uint8_t *buffer)
{
    SPIFI_SetCommand(BOARD_SPIFI_QSPI, &command[WRITE_ENABLE]);
    SPIFI_SetCommandAddress(BOARD_SPIFI_QSPI, address);
    SPIFI_SetCommand(BOARD_SPIFI_QSPI, &command[PROGRAM_PAGE]);

    uint32_t i = 0, j = 0, pdata = 0;

    for (i = 0; i < FLASH_PAGE_SIZE_BYTES; i += 4)
    {
        for (j = 0; j < 4; j++)
        {
            pdata |= ((uint32_t)(buffer[i + j])) << (j * 8);
        }
        SPIFI_WriteData(BOARD_SPIFI_QSPI, pdata);
        pdata = 0;
    }
    check_if_finish();
}
static status_t qspiflash_init(void)
{
    status_t status;
    spifi_config_t config = {0};
    uint32_t sourceClockFreq;

    /* Set SPIFI clock source */
    CLOCK_AttachClk(kFRO_HF_to_SPIFI_CLK);
    sourceClockFreq = CLOCK_GetFroHfFreq();

    /* Set the clock divider */
    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, sourceClockFreq / 96000000, false);

    /* Initialize SPIFI */
    SPIFI_GetDefaultConfig(&config);
    SPIFI_Init(BOARD_SPIFI_QSPI, &config);

    /* Enable Quad mode */
    enable_quad_mode();

    /* Setup memory command */
    SPIFI_SetMemoryCommand(BOARD_SPIFI_QSPI, &command[READ]);
    SPIFI_ResetCommand(BOARD_SPIFI_QSPI);
    EnableIRQ(SPIFI0_IRQn);

    isFlashInited = true;

    return kStatus_Success;
}

static status_t qspiflash_read(uint32_t addr, void *data, uint32_t size)
{
    status_t status;

    if (!isFlashInited)
    {
        status = qspiflash_init();

        if(kStatus_Success != status)
        {
            return status;
        }
    }

    /* Reset to memory command mode */
    SPIFI_ResetCommand(BOARD_SPIFI_QSPI);

    SPIFI_SetMemoryCommand(BOARD_SPIFI_QSPI, &command[READ]);
    
    memcpy(data, (void*)addr, size);

    return kStatus_Success;
}

static status_t qspiflash_write(uint32_t addr, const void *data, uint32_t size)
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
        status = qspiflash_init();

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

        program_page(phyAddrAlign, intBuffer);

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
            program_page(phy_address, intBuffer);
        }
        else
        {
            program_page(phy_address, buffer);
        }

        phy_address += FLASH_PAGE_SIZE_BYTES;
        sizeLeft -= FLASH_PAGE_SIZE_BYTES;
        buffer += FLASH_PAGE_SIZE_BYTES;
    }

    if (sizeLeft)
    {
        memcpy(intBuffer, buffer, sizeLeft);

        memcpy(intBuffer + sizeLeft, (void*)(phy_address + FLASH_BASE + sizeLeft), FLASH_PAGE_SIZE_BYTES - sizeLeft);

        program_page(phy_address, intBuffer);
    }

    return kStatus_Success;
}

static status_t qspiflash_erase(uint32_t addr, uint32_t size)
{
    status_t status;
    uint32_t phy_address = addr - FLASH_BASE;
    uint32_t count = 0;

    size = ((size - 1 ) / FLASH_SECTOR_SIZE_BYTES) + 1;

    if (!isFlashInited)
    {
        status = qspiflash_init();

        if(kStatus_Success != status)
        {
            return status;
        }
    }
    while(count < size)
    {
        /* Reset to memory command mode */
        SPIFI_ResetCommand(BOARD_SPIFI_QSPI);
        /* Write enable */
        SPIFI_SetCommand(BOARD_SPIFI_QSPI, &command[WRITE_ENABLE]);
        /* Set address */
        SPIFI_SetCommandAddress(BOARD_SPIFI_QSPI, phy_address + count * FLASH_SECTOR_SIZE_BYTES);
        /* Erase sector */
        SPIFI_SetCommand(BOARD_SPIFI_QSPI, &command[ERASE_SECTOR]);
        /* Check if finished */
        check_if_finish();
        count++;
    }
   
    return kStatus_Success;
}

static flash_device_t flash_device[HAL_FLASH_MAX] =
{
    {
        .start_address = FLASH_BASE,
        .disk_size = FLASH_SIZE_KBYTES*1024,
        .page_size = FLASH_PAGE_SIZE_BYTES,
        .sector_size = FLASH_SECTOR_SIZE_BYTES,
        qspiflash_erase,
        qspiflash_write,
        qspiflash_read,
    },
    { 0, 0, 0, 0, NULL, NULL, NULL },
    { 0, 0, 0, 0, NULL, NULL, NULL }
};


hal_logic_partition_t hal_logic_partition[HAL_PARTITION_MAX] =
{
    /*
     * First 512K reserved for mini boot loader. But only set the second
     * sector here, AOS should not touch the first sector.
     */
    [HAL_PARTITION_BOOTLOADER].partition_owner            = HAL_FLASH_SPI,
    [HAL_PARTITION_BOOTLOADER].partition_description      = "Mini Boot Loader",
    [HAL_PARTITION_BOOTLOADER].partition_start_addr       = 0,
    [HAL_PARTITION_BOOTLOADER].partition_length           = 0,
    [HAL_PARTITION_BOOTLOADER].partition_options          = PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS,

    [HAL_PARTITION_APPLICATION].partition_owner            = HAL_FLASH_SPI,
    [HAL_PARTITION_APPLICATION].partition_description      = "Application",
    [HAL_PARTITION_APPLICATION].partition_start_addr       = 0x10000000,
    [HAL_PARTITION_APPLICATION].partition_length           = 0x00800000,    // 8M bytes
    [HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,

    [HAL_PARTITION_PARAMETER_1].partition_owner            = HAL_FLASH_SPI,
    [HAL_PARTITION_PARAMETER_1].partition_description      = "PARAMETER1",
    [HAL_PARTITION_PARAMETER_1].partition_start_addr       = 0x10880000,
    [HAL_PARTITION_PARAMETER_1].partition_length           = 0x00100000, // 1M bytes
    [HAL_PARTITION_PARAMETER_1].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,

    [HAL_PARTITION_PARAMETER_2].partition_owner            = HAL_FLASH_SPI,
    [HAL_PARTITION_PARAMETER_2].partition_description      = "PARAMETER2",
    [HAL_PARTITION_PARAMETER_2].partition_start_addr       = 0x10980000,
    [HAL_PARTITION_PARAMETER_2].partition_length           = 0x00100000, // 1M bytes
    [HAL_PARTITION_PARAMETER_2].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,

    [HAL_PARTITION_OTA_TEMP].partition_owner               = HAL_FLASH_SPI,
    [HAL_PARTITION_OTA_TEMP].partition_description         = "OTA Storage",
    [HAL_PARTITION_OTA_TEMP].partition_start_addr          = 0x10A80000,
    [HAL_PARTITION_OTA_TEMP].partition_length              = 0x00800000, // 8M bytes
    [HAL_PARTITION_OTA_TEMP].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,

    [HAL_PARTITION_PARAMETER_3].partition_owner            = HAL_FLASH_SPI,
    [HAL_PARTITION_PARAMETER_3].partition_description      = "PARAMETER3",
    [HAL_PARTITION_PARAMETER_3].partition_start_addr       = 0x11280000,
    [HAL_PARTITION_PARAMETER_3].partition_length           = 0x00100000, // 1M bytes
    [HAL_PARTITION_PARAMETER_3].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,

    [HAL_PARTITION_PARAMETER_4].partition_owner            = HAL_FLASH_SPI,
    [HAL_PARTITION_PARAMETER_4].partition_description      = "PARAMETER4",
    [HAL_PARTITION_PARAMETER_4].partition_start_addr       = 0x11380000,
    [HAL_PARTITION_PARAMETER_4].partition_length           = 0x00100000, // 1M bytes
    [HAL_PARTITION_PARAMETER_4].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
};


/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success， otherwise is error
 */
int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *p_logic_partition = NULL;

    if (in_partition >= HAL_PARTITION_MAX || partition == NULL) {
        return -1;
    }

    p_logic_partition = &hal_logic_partition[in_partition];
    memcpy(partition, p_logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    hal_logic_partition_t  logic_partition;
    hal_logic_partition_t *p_logic_partition;
    uint32_t start_addr_phy = 0;
    flash_device_t *device = NULL;
    int32_t ret = 0;

    p_logic_partition = &logic_partition;
    memset(p_logic_partition, 0, sizeof(hal_logic_partition_t));
    ret = hal_flash_info_get(in_partition, p_logic_partition);
    if ((ret != 0) || (p_logic_partition->partition_owner == HAL_FLASH_NONE))
    {
        return -EIO;
    }

    device = &flash_device[p_logic_partition->partition_owner];

    start_addr_phy = off_set + device->start_address;
    if (start_addr_phy + size > device->disk_size)
    {
        return -EIO;
    }

    ret = device->erase(start_addr_phy, size);
    
    return ret;
}

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
    hal_logic_partition_t  logic_partition;
    hal_logic_partition_t *p_logic_partition;
    uint32_t start_addr_phy = 0;
    flash_device_t *device = NULL;
    int32_t ret = 0;

    p_logic_partition = &logic_partition;
    memset(p_logic_partition, 0, sizeof(hal_logic_partition_t));
    ret = hal_flash_info_get(in_partition, p_logic_partition);
    if ((ret != 0) || (p_logic_partition->partition_owner == HAL_FLASH_NONE))
    {
        return -EIO;
    }

    device = &flash_device[p_logic_partition->partition_owner];

    start_addr_phy = *off_set + device->start_address;
    if ((uint32_t)start_addr_phy + in_buf_len > device->disk_size)
    {
        return -EIO;
    }

    ret = device->write(start_addr_phy, in_buf, in_buf_len);
    *off_set += in_buf_len;
    
    return ret;
}

/**
 * Write data to an area on a flash logical partition with erase first
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    hal_logic_partition_t  logic_partition;
    hal_logic_partition_t *p_logic_partition = NULL;
    uint32_t start_addr_phy = 0;
    flash_device_t *device = NULL;
    int32_t ret = 0;

    p_logic_partition = &logic_partition;
    memset(p_logic_partition, 0, sizeof(hal_logic_partition_t));
    ret = hal_flash_info_get(in_partition, p_logic_partition);
    if ((ret != 0) || (p_logic_partition->partition_owner == HAL_FLASH_NONE))
    {
        return -EIO;
    }

    device = &flash_device[p_logic_partition->partition_owner];

    start_addr_phy = *off_set + device->start_address + p_logic_partition->partition_start_addr;
    if (start_addr_phy + in_buf_len > device->disk_size)
    {
        return -EIO;
    }

    ret = device->erase(start_addr_phy, in_buf_len);
    if (ret < 0)
    {
        return ret;
    }

    ret = device->write(start_addr_phy, in_buf, in_buf_len);

    *off_set += in_buf_len;

    return ret;
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    hal_logic_partition_t  logic_partition;
    hal_logic_partition_t *p_logic_partition = NULL;
    uint32_t start_addr_phy = 0;
    flash_device_t *device = NULL;
    int32_t ret = 0;

    p_logic_partition = &logic_partition;
    memset(p_logic_partition, 0, sizeof(hal_logic_partition_t));
    ret = hal_flash_info_get(in_partition, p_logic_partition);
    if ((ret != 0) || (p_logic_partition->partition_owner == HAL_FLASH_NONE))
    {
        return -EIO;
    }

    device = &flash_device[p_logic_partition->partition_owner];

    start_addr_phy = *off_set + device->start_address;
    if (start_addr_phy + in_buf_len > device->disk_size)
    {
        return -EIO;
    }

    ret = device->read(start_addr_phy, out_buf, in_buf_len);
		
    *off_set += in_buf_len;

    return ret;
}

/**
 * Set security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of enabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

/**
 * Disable security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of disabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


