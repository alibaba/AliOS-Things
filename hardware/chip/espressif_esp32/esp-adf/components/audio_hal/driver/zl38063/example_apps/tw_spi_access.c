/****************************************************************************
* tw_spi_access.c - Demo apps demonstrating how to access registers of the
* device over spi or I2C. Loading a firmware and or config into the device
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

/*NOTE: notice that the *.c code are included in the apps-
* This is because the compiler I'm using requires that
* But if your makefile is such that compiler knows where to find these files
* then remove the #include *.c below
*/
#include "zl38063_config.h"
#include "zl38063_firmware.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#undef SAVE_IMAGE_TO_FLASH /*define this macro to save the firmware from RAM to flash*/
#undef SAVE_CFG_TO_FLASH   /*define this macro to save the cfg from RAM to flash*/

#define SAVE_IMAGE_TO_FLASH
#define SAVE_CFG_TO_FLASH
/*quick test*/

/*LoadFwrConfig_Alt - to load a converted *s3, *cr2 to c code into the device.
* Basically instead of loading the *.s3, *cr2 directly,
* use the tw_convert tool to convert the ascii hex fwr mage into code and compile
* with the application
*
* input arg: mode:  0 - load both firmware and confing
*                   1 - load firmware only
*                   2 - load config only
*                  -1 - Force loading
*/
int tw_upload_dsp_firmware(int mode)
{
    union {
        short a;
        char b;
    } test_bigendian;
    if (mode >= 0) {
        uint16 vol = 0;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        int ret = VprocTwolfGetAppStatus(&vol);
        if (vol) {
            ESP_LOGW(TAG_SPI, "MCS ret:%d,Status:%d", ret, vol);
            return 0;
        }
        ESP_LOGI(TAG_SPI, "** Loading DSP firmware ret:%d,Status:%d **", ret, vol);
    } else {
        mode = 0;
    }
    test_bigendian.a = 1;
    ESP_LOGI(TAG_SPI, "b=%d", test_bigendian.b);

    int status = VprocTwolfHbiInit();
    if (status < 0) {
        DEBUG_LOGE(TAG_SPI, "tw_spi_access open");
        return -1;
    }

    if ((mode == 0) || (mode == 1)) {
        twFirmware st_Firmware;
        st_Firmware.st_Fwr = (twFwr *)st_twFirmware;
        st_Firmware.twFirmwareStreamLen = (uint16)firmwareStreamLen;
        st_Firmware.execAddr  = (uint32)executionAddress;
        st_Firmware.havePrgmBase = (uint8)haveProgramBaseAddress;
        st_Firmware.prgmBase = (uint32)programBaseAddress;

        ESP_LOGI(TAG_SPI, "1- Firmware boot loading started ....");

        status  = VprocTwolfHbiBoot_alt(&st_Firmware);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Error %d:VprocTwolfHbiBoot()", status);
            // VprocTwolfHbiCleanup();
            return -1;
        }

        ESP_LOGI(TAG_SPI, "2- Loading the image to RAM....done");
#ifdef SAVE_IMAGE_TO_FLASH
        ESP_LOGI(TAG_SPI, "-- Saving firmware to flash....");
        status = VprocTwolfSaveImgToFlash();
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Error %d:VprocTwolfSaveImgToFlash()", status);
            // VprocTwolfHbiCleanup();
            return status;
        }
        ESP_LOGI(TAG_SPI, "-- Saving firmware to flash....done");

#endif
        status  = VprocTwolfFirmwareStart();
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Error %d:VprocTwolfFirmwareStart()", status);
            // VprocTwolfHbiCleanup();
            return status;
        }
    }

#if 1
    if ((mode == 0) || (mode == 2)) {
        ESP_LOGI(TAG_SPI, "3- Loading the config file into the device RAM....");

        status  = VprocTwolfLoadConfig((dataArr *)st_twConfig, (uint16)configStreamLen);
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Error %d:VprocTwolfLoadConfig()", status);
            // VprocTwolfHbiCleanup();
            return status;
        }
#ifdef SAVE_CFG_TO_FLASH
        ESP_LOGI(TAG_SPI, "-- Saving config to flash....");
        status = VprocTwolfSaveCfgToFlash();
        if (status != VPROC_STATUS_SUCCESS) {
            DEBUG_LOGE(TAG_SPI, "Error %d:VprocTwolfSaveCfgToFlash()", status);
            // VprocTwolfHbiCleanup();
            return status;
        }
        ESP_LOGI(TAG_SPI, "-- Saving config to flash....done");

#endif
    }
    /*Firmware reset - in order for the configuration to take effect*/
    status  = VprocTwolfReset(VPROC_RST_SOFTWARE);
    if (status != VPROC_STATUS_SUCCESS) {
        DEBUG_LOGE(TAG_SPI, "Error %d:VprocTwolfReset()", status);
        ESP_LOGI(TAG_SPI,"Error");
        // VprocTwolfHbiCleanup();
        return status;
    }
#endif

    ESP_LOGI(TAG_SPI, "Device boot loading completed successfully...");
    return status;
}


int zl38063_comm(int argc, char **argv)
{
    VprocStatusType status = VPROC_STATUS_SUCCESS;

    if (argc == 1) {
        printf("Usage: for help type:%s -h", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "-h") == 0) {
        printf("\nUsage:\n\t%s [-cmd mode] [options...] see supported"
               " command modes below\n\n", argv[0]);
        printf("\t-wr    : to write one or more 16-bit words to the device\n"
               "\t-rd    : to read one or more 16-bit words from the device\n"
               "\t-rst   : to reset the device in one of these"
               " supported reset modes:\n"
               "\t\t - [0: RAM | 1: ROM | 2: SOFT | 3:  AEC | 4: BOOT]\n");
        printf("\t-lfcff : to load a specific firmware and related"
               " config from flash - arg: 1 to 14\n");
        printf("\t-lfcfh-a : to load a pre-compiled firmware and related config"
               " from host via SPI\n");
        printf("\t-lffh-a: to load a pre-compiled firmware from host via SPI\n");
        printf("\t-lcfh-a: to load a pre-compiled config from host via SPI\n");
        printf("\t-fclr  : to erase the content of the ZL380xx slave flash\n");
        printf("\t-sto   : to reset the device into boot mode\n");
        printf("\t-sta   : to start execution of firmware found at "
               "exec address in RAM\n");
        printf("\t-apla  : to configure the ZL380xx x-point for "
               "audio playback mode\n");
        printf("\t-arec  : to configure the ZL380xx x-point for audio "
               "recording mode\n");
        printf("Example:\n");
        printf("\tEx to write 0x8004 into register 0x0300:"
               "\n\t%s -wr 0x0300 0x8004\n\n", argv[0]);
        printf("\tEx to read 12 words starting from register 0x020:"
               "\n\t%s -rd 0x0020 12\n\n", argv[0]);
        printf("\tEx to reset the device in boot mode:"
               "\n\t%s -rst n 'where n:[1-4]\n\n", argv[0]);
        printf("\tEx to load to RAM a firmware and config previously"
               " saved to flash at index 1:\n\t%s -lfcff 1\n\n", argv[0]);
        printf("\tEx to load to RAM a firmware previously"
               " saved to flash at index 1:\n\t%s -lfff 1\n\n", argv[0]);
        printf("\tEx to load to RAM the pre-compiled (in)firmware "
               "from teh host "
               "HBI (SPI):\n\t%s -lffh-a\n\n", argv[0]);
        printf("\tEx to load to RAM the pre-compiled (in)config "
               "from teh host "
               "HBI (SPI):\n\t%s -lcfh-a\n\n", argv[0]);
        printf("\tEx to load to RAM the pre-compiled (in)firmware and config "
               "from teh host "
               "HBI (SPI):\n\t%s -lfcfh-a\n\n", argv[0]);
        printf("\tEx to stop the firmware currently running and clear "
               "the RAM:\n\t%s -sto\n\n", argv[0]);
        printf("\tEx to start a firmware previously loaded into "
               "RAM:\n\t%s -sta\n\n", argv[0]);
        printf("\tEx to mute SOUT :\n\t%s -mute_s [1 | 0]\n\n", argv[0]);
        printf("\tEx to mute ROUT :\n\t%s -mute_r [1 | 0]\n\n", argv[0]);
        printf("\tEx to erase the slave flash device controlled by "
               "the ZL380xx :\n\t%s -fclr\n\n", argv[0]);
        printf("\tEx to configure the device for recording mode at a "
               "desired clock and sample rates"
               " with AEC off[0] or on [1]:\n\t%s -arec clkrate "
               "fsrate  n 'where n:[0 | 1]'\n\n", argv[0]);
        printf("\tEx to configure the device for playback mode at a "
               "desired clock and sample rates"
               " with AEC off[0] or on [1]:\n\t%s -apla clkrate "
               "fsrate n 'where n:[0 | 1]'\n\n", argv[0]);
        return -1;
    }

    if ((argc < 3) && (strcmp(argv[1], "-wr") == 0)) {
        printf("Usage:%s -wr register value0 value1....value124 \n",
               argv[0]);
        return -1;
    }
    if ((argc < 3) && (strcmp(argv[1], "-rd") == 0)) {
        printf("Usage:%s -rd register n 'where n:[1-127]'\n", argv[0]);
        return -1;
    }
    if ((argc < 3) && (strcmp(argv[1], "-rst") == 0 )) {
        printf("Usage:%s -rst  n 'where n:[0-4]'\n", argv[0]);
        return -1;
    }
    if ((argc < 3) && (strcmp(argv[1], "-apla") == 0)) {
        printf("Usage:%s -apla <clkrate in KHz> <fsrate in Hz> n"
               " 'where n:[0 | 1]'\n", argv[0]);
        return -1;
    }
    if ((argc < 3) && (strcmp(argv[1], "-arec") == 0)) {
        printf("Usage:%s -arec <clkrate in KHz> <fsrate in Hz> n"
               " 'where n:[0 | 1]'\n", argv[0]);
        return -1;
    }
    if ((argc < 3) && (strcmp(argv[1], "-lfcff") == 0)) {
        printf("Usage:%s -lfcfh 1\n", argv[0]);
        return -1;
    }
    /*global file handle*/
    status = VprocTwolfHbiInit();
    if (status < 0) {
        perror("tw_spi_access open");
        return -1;
    }
    if ((strcmp(argv[1], "-wr") == 0) || (strcmp(argv[1], "-rd") == 0)) {
        int i = 0, j = 0;
        unsigned short val[128];
        unsigned short  cmdword = (unsigned short)strtoul(argv[2], NULL, 0);
        unsigned char  numwords = 0;

        memset(val, 0, sizeof(val));

        if  (strcmp(argv[1], "-wr") == 0) { /*for WRITING 1 or more ZL380xx registers*/

            unsigned short val[128];
            numwords = argc - 3; /*calculate the number of words to write*/;
            for (i = 0; i < numwords; i++) {
                val[i] = (unsigned short)strtoul(argv[3 + i], NULL, 0);
            }
            status  = VprocTwolfHbiWrite(cmdword, numwords, val);
            if (status != VPROC_STATUS_SUCCESS) {
                printf("Error %d:VprocTwolfHbiWrite()\n", status);
                VprocTwolfHbiCleanup();
                return -1;
            }
            for (i = 0; i < numwords; i++) {
                printf("wr: addr 0x%04x = 0x%04x\n", (cmdword + j), val[i]);
                j = j + 2;
            }

        } else {    /*for READING 1 or more ZL380xx registers**/
            numwords = (unsigned char)strtoul(argv[3], NULL, 0);
            if ((numwords == 0) || (numwords > 128)) {
                printf("number of words is out of range. Maximum is 128\n");
                VprocTwolfHbiCleanup();
                return -1;
            }
            status  = VprocTwolfHbiRead(cmdword, numwords, val);
            if (status != VPROC_STATUS_SUCCESS) {
                printf("Error %d:VprocTwolfHbiRead()\n", status);
                VprocTwolfHbiCleanup();
                return -1;
            }
            for (i = 0; i < numwords; i++) {
                printf("RD: addr 0x%04x = 0x%04x\n", (cmdword + j), val[i]);
                j = j + 2;
            }
        }
    } else if (strcmp(argv[1], "-rst") == 0) {  /*for RESETTING ZL380xx*/
        unsigned char rstMode = (unsigned char)strtoul(argv[2], NULL, 0);
        status  = VprocTwolfReset((uint16)rstMode);
        if (status != VPROC_STATUS_SUCCESS) {
            printf("Error %d:VprocTwolfHbiRead()\n", status);
            VprocTwolfHbiCleanup();
            return -1;
        }
        printf("Device reset completed successfully...\n");

    } else if (strcmp(argv[1], "-lfcff") == 0) {
        /*Load ZL380x0 firmware + related config record from flash*/
        unsigned short image_num = (unsigned short)strtoul(argv[2], NULL, 0);
        status  = VprocTwolfFirmwareStop();
        if (status != VPROC_STATUS_SUCCESS) {
            printf("Error %d:VprocTwolfFirmwareStop()\n", status);
            VprocTwolfHbiCleanup();
            return -1;
        }
        status  = VprocTwolfLoadFwrCfgFromFlash(image_num);
        if (status != VPROC_STATUS_SUCCESS) {
            printf("Error %d:VprocTwolfLoadFwrCfgFromFlash()\n", status);
            VprocTwolfHbiCleanup();
            return -1;
        }
        status  =  VprocTwolfReset(VPROC_RST_HARDWARE_RAM);
        if (status != VPROC_STATUS_SUCCESS) {
            printf("Error %d:VprocTwolfReset()\n", status);
            VprocTwolfHbiCleanup();
            return -1;
        }
        printf("Device boot loading from flash completed successfully...\n");
    } else if (strcmp(argv[1], "-lfff") == 0) {


        if (status != VPROC_STATUS_SUCCESS) {
            printf("Error %d:VprocTwolfLoadFwrFromFlash()\n", status);
            VprocTwolfHbiCleanup();
            return -1;
        }

        printf("Device boot loading from flash completed successfully...\n");

    } else if (strcmp(argv[1], "-lfcfh-a") == 0) { /*for LOADING FWR/CFG via SPI*/
        if (tw_upload_dsp_firmware(0) !=  VPROC_STATUS_SUCCESS)
            printf("Device boot loading failed.....\n");


    } else if (strcmp(argv[1], "-lcfh-a") == 0) { /*for LOADING CFG via SPI*/
        if (tw_upload_dsp_firmware(2) !=  VPROC_STATUS_SUCCESS)
            printf("Device boot loading failed.....\n");


    } else if (strcmp(argv[1], "-lffh-a") == 0) { /*for LOADING FWR via SPI*/
        if (tw_upload_dsp_firmware(1) !=  VPROC_STATUS_SUCCESS)
            printf("Device boot loading failed.....\n");

    } else if (strcmp(argv[1], "-sto") == 0) {  /*for resetting into boot mode*/
        if (VprocTwolfFirmwareStop() != 0)
            VprocTwolfHbiCleanup();
        else
            printf("Firmware stopped to boot mode completed"
                   " successfully...\n");
    } else if (strcmp(argv[1], "-sta") == 0) {  /*start executing FWR/CFG */
        if (VprocTwolfFirmwareStart() != 0)
            VprocTwolfHbiCleanup();
        else
            printf("Firmware is now running successfully...\n");
    } else if (strcmp(argv[1], "-mute_r") == 0) {  /*start executing FWR/CFG */
        uint8 mute = (uint8)strtoul(argv[2], NULL, 0);
        //to do need fix
        //if(VprocTwolfMute(VPROC_ROUT, mute) != 0)
        if (1) {
            VprocTwolfHbiCleanup();
        } else {
            if (mute)
                printf("ROUT Port muted sucessfully...\n");
            else
                printf("ROUT Port unmuted sucessfully...\n");
        }
    } else if (strcmp(argv[1], "-mute_s") == 0) {  /*start executing FWR/CFG */
        uint8 mute = (uint8)strtoul(argv[2], NULL, 0);
        //to do need fix
        //if(VprocTwolfMute(VPROC_SOUT, mute) != 0)
        if (1)
            VprocTwolfHbiCleanup();
        else {
            if (mute)
                printf("SOUT Port muted sucessfully...\n");
            else
                printf("SOUT Port unmuted sucessfully...\n");
        }
    } else if ((strcmp(argv[1], "-arec") == 0) || (strcmp(argv[1], "-apla") == 0))
        /* configure the ZL380x0 for either audio recording or playback
         * Over an I2S link
         */
    {
        unsigned short pclkrate = (unsigned short)strtoul(argv[2], NULL, 0);
        unsigned short fsrate = (unsigned short)strtoul(argv[3], NULL, 0);
        unsigned short aecState = (unsigned char)strtoul(argv[4], NULL, 0);
        printf("pclkrate = %u KHz, fsrate = %u Hz, AEC state = %d\n",
               pclkrate, fsrate, aecState);
        //to do need fix
#if 0
        if (strcmp(argv[1], "-arec") == 0) {
            if (VprocTwolfUpstreamConfigure(pclkrate, fsrate, aecState) != 0)
                VprocTwolfHbiCleanup();
            else
                printf("Device configured for audio recording...\n");
        } else if (strcmp(argv[1], "-apla") == 0) {
            if (VprocTwolfDownstreamConfigure(pclkrate, fsrate, aecState) != 0)
                VprocTwolfHbiCleanup();
            else
                printf("Device configured for audio playback...\n");
        }
#endif
    } else if (strcmp(argv[1], "-fclr") == 0) {
        /*Erase the full content of the ZL380x0 controlled slave flash*/
        status  = VprocTwolfEraseFlash();
        if (status != VPROC_STATUS_SUCCESS) {
            printf("Error %d:VprocTwolfEraseFlash()\n", status);
            VprocTwolfHbiCleanup();
            return -1;
        }
        printf("flash erasing completed successfully...\n");
    } else {
        printf("Usage: for help type:\n%s -h\n", argv[0]);
    }
    printf("\n");

    return 0;
}
