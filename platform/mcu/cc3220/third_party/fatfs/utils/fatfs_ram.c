/*
 * Copyright (c) 2012-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * ======== fatfs_ram.c ========
 * Simple RAM disk layer for FatFs.
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <diskio.h>

#define SECTORSIZE 512

static unsigned char * diskMem = NULL;
static unsigned numSectors = 0;

void ramdisk_init(unsigned char * data, int numBytes)
{
    diskMem = data;
    numSectors = numBytes / SECTORSIZE;
}

DSTATUS disk_initialize(BYTE drive)
{
    assert(drive == 0);

    return (0);
}

DSTATUS disk_status(BYTE drive)
{
    assert(drive == 0);

    return (0);
}

DRESULT disk_read(BYTE drive, BYTE * buf, DWORD start, BYTE num)
{
    assert(drive == 0);

    memcpy(buf, diskMem + start * SECTORSIZE, num * SECTORSIZE);

    return (RES_OK);
}

#if     _READONLY == 0
DRESULT disk_write(BYTE drive, const BYTE * buf, DWORD start, BYTE num)
{
    assert(drive == 0);

    memcpy(diskMem + start * SECTORSIZE, buf, num * SECTORSIZE);

    return (RES_OK);
}
#endif

DRESULT disk_ioctl(BYTE drive, BYTE cmd, void * buf)
{
    DRESULT result;

    assert(drive == 0);

    switch (cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE:
        case CTRL_ERASE_SECTOR:
            result = RES_PARERR;
            break;

        case GET_SECTOR_SIZE:
            *(WORD *)buf = SECTORSIZE;
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buf = numSectors;
            result = RES_OK;
            break;

        default:
            result = RES_ERROR;
            break;
    }

    return (result);
}

DWORD get_fattime()
{
    return (0x23556622);
}
