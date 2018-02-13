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
 * ======== fsutils.c ========
 *
 * This file contains APIs that are commonly used by the utilities defined in
 * this package.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ff.h>
#include <fsutils.h>

#define CLUSTER_SIZE 512
#define READ_BLOCK_SIZE 64

static FATFS fs;

extern void ramdisk_init(void *, int);

/*
 * Microsoft compiler uses _fileno() instead of fileno().
 */
#ifdef _MSC_VER
#define FILENO  _fileno
#else
#define FILENO fileno
#endif


/*
 *  ======== createImage ========
 *
 *  Create a RAM disk image of size [size].
 *
 *  This function performs the following to create the image:
 *
 *      a. allocate storage of size [size] for the RAM disk
 *      b. call ramdisk_init to intialize the disk
 *      c. call f_mount() to mount the file system
 *      d. call f_mkfs() to create the file system on the RAM disk drive
 *
 *  returns an dynamically allocated block of memory for use as a RAM disk on
 *  success, NULL on failure.
 */
unsigned char *createImage(unsigned int *size)
{
    unsigned char *data;
    FRESULT result;

    /* allocate storage for the RAM disk image */
    data = calloc(*size, 1);

    /* initialize, mount, and create the RAM disk */
    if (*size && data) {

        ramdisk_init(data, *size);

        if ((result = f_mount(0, &fs)) != FR_OK) {
            fprintf(stderr, "ERROR: could not mount the RAM disk drive.");
            fprintf(stderr, "  f_mount() returned %d\n", result);
            return NULL;
        }

        if ((result = f_mkfs(0, 0, CLUSTER_SIZE)) != FR_OK) {
            fprintf(stderr, "ERROR: could not create the RAM disk file");
            fprintf(stderr, " system.  f_mkfs() returned %d\n", result);
            return NULL;
        }
    }

    return data;
}

/*
 * ======== catTextFile ========
 *
 * cat the contents of a text file that exists on a RAM disk image.
 *
 * returns FR_OK on success, on failure it returns either the result returned by
 * the FatFS API which failed, or -1 if dynamic memory allocation failed.
 */
FRESULT catTextFile(char *filename)
{
    unsigned char data[READ_BLOCK_SIZE];
    unsigned int  actual;
    FIL           fil;
    FRESULT       result;

    actual = 0;

    /* open the ramdisk file for reading */
    if ((result = f_open(&fil, filename, FA_READ)) != FR_OK) {
        printf("ERROR: f_open %s returned: %d\n", filename, result);
        return result;
    }

    do {

        /* read contents of the file into a buffer */
        if ((result = f_read(&fil, data, sizeof(data), &actual)) != FR_OK) {
            printf("ERROR FatFs fxn 'f_read' returned FRESULT.%d\n", result);

            f_close(&fil);
            return result;
        }

        /* output file contents */
        fwrite(data, 1, actual, stdout);
    } while (actual > 0);


    /* clean up */
    if ((result = f_close(&fil)) != FR_OK) {
        fprintf(stderr, "ERROR: f_close() returned %d.\n", result);
        fprintf(stderr, "ERROR: could not close RAM disk file %s.\n",
                filename);
        return result;
    }

    return result;
}

/*
 *  ======== copyFile ========
 *
 *  opens a file from the local HDD ('src') and copies its contents into a
 *  file on the RAM disk ('dst').
 *
 *  returns 0 on success, -1 on failure.
 */
int copyFile(char *src, char *dst)
{
    FILE *fp;
    FIL ffFile;
    char buf[1024];
    FRESULT result = FR_OK;
    UINT wrote;
    size_t read;

    /* open the source file which exists on the local HDD */
    if ((fp = fopen(src, "rb")) == NULL) {
        fprintf(stderr, "ERROR: could not open file %s on local HDD.\n",
                src);
        return -1;
    }

    if ((result = f_open(&ffFile, dst, FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK) {
        printf("ERROR: copyFile(): f_open returned: %d\n", result);
        return -1;
    }

    while ((read = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if ((result = f_write(&ffFile, buf, read, &wrote)) != FR_OK) {
            fprintf(stderr, "ERROR: f_write() returned %d.\n", result);
            fprintf(stderr, "ERROR: attempted to write %d bytes to file %s, only");
            fprintf(stderr, " %d bytes actually written.\n", read, dst, wrote);
            return -1;
        }
    }

    if ((result = fclose(fp)) == EOF) {
        fprintf(stderr, "ERROR: could not close HDD file %s.\n", src);
        return -1;
    }

    if ((result = f_close(&ffFile)) != FR_OK) {
        fprintf(stderr, "ERROR: f_close() returned %d.\n", result);
        fprintf(stderr, "ERROR: could not close RAM disk file %s.\n", dst);
        return -1;
    }

    return 0;

}

/*
 * ======== imageToCfile ========
 *
 * convert RAM file system image into a C file which contains an array with
 * the raw data of the image.
 *
 * returns 0 on success, -1 on error
 */
int imageToCfile(unsigned char *image, unsigned int imagesize,
        char *filename, char *symbolname)
{
    FILE         *out;
    unsigned int *wordData = (unsigned int *)image;
    int           result   = 0;

    if ((out = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "ERROR: could not open file \'%s\'\n", filename);
        return -1;
    }

    /* generate the C array with symbolname and the RAM disk image data */
    fprintf(out, "#include <xdc/std.h>\n");
    fprintf(out, "UInt32 %s[] = {\n", symbolname);
    for (; imagesize > 0; imagesize -= sizeof(unsigned int)) {
        fprintf(out, "0x%x,\n", *wordData++);
    }
    fprintf(out, "};\n");

    if ((result = fclose(out)) == EOF) {
        fprintf(stderr, "ERROR: could not close file %s.\n");
        return -1;
    }

    return 0;
}

/*
 *  ======== loadRamImage ========
 *
 *  load an existing RAM disk image from a file on the local HDD.
 *
 *  returns an dynamically allocated block of memory containing the existing
 *  RAM disk image data on success, NULL on failure.
 */
unsigned char *loadRamImage(char *name, unsigned int *len)
{
    FILE *fp;
    struct stat statBuf;
    unsigned char *data;
    FRESULT result;
    int numBlocks = 1;

    /* open the RAM disk image that lives on the local HDD */
    if ((fp = fopen(name, "rb")) != NULL) {
        /* call fstat to get the size of the file */
        if (fstat(FILENO(fp), &statBuf) < 0) {
            fprintf(stderr, "ERROR: call to fstat() on %s failed.\n",
                    name);
            return NULL;
        }

        /* allocate a block of memory for the RAM disk */
        *len = statBuf.st_size;
        if ((data = malloc(*len)) == NULL) {
            fprintf(stderr, "ERROR: out of memory, alloc() returned NULL.\n");
            return NULL;
        }

        /* copy the existing RAM disk image into the allocated block */
        if ((result = fread(data, *len, numBlocks, fp)) < numBlocks) {
            fprintf(stderr, "ERROR: attempted to read %d elements of size %d",
                    numBlocks, *len);
            fprintf(stderr, " bytes from file %s, only %d bytes", name, result);
            fprintf(stderr, " actually read.\n");
            return NULL;
        }

        /* close the local HDD file */
        if (fclose(fp)) {
            fprintf(stderr, "ERROR: could not close file %s\n", name);
            return NULL;
        }
    }
    else {
        fprintf(stderr, "ERROR: could not open RAM disk image \'%s\'\n", name);
        return NULL;
    }

    if (*len && data) {
        ramdisk_init(data, *len);

        if ((result = f_mount(0, &fs)) != FR_OK) {
            fprintf(stderr, "ERROR: could not mount the RAM disk drive.");
            fprintf(stderr, "  f_mount() returned %d\n", result);
            return NULL;
        }
    }

    return data;
}

/*
 *  ======== saveImage ========
 *
 *  Save the RAM disk image to the local HDD as binary file [filename].
 *
 *  This function uses the standard C file APIs to write the RAM disk image to
 *  a file on the local HDD.
 *
 *  returns 0 on success, -1 on failure.
 */
int saveImage(unsigned char *data, unsigned int len, char *filename)
{
    FILE *out;

    /* open the file to save the RAM disk image to on the local HDD */
    if ((out = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "ERROR: could not open file %s on local HDD.\n",
                filename);
        return -1;
    }

    if (fwrite(data, 1, len, out) != len) {
        fprintf(stderr, "ERROR: fwrite failed.\n");
        return -1;
    }

    if (fclose(out) != 0) {
        fprintf(stderr, "ERROR: could not close file %s.\n", filename);
        return -1;
    }

    return 0;
}

/*
 * ======== scanFiles ========
 *
 * print out the directories and files of the RAM disk image.
 */
FRESULT scanFiles (char* path)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);

    if (res == FR_OK) {

        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) {
                break;
            }

            if (fno.fname[0] == '.') {
                continue;
            }

#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif

            if (fno.fattrib & AM_DIR) {

                if (path[i - 1] == '/') {
                    sprintf(&path[i], "%s", fn);
                    printf("%s\n", path);
                }
                else {
                    sprintf(&path[i], "/%s", fn);
                    printf("%s\n", path);
                }

                res = scanFiles(path);

                if (res != FR_OK) {
                    break;
                }

                path[i] = 0;

            }
            else {
                printf("%s/%s\n", path, fn);
            }

        } // end for (;;)
    } // end if (res == FR_OK)
    else {
        printf("ERROR: f_opendir() returned %d\n", res);
    }

    return res;
}
