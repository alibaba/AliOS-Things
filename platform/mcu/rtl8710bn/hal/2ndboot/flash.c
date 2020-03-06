/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "bootloader.h"
#include "flash.h"
#include "flash_api.h"
#include "hal_platform.h"

static flash_t flash_obj;

extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

hal_logic_partition_t *flash_get_info(hal_partition_t pno)
{
    if(pno >= hal_partitions_amount)
    {
        return NULL;
    }
    return &hal_partitions[pno];
}

int32_t hal_flash_init(void)
{
    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;
    unsigned int           addr;
    unsigned int           start_addr;
    unsigned int           end_addr;

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    start_addr = ROUND_DOWN(partition->partition_start_addr + off_set, SECTOR_SIZE);
    end_addr = ROUND_DOWN((partition->partition_start_addr + off_set + size - 1), SECTOR_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE) {
        flash_erase_sector(&flash_obj, addr);
    }

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t in_buf_len)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;

    if (off_set == NULL) {
        return -1;
    }

    if (out_buf == NULL) {
        return -1;
    }

    if (in_buf_len == 0) {
        return -1;
    }

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    if (1 != flash_stream_read(&flash_obj, partition->partition_start_addr + *off_set, in_buf_len, out_buf)) {
        return -1;
    }

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;

    if (off_set == NULL) {
        return -1;
    }

    if (in_buf == NULL) {
        return -1;
    }

    if (in_buf_len == 0) {
        return -1;
    }

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    if (1 != flash_stream_write(&flash_obj, partition->partition_start_addr + *off_set, in_buf_len, in_buf)) {
        return -1;
    }

    *off_set += in_buf_len;

    return 0;
}


