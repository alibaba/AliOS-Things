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
#include "esp_log.h"
#include "vproc_common.h"
#include "vprocTwolf_access.h"

/*NOTE: notice that the *.c code are included in the apps-
* This is because the compiler I'm using requires that
* But if your makefile is such that compiler knows where to find these files
* then remove the #include *.c below
*/

#undef SAVE_CFG_TO_FLASH   /*define this macro to save the cfg from RAM to flash*/
static const char *TAG = "TW_LDCFG";
uint16 numElements;
dataArr* pCr2Buf;

/* fseekNunlines() -- The firmware file is an ascii text file.
 * the information from fseek will not be useful.
 * this is our own fseek equivalent
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
            ESP_LOGD(TAG,"Error: file is not of the correct format...");
            return -1;
        }
        ESP_LOGD(TAG,"fileLength = %u", len);
        /*start at the beginning of the file*/
        //fseek(BOOT_FD, 0, SEEK_SET);

        /* allocate memory to contain the reg and val:*/
        pCr2Buf = (dataArr*) malloc(len * sizeof(dataArr));
        if (pCr2Buf == NULL) {
            ESP_LOGD(TAG,"not enough memory to allocate %u bytes.. ", len * sizeof(dataArr));
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
                // ESP_LOGD(TAG,"pCr2Buf[%d].reg pCr2Buf[%d].value  = 0x%04x\t0x%04x\n", index, index, pCr2Buf[index].reg, pCr2Buf[index].value);
                index++;
            } else { done = 1;}

        } while (done == 0);

        fclose(BOOT_FD);
        ESP_LOGD(TAG,"size of pCr2Buf = %u bytes.. ", sizeof(pCr2Buf));
    } else {
        ESP_LOGD(TAG,"Error: can't open file");
    }
    return 0;
}

/*This example host app load the *.s3 firmware to the device RAM. Optionally save it to flash
 * Then start the firmware from the execution address in RAM
 * It then stops the firmware - Load the cr2 file into RAM. Optionally save it to flash
 * Then restarts the firmware
 */

int main (int argc, char** argv)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;


    if (argc != 2) {
        ESP_LOGD(TAG,"Error: argc = %d - missing %d arg(s)... ", argc, 3 - (argc - 1));
        ESP_LOGD(TAG,"command Usage:%s ConfigPath", argv[0]);
        exit(1);
    }
    ESP_LOGD(TAG,":%s %s %s", argv[0], argv[1], argv[2]);


    /*global file handle*/
    status = VprocTwolfHbiInit();

    if (status < 0) {
        perror("tw_spi_access open");
        return -1;
    }

    if (readCfgFile(argv[1]) < 0) {
        ESP_LOGD(TAG,"Error:read %s file", argv[1]);
    }
    ESP_LOGD(TAG,"a- Reading config file to host RAM - done....");


    ESP_LOGD(TAG,"c- Loading the config file into the device RAM");
    status  = VprocTwolfLoadConfig(pCr2Buf, numElements);
    if (status != VPROC_STATUS_SUCCESS) {
        ESP_LOGD(TAG,"Error %d:VprocTwolfLoadConfig()", status);
        VprocTwolfHbiCleanup();
        return -1;
    }

#ifdef SAVE_CONFIG_TO_FLASH
    status = VprocTwolfSaveCfgToFlash();
    if (status != VPROC_STATUS_SUCCESS) {
        ESP_LOGD(TAG,"Error %d:VprocTwolfSaveCfgToFlash()", status);
        VprocTwolfHbiCleanup();
        return -1;
    }
    ESP_LOGD(TAG,"d- Saving config to flash- done....");
#endif

    ESP_LOGD(TAG,"e- Loading config record - done....");
    free(pCr2Buf);
    pCr2Buf = NULL;
    VprocTwolfHbiCleanup();

    return 0;
}
