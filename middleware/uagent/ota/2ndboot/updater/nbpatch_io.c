/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "nbpatch.h"
#include "updater.h"

int ota_patch_read(unsigned long src, unsigned char *buffer, unsigned long pos, unsigned long size)
{
    unsigned long base = pos;
    unsigned long read_size = 0;
    int ret = 0;

    while(size > 0) {
        read_size = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = flash_read_data(src + pos, buffer+pos-base, read_size);
        if(ret < 0) {
            return -1;
        }
        pos += read_size;
        size -= read_size;
    }
    return pos - base;
}

int ota_patch_write(unsigned long dst, unsigned char *buffer, unsigned long pos, unsigned long size)
{
    unsigned long base = pos;
    unsigned long write_size = 0;
    int ret = 0;

    while(size > 0) {
        write_size = size > SECTOR_SIZE? SECTOR_SIZE:size;
        ret = flash_write_data(dst + pos, buffer+pos-base, write_size);
        if(ret < 0) {
            return ret;
        }
        pos += write_size;
        size -= write_size;
    }
    return pos - base;
}

int ota_patch_new_data(unsigned char *buf, long size)
{
    unsigned long pos = 0;
    unsigned long bsize = 0;
    int ret = 0;
    hal_logic_partition_t *ota_info = flash_get_info(HAL_PARTITION_OTA_TEMP);
    if(ota_info == NULL) {
        printf("ota flash info err.\r\n");
        return -1;
    }
    unsigned int diff_data = ota_info->partition_start_addr + ota_info->partition_length - 0x10000;
    ret = flash_erase(diff_data, size);
    if(ret < 0) {
        return ret;
    }
    while (size > 0) {
        bsize = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        ret = flash_write_data(diff_data + pos, buf + pos, bsize);
        if(ret < 0) {
            return ret;
        }
        pos += bsize;
        size -= bsize;
    }
    return 0;
}
int ota_load_new_data(unsigned long dst, long size) {
    unsigned char tmp[SECTOR_SIZE];
    unsigned long pos = 0;
    unsigned long bsize = 0;
    int ret = 0;
    hal_logic_partition_t *app_info = flash_get_info(HAL_PARTITION_APPLICATION);
    hal_logic_partition_t *ota_info = flash_get_info(HAL_PARTITION_OTA_TEMP);
    if(app_info == NULL || ota_info == NULL) {
        printf("ota data & app info err.\r\n");
        return -1;
    }
    unsigned int diff_data = ota_info->partition_start_addr + ota_info->partition_length - 0x10000;
    unsigned int diff_app = app_info->partition_start_addr;
    ret = flash_erase(diff_app + dst, size);
    if(ret < 0) {
        return ret;
    }
    while (size > 0) {
        bsize = size > SECTOR_SIZE ? SECTOR_SIZE : size;
        memset(tmp, 0, SECTOR_SIZE);
        ret = flash_read_data(diff_data + pos, tmp, bsize);
        if(ret < 0) {
            return ret;
        }
        ret = flash_write_data(diff_app + dst + pos, tmp, bsize);
        if(ret < 0) {
            return ret;
        }
        pos += bsize;
        size -= bsize;
    }
    return ret;
}

int ota_patch_read_param(ota_boot_param_t *ota_param) {
    int ret = 0;
    unsigned short patch_crc = 0;
    if(!ota_param){
        return -1;
    }
    hal_logic_partition_t *par_info = flash_get_info(HAL_PARTITION_PARAMETER_1);
    if(par_info == NULL) {
        printf("par info err.\r\n");
        return -1;
    }
    ret = flash_read_data(par_info->partition_start_addr,(unsigned char *)ota_param, sizeof(ota_boot_param_t));
    if(ret < 0) {
        return -1;
    }
    patch_crc = crc16_computer(ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    if(patch_crc == ota_param->param_crc) {
        return 0;
    }
    OTA_LOG_I("ota crc cal:0x%04x  param:0x%04x \n", patch_crc, ota_param->param_crc);
    return ret;
}

int ota_patch_write_param(ota_boot_param_t *ota_param) {
    int ret = 0;
    if(!ota_param){
        return -1;
    }
    hal_logic_partition_t *par_info = flash_get_info(HAL_PARTITION_PARAMETER_1);
    if(par_info == NULL) {
        printf("par info err.\r\n");
        return -1;
    }

    ota_param->param_crc = crc16_computer(ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    ret = flash_erase(par_info->partition_start_addr, sizeof(ota_boot_param_t));
    if(ret < 0) {
        return -1;
    }
    ret = flash_write_data(par_info->partition_start_addr, (unsigned char *) ota_param, sizeof(ota_boot_param_t));
    if (ret < 0) {
        return -1;
    }
    return ret;
}

