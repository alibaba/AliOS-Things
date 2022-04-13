/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
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
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fsl_mmc.h"
#include "fsl_mmc_disk.h"
#include "diskio.h"

/* New project wizard guide note. */
#ifndef BOARD_MMC_VCC_SUPPLY
#error Undefined macro. Define BOARD_MMC_VCC_SUPPLY in board.h
#endif

/*******************************************************************************
 * Definitons
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Card descriptor */
static mmc_card_t g_mmc;

/*******************************************************************************
 * Code
 ******************************************************************************/

DRESULT mmc_disk_write(uint8_t physicalDrive, const uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != MMCDISK) {
        return RES_PARERR;
    }

    if (kStatus_Success != MMC_WriteBlocks(&g_mmc, buffer, sector, count)) {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT mmc_disk_read(uint8_t physicalDrive, uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != MMCDISK) {
        return RES_PARERR;
    }

    if (kStatus_Success != MMC_ReadBlocks(&g_mmc, buffer, sector, count)) {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT mmc_disk_ioctl(uint8_t physicalDrive, uint8_t command, void *buffer)
{
    DRESULT res = RES_OK;

    if (physicalDrive != MMCDISK) {
        return RES_PARERR;
    }

    switch (command) {
        case GET_SECTOR_COUNT:
            if (buffer) {
                *(uint32_t *)buffer = g_mmc.userPartitionBlocks;
            } else {
                res = RES_PARERR;
            }

            break;

        case GET_SECTOR_SIZE:
            if (buffer) {
                *(uint32_t *)buffer = g_mmc.block_size;
            } else {
                res = RES_PARERR;
            }

            break;

        case GET_BLOCK_SIZE:
            if (buffer) {
                *(uint32_t *)buffer = g_mmc.eraseGroupBlocks;
            } else {
                res = RES_PARERR;
            }

            break;

        case CTRL_SYNC:
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
            break;
    }

    return res;
}

DSTATUS mmc_disk_status(uint8_t physicalDrive)
{
    if (physicalDrive != MMCDISK) {
        return STA_NOINIT;
    }

    return RES_OK;
}

DSTATUS mmc_disk_initialize(uint8_t physicalDrive)
{
    if (physicalDrive != MMCDISK) {
        return STA_NOINIT;
    }

    /* Save host information. */
    g_mmc.host.base = MMC_HOST_BASEADDR;
    g_mmc.host.source_clock_hz = MMC_HOST_CLK_FREQ;

    /* MMC card VCC supply, only allow 3.3 or 1.8v, depend on your board config.
    * If a power reset circuit is avaliable on you board for mmc, and 1.8v is supported,
    * #define BOARD_USDHC_MMCCARD_POWER_CONTROL_INIT()
    * #define BOARD_USDHC_MMCCARD_POWER_CONTROL(state)
    * in board.h must be implemented.
    * User can remove preset the voltage window and sdmmc will switch VCC automatically. */
    g_mmc.hostVoltageWindowVCC = BOARD_MMC_VCC_SUPPLY;

    if (kStatus_Success != MMC_Init(&g_mmc)) {
        MMC_Deinit(&g_mmc);
        memset(&g_mmc, 0U, sizeof(g_mmc));
        return STA_NOINIT;
    }

    return RES_OK;
}
