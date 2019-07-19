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
#include <k_api.h>
#include "aos/hal/flash.h"
#include "fsl_clock.h"
#include "fsl_flash.h"

static flash_config_t s_flashConfig = {0};

hal_logic_partition_t hal_logic_partition[HAL_PARTITION_MAX] = 
{
    { HAL_FLASH_NONE, NULL, 0, 0, PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    { HAL_FLASH_EMBEDDED, "Application", 0x410, 0xFBF0, PAR_OPT_WRITE_EN|PAR_OPT_READ_EN},
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
    hal_logic_partition_t *p_logic_partition;
    /* Get the config information first for other flash operations */
    memset(&s_flashConfig, 0, sizeof(flash_config_t));
    FLASH_Init(&s_flashConfig);
    p_logic_partition = &hal_logic_partition[in_partition];
    memcpy(partition, p_logic_partition, sizeof(hal_logic_partition));

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
 * @return  0 : On success, -5 : If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    status_t status = kStatus_FLASH_Success;
    hal_logic_partition_t logic;
    hal_logic_partition_t *p_logic = NULL;

    p_logic = &logic;
    memset(p_logic, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(in_partition, p_logic);
    uint32_t address = p_logic->partition_start_addr + off_set;

    status = FLASH_Erase(&s_flashConfig, address, size, 0xFFFFFFFFU);
    if (status == kStatus_FLASH_Success)
    {
        return 0;
    }
    else
    {
        return -5;
    }
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
 * @return  0 : On success, -5 : If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    status_t status = kStatus_FLASH_Success;
    uint32_t address = 0;

    hal_logic_partition_t logic;
    hal_logic_partition_t *p_logic = NULL;

    p_logic = &logic;
    memset(p_logic, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(in_partition, p_logic);
    address = p_logic->partition_start_addr + *off_set;

    status = FLASH_Program(&s_flashConfig, address, (uint32_t *)in_buf, in_buf_len);

    if (status == kStatus_FLASH_Success)
    {
        return 0;
    }
    else
    {
        return -5;
    }
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
 * @return  0 : On success, -5 : If an error occurred with any step
 */
int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    hal_flash_erase(in_partition, *off_set, in_buf_len);
    hal_flash_write(in_partition, off_set, in_buf, in_buf_len);

    return 0;
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
 * @return  0 : On success, -5 : If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t in_buf_len)
{
    uint32_t address = 0;

    hal_logic_partition_t logic;
    hal_logic_partition_t *p_logic = NULL;

    p_logic = &logic;
    memset(p_logic, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(in_partition, p_logic);
    address = p_logic->partition_start_addr + *off_set;

    memcpy(out_buf, (void *)address, in_buf_len);
    
    return 0;
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
 * @return  0 : On success, -5 : If an error occurred with any step
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
 * @return  0 : On success, -5 : If an error occurred with any step
 */
int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}
