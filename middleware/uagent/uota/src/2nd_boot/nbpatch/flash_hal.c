/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "flash_hal.h"
#include "nbpatch.h"

uint32_t patch_flash_get_partion_length(int par)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)rec_flash_get_info(par);

    return logic_partition->partition_length;
}

static hal_logic_partition_t *patch_flash_get_info(hal_partition_t par)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)rec_flash_get_info(par);

    return logic_partition;
}

int patch_flash_write(int par, const unsigned char *buffer, unsigned long offset, unsigned long len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = patch_flash_get_info( par );
    if(partition_info == NULL || offset + len > partition_info->partition_length){
        return -1;
    }

    start_addr = partition_info->partition_start_addr + offset;
    rec_wdt_feed();
    rec_flash_write_data((unsigned char *)buffer, start_addr, len);
    rec_wdt_feed();
    return  len;
}

int patch_flash_read(int par, const unsigned char *buffer, unsigned long offset, unsigned long len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    partition_info = patch_flash_get_info( par );
    if (partition_info == NULL || offset + len > partition_info->partition_length){
        return -1;
    }

    start_addr = partition_info->partition_start_addr + offset;
    rec_wdt_feed();
    rec_flash_read_data((unsigned char *)buffer, start_addr, len);
    rec_wdt_feed();

    return  len;
}

int patch_flash_erase(int par, unsigned long offset, size_t esize)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    hal_logic_partition_t *partition_info;

    partition_info = patch_flash_get_info( par );
    if (partition_info == NULL || offset + esize > partition_info->partition_length) {
        return -1;
    }

    start_addr = (partition_info->partition_start_addr + offset) & (~(SECTOR_SIZE-1));
    end_addr = (partition_info->partition_start_addr + offset + esize  - 1) & (~(SECTOR_SIZE-1));
    for(addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE) {
        rec_wdt_feed();
        rec_flash_erase(addr);
        rec_wdt_feed();
    }
    return esize;
}

#if (AOS_OTA_RECOVERY_TYPE != OTA_RECOVERY_TYPE_DIRECT)
int patch_flash_copy(int par, unsigned long dst_offset, unsigned long src_offset, size_t size)
{
    unsigned char tmp_buf[SECTOR_SIZE];
    uint32_t pos = 0;
    int ret = 0;

    while(pos < size){
        memset(tmp_buf, 0, SECTOR_SIZE);
        ret = patch_flash_read(par, tmp_buf, src_offset + pos, SECTOR_SIZE);
        if(ret < 0) {
            return -1;
        }

        patch_flash_erase(par, dst_offset + pos, SECTOR_SIZE);
        ret = patch_flash_write(par, tmp_buf, dst_offset + pos, SECTOR_SIZE);
        if (ret < 0) {
            return -1;
        }
        pos += SECTOR_SIZE;
    }

    return 0;
}

int patch_flash_copy_par(int dst_par, int src_par, unsigned long offset, size_t size)
{
    unsigned char tmp_buf[SECTOR_SIZE];
    uint32_t pos = 0;
    int ret = 0;

    while(pos < size){
        memset(tmp_buf, 0, SECTOR_SIZE);
        ret = patch_flash_read(src_par, tmp_buf, offset + pos, SECTOR_SIZE);
        if(ret < 0){
            return ret;
        }

        patch_flash_erase(dst_par, offset + pos, SECTOR_SIZE);
        ret = patch_flash_write(dst_par, tmp_buf, offset + pos, SECTOR_SIZE);
        if(ret < 0) {
            return ret;
        }
        pos += SECTOR_SIZE;
    }

    return 0;
}
#endif
