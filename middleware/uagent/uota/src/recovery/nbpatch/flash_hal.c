/*
 * flash_hal.c
 *
 *  Created on: 2017年11月16日
 *      Author: ting.guit
 */
#include "flash_hal.h"

uint32_t get_partion_length(int par)
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
    if ( partition_info == NULL || offset + len > partition_info->partition_length )
    {
        printf("patch_flash_write %d error\r\n", par);
        return -1;
    }

    start_addr = partition_info->partition_start_addr + offset;
    rec_flash_write_data((unsigned char *)buffer, start_addr, len);
    return  len;
}

int patch_flash_read(int par, const unsigned char *buffer, unsigned long offset, unsigned long len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

    partition_info = patch_flash_get_info( par );
    if ( partition_info == NULL || offset + len > partition_info->partition_length )
    {
        printf("patch_flash_read %d error\r\n",par);
        return -1;
    }

    start_addr = partition_info->partition_start_addr + offset;
    rec_flash_read_data((unsigned char *)buffer, start_addr, len);

    return  len;
}

int patch_flash_erase(int par, unsigned long offset, size_t esize)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    hal_logic_partition_t *partition_info;

    partition_info = patch_flash_get_info( par );
    if ( partition_info == NULL || offset + esize > partition_info->partition_length )
    {
        printf("patch_flash_erase %d, 0x%x, 0x%x error\r\n", par, offset, esize);
        return -1;
    }

    start_addr = (partition_info->partition_start_addr + offset) & (~(FLASH_SECTOR_SIZE-1));
    end_addr = (partition_info->partition_start_addr + offset + esize  - 1) & (~(FLASH_SECTOR_SIZE-1));

    for(addr = start_addr; addr <= end_addr; addr += FLASH_SECTOR_SIZE)
    {
        rec_flash_erase(addr);
    }
    return esize;
}

#if (defined IS_ESP8266)
int patch_flash_copy(int par, unsigned long dst_offset, unsigned long src_offset, size_t size) {
    hal_logic_partition_t *partition_info;
    unsigned char tmp_buf[FLASH_SECTOR_SIZE];
    uint32_t pos = 0;
    int ret = 0;

    while(pos < size){
        memset(tmp_buf, 0, FLASH_SECTOR_SIZE);
        ret = patch_flash_read(par, tmp_buf, src_offset + pos, FLASH_SECTOR_SIZE);
        if ( ret == -1 )
        {
            printf("patch_flash_copy: patch_flash_read %d, offset 0x%x, size 0x%x error\r\n", par, src_offset, FLASH_SECTOR_SIZE);
            return -1;
        }

        patch_flash_erase(par, dst_offset + pos, FLASH_SECTOR_SIZE);
        ret = patch_flash_write(par, tmp_buf, dst_offset + pos, FLASH_SECTOR_SIZE);
        if ( ret == -1 )
        {
            printf("patch_flash_copy: patch_flash_read %d, offset 0x%x, size 0x%x error\r\n", par, src_offset, FLASH_SECTOR_SIZE);
            return -1;
        }
        pos += FLASH_SECTOR_SIZE;
    }
}

int patch_flash_copy_par(int dst_par, int src_par, unsigned long offset, size_t size) {
    hal_logic_partition_t *partition_info;
    unsigned char tmp_buf[FLASH_SECTOR_SIZE];
    uint32_t pos = 0;
    int ret = 0;

    while(pos < size){
        memset(tmp_buf, 0, FLASH_SECTOR_SIZE);
        ret = patch_flash_read(src_par, tmp_buf, offset + pos, FLASH_SECTOR_SIZE);
        if ( ret == -1 )
        {
            printf("patch_flash_copy: patch_flash_read %d, offset 0x%x, size 0x%x error\r\n", src_par, offset + pos, FLASH_SECTOR_SIZE);
            return -1;
        }

        patch_flash_erase(dst_par, offset + pos, FLASH_SECTOR_SIZE);
        ret = patch_flash_write(dst_par, tmp_buf, offset + pos, FLASH_SECTOR_SIZE);
        if ( ret == -1 )
        {
            printf("patch_flash_copy: patch_flash_read %d, offset 0x%x, size 0x%x error\r\n", dst_par, offset + pos, FLASH_SECTOR_SIZE);
            return -1;
        }
        pos += FLASH_SECTOR_SIZE;
    }
}
#endif
