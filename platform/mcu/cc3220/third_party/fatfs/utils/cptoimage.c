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
 * ======== cptoimage.c ========
 *
 * copy a file from the local HDD into an existing RAM disk image.
 *
 * Usage:
 *
 *     cptoimage [RAM-disk-filename] [source] [destination]
 *
 *     RAM-disk-filename: name of an existing RAM disk image file (usually
 *                        created with 'mkimage' utility).
 *
 *     source: name of the source file on the local HDD that is to be copied
 *             into the RAM disk image.
 *
 *     destination: name to write the source file to on the RAM disk image.  If
 *                  the file already exists on the RAM disk, it will be
 *                  overwritten.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <ff.h>
#include <fsutils.h>

extern void ramdisk_init(void *, int);

/*
 *  ======== main ========
 */
int main(int argc, char *argv[])
{
    unsigned char *diskBuf;
    FRESULT result;
    char *ramImageName;
    unsigned int size = 0;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s ram-file-name [source file]", argv[0]);
        fprintf(stderr, " [destination file]\n");
        exit(1);
    }

    ramImageName = argv[1];

    /* create and mount RAM disk from existing image file */
    if ((diskBuf = loadRamImage(ramImageName, &size)) == NULL) {
        fprintf(stderr, "ERROR: could not load RAM disk image \'%s\'.\n",
                ramImageName);
        exit(1);
    }

    /* copy the local HDD file into the RAM disk */
    if (copyFile(argv[2], argv[3]) < 0) {
        fprintf(stderr, "ERROR: could not copy local file \'%s\'", argv[2]);
        fprintf(stderr, " into RAM disk image file \'%s\'.\n", ramImageName);
        exit(1);
    }


    /* un-register the RAM disk drive */
    if ((result = f_mount(0, NULL)) != FR_OK) {
        fprintf(stderr, "ERROR: un-mounting drive failed: %d\n", result);
        exit(1);
    }

    /* save the updated RAM disk image */
    if (saveImage(diskBuf, size, ramImageName) < 0) {
        fprintf(stderr, "ERROR: could not save RAM disk image to file %s\n",
                ramImageName);
        exit(1);
    }

    return 0;
}
