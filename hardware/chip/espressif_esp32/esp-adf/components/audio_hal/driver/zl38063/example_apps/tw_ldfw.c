
/****************************************************************************
* tw_ldfw.c - To load a *.s3 firmware into the device
*             and optionally save the loaded  image to flash
*
*
****************************************************************************
* Copyright Microsemi Inc, 2018. All rights reserved. 
* Licensed under the MIT License. See LICENSE.txt in the project 
* root for license information.
*
***************************************************************************/

#include "vproc_common.h"
#include "vprocTwolf_access.h"
#include "esp_log.h"

/*NOTE: notice that the *.c code are included in the apps-
* This is because the compiler I'm using requires that
* But if your makefile is such that compiler knows where to find these files
* then remove the #include *.c below
*/

//#undef SAVE_IMAGE_TO_FLASH /*define this macro to save the firmware from RAM to flash*/
static const char *TAG = "TW_LDFW";
/*quick test*/

/*This example host app load the *.s3 firmware to the device RAM. Optionally save it to flash
 * Then start the firmware from the execution address in RAM
 */
int main (int argc, char** argv)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;
    FILE* BOOT_FD;
    char line[256] = "";

    if (argc != 2) {
        ESP_LOGD(TAG,"Error: argc = %d - missing %d arg(s)... ", argc, 3 - (argc - 1));
        ESP_LOGD(TAG,"command Usage:%s firmwarePath", argv[0]);
        exit(1);
    }
    ESP_LOGD(TAG,":%s %s %s", argv[0], argv[1], argv[2]);


    BOOT_FD = fopen(argv[1], "rb");
    if (BOOT_FD == NULL) {
        ESP_LOGD(TAG,"Error: can't open file %s", argv[1]);
        return -1;
    }

    /*global file handle*/
    status = VprocTwolfHbiInit();
    if (status < 0) {
        perror("tw_spi_access open");
        fclose(BOOT_FD);
        return -1;
    }

    ESP_LOGD(TAG,"1- Opening firmware file - done....");

    status  = VprocTwolfHbiBootPrepare();
    if (status != VPROC_STATUS_SUCCESS) {
        ESP_LOGD(TAG,"Error %d:VprocTwolfHbiBootPrepare()", status);
        fclose(BOOT_FD);
        VprocHALcleanup();
        return -1;
    }
    ESP_LOGD(TAG,"-- Boot prepare - done....");

    while (fgets(line, 256, BOOT_FD) != NULL) {
        status = VprocTwolfHbiBootMoreData(line);
        if (status == VPROC_STATUS_BOOT_LOADING_MORE_DATA) {
            continue;
        } else if (status == VPROC_STATUS_BOOT_LOADING_CMP) {

            break ;
        } else if (status != VPROC_STATUS_SUCCESS) {
            ESP_LOGD(TAG,"Error %d:VprocTwolfHbiBootMoreData()", status);
            fclose(BOOT_FD);
            VprocHALcleanup();
            return -1;
        }
    }
    ESP_LOGD(TAG,"-- Firmware data transfer - done....");
    fclose(BOOT_FD);
    /*clean up and verify that the boodloading completed correctly*/
    status  = VprocTwolfHbiBootConclude();
    if (status != VPROC_STATUS_SUCCESS) {
        ESP_LOGD(TAG,"Error %d:VprocTwolfHbiBootConclude()", status);
        VprocHALcleanup();
        return -1;
    }

    ESP_LOGD(TAG,"2- Loading firmware - done....");
#ifdef SAVE_IMAGE_TO_FLASH
    ESP_LOGD(TAG,"-- Saving firmware to flash....");
    status = VprocTwolfSaveImgToFlash();
    if (status != VPROC_STATUS_SUCCESS) {
        ESP_LOGD(TAG,"Error %d:VprocTwolfSaveImgToFlash()", status);
        VprocHALcleanup();
        return -1;
    }
    ESP_LOGD(TAG,"-- Saving firmware to flash....done");

#endif

    status  = VprocTwolfFirmwareStart();
    if (status != VPROC_STATUS_SUCCESS) {
        ESP_LOGD(TAG,"Error %d:VprocTwolfFirmwareStart()", status);
        VprocHALcleanup();
        return -1;
    }

    ESP_LOGD(TAG,"Device boot loading completed successfully...");

    VprocHALcleanup();

    return 0;
}

