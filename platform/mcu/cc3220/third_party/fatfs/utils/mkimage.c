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
 * ======== mkimage.c ========
 *
 * Utility to create a RAM disk image using the FatFs file system.  The disk
 * image is saved into a binary file that is stored onto the local hard drive.
 *
 * Once the image has been created, it can be manipulated using the other FatFs
 * utilities that are available (e.g. adding files or directories to the image).
 *
 * Usage:
 *
 *     mkimage [filename] [size]
 *
 *     filename: name of the binary file to store the RAM disk image.  This file
 *               will be stored on the local hard drive.  If the file already
 *               exists, it will be overwritten.
 *
 *     size:     size of the RAM disk to create.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ff.h>
#include <fsutils.h>

extern void ramdisk_init(void *, int);

/*
 *  ======== main ========
 */
int main(int argc, char *argv[])
{
    unsigned int size = 0;
    unsigned char *diskBuf;
    FRESULT result;
    char *fileName;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s [FILENAME] [SIZE]\n", argv[0]);
        fprintf(stderr, "\tCreate RAM disk image of size [SIZE] and output to");
        fprintf(stderr, " binary file [FILENAME]\n");
        exit(1);
    }

    fileName = argv[1];

    if ((size = strtoul(argv[2], NULL, 0)) <= 0) {
        fprintf(stderr, "Error: illegal value (%d) specified for [SIZE]");
        fprintf(stderr, " argument. [SIZE] must be greater than zero.\n");
        exit(1);
    }

    /* create the RAM disk image */
    if ((diskBuf = createImage(&size)) == NULL) {
        fprintf(stderr, "ERROR: out of memory, calloc() returned NULL.\n");
        exit(1);
    }

    /* un-register the RAM disk drive */
    if ((result = f_mount(0, NULL)) != FR_OK) {
        fprintf(stderr, "ERROR: un-mounting drive failed: %d\n", result);
        exit(1);
    }

    if (saveImage(diskBuf, size, fileName) < 0) {
        fprintf(stderr, "ERROR: could not save RAM disk image to file %s\n",
                fileName);
        exit(1);
    }

    return 0;
}
