/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 * ======== ffcio.c ========
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <file.h>

#include <ff.h>
#include <ffcio.h>

static FIL *filTable[_NSTREAM];

static int init = 0;

/*
 *  ======== ffcio_open ========
 */
int ffcio_open(const char *path, unsigned flags, int llv_fd)
{
    int         dev_fd;
    BYTE        fflags;

    (void)llv_fd;

    if (!init) {
        for (dev_fd = 0; dev_fd < _NSTREAM; dev_fd++) {
            filTable[dev_fd] = NULL;
        }
        init = 1;
    }

    for (dev_fd = 0; dev_fd < _NSTREAM && filTable[dev_fd] != NULL; dev_fd++) {
    }

    if (dev_fd == _NSTREAM) {
        /* no available file handles */
        return (-1);
    }


    filTable[dev_fd] = malloc(sizeof(FIL));

    if (filTable[dev_fd] == NULL) {
        /* allocation failed */
        return (-1);
    }

    switch (flags & 0x3) {
        case O_RDONLY:
            fflags = FA_READ;
            break;

        case O_WRONLY:
            fflags = FA_WRITE;
            break;

        case O_RDWR:
            fflags = FA_READ | FA_WRITE;
            break;

        default:
            return (-1);
    }

    if (flags & O_TRUNC) {
        fflags |= FA_CREATE_ALWAYS;
    }

    if (flags & O_CREAT) {
        fflags |= FA_OPEN_ALWAYS;
    }

    if (flags & O_APPEND) {
            ;
    }

    if (f_open(filTable[dev_fd], path, fflags) != FR_OK) {
        free(filTable[dev_fd]);

        filTable[dev_fd] = NULL;

        dev_fd = -1;
    }

    return (dev_fd);
}

/*
 *  ======== ffcio_close ========
 */
int ffcio_close(int dev_fd)
{
    f_close(filTable[dev_fd]);

    free(filTable[dev_fd]);

    filTable[dev_fd] = NULL;

    return (0);
}

/*
 *  ======== ffcio_read ========
 */
int ffcio_read(int dev_fd, char *buf, unsigned count)
{
    FRESULT result;
    unsigned int actual;

    result = f_read(filTable[dev_fd], buf, count, &actual);

    if (result == FR_OK) {
        return ((int)actual);
    }
    else {
        return (-1);
    }
}

/*
 *  ======== ffcio_write ========
 */
int ffcio_write(int dev_fd, const char *buf, unsigned count)
{
    FRESULT result;
    unsigned int actual;

    result = f_write(filTable[dev_fd], buf, count, &actual);

    if (result == FR_OK) {
        return ((int)actual);
    }
    else {
        return (-1);
    }
}

/*
 *  ======== ffcio_lseek ========
 */
fpos_t ffcio_lseek(int dev_fd, fpos_t offset, int origin)
{
    FRESULT result;

    /* HACK: FatFS has no "tell" functions, so peek inside FIL object */
    if (origin == SEEK_CUR) {
        offset += filTable[dev_fd]->fptr;
    }

    if (origin == SEEK_END) {
        offset += filTable[dev_fd]->fsize;
    }

    result = f_lseek(filTable[dev_fd], offset);

    if (result != FR_OK) {
        return (-1);
    }
    else {
        return (offset);
    }
}

/*
 *  ======== ffcio_unlink ========
 */
int ffcio_unlink(const char *path)
{
    FRESULT result;

    result = f_unlink(path);

    if (result != FR_OK) {
        return (-1);
    }
    else {
        return (0);
    }
}

/*
 *  ======== ffcio_rename ========
 */
int ffcio_rename(const char *old_name, const char *new_name)
{
    FRESULT result;

    result = f_rename(old_name, new_name);

    if (result != FR_OK) {
        return (-1);
    }
    else {
        return (0);
    }
}

/*
 *  ======== mkdir ========
 */
int mkdir(const char * path, unsigned int mode)
{
    (void)mode;

    if (f_mkdir(path) == FR_OK) {
        return (0);
    }
    else {
        return (-1);
    }
}

/*
 *  ======== rmdir ========
 */
int rmdir(const char * path)
{
    if (f_unlink(path) == FR_OK) {
        return (0);
    }
    else {
        return (-1);
    }
}
