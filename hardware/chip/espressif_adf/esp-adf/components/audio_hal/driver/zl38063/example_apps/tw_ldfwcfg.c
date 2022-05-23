/****************************************************************************
* tw_ldfwcfg.c - To load a *.s3 firmware and/or a *.cr2 into the device
*                and optionally save the loaded  image to flash
*
*
****************************************************************************
* Copyright Microsemi Inc, 2018. All rights reserved. 
* Licensed under the MIT License. See LICENSE.txt in the project 
* root for license information.
*
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>  /* malloc, free, rand */

#include "vproc_common.h"
#include "vprocTwolf_access.h"

/*NOTE: notice that the *.c code are included in the apps-
* This is because the compiler I'm using requires that
* But if your makefile is such that compiler knows where to find these files
* then remove the #include *.c below
*/

#undef SAVE_IMAGE_TO_FLASH /*define this macro to save the firmware from RAM to flash*/
#undef SAVE_CFG_TO_FLASH   /*define this macro to save the cfg from RAM to flash*/
/*quick test*/

uint16 numElements;

dataArr* pCr2Buf;
/* fseekNunlines() -- The firmware file is an ascii text file.
 * the information from fseek will not be useful.
 * this is our own fseek equivalent.
 */
static unsigned long fseekNunlines(FILE* BOOT_FD)
{
    uint32 line_count = 0;
    int c;

    while ( (c = fgetc(BOOT_FD)) != EOF ) {
        if ( c == '\n' )
            line_count++;
    }
    return line_count;
}

/* readCfgFile() use this function to
 * Read the Voice processing cr2 config file into RAM
 * filepath -- pointer to the location where to find the file
 * pCr2Buf -- the actual firmware data array will be pointed to this buffer
 */
static int readCfgFile(char* filepath)
{
    unsigned int reg[2], val[2], len;
    uint8 done = 0;
    uint16 index = 0;
    FILE* BOOT_FD;
    char* s;
    char line[512] = "";

    BOOT_FD = fopen(filepath, "rb");
    if (BOOT_FD != NULL) {
        len = fseekNunlines(BOOT_FD);
        if (len <= 0) {
            printf("Error: file is not of the correct format...\n");
            return -1;
        }
        //printf("fileLength = %u\n", len);
        /*start at the beginning of the file*/
        //fseek(BOOT_FD, 0, SEEK_SET);

        /* allocate memory to contain the reg and val:*/
        pCr2Buf = (dataArr*) malloc(len * sizeof(dataArr));
        if (pCr2Buf == NULL) {
            printf ("not enough memory to allocate %u bytes.. ", len * sizeof(dataArr));
            return -1;
        }

        rewind(BOOT_FD);
        /*read and format the data accordingly*/
        numElements  = 0;
        do {
            s = fgets(line, 512, BOOT_FD);
            if (line[0] == ';') {
                continue;
            } else if (s != NULL) {
                numElements++;
                sscanf(line, "%x %c %x", reg, s, val);
                pCr2Buf[index].reg = reg[0];
                pCr2Buf[index].value = val[0];
                // printf("pCr2Buf[%d].reg pCr2Buf[%d].value  = 0x%04x\t0x%04x\n", index, index, pCr2Buf[index].reg, pCr2Buf[index].value);
                index++;
            } else { done = 1;}

        } while (done == 0);

        fclose(BOOT_FD);
        //printf ("size of pCr2Buf = %u bytes.. \n", len*sizeof(pCr2Buf));
    } else {
        printf("Error: can't open file\n");
    }
    return 0;
}

/*This example host app load the *.s3 firmware to the device RAM. Optionally save it to flash
 * Then start the firmware from the execution address in RAM
 * It then stops the firmware - Load the cr2 file into RAM. Optionally save it to flash
 * Then resstarts the firmware
 */
int main (int argc, char** argv)
{

    VprocStatusType status = VPROC_STATUS_SUCCESS;
    FILE* BOOT_FD;
    char line[256] = "";


    if (argc < 3) {
        printf("Error: argc = %d - missing %d arg(s)... \n", argc, 3 - (argc - 1));
        printf("command Usage:%s firmwarePath ConfigPath\n", argv[0]);
        exit(1);
    }
    printf(":%s %s %s\n", argv[0], argv[1], argv[2]);


    BOOT_FD = fopen(argv[1], "rb");
    if (BOOT_FD == NULL) {
        printf("Error: can't open file %s\n", argv[1]);
        return -1;
    }
    /*global file handle*/
    status = VprocTwolfHbiInit();
    //gTwolf_fd = open(file_name, O_RDWR);
    if (status < 0) {
        perror("tw_spi_access open");
        fclose(BOOT_FD);
        return -1;
    }

    printf("1- Opening firmware file - done....\n");


    status  = VprocTwolfHbiBootPrepare();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfHbiBootPrepare()\n", status);
        fclose(BOOT_FD);
        VprocTwolfHbiCleanup();
        return -1;
    }
    printf("-- Boot prepare - done....\n");

    while (fgets(line, 256, BOOT_FD) != NULL) {
        status = VprocTwolfHbiBootMoreData(line);
        if (status == VPROC_STATUS_BOOT_LOADING_MORE_DATA) {
            continue;
        } else if (status == VPROC_STATUS_BOOT_LOADING_CMP) {

            break ;
        } else if (status != VPROC_STATUS_SUCCESS) {
            printf("Error %d:VprocTwolfHbiBootMoreData()\n", status);
            fclose(BOOT_FD);
            VprocTwolfHbiCleanup();
            return -1;
        }
    }
    printf("-- Firmware data transfer - done....\n");
    fclose(BOOT_FD);

    status  = VprocTwolfHbiBootConclude();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfHbiBootConclude()\n", status);
        VprocTwolfHbiCleanup();
        return -1;
    }

#ifdef SAVE_IMAGE_TO_FLASH
    printf("-- Saving firmware to flash....\n");
    status = VprocTwolfSaveImgToFlash();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfSaveImgToFlash()\n", status);
        VprocTwolfHbiCleanup();
        return -1;
    }
    printf("-- Saving firmware to flash....done\n");

#endif

    status  = VprocTwolfFirmwareStart();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfFirmwareStart()\n", status);
        VprocTwolfHbiCleanup();
        return -1;
    }

    printf("3- Loading the config file into the device RAM\n");
    if (readCfgFile(argv[2]) < 0) {
        printf("Error:read %s file\n", argv[2]);
    }
    printf("a- Reading config file to host RAM - done....\n");


    status  = VprocTwolfLoadConfig(pCr2Buf, numElements);
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfLoadConfig()\n", status);
        VprocTwolfHbiCleanup();
        return -1;
    }

#ifdef SAVE_CFG_TO_FLASH
    printf("-- Saving config to flash....\n");
    status = VprocTwolfSaveCfgToFlash();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfSaveCfgToFlash()\n", status);
        VprocTwolfHbiCleanup();
        return -1;
    }
    printf("-- Saving config to flash....done\n");

#endif
    printf("Device boot loading completed successfully...\n");

    VprocTwolfHbiCleanup();

    return 0;
}


