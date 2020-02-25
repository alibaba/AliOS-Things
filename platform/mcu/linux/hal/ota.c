/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ota/ota_hal.h"

static FILE *ota_fd     = NULL;
#if defined PLAT_RASPBERRY /*raspberry*/
static int boot_part    = 2;
static int boot_part_in = 3;
#define OTA_IMAGE_FILE     "/dev/mmcblk0p"
#else
static int boot_part    = 4;
static int boot_part_in = 5;
#define OTA_IMAGE_FILE     "rkflash0p"
#endif

int getCmdVal(const char *cmd)
{
    char ps[1024] = {0};
    FILE *ptr = NULL;
    char *sstr = NULL;
    if((ptr=popen(cmd, "r")) != NULL) {
        while(fgets(ps, 1024, ptr) != NULL) {
            ;
        }
        pclose(ptr);
        ptr = NULL;
        printf("ret:%s\n", ps);
    }
    else {
        printf("popen %s error\n", ps);
    }
    sstr = strstr(ps, "mender_boot_part=");
    if (sstr) {
        sscanf(sstr, "%*[^ ]%d", &boot_part);
        printf("mender_boot_part:%d \n", boot_part);
    }
#if defined PLAT_RASPBERRY
    if(boot_part == 3) {
        boot_part_in = 2;
    }
    else {
        boot_part_in = 3;
    }
#else
    if(boot_part == 5) {
        boot_part_in = 4;
    }
    else {
        boot_part_in = 5;
    }
#endif
    printf("ota init:%d  boot_part_in:%d\n", boot_part, boot_part_in);
    return 0;
}

int ota_hal_init(ota_boot_param_t *param)
{  
    char path[64] = {0};
    int ret       = OTA_INIT_FAIL;
    printf("ota hal linux init.\n");
    if(param != NULL) {
        getCmdVal("fw_printenv mender_boot_part");
        snprintf(path, sizeof(path), "%s%d", OTA_IMAGE_FILE, boot_part_in);
        printf("init size:0x%x \n", param->len);
        if(param->len != 0) {
            ret = 0;
#if !defined PLAT_RASPBERRY
            if(param->len > 0x3100000) {
                printf("firmware size is too big.\n");
            }
#endif
        }
    }
    return ret;
}

int ota_hal_write(unsigned int *off_set, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    if(ota_fd == NULL) {
        char path[64] = {0};
        snprintf(path, sizeof(path), "%s%d", OTA_IMAGE_FILE, boot_part_in);
        ota_fd = fopen(path, "w");
    }
    ret = fwrite(in_buf, in_buf_len, 1, ota_fd);
    if (ret < 0) {
        printf("write err: %d, %d ,%s\n", ret, in_buf_len, strerror(errno));
        ret = OTA_UPGRADE_WRITE_FAIL;
    }
    return ret;
}

int ota_hal_read(unsigned int *off_set, char *out_buf, unsigned int out_buf_len)
{
    int ret = 0;
    if (ota_fd != NULL) {
        fflush(ota_fd);
        fclose(ota_fd);
        ota_fd = NULL;
    }
    if(ota_fd == NULL) {
        char path[64] = {0};
        snprintf(path, sizeof(path), "%s%d", OTA_IMAGE_FILE, boot_part_in);
        ota_fd = fopen(path, "r");
        if(ota_fd == NULL) {
            ret = OTA_UPGRADE_WRITE_FAIL;
        }
    }
    if(ota_fd != NULL) {
        ret = fseek(ota_fd, *off_set, SEEK_SET);
        ret = fread(out_buf, out_buf_len, 1, ota_fd);
        if (ret != 1) {
            printf("read err: %d, %d, %s\n", ret, out_buf_len, strerror(errno));
            ret = OTA_UPGRADE_WRITE_FAIL;
        }
        else {
            *off_set += out_buf_len;
        }
        fclose(ota_fd);
        ota_fd = NULL;
    }
    return ret;
}

int ota_hal_boot(ota_boot_param_t *param)
{
   int ret = OTA_UPGRADE_WRITE_FAIL;
   char cmd1[64] = { 0 };
   if (ota_fd != NULL) {
       fflush(ota_fd);
       fclose(ota_fd);
       ota_fd = NULL;
   }
   if (param != NULL) {
       printf("boot err\n");
       ret = 0;
       if (param->upg_status == OTA_FINISH) {
           sprintf(cmd1, "fw_setenv mender_boot_part %d", boot_part_in);
           ret = system(cmd1);
           if(ret >= 0) {
               printf("boot:%s \n", cmd1);
               memset(cmd1, 0x00, sizeof(cmd1));
               sprintf(cmd1, "fw_setenv upgrade_available 1");
               ret = system(cmd1);
               printf("boot:%s \n", cmd1);
               if(ret >= 0) {
                   memset(cmd1, 0x00, sizeof(cmd1));
                   sprintf(cmd1, "sync;reboot");
                   ret = system(cmd1);
                   if(ret < 0) {
                       printf("sync err\n");
                       ret = OTA_UPGRADE_WRITE_FAIL;
                    }
               }
           }
       }
   }
   printf("Rebooting and updating FLASH now....\n");
   return ret;
}

int ota_hal_rollback(void)
{
    char cmd1[128] = {0};
    int ret = 0;
    sprintf(cmd1, "fw_setenv upgrade_available 0; fw_setenv bootcount 0; sync");
    ret = system(cmd1);
    if(ret < 0) {
        printf("roll system fail .");
        ret = OTA_UPGRADE_WRITE_FAIL;
    }
    printf("roll:%s \n", cmd1);
    return ret;
}

const char *ota_hal_version(unsigned char dev_type, char* dn)
{
    if(dev_type > 0) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return SYSINFO_APP_VERSION;
    }
}
