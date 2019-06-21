/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "2ndboot.h"
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

int flash_read_data(unsigned int offset, unsigned char *buffer, unsigned int len)
{   
    flash_stream_read(&flash_obj, offset, len, buffer);
    return 0;
}

int flash_write_data(unsigned int offset, unsigned char *buffer, unsigned int len)
{
    flash_stream_write(&flash_obj, offset, len, buffer);
    return 0;
}

int flash_erase(unsigned int offset,  unsigned int size)
{
    int ret = 0;
    unsigned int addr;
    unsigned int start_addr, end_addr;

    start_addr = ROUND_DOWN(offset, SECTOR_SIZE);
    end_addr = ROUND_DOWN((offset + size - 1), SECTOR_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE) {
        flash_erase_sector(&flash_obj, addr);
    }
    return ret;
}
