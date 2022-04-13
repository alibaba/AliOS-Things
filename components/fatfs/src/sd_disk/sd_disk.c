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

#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>

#include "sd_disk/sd_disk.h"
#include "aos/hal/sd.h"

static sd_dev_t sdmmc;
DSTATUS  SDMMC_initialize()
{
    DSTATUS stat = STA_NOINIT;

    memset(&sdmmc, 0, sizeof(sd_dev_t));
    if (hal_sd_init(&sdmmc) == RES_OK) {
        stat &= ~STA_NOINIT;
    }

    return stat;
}

DSTATUS SDMMC_status()
{
    hal_sd_stat status;
    if (hal_sd_stat_get(&sdmmc, &status) == 0) {
        if (status == SD_STAT_TRANSFER)
            return 0;
    }

    return STA_NOINIT;
}

DRESULT SDMMC_read(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;

    if (hal_sd_blks_read(&sdmmc, buff, sector, count, AOS_WAIT_FOREVER) != 0) {
        res = RES_ERROR;
    }

    return res;
}

DRESULT SDMMC_write(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;

    if (hal_sd_blks_write(&sdmmc, buff, sector, count, AOS_WAIT_FOREVER) != 0) {
        res = RES_ERROR;
    }

    return res;
}

DRESULT SDMMC_ioctl(BYTE cmd, void *buff)
{
    DRESULT       res = RES_ERROR;
    hal_sd_info_t info;

    if (SDMMC_status() & STA_NOINIT)
        return RES_NOTRDY;

    switch (cmd) {
    case CTRL_SYNC:
        res = RES_OK;
        break;

#if FF_USE_MKFS
    case GET_SECTOR_COUNT:
        if (hal_sd_info_get(&sdmmc, &info) == 0) {
            *(DWORD *)buff = info.blk_nums;
            res            = RES_OK;
        }
        break;

    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1;
        res            = RES_OK;
        break;

    case GET_FORMAT_OPTION:
        *(BYTE *)buff = FM_ANY;
        res           = RES_OK;
        break;
#endif

    case GET_SECTOR_SIZE:
        if (hal_sd_info_get(&sdmmc, &info) == 0) {
            *(WORD *)buff = info.blk_size;
            res           = RES_OK;
        }
        break;
    default:
        res = RES_PARERR;
    }

    return res;
}

#endif


