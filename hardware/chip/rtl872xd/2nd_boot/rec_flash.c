/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "rec_flash.h"

#include "flash_api.h"
#include "hal_platform.h"

static flash_t flash_obj;
/* Logic partition on flash devices */
extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

static void flash_read_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    flash_stream_read(&flash_obj, address, len, buffer);
}

static void flash_write_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    flash_stream_write(&flash_obj, address, len, buffer);
}

void rec_flash_init(void)
{
    //flash has init in boot code;
}

/* offset means physical address */
void rec_flash_erase(unsigned long offset)
{
    offset &= ~(FLASH_SECTOR_SIZE-1);
    flash_erase_sector(&flash_obj, offset);
}

/* offset means physical address */
void rec_flash_read_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{
    flash_read_data(buffer, offset, len);
}

/* offset means physical address */
void rec_flash_write_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{
    flash_write_data(buffer, offset, len);
}

hal_logic_partition_t *rec_flash_get_info(hal_partition_t pno)
{
    if(pno >= hal_partitions_amount)
    {
        return NULL;
    }
    return &hal_partitions[pno];
}
