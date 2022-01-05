#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include "nbpatch.h"
#include "updater.h"
#include "aos/hal/flash.h"

#define FLASH_SIZE 0x2000000

unsigned char* memory_base = NULL;

int ota_patch_init(char* old_file, char* patch_file){
    int ret = 0;
    int app_size = 0;
    int patch_size = 0;
    unsigned char* pos = NULL;
    int read_size = 0;
    int size = 0;
    FILE *old_fd = NULL;

    hal_logic_partition_t *ota_info = flash_get_info(HAL_PARTITION_OTA_TEMP);
    hal_logic_partition_t *app_info = flash_get_info(HAL_PARTITION_APPLICATION);
    if(ota_info == NULL || app_info == NULL) {
        OTA_LOG_I("flash info err.\r\n");
        return -1;
    }
    ota_boot_param_t ota_param = {0};
    memory_base  = malloc (FLASH_SIZE);
    if(memory_base == NULL) {
        return -1;
    }
    memset(memory_base, 0xFF, FLASH_SIZE);
    old_fd = fopen (old_file, "rb" );
    if (old_fd==NULL) {
        OTA_LOG_I("open old err:%d\n",errno);
        return -1;
    }
    fseek (old_fd , 0 , SEEK_END);
    app_size = ftell(old_fd);
    rewind (old_fd);

    pos = (unsigned char*)(memory_base + app_info->partition_start_addr);
    read_size = 0;
    size = app_size;
    while(size > 0) {
        read_size = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = fread(pos, read_size, 1, old_fd);
        if(ret < 0 ) {
            OTA_LOG_I("read old err:%d  ret:%d rsize:%d \n",errno, ret, read_size);
            return -1;
        }
        pos += read_size;
        size -= read_size;
    }
    fclose(old_fd);
    old_fd = fopen (patch_file, "rb" );
    if (old_fd==NULL) {
        OTA_LOG_I("open old err:%d\n",errno);
        return -1;
    }
    fseek (old_fd , 0 , SEEK_END);
    patch_size = ftell(old_fd);
    rewind (old_fd);
    memset(&ota_param, 0x00, sizeof(ota_boot_param_t));
    ota_param.dst_adr = app_info->partition_start_addr;
    ota_param.src_adr = ota_info->partition_start_addr;
    ota_param.len = patch_size;
    ota_param.old_size = app_info->partition_length;
    ota_param.upg_flag = OTA_UPGRADE_DIFF;
    ret = ota_patch_write_param(&ota_param);
    if(ret < 0) {
        OTA_LOG_I("write parameter err.\n");
        return -1;
    }
    pos = (unsigned char*)(memory_base + ota_info->partition_start_addr);
    read_size = 0;
    size = app_size;
    while(size > 0) {
        read_size = size > SECTOR_SIZE ? SECTOR_SIZE:size;
        ret = fread(pos, read_size, 1, old_fd);
        if(ret < 0) {
            OTA_LOG_I("read patch err:%d  ret:%d rsize:%d \n",errno, ret, read_size);
            return -1;
        }
        pos += read_size;
        size -= read_size;
    }
    fclose(old_fd);
    old_fd = NULL;
    OTA_LOG_I("ota patch load app size:0x%x patch size:0x%x success.\n", app_size, patch_size);
    return ret;
}

int ota_patch_dump_flash(char* name, int size){
    int ret = 0;
    FILE* all_fd = NULL;
    all_fd = fopen (name, "wb" );
    if (all_fd==NULL) {
        OTA_LOG_I("open name err:%d\n",errno);
        return -1;
    }
    hal_logic_partition_t *app_info = flash_get_info(HAL_PARTITION_APPLICATION);
    if(app_info == NULL) {
        OTA_LOG_I("app info err.\r\n");
        return -1;
    }
    OTA_LOG_I("ota patch new file off:%d size:%d.\n", app_info->partition_start_addr, size);
    ret = fwrite(memory_base + app_info->partition_start_addr, size, 1, all_fd);
    if(ret < 0){
        OTA_LOG_I("write name err:%d\n",errno);
        return -1;
    }
    fflush(all_fd);
    return ret;
}

int main(int argc, char *argv[])
{
    char *old_file  = NULL;
    char *new_file  = NULL;
    char *diff_file = NULL;
    int ret = 0;

    OTA_LOG_I("nbpatch ver:%s start ...\n", NBPATCH_VERSION);
    if(argc < 4) {
        OTA_LOG_I("Usage: %s old_file new_file diff file [log_file]\r\n", argv[0]);
        OTA_LOG_I("Notice: old file and new file less than 8M. \r\n");
        return -1;
    }
    old_file = argv[1];
    new_file = argv[2];
    diff_file = argv[3];

    OTA_LOG_I("ota patch old:%s new:%s diff:%s.\n", old_file, new_file, diff_file);
    ret = ota_patch_init(old_file, diff_file);
    if(ret < 0){
        return -1;
    }
    ret = ota_nbpatch_main();
    if(ret < 0) {
        return -1;
    }
    ret = ota_patch_dump_flash(new_file, ret);
    if(ret < 0) {
        return -1;
    }
    OTA_LOG_I("ota patch success. \n");
    return 0;
}
