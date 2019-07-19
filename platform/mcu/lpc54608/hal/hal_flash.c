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
#include "fsl_flashiap.h"


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


//static uint8_t page_buffer[FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES];

static int32_t embedded_flash_erase(uint32_t phy_address, uint32_t size)
{
    int32_t ret = 0;
    uint32_t system_clock = 0;
    status_t status = 0;

    uint32_t start_sector = 0;
    uint32_t end_sector = 0;
    uint32_t sectors_to_erase = 0;
    uint32_t start_page = 0;
    uint32_t end_page = 0;
    uint32_t pages_to_erase = 0;

    uint32_t write_step1_size = 0;
    uint32_t write_step2_size = 0;
    uint32_t residue = size;

    /* FLASH Controller Clock should be enabled firstly */
    CLOCK_EnableClock(kCLOCK_Flash);

    system_clock = CLOCK_GetCoreSysClkFreq();
    /*  Step 1, Erase flash until sector boundary or end */
    if (phy_address % FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES != 0) // Not at sector boundary
    {
        write_step1_size = FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES - 
                           phy_address % FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES;
        if (size < write_step1_size)
        {
            write_step1_size = size;
        }

        if((phy_address % FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES) != 0)
        {
            ret = -EIO; //Not supported yet
            goto exit;
        }

        if((write_step1_size % FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES) != 0)
        {
            ret = -EIO;
            goto exit;
        }

        start_sector = phy_address/FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES;
        start_page = phy_address / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;

        __disable_irq();
        status = FLASHIAP_PrepareSectorForWrite(start_sector, start_sector);
        if (status != kStatus_FLASHIAP_Success) {
            __enable_irq();
            ret = -EIO; //Check status flag for detail error
            goto exit;
        }
        pages_to_erase = write_step1_size / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;
        end_page = start_page + pages_to_erase - 1;

        status = FLASHIAP_ErasePage(start_page, end_page, system_clock);
        if (status != kStatus_FLASHIAP_Success) {
            __enable_irq();
            ret = -EIO; //Check status flag for detail error
            goto exit;
        }
        __enable_irq();
    }

    residue = size - write_step1_size;

    sectors_to_erase = residue / FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES;

    /* Step 2, sector erasing */
    if(sectors_to_erase > 0) { /* More than one sector to erase */
        start_sector = (phy_address + write_step1_size) / FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES;
        end_sector = start_sector + sectors_to_erase - 1;
        __disable_irq();
        status = FLASHIAP_PrepareSectorForWrite(start_sector, end_sector);
        if (status != kStatus_FLASHIAP_Success) {
            __enable_irq();
            ret = -EIO; //Check status flag for detailed error
            goto exit;
        }
        status = FLASHIAP_EraseSector(start_sector, end_sector, system_clock);
        if (status != kStatus_FLASHIAP_Success) {
            __enable_irq();
            ret = -EIO; //Check status flag for detailed error
            goto exit;
        }
        __enable_irq();
        write_step2_size = FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES * sectors_to_erase;
    }

    residue = size - write_step1_size - write_step2_size; 
    /* Step 3, erasing residues */
    if (residue > 0)
    {
        if (residue % FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES != 0)
        {
            ret = -EIO; //Check status flag for detailed error
            goto exit;
        }
        start_sector = (phy_address + write_step1_size + write_step2_size) / FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES;
        start_page = (phy_address + write_step1_size + write_step2_size) / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;

        __disable_irq();
        status = FLASHIAP_PrepareSectorForWrite(start_sector, start_sector);
        if (status != kStatus_FLASHIAP_Success)
        {
            __enable_irq();
            ret = -EIO;
            goto exit;
        }
        pages_to_erase = residue / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;
        end_page = start_page + pages_to_erase - 1;

        status = FLASHIAP_ErasePage(start_page, end_page, system_clock);
        if (status != kStatus_FLASHIAP_Success) {
            __enable_irq();
            ret = -EIO; //Check status flag for detail error
            goto exit;
        }
        __enable_irq();
    }
exit:
    CLOCK_DisableClock(kCLOCK_Flash);
    return ret;
}

static int32_t embedded_flash_write(uint32_t phy_address, const void *data, uint32_t size)
{
    int32_t ret = 0;
    uint32_t system_clock = 0;
    status_t status = 0;
    
    uint32_t start_sector = 0;
    uint32_t end_sector = 0;

    /* FLASH Program size 256 | 512 | 1024 | 4096 */
    uint16_t write_size_option[4] = {256, 512, 1024, 4096};
    uint8_t option = 0;
    uint32_t offset = 0;


    /* FLASH Controller Clock should be enabled firstly */
    CLOCK_EnableClock(kCLOCK_Flash);
    system_clock = CLOCK_GetCoreSysClkFreq();
    if (phy_address % FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES != 0)
    {
        ret = -EIO; // Not Support yet
        goto exit;
    }

    if (size % FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES != 0)
    {
        ret = -EIO;
        goto exit;
    }

    while (offset < size) {  
        start_sector = (phy_address + offset)/FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES;
        option = sizeof(write_size_option)/sizeof(write_size_option[0]) - 1;            
        while(option >= 0)
        {
            if ((option == 0) || ((size - offset) / write_size_option[option] != 0 && 
               (phy_address + offset + write_size_option[option]) <= (start_sector + 1) * FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES))
                break;
            option--;
        }

        end_sector = start_sector;
        __disable_irq();
        status = FLASHIAP_PrepareSectorForWrite(start_sector, end_sector);
        if (status != kStatus_FLASHIAP_Success) {
            __enable_irq();
            ret = -EIO; //Check status flag for detail error
            goto exit;
        }

        status = FLASHIAP_CopyRamToFlash(phy_address + offset, ((uint32_t *)data) + (offset >> 2), write_size_option[option], system_clock);
        if (status != kStatus_FLASHIAP_Success) {
            __enable_irq();
            ret = -EIO; //Check status flag for detail error
            goto exit;
        }
        __enable_irq();
        offset += write_size_option[option];
    }

exit:
    CLOCK_DisableClock(kCLOCK_Flash);
    return ret;
}

static int32_t embedded_flash_read(uint32_t phy_address, void *data, uint32_t size)
{
    /* Use Address to access embeded FLASH, it is on the memory map */
    /* Do NOT use this method */
    return -EIO;
}

static flash_device_t flash_device[HAL_FLASH_MAX] =
{
    {
        .start_address = 0,
        .disk_size = 0x80000,
        .page_size = 0x100,
        .sector_size = 0x8000,
        embedded_flash_erase,
        embedded_flash_write,
        embedded_flash_read,
    },
    { 0, 0, 0, 0, NULL, NULL, NULL },
    { 0, 0, 0, 0, NULL, NULL, NULL }
};


hal_logic_partition_t hal_logic_partition[HAL_PARTITION_MAX] = 
{
    { HAL_FLASH_EMBEDDED, "Bootloader", 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_EMBEDDED, "Application", 0x40000, 0x40000, PAR_OPT_WRITE_EN|PAR_OPT_READ_EN},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
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
    int32_t ret = -1;

    hal_logic_partition_t *p_logic_partition = NULL;

    if (in_partition >= HAL_PARTITION_MAX || partition == NULL) {
        return -1;
    }

    switch (in_partition)
    {
        case HAL_PARTITION_APPLICATION:
        p_logic_partition = &hal_logic_partition[in_partition];
        memcpy(partition, p_logic_partition, sizeof(hal_logic_partition_t));
        ret = 0;
        break;
        default:
        break;
    }
    return ret;
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

