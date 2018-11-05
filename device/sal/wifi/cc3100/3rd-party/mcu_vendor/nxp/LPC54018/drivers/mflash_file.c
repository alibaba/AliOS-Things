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

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "mflash_file.h"
#include "mflash_drv.h"

static mflash_file_t *g_file_table = NULL;

#if 0
/* Example of NULL terminated file table */
mflash_file_t g_flash_files[] = {
    { .flash_addr = MFLASH_FILE_BASEADDR, .max_size = MFLASH_FILE_SIZE, .path = "/test/file1" },
    { .flash_addr = MFLASH_FILE_BASEADDR + 1 * MFLASH_FILE_SIZE, .max_size = MFLASH_FILE_SIZE, .path = "/test/file2" },
    { .flash_addr = MFLASH_FILE_BASEADDR + 2 * MFLASH_FILE_SIZE, .max_size = MFLASH_FILE_SIZE, .path = "/test/file3" },
    { 0 }
};
#endif

/* Help to identify mfile without knowing an address */
#define MFLASH_META_MAGIC_NO (0xABECEDA8)

typedef struct {
    uint32_t file_size;
    uint32_t magic_no;
} mfile_meta_t;


BaseType_t mflash_init(mflash_file_t *user_file_table, bool init_drv)
{
    /* Check params */
    if (NULL == user_file_table)
        return pdFALSE;

    /* Check attributes of each file in table */
    for (
        mflash_file_t *tmp_file = user_file_table;
        (0 != tmp_file->flash_addr) && (0 != tmp_file->max_size);
        tmp_file += 1
    )
    {
        /* NOTE: This loop does not check overlapping sectors !!! */
        /* Assume that zero address and zero size belong to unitialized struct. */
        if ((0 == tmp_file->flash_addr) || (0 == tmp_file->max_size))
            return pdFALSE;
        /* Starting file address ('flash_addr') must be sector aligned */
        if (!mflash_drv_is_sector_aligned(tmp_file->flash_addr))
            return pdFALSE;
        /* File max_size must be multiple of sector size */
        if (!mflash_drv_is_sector_aligned(tmp_file->max_size))
            return pdFALSE;
    }
    /* Store reference to user*/
    g_file_table = user_file_table;
    /* Init flash driver */
    if (init_drv)
        mflash_drv_init();
    return pdTRUE;
}

/* Find a file in file_table */
int32_t mflash_find_file(char *path, mflash_file_t *file)
{
    int32_t status = -1;

    /* Check file_table */
    if (NULL == g_file_table)
        return -1;

    for (
        mflash_file_t *tmp_file = g_file_table;
        (0 != tmp_file->flash_addr) && (0 != tmp_file->max_size);
        tmp_file += 1
    )
    {
        /* Found a file by path */
        if (0 == strncmp(path, tmp_file->path, 64))
        {
            status = 0;
            *file = *tmp_file;
            break;
        }
    }
    return status;
}

/* NOTE: Don't try to store constant data that are located in XIP !! */
/* API, write to file of path 'pcFileName' */
BaseType_t mflash_save_file(
    char * pcFileName,
    uint8_t * pucData,
    uint32_t ulDataSize )
{
    mflash_file_t tmp_file = {0};
    mfile_meta_t tmp_meta = {0};

    /* No file was found in file table */
    if (0 != mflash_find_file(pcFileName, &tmp_file))
        return pdFALSE;
    /* Trying to write data over file boundary */
    if (ulDataSize > tmp_file.max_size + sizeof(tmp_meta))
        return pdFALSE;
    /* Write real data after a meta data location */
    if (0 != mflash_drv_write((void*)(tmp_file.flash_addr + sizeof(tmp_meta)), pucData, ulDataSize))
        return pdFALSE;
    /* Set & write meta data before a real data */
    tmp_meta.magic_no = MFLASH_META_MAGIC_NO;
    tmp_meta.file_size = ulDataSize;
    if (0 != mflash_drv_write((void*)tmp_file.flash_addr, (uint8_t*)&tmp_meta, sizeof(tmp_meta)))
        return pdFALSE;
    return pdTRUE;
}

/* API, read file of path 'pcFileName' */
BaseType_t mflash_read_file(
    char * pcFileName,
    uint8_t ** ppucData,
    uint32_t * pulDataSize )
{
    mflash_file_t tmp_file = {0};
    mfile_meta_t *tmp_meta = NULL;
    /* No file was found in file table */
    if (0 != mflash_find_file(pcFileName, &tmp_file))
        return pdFALSE;
    /* Locate meta data and check magic number */
    tmp_meta = (mfile_meta_t*)tmp_file.flash_addr;
    /* To avoid hard-fault better check NULL ptr */
    if (NULL == tmp_meta)
            return pdFALSE;
    /* Magic number does not match, this location is not a valid file */
    if (MFLASH_META_MAGIC_NO != tmp_meta->magic_no)
            return pdFALSE;
    /* Set file size and set data pointer to real_data */
    *pulDataSize = tmp_meta->file_size;
    *ppucData = (uint8_t*)(tmp_file.flash_addr + sizeof(*tmp_meta));
    return pdTRUE;
}

