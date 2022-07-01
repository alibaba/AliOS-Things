/**
 * \file            lwesp_http_server_fs_win32.c
 * \brief           WIN32 file system implementation
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

static char fs_path[256];

/**
 * \brief           Open a file of specific path
 * \param[in]       file: File structure to fill if file is successfully open
 * \param[in]       path: File path to open in format "/js/scripts.js" or "/index.html"
 * \return          `1` on success, `0` otherwise
 */
uint8_t
http_fs_open(http_fs_file_t* file, const char* path) {
    FILE* fil;

    /* Format file path in "www" directory of root directory */
    sprintf(fs_path, "../../../www%s", path);

    /* Try to open file on disk as read and binary mode */
    if (!fopen_s(&fil, fs_path, "rb")) {
        file->arg = fil;
        fseek(fil, 0, SEEK_END);
        file->size = ftell(fil);
        fseek(fil, 0, SEEK_SET);
        return 1;
    }
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
    FILE* fil;
    uint32_t br;

    fil = file->arg;                            /* Get file argument */
    if (fil == NULL) {                          /* Check if argument is valid */
        return 0;
    }

    /*
     * When buffer is NULL, return available
     * length we can read in the next step
     */
    if (buff == NULL) {
        uint32_t s, e;
        s = ftell(fil);                         /* Get current position */
        fseek(fil, 0, SEEK_END);                /* Go to the end */
        e = ftell(fil);                         /* Get end position */
        fseek(fil, s, SEEK_SET);                /* Set back to current position */
        return e - s;                           /* Return difference in positions */
    }

    /* Read the file and return read length */
    br = (uint32_t)fread(buff, 1, btr, fil);
    return br;
}

/**
 * \brief           Close a file handle
 * \param[in]       file: File handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
http_fs_close(http_fs_file_t* file) {
    FILE* fil;

    fil = file->arg;                            /* Get file argument */
    if (fil == NULL) {                          /* Check if argument is valid */
        return 0;
    }

    fclose(fil);                                /* Close file */

    /*
     * At this step, check if we are last opened file
     * and unmount system if necessary
     */
    if (*file->rem_open_files == 1) {

    }
    return 1;                                   /* Close was successful */
}
