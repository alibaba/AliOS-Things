/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include "ota_log.h"
#include "ota_hal_plat.h"

static FILE* ota_fd = NULL;
static int offset = 0;
#if defined PLAT_RASPBERRY /*raspberry*/
static int boot_part = 2;
static int boot_part_in = 3;
#define OTA_IMAGE_FILE     "/dev/mmcblk0p"
#else
static int boot_part = 4;
static int boot_part_in = 5;
#define OTA_IMAGE_FILE    "rkflash0p"
#endif

int getCmdVal(const char *cmd)
{
    char ps[1024]={0};
    FILE *ptr = NULL;
    char *sstr = NULL;
    if((ptr=popen(cmd, "r"))!=NULL)
    {
        while(fgets(ps, 1024, ptr)!=NULL)
        {
        }
        pclose(ptr);
        ptr = NULL;
        OTA_LOG_I("ret:%s\n",ps);
    }
    else
    {
        OTA_LOG_E("popen %s error\n", ps);
    }
    sstr = strstr(ps, "mender_boot_part=");
    if (sstr) {
        sscanf(sstr, "%*[^ ]%d", &boot_part);
        OTA_LOG_I("mender_boot_part:%d \n", boot_part);
    }
#if defined PLAT_RASPBERRY
    if(boot_part == 3) {
        boot_part_in = 2;
    } else {
        boot_part_in = 3;
    }
#else
    if(boot_part == 5) {
        boot_part_in = 4; 
    } else {
        boot_part_in = 5;
    }
#endif
    OTA_LOG_I("ota init:%d  boot_part_in:%d\n", boot_part,boot_part_in);
    return 0;
}

static int ota_init(void *something)
{
    char path[64] = {0};
    int ret = 0;
    ota_boot_param_t * param = (ota_boot_param_t*)something;
    getCmdVal("fw_printenv mender_boot_part");
    snprintf(path,sizeof(path),"%s%d",OTA_IMAGE_FILE,boot_part_in);
    offset= param->off_bp;
    OTA_LOG_I("init off: %d size:0x%x \n", offset,param->len);
    if(param->len == 0) {
        OTA_LOG_E("ota init size error:");
        return -1;
    }
#if !defined PLAT_RASPBERRY
    if(param->len > 0x3100000){
        OTA_LOG_E("firmware size is too big.\n");
        return -1;
    }
#endif
    if(offset!=0){ /*breakpoint resume*/
        if(ota_fd==NULL){
            if((ota_fd = fopen(path, "a+"))==NULL) {
                OTA_LOG_E("init err: %d, %s\n", ret, strerror(errno));
                return -1;
            }
            if(ftell(ota_fd)<=0){
                OTA_LOG_E("init error: %d, %s\n", ret, strerror(errno));
                return -1;
            }
        }
    }
    return ret;
}

int ota_write(int *off_set, char *in_buf, int in_buf_len)
{
    int ret = 0;
    if(ota_fd == NULL) 
    {
        char path[64] = {0};
        snprintf(path,sizeof(path),"%s%d",OTA_IMAGE_FILE,boot_part_in);
        ota_fd = fopen(path, "w");
    }
    ret = fwrite(in_buf, in_buf_len, 1, ota_fd);
    if (ret < 0) {
        OTA_LOG_E("write err: %d, %d ,%s\n", ret, in_buf_len,
                  strerror(errno));
        return -1;
    }
    return 0;
}

static int ota_read(int *off_set, char *out_buf, int out_buf_len)
{
    if (ota_fd != NULL) {
        fflush(ota_fd);
        fclose(ota_fd);
        ota_fd = NULL;
    }
    int ret = 0;
    if(ota_fd == NULL)
    {
        char path[64] = {0};
        snprintf(path,sizeof(path),"%s%d",OTA_IMAGE_FILE,boot_part_in);
        ota_fd = fopen(path, "r");
    }
    ret = fseek(ota_fd, *off_set, SEEK_SET);
    ret = fread(out_buf, out_buf_len, 1, ota_fd);
    if (ret != 1) {
        OTA_LOG_E("read err: %d, %d ,%s\n", ret, out_buf_len, strerror(errno));
        return -1;
    }
    fclose(ota_fd);
    ota_fd = NULL;
    *off_set += out_buf_len;
    return 0;
}

static int ota_boot(void *something)
{
   int ret = 0;
   if (ota_fd != NULL) {
       fflush(ota_fd);
       fclose(ota_fd);
       ota_fd = NULL;
   }
   char cmd1[64] = { 0 };
   ota_boot_param_t *param = (ota_boot_param_t *)something;
   if (param==NULL){
       OTA_LOG_E("boot err\n");
       return -1;
   }
   if (param->res_type==OTA_FINISH) {
       sprintf(cmd1, "fw_setenv mender_boot_part %d",boot_part_in);
       ret = system(cmd1);
       if(ret < 0) {
           OTA_LOG_E("set boot part err.\n");
           return -1;
       }
       OTA_LOG_E("boot:%s \n",cmd1);
       memset(cmd1,0x00,sizeof(cmd1));
       sprintf(cmd1, "fw_setenv upgrade_available 1");
       ret = system(cmd1);
       if(ret < 0) {
           OTA_LOG_E("upgrade_available err\n");
           return -1;
       }
       OTA_LOG_E("boot:%s \n",cmd1);
       memset(cmd1,0x00,sizeof(cmd1));
       sprintf(cmd1, "sync;reboot");
       ret = system(cmd1);
       if(ret < 0) {
           OTA_LOG_E("sync err\n");
           return -1;
       }
   }
   OTA_LOG_I("Rebooting and updating FLASH now....\n");
   return 0;
}

static int ota_rollback(void *something)
{
    char cmd1[128] = {0};
    int ret = 0;
    sprintf(cmd1, "fw_setenv upgrade_available 0; fw_setenv bootcount 0; sync");
    ret = system(cmd1);
    if(ret < 0) {
        OTA_LOG_E("roll system fail .");
    }
    OTA_LOG_I("roll:%s \n",cmd1);
    return 0;
}

const char   *aos_get_app_version(void);
static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return aos_get_app_version();
    }
}

ota_hal_module_t ota_hal_module = {
    .init     = ota_init,
    .write    = ota_write,
    .read     = ota_read,
    .boot     = ota_boot,
    .rollback = ota_rollback,
    .version  = ota_get_version,
};
