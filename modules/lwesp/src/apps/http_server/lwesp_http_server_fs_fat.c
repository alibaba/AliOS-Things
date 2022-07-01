/**
 * \file            lwesp_http_server_fs_fat.c
 * \brief           FATFS library implementation for HTTP server file system
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "lwesp/apps/lwesp_http_server.h"
#include "lwesp/apps/lwesp_http_server_fs.h"
#include "lwesp/lwesp_mem.h"

#if !__DOXYGEN__

#include "ff.h"                                 /* Include FATFS file system file */

/* File system object handle */
static FATFS fs;

/* File path */
static char fs_path[128];

/**
 * \brief           Open a file of specific path
 * \param[in]       file: File structure to fill if file is successfully open
 * \param[in]       path: File path to open in format "/js/scripts.js" or "/index.html"
 * \return          1 on success, 0 otherwise
 */
uint8_t
http_fs_open(http_fs_file_t* file, const char* path) {
    FIL* fil;

    /* Do we have to mount our file system? */
    if (!*file->rem_open_files) {
        if (f_mount(&fs, "SD:", 1) != FR_OK) {
            return 0;
        }
    }

    /* Format file path in "www" directory of root directory */
    sprintf(fs_path, "SD:www%s", path);

    /* Allocate memory for FATFS file structure */
    fil = lwesp_mem_malloc(sizeof(*fil));
    if (fil == NULL) {
        return 0;
    }

    /*
     * Try to open file in read mode and
     * set required parameters for file length
     */
    if (f_open(fil, fs_path, FA_READ) == FR_OK) {
        file->arg = fil;                        /* Set user file argument to FATFS file structure */
        file->size = f_size(fil);               /* Set file length, most important part */
        return 1;
    }

    lwesp_mem_free(fil);                        /* We failed, free memory */
    return 0;
}

/**
 * \brief           Read a file content
 * \param[in]       file: File handle to read
 * \param[out]      buff: Buffer to read data to. When set to NULL, function should return remaining available data to read
 * \param[in]       btr: Number of bytes to read. Has no meaning when buff = NULL
 * \return          Number of bytes read or number of bytes available to read
 */
uint32_t
http_fs_read(http_fs_file_t* file, void* buff, size_t btr) {
    FIL* fil;
    UINT br;

    fil = file->arg;                            /* Get file argument */
    if (fil == NULL) {                          /* Check if argument is valid */
        return 0;
    }

    /*
     * When buffer is NULL, return available
     * length we can read in the next step
     */
    if (buff == NULL) {
        return f_size(fil) - f_tell(fil);
    }

    /*
     * Read the file and return read length
     */
    br = 0;
    if (f_read(fil, buff, btr, &br) == FR_OK) {
        return br;
    }
    return 0;
}

/**
 * \brief           Close a file handle
 * \param[in]       file: File handle
 * \return          1 on success, 0 otherwise
 */
uint8_t
http_fs_close(http_fs_file_t* file) {
    FIL* fil;

    fil = file->arg;                            /* Get file argument */
    if (fil == NULL) {                          /* Check if argument is valid */
        return 0;
    }

    f_close(fil);                               /* Close file */

    /*
     * At this step, check if we are last opened file
     * and unmount system if necessary
     */
    if (*file->rem_open_files == 1) {
        f_mount(NULL, "SD:", 1);
    }
    lwesp_mem_free(fil);                        /* Free user argument */

    return 1;                                   /* Close was successful */
}

#endif /* !__DOXYGEN__ */
